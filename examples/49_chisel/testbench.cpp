/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 28 15:18:06 PST 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.7                                               *
 *                                                                        *
 *  Unpublished work. Copyright 2020 Siemens                              *
 *                                                                        *
 **************************************************************************/

// This document contains information that is proprietary to Siemens
// The original recipient of this document may duplicate this  
// document in whole or in part for internal business purposes only, provided  
// that this entire notice appears in all copies. In duplicating any part of  
// this document, the recipient agrees to make every reasonable effort to  
// prevent the unauthorized use and distribution of the proprietary information.

#define SC_INCLUDE_DYNAMIC_PROCESSES 1
#include <ac_sysc_trace.h>
#include "dut.h"

#include <mc_scverify.h>
using namespace::std;

class Top : public sc_module
{
  static const int N = 6;
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk{"clk"};
  SC_SIG(bool, rst_bar);
  sc_signal<uint32_t>    in1[N];    // prefer sc_vector
  SC_SIG(uint32_t, out1);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    dut1.clk(clk);
    dut1.rst_bar(rst_bar);
    for (auto i=0; i<N; i++) {
      dut1.in1[i](in1[i]);
    }
    dut1.out1(out1);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    sc_object_tracer<sc_clock> trace_clk(clk);
    sc_object_tracer<sc_signal<uint32_t>> in1_0(in1[0]);
    sc_object_tracer<sc_signal<uint32_t>> in1_1(in1[1]);
    sc_object_tracer<sc_signal<uint32_t>> in1_2(in1[2]);
    sc_object_tracer<sc_signal<uint32_t>> in1_3(in1[3]);
    sc_object_tracer<sc_signal<uint32_t>> in1_4(in1[4]);
    sc_object_tracer<sc_signal<uint32_t>> in1_5(in1[5]);
  }

  void stim() {
    CCS_LOG("Stimulus started");
    rst_bar = 0;
    wait(5);

    // take dut out of reset
    rst_bar = 1;

    for (int i=0; i<10; i++) {
      for (auto j=0; j<N; j++) {
        in1[j] = j + i;
      }
      wait();
    }

    wait(5);
    CCS_LOG("stopping sim at end of stimulus");
    sc_stop();
    wait(5);
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
