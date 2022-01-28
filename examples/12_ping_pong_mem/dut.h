/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 28 15:07:52 PST 2022                        *
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
#include "RAM_1R1W.h"

SC_MODULE(dut)
{
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::In<ac_int<16> >  CCS_INIT_S1(in1);
  Connections::Out<ac_int<16> > CCS_INIT_S1(out1);

  SC_CTOR(dut) {
    SC_THREAD(thread1);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
    SC_THREAD(thread2);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    //Instantiated memories must bind clocks
    mem.CK(clk);
  }

  void thread1() {
    bool ping_pong = false;
    in1.Reset();
    sync1.reset_sync_out();
    wait();                                 // WAIT
    while (1) {
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
      for (int i=0; i < 8; i++) {
        mem[i + (8 * ping_pong)] = in1.Pop();
      }
      sync1.sync_out();
      ping_pong = !ping_pong;
    }
  }

  void thread2() {
    bool ping_pong = false;
    out1.Reset();
    sync1.reset_sync_in();
    wait();                                 // WAIT

    while (1) {
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
      sync1.sync_in();
      for (int i=0; i < 8; i++) {
        out1.Push(mem[i + (8 * ping_pong)]);
      }
      ping_pong = !ping_pong;
    }
  }

  private:
  Connections::SyncChannel CCS_INIT_S1(sync1); // memory synchronization between threads
  RAM_1R1W_model<>::mem<ac_int<16>,128> CCS_INIT_S1(mem);//Ping-pong shared memory
};

