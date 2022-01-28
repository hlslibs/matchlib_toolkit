/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 28 15:04:19 PST 2022                        *
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

#pragma once

#include "testbench.h"
#include "pixelpipe_mod.h"
#include <mc_scverify.h>

SC_MODULE(top)
{
  sc_clock        clk;
  sc_signal<bool> CCS_INIT_S1(rst);

  Connections::Combinational<ac_int<2, false>> CCS_INIT_S1(blocktype);
  Connections::Combinational<rgb_t> CCS_INIT_S1(rgbstream);
  Connections::Combinational<codes_t> CCS_INIT_S1(hufstream);

  testbench testbench_INST; // Instance of testbench

#ifdef TOP_PIXELPIPE_MOD
  CCS_DESIGN(pixelpipe_mod) CCS_INIT_S1(p_inst);
#else
  pixelpipe_mod CCS_INIT_S1(p_inst);
#endif

  SC_HAS_PROCESS(top);
  top(const sc_module_name& name, const std::string &input_image_bmp) :
    clk("clk", 5.00, SC_NS, 0.5, 0, SC_NS, true),
    testbench_INST("testbench_INST",input_image_bmp) {
    Connections::set_sim_clk(&clk);
    sc_object_tracer<sc_clock> trace_clk(clk);

    testbench_INST.clk(clk);
    testbench_INST.rst(rst);
    testbench_INST.blocktype(blocktype);
    testbench_INST.rgbstream(rgbstream);
    testbench_INST.hufstream(hufstream);

    p_inst.clk(clk);
    p_inst.rstn(rst);
    p_inst.blocktype(blocktype);
    p_inst.rgb(rgbstream);
    p_inst.codes(hufstream);

    SC_THREAD(reset);
  }

  void reset() {
    rst.write(0);
    wait(50, SC_NS);                        // WAIT
    rst.write(1);
  }
};

