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

#pragma once

#include "jpeg.h"

SC_MODULE(testbench)
{
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst);

  Connections::Out<ac_int<2, false > > CCS_INIT_S1(blocktype);
  Connections::Out<rgb_t > CCS_INIT_S1(rgbstream);
  Connections::In<codes_t> CCS_INIT_S1(hufstream);

  SC_HAS_PROCESS(testbench);
  testbench(const sc_module_name& name, const std::string &input_image_bmp) :
    d_input_image_bmp(input_image_bmp) {
    SC_THREAD(stimulus);
    sensitive << clk.pos();
    async_reset_signal_is(rst, false);

    SC_THREAD(monitor);
    sensitive << clk.pos();
    async_reset_signal_is(rst, false);
  }

  // Construct a Jpeg output object
  sc_fifo<int> img_dat;
  void monitor();
  void stimulus();
  std::string d_input_image_bmp;
};

