/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 28 15:18:06 PST 2022                        *
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

#include <ac_int.h>
#include <ac_fixed.h>
#include <mc_connections.h>
#include "RAM_1R1W.h"

SC_MODULE(dct)
{
  public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rstn);

  Connections::In<ac_int<8> > CCS_INIT_S1(input);
  RAM_1R1W_model<>::wr1_port<ac_int<16>,128> CCS_INIT_S1(output); // Ping-pong output memory
  Connections::SyncOut CCS_INIT_S1(sync_out);

  SC_CTOR(dct) {
    SC_THREAD(dct_h);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
    SC_THREAD(dct_v);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);

    // Instantiated memories must bind clocks
    mem.CK(clk);
  }

  void dct_h();
  void dct_v();

  private:
  Connections::SyncChannel CCS_INIT_S1(valid); // memory synchronization between threads
  RAM_1R1W_model<>::mem<ac_int<16>,128> CCS_INIT_S1(mem); // Ping-pong shared memory
};
