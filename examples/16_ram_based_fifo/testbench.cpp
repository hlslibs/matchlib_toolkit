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

class Top : public sc_module {
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);
  sc_clock clk;
  SC_SIG(bool, rst_bar);
  Connections::Combinational<sc_uint<32>>        CCS_INIT_S1(out1);
  Connections::Combinational<sc_uint<32>>        CCS_INIT_S1(in1);

  SC_CTOR(Top) 
     :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) 
  {
    Connections::set_sim_clk(&clk);
    sc_object_tracer<sc_clock> trace_clk(clk);
    
    dut1.clk(clk);
    dut1.rst_bar(rst_bar);
    dut1.out1(out1);
    dut1.in1(in1);

    SC_CTHREAD(reset, clk);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(resp);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);
  }


  void stim() {
    CCS_LOG("Stimulus started");
    in1.ResetWrite();
    wait();

    for (int i = 0; i < 10; i++)
    in1.Push(i);

    wait(100);
    
    sc_stop();
  }

  void resp() {
    out1.ResetRead();
    wait();

    while (1)
      CCS_LOG("TB resp sees: " << std::hex << out1.Pop());
  }

  void reset() {
    rst_bar.write(0);
    wait(5);
    rst_bar.write(1);
    wait();
  }
};


sc_trace_file* trace_file_ptr;

int sc_main(int argc, char** argv) {
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  trace_file_ptr = sc_create_vcd_trace_file("trace");

  Top top("top");
  trace_hierarchy(&top, trace_file_ptr);
  sc_start();
  return 0;
}
