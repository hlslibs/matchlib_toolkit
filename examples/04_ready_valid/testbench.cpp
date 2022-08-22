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

#include "dut.h"
#include <mc_scverify.h>

class Top : public sc_module
{
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);

  SC_SIG(bool, in1_rdy);
  SC_SIG(bool, in1_vld);
  SC_SIG(sc_uint<32>, in1_data);
  SC_SIG(bool, out1_rdy);
  SC_SIG(bool, out1_vld);
  SC_SIG(sc_uint<32>, out1_data);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    sc_object_tracer<sc_clock> trace_clk(clk);

    dut1.clk(clk);
    dut1.rst_bar(rst_bar);

    dut1.in1_rdy(in1_rdy);
    dut1.in1_vld(in1_vld);
    dut1.in1_data(in1_data);

    dut1.out1_rdy(out1_rdy);
    dut1.out1_vld(out1_vld);
    dut1.out1_data(out1_data);

    SC_CTHREAD(reset, clk);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(resp);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);
  }

  sc_uint<32> out1_Pop() {
    sc_uint<32> i1;

    out1_rdy = 1;

    do {
      wait();
    } while (!out1_vld);

    out1_rdy = 0;
    i1 = out1_data;

    return i1;
  }

  void in1_Push(sc_uint<32> o) {
    in1_vld = 1;
    in1_data = o;

    do {
      wait();
    } while (!in1_rdy);

    in1_vld = 0;
  }


  void stim() {
    CCS_LOG("Stimulus started");
    in1_vld = 0;
    wait();

    for (int i = 0; i < 10; i++)
    { in1_Push(i); }

    sc_stop();
    wait();
  }

  void resp() {
    out1_rdy = 0;
    wait();

    while (1) {
      sc_uint<32> o1 = out1_Pop();

      CCS_LOG("TB resp sees: " << std::hex << o1);
    }
  }

  void reset() {
    rst_bar.write(0);
    wait(5);
    rst_bar.write(1);
    wait();
  }
};


int sc_main(int argc, char **argv)
{
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file("trace");

  Top top("top");
  trace_hierarchy(&top, trace_file_ptr);
  sc_start();
  if (sc_report_handler::get_count(SC_ERROR) > 0) {
    std::cout << "Simulation FAILED" << std::endl;
    return -1;
  }
  std::cout << "Simulation PASSED" << std::endl;
  return 0;
}

