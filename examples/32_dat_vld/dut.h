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

#include <mc_connections.h>

#include "dat_vld.h"

#pragma hls_design top
class dut : public sc_module
{
public:
  typedef uint32 T;
  sc_in<bool>    CCS_INIT_S1(clk);
  sc_in<bool>    CCS_INIT_S1(rst_bar);

  IN_FROM_DAT(in1, T);
  IN_FROM_VLD(in1);
  OUT_TO_DAT(out1, T);
  OUT_TO_VLD(out1);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    IN_FROM_DAT_VLD_BIND(in1);
    OUT_TO_DAT_VLD_BIND(out1);
  }

private:

  void main() {
    out1_out.ResetWrite();
    in1_in.ResetRead();
    wait(); 
// NOTE: stall_mode must be stall for this example. If it is flush then FSM may apply backpressure,
// which would cause the protocol adaptor to operate incorrectly

#pragma hls_pipeline_init_interval 2
#pragma pipeline_stall_mode stall
    while (1) {
      uint32 x = in1_in.Pop();
      uint32 y = 0;
      y += 3 * x;
      y += 7 * x * x;
      out1_out.Push(y);
    }
  }
};
