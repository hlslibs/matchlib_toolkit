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
 *  Copyright 2024 Siemens                                                *
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
#include <ac_int.h> 

#pragma hls_design top
class matrixMultiply  : public sc_module {
 public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rstn);

  Connections::In <ac_int<8>> CCS_INIT_S1(A);
  Connections::In <ac_int<8>> CCS_INIT_S1(B);
  Connections::Out<ac_int<8+8+3>> CCS_INIT_S1(C);

  SC_CTOR(matrixMultiply) {
    SC_THREAD(run);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
  }

  void run() {
    A.Reset();
    B.Reset();
    C.Reset();
    ac_int<8> A_row[8];
    ac_int<8> B_transpose[8][8]; 
    wait();
    #pragma hls_pipeline_init_interval 1
    #pragma pipeline_stall_mode flush
    while (1) {
      ac_int<8+8+3> acc = 0;
      TRANSPOSEB_ROW:for (int i=0; i<8; i++) { // Transpose operation must complete first
        TRANSPOSEB_COL:for (int j=0; j<8; j++) {
          B_transpose[j][i] = B.Pop();
        }
      }
      ROW:for (int i = 0; i < 8; i++) {
        CPY_A:for (int c=0; c<8; c++){ //Copy one row from A
          A_row[c] = A.Pop(); 
        }
        COL:for (int j = 0; j < 8; j++) {//Multiply row of A against all cols of B
          acc = 0;
          // Cannot unroll MAC loop since it would result in memory port contention..
          #pragma hls_unroll no
          MAC:for (int k = 0; k < 8; k++) {
            acc += A_row[k] * B_transpose[j][k];
            #ifndef __SYNTHESIS__
            wait();//wait used to simulate not unrolling the loop in hardware
            #endif
          }
          C.Push(acc);
        }
      }
    }
  }
};

