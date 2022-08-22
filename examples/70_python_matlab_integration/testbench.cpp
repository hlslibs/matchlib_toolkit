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
 *  Copyright 2022 Siemens                                                *
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


#include <systemc.h>
#include "dut.h"
#include <mc_scverify.h>

using namespace::std;

class Top : public sc_module
{
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);

  Connections::Combinational<dut::T>        CCS_INIT_S1(out1);
  Connections::Combinational<dut::T>        CCS_INIT_S1(in1);
  int matlab_input;
  bool matlab_input_valid;
  int matlab_output;
  bool matlab_output_valid;

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
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
    int i1 = 0;

    while (1) {
      #ifdef EXTERNAL_TESTBENCH
      if (matlab_input_valid) {
        in1.Push(matlab_input);
        matlab_input_valid = 0;
      } else {
        wait();
      }
      #else
      in1.Push(i1++);
      if (i1 > 10) { sc_stop(); }
      #endif
    }
  }

  void resp() {
    out1.ResetRead();
    wait();

    while (1) {
      #ifdef EXTERNAL_TESTBENCH
      matlab_output = out1.Pop();
      matlab_output_valid = 1;
      std::cout << "calling sc_pause()\n";
      sc_pause();
      #else
      CCS_LOG("See: " << out1.Pop());
      #endif
    }
  }

  void reset() {
    rst_bar.write(0);
    wait(5);
    rst_bar.write(1);
    wait();
  }
};

Top *top_ptr{0};

// sc_main instantiates the SC hierarchy but does not run the simulation - that is handled elsewhere
int sc_main(int argc, char **argv)
{
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  // This function instantiates the design hiearchy including the testbench.

  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file("trace");

  top_ptr = new Top("top");
  trace_hierarchy(top_ptr, trace_file_ptr);

  #ifndef EXTERNAL_TESTBENCH
  sc_start();
  #endif
  return 0;
}

#ifdef EXTERNAL_TESTBENCH

// This class represents the SC simulator to Python or Matlab Mex
class sc_simulator
{
public:
  sc_simulator() {
    sc_elab_and_sim(0, 0);
  }

// This function represents the python or Matlab Mex function that would be called repeatedly
// to process one (or more) data inputs and return one (or more) data outputs
  int process_one_sample(int in1) {
    top_ptr->matlab_input = in1;
    top_ptr->matlab_input_valid = true;
    std::cout << "calling sc_start()\n";
    sc_start(); // This returns when sc_pause is called above

    return top_ptr->matlab_output;
  }
};

// Python does not have a native C++ interface, so we export regular C functions to Python
extern "C" {
  sc_simulator *sc_simulator_new() { return new sc_simulator(); }
  int process_one_sample(sc_simulator *sim, int in1) { return sim->process_one_sample(in1); }
}

#endif
