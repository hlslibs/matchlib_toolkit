/*
 * Copyright (c) 2016-2019, NVIDIA CORPORATION.  All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <cstdio>
#include <systemc.h>
#include <nvhls_array_new.h>
#include <nvhls_int.h>
#include <nvhls_types.h>
#include <hls_globals.h>
#include <nvhls_connections.h>
#include <crossbar.h>
#include <extended_array.h>

/**
 * \brief Parameterized banked scratchpad memory 
 * \ingroup Scratchpad 
 *
 * \tparam T                   EntryType 
 * \tparam N                   Number of requests 
 * \tparam CAPACITY_IN_BYTES 
 *
 * \par Overview
 *   -Assumptions:  All N requests are guaranteed conflict-free.
 *  
 *   Ports:
 *     1 input port for requests (load OR store) from client
 *     1 output port for replies (load only) back to client
 *  
 *   Input parameters:
 *     Number of banks (assumed to be same as the number of requests)
 *     Address Width
 *  
 *
 * \par A Simple Example
 * \code
 *      #include <Scratchpad.h>
 *
 *      ...
 *      Connections::In< cli_req_t<data32_t, ADDR_WIDTH,N> > cli_req;
 *      Connections::Out< cli_rsp_t<data32_t, N> > cli_rsp;
 *      Scratchpad<data32_t, SCRATCHPAD_BANKS,SCRATCHPAD_CAPACITY> myscratchpad;

 *      ...
 *        // Connect the DUT
 *        myscratchpad.clk(clk);
 *        myscratchpad.rst(rst);
 *        myscratchpad.cli_req(cli_req);
 *        myscratchpad.cli_rsp(cli_rsp);
 *      ...
 *
 * \endcode
 *
 * \par Usage Guidelines
 *
 * This module sets the stall mode to flush by default to mitigate possible RTL
 * bugs that can occur in the default stall mode. If you are confident that
 * this class of bugs will not occur in your use case, you can change the stall
 * mode via TCL directive:
 *
 * \code
 * directive set /path/to/Scratchpad/run/while -PIPELINE_STALL_MODE stall
 * \endcode
 *
 * This may reduce area/power.
 * \par
 *
 *
 */

#include <Scratchpad/ScratchpadTypes.h>
#include <mem_array.h>

template <typename T, int N, int CAPACITY_IN_BYTES>
class ScratchpadClass  {
 // CAPACITY_IN_BYTES is incorrectly named, it is actually CAPACITY_IN_WORDS
 public:
  ScratchpadClass(std::string _name="", bool _add_time_stamp=0) {
#ifndef __SYNTHESIS__
    if (_name != "") {
     ext_array = new extended_array<T, CAPACITY_IN_BYTES>(_name, _add_time_stamp);
    }
#endif
  }
  ~ScratchpadClass() {
#ifndef __SYNTHESIS__
    delete ext_array;
#endif
  }
  extended_array<T, CAPACITY_IN_BYTES>* ext_array{0};
  static const int ADDR_WIDTH = nvhls::nbits<CAPACITY_IN_BYTES - 1>::val;
  //------------Constants Here---------------------------
  // Derived parameters
  static const int NBANKS_LOG2 = nvhls::nbits<N - 1>::val;

  //------------Local typedefs---------------------------
  typedef NVUINTW(NBANKS_LOG2) bank_sel_t;
  typedef NVUINTW(ADDR_WIDTH - NBANKS_LOG2) bank_addr_t;
  struct bank_req_t {
    bank_addr_t addr;
    T wdata;
  };
  typedef cli_req_t<T, ADDR_WIDTH, N> req_t;
  typedef cli_rsp_t<T, N> rsp_t;

