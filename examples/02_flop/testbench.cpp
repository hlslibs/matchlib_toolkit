/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 28 15:07:52 PST 2022                        *
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

#include "flop.h"
#include <mc_scverify.h>
using namespace::std;

class Top : public sc_module
{
public:
  CCS_DESIGN(flop) CCS_INIT_S1(flop1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);
  SC_SIG(uint32_t, in1);
  SC_SIG(uint32_t, out1);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    flop1.clk(clk);
    flop1.rst_bar(rst_bar);
    flop1.in1(in1);
    flop1.out1(out1);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    sc_object_tracer<sc_clock> trace_clk(clk);
  }

  void stim() {
    CCS_LOG("Stimulus started");
    rst_bar = 0;
    in1 = 0;
    wait(5);

    // take dut out of reset
    rst_bar = 1;

    for (int i=0; i<10; i++) {
      in1 = i;
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

