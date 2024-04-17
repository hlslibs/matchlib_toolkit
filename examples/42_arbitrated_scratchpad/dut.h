// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

#undef CONNECTIONS_SIM_ONLY_ASSERT_MSG
#undef CONNECTIONS_ASSERT_MSG

#include "nvhls_array_new.h"

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


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::In <local_mem::req_t> CCS_INIT_S1(in1);
  Connections::Out<local_mem::rsp_t> CCS_INIT_S1(out1);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  local_mem::mem_t mem;

  void main() {
    out1.Reset();
    in1.Reset();

    bool was_consumed[local_mem::num_inputs];
    bool all_consumed = 1;

    wait();

    local_mem::req_t req; 
    local_mem::rsp_t rsp;

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      // if there are remaining requests in some lanes that were not yet consumed
      // then dont Pop a new input request
      if (all_consumed)
        req = in1.Pop();

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
        out1.Push(rsp);

      wait();
    }
  }
};

