/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 21 15:24:39 PST 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.7                                               *
 *                                                                        *
 *  Unpublished work. Copyright 2020 Siemens                              *
 *                                                                        *
 **************************************************************************/

// This document contains information that is proprietary to Siemens
// The original recipient of this document may duplicate this  
// document in whole or in part for internal business purposes only, provided  
// that this entire notice appears in all copies. In duplicating any part of  
// this document, the recipient agrees to make every reasonable effort to  
// prevent the unauthorized use and distribution of the proprietary information.

#pragma once

#include "apb_transactor.h"

// Create a typedef for the configuration of this local APB bus
typedef apb::apb_transactor<axi::cfg::lite> local_apb;

/**
 *  * \brief Define struct for cmds sent to the DMA engine
*/
struct dma_cmd {
  NVUINTW(32) ar_addr {0};
  NVUINTW(32) aw_addr {0};
  NVUINTW(32) len {0};

  static const unsigned int width = 32 + 32 + 32;
  template <unsigned int Size> void Marshall(Marshaller<Size> &m) {
    m &ar_addr;
    m &aw_addr;
    m &len;
  }
  inline friend void sc_trace(sc_trace_file *tf, const dma_cmd &v, const std::string &NAME ) {
    sc_trace(tf,v.ar_addr,  NAME + ".ar_addr");
    sc_trace(tf,v.aw_addr,  NAME + ".aw_addr");
    sc_trace(tf,v.len,      NAME + ".len");
  }

  inline friend std::ostream &operator<<(ostream &os, const dma_cmd &rhs) {
    os << rhs.ar_addr << " " << rhs.aw_addr << " " << rhs.len;
    return os;
  }
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

  apb_master_xactor<>                     CCS_INIT_S1(master0_xactor);
  apb_master_ports<>                      CCS_INIT_S1(master0_ports);
  Connections::Combinational<apb_req>     CCS_INIT_S1(master0_req_chan); // Connections channel for the apb_req message from CPU
  Connections::Combinational<apb_rsp>     CCS_INIT_S1(master0_rsp_chan); // Connections channel for the apb_rsp message to CPU

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

    master0_xactor.clk(clk);
    master0_xactor.rst_bar(rst_bar);
    master0_xactor.req_port(master0_req_chan);
    master0_xactor.rsp_port(master0_rsp_chan);
    master0_xactor(master0_ports);
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

    master0_req_chan.ResetWrite();
    master0_rsp_chan.ResetRead();

    wait();

    while (1) {
      dma_cmd cmd = dma_cmd_chan.Pop(); // Blocking read - waiting for cmd
      bool status = Enc::XRESP::OKAY;
      while (1) {
        apb_req req;
        apb_rsp rsp;

        req.is_write = false;
        req.addr.addr = cmd.ar_addr;
        master0_req_chan.Push(req);
        rsp = master0_rsp_chan.Pop();

        req.is_write = true;
        req.addr.addr = cmd.aw_addr;
        req.w.data = rsp.r.data;
        master0_req_chan.Push(req);
        rsp = master0_rsp_chan.Pop();

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

