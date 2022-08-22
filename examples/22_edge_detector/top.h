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

#include "edge_hierarchy.h"    // The design under test (what Catapult synthesizes)
#include "testbench.h"  // The stimulus and correctness checking.
#include <mc_scverify.h>  // Supplies the CCS_DESIGN macro which makes it easier to run SCVerify flows.

SC_MODULE (top)
{
  CCS_DESIGN(EdgeHierarchy) EdgeHierarchy_INST;   	// instance of Design Under Test.
  testbench testbench_INST; // instance of testbench

  sc_clock       	clk;
  sc_signal<bool>	rst_bar;
  Connections::Combinational<uint8> dat_in;
  Connections::Combinational<uint9> magn;
  Connections::Combinational<ac_fixed<8,3> > angle;

  SC_HAS_PROCESS(top);
  top(const sc_module_name& name, const std::string &input_image_bmp) :
    testbench_INST("testbench_INST",input_image_bmp),
    EdgeHierarchy_INST("EdgeHierarchy_INST"),
    clk("clk",10, SC_NS, 0.5, 0, SC_NS, true),
    rst_bar("rst_bar"),
    dat_in("dat_in"),
    magn("magn"),
    angle("angle") {
    Connections::set_sim_clk(&clk); // needed for matchlib simulation
    // DUT connections
    EdgeHierarchy_INST.clk(clk);
    EdgeHierarchy_INST.rst_bar(rst_bar);
    EdgeHierarchy_INST.dat_in(dat_in);
    EdgeHierarchy_INST.magn(magn);
    EdgeHierarchy_INST.angle(angle);
    // stimulus and checker connections
    testbench_INST.clk(clk);
    testbench_INST.rst_bar(rst_bar);
    testbench_INST.dat_in(dat_in);
    testbench_INST.magn(magn);
    testbench_INST.angle(angle);
  }
};

