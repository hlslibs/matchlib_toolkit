/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Tue Feb  1 16:08:47 PST 2022                        *
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

#include <mc_connections.h>
#include "mc_fifo.h"

template <class T>
class copyio : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::Out<T> CCS_INIT_S1(out1);
  Connections::In <T> CCS_INIT_S1(in1);

  SC_CTOR(copyio) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    out1.Reset();
    in1.Reset();

    wait();                                 // WAIT

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) { out1.Push(in1.Pop()); }
  }
};

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  typedef ac_int<32, false> T;
  static const int N = 8;

  Connections::In <T> CCS_INIT_S1(in1);
  Connections::Out<T> CCS_INIT_S1(out1);

  copyio<T> CCS_INIT_S1(copy1);
  copyio<T> CCS_INIT_S1(copy2);

  Connections::FifoModule<T, N> CCS_INIT_S1(fifo1);
  Connections::Combinational<T> CCS_INIT_S1(fifo1_in1);
  Connections::Combinational<T> CCS_INIT_S1(fifo1_out1);


  SC_CTOR(dut) {
    copy1.clk(clk);
    copy1.rst_bar(rst_bar);
    copy1.in1(in1);
    copy1.out1(fifo1_in1);

    fifo1.clk(clk);
    fifo1.rst_bar(rst_bar);
    fifo1.in1(fifo1_in1);
    fifo1.out1(fifo1_out1);

    copy2.clk(clk);
    copy2.rst_bar(rst_bar);
    copy2.in1(fifo1_out1);
    copy2.out1(out1);
  }
};

