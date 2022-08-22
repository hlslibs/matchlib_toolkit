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

SC_MODULE(dut)
{
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::In<ac_int<16> >  CCS_INIT_S1(in1);
  Connections::Out<ac_int<16> > CCS_INIT_S1(out1);
  Connections::In<ac_int<16> >  CCS_INIT_S1(in2);
  Connections::Out<ac_int<16> > CCS_INIT_S1(out2);

  SC_CTOR(dut) {
    SC_THREAD(thread1);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
    SC_THREAD(thread2);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  SC_SIG(ac_int<16>, thread1_out);
  SC_SIG(ac_int<16>, thread2_out);

  void thread1() {
    ac_int<16> count = 0;
    thread1_out = 0;
    in1.Reset();
    out1.Reset();
    sync1.reset_sync_out();
    wait();                                 // WAIT

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      ac_int<16> v = in1.Pop();
      if ((count++ & 0x3) == 0x3) {
        // swap input values with thread2
        thread1_out = v;
        sync1.sync_out();
        v = thread2_out;
      }
      out1.Push(v);
    }
  }

  void thread2() {
    ac_int<16> count = 0;
    thread2_out = 0;
    in2.Reset();
    out2.Reset();
    sync1.reset_sync_in();
    wait();                                 // WAIT

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      ac_int<16> v = in2.Pop();
      if ((count++ & 0x3) == 0x3) {
        // swap input values with thread1
        thread2_out = v;
        sync1.sync_in();
        v = thread1_out;
      }
      out2.Push(v);
    }
  }

  private:
  Connections::SyncChannel CCS_INIT_S1(sync1); // synchronization between threads
};

