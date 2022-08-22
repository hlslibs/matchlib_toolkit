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

#include <mc_connections.h>
#include <ac_math.h>

static const int num_direct_inputs = 8;
static const int num_samples = 3;

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool>               CCS_INIT_S1(clk);
  sc_in<bool>               CCS_INIT_S1(rst_bar);

  Connections::Out<uint32>  CCS_INIT_S1(out1);
  Connections::In <uint32>  sample_in[num_samples];
  Connections::SyncIn       CCS_INIT_S1(sync_in);

  #ifdef HLS_DIRECT_INPUT
  #pragma hls_direct_input
  #endif
  sc_in<uint32_t> direct_inputs[num_direct_inputs];

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  void main() {
    out1.Reset();
    sync_in.Reset();

    #pragma hls_unroll yes
    for (int i=0; i < num_samples; i++) {
      sample_in[i].Reset();
    }

    wait();  // reset state

    while (1) {
      sync_in.sync_in();  // DIRECT_INPUT_SYNC for direct inputs syncs to this

      #pragma hls_pipeline_init_interval 1
      #pragma hls_stall_mode flush
      for (uint32_t x=0; x < direct_inputs[0]; x++) {
        for (uint32_t y=0; y < direct_inputs[1]; y++) {
          uint32_t sum = 0;
          #pragma hls_unroll yes
          for (uint32_t s=0; s < num_samples; s++) {
            sum += sample_in[s].Pop() * direct_inputs[2 + s];
          }
          ac_int<32, false> ac_sum = sum;
          ac_int<32, false> sqrt = 0;
          ac_math::ac_sqrt(ac_sum, sqrt);  // internal loop is unrolled in catapult .tcl file
          if (sqrt > direct_inputs[7]) { out1.Push(sqrt); }
        }
      }
    }
  }
};