  //------------Local Variables Here---------------------
  mem_array_sep<T, CAPACITY_IN_BYTES, N> banks;
  bank_req_t input_reqs[N];
  bool input_reqs_valid[N];
  bank_req_t bank_reqs[N];
  bool bank_reqs_valid[N];
  T bank_rsps_data[N];
  bool bank_rsps_valid[N];
  bool load_rsps_valid[N];
  cli_rsp_t<T, N> load_rsp;
  bank_sel_t bank_src_lane[N];
  bank_sel_t bank_dst_lane[N];

  void store(req_t req) {
    rsp_t load_rsp;
    req.opcode = STORE;
    load_store(req, load_rsp);
  }

  rsp_t load(req_t req) {
    rsp_t load_rsp;
    req.opcode = LOAD;
    load_store(req, load_rsp);
    return load_rsp;
  }

  void load_store(req_t curr_cli_req, rsp_t& load_rsp) {
      bool is_load;

      is_load = (curr_cli_req.opcode == LOAD);

#ifndef __SYNTHESIS__
      if (ext_array) {
        T tmp;
        for (int i = 0; i < N; i++) {
         if (curr_cli_req.valids[i] == true) {
          if (is_load) {
            tmp = (*ext_array)[curr_cli_req.addr[i]];  // emit the memory log entry 
            volatile int v = tmp;
          }
          else {
            (*ext_array)[curr_cli_req.addr[i]] = curr_cli_req.data[i]; // emit log entry
          }
          }
        }
      }
#endif

// Pre-process the bank requests and compute lane selects from addresses
#pragma hls_unroll yes
      for (int i = 0; i < N; i++) {
        // For each request, figure out the target bank and update its bank_req
        // fields
        bank_sel_t bank_sel;
        bank_sel = nvhls::get_slc<NBANKS_LOG2>(curr_cli_req.addr[i], 0);
        bank_src_lane[bank_sel] = i;

        // Save the lane->bank mapping for the response xbar
        bank_dst_lane[i] = bank_sel;

#ifndef __SYNTHESIS__
        for (int j=0; j < i; j++) {
          if (bank_dst_lane[j] == bank_sel) {
           std::cout << "Conflicting bank requests indexes are: " 
                    << std::dec << i << " " << j << "\n";
          }
          NVHLS_ASSERT_MSG(bank_dst_lane[j] != bank_sel, "conflicting bank requests");
        }
#endif

        // Convert from curr_cli_req to internal format
        input_reqs_valid[i] = (curr_cli_req.valids[i] == true);
        input_reqs[i].addr = nvhls::get_slc<ADDR_WIDTH - NBANKS_LOG2>(
            curr_cli_req.addr[i], NBANKS_LOG2);
        if (!is_load)
          input_reqs[i].wdata = curr_cli_req.data[i];
      }

      // Bank request crossbar
      crossbar<bank_req_t, N, N>(input_reqs, input_reqs_valid, bank_src_lane,
                                 bank_reqs, bank_reqs_valid);

// Loop over scratchpad banks, execute load or store on each bank
#pragma hls_unroll yes
      for (int i = 0; i < N; i++) {
        if ((bank_reqs_valid[i] == true) && is_load) {
          bank_rsps_valid[i] = true;
          bank_rsps_data[i] = banks.read(bank_reqs[i].addr, i);
        } else if ((bank_reqs_valid[i] == true) && !is_load) {
          banks.write(bank_reqs[i].addr, i, bank_reqs[i].wdata);
          bank_rsps_valid[i] = false;
        } else {
          bank_rsps_valid[i] = false;
        }
      }

      // Bank response crossbar
      crossbar<T, N, N>(bank_rsps_data, bank_rsps_valid, bank_dst_lane,
                        load_rsp.data, load_rsps_valid);
      #pragma hls_unroll yes
      for (int i = 0; i < N; i++) {
        load_rsp.valids[i] = load_rsps_valid[i];  // sc_lv to bool conversion
      }
  }
};

