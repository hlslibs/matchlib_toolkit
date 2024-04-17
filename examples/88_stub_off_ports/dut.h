// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

template <class T>
class copyio : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::Out<T> CCS_INIT_S1(out1);
  Connections::Out<T> CCS_INIT_S1(out1_nc);
  Connections::In <T> CCS_INIT_S1(in1);

  SC_CTOR(copyio) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    out1.Reset();
    out1_nc.Reset();
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

  Connections::Fifo<T, N> CCS_INIT_S1(fifo1);
  Connections::Combinational<T> CCS_INIT_S1(fifo1_in1);
  Connections::Combinational<T> CCS_INIT_S1(fifo1_out1);
  Connections::Combinational<T> CCS_INIT_S1(stub1);
  Connections::Combinational<T> CCS_INIT_S1(stub2);


  SC_CTOR(dut) {
    copy1.clk(clk);
    copy1.rst_bar(rst_bar);
    copy1.in1(in1);
    copy1.out1(fifo1_in1);
    copy1.out1_nc(stub1);

    fifo1.clk(clk);
    fifo1.rst(rst_bar);
    fifo1.enq(fifo1_in1);
    fifo1.deq(fifo1_out1);

    copy2.clk(clk);
    copy2.rst_bar(rst_bar);
    copy2.in1(fifo1_out1);
    copy2.out1(out1);
    copy2.out1_nc(stub2);

#ifndef __SYNTHESIS__
    SC_THREAD(stub_thread);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
#endif
  }

  void stub_thread() {
    stub1.ResetRead();
    stub2.ResetRead();
  }
};

