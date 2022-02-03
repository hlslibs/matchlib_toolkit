/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Wed Feb  2 17:40:47 PST 2022                        *
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

#pragma once

#include <ac_sysc_macros.h>
#include <mc_connections.h>

class flop : public sc_module
{
public:
  sc_in<bool>      CCS_INIT_S1(clk);
  sc_in<bool>      CCS_INIT_S1(rst_bar);
  sc_in<uint32_t>  CCS_INIT_S1(in1);
  sc_out<uint32_t> CCS_INIT_S1(out1);

  SC_CTOR(flop) {
    SC_THREAD(process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void process() {
    // this is the reset state:
    out1 = 0;
    wait();                                 // WAIT
    // this is the non-reset state:
    while (1) {
      out1 = in1.read();
      wait();                              // WAIT
    }
  }
};

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool>      CCS_INIT_S1(clk);
  sc_in<bool>      CCS_INIT_S1(rst_bar);
  sc_in<uint32_t>  CCS_INIT_S1(in1);
  sc_out<uint32_t> CCS_INIT_S1(out1);

  SC_CTOR(dut) {
    const int len = 5;
    flop* flop_array[len];
    sc_signal<uint32_t>* sig_array[len];

    for (int i=0; i < len; i++) {
      flop_array[i] = new flop("");
      sig_array[i] = new sc_signal<uint32_t>("");

      flop_array[i]->clk(clk);
      flop_array[i]->rst_bar(rst_bar);
      if (i == 0) {
        flop_array[i]->in1(in1);
      } else {
        flop_array[i]->in1(*sig_array[i-1]);
      }

      if (i == (len-1)) {
        flop_array[i]->out1(out1);
      } else {
        flop_array[i]->out1(*sig_array[i]);
      }
    }
  }
};

