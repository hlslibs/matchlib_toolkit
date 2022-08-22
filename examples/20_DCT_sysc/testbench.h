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

