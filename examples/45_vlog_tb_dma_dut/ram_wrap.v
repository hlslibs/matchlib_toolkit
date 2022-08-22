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


module ram (
  clk
, rst_bar
, r_slave0_ar_msg
, r_slave0_ar_val
, r_slave0_ar_rdy
, r_slave0_r_msg
, r_slave0_r_val
, r_slave0_r_rdy
, w_slave0_aw_msg
, w_slave0_aw_val
, w_slave0_aw_rdy
, w_slave0_w_msg
, w_slave0_w_val
, w_slave0_w_rdy
, w_slave0_b_msg
, w_slave0_b_val
, w_slave0_b_rdy
);
  input [0:0] clk;
  input [0:0] rst_bar;
  input  [43:0] r_slave0_ar_msg;
  input  r_slave0_ar_val;
  output r_slave0_ar_rdy;
  output [70:0] r_slave0_r_msg;
  output r_slave0_r_val;
  input  r_slave0_r_rdy;
  input  [43:0] w_slave0_aw_msg;
  input  w_slave0_aw_val;
  output w_slave0_aw_rdy;
  input  [72:0] w_slave0_w_msg;
  input  w_slave0_w_val;
  output w_slave0_w_rdy;
  output [5:0] w_slave0_b_msg;
  output w_slave0_b_val;
  input  w_slave0_b_rdy;
endmodule;
