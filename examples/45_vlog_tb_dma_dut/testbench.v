// INSERT_EULA_COPYRIGHT: 2020


`timescale 1ns / 1ps

module testbench ();

  reg clk;
  reg rst_bar;
  wire [43:0] r_master0_ar_dat;
  wire r_master0_ar_vld;
  wire r_master0_ar_rdy;
  wire [70:0] r_master0_r_dat;
  wire r_master0_r_vld;
  wire r_master0_r_rdy;
  wire [43:0] w_master0_aw_dat;
  wire w_master0_aw_vld;
  wire w_master0_aw_rdy;
  wire [72:0] w_master0_w_dat;
  wire w_master0_w_vld;
  wire w_master0_w_rdy;
  wire [5:0] w_master0_b_dat;
  wire w_master0_b_vld;
  wire w_master0_b_rdy;
  wire [43:0] r_slave0_ar_dat;
  wire r_slave0_ar_vld;
  wire r_slave0_ar_rdy;
  wire [70:0] r_slave0_r_dat;
  wire r_slave0_r_vld;
  wire r_slave0_r_rdy;
  reg [43:0] w_slave0_aw_dat;
  reg w_slave0_aw_vld;
  wire w_slave0_aw_rdy;
  reg [72:0] w_slave0_w_dat;
  reg w_slave0_w_vld;
  wire w_slave0_w_rdy;
  wire [5:0] w_slave0_b_dat;
  wire w_slave0_b_vld;
  wire w_slave0_b_rdy;
  wire dma_done_dat;
  wire dma_done_vld;
  wire dma_done_rdy;
  wire [31:0] dma_dbg_dat;
  wire dma_dbg_vld;
  wire dma_dbg_rdy;


  assign dma_dbg_rdy = 1'b1;
  assign dma_done_rdy = 1'b1;
  assign w_slave0_b_rdy = 1'b1;


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
        #clock_period;
        #clock_period;
        w_slave0_aw_dat = 44'h0;
        w_slave0_aw_vld = 1'b1;
          #clock_period;
        while (w_slave0_aw_rdy != 1'b1) begin
          #clock_period;
        end
        w_slave0_aw_vld = 1'b0;

        w_slave0_w_dat = 73'h0010000000000001000;
        w_slave0_w_vld = 1'b1;
          #clock_period;
        while (w_slave0_w_rdy != 1'b1) begin
          #clock_period;
        end
        w_slave0_w_vld = 1'b0;

        w_slave0_aw_dat = 44'h80;
        w_slave0_aw_vld = 1'b1;
          #clock_period;
        while (w_slave0_aw_rdy != 1'b1) begin
          #clock_period;
        end
        w_slave0_aw_vld = 1'b0;

        w_slave0_w_dat = 73'h0010000000000004000;
        w_slave0_w_vld = 1'b1;
          #clock_period;
        while (w_slave0_w_rdy != 1'b1) begin
          #clock_period;
        end
        w_slave0_w_vld = 1'b0;

        w_slave0_aw_dat = 44'h100;
        w_slave0_aw_vld = 1'b1;
          #clock_period;
        while (w_slave0_aw_rdy != 1'b1) begin
          #clock_period;
        end
        w_slave0_aw_vld = 1'b0;

        w_slave0_w_dat = 73'h001000000000000003F;
        w_slave0_w_vld = 1'b1;
          #clock_period;
        while (w_slave0_w_rdy != 1'b1) begin
          #clock_period;
        end
        w_slave0_w_vld = 1'b0;

        w_slave0_aw_dat = 44'h180;
        w_slave0_aw_vld = 1'b1;
          #clock_period;
        while (w_slave0_aw_rdy != 1'b1) begin
          #clock_period;
        end
        w_slave0_aw_vld = 1'b0;

        w_slave0_w_dat = 73'h0010000000000000001;
        w_slave0_w_vld = 1'b1;
          #clock_period;
        while (w_slave0_w_rdy != 1'b1) begin
          #clock_period;
        end
        w_slave0_w_vld = 1'b0;

        while (dma_done_vld != 1'b1) begin
          #clock_period;
        end
          #clock_period;
          #clock_period;
        $stop();

    end



    dma_wrap dma_INST ( 
           .clk(clk)
         , .rst_bar(rst_bar)
         , .r_master0_ar_dat(r_master0_ar_dat)
         , .r_master0_ar_vld(r_master0_ar_vld)
         , .r_master0_ar_rdy(r_master0_ar_rdy)
         , .r_master0_r_dat(r_master0_r_dat)
         , .r_master0_r_vld(r_master0_r_vld)
         , .r_master0_r_rdy(r_master0_r_rdy)
         , .w_master0_aw_dat(w_master0_aw_dat)
         , .w_master0_aw_vld(w_master0_aw_vld)
         , .w_master0_aw_rdy(w_master0_aw_rdy)
         , .w_master0_w_dat(w_master0_w_dat)
         , .w_master0_w_vld(w_master0_w_vld)
         , .w_master0_w_rdy(w_master0_w_rdy)
         , .w_master0_b_dat(w_master0_b_dat)
         , .w_master0_b_vld(w_master0_b_vld)
         , .w_master0_b_rdy(w_master0_b_rdy)
         , .r_slave0_ar_dat(r_slave0_ar_dat)
         , .r_slave0_ar_vld(r_slave0_ar_vld)
         , .r_slave0_ar_rdy(r_slave0_ar_rdy)
         , .r_slave0_r_dat(r_slave0_r_dat)
         , .r_slave0_r_vld(r_slave0_r_vld)
         , .r_slave0_r_rdy(r_slave0_r_rdy)
         , .w_slave0_aw_dat(w_slave0_aw_dat)
         , .w_slave0_aw_vld(w_slave0_aw_vld)
         , .w_slave0_aw_rdy(w_slave0_aw_rdy)
         , .w_slave0_w_dat(w_slave0_w_dat)
         , .w_slave0_w_vld(w_slave0_w_vld)
         , .w_slave0_w_rdy(w_slave0_w_rdy)
         , .w_slave0_b_dat(w_slave0_b_dat)
         , .w_slave0_b_vld(w_slave0_b_vld)
         , .w_slave0_b_rdy(w_slave0_b_rdy)
         , .dma_done_dat(dma_done_dat)
         , .dma_done_vld(dma_done_vld)
         , .dma_done_rdy(dma_done_rdy)
         , .dma_dbg_dat(dma_dbg_dat)
         , .dma_dbg_vld(dma_dbg_vld)
         , .dma_dbg_rdy(dma_dbg_rdy)
      );

    ram_wrap ram_INST ( 
           .clk(clk)
         , .rst_bar(rst_bar)
         , .r_slave0_ar_dat(r_master0_ar_dat)
         , .r_slave0_ar_vld(r_master0_ar_vld)
         , .r_slave0_ar_rdy(r_master0_ar_rdy)
         , .r_slave0_r_dat(r_master0_r_dat)
         , .r_slave0_r_vld(r_master0_r_vld)
         , .r_slave0_r_rdy(r_master0_r_rdy)
         , .w_slave0_aw_dat(w_master0_aw_dat)
         , .w_slave0_aw_vld(w_master0_aw_vld)
         , .w_slave0_aw_rdy(w_master0_aw_rdy)
         , .w_slave0_w_dat(w_master0_w_dat)
         , .w_slave0_w_vld(w_master0_w_vld)
         , .w_slave0_w_rdy(w_master0_w_rdy)
         , .w_slave0_b_dat(w_master0_b_dat)
         , .w_slave0_b_vld(w_master0_b_vld)
         , .w_slave0_b_rdy(w_master0_b_rdy)
      );

endmodule

