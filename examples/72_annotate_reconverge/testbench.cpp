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

#include <mc_connections.h>
#include <ac_sysc_trace.h>

#ifndef __SYNTHESIS__
#ifndef CCS_SYSC
#include <connections/annotate.h>
#endif
#endif

#include "reconvergence.h"
#include <mc_scverify.h>

class Testbench : public sc_module
{
public:

  CCS_DESIGN(top) CCS_INIT_S1(dut1);

  sc_clock clk;
  SC_SIG(bool, rstn);

  Connections::Combinational<ac_int<16,false>>        CCS_INIT_S1(din);
  Connections::Combinational<ac_int<16,false>>        CCS_INIT_S1(dout);

  SC_CTOR(Testbench)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    sc_object_tracer<sc_clock> trace_clk(clk);

    // Bindings for the DUT
    dut1.clk(clk);
    dut1.rstn(rstn);
    dut1.din(din);
    dut1.dout(dout);

    SC_THREAD(reset);
    sensitive << clk.posedge_event();

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rstn, false);

    SC_THREAD(resp);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rstn, false);
  }

  // Drives "din" Matchlib port on DUT
  void stim() {
    CCS_LOG("Stimulus reset started");
    din.ResetWrite();
    wait();
    ac_int<16,false> aval=0;

    for (int i = 0; i < 20; i++) {
      din.Push(i+1);
    }
    wait(100);
  }

  void resp() {
    dout.ResetRead();
    wait();
    for (int i = 0; i < 20; i++) {
      ac_int<16,false> res = dout.Pop();
      CCS_LOG("TB resp sees: " << res);
    }
    sc_stop();
  }

  void reset() {
    rstn.write(0);
    wait(3); // Drive reset for 3 clock cycles
    rstn.write(1);
    wait();
  }

};

int sc_main(int argc, char **argv)
{
  // Configure error handling
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  // Create a trace file
  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file("trace");

  Testbench testbench("testbench");

  // Trace waveforms
  trace_hierarchy(&testbench, trace_file_ptr);

  // Enable data logging
  channel_logs logs;
  logs.enable("my_log",true);
  logs.log_hierarchy(testbench);

  #ifndef __SYNTHESIS__
  #ifndef CCS_SYSC
  Connections::annotate_design(testbench);
  #endif
  #endif

  sc_start();
  if (sc_report_handler::get_count(SC_ERROR) > 0) {
    std::cout << "Simulation FAILED with " << sc_report_handler::get_count(SC_ERROR) << " errors" << std::endl;
    return -1;
  }
  std::cout << "Simulation PASSED" << std::endl;
  return 0;
}
