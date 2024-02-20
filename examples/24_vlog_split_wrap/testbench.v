
`timescale 1ns / 1ps

module testbench ();
  reg clk;
  reg rst_bar;
  reg [15:0] in1_value;
  wire in1_rdy;
  reg in1_vld;
  wire [15:0] out1_value;
  wire out1_rdy;
  wire out1_vld;

  wire  [15:0] out1_sub_array_0_0;
  wire  [15:0] out1_sub_array_0_1;
  wire  [15:0] out1_sub_array_1_0;
  wire  [15:0] out1_sub_array_1_1;
  wire  [15:0] out1_sub_array_2_0;
  wire  [15:0] out1_sub_array_2_1;
  wire  [15:0] out1_sub_array_3_0;
  wire  [15:0] out1_sub_array_3_1;
  wire  [15:0] out1_sub_array_4_0;
  wire  [15:0] out1_sub_array_4_1;
  wire [15:0] in1_sub_array_0_0;
  wire [15:0] in1_sub_array_0_1;
  wire [15:0] in1_sub_array_1_0;
  wire [15:0] in1_sub_array_1_1;
  wire [15:0] in1_sub_array_2_0;
  wire [15:0] in1_sub_array_2_1;
  wire [15:0] in1_sub_array_3_0;
  wire [15:0] in1_sub_array_3_1;
  wire [15:0] in1_sub_array_4_0;
  wire [15:0] in1_sub_array_4_1;

  assign out1_rdy = 1'b1;
  assign in1_sub_array_0_0 = 15'b0;
  assign in1_sub_array_0_1 = 15'b0;
  assign in1_sub_array_1_0 = 15'b0;
  assign in1_sub_array_1_1 = 15'b0;
  assign in1_sub_array_2_0 = 15'b0;
  assign in1_sub_array_2_1 = 15'b0;
  assign in1_sub_array_3_0 = 15'b0;
  assign in1_sub_array_3_1 = 15'b0;
  assign in1_sub_array_4_0 = 15'b0;
  assign in1_sub_array_4_1 = 15'b0;

    parameter clock_period = 10;

    initial begin
        rst_bar <= 1'b0;
        #clock_period
        rst_bar <= 1'b1;
    end

    initial
    begin
        clk <= 1'b1;
    end

    always
    begin : Clock_Generator
        #(clock_period / 2) clk <= ~clk;
    end

    initial begin
        in1_value = 16'h0;
        in1_vld = 1'b0;
        #clock_period;
        #clock_period;
        #clock_period;
        #clock_period;

        in1_value = 16'h0;
        in1_vld = 1'b1;
          #clock_period;
        while (in1_rdy != 1'b1) begin
          #clock_period;
        end
        in1_vld = 1'b0;
        #clock_period;

        in1_value = 16'h1;
        in1_vld = 1'b1;
          #clock_period;
        while (in1_rdy != 1'b1) begin
          #clock_period;
        end
        in1_vld = 1'b0;
        #clock_period;

        in1_value = 16'h2;
        in1_vld = 1'b1;
          #clock_period;
        while (in1_rdy != 1'b1) begin
          #clock_period;
        end
        in1_vld = 1'b0;
        #clock_period;

        #clock_period;
        #clock_period;
        #clock_period;
        #clock_period;
        $stop();
     end



  dut_wrap dut_wrap_inst(
           .clk(clk)
         , .rst_bar(rst_bar)
         , .in1_rdy(in1_rdy)
         , .in1_vld(in1_vld)
         , .in1_value(in1_value)
         , .out1_rdy(out1_rdy)
         , .out1_vld(out1_vld)
         , .out1_value(out1_value)
         , .out1_sub_array_0_0(out1_sub_array_0_0)
         , .out1_sub_array_0_1(out1_sub_array_0_1)
         , .out1_sub_array_1_0(out1_sub_array_1_0)
         , .out1_sub_array_1_1(out1_sub_array_1_1)
         , .out1_sub_array_2_0(out1_sub_array_2_0)
         , .out1_sub_array_2_1(out1_sub_array_2_1)
         , .out1_sub_array_3_0(out1_sub_array_3_0)
         , .out1_sub_array_3_1(out1_sub_array_3_1)
         , .out1_sub_array_4_0(out1_sub_array_4_0)
         , .out1_sub_array_4_1(out1_sub_array_4_1)
         , .in1_sub_array_0_0(in1_sub_array_0_0)
         , .in1_sub_array_0_1(in1_sub_array_0_1)
         , .in1_sub_array_1_0(in1_sub_array_1_0)
         , .in1_sub_array_1_1(in1_sub_array_1_1)
         , .in1_sub_array_2_0(in1_sub_array_2_0)
         , .in1_sub_array_2_1(in1_sub_array_2_1)
         , .in1_sub_array_3_0(in1_sub_array_3_0)
         , .in1_sub_array_3_1(in1_sub_array_3_1)
         , .in1_sub_array_4_0(in1_sub_array_4_0)
         , .in1_sub_array_4_1(in1_sub_array_4_1)
  );
endmodule
