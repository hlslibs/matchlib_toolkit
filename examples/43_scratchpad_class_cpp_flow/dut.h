/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2.1       *
 *                                                                        *
 *  Release Date    : Mon Jan 15 20:15:38 PST 2024       *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 2.1.1       *
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

#ifdef __SYNTHESIS__
// Force Catapult to see that this is not a SystemC design..
#define sc_module dummy_module
#endif

#include "ac_channel.h"
#include "mc_scverify.h"

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


class dut 
{
public:
  dut(){}

  local_mem::mem_class_t scratchpad1{"mem_prehls"};

#pragma hls_design interface top
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
  void CCS_BLOCK(run)(ac_channel<dut_in_t>& in1, ac_channel<local_mem::word_type>& out1)
  {
      // get the input request from the testbench
      dut_in_t req1 = in1.read();

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
        out1.write(sum);
      }
      else
      {
        // if it is a store (i.e. write) operation, write the data to the RAM
        scratchpad1.store(sp_req);
      }
  }
};