template <typename T, int N, int CAPACITY_IN_BYTES>
class Scratchpad : public sc_module {
 public:
  static const int ADDR_WIDTH = nvhls::nbits<CAPACITY_IN_BYTES - 1>::val;
  sc_in_clk clk;
  sc_in<bool> rst;
  Connections::In<cli_req_t<T, ADDR_WIDTH, N> > cli_req;
  Connections::Out<cli_rsp_t<T, N> > cli_rsp;

  //------------Constants Here---------------------------
  // Derived parameters
  static const int NBANKS_LOG2 = nvhls::nbits<N - 1>::val;

  //------------Local typedefs---------------------------
  typedef NVUINTW(NBANKS_LOG2) bank_sel_t;
  typedef NVUINTW(ADDR_WIDTH - NBANKS_LOG2) bank_addr_t;
  struct bank_req_t {
    bank_addr_t addr;
    T wdata;
  };
  typedef cli_req_t<T, ADDR_WIDTH, N> req_t;
  typedef cli_rsp_t<T, N> rsp_t;

  ScratchpadClass<T, N, CAPACITY_IN_BYTES> scratchpad_class;

  //----------- Constructor -----------------------------
  //   -Allocate and declare sub-modules
  //   -Declare all SC_METHODs and SC_THREADs
  SC_HAS_PROCESS(Scratchpad);
  Scratchpad(sc_module_name name_) : sc_module(name_) {
    SC_THREAD(run);
    sensitive << clk.pos();
    NVHLS_NEG_RESET_SIGNAL_IS(rst);
    //DCOUT("Capacity (bytes): " << CAPACITY_IN_BYTES << ", banks: " << N
    //                           << ", addr width: " << ADDR_WIDTH << endl);
  }

  void run() {

    // Reset behavior
    cli_req.Reset();
    cli_rsp.Reset();
    wait();

    #pragma hls_pipeline_init_interval 1
    #pragma pipeline_stall_mode flush
    while (true) {

      bool is_load;
      is_load = false;

      req_t curr_cli_req;
      rsp_t load_rsp;

      // Read client request
      // (Implemented as a blocking read since there's no other work to do if no
      // valid request in the channel)

      curr_cli_req = cli_req.Pop();

      scratchpad_class.load_store(curr_cli_req, load_rsp);
      is_load = (curr_cli_req.opcode == LOAD);

      // Write client responses
      if (is_load) {
        cli_rsp.Push(load_rsp);
      }

      wait();
    }
  }
};


template <typename WORD_TYPE, int NUM_BANKS, int CAPACITY_IN_WORDS>
struct ScratchpadTraits {

typedef WORD_TYPE word_type;
static const int num_banks = NUM_BANKS;
static const int capacity_in_words = CAPACITY_IN_WORDS;

static const int num_inputs = num_banks; // Note: for Scratchpad num_banks must equal num_inputs
static const int words_per_bank = capacity_in_words / num_banks; 


typedef ScratchpadClass<word_type, num_banks, capacity_in_words> mem_class_t;
typedef Scratchpad<word_type, num_banks, capacity_in_words> mem_module_t;

typedef typename mem_class_t::req_t base_req_t;
typedef typename mem_class_t::rsp_t base_rsp_t;

static const int addr_width = mem_class_t::ADDR_WIDTH;
static const int bank_sel_width = mem_class_t::NBANKS_LOG2;
typedef typename mem_class_t::bank_sel_t bank_sel_t;

typedef ac_int<addr_width, false> addr_t;

struct scratchpad_req_t : public base_req_t {
    // Note that it is important to initialize all valids to false since unitialized valids
    // may lead to pre-HLS versus post-HLS simulation mismatches.
    // This initialization should probably be moved into the base class..
    scratchpad_req_t() { 
     for (unsigned i=0; i < num_inputs; i++) 
       this->valids[i] = 0; 
    }

    void set(unsigned idx, addr_t _addr, word_type _data=0) {
      sc_assert(idx < num_inputs);
      this->valids[idx] = 1;
      this->addr[idx] = _addr;
      this->data[idx] = _data;
    }
};

};
