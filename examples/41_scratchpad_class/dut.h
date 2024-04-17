// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

#undef CONNECTIONS_SIM_ONLY_ASSERT_MSG
#undef CONNECTIONS_ASSERT_MSG

#include "ScratchpadClass.h"

#include "auto_gen_fields.h"


// local Scratchpad "traits" class sets all of its characteristics
// Here the 
//     word_type is uint32
//     number of banks is 16
//     total capacity in words is 16 * 0x1000
typedef ScratchpadTraits<uint32, 16, 16 * 0x1000> local_mem;

// type declaration for input request to DUT
struct dut_in_t {
  local_mem::addr_t addr;
  local_mem::word_type data[local_mem::num_inputs];
  bool is_load{0};

  AUTO_GEN_FIELD_METHODS(dut_in_t, ( \
     addr \
   , data \
   , is_load \
  ) )
};


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::In <dut_in_t> CCS_INIT_S1(in1);
  Connections::Out<local_mem::word_type> CCS_INIT_S1(out1);

  local_mem::mem_class_t scratchpad1{"mem_prehls"};

  SC_CTOR(dut) {
    SC_THREAD(run);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void run() {
    in1.Reset();
    out1.Reset();
    wait();

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      // get the input request from the testbench
      dut_in_t req1 = in1.Pop();

      local_mem::scratchpad_req_t sp_req;  // local scratchpad request type

      // copy incoming request to scratchpad request
#pragma hls_unroll yes
      for (int i=0 ; i < local_mem::num_inputs; i++)
        sp_req.set(i, req1.addr + i, req1.data[i]); 

      if (req1.is_load)
      {
        // if it is a load (i.e. read) operation, get the read data from the RAM
        local_mem::base_rsp_t rsp = scratchpad1.load(sp_req);

        // compute MAC
        local_mem::word_type sum=0;
#pragma hls_unroll yes
        for (int i=0; i < local_mem::num_inputs; i++) {
          sum += rsp.data[i] * req1.data[i];
        }

        // Push out the sum
        out1.Push(sum);
      }
      else
      {
        // if it is a store (i.e. write) operation, write the data to the RAM
        scratchpad1.store(sp_req);
      }
    }
  }
};
