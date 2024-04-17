// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

class flop : public sc_module
{
public:
  sc_in<bool>      CCS_INIT_S1(clk);
  sc_in<bool>      CCS_INIT_S1(rst_bar);
  sc_in<uint32_t>  CCS_INIT_S1(in1);
  sc_out<uint32_t> CCS_INIT_S1(out1);

  const uint32_t reset_value;

  SC_HAS_PROCESS(flop);
  flop(sc_module_name name, const uint32_t init=0)
    : sc_module(name)
    , reset_value(init) {
    SC_THREAD(process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

#pragma implicit_fsm true
  void process() {
    // this is the reset state:
    out1 = reset_value;
    wait();                                 // WAIT
    // this is the non-reset state:
    while (1) {
      out1 = in1.read();
      wait();                                 // WAIT
    }
  }
};

#pragma hls_design top
class crossed_flops : public sc_module
{
public:
  sc_in<bool>      CCS_INIT_S1(clk);
  sc_in<bool>      CCS_INIT_S1(rst_bar);
  sc_out<uint32_t> CCS_INIT_S1(out0);
  sc_out<uint32_t> CCS_INIT_S1(out1);

  flop CCS_INIT_S1(flop0);
  flop CCS_INIT_S2(flop1, 1);

  SC_CTOR(crossed_flops) {
    flop0.clk(clk);
    flop0.rst_bar(rst_bar);
    flop0.in1(flop1.out1);
    flop0.out1(out0);

    flop1.clk(clk);
    flop1.rst_bar(rst_bar);
    flop1.in1(flop0.out1);
    flop1.out1(out1);
  }
};

