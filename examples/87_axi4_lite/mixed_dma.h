/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.5                                                 *
 *                                                                        *
 *  Release Date    : Wed Jul 19 09:26:27 PDT 2023                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.5.0                                               *
 *                                                                        *
 *  Copyright 2022 Siemens                                                *
 *                                                                        *
 **************************************************************************
 *  Licensed under the Apache License, Version 2.0 (the "License");       *
 *  you may not use this file except in compliance with the License.      * 
 *  You may obtain a copy of the License at                               *
 *                                                                        *
 *      http://www.apache.org/licenses/LICENSE-2.0                        *
 *                                                                        *
 *  Unless required by applicable law or agreed to in writing, software   * 
 *  distributed under the License is distributed on an "AS IS" BASIS,     * 
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or       *
 *  implied.                                                              * 
 *  See the License for the specific language governing permissions and   * 
 *  limitations under the License.                                        *
 **************************************************************************
 *                                                                        *
 *  The most recent version of this package is available at github.       *
 *                                                                        *
 *************************************************************************/

#pragma once

#include "dma_cmd.h"

#include "axi4_segment.h"

#include "axi_bus_params.h"

#include "dma_axi_slave.h"

/**
 *  * \brief dma module
*/
#pragma hls_design top
class dma : public sc_module
{
public:
  sc_in<bool>                CCS_INIT_S1(clk);
  sc_in<bool>                CCS_INIT_S1(rst_bar);
  Connections::Out<bool>     CCS_INIT_S1(dma_done);

  // Declare the slave connection to the CPU
  local_axi_lite::r_slave<>  CCS_INIT_S1(r_slave0);
  local_axi_lite::w_slave<>  CCS_INIT_S1(w_slave0);

  // Declare the master connection to the RAM
  local_axi_64::r_master<>   CCS_INIT_S1(r_master0);
  local_axi_64::w_master<>   CCS_INIT_S1(w_master0);

private:
  // Instantiate AXI-4 Lite configuration register
  dma_axi_slave              CCS_INIT_S1(slave_cfg_inst);

  // Local connection from the AXI slave interface to the DMA engine "master_process"
  Connections::Combinational<dma_cmd> CCS_INIT_S1(dma_cmd_chan);

  // Create write and read segmenters to segment long bursts to conform to 
  // AXI4 protocol (which allows 256 beats maximum).
  AXI4_W_SEGMENT_CFG(local_axi_64, w_segment0) // This macro effectively does the following:
  // typename local_axi_64::w_segment                           CCS_INIT_S1(w_segment0);
  // Connections::Combinational<local_axi_64::ex_aw_payload>    CCS_INIT_S1(w_segment0_ex_aw_chan);
  // Connections::Combinational<local_axi_64::w_payload>        CCS_INIT_S1(w_segment0_w_chan);
  // Connections::Combinational<local_axi_64::b_payload>        CCS_INIT_S1(w_segment0_b_chan);
  AXI4_R_SEGMENT_CFG(local_axi_64, r_segment0)

public:
  // Constructor
  SC_CTOR(dma) {
    // Connect up the config register
    slave_cfg_inst.clk(clk);
    slave_cfg_inst.rst_bar(rst_bar);
    slave_cfg_inst.r_slave0(r_slave0);          // CPU connection passed directly down to cfg reg
    slave_cfg_inst.w_slave0(w_slave0);          // CPU connection passed directly down to cfg reg
    slave_cfg_inst.dma_cmd_chan(dma_cmd_chan);  // connect output of cfg reg to control of this DMA obj

    SC_THREAD(master_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    AXI4_W_SEGMENT_BIND(w_segment0, clk, rst_bar, w_master0);  // This macro effectively does the following:
    // w_segment0.clk(       clk);
    // w_segment0.rst_bar(   rst_bar);
    // w_segment0.aw_out(    w_master0.aw);
    // w_segment0.w_out(     w_master0.w);
    // w_segment0.b_in(      w_master0.b);
    // w_segment0.ex_aw_chan(w_segment0_ex_aw_chan);
    // w_segment0.w_chan(    w_segment0_w_chan);
    // w_segment0.b_chan(    w_segment0_b_chan);
    AXI4_R_SEGMENT_BIND(r_segment0, clk, rst_bar, r_master0);
  }

private:
  //--------------------------------------------------------------------------------------------
  // Implement the DirectMemoryAccess transfer behavior.

  // master_process receives dma_cmd transactions from the w_slave_process.
  // the master_process performs the dma operations via the master0 axi port,
  // and then sends a done signal to the requester via the dma_done transaction.
  void master_process() {
    AXI4_W_SEGMENT_RESET(w_segment0, w_master0); // This macro effectively does the following:
    // w_segment0_ex_aw_chan.ResetWrite();
    // w_segment0_w_chan.ResetWrite();
    // w_segment0_b_chan.ResetRead();
    AXI4_R_SEGMENT_RESET(r_segment0, r_master0);

    dma_cmd_chan.ResetRead();
    dma_done.Reset();

    wait();

    while (1) {
      local_axi_64::ex_ar_payload ar;
      local_axi_64::ex_aw_payload aw;

      dma_cmd cmd = dma_cmd_chan.Pop(); // Blocking read - waiting for cmd
      ar.ex_len = cmd.len-1;              // Get transfer settings into payloads (axi encoding: 0 means 1 beat)
      aw.ex_len = cmd.len-1;
      ar.addr = cmd.src_addr;
      aw.addr = cmd.dst_addr;
      r_segment0_ex_ar_chan.Push(ar);   // Start the AXI4 read request
      w_segment0_ex_aw_chan.Push(aw);   // Start the AXI4 write request

      local_axi_64::r_payload r;
      local_axi_64::w_payload w;

      #pragma hls_pipeline_init_interval 1
      #pragma pipeline_stall_mode flush
      while (1) {
        r = r_master0.r.Pop();      // Read the data in ...
        w.data = r.data;
        w_segment0_w_chan.Push(w);  // ... and write it out

        if (ar.ex_len-- == 0) { break; }
      }

      local_axi_64::b_payload b = w_segment0_b_chan.Pop();
      dma_done.Push(b.resp == local_axi_lite::Enc::XRESP::OKAY);
    }
  }
};

