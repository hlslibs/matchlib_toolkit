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
 *  Copyright 2022 Siemens                                                *
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

class wrap_dma_tlm2 : public sc_module, public local_axi
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);
  tlm_utils::multi_passthrough_target_socket<wrap_dma_tlm2>    tlm2_target;
  tlm_utils::multi_passthrough_initiator_socket<wrap_dma_tlm2> tlm2_initiator;
  Connections::Out<bool> CCS_INIT_S1(dma_done);
  Connections::Out<sc_uint<32>> CCS_INIT_S1(dma_dbg);

  dma CCS_INIT_S1(dma1);

  tlm2_target_to_axi4_master<local_axi> CCS_INIT_S1(target_to_master);
  axi4_slave_to_tlm2_initiator<local_axi> CCS_INIT_S1(slave_to_initiator);
  r_chan<> CCS_INIT_S1(dma_slave_r_chan);
  w_chan<> CCS_INIT_S1(dma_slave_w_chan);
  r_chan<> CCS_INIT_S1(dma_master_r_chan);
  w_chan<> CCS_INIT_S1(dma_master_w_chan);

  SC_CTOR(wrap_dma_tlm2) {
    dma1.clk(clk);
    dma1.rst_bar(rst_bar);
    dma1.r_slave0(dma_slave_r_chan);
    dma1.w_slave0(dma_slave_w_chan);
    dma1.r_master0(dma_master_r_chan);
    dma1.w_master0(dma_master_w_chan);
    dma1.dma_done(dma_done);
    dma1.dma_dbg(dma_dbg);

    target_to_master.clk(clk);
    target_to_master.rst_bar(rst_bar);
    target_to_master.r_master0(dma_slave_r_chan);
    target_to_master.w_master0(dma_slave_w_chan);
    tlm2_target(target_to_master.tlm2_target);

    slave_to_initiator.clk(clk);
    slave_to_initiator.rst_bar(rst_bar);
    slave_to_initiator.r_slave0(dma_master_r_chan);
    slave_to_initiator.w_slave0(dma_master_w_chan);
    slave_to_initiator.tlm2_initiator(tlm2_initiator);
  }
};
