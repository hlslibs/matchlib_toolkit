/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Thu Aug 11 16:24:59 PDT 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.9                                               *
 *                                                                        *
 *  Copyright 2020 Siemens                                                *
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

#include "apb_transactor.h"


class ram : public sc_module
{
public:
  // This RAM module has one AXI4 connection - from the DMA (an interface to a master).
  // It is configured as AXI4-Lite.
  // Create some typedefs to simplify the SystemC declarations below.
  typedef apb::apb_transactor<axi::cfg::lite> from_dma_trans;
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  from_dma_trans::apb_slave_xactor<>   CCS_INIT_S1(slave0_xactor);
  from_dma_trans::apb_slave_ports<>    CCS_INIT_S1(slave0_ports);
  from_dma_trans::apb_req_chan         CCS_INIT_S1(req_chan);
  from_dma_trans::apb_rsp_chan         CCS_INIT_S1(rsp_chan);

  static const int sz = 0x10000; // size in axi::cfg::lite::dataWidth words

  typedef NVUINTW(axi::cfg::lite::dataWidth) arr_t;
  arr_t *array {0};

  SC_CTOR(ram) {
    array = new arr_t[sz];

    SC_THREAD(slave_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    for (int i=0; i < sz; i++) {
      array[i] = i * from_dma_trans::bytesPerBeat;
    }

    slave0_xactor.clk(clk);
    slave0_xactor.rst_bar(rst_bar);
    slave0_xactor.req_port(req_chan);
    slave0_xactor.rsp_port(rsp_chan);
    slave0_xactor(slave0_ports);
  }

  NVUINTW(axi::cfg::lite::dataWidth) debug_read_addr(uint32_t addr) {
    if (addr >= (sz * from_dma_trans::bytesPerBeat)) {
      SC_REPORT_ERROR("ram", "invalid addr");
      return 0;
    }

    return (array[addr / from_dma_trans::bytesPerBeat]);
  }

  void slave_process() {
    req_chan.ResetRead();
    rsp_chan.ResetWrite();

    wait();

    while (1) {
      from_dma_trans::apb_req req = req_chan.Pop();

      // CCS_LOG("ram addr: " << std::hex << req.addr.addr << " is_write: " << req.is_write);

      from_dma_trans::apb_rsp rsp;

      if (req.addr.addr >= (sz * from_dma_trans::bytesPerBeat)) {
        SC_REPORT_ERROR("ram", "invalid addr");
        rsp.r.resp = from_dma_trans::Enc::XRESP::SLVERR;
      } else if (!req.is_write) {
        rsp.r.data = array[req.addr.addr / from_dma_trans::bytesPerBeat];
        // CCS_LOG("read data: " << std::hex << rsp.r.data);
      } else if  (req.is_write) {
        decltype(req.w.wstrb) all_on{~0};

        if (req.w.wstrb == all_on) {
          array[req.addr.addr / from_dma_trans::bytesPerBeat] = req.w.data.to_uint64();
          // CCS_LOG("write data: " << std::hex << req.w.data.to_uint64());
        } else {
          CCS_LOG("write strobe enabled");
          arr_t orig  = array[req.addr.addr / from_dma_trans::bytesPerBeat];
          arr_t data = req.w.data.to_uint64();

#pragma unroll
          for (int i=0; i<from_dma_trans::WSTRB_WIDTH; i++)
            if (req.w.wstrb[i]) {
              orig = nvhls::set_slc(orig, nvhls::get_slc<8>(data, (i*8)), (i*8));
            }

          array[req.addr.addr / from_dma_trans::bytesPerBeat] = orig;
        }
      }

      rsp_chan.Push(rsp);
    }
  }
};

