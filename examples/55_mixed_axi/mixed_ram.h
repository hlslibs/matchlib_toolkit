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

#include "axi4_segment.h"

/**
 *  \brief A simple RAM module with 1 axi4 read slave and 1 axi4 write slave
*/

template <class cfg>
class ram : public sc_module
{
public:
  sc_in<bool>                                   CCS_INIT_S1(clk);
  sc_in<bool>                                   CCS_INIT_S1(rst_bar);
  typename cfg::template r_slave<AUTO_PORT>     CCS_INIT_S1(r_slave0);
  typename cfg::template w_slave<AUTO_PORT>     CCS_INIT_S1(w_slave0);

  static const int sz = 0x10000; // size in cfg::DATA_WIDTH words

  typedef NVUINTW(cfg::DATA_WIDTH) arr_t;
  arr_t *array {0};

  SC_CTOR(ram) {
    array = new arr_t[sz];

    SC_THREAD(slave_r_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(slave_w_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    for (int i=0; i < sz; i++) {
      array[i] = i * cfg::bytesPerBeat;
    }
  }

  NVUINTW(cfg::DATA_WIDTH) debug_read_addr(uint32_t addr) {
    if (addr >= (sz * cfg::bytesPerBeat)) {
      SC_REPORT_ERROR("ram", "invalid addr");
      return 0;
    }

    return (array[addr / cfg::bytesPerBeat]);
  }

  void slave_r_process() {
    r_slave0.reset();

    wait();

    while (1) {
      typename cfg::ar_payload ar;
      r_slave0.start_multi_read(ar);

      CCS_LOG("ram read  addr: " << std::hex << ar.addr << " len: " << ar.len);

      while (1) {
        typename cfg::r_payload r;

        if (ar.addr >= (sz * cfg::bytesPerBeat)) {
          SC_REPORT_ERROR("ram", "invalid addr");
          r.resp = cfg::Enc::XRESP::SLVERR;
        } else {
          r.data = array[ar.addr / cfg::bytesPerBeat];
        }

        if (!r_slave0.next_multi_read(ar, r)) { break; }
      }
    }
  }

  void slave_w_process() {
    w_slave0.reset();
    wait();

    while (1) {
      typename cfg::aw_payload aw;
      typename cfg::b_payload b;

      w_slave0.start_multi_write(aw, b);

      CCS_LOG("ram write addr: " << std::hex << aw.addr << " len: " << aw.len);

      while (1) {
        typename cfg::w_payload w = w_slave0.w.Pop();

        if (aw.addr >= (sz * cfg::bytesPerBeat)) {
          SC_REPORT_ERROR("ram", "invalid addr");
          b.resp = cfg::Enc::XRESP::SLVERR;
        } else {
          decltype(w.wstrb) all_on{~0};

          if (w.wstrb == all_on)
          { array[aw.addr / cfg::bytesPerBeat] = w.data.to_uint64(); }
          else {
            CCS_LOG("write strobe enabled");
            arr_t orig  = array[aw.addr / cfg::bytesPerBeat];
            arr_t wdata = w.data.to_uint64();

#pragma unroll
            for (int i=0; i<cfg::WSTRB_WIDTH; i++) {
              if (w.wstrb[i]) {
                orig = nvhls::set_slc(orig, nvhls::get_slc<8>(wdata, (i*8)), (i*8));
              }
            }

            array[aw.addr / cfg::bytesPerBeat] = orig;
          }
        }

        if (!w_slave0.next_multi_write(aw)) { break; }
      }

      w_slave0.b.Push(b);
    }
  }
};

