/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Thu Aug 11 16:24:59 PDT 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.9                                               *
 *                                                                        *
 *  Copyright 2020 Siemens                                                *
 *                                                                        *
 **************************************************************************
 *  Licensed under the Apache License, Version 2.0 (the "License");       *
 *  you may not use this file except in compliance with the License.      * 
 *  You may obtain a copy of the License at                               *
 *                                                                        *
 *      http://www.apache.org/licenses/LICENSE-2.0                        *
 *                                                                        *
 *  Unless required by applicable law or agreed to in writing, software   * 
 *  distributed under the License is distributed on an "AS IS" BASIS,     * 
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or       *
 *  implied.                                                              * 
 *  See the License for the specific language governing permissions and   * 
 *  limitations under the License.                                        *
 **************************************************************************
 *                                                                        *
 *  The most recent version of this package is available at github.       *
 *                                                                        *
 *************************************************************************/

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

    #if !defined(__SYNTHESIS__) && defined(CONNECTIONS_FAST_SIM)
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

