/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2.1                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan  5 08:41:49 PST 2024                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 2.1.1                                               *
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

#include <ac_math.h>
#include <mc_connections.h>
#include "axi4_segment.h"


typedef axi::axi4_segment<axi::cfg::standard> local_axi;


#pragma hls_design top
class dut : public sc_module, public local_axi
{
public:
  sc_in<bool>                             CCS_INIT_S1(clk);
  sc_in<bool>                             CCS_INIT_S1(rst_bar);
  Connections::Out<uint32>                CCS_INIT_S1(out1);
  Connections::In<uint32>                 CCS_INIT_S1(in1);
  Connections::In<uint32>                 CCS_INIT_S1(in2);
  Connections::In<uint32>                 CCS_INIT_S1(in3);

  r_master<> CCS_INIT_S1(r_master0);
  w_master<> CCS_INIT_S1(w_master0);

  uint32 mem[0x1000];

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    AXI4_W_SEGMENT_BIND(w_segment0, clk, rst_bar, w_master0);
    AXI4_R_SEGMENT_BIND(r_segment0, clk, rst_bar, r_master0);
  }

private:

  AXI4_W_SEGMENT(w_segment0)
  AXI4_R_SEGMENT(r_segment0)

  void main() {
    in1.Reset();
    in2.Reset();
    in3.Reset();
    out1.Reset();
    AXI4_W_SEGMENT_RESET(w_segment0, w_master0);
    AXI4_R_SEGMENT_RESET(r_segment0, r_master0);
    wait();

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      uint32 t1 = in1.Pop();
      uint32 t2 = in2.Pop();
      uint32 t3 = in3.Pop();

      if (t1 == 0) {
#ifndef USE_AXI_MEM
        mem[t3] = t2;
#else
        ex_aw_payload aw;
        aw.ex_len = 0;
        aw.addr = t3;
        w_segment0_ex_aw_chan.Push(aw);
        w_payload w;
        w.data = t2;
        w_segment0_w_chan.Push(w);
        w_segment0_b_chan.Pop();
#endif
      } else {
        ac_int<32, false> prod = (t1 * t2);
        ac_int<32, false> sqrt = 0;
        ac_math::ac_sqrt(prod, sqrt);

        uint32 rdata;
#ifndef USE_AXI_MEM
        rdata = mem[t3];
#else
        ex_ar_payload ar;
        ar.ex_len = 0;
        ar.addr = t3;
        r_segment0_ex_ar_chan.Push(ar);
        r_payload r = r_master0.r.Pop();
        rdata = r.data;
#endif
        uint32 result = sqrt + rdata;
        out1.Push(result);
      }
    }
  }
};
