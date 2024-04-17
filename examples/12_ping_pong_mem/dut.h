// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

#include <ac_shared_array_1D.h>
#include <extended_array.h>

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
      sync1.sync_in();
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
      for (int i=0; i < 8; i++) {
        out1.Push(mem[i + (8 * ping_pong)]);
      }
      ping_pong = !ping_pong;
    }
  }

  private:
  Connections::SyncChannel CCS_INIT_S1(sync1); // memory synchronization between threads
#ifdef USE_EXT_ARRAY
  extended_array<ac_int<16>,128> mem{"mem_prehls"};
#else
  ac_shared_array_1D<ac_int<16>, 128> mem;
#endif
};

