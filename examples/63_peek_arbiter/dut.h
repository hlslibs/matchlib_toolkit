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


#pragma once

#include <mc_connections.h>
#include "auto_gen_fields.h"

typedef uint32 DTYPE;

struct packet {
static const int SIZE = 64;
  DTYPE data[SIZE];

  AUTO_GEN_FIELD_METHODS(packet, ( \
     data \
  ) )
};

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::In <DTYPE> CCS_INIT_S1(in1_peek);
  Connections::In <DTYPE> CCS_INIT_S1(in2_peek);
  Connections::In <packet> CCS_INIT_S1(in1_dat);
  Connections::In <packet> CCS_INIT_S1(in2_dat);
  Connections::Out<packet> CCS_INIT_S1(out1);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  void main() {
    out1.Reset();
    in1_peek.Reset();
    in2_peek.Reset();
    in1_dat.Reset();
    in2_dat.Reset();
    wait();

    bool peek1_vld{0};
    bool peek2_vld{0};
    DTYPE peek1, peek2;

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      wait();

      if (!peek1_vld)
        peek1_vld = in1_peek.PopNB(peek1);
      if (!peek2_vld)
        peek2_vld = in2_peek.PopNB(peek2);

      if (peek1_vld && peek2_vld) {
        // if both inputs are available give priority to input with higher data value
        if (peek1 > peek2) {
          out1.Push(in1_dat.Pop());
          peek1_vld = 0;
        } else {
          out1.Push(in2_dat.Pop());
          peek2_vld = 0;
        }
      } else if (peek1_vld) {
          out1.Push(in1_dat.Pop());
          peek1_vld = 0;
      } else if (peek2_vld) {
          out1.Push(in2_dat.Pop());
          peek2_vld = 0;
      }
    }
  }
};
