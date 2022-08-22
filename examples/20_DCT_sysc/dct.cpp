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

#include "dct.h"

// Templatized to allow different bit widths
// Folded filter because of symmetrical coefficients
template<typename T0, typename T1,typename T2>
T2 mult_add(T0 data[8], ac_int<3,false> i)
{
  T2 acc = 0;
  T1 pa[4];
  const ac_int<10> coeff[8][4] = {
    {362,  362,  362,  362},
    {502,  425,  284,   99},
    {473,  195, -195, -473},
    {425,  -99, -502, -284},
    {362, -362, -362,  362},
    {284, -502,   99,  425},
    {195, -473,  473, -195},
    { 99, -284,  425, -502}
  };

  PRE_ADD:for (int k=0 ; k < 4 ; ++k ) {
    pa[k] = data[k] + ((i&1)? (int)-data[7-k]:(int)data[7-k]);
  }

  MAC:for (int k=0 ; k < 4 ; ++k ) {
    acc += coeff[i][k] * pa[k];
  }
  return acc;
}

void dct::dct_h()
{
  bool pp=false;//Memory ping-pong signal
  ac_int<21> acc0;
  ac_int<8> buf0[8];

  //Reset modular IO
  input.Reset();
  mem.reset_write();
  valid.reset_sync_out();
  wait();

  while (1) {
    ROW0: for (int i=0; i < 8; ++i ) {
      COPY_ROW0: for (int p = 0; p<8; p++) {
        buf0[p] = input.Pop();
      }
      COL0: for (int j=0; j < 8; ++j ) {
        acc0 = mult_add<ac_int<8>,ac_int<9>,ac_int<21> >(buf0,j);
        // Write shared memory in ping-pong fashion
        // pp bit controls ping-ping to lower/upper half of memory
        mem[j*8+i + (pp ? 64 : 0)] = acc0>>5;
      }
    }
    // Send sync signal to dct_v so it knows it can read the shared memory
    valid.sync_out();
    pp = !pp;//Toggle ping-pong
  }
}

void dct::dct_v()
{
  bool pp=false; // Memory ping-pong signal
  ac_int<16> buf1[8];
  ac_int<31> acc1;

  // Reset modular IO
  output.reset();
  mem.reset_read();
  valid.reset_sync_in();
  sync_out.reset_sync_out();
  wait();

  while (1) {
    // Wait for dct_h to indicate that shared memory is avaiable for reading
    valid.sync_in();
    COL1: for (int j=0; j < 8; ++j ) {
      COPY_ROW1: for (int p = 0; p<8; p++) {
        // Read the shared memory in ping-pong fashion
        // pp bit controls ping-ping to lower/upper half of memory
        buf1[p] = mem[j*8 + p + (pp ? 64 : 0)];
      }
      ROW1: for (int i=0 ; i < 8; ++i ) {
        acc1 = mult_add<ac_int<16>,ac_int<17>,ac_int<31> >(buf1,i);
        // Write DCT output memory interface in ping-pong fashion
        output.write((i*8 + j + (pp ? 64 : 0)),   (acc1 >> 15));
      }
    }
    // Send sync signal to DCT interface indicating that the output memory is avaiable for reading
    sync_out.sync_out();
    pp=!pp;//Toggle ping-pong
  }
}

