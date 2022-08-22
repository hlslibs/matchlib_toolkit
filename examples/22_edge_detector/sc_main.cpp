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

#include "top.h"     // contains the DUT and testbench
#include <iostream>

/*! Test Harness Overview.
   - The sc_main simply instantiates top which contains both the DUT and the testbench.
   - The testbench issues SC_ERRORs on any failures.
   - sc_main uses the count of errors to return an appropriate error status
   - 0 == NO_ERROR
 */

int sc_main (int argc, char *argv[])
{
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <input_image_bmp> <output_trace_vcd>" << std::endl;
    std::cerr << "where:  <input_image_bmp> - path to input BMP image file" << std::endl;
    std::cerr << "        <output_trace_vcd> - path to output VCD trace file (no suffix)" << std::endl;
    return -1;
  }
  std::string input_image_bmp = argv[1];
  std::string output_trace_vcd = argv[2];

  std::cout << "Creating VCD trace file '" << output_trace_vcd << "'" << std::endl;
  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file(output_trace_vcd.c_str());

  top top("top",input_image_bmp);
  trace_hierarchy(&top, trace_file_ptr);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  sc_start();
  int errcnt = sc_report_handler::get_count(SC_ERROR);
  if (errcnt > 0) {
    std::cout << "Simulation FAILED";
  } else {
    std::cout << "Simulation PASSED" <<endl << endl;
  }

  return errcnt; // return 0 for passed
}
