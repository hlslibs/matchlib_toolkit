/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 28 15:04:19 PST 2022                        *
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

  typedef sc_uint<32> T;
  static const int N = 8;

  Connections::In <T> CCS_INIT_S1(in1);
  Connections::Out<T> CCS_INIT_S1(out1);

  Connections::FifoModule<T, N> CCS_INIT_S1(fifo1);
  Connections::Combinational<T> CCS_INIT_S1(fifo1_in1);
  Connections::Combinational<T> CCS_INIT_S1(fifo1_out1);

  SC_CTOR(dut) {
    fifo1.clk(clk);
    fifo1.rst_bar(rst_bar);
    fifo1.in1(fifo1_in1);
    fifo1.out1(fifo1_out1);

    SC_THREAD(main1);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(main2);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main1() {
    fifo1_in1.ResetWrite();
    in1.Reset();
    wait();                                 // WAeIT
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) { fifo1_in1.Push(in1.Pop()); }
  }

  void main2() {
    fifo1_out1.ResetRead();
    out1.Reset();
    wait();                                 // WAIT
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) { out1.Push(fifo1_out1.Pop()); }
  }
};

