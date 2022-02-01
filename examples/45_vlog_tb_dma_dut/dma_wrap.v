/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Tue Feb  1 15:18:00 PST 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.8                                               *
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


module dma (
  clk
, rst_bar
, r_master0_ar_msg
, r_master0_ar_val
, r_master0_ar_rdy
, r_master0_r_msg
, r_master0_r_val
, r_master0_r_rdy
, w_master0_aw_msg
, w_master0_aw_val
, w_master0_aw_rdy
, w_master0_w_msg
, w_master0_w_val
, w_master0_w_rdy
, w_master0_b_msg
, w_master0_b_val
, w_master0_b_rdy
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
, dma_done_msg
, dma_done_val
, dma_done_rdy
, dma_dbg_msg
, dma_dbg_val
, dma_dbg_rdy
);
  input [0:0] clk;
  input [0:0] rst_bar;
  output [43:0] r_master0_ar_msg;
  output r_master0_ar_val;
  input  r_master0_ar_rdy;
  input  [70:0] r_master0_r_msg;
  input  r_master0_r_val;
  output r_master0_r_rdy;
  output [43:0] w_master0_aw_msg;
  output w_master0_aw_val;
  input  w_master0_aw_rdy;
  output [72:0] w_master0_w_msg;
  output w_master0_w_val;
  input  w_master0_w_rdy;
  input  [5:0] w_master0_b_msg;
  input  w_master0_b_val;
  output w_master0_b_rdy;
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
  output [0:0] dma_done_msg;
  output dma_done_val;
  input  dma_done_rdy;
  output [31:0] dma_dbg_msg;
  output dma_dbg_val;
  input  dma_dbg_rdy;
endmodule;
