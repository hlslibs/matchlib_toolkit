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

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  sc_out<bool> CCS_INIT_S1(in1_rdy);
  sc_in<bool>  CCS_INIT_S1(in1_vld);
  sc_in<sc_uint<32>>  CCS_INIT_S1(in1_data);

  sc_in<bool>  CCS_INIT_S1(out1_rdy);
  sc_out<bool> CCS_INIT_S1(out1_vld);
  sc_out<sc_uint<32>> CCS_INIT_S1(out1_data);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  sc_uint<32> in1_Pop() {
    sc_uint<32> i1;

    in1_rdy = 1;
    do {
      wait();                               // WAIT
    } while (!in1_vld);
    i1 = in1_data;
    in1_rdy = 0;
    return i1;
  }

  void out1_Push(sc_uint<32> o) {
    out1_vld = 1;
    out1_data = o;

    do {
      wait();                               // WAIT
    } while (!out1_rdy);
    out1_vld = 0;
  }

  void main() {
    in1_rdy = 0;
    out1_vld = 0;
    out1_data = 0;
    wait();                                 // WAIT
    while (1) {
      sc_uint<32> i1;
      i1 = in1_Pop();
      out1_Push(i1 + 0x100);
    }
  }
};

