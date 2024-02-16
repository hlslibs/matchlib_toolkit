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


#include "dut.h"
#include <mc_scverify.h>
#include <fstream>

ac_channel<dir_input_t> dir_input_chan;


CCS_MAIN(int argc, char **argv)
{
  dut dut1;

  ac_channel<uint32> out1;
  ac_channel<uint32> sample_in[num_samples];
  ac_sync sync_chan;
  uint32 direct_inputs[num_direct_inputs];

  std::ofstream log_stream;
  std::string resp_log("resp.log");

  log_stream.open(resp_log);

  for (int y=0; y < num_direct_inputs; y++) {
      direct_inputs[y] = y;
  }

  direct_inputs[0] = 5;
  direct_inputs[1] = 5;
  direct_inputs[7] = 4;

  log_stream << "START_OF_SIMULATION\n";

  for (int i = 0; i < 10; i++) {
      // dynamically update dir inputs here:
      direct_inputs[2] = 1 + direct_inputs[2];
      uint32 d4 = direct_inputs[4];
      uint32 d3 = direct_inputs[3];
      direct_inputs[4] = d3;
      direct_inputs[3] = d4;
      direct_inputs[7] = 4 + (i&3);

      dir_input_t v;

      for (int n=0; n < num_direct_inputs; n++)
        v.inputs[n] = direct_inputs[n];

      // In the pre-HLS sim, the DUT direct inputs get updated via writes to this "dir_input_chan".
      // This channel is a pre-HLS simulation "artifact" and is not part of the DUT HW interface.
      dir_input_chan.write(v);

      for (uint32_t x=0; x < direct_inputs[0]; x++) {
        for (uint32_t y=0; y < direct_inputs[1]; y++) {
          for (uint32_t s=0; s < num_samples; s++) {
            sample_in[s].write((x*y) + i + s);
          }
        }
      }

      // In the post-HLS sim, the DUT direct inputs get updated when the sync_chan sync occurs,
      // which is when the DUT calls sync_out().
      dut1.run(sample_in, direct_inputs, sync_chan, out1);
      sync_chan.sync_in();  // In reality, the sync_out occurred earlier, but we "consume" it here..
  }

  while (out1.available(1)) {
      uint32_t t = out1.read();
      // std::cout << "TB resp sees: " << std::hex << t  << "\n";
      log_stream << "TB resp sees: " << std::hex << t << "\n";
  }

  log_stream << "END_OF_SIMULATION\n";
  log_stream.close();

  CCS_RETURN(0);
}
