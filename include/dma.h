// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include "mc_connections.h"
#include "auto_gen_fields.h"
#include "auto_gen_port_info.h"
#include "axi4_segment.h"

/**
 *  * \brief dma command sent to the DMA engine
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
 *  * \brief dma address map as seen by the CPU
*/
struct dma_address_map {
  uint64_t  ar_addr;
  uint64_t  aw_addr;
  uint64_t  len;
  uint64_t  start;
};

typedef axi::axi4_segment<axi::cfg::standard> local_axi;


/**
 *  * \brief dma module
*/
#pragma hls_design top
class dma : public sc_module, public local_axi
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  r_master<> CCS_INIT_S1(r_master0);
  w_master<> CCS_INIT_S1(w_master0);
  r_slave<>  CCS_INIT_S1(r_slave0);
  w_slave<>  CCS_INIT_S1(w_slave0);
  Connections::Out<bool> CCS_INIT_S1(dma_done);
  Connections::Out<sc_uint<32>> CCS_INIT_S1(dma_dbg);

  AUTO_GEN_PORT_INFO(dma, ( \
    clk \
  , rst_bar \
  , r_master0 \
  , w_master0 \
  , r_slave0 \
  , w_slave0 \
  , dma_done \
  , dma_dbg \
  ) )
  //

  SC_CTOR(dma) {
    SC_THREAD(slave_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(master_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    AXI4_W_SEGMENT_BIND(w_segment0, clk, rst_bar, w_master0);
    AXI4_R_SEGMENT_BIND(r_segment0, clk, rst_bar, r_master0);
  }

private:

  Connections::Combinational<dma_cmd> CCS_INIT_S1(dma_cmd_chan);

  // write and read segmenters segment long bursts to conform to AXI4 protocol (which allows 256 beats maximum).
  AXI4_W_SEGMENT(w_segment0)
  AXI4_R_SEGMENT(r_segment0)

  // master_process recieves dma_cmd transactions from the slave_process.
  // the master_process performs the dma operations via the master0 axi port,
  // and then sends a done signal to the requester via the dma_done transaction.
  void master_process() {
    AXI4_W_SEGMENT_RESET(w_segment0, w_master0);
    AXI4_R_SEGMENT_RESET(r_segment0, r_master0);

    dma_cmd_chan.ResetRead();
    dma_dbg.Reset();
    dma_done.Reset();

    wait();

    while (1) {
      ex_ar_payload ar;
      ex_aw_payload aw;

      dma_cmd cmd = dma_cmd_chan.Pop();
      ar.ex_len = cmd.len;
      aw.ex_len = cmd.len;
      ar.addr = cmd.ar_addr;
      aw.addr = cmd.aw_addr;
      r_segment0_ex_ar_chan.Push(ar);
      w_segment0_ex_aw_chan.Push(aw);

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
      while (1) {
        r_payload r = r_master0.r.Pop();
        w_payload w;
        w.data = r.data;
        w_segment0_w_chan.Push(w);

        if (ar.ex_len-- == 0) { break; }
      }

      b_payload b = w_segment0_b_chan.Pop();
      dma_done.Push(b.resp == Enc::XRESP::OKAY);
    }
  }

  // slave_process accepts incoming axi4 requests from slave0 and programs the dma registers.
  // when the start register is written to, a dma_cmd transaction is sent to the dma master_process
  void slave_process() {
    r_slave0.reset();
    w_slave0.reset();
    dma_cmd_chan.ResetWrite();

    wait();

    dma_cmd cmd1;

    while (1) {
      aw_payload aw;
      w_payload w;
      b_payload b;

      if (w_slave0.get_single_write(aw, w, b)) {
        b.resp = Enc::XRESP::SLVERR;
        switch (aw.addr) {
          case offsetof(dma_address_map, ar_addr):
            cmd1.ar_addr = w.data;
            b.resp = Enc::XRESP::OKAY;
            break;
          case offsetof(dma_address_map, aw_addr):
            cmd1.aw_addr = w.data;
            b.resp = Enc::XRESP::OKAY;
            break;
          case offsetof(dma_address_map, len):
            cmd1.len = w.data;
            b.resp = Enc::XRESP::OKAY;
            break;
          case offsetof(dma_address_map, start):
            dma_cmd_chan.Push(cmd1);
            b.resp = Enc::XRESP::OKAY;
            break;
        }
        w_slave0.b.Push(b);
      }
    }
  }
};

