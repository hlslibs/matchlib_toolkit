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
 *  Copyright  Siemens                                                *
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

#include <mc_connections.h>

#include "dat_vld.h"

#pragma hls_design top
class dut : public sc_module
{
public:
  typedef uint32 mytype;
  sc_in<bool>    CCS_INIT_S1(clk);
  sc_in<bool>    CCS_INIT_S1(rst_bar);

  // IN_FROM_DAT(in1, mytype);
  InFromDatVld<mytype>                CCS_INIT_S1(in1_mod);
  sc_in<mytype>                       CCS_INIT_S1(in1_dat);
  Connections::Combinational<mytype>  CCS_INIT_S1(in1_in);

  // IN_FROM_VLD(in1);
  sc_in<bool>                         CCS_INIT_S1(in1_vld);

  // OUT_TO_DAT(out1, mytype);
  OutToDatVld<mytype>                 CCS_INIT_S1(out1_mod);
  sc_out<mytype>                      CCS_INIT_S1(out1_dat);
  Connections::Combinational<mytype>  CCS_INIT_S1(out1_out);

  // OUT_TO_VLD(out1);
  sc_out<bool>                        CCS_INIT_S1(out1_vld);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    // IN_FROM_DAT_VLD_BIND(in1);
    in1_mod.out1(in1_in);
    in1_mod.dat(in1_dat);
    in1_mod.vld(in1_vld);
    // OUT_TO_DAT_VLD_BIND(out1);
    out1_mod.in1(out1_out);
    out1_mod.dat(out1_dat);
    out1_mod.vld(out1_vld);
  }

private:

  void main() {
    out1_out.ResetWrite();
    in1_in.ResetRead();
    wait();
    #pragma hls_pipeline_init_interval 2
#pragma pipeline_stall_mode flush
    while (1) {
      mytype x = in1_in.Pop();
      mytype y = 0;
      y += 3 * x;
      y += 7 * x * x;
      out1_out.Push(y);
    }
  }
};

