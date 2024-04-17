// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

#include "extended_array.h"


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  typedef ac_int<32, false> elem_type;
  static const int mem_size = 128;

#ifdef USE_EXT_ARRAY
  extended_array<elem_type, mem_size> mem{"mem_prehls"};
#else
  elem_type mem[mem_size];
#endif

  Connections::In <elem_type> CCS_INIT_S1(raddr_in);
  Connections::In <elem_type> CCS_INIT_S1(waddr_in);
  Connections::In <elem_type> CCS_INIT_S1(data_in);
  Connections::Out<elem_type> CCS_INIT_S1(data_out);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    raddr_in.Reset();
    waddr_in.Reset();
    data_in.Reset();
    data_out.Reset();

// Not sure why mem initialization is required for Catapult to synthesize,
// but it makes the memory logs more interesting anyway..
#ifdef __SYNTHESIS__
    for (unsigned u=0; u < mem_size; u++)
      mem[u] = 0;
#endif

//  static bool temp = ac::init_array<AC_VAL_DC>(mem,mem_size);  // does not resolve hls mem init issue
    wait();

// This is here to balance initialization above in mem logs..
#ifndef __SYNTHESIS__
    for (unsigned u=0; u < mem_size; u++)
      mem[u] = 0;
#endif

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      mem[waddr_in.Pop()] = data_in.Pop();
      elem_type t = mem[raddr_in.Pop()];
      data_out.Push(t);
    }
  }
};
