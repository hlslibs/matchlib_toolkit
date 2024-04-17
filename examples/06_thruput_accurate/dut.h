// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::In <sc_uint<32>> CCS_INIT_S1(in1);
  Connections::In <sc_uint<32>> CCS_INIT_S1(in2);
  Connections::In <sc_uint<32>> CCS_INIT_S1(in3);
  Connections::Out<sc_uint<32>> CCS_INIT_S1(out1);
  Connections::Out<sc_uint<32>> CCS_INIT_S1(out2);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  void main() {
    in1.Reset();
    in2.Reset();
    in3.Reset();
    out1.Reset();
    out2.Reset();
    wait();                                 // WAIT
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      uint32_t i1 = in1.Pop();
      uint32_t i2 = in2.Pop();
      uint32_t i3 = in3.Pop();
      out1.Push(i1 + i2);
      out2.Push(i1 + i3);
    }
  }
};

