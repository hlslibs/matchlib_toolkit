/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2.3                                                 *
 *                                                                        *
 *  Release Date    : Tue May 13 15:55:46 PDT 2025                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 2.3.0                                               *
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

#include <mc_connections.h>

#pragma hls_design top
class flop : public sc_module
{
public:
  sc_in<bool>      CCS_INIT_S1(clk);
  sc_in<bool>      CCS_INIT_S1(rst_bar);
  sc_in<uint32_t>  CCS_INIT_S1(in1);
  sc_out<uint32_t> CCS_INIT_S1(out1);

  SC_CTOR(flop) {
    SC_THREAD(process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void process() {
    // this is the reset state:
    out1 = 0;
    wait();                                 // WAIT
    // this is the non-reset state:
    while (1) {
      out1 = in1.read();
      wait();                              // WAIT
    }
  }
};

