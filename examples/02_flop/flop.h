// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

#pragma hls_design top
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

