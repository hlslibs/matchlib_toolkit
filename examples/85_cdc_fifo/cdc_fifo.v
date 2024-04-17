// INSERT_EULA_COPYRIGHT: 2020


// This is a simple SIMULATION ONLY model of a clock domain crossing fifo.
// It is expected that a proper silicon target technology specific model for the clock domain crossing
// fifo will be substituted in the final RTL netlist, but will use the same top level interface
// as this model. 

`timescale 1ns / 1ns

module cdc_fifo (clk1, clk2, rst_bar1, rst_bar2, in1_rdy, in1_vld, in1_dat, out1_rdy, out1_vld, out1_dat);
    parameter width=32;
    input  clk1;
    input  clk2;
    input  rst_bar1;
    input  rst_bar2;
    output in1_rdy;
    reg    in1_rdy;
    input  in1_vld;
    input  [width-1:0] in1_dat;
    output out1_vld;
    reg    out1_vld;
    input  out1_rdy;
    output [width-1:0] out1_dat;
    reg    [width-1:0] out1_dat;

    reg in_toggle;
    reg [width-1:0] in_dat;

    reg out_toggle;

    initial
    begin
     in_toggle <= 0;
     # 1;
     while (1) begin
      @( posedge clk1);
      in1_rdy <= 1;
      @( posedge clk1);
      while (in1_vld == 0) begin
        @( posedge clk1);
      end
      in1_rdy <= 0;
      while (in_toggle != out_toggle) begin
        @( posedge clk1);
      end
      in_dat <= in1_dat;
      in_toggle <= !in_toggle;
     end
    end

    initial 
    begin
     out_toggle <= 0;
     out1_vld <= 0;
     out1_dat <= 0;
     # 1;
     while (1) begin
      @( posedge clk2);
      while (in_toggle == out_toggle) begin
        @( posedge clk2);
      end
      out1_vld <= 1;
      out1_dat <= in_dat;
      out_toggle <= !out_toggle;
      @( posedge clk2);
      while (out1_rdy == 0) begin
        @( posedge clk2);
      end
      out1_vld <= 0;
     end
    end
endmodule
