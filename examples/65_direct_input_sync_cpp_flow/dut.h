/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2.1                                                 *
 *                                                                        *
 *  Release Date    : Mon Jan 15 20:15:38 PST 2024                        *
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

#include "ac_channel.h"
#include "ac_sync.h"
#include "mc_scverify.h"
#include <ac_math.h>

static const int num_direct_inputs = 8;
static const int num_samples = 3;

struct dir_input_t {
  uint32 inputs[num_direct_inputs];
};

extern ac_channel<dir_input_t> dir_input_chan;

class dut 
{
public:
  dut() {}

#pragma hls_design interface top
  void CCS_BLOCK(run)(
      ac_channel<uint32> sample_in[num_samples]
    , uint32 direct_inputs[num_direct_inputs]
    , ac_sync& sync_out
    , ac_channel<uint32>& out1
  )
  {
#pragma hls_direct_input_sync all
      sync_out.sync_out(); 

      // here we force the direct inputs to be updated at exactly the point in the DUT where
      // the sync_out() operation occurs. This insures the pre-hls and post-hls simulations will match.
#ifndef __SYNTHESIS__
      dir_input_t v = dir_input_chan.read();
      for (int i=0; i < num_direct_inputs; i++)
        direct_inputs[i] = v.inputs[i];
#endif

      #pragma hls_pipeline_init_interval 1
      #pragma hls_stall_mode flush
      for (uint32_t x=0; x < direct_inputs[0]; x++) {
        for (uint32_t y=0; y < direct_inputs[1]; y++) {
          uint32_t sum = 0;
          #pragma hls_unroll yes
          for (uint32_t s=0; s < num_samples; s++) {
            sum += sample_in[s].read() * direct_inputs[2 + s];
          }
          ac_int<32, false> ac_sum = sum;
          ac_int<32, false> sqrt = 0;
          ac_math::ac_sqrt(ac_sum, sqrt);  // internal loop is unrolled in catapult .tcl file
          if (sqrt > direct_inputs[7]) { out1.write(sqrt); }
        }
      }
  }
};
