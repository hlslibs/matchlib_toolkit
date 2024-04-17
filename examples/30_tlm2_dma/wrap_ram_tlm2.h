// INSERT_EULA_COPYRIGHT: 2020

#pragma once
#include "ram.h"

class wrap_ram_tlm2 : public sc_module , public local_axi {
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);
  tlm_utils::multi_passthrough_target_socket<wrap_ram_tlm2> tlm2_target;

  ram CCS_INIT_S1(ram1);

  tlm2_target_to_axi4_master<local_axi> CCS_INIT_S1(target_to_master);
  r_chan<> CCS_INIT_S1(ram_slave_r_chan);
  w_chan<> CCS_INIT_S1(ram_slave_w_chan);
  
  SC_CTOR(wrap_ram_tlm2) {
    ram1.clk(clk);
    ram1.rst_bar(rst_bar);
    ram1.r_slave0(ram_slave_r_chan);
    ram1.w_slave0(ram_slave_w_chan);

    target_to_master.clk(clk);
    target_to_master.rst_bar(rst_bar);
    target_to_master.r_master0(ram_slave_r_chan);
    target_to_master.w_master0(ram_slave_w_chan);
    tlm2_target(target_to_master.tlm2_target);
  }
};
