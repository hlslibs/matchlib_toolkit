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

#include <ac_sysc_macros.h>
#include <mc_connections.h>

typedef uint32_t T;

class func : public sc_module
{
public:
  sc_in<bool>      clk{"clk"};
  sc_in<bool>      rst_bar{"rst_bar"};
  sc_in<T>         in0{"in0"};
  sc_in<T>         in1{"in1"};
  sc_out<T>        out1{"out1"};

  SC_CTOR(func) {
    SC_THREAD(process);  sensitive << clk.pos();   async_reset_signal_is(rst_bar, false);
  }

  void process() {
    out1 = 0;
    wait();                                 
    while (1) {
      out1 = in0.read() + in1.read();
      wait();                              
    }
  }
};

#pragma hls_design top
class dut : public sc_module
{
public:
  static const int len = 6;                   // TODO -- pass len in to ctor

  sc_in<bool>      clk{"clk"};
  sc_in<bool>      rst_bar{"rst_bar"};
  sc_in<T>  in1[len];
  sc_out<T> out1{"out1"};

  sc_signal<T>* sig_array;

  SC_CTOR(dut) {
    sig_array = new sc_signal<T>[len/2] ;
    func* func_array[len/2];

    for (int i=1, k=0; i < len; i += 2, k++) {
        func_array[k] = new func("");
        func_array[k]->clk(clk);
        func_array[k]->rst_bar(rst_bar);
        func_array[k]->in0(in1[i-1]);
        func_array[k]->in1(in1[i]);
        func_array[k]->out1(sig_array[k]);
    }

    SC_THREAD(sum);  sensitive << clk.pos();  async_reset_signal_is(rst_bar, false);
  }

  #pragma hls_implicit_fsm true
  void sum() {
    out1 = 0;
    wait();
    while (true) {
      T t{0};
      #pragma unroll yes
      for (int k=0; k<(len/2); k++) {
        t = t + sig_array[k].read(); 
      }
      out1.write(t);
      wait();
    }
  }
};

