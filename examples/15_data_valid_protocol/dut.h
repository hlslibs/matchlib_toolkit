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

#include <stdlib.h>
#include <mc_connections.h>

struct packet : public Connections::message {
  static const int data_len = 4;

  ac_int<32,true> coeff;
  ac_int<32,true> data[data_len];

  static const unsigned int width = 32 + (32 * data_len);

  template <unsigned int Size> void Marshall(Marshaller<Size> &m) {
    m &coeff;
    for (int i=0; i<data_len; i++) {
      m &data[i];
    }
  }
  inline friend void sc_trace(sc_trace_file *tf, const packet &v, const std::string &NAME ) {
    sc_trace(tf,v.coeff,  NAME + ".ar_addr");
    for (int i=0; i<data_len; i++) {
      std::ostringstream os;
      os << NAME << ".data_" << i;
      sc_trace(tf,v.data[i],  os.str());
    }
  }
};


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  sc_in <packet> CCS_INIT_S1(in1_data);
  sc_in <bool  > CCS_INIT_S1(in1_vld);
  sc_out<packet> CCS_INIT_S1(out1_data);
  sc_out<bool  > CCS_INIT_S1(out1_vld);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  void main() {
    out1_vld = false;
    out1_data = packet();

    wait();

    // #pragma hls_pipeline_init_interval 1
    // #pragma pipeline_stall_mode flush
    while (1) {
      do {
        wait();
      } while (!in1_vld);
      packet p = in1_data;

      // Note that we leave inner loop rolled. This will stall the pipeline for several cycles,
      // and this will cause the RTL to drop input packets as compared to the SC DUT.
      // This is to be expected, since the data/valid protocol is time-sensitive and may drop data.
      // #pragma unroll yes
      for (int i=0; i < packet::data_len; i++) {
        p.data[i] *= p.coeff;
      }

      out1_data = p;
      out1_vld = true;
    }
  }
};

