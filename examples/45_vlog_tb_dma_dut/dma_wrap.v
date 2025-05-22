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
 *  Copyright  Siemens                                                *
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

// Auto generated on: Tue Oct 10 15:48:02 2023


module dma(
  clk
, rst_bar
, r_master0_ar_rdy
, r_master0_ar_vld
, r_master0_ar_dat
, r_master0_r_rdy
, r_master0_r_vld
, r_master0_r_dat
, w_master0_aw_rdy
, w_master0_aw_vld
, w_master0_aw_dat
, w_master0_w_rdy
, w_master0_w_vld
, w_master0_w_dat
, w_master0_b_rdy
, w_master0_b_vld
, w_master0_b_dat
, r_slave0_ar_rdy
, r_slave0_ar_vld
, r_slave0_ar_dat
, r_slave0_r_rdy
, r_slave0_r_vld
, r_slave0_r_dat
, w_slave0_aw_rdy
, w_slave0_aw_vld
, w_slave0_aw_dat
, w_slave0_w_rdy
, w_slave0_w_vld
, w_slave0_w_dat
, w_slave0_b_rdy
, w_slave0_b_vld
, w_slave0_b_dat
, dma_done_rdy
, dma_done_vld
, dma_done_dat
, dma_dbg_rdy
, dma_dbg_vld
, dma_dbg_dat
);
  input [0:0] clk;
  input [0:0] rst_bar;
  input  r_master0_ar_rdy;
  output r_master0_ar_vld;
  output [43:0] r_master0_ar_dat;
  output r_master0_r_rdy;
  input  r_master0_r_vld;
  input [70:0] r_master0_r_dat;
  input  w_master0_aw_rdy;
  output w_master0_aw_vld;
  output [43:0] w_master0_aw_dat;
  input  w_master0_w_rdy;
  output w_master0_w_vld;
  output [72:0] w_master0_w_dat;
  output w_master0_b_rdy;
  input  w_master0_b_vld;
  input [5:0] w_master0_b_dat;
  output r_slave0_ar_rdy;
  input  r_slave0_ar_vld;
  input [43:0] r_slave0_ar_dat;
  input  r_slave0_r_rdy;
  output r_slave0_r_vld;
  output [70:0] r_slave0_r_dat;
  output w_slave0_aw_rdy;
  input  w_slave0_aw_vld;
  input [43:0] w_slave0_aw_dat;
  output w_slave0_w_rdy;
  input  w_slave0_w_vld;
  input [72:0] w_slave0_w_dat;
  input  w_slave0_b_rdy;
  output w_slave0_b_vld;
  output [5:0] w_slave0_b_dat;
  input  dma_done_rdy;
  output dma_done_vld;
  output [0:0] dma_done_dat;
  input  dma_dbg_rdy;
  output dma_dbg_vld;
  output [31:0] dma_dbg_dat;
endmodule;
