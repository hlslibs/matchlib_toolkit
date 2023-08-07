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

#include "axi_bus_params.h"

/**
 *  * \brief AXI-4 Slave configuration register for DMA module
 *  *   See dma_cmd.h for the address map.
*/
#pragma hls_design
class dma_axi_slave : public sc_module
{
public:
  sc_in<bool>                CCS_INIT_S1(clk);
  sc_in<bool>                CCS_INIT_S1(rst_bar);
  Connections::Out<dma_cmd>  CCS_INIT_S1(dma_cmd_chan); // push cmd out to DMA block

  // Declare the slave connection to the CPU
  local_axi_lite::r_slave<>  CCS_INIT_S1(r_slave0);
  local_axi_lite::w_slave<>  CCS_INIT_S1(w_slave0);

public:
  // Constructor
  SC_CTOR(dma_axi_slave) {
    SC_THREAD(w_slave_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(r_slave_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    // Reset action
    d_cfg_src_addr = 0;
    d_cfg_dst_addr = 0;
    d_cfg_len = 0;
  }

private:
  //--------------------------------------------------------------------------------------------
  // Implement an AXI-4 Slave interface (write and read) to this DMA object so that the CPU can 
  // program the DMA operations. This could be refactored out into a separate block with 
  // dma_cmd_chan and dma_done connections to this DMA object.

  //   w_slave_process - accepts incoming axi4 requests from port w_slave0 and programs the dma
  //     registers. when the start register is written to, a dma_cmd transaction is sent to 
  //     the dma master_process
  void w_slave_process() {
    w_slave0.reset();
    //dma_cmd_chan.ResetWrite();

    wait(); // wait for first edge after reset

    while (1) {
      local_axi_lite::aw_payload aw;
      local_axi_lite::w_payload w;
      local_axi_lite::b_payload b;

      aw = w_slave0.aw.Pop(); // Blocking read
      w  = w_slave0.w.Pop();  // Blocking read
      b.resp = local_axi_lite::Enc::XRESP::SLVERR;
      switch (aw.addr) {
        case offsetof(dma_address_map, src_addr):      // CPU programming the read address
          d_cfg_src_addr = w.data;
          b.resp = local_axi_lite::Enc::XRESP::OKAY;
          break;
        case offsetof(dma_address_map, dst_addr):      // CPU programming the write address
          d_cfg_dst_addr = w.data;
          b.resp = local_axi_lite::Enc::XRESP::OKAY;
          break;
        case offsetof(dma_address_map, len):           // CPU programming the length
          d_cfg_len = w.data;
          b.resp = local_axi_lite::Enc::XRESP::OKAY;
          break;
        case offsetof(dma_address_map, start):         // CPU initiating the DMA operation
          dma_cmd cmd1;
          cmd1.src_addr = d_cfg_src_addr.read();
          cmd1.dst_addr = d_cfg_dst_addr.read();
          cmd1.len = d_cfg_len.read();
          dma_cmd_chan.Push(cmd1);                     // actually push the message to start DMA
          b.resp = local_axi_lite::Enc::XRESP::OKAY;
          break;
      }
      w_slave0.b.Push(b); // send the write response
    }
  }

  //   r_slave_process - accepts incoming axi4 requests from port r_slave0 and returns dummy data.
  //     reads of invalid addresses returns an error.
  void r_slave_process() {
    r_slave0.reset();

    wait(); // wait for first edge after reset

    while (1) {
      local_axi_lite::ar_payload ar;
      local_axi_lite::r_payload r;

      ar = r_slave0.ar.Pop(); // Blocking read

      r.resp = local_axi_lite::Enc::XRESP::SLVERR;  // default response is error
      switch (ar.addr) {
        case offsetof(dma_address_map, src_addr):     // Get current configuration of src_addr register
          r.data = d_cfg_src_addr.read();
          r.resp = local_axi_lite::Enc::XRESP::OKAY;
          break;
        case offsetof(dma_address_map, dst_addr):     // Get current configuration of dst_addr register
          r.data = d_cfg_dst_addr.read();
          r.resp = local_axi_lite::Enc::XRESP::OKAY;
          break;
        case offsetof(dma_address_map, len):          // Get current configuration of len register
          r.data = d_cfg_len.read();
          r.resp = local_axi_lite::Enc::XRESP::OKAY;
          break;
        default: // all other addresses are not implemented or write-only so just return error
          break;
      }
      r_slave0.r.Push(r);  // return the data/response
    }
  }

private: // registers
  sc_signal<uint32_t> d_cfg_src_addr;   // source address to read from
  sc_signal<uint32_t> d_cfg_dst_addr;   // destination address to write to
  sc_signal<uint32_t> d_cfg_len;        // number of elements to transfer
};

