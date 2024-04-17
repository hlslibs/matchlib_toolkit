// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include "axi4_segment.h"
#include "auto_gen_fields.h"

// Below 2 macros should be moved into ../../include/axi4_segment.h
#ifndef AXI4_W_SEGMENT_CFG
#define AXI4_W_SEGMENT_CFG(cfg, n) \
  cfg::w_segment CCS_INIT_S1(n); \
  Connections::Combinational<cfg::ex_aw_payload> CCS_INIT_S1(n ## _ex_aw_chan); \
  Connections::Combinational<cfg::w_payload>     CCS_INIT_S1(n ## _w_chan); \
  Connections::Combinational<cfg::b_payload> CCS_INIT_S1(n ## _b_chan);
#endif

#ifndef AXI4_R_SEGMENT_CFG
#define AXI4_R_SEGMENT_CFG(cfg, n) \
  cfg::r_segment CCS_INIT_S1(n); \
  Connections::Combinational<cfg::ex_ar_payload> CCS_INIT_S1(n ## _ex_ar_chan);
#endif

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

typedef axi::axi4_segment<axi::cfg::standard> local_axi_64;

struct axi_16 {
  enum {
    dataWidth = 16,
    useVariableBeatSize = 0,
    useMisalignedAddresses = 0,
    useLast = 1,
    useWriteStrobes = 1,
    useBurst = 1, useFixedBurst = 0, useWrapBurst = 0, maxBurstSize = 256,
    useQoS = 0, useLock = 0, useProt = 0, useCache = 0, useRegion = 0,
    aUserWidth = 0, wUserWidth = 0, bUserWidth = 0, rUserWidth = 0,
    addrWidth = 32,
    idWidth = 4,
    useWriteResponses = 1,
  };
};

typedef axi::axi4_segment<axi_16> local_axi_16;

/**
 *  * \brief dma module
*/
#pragma hls_design top
class dma : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  local_axi_16::r_master<> CCS_INIT_S1(r_master0);
  local_axi_16::w_master<> CCS_INIT_S1(w_master0);
  local_axi_64::r_slave<>  CCS_INIT_S1(r_slave0);
  local_axi_64::w_slave<>  CCS_INIT_S1(w_slave0);
  Connections::Out<bool> CCS_INIT_S1(dma_done);
  Connections::Out<sc_uint<32>> CCS_INIT_S1(dma_dbg);

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
  AXI4_W_SEGMENT_CFG(local_axi_16, w_segment0)
  AXI4_R_SEGMENT_CFG(local_axi_16, r_segment0)

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
      local_axi_16::ex_ar_payload ar;
      local_axi_16::ex_aw_payload aw;

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
        local_axi_16::r_payload r = r_master0.r.Pop();
        local_axi_16::w_payload w;
        w.data = r.data;
        w_segment0_w_chan.Push(w);

        if (ar.ex_len-- == 0) { break; }
      }

      local_axi_16::b_payload b = w_segment0_b_chan.Pop();
      dma_done.Push(b.resp == local_axi_64::Enc::XRESP::OKAY);
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
      local_axi_64::aw_payload aw;
      local_axi_64::w_payload w;
      local_axi_64::b_payload b;

      if (w_slave0.get_single_write(aw, w, b)) {
        b.resp = local_axi_64::Enc::XRESP::SLVERR;
        switch (aw.addr) {
          case offsetof(dma_address_map, ar_addr):
            cmd1.ar_addr = w.data;
            b.resp = local_axi_64::Enc::XRESP::OKAY;
            break;
          case offsetof(dma_address_map, aw_addr):
            cmd1.aw_addr = w.data;
            b.resp = local_axi_64::Enc::XRESP::OKAY;
            break;
          case offsetof(dma_address_map, len):
            cmd1.len = w.data;
            b.resp = local_axi_64::Enc::XRESP::OKAY;
            break;
          case offsetof(dma_address_map, start):
            dma_cmd_chan.Push(cmd1);
            b.resp = local_axi_64::Enc::XRESP::OKAY;
            break;
        }
        w_slave0.b.Push(b);
      }
    }
  }
};

