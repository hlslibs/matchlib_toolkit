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
