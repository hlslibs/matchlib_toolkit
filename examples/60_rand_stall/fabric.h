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

#include "dma.h"
#include "axi/AxiSplitter.h"
#include "axi/AxiArbiter.h"

typedef axi::axi4_segment<axi::cfg::standard> local_axi;

//
//  fabric module
//
#pragma hls_design top
class fabric : public sc_module, public local_axi
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  r_master<> CCS_INIT_S1(r_master0);
  w_master<> CCS_INIT_S1(w_master0);
  r_master<> CCS_INIT_S1(r_master1);
  w_master<> CCS_INIT_S1(w_master1);
  r_slave<>  CCS_INIT_S1(r_slave0);
  w_slave<>  CCS_INIT_S1(w_slave0);
  Connections::Out<bool> CCS_INIT_S1(dma0_done);
  Connections::Out<bool> CCS_INIT_S1(dma1_done);
  Connections::Out<sc_uint<32>> CCS_INIT_S1(dma0_dbg);
  Connections::Out<sc_uint<32>> CCS_INIT_S1(dma1_dbg);

  static const int numAddrBitsToInspect = 20;
  static const int numSlaves = 2;
  sc_signal<NVUINTW(numAddrBitsToInspect)> addrBound[numSlaves][2];
  AxiSplitter<axi::cfg::standard, numSlaves, numAddrBitsToInspect, false, true> CCS_INIT_S1(input_router);
  AxiSplitter<axi::cfg::standard, numSlaves, numAddrBitsToInspect, false, true> CCS_INIT_S1(dma0_router);
  AxiSplitter<axi::cfg::standard, numSlaves, numAddrBitsToInspect, false, true> CCS_INIT_S1(dma1_router);
  AxiArbiter<axi::cfg::standard, 2, 4> CCS_INIT_S1(axi_arbiter0);
  AxiArbiter<axi::cfg::standard, 2, 4> CCS_INIT_S1(axi_arbiter1);

  typename axi::axi4<axi::cfg::standard>::read::template chan<>  CCS_INIT_S1(dma0_r_master0_out);
  typename axi::axi4<axi::cfg::standard>::write::template chan<> CCS_INIT_S1(dma0_w_master0_out);
  typename axi::axi4<axi::cfg::standard>::read::template chan<>  CCS_INIT_S1(dma0_r_slave0_in);
  typename axi::axi4<axi::cfg::standard>::write::template chan<> CCS_INIT_S1(dma0_w_slave0_in);

  typename axi::axi4<axi::cfg::standard>::read::template chan<>  CCS_INIT_S1(dma1_r_master0_out);
  typename axi::axi4<axi::cfg::standard>::write::template chan<> CCS_INIT_S1(dma1_w_master0_out);
  typename axi::axi4<axi::cfg::standard>::read::template chan<>  CCS_INIT_S1(dma1_r_slave0_in);
  typename axi::axi4<axi::cfg::standard>::write::template chan<> CCS_INIT_S1(dma1_w_slave0_in);

  typename axi::axi4<axi::cfg::standard>::read::template chan<>  CCS_INIT_S1(d0_a0_r);
  typename axi::axi4<axi::cfg::standard>::write::template chan<> CCS_INIT_S1(d0_a0_w);
  typename axi::axi4<axi::cfg::standard>::read::template chan<>  CCS_INIT_S1(d0_a1_r);
  typename axi::axi4<axi::cfg::standard>::write::template chan<> CCS_INIT_S1(d0_a1_w);

  typename axi::axi4<axi::cfg::standard>::read::template chan<>  CCS_INIT_S1(d1_a0_r);
  typename axi::axi4<axi::cfg::standard>::write::template chan<> CCS_INIT_S1(d1_a0_w);
  typename axi::axi4<axi::cfg::standard>::read::template chan<>  CCS_INIT_S1(d1_a1_r);
  typename axi::axi4<axi::cfg::standard>::write::template chan<> CCS_INIT_S1(d1_a1_w);

  dma CCS_INIT_S1(dma0);
  dma CCS_INIT_S1(dma1);

  SC_CTOR(fabric) {
    addrBound[0][0] = 0;
    addrBound[0][1] = 0x7ffff;
    addrBound[1][0] = 0x80000;
    addrBound[1][1] = 0xfffff;

    input_router.clk(clk);
    input_router.reset_bar(rst_bar);
    input_router.addrBound[0][0](addrBound[0][0]);
    input_router.addrBound[0][1](addrBound[0][1]);
    input_router.addrBound[1][0](addrBound[1][0]);
    input_router.addrBound[1][1](addrBound[1][1]);

    input_router.axi_rd_m(r_slave0);
    input_router.axi_wr_m(w_slave0);
    input_router.axi_rd_s_ar[0](dma0_r_slave0_in.ar);
    input_router.axi_rd_s_r[0] (dma0_r_slave0_in.r);
    input_router.axi_wr_s_aw[0](dma0_w_slave0_in.aw);
    input_router.axi_wr_s_w[0] (dma0_w_slave0_in.w);
    input_router.axi_wr_s_b[0] (dma0_w_slave0_in.b);
    input_router.axi_rd_s_ar[1](dma1_r_slave0_in.ar);
    input_router.axi_rd_s_r[1] (dma1_r_slave0_in.r);
    input_router.axi_wr_s_aw[1](dma1_w_slave0_in.aw);
    input_router.axi_wr_s_w[1] (dma1_w_slave0_in.w);
    input_router.axi_wr_s_b[1] (dma1_w_slave0_in.b);

    dma0.clk(clk);
    dma0.rst_bar(rst_bar);
    dma0.r_master0(dma0_r_master0_out);
    dma0.w_master0(dma0_w_master0_out);
    dma0.r_slave0(dma0_r_slave0_in);
    dma0.w_slave0(dma0_w_slave0_in);
    dma0.dma_done(dma0_done);
    dma0.dma_dbg(dma0_dbg);

    dma1.clk(clk);
    dma1.rst_bar(rst_bar);
    dma1.r_master0(dma1_r_master0_out);
    dma1.w_master0(dma1_w_master0_out);
    dma1.r_slave0(dma1_r_slave0_in);
    dma1.w_slave0(dma1_w_slave0_in);
    dma1.dma_done(dma1_done);
    dma1.dma_dbg(dma1_dbg);

    dma0_router.clk(clk);
    dma0_router.reset_bar(rst_bar);
    dma0_router.addrBound[0][0](addrBound[0][0]);
    dma0_router.addrBound[0][1](addrBound[0][1]);
    dma0_router.addrBound[1][0](addrBound[1][0]);
    dma0_router.addrBound[1][1](addrBound[1][1]);

    dma0_router.axi_rd_m(dma0_r_master0_out);
    dma0_router.axi_wr_m(dma0_w_master0_out);

    dma0_router.axi_rd_s_ar[0](d0_a0_r.ar);
    dma0_router.axi_rd_s_r[0] (d0_a0_r.r);
    dma0_router.axi_wr_s_aw[0](d0_a0_w.aw);
    dma0_router.axi_wr_s_w[0] (d0_a0_w.w);
    dma0_router.axi_wr_s_b[0] (d0_a0_w.b);

    dma0_router.axi_rd_s_ar[1](d0_a1_r.ar);
    dma0_router.axi_rd_s_r[1] (d0_a1_r.r);
    dma0_router.axi_wr_s_aw[1](d0_a1_w.aw);
    dma0_router.axi_wr_s_w[1] (d0_a1_w.w);
    dma0_router.axi_wr_s_b[1] (d0_a1_w.b);

    dma1_router.clk(clk);
    dma1_router.reset_bar(rst_bar);
    dma1_router.addrBound[0][0](addrBound[0][0]);
    dma1_router.addrBound[0][1](addrBound[0][1]);
    dma1_router.addrBound[1][0](addrBound[1][0]);
    dma1_router.addrBound[1][1](addrBound[1][1]);

    dma1_router.axi_rd_m(dma1_r_master0_out);
    dma1_router.axi_wr_m(dma1_w_master0_out);

    dma1_router.axi_rd_s_ar[0](d1_a0_r.ar);
    dma1_router.axi_rd_s_r[0] (d1_a0_r.r);
    dma1_router.axi_wr_s_aw[0](d1_a0_w.aw);
    dma1_router.axi_wr_s_w[0] (d1_a0_w.w);
    dma1_router.axi_wr_s_b[0] (d1_a0_w.b);

    dma1_router.axi_rd_s_ar[1](d1_a1_r.ar);
    dma1_router.axi_rd_s_r[1] (d1_a1_r.r);
    dma1_router.axi_wr_s_aw[1](d1_a1_w.aw);
    dma1_router.axi_wr_s_w[1] (d1_a1_w.w);
    dma1_router.axi_wr_s_b[1] (d1_a1_w.b);

    axi_arbiter0.clk(clk);
    axi_arbiter0.reset_bar(rst_bar);
    axi_arbiter1.clk(clk);
    axi_arbiter1.reset_bar(rst_bar);

    axi_arbiter0.axi_rd_m_ar[0](d0_a0_r.ar);
    axi_arbiter0.axi_rd_m_r[0] (d0_a0_r.r);
    axi_arbiter0.axi_wr_m_aw[0](d0_a0_w.aw);
    axi_arbiter0.axi_wr_m_w[0] (d0_a0_w.w);
    axi_arbiter0.axi_wr_m_b[0] (d0_a0_w.b);

    axi_arbiter0.axi_rd_m_ar[1](d1_a0_r.ar);
    axi_arbiter0.axi_rd_m_r[1] (d1_a0_r.r);
    axi_arbiter0.axi_wr_m_aw[1](d1_a0_w.aw);
    axi_arbiter0.axi_wr_m_w[1] (d1_a0_w.w);
    axi_arbiter0.axi_wr_m_b[1] (d1_a0_w.b);

    axi_arbiter1.axi_rd_m_ar[0](d0_a1_r.ar);
    axi_arbiter1.axi_rd_m_r[0] (d0_a1_r.r);
    axi_arbiter1.axi_wr_m_aw[0](d0_a1_w.aw);
    axi_arbiter1.axi_wr_m_w[0] (d0_a1_w.w);
    axi_arbiter1.axi_wr_m_b[0] (d0_a1_w.b);

    axi_arbiter1.axi_rd_m_ar[1](d1_a1_r.ar);
    axi_arbiter1.axi_rd_m_r[1] (d1_a1_r.r);
    axi_arbiter1.axi_wr_m_aw[1](d1_a1_w.aw);
    axi_arbiter1.axi_wr_m_w[1] (d1_a1_w.w);
    axi_arbiter1.axi_wr_m_b[1] (d1_a1_w.b);

    axi_arbiter0.axi_rd_s(r_master0);
    axi_arbiter0.axi_wr_s(w_master0);

    axi_arbiter1.axi_rd_s(r_master1);
    axi_arbiter1.axi_wr_s(w_master1);
  }

private:
};

