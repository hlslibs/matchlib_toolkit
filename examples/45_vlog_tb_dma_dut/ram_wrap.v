// INSERT_EULA_COPYRIGHT: 2020


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
