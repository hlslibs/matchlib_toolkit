// Auto generated on: Tue Oct 10 15:48:02 2023


module ram(
  clk
, rst_bar
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
);
  input [0:0] clk;
  input [0:0] rst_bar;
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
endmodule;
