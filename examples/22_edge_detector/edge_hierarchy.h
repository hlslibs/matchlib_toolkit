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

#include <mc_connections.h>
#include <mc_fifo.h>
#include "types.h"

#pragma hls_design top
class EdgeHierarchy : public sc_module
{
public: // Interface
  sc_in<bool>                      CCS_INIT_S1(clk);
  sc_in<bool>                      CCS_INIT_S1(rst_bar);
  Connections::In<uint8>           CCS_INIT_S1(dat_in);
  Connections::Out<uint9>          CCS_INIT_S1(magn);
  Connections::Out<ac_fixed<8,3> > CCS_INIT_S1(angle);

  SC_CTOR(EdgeHierarchy) {
    SC_THREAD(VerticalGradient);
    sensitive << clk.pos();
    reset_signal_is(rst_bar,false);

    SC_THREAD(HorizontalGradient);
    sensitive << clk.pos();
    reset_signal_is(rst_bar,false);

    SC_THREAD(MagnitudeAngle);
    sensitive << clk.pos();
    reset_signal_is(rst_bar,false);

    dy_chan.clk(clk);
    dy_chan.rst_bar(rst_bar);
    dy_chan.in1(dy_chan_in);
    dy_chan.out1(dy_chan_out);
  }

  void VerticalGradient() {
    // Line buffers store pixel line history
    // Mapped to RAM
    uint16 line_buf0[1296/2];
    uint16 line_buf1[1296/2];
    uint16 rdbuf0_pix,rdbuf1_pix;
    uint16 wrbuf0_pix,wrbuf1_pix;
    uint8 pix0,pix1, pix2;
    int9 pix;
    dat_in.Reset();
    dat.ResetWrite();
    dy_chan_in.ResetWrite();
    wait();                                 // WAIT
    while (true) {
      VROW:for (int i = 0; i < 864+1; i++) { // One extra iteration to ramp-up window
        VCOL:for (int j = 0; j < 1296; j++) {
          if ((i <= 863)&(j <= 1295)) { pix0 = dat_in.Pop(); } // Read streaming interface
          if ((j&1)==0) {
            wrbuf0_pix.set_slc(0,pix0);
          } else {
            wrbuf0_pix.set_slc(8,pix0);
          }
          if ((j&1)==0) {
            // Vertical window of pixels
            rdbuf1_pix = line_buf1[j/2];
            rdbuf0_pix = line_buf0[j/2];
          } else {
            line_buf1[j/2] = rdbuf0_pix;// copy previous line
            line_buf0[j/2] = wrbuf0_pix;// store current line
          }
          pix2 = ((j&1)==0) ? rdbuf1_pix.slc<8>(0) : rdbuf1_pix.slc<8>(8);
          pix1 = ((j&1)==0) ? rdbuf0_pix.slc<8>(0) : rdbuf0_pix.slc<8>(8);

          if (i == 1) { pix2 = pix1; } // top boundary
          if (i == 864) { pix0 = pix1; } // bottom boundary

          // Calculate gradient
          pix = pix2*kernel[0] + pix1*kernel[1] + pix0*kernel[2];

          if (i != 0) { // Write streaming interfaces
            dat.Push(pix1);// Original data
            dy_chan_in.Push(pix);
          }
          wait();                           // WAIT
        }
      }
    }
  }

  // Channelized interfaces to stream data in/out
  void HorizontalGradient() {
    // pixel buffers store pixel history
    uint8 pix_buf1, pix_buf0;
    uint8 pix0=0,pix1=0,pix2=0;
    int9 pix;
    dat.ResetRead();
    dx_chan.ResetWrite();
    wait();                                 // WAIT
    while (true) {
      VROW:for (int i = 0; i < 864; i++) { // One extra iteration to ramp-up window
        VCOL:for (int j = 0; j < 1296+1; j++) {
          pix2 = pix_buf1;
          pix1 = pix_buf0;
          if (j <= 1295) { pix0 = dat.Pop(); } // Read streaming interface
          if (j == 1) { pix2 = pix1; } // left boundary
          if (j == 1296) { pix0 = pix1; } // right boundary

          pix_buf1 = pix_buf0;
          pix_buf0 = pix0;
          // Calculate gradient
          pix = pix2*kernel[0] + pix1*kernel[1] + pix0*kernel[2];

          if (j != 0 ) { // Write streaming interface
            dx_chan.Push(pix);// Original data
          }
          wait();                           // WAIT
        }
      }
    }
  }

  // Channelized interfaces to stream data in/out
  void MagnitudeAngle() {
    int9 dx,dy;
    uint18 dx_sq;
    uint18 dy_sq;
    ac_fixed<19,19,false> sum;
    ac_fixed<16,9,false> sq_rt;
    ac_fixed<8,3>at;
    uint9 magn_i;

    dx_chan.ResetRead();
    dy_chan_out.ResetRead();
    magn.Reset();
    angle.Reset();

    wait();                                 // WAIT
    while (true) {
      VROW:for (int i = 0; i < 864; i++) { // One extra iteration to ramp-up window
        VCOL:for (int j = 0; j < 1296; j++) {
          dy = dy_chan_out.Pop();
          dx = dx_chan.Pop();
          dx_sq = dx * dx;
          dy_sq = dy * dy;
          sum = dx_sq + dy_sq;
          // Catapult's math library implementation of piecewise linear sqrt and atan2
          ac_sqrt_pwl(sum,sq_rt);
          // sqrt(sum,sq_rt);
          ac_atan2_cordic((ac_fixed<9,9>)dy,(ac_fixed<9,9>)dx,at);
          //atan2((ac_fixed<9,9>)dy,(ac_fixed<9,9>)dx,at);
          magn_i = sq_rt.to_uint();

          magn.Push(magn_i);
          angle.Push(at);

          wait();                           // WAIT
        }
      }
    }
  }

private:
  // interconnect channels  between blocks
  Connections::FifoModule<int9,3>   CCS_INIT_S1(dy_chan); // need fifo to balance latency
  Connections::Combinational<int9>  CCS_INIT_S1(dy_chan_in);
  Connections::Combinational<int9>  CCS_INIT_S1(dy_chan_out);
  Connections::Combinational<int9>  dx_chan;
  Connections::Combinational<uint8> dat;
};

