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


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  typedef ac_int<32, false> T;
  static const int N = 8;

  Connections::In <T> CCS_INIT_S1(in1);
  Connections::Out<T> CCS_INIT_S1(out1);

  Connections::Fifo<T, N> CCS_INIT_S1(fifo1);
  Connections::Combinational<T> CCS_INIT_S1(fifo1_in1);
  Connections::Combinational<T> CCS_INIT_S1(fifo1_out1);

  SC_CTOR(dut) {
    fifo1.clk(clk);
    fifo1.rst(rst_bar);
    fifo1.enq(fifo1_in1);
    fifo1.deq(fifo1_out1);

    SC_THREAD(main1);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(main2);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main1() {
    fifo1_in1.ResetWrite();
    in1.Reset();
    wait();  
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) { fifo1_in1.Push(in1.Pop()); }
  }

  void main2() {
    fifo1_out1.ResetRead();
    out1.Reset();
    wait();                                 // WAIT
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) { out1.Push(fifo1_out1.Pop()); }
  }
};

