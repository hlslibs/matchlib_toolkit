// INSERT_EULA_COPYRIGHT: 2020

#pragma once


#include <mc_connections.h>


#undef CONNECTIONS_SIM_ONLY_ASSERT_MSG
#undef CONNECTIONS_ASSERT_MSG

#include "ScratchpadClass.h"


// local Scratchpad "traits" class sets all of its characteristics
// Here the 
//     word_type is uint32
//     number of banks is 16
//     total capacity in words is 16 * 0x1000
typedef ScratchpadTraits<uint32, 16, 16 * 0x1000> local_mem;


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::In <local_mem::base_req_t> CCS_INIT_S1(in1);
  Connections::Out<local_mem::base_rsp_t> CCS_INIT_S1(out1);

  local_mem::mem_module_t CCS_INIT_S1(scratchpad1);

  SC_CTOR(dut) {
    scratchpad1.clk(clk);
    scratchpad1.rst(rst_bar);
    scratchpad1.cli_req(in1);
    scratchpad1.cli_rsp(out1);
  }
};

