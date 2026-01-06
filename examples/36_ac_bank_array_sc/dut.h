/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2025.4                                              *
 *                                                                        *
 *  Release Date    : Thu Dec 11 11:04:23 PST 2025                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 2025.4.1                                            *
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
#include <ac_bank_array.h>
#include "auto_gen_fields.h"


static const int NumDims = 2;
constexpr int DimSize[NumDims] = {0x10, 0x1000};

struct mem_req {
  bool is_write{0};
  uint16 index[NumDims];
  uint16 data;

  AUTO_GEN_FIELD_METHODS(mem_req, ( \
     is_write \
   , index \
   , data \
  ) )
  //
};


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool>                             CCS_INIT_S1(clk);
  sc_in<bool>                             CCS_INIT_S1(rst_bar);
  Connections::Out<uint16>                CCS_INIT_S1(out1);
  Connections::In<mem_req>                CCS_INIT_S1(in1);

  // uint16 mem[0x10][0x1000];
  // ac_bank_array_3D<uint16, 4, 4, 0x1000> mem;
  // ac_bank_array_2D<uint16, DimSize[0], DimSize[1]> mem;
  // ac_bank_array_vary<uint16 [0x1000], 0x10> mem;
  ac_bank_array_2D<uint16, DimSize[0], DimSize[1]> mem;

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  void main() {
    in1.Reset();
    out1.Reset();
    wait();

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      mem_req req1 = in1.Pop();

      if (req1.is_write) {
        mem[req1.index[0]][req1.index[1]] = req1.data;
      } else {
        uint16 rdata;
        rdata = mem[req1.index[0]][req1.index[1]];
        out1.Push(rdata);
      }
    }
  }
};
