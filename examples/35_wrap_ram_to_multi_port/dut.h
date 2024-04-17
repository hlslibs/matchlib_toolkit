// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>
#include "auto_gen_fields.h"
#include <ac_array_1D.h>

typedef ac_int<32, false> rd_req;
typedef ac_int<32, false> rd_rsp;

struct wr_req {
  ac_int<32, false> addr;
  ac_int<32, false> data;

  AUTO_GEN_FIELD_METHODS(wr_req, ( \
    addr \
  , data \
  ) )
  //
};

typedef bool wr_rsp;

struct rd_wr_req {
  typedef ac_int<2, false> id_type;
  rd_req rd_req1;
  wr_req wr_req1;
  id_type rd_req_id{0};
  id_type wr_req_id{0};
};

static const int mem_rows{16 * 1024};

class mem_wrapper : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::In <rd_req> CCS_INIT_S1(rd_req1);
  Connections::In <rd_req> CCS_INIT_S1(rd_req2);
  Connections::In <wr_req> CCS_INIT_S1(wr_req1);
  Connections::In <wr_req> CCS_INIT_S1(wr_req2);
  Connections::Out<rd_rsp> CCS_INIT_S1(rd_rsp1);
  Connections::Out<rd_rsp> CCS_INIT_S1(rd_rsp2);
  Connections::Out<wr_rsp> CCS_INIT_S1(wr_rsp1);
  Connections::Out<wr_rsp> CCS_INIT_S1(wr_rsp2);

  SC_CTOR(mem_wrapper) {
    SC_THREAD(main_thread);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }


  void main_thread() {
    rd_req1.Reset();
    rd_req2.Reset();
    wr_req1.Reset();
    wr_req2.Reset();

    rd_rsp1.Reset();
    rd_rsp2.Reset();
    wr_rsp1.Reset();
    wr_rsp2.Reset();

    ac_array_1D<ac_int<32,false>, mem_rows> mem;

    wait(); 

    int got_rd_req1 = 0;
    int got_rd_req2 = 0;
    int got_wr_req1 = 0;
    int got_wr_req2 = 0;

    rd_req rd_req_pending1;
    rd_req rd_req_pending2;
    wr_req wr_req_pending1;
    wr_req wr_req_pending2;
    bool rd_priority2 = false;
    bool wr_priority2 = false;
    int got_rd_rsp = 0;
    int got_wr_rsp = 0;

    rd_rsp rd_rsp_pending;
    wr_rsp wr_rsp_pending;


#pragma hls_pipeline_init_interval 1
    while (1) {
      wait();

      // Pop all the requests:

      if (!got_rd_req1)
        if (rd_req1.PopNB(rd_req_pending1))
          got_rd_req1 = 1;

      if (!got_rd_req2)
        if (rd_req2.PopNB(rd_req_pending2))
          got_rd_req2 = 1;

      if (!got_wr_req1)
        if (wr_req1.PopNB(wr_req_pending1))
          got_wr_req1 = 1;

      if (!got_wr_req2)
        if (wr_req2.PopNB(wr_req_pending2))
          got_wr_req2 = 1;

      
      // arbitrate the requests:
      // This is a simplistic round robin arbiter. A cleaner and more scalable
      // (but perhaps slightly harder to understand initially) solution is available
      // in the Matchlib Arbiter.h class.

      int got_rd_req = 0;
      rd_req rd_req_pending;
      if (got_rd_req1 && (!got_rd_req2 || !rd_priority2)) {
         got_rd_req = 1;
         got_rd_req1 = 0;
         rd_req_pending = rd_req_pending1;
         rd_priority2 = true;
      } else if (got_rd_req2) {
         got_rd_req = 2;
         got_rd_req2 = 0;
         rd_req_pending = rd_req_pending2;
         rd_priority2 = false;
      }
  
      int got_wr_req = 0;
      wr_req wr_req_pending;
      if (got_wr_req1 && (!got_wr_req2 || !wr_priority2)) {
         got_wr_req = 1;
         wr_req_pending = wr_req_pending1;
         got_wr_req1 = 0;
         wr_priority2 = true;
      } else if (got_wr_req2) {
         got_wr_req = 2;
         wr_req_pending = wr_req_pending2;
         got_wr_req2 = 0;
         wr_priority2 = false;
      }

      rd_wr_req rd_wr_req1;
      rd_wr_req1.rd_req1 = rd_req_pending;
      rd_wr_req1.rd_req_id = got_rd_req;
      rd_wr_req1.wr_req1 = wr_req_pending;
      rd_wr_req1.wr_req_id = got_wr_req;

      // read and write to the RAM:

      // NOTE: ignore_memory_precedences is set on these write and reads to enable II=1
      if (rd_wr_req1.wr_req_id) {
        // CCS_LOG("got wr req: " << rd_wr_req1.wr_req_id);
        got_wr_rsp = rd_wr_req1.wr_req_id;
        assert(rd_wr_req1.wr_req1.addr < mem_rows);
        mem[rd_wr_req1.wr_req1.addr] = rd_wr_req1.wr_req1.data;
        wr_rsp_pending = 1;
      }

      if (rd_wr_req1.rd_req_id) {
        // CCS_LOG("got rd req: " << rd_wr_req1.rd_req_id);
        got_rd_rsp = rd_wr_req1.rd_req_id;
        assert(rd_wr_req1.rd_req1 < mem_rows);
        rd_rsp_pending = mem[rd_wr_req1.rd_req1];
      }

      // Push all the responses:

      if (got_rd_rsp == 1) {
          rd_rsp1.Push(rd_rsp_pending);
          got_rd_rsp = 0;
      }

      if (got_rd_rsp == 2) {
          rd_rsp2.Push(rd_rsp_pending);
          got_rd_rsp = 0;
      }

      if (got_wr_rsp == 1) {
          wr_rsp1.Push(wr_rsp_pending);
          got_wr_rsp = 0;
      }

      if (got_wr_rsp == 2) {
          wr_rsp2.Push(wr_rsp_pending);
          got_wr_rsp = 0;
      }
    }
  }
};


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  typedef ac_int<32, false> T;

  Connections::In<T> CCS_INIT_S1(in1);
  Connections::Out<T> CCS_INIT_S1(out1);
  Connections::Combinational<rd_req> CCS_INIT_S1(rd_req1);
  Connections::Combinational<rd_rsp> CCS_INIT_S1(rd_rsp1);
  Connections::Combinational<rd_req> CCS_INIT_S1(rd_req2);
  Connections::Combinational<rd_rsp> CCS_INIT_S1(rd_rsp2);
  Connections::Combinational<wr_req> CCS_INIT_S1(wr_req1);
  Connections::Combinational<wr_rsp> CCS_INIT_S1(wr_rsp1);
  Connections::Combinational<wr_req> CCS_INIT_S1(wr_req2);
  Connections::Combinational<wr_rsp> CCS_INIT_S1(wr_rsp2);

  mem_wrapper CCS_INIT_S1(mem_wrapper1);

  SC_CTOR(dut) {
    mem_wrapper1.clk(clk);
    mem_wrapper1.rst_bar(rst_bar);

    mem_wrapper1.rd_req1(rd_req1);
    mem_wrapper1.rd_rsp1(rd_rsp1);
    mem_wrapper1.rd_req2(rd_req2);
    mem_wrapper1.rd_rsp2(rd_rsp2);

    mem_wrapper1.wr_req1(wr_req1);
    mem_wrapper1.wr_rsp1(wr_rsp1);
    mem_wrapper1.wr_req2(wr_req2);
    mem_wrapper1.wr_rsp2(wr_rsp2);

    SC_THREAD(write_req_thread);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(write_rsp_thread);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(read_req_thread);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(read_rsp_thread);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    fifo1.clk(clk);
    fifo1.rst(rst_bar);
    fifo1.enq(fifo1_in1);
    fifo1.deq(fifo1_out1);
  }

  // This fifo is used to insure that mem reads do not occur before corresponding mem write is done.
  Connections::Fifo<bool, 4> CCS_INIT_S1(fifo1);
  Connections::Combinational<bool> CCS_INIT_S1(fifo1_in1);
  Connections::Combinational<bool> CCS_INIT_S1(fifo1_out1);

  void read_req_thread() {
    rd_req1.ResetWrite();
    rd_req2.ResetWrite();
    fifo1_out1.ResetRead();
    wait();

    ac_int<32, false> addr = 0;

#pragma hls_pipeline_init_interval 1
    while (1) {
      bool v = fifo1_out1.Pop();
      rd_req rd = addr;
      if (v) {
       if ((addr & 1) == 0) {
        rd_req1.Push(rd);
       } else {
        rd_req2.Push(rd);
       }
      }
      ++addr;
    }
  }

  void read_rsp_thread() {
    out1.Reset();
    rd_rsp1.ResetRead();
    rd_rsp2.ResetRead();
    wait();

#pragma hls_pipeline_init_interval 2
    while (1) {
      rd_rsp rsp;
      rsp = rd_rsp1.Pop();
      out1.Push(rsp);
      rsp = rd_rsp2.Pop();
      out1.Push(rsp);
    }
  }

  void write_rsp_thread() {
    wr_rsp1.ResetRead();
    wr_rsp2.ResetRead();
    fifo1_in1.ResetWrite();
    wait();

#pragma hls_pipeline_init_interval 2
    while (1) {
      fifo1_in1.Push(wr_rsp1.Pop());
      fifo1_in1.Push(wr_rsp2.Pop());
    }
  }

  void write_req_thread() {
    in1.Reset();
    wr_req1.ResetWrite();
    wr_req2.ResetWrite();
    wait();

    int addr=0;
    wr_req wr;

#pragma hls_pipeline_init_interval 1
    while (1) {
      ac_int<32, false> data = in1.Pop();
      wr.addr = addr;
      wr.data = data + 0x100;
      if ((addr & 1) == 0) {
          wr_req1.Push(wr);
      } else {
          wr_req2.Push(wr);
      }
      ++addr;
    }
  }
};
