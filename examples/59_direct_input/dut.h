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

static const int num_coefs = 16;

struct coefs_t {
  uint32_t coef[num_coefs];

  static const unsigned int width = 8 * sizeof(uint32_t) * num_coefs;
  template <unsigned int Size> void Marshall(Marshaller<Size> &m) {
    for (int i=0; i < num_coefs; i++) { m &coef[i]; }
  }
  inline friend void sc_trace(sc_trace_file *tf, const coefs_t &v, const std::string &NAME ) {
    for (int i=0; i < num_coefs; i++) { sc_trace(tf,v.coef[i],  NAME + ".coef_" ); }
  }
  inline friend std::ostream &operator<<(ostream &os, const coefs_t &rhs) {
    for (int i=0; i < num_coefs; i++) { os << rhs.coef[i] << " "; }
    return os;
  }
};

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool>               CCS_INIT_S1(clk);
  sc_in<bool>               CCS_INIT_S1(rst_bar);

  Connections::Out<uint32>  CCS_INIT_S1(out1);
  Connections::In <uint32>  CCS_INIT_S1(sample_in);
  Connections::SyncIn       CCS_INIT_S1(sync_in);

  #ifdef HLS_DIRECT_INPUT
  #pragma hls_direct_input
  #endif
  sc_in<uint32_t> coefs_in[num_coefs];

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  void main() {
    out1.Reset();
    sample_in.Reset();
    sync_in.Reset();
    uint32_t samples[num_coefs];

    #pragma hls_unroll yes
    for (int i=0; i < num_coefs; i++) {
      samples[i] = 0;
    }

    wait();  // reset state

    while (1) {
      sync_in.sync_in();

      #pragma hls_pipeline_init_interval 4
      #pragma pipeline_stall_mode flush
      for (int s=0; s < num_coefs; s++) {
        coefs_t coefs;

        #pragma hls_unroll yes
        for (int i=0; i < num_coefs; i++) {
          coefs.coef[i] = coefs_in[i];
        }

        #pragma hls_unroll yes
        for (int i=num_coefs-1; i > 0 ; i--) {
          samples[i] = samples[i-1];
        }

        samples[0] = sample_in.Pop();

        uint32_t sum = 0;

        #pragma hls_unroll yes
        for (int i=0; i < num_coefs; i++) {
          sum += (coefs.coef[i] * samples[i]) + coefs.coef[i];
        }

        out1.Push(sum);
      }
    }
  }
};

