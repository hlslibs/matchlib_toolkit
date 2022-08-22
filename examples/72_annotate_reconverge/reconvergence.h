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

#ifndef _INCLUDED_RECONVERGENCE_H_
#define _INCLUDED_RECONVERGENCE_H_

#include <ac_sysc_macros.h>
#include <mc_connections.h>

class divergence : public sc_module
{
public:
  sc_in<bool>                        CCS_INIT_S1(clk);
  sc_in<bool>                        CCS_INIT_S1(rstn);

  Connections::In <ac_int<16,false>> CCS_INIT_S1(din);
  Connections::Out<ac_int<16,false>> CCS_INIT_S1(dout0);
  Connections::Out<ac_int<16,false>> CCS_INIT_S1(dout1);

  SC_CTOR(divergence) {
    SC_THREAD(run);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
  }

private:
  void run() {
    din.Reset();
    dout0.Reset();
    dout1.Reset();
    wait();
    #pragma hls_pipeline_init_interval 1
    #pragma pipeline_stall_mode flush
    while (1) {
      ac_int<16,false> tmp = din.Pop();
      dout0.Push(tmp);
      dout1.Push(tmp);
    }
  }
};

class block0 : public sc_module
{
public:
  sc_in<bool>                        CCS_INIT_S1(clk);
  sc_in<bool>                        CCS_INIT_S1(rstn);

  Connections::In <ac_int<16,false>> CCS_INIT_S1(din);
  Connections::Out<ac_int<16,false>> CCS_INIT_S1(dout);

  SC_CTOR(block0) {
    SC_THREAD(run);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
  }

private:
  void run() {
    din.Reset();
    dout.Reset();
    wait();
    #pragma hls_pipeline_init_interval 1
    #pragma pipeline_stall_mode flush
    while (1) {
      ac_int<16,false> tmp = din.Pop();
      dout.Push(tmp);
    }
  }
};

class block1 : public sc_module
{
public:
  sc_in<bool>                        CCS_INIT_S1(clk);
  sc_in<bool>                        CCS_INIT_S1(rstn);

  Connections::In <ac_int<16,false>> CCS_INIT_S1(din);
  Connections::Out<ac_int<16,false>> CCS_INIT_S1(dout);

  SC_CTOR(block1) {
    SC_THREAD(run);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
  }

private:
  void run() {
    din.Reset();
    dout.Reset();
    wait();
    #pragma hls_pipeline_init_interval 1
    #pragma pipeline_stall_mode flush
    while (1) {
      ac_int<16,false> tmp = din.Pop();
      dout.Push(tmp);
    }
  }
};

class reconvergence : public sc_module
{
public:
  sc_in<bool>                        CCS_INIT_S1(clk);
  sc_in<bool>                        CCS_INIT_S1(rstn);

  Connections::In <ac_int<16,false>> CCS_INIT_S1(din0);
  Connections::In<ac_int<16,false>>  CCS_INIT_S1(din1);
  Connections::Out<ac_int<16,false>> CCS_INIT_S1(dout);

  SC_CTOR(reconvergence) {
    SC_THREAD(run);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
  }

private:
  void run() {
    din0.Reset();
    din1.Reset();
    dout.Reset();
    wait();
    #pragma hls_pipeline_init_interval 1
    #pragma pipeline_stall_mode flush
    while (1) {
      ac_int<16,false> tmp0 = din0.Pop();
      ac_int<16,false> tmp1 = din1.Pop();
      dout.Push(tmp0 + tmp1);
    }
  }
};

#pragma hls_design top
class top : public sc_module
{
public:
  sc_in<bool>                        CCS_INIT_S1(clk);
  sc_in<bool>                        CCS_INIT_S1(rstn);
  Connections::In <ac_int<16,false>> CCS_INIT_S1(din);
  Connections::Out<ac_int<16,false>> CCS_INIT_S1(dout);

  SC_CTOR(top) {

    divergence_inst.clk(clk);
    divergence_inst.rstn(rstn);
    divergence_inst.din(din);
    divergence_inst.dout0(dout0);
    divergence_inst.dout1(dout1);

    block0_inst.clk(clk);
    block0_inst.rstn(rstn);
    block0_inst.din(dout0);
    block0_inst.dout(bout0);

    block1_inst.clk(clk);
    block1_inst.rstn(rstn);
    block1_inst.din(dout1);
    block1_inst.dout(bout1);

    reconvergence_inst.clk(clk);
    reconvergence_inst.rstn(rstn);
    reconvergence_inst.din0(bout0);
    reconvergence_inst.din1(bout1);
    reconvergence_inst.dout(dout);
  }

private: // Instances and interconnect
  divergence                                   CCS_INIT_S1(divergence_inst);
  block0                                       CCS_INIT_S1(block0_inst);
  block1                                       CCS_INIT_S1(block1_inst);
  reconvergence                                CCS_INIT_S1(reconvergence_inst);
  Connections::Combinational<ac_int<16,false>> CCS_INIT_S1(dout0);
  Connections::Combinational<ac_int<16,false>> CCS_INIT_S1(dout1);
  Connections::Combinational<ac_int<16,false>> CCS_INIT_S1(bout0);
  Connections::Combinational<ac_int<16,false>> CCS_INIT_S1(bout1);
};
#endif

