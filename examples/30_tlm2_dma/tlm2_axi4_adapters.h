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

// These adapters convert to/from Matchlib axi4 masters/slaves to TLM2 targets/initiators

#include "axi4_segment.h"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h"

using namespace tlm;

template <class axi_cfg>
class axi4_slave_to_tlm2_initiator
  : public sc_module
  , public axi_cfg
{
public:
  tlm_utils::multi_passthrough_initiator_socket<axi4_slave_to_tlm2_initiator> tlm2_initiator;
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);
  typename axi_cfg::template r_slave<>   CCS_INIT_S1(r_slave0);
  typename axi_cfg::template w_slave<>   CCS_INIT_S1(w_slave0);

  // does not need axi4_segmenter, axi_slave transactions already segmented.
  // need read_buf and write_buf, static size 256 * datawidth as per axi4 protocol.
  // can accept concurrent read and write bursts from axi4_slave, but tlm2 write will
  // not occur until all axi4 write beats are received.
  // not handling any endianness conversion or write byte enables currently (easy to add)
  // not handling any axi4 narrow transfers currently (fairly easy to add)

  ac_int<axi_cfg::bytesPerBeat*8, false> read_buf[256];
  ac_int<axi_cfg::bytesPerBeat*8, false> write_buf[256];

  SC_HAS_PROCESS(axi4_slave_to_tlm2_initiator);

  axi4_slave_to_tlm2_initiator(sc_module_name nm)
    : sc_module(nm) {
    SC_THREAD(slave_r_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(slave_w_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void slave_r_process() {
    r_slave0.reset();
    wait();

    while (1) {
      typename axi_cfg::ar_payload ar;
      r_slave0.start_multi_read(ar);
      uint64_t start_addr = ar.addr;
      uint64_t data_len = (int)(ar.len + 1) * (int)axi_cfg::bytesPerBeat;
      int read_beat{0};

      //CCS_LOG("axi4_slave_to_tlm2 read  addr: " << std::hex << ar.addr << " len: " << ar.len);

      sc_assert((start_addr % axi_cfg::bytesPerBeat) == 0);

      tlm::tlm_generic_payload trans;
      sc_time zero_time(SC_ZERO_TIME);

      trans.set_read();
      trans.set_address(start_addr);
      trans.set_data_ptr((unsigned char *)&read_buf);
      trans.set_data_length(data_len);
      trans.set_byte_enable_ptr(0);
      trans.set_byte_enable_length(0);
      trans.set_streaming_width(data_len);

      tlm2_initiator->b_transport(trans, zero_time);

      bool resp  = (trans.get_response_status() == TLM_OK_RESPONSE) ?
                   axi_cfg::Enc::XRESP::OKAY : axi_cfg::Enc::XRESP::SLVERR;

      while (1) {
        typename axi_cfg::r_payload r;

        r.data = read_buf[read_beat++];
        r.resp = resp;

        if (!r_slave0.next_multi_read(ar, r)) { break; }
      }
    }
  }

  void slave_w_process() {
    w_slave0.reset();

    while (1) {
      typename axi_cfg::aw_payload aw;
      typename axi_cfg::b_payload b;

      w_slave0.start_multi_write(aw, b);

      uint64_t start_addr = aw.addr;
      uint64_t data_len = (int)(aw.len + 1) * (int)axi_cfg::bytesPerBeat;
      int write_beat{0};

      sc_assert((start_addr % axi_cfg::bytesPerBeat) == 0);

      //CCS_LOG("axi4_slave_to_tlm2 write addr: " << std::hex << aw.addr << " len: " << aw.len);

      while (1) {
        typename axi_cfg::w_payload w = w_slave0.w.Pop();
        decltype(w.wstrb) all_on{~0};
        sc_assert(w.wstrb == all_on);
        write_buf[write_beat++] = w.data;

        if (!w_slave0.next_multi_write(aw)) { break; }
      }

      tlm::tlm_generic_payload trans;
      sc_time zero_time(SC_ZERO_TIME);

      trans.set_write();
      trans.set_address(start_addr);
      trans.set_data_ptr((unsigned char *)&write_buf);
      trans.set_data_length(data_len);
      trans.set_byte_enable_ptr(0);
      trans.set_byte_enable_length(0);
      trans.set_streaming_width(data_len);

      tlm2_initiator->b_transport(trans, zero_time);

      b.resp  = (trans.get_response_status() == TLM_OK_RESPONSE) ?
                axi_cfg::Enc::XRESP::OKAY : axi_cfg::Enc::XRESP::SLVERR;

      w_slave0.b.Push(b);
    }
  }
};

template <class axi_cfg>
class tlm2_target_to_axi4_master
  : public sc_module
  , public axi_cfg
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);
  tlm_utils::multi_passthrough_target_socket<tlm2_target_to_axi4_master> tlm2_target;
  typename axi_cfg::template r_master<> CCS_INIT_S1(r_master0);
  typename axi_cfg::template w_master<> CCS_INIT_S1(w_master0);

  // incoming tlm2 transaction may not be segmented according to axi4 protocol,
  // so we do AXI4 segmentation here so master AXI4 interfaces conform to protocol.
  // TODO BELOW: (these mostly error out properly right now)
  // if address or data_len is not aligned to axi4 bus width then probably simplest
  // approach is to allocate new buffers with needed padding, copy data, and emit axi4
  // bursts that are fully aligned and use full buswidth. If needed, use wstrb to precisely
  // control what is written.
  // For read bursts with unaligned addresses or data_lens, do axi4 read bursts with fully
  // aligned addresses using full buswidth then copy data back to tlm2 to reflect selected data.

  tlm_generic_payload *current_trans{0};
  tlm_fifo<bool> start_fifo, done_fifo;

  SC_HAS_PROCESS(tlm2_target_to_axi4_master);

  tlm2_target_to_axi4_master(sc_module_name nm)
    : sc_module(nm) {
    tlm2_target.register_b_transport(this, &tlm2_target_to_axi4_master::b_transport);

    SC_THREAD(master_thread);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    AXI4_W_SEGMENT_BIND(w_segment0, clk, rst_bar, w_master0);
    AXI4_R_SEGMENT_BIND(r_segment0, clk, rst_bar, r_master0);
  }

  AXI4_W_SEGMENT_CFG(axi_cfg, w_segment0)
  AXI4_R_SEGMENT_CFG(axi_cfg, r_segment0)

  virtual void b_transport(int tag, tlm_generic_payload &trans, sc_time &tm) {
    sc_assert(tag == 0);
    current_trans = &trans;
    // separate thread used here is needed for proper handling of reset calls for Matchlib
    // not properly handling "dynamic resets" here, would need to clear the fifos in that case

    start_fifo.put(1);
    (void)done_fifo.get();
  }

  void master_thread() {
    AXI4_W_SEGMENT_RESET(w_segment0, w_master0);
    AXI4_R_SEGMENT_RESET(r_segment0, r_master0);
    wait();

    while (1) {
      start_fifo.get();

      tlm_command cmd = current_trans->get_command();
      uint64_t addr   = current_trans->get_address();
      uint64_t len    = current_trans->get_data_length();
      unsigned char *data_ptr = current_trans->get_data_ptr();
      typedef ac_int<axi_cfg::bytesPerBeat*8, false> beat_t;
      beat_t *beat_ptr = (beat_t *)data_ptr;

      current_trans->set_response_status(TLM_OK_RESPONSE);

      sc_assert((addr % axi_cfg::bytesPerBeat) == 0);

      if (cmd == TLM_WRITE_COMMAND) {
        typename axi_cfg::ex_aw_payload aw;
        typename axi_cfg::b_payload b;
        aw.addr = addr;
        aw.ex_len = (len / axi_cfg::bytesPerBeat) - 1;

        if (((aw.ex_len + 1) * axi_cfg::bytesPerBeat) != len)
        { sc_assert(0); }

        w_segment0_ex_aw_chan.Push(aw);

        do {
          typename axi_cfg::w_payload w;
          w.data = *beat_ptr++;
          w_segment0_w_chan.Push(w);
        } while (aw.ex_len--);

        b = w_segment0_b_chan.Pop();
        if (b.resp != axi_cfg::Enc::XRESP::OKAY)
        { current_trans->set_response_status(TLM_GENERIC_ERROR_RESPONSE); }
      }

      if (cmd == TLM_READ_COMMAND) {
        typename axi_cfg::ex_ar_payload ar;
        ar.addr = addr;
        ar.ex_len = (len / axi_cfg::bytesPerBeat) - 1;

        if (((ar.ex_len + 1) * axi_cfg::bytesPerBeat) != len)
        { sc_assert(0); }

        r_segment0_ex_ar_chan.Push(ar);

        do {
          typename axi_cfg::r_payload r;
          r = r_master0.r.Pop();
          *beat_ptr++ = r.data;
          if (r.resp != axi_cfg::Enc::XRESP::OKAY)
          { current_trans->set_response_status(TLM_GENERIC_ERROR_RESPONSE); }
        } while (ar.ex_len--);
      }

      done_fifo.put(1);
    }
  }
};
