/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2.1       *
 *                                                                        *
 *  Release Date    : Mon Jan 15 20:15:38 PST 2024       *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 2.1.1       *
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


#include <mc_scverify.h>

#include "dut.h"

unsigned rand_up_to(unsigned n) {
   return rand() & (n-1);
}

CCS_MAIN(int argc, char **argv)
{
  dut dut1;

  ac_channel<mem_req> in1_chan;
  ac_channel<uint16> out1_chan;

  static constexpr unsigned test_cnt = 1000;
  mem_req writes[test_cnt];
  uint16 ref_mem[DimSize[0]][DimSize[1]];

  // First write to the memory in DUT
  for (unsigned i=0; i < test_cnt; i++) {
      mem_req req1;
      req1.is_write = 1;
      req1.index[0] = rand_up_to(DimSize[0]);
      req1.index[1] = rand_up_to(DimSize[1]);
      req1.data  = rand();
      writes[i] = req1;
      ref_mem[req1.index[0]][req1.index[1]] = req1.data;
      in1_chan.write(req1);
      dut1.run(in1_chan, out1_chan);
  }

  // Now read the memory in DUT
  for (unsigned i=0; i < test_cnt; i++) {
      mem_req req1 = writes[i];
      req1.is_write = 0;
      in1_chan.write(req1);
      dut1.run(in1_chan, out1_chan);
  }

  // Now check the DUT output data
  for (unsigned i=0; i < test_cnt; i++) {
      uint16 r = out1_chan.read();
      if (r != ref_mem[writes[i].index[0]][writes[i].index[1]]) {
        std::cout << "ERROR: index: " << i << "\n";
        return 1;
      }

      std::cout << "TB response: " << r << "\n";
  }

  std::cout << "Test passed" << "\n";

  return 0;
}
