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

#include <mc_connections.h>

#pragma hls_design top
class and_gate : public sc_module
{
public:
  sc_in<bool>  CCS_INIT_S1(in1);
  sc_in<bool>  CCS_INIT_S1(in2);
  sc_out<bool> CCS_INIT_S1(out1);

  SC_CTOR(and_gate) {
    SC_METHOD(run);
    sensitive << in1 << in2;
  }

  void run() {
    out1 = in1.read() & in2.read();
  }
};

