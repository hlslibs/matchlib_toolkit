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

#include "top.h"  //contains the DUT and testbench

sc_trace_file *trace_file_ptr;

int sc_main (int argc, char *argv[])
{
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input_image_bmp> <output_trace_vcd>" << std::endl;
    std::cerr << "where:  <input_image_bmp> - path to input BMP image file" << std::endl;
    std::cerr << "        <output_trace_vcd> - (optional) path to output VCD trace file (no suffix)" << std::endl;
    return -1;
  }
  std::string input_image_bmp = argv[1];

  if (argc == 3) {
    std::string output_trace_vcd = argv[2];
    std::cout << "Creating VCD trace file '" << output_trace_vcd << "'" << std::endl;
    trace_file_ptr = sc_create_vcd_trace_file(output_trace_vcd.c_str());
  }

  top top_inst("top",input_image_bmp);
  trace_hierarchy(&top_inst, trace_file_ptr);

  // sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_start();

  int errcnt = sc_report_handler::get_count(SC_ERROR);
  if (errcnt > 0) {
    std::cout << "Simulation FAILED\n";
  } else {
    std::cout << "Simulation PASSED\n";
  }

  return errcnt; // return 0 for passed
}
