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

