// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

#include "dat_vld.h"

#pragma hls_design top
class dut : public sc_module
{
public:
  typedef uint32 T;
  sc_in<bool>    CCS_INIT_S1(clk);
  sc_in<bool>    CCS_INIT_S1(rst_bar);

  IN_FROM_DAT(in1, T);
  IN_FROM_VLD(in1);
  OUT_TO_DAT(out1, T);
  OUT_TO_VLD(out1);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    IN_FROM_DAT_VLD_BIND(in1);
    OUT_TO_DAT_VLD_BIND(out1);
  }

private:

  void main() {
    out1_out.ResetWrite();
    in1_in.ResetRead();
    wait(); 
// NOTE: stall_mode must be stall for this example. If it is flush then FSM may apply backpressure,
// which would cause the protocol adaptor to operate incorrectly

#pragma hls_pipeline_init_interval 2
#pragma pipeline_stall_mode stall
    while (1) {
      uint32 x = in1_in.Pop();
      uint32 y = 0;
      y += 3 * x;
      y += 7 * x * x;
      out1_out.Push(y);
    }
  }
};
