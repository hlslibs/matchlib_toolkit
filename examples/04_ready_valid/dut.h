/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 28 15:14:12 PST 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.7                                               *
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

#pragma once

#include <mc_connections.h>

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  sc_out<bool> CCS_INIT_S1(in1_rdy);
  sc_in<bool>  CCS_INIT_S1(in1_vld);
  sc_in<sc_uint<32>>  CCS_INIT_S1(in1_data);

  sc_in<bool>  CCS_INIT_S1(out1_rdy);
  sc_out<bool> CCS_INIT_S1(out1_vld);
  sc_out<sc_uint<32>> CCS_INIT_S1(out1_data);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  sc_uint<32> in1_Pop() {
    sc_uint<32> i1;

    in1_rdy = 1;
    do {
      wait();                               // WAIT
    } while (!in1_vld);
    i1 = in1_data;
    in1_rdy = 0;
    return i1;
  }

  void out1_Push(sc_uint<32> o) {
    out1_vld = 1;
    out1_data = o;

    do {
      wait();                               // WAIT
    } while (!out1_rdy);
    out1_vld = 0;
  }

  void main() {
    in1_rdy = 0;
    out1_vld = 0;
    out1_data = 0;
    wait();                                 // WAIT
    while (1) {
      sc_uint<32> i1;
      i1 = in1_Pop();
      out1_Push(i1 + 0x100);
    }
  }
};

