// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

namespace Connections
{
  template <class T, int N>
  class FifoModule: public sc_module
  {
  public:
    sc_in<bool> CCS_INIT_S1(clk);
    sc_in<bool> CCS_INIT_S1(rst_bar);

    Connections::In <T> CCS_INIT_S1(in1);
    Connections::Out<T> CCS_INIT_S1(out1);

    #ifdef CONNECTIONS_FAST_SIM
    tlm::tlm_fifo<T> fifo1;
    SC_CTOR(FifoModule)
      : fifo1(N) {
      SC_THREAD(t1);
      //async_reset_signal_is(rst_bar, false);
      SC_THREAD(t2);
      //async_reset_signal_is(rst_bar, false);
    }

    void t1() {
      while (1) {
        fifo1.put(in1.Pop());
      }
    }

    void t2() {
      while (1) {
        out1.Push(fifo1.get());
      }
    }

    #else
    Connections::Fifo<T, N> CCS_INIT_S1(fifo1);

    SC_CTOR(FifoModule) {
      fifo1.clk(clk);
      fifo1.rst(rst_bar);
      fifo1.enq(in1);
      fifo1.deq(out1);

      /*
      #ifdef CONNECTIONS_SIM_ONLY
          fifo1.enq.disable_spawn();
          fifo1.deq.disable_spawn();
      #endif
      */
    }
    #endif
  };
}

