/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2.3                                                 *
 *                                                                        *
 *  Release Date    : Tue May 13 15:55:46 PDT 2025                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 2.3.0                                               *
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

#define LATENCY_CONTROL_BEGIN() \
	_Pragma("hls_preserve_loop yes") \
        while(true){


#define LATENCY_CONTROL_END() \
          break; \
        }


struct packet : public Connections::message {
  static const int data_len = 10;

  ac_int<32,false> coeff;
  ac_int<32,false> data[data_len];

  AUTO_GEN_FIELD_METHODS(packet, ( \
     coeff \
   , data \
  ) )
  //
};


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::Out<packet> CCS_INIT_S1(out1);
  Connections::In <packet> CCS_INIT_S1(in1);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  void main() {
    out1.Reset();
    in1.Reset();
    wait();                                 // WAIT
#define DESIGN_3 true
#ifdef DESIGN_1
    while (1) {
      packet p = in1.Pop();
      for (int i=0; i < packet::data_len; i++) { p.data[i] *= p.coeff; }
      out1.Push(p);
    }

#elif DESIGN_2
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      packet p = in1.Pop();
      for (int i=0; i < packet::data_len; i++) { p.data[i] *= p.coeff; }
      out1.Push(p);
    }

#elif DESIGN_3
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      packet p = in1.Pop();
#pragma hls_unroll yes
      for (int i=0; i < packet::data_len; i++) { p.data[i] *= p.coeff; }
      out1.Push(p);
    }

#elif DESIGN_4
#pragma hls_pipeline_init_interval 2
#pragma pipeline_stall_mode flush
    while (1) {
      packet p = in1.Pop();
#pragma hls_unroll yes
      for (int i=0; i < packet::data_len; i++) { p.data[i] *= p.coeff; }
      out1.Push(p);
    }

#elif DESIGN_5
    while (1) {
      packet p = in1.Pop();

      if (p.coeff == 0) {
#pragma hls_unroll yes
        for (int i=0; i < packet::data_len; i++) { p.data[i] = 0; }
      } else if (p.coeff != 1) {
#pragma hls_unroll yes
        for (int i=0; i < packet::data_len; i++) { p.data[i] *= p.coeff; }
      }
      out1.Push(p);
    }

#elif DESIGN_6

    while (1) {
      packet p = in1.Pop();
      if (p.coeff == 0) {
#pragma hls_unroll yes
        for (int i=0; i < packet::data_len; i++) { p.data[i] = 0; }
      } else if (p.coeff != 1) {
        for (int i=0; i < packet::data_len; i++) { p.data[i] *= p.coeff; }
      }
      out1.Push(p);
    }

#elif DESIGN_7
    while (1) {
      packet p = in1.Pop();

      if (p.coeff == 0) {
        LATENCY_CONTROL_BEGIN()
#pragma hls_unroll yes
        for (int i=0; i < packet::data_len; i++) { p.data[i] = 0; }
        LATENCY_CONTROL_END()
      } else if (p.coeff != 1) {
        LATENCY_CONTROL_BEGIN()
#pragma hls_unroll yes
        for (int i=0; i < packet::data_len; i++) { p.data[i] *= p.coeff; }
        LATENCY_CONTROL_END()
      }
      out1.Push(p);
    }

#elif DESIGN_8

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      packet p = in1.Pop();

      if (p.coeff == 0) {
#pragma hls_unroll yes
        for (int i=0; i < packet::data_len; i++) { p.data[i] = 0; }
      } else if (p.coeff != 1) {
        for (int i=0; i < packet::data_len; i++) { p.data[i] *= p.coeff; }
      }
      out1.Push(p);
    }


#elif DESIGN_9
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      packet p = in1.Pop();
#pragma hls_unroll 5
      for (int i=0; i < packet::data_len; i++) { p.data[i] *= p.coeff; }
      out1.Push(p);
    }
#endif
  }
};

