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

class block1 : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::Out<uint32_t> CCS_INIT_S1(out1);
  Connections::In <uint32_t> CCS_INIT_S1(in1);

  SC_CTOR(block1) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    out1.Reset();
    in1.Reset();
    wait();
    #pragma hls_pipeline_init_interval 1
    #pragma pipeline_stall_mode flush
    while (1) {
      uint32_t t = in1.Pop();
      out1.Push(t + 0x100);
    }
  }
};

class block2 : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::Out<uint32_t> CCS_INIT_S1(out1);
  Connections::In <uint32_t> CCS_INIT_S1(in1);

  SC_CTOR(block2) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    out1.Reset();
    in1.Reset();
    wait();
    #pragma hls_pipeline_init_interval 1
    #pragma pipeline_stall_mode flush
    while (1) {
      uint32_t t = in1.Pop();
      out1.Push(t + 0x100);
    }
  }
};

class cdc_fifo : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk1);
  sc_in<bool> CCS_INIT_S1(clk2);
  sc_in<bool> CCS_INIT_S1(rst_bar1);
  sc_in<bool> CCS_INIT_S1(rst_bar2);

  Connections::Out<uint32_t> CCS_INIT_S1(out1);
  Connections::In <uint32_t> CCS_INIT_S1(in1);

#ifdef __SYNTHESIS__
  static void hls_blackbox_module(const char *entity="cdc_fifo",
                                  const char *architecture="cdc_fifo",
                                  const char *library="work",
                                  const char *vhdl_files="",
                                  const char *verilog_files="cdc_fifo.v");

  SC_CTOR(cdc_fifo) {}

#else

  tlm::tlm_fifo<uint32_t>    CCS_INIT_S1(fifo1);

  SC_CTOR(cdc_fifo) {
    #ifdef CONNECTIONS_SIM_ONLY
    out1.disable_spawn();
    in1.disable_spawn();
    #endif

    SC_THREAD(thread1);
    sensitive << clk1.pos();
    async_reset_signal_is(rst_bar1, false);

    SC_THREAD(thread2);
    sensitive << clk2.pos();
    async_reset_signal_is(rst_bar2, false);
  }

  void thread1() {
    // in1.Reset();
    wait();
    while (1) {
      in1.rdy = true;
      do {
        wait();
      } while (!in1.vld);
      in1.rdy = false;
      fifo1.put(in1.dat.read().to_uint64());
    }
  }

  void thread2() {
    // out1.Reset();
    wait();
    while (1) {
      uint32_t t = fifo1.get();
      out1.vld = true;
      out1.dat = t;
      do {
        wait();
      } while (!out1.rdy);
      out1.vld = false;
    }
  }

#endif
};

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk1);
  sc_in<bool> CCS_INIT_S1(clk2);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::Out<uint32_t> CCS_INIT_S1(out1);
  Connections::In <uint32_t> CCS_INIT_S1(in1);
  Connections::Combinational<uint32_t> CCS_INIT_S1(chan1);
  Connections::Combinational<uint32_t> CCS_INIT_S1(chan2);

  block1    CCS_INIT_S1(b1);
  block2    CCS_INIT_S1(b2);
  cdc_fifo  CCS_INIT_S1(cdc_fifo1);

  SC_CTOR(dut) {
    b1.clk(clk1);
    b1.rst_bar(rst_bar);
    b1.in1(in1);
    b1.out1(chan1);

    cdc_fifo1.clk1(clk1);
    cdc_fifo1.clk2(clk2);
    cdc_fifo1.rst_bar1(rst_bar);
    cdc_fifo1.rst_bar2(rst_bar);
    cdc_fifo1.in1(chan1);
    cdc_fifo1.out1(chan2);

    b2.clk(clk2);
    b2.rst_bar(rst_bar);
    b2.in1(chan2);
    b2.out1(out1);
  }
};
