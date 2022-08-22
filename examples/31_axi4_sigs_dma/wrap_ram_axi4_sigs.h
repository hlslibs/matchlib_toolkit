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
#include "ram.h"

// This module wraps the RAM which uses Connections::In/Out interfaces to use signal port
// interfaces where each AXI4 field is a separate port

class wrap_ram_axi4_sigs : public sc_module, public local_axi_sig_cfg
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  axi4_signal_r_slave_ports<> CCS_INIT_S1(r_slave0_ports);
  axi4_signal_w_slave_ports<> CCS_INIT_S1(w_slave0_ports);
  axi4_signal_r_slave_xactor<> CCS_INIT_S1(r_slave0_xactor);
  axi4_signal_w_slave_xactor<> CCS_INIT_S1(w_slave0_xactor);

  ram CCS_INIT_S1(ram1);

  local_axi_sig_cfg::r_chan<> CCS_INIT_S1(ram_slave_r_chan);
  local_axi_sig_cfg::w_chan<> CCS_INIT_S1(ram_slave_w_chan);

  SC_CTOR(wrap_ram_axi4_sigs) {
    ram1.clk(clk);
    ram1.rst_bar(rst_bar);
    ram1.r_slave0(ram_slave_r_chan);
    ram1.w_slave0(ram_slave_w_chan);

    r_slave0_xactor(r_slave0_ports);
    r_slave0_xactor.ar_port(ram_slave_r_chan.ar);
    r_slave0_xactor.r_port(ram_slave_r_chan.r);

    w_slave0_xactor(w_slave0_ports);
    w_slave0_xactor.aw_port(ram_slave_w_chan.aw);
    w_slave0_xactor.w_port(ram_slave_w_chan.w);
    w_slave0_xactor.b_port(ram_slave_w_chan.b);
  }
};
