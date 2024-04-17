// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include "apb_transactor.h"
#include "auto_gen_fields.h"

// Create a typedef for the configuration of this local APB bus
typedef apb::apb_transactor<axi::cfg::lite> local_apb;

/**
 *  * \brief Define struct for cmds sent to the DMA engine
*/
struct dma_cmd {
  ac_int<32, false> ar_addr {0};
  ac_int<32, false> aw_addr {0};
  ac_int<32, false> len {0};

  AUTO_GEN_FIELD_METHODS(dma_cmd, ( \
     ar_addr \
   , aw_addr \
   , len \
  ) )
  //
};

/**
 *  * \brief Define configuration register addres map of the DMA as seen by CPU
*/
struct dma_configuration_regs {
  uint32_t  ar_addr;
  uint32_t  aw_addr;
  uint32_t  len;
  uint32_t  start;
};


/**
 *  * \brief dma module
*/
#pragma hls_design top
class dma : public sc_module, public local_apb
{
public:
  sc_in<bool>                             CCS_INIT_S1(clk);
  sc_in<bool>                             CCS_INIT_S1(rst_bar);
  Connections::Out<bool>                  CCS_INIT_S1(dma_done);
  Connections::Out<uint32_t>              CCS_INIT_S1(dma_dbg);

  apb_master_ports<>                      CCS_INIT_S1(master0_ports);
#define MASTER_XACTOR 1
#ifdef MASTER_XACTOR
  apb_master_xactor<>                     CCS_INIT_S1(master0_xactor);
  Connections::Combinational<apb_req>     CCS_INIT_S1(master0_req_chan);
  Connections::Combinational<apb_rsp>     CCS_INIT_S1(master0_rsp_chan);
#endif

  apb_slave_xactor<>                      CCS_INIT_S1(slave0_xactor);
  apb_slave_ports<>                       CCS_INIT_S1(slave0_ports);
  Connections::Combinational<apb_req>     CCS_INIT_S1(slave0_req_chan); // Connections channel for the apb_req message to RAM
  Connections::Combinational<apb_rsp>     CCS_INIT_S1(slave0_rsp_chan); // Connections channel for the apb_rsp message from RAM

  SC_CTOR(dma) {
    SC_THREAD(slave_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(master_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    slave0_xactor.clk(clk);
    slave0_xactor.rst_bar(rst_bar);
    slave0_xactor.req_port(slave0_req_chan);
    slave0_xactor.rsp_port(slave0_rsp_chan);
    slave0_xactor(slave0_ports);

#ifdef MASTER_XACTOR
    master0_xactor.clk(clk);
    master0_xactor.rst_bar(rst_bar);
    master0_xactor.req_port(master0_req_chan);
    master0_xactor.rsp_port(master0_rsp_chan);
    master0_xactor(master0_ports);
#endif
  }

private:

  Connections::Combinational<dma_cmd> CCS_INIT_S1(dma_cmd_chan);

  // master_process recieves dma_cmd transactions from the slave_process.
  // the master_process performs the dma operations via the master0_xactor,
  // and then sends a done signal to the requester via the dma_done transaction.
  void master_process() {

    dma_cmd_chan.ResetRead();
    dma_dbg.Reset();
    dma_done.Reset();
#ifdef MASTER_XACTOR
    master0_req_chan.ResetWrite();
    master0_rsp_chan.ResetRead();
#else
    apb_master_rw_reset(master0_ports);
#endif

    wait();

    while (1) {
      dma_cmd cmd = dma_cmd_chan.Pop(); // Blocking read - waiting for cmd
      bool status = Enc::XRESP::OKAY;
// #pragma hls_pipeline_init_interval 2
// #pragma pipeline_stall_mode stall
      while (1) {
        apb_req req;
        apb_rsp rsp;

        req.is_write = false;
        req.addr.addr = cmd.ar_addr;
#ifdef MASTER_XACTOR
        master0_req_chan.Push(req);
        rsp = master0_rsp_chan.Pop();
#else
        apb_master_rw(master0_ports, req, rsp);
#endif

        req.is_write = true;
        req.addr.addr = cmd.aw_addr;
        req.w.data = rsp.r.data;
#ifdef MASTER_XACTOR
        master0_req_chan.Push(req);
        rsp = master0_rsp_chan.Pop();
#else
        apb_master_rw(master0_ports, req, rsp);
#endif

        if (rsp.r.resp != Enc::XRESP::OKAY) { status = 0; }

        if (cmd.len-- == 0) { break; } // DMA transfer done

        cmd.aw_addr += bytesPerBeat;
        cmd.ar_addr += bytesPerBeat;
      }
      dma_done.Push(status);
    }
  }

  // slave_process accepts incoming apb requests from slave0_xactor and programs the dma registers.
  // when the start register is written to, a dma_cmd transaction is sent to the dma master_process
  void slave_process() {
    dma_cmd_chan.ResetWrite();
    slave0_req_chan.ResetRead();
    slave0_rsp_chan.ResetWrite();

    wait();

    dma_cmd cmd1;

    while (1) {
      apb_rsp rsp;
      apb_req req = slave0_req_chan.Pop(); // Blocking read - waiting for request from CPU

      rsp.r.resp = Enc::XRESP::SLVERR;
      switch (req.addr.addr) {
        case offsetof(dma_configuration_regs, ar_addr): // CPU programming the read address
          if (req.is_write) {
            cmd1.ar_addr = req.w.data;  // write
          } else {
            rsp.r.data = cmd1.ar_addr;  //  read - not really useful, just for completeness..
          }
          rsp.r.resp = Enc::XRESP::OKAY;
          break;
        case offsetof(dma_configuration_regs, aw_addr): // CPU programming the write address
          cmd1.aw_addr = req.w.data;
          rsp.r.resp = Enc::XRESP::OKAY;
          break;
        case offsetof(dma_configuration_regs, len): // CPU programming the length
          cmd1.len = req.w.data;
          rsp.r.resp = Enc::XRESP::OKAY;
          break;
        case offsetof(dma_configuration_regs, start): // CPU initiating DMA operation
          dma_cmd_chan.Push(cmd1);             //   actually push message
          rsp.r.resp = Enc::XRESP::OKAY;
          break;
        default:
          break;
      }

      slave0_rsp_chan.Push(rsp);
    }
  }
};
