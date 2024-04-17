// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#ifdef __SYNTHESIS__
// Force Catapult to see that this is not a SystemC design..
#define sc_module dummy_module
#endif

#include "ac_channel.h"
#include "mc_scverify.h"
#include "nvhls_array_new.h"


#undef CONNECTIONS_SIM_ONLY_ASSERT_MSG
#undef CONNECTIONS_ASSERT_MSG

#define HLS_ALGORITHMICC
#include <ArbitratedScratchpad.h>
#undef HLS_ALGORITHMICC


namespace local_mem {

typedef uint32 word_type;
// with ArbitratedScratchpad, good strategy is to keep num_banks >> num_inputs, since
// bank contention is allowed, but if num_banks >> num_inputs then contention will be
// less likely
static const int num_inputs = 4;
static const int num_banks = 16;
static const int words_per_bank = 0x1000; 
static const int capacity_in_words = num_banks * words_per_bank;
static const int input_queue_len = 4;

typedef ArbitratedScratchpad<word_type, capacity_in_words, num_inputs, num_banks, input_queue_len> mem_t;

struct req_t : mem_t::req_t {
    // Note that it is important to initialize all valids to false since unitialized valids
    // may lead to pre-HLS versus post-HLS simulation mismatches.
    // This initialization should probably be moved into the base class..
    req_t() { for (unsigned i=0; i < sizeof(data) / sizeof(data[0]); i++) valids[i] = 0; }
};

struct rsp_t : mem_t::rsp_t {};

typedef ac_int<mem_t::addr_width, false> addr_t;

static const int addr_width = mem_t::addr_width;
};


class dut 
{
public:
  dut(){}

  local_mem::mem_t mem;
  bool was_consumed[local_mem::num_inputs];
  bool all_consumed{1};
  local_mem::req_t req; 
  local_mem::rsp_t rsp;

#pragma hls_design interface top
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
  void CCS_BLOCK(run)(ac_channel<local_mem::req_t>& in1, ac_channel<local_mem::rsp_t>& out1)
  {
    // if there are remaining requests in some lanes that were not yet consumed
    // then dont Pop a new input request
    if (all_consumed)
        req = in1.read();

    mem.load_store(req, rsp, was_consumed);

    bool any_valid = 0;
    all_consumed = 1;
#pragma hls_unroll yes
    for (unsigned i=0; i < local_mem::num_inputs; i++) {
        // check if some of the requests were not consumed..
        if (was_consumed[i] == 0)
          all_consumed = 0;

        // if a particular request was consumed, then it is no longer valid
        if (was_consumed[i] == 1)
          req.valids[i] = 0;

        // check if we have any read (aka "load") responses
        if (rsp.valids[i] == 1)
          any_valid = 1;
    }

    if (any_valid)
        out1.write(rsp);
  }
};
