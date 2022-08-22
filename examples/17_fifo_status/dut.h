// INSERT_EULA_COPYRIGHT: 2020
#pragma once

#include <ac_sysc_macros.h>
#include <mc_connections.h>
// Prevent redefine warnings from NVHLS
#undef CONNECTIONS_ASSERT_MSG
#undef CONNECTIONS_SIM_ONLY_ASSERT_MSG
#include <nvhls_connections_buffered_ports.h>


namespace Connections {

template <typename Message, unsigned int NumEntries, class STATUS_TYPE>
class BufferStatus : public sc_module {
  SC_HAS_PROCESS(BufferStatus);

 public:
  // Interface
  sc_in_clk clk;
  sc_in<bool> rst;
  sc_out<bool>        CCS_INIT_S1(is_full);
  sc_out<bool>        CCS_INIT_S1(is_empty);
  sc_out<STATUS_TYPE> CCS_INIT_S1(num_filled);
  sc_out<STATUS_TYPE> CCS_INIT_S1(num_free);
  In<Message> enq;
  Out<Message> deq;

  BufferStatus()
      : sc_module(sc_module_name(sc_gen_unique_name("bufferstatus"))),
        clk("clk"),
        rst("rst") {
    Init();
  }

  BufferStatus(sc_module_name name) : sc_module(name), clk("clk"), rst("rst") {
    Init();
  }

 protected:
  typedef bool Bit;
  static const int AddrWidth = nvhls::index_width<NumEntries>::val;
  typedef NVUINTW(AddrWidth) BuffIdx;

  // Internal wires
  sc_signal<Bit> full_next;
  sc_signal<BuffIdx> head_next;
  sc_signal<BuffIdx> tail_next;

  // Internal state
  sc_signal<Bit> full;
  sc_signal<BuffIdx> head;
  sc_signal<BuffIdx> tail;
  StateSignal<Message> buffer[NumEntries];

  // Helper functions
  void Init() {
#ifdef CONNECTIONS_SIM_ONLY
    enq.disable_spawn();
    deq.disable_spawn();
#endif
    
    SC_METHOD(EnqRdy);
    sensitive << full;

    SC_METHOD(DeqVal);
    sensitive << full << head << tail;

    SC_METHOD(DeqMsg);
#ifndef __SYNTHESIS__
    sensitive << deq.rdy << full << head << tail;
#else
    sensitive << tail;
#endif

    SC_METHOD(FilledFree);
    sensitive << head_next << tail_next;

    SC_METHOD(HeadNext);
    sensitive << enq.vld << full << head;

    SC_METHOD(TailNext);
    sensitive << deq.rdy << full << head << tail;

    SC_METHOD(FullNext);
    sensitive << enq.vld << deq.rdy << full << head << tail;

    SC_THREAD(Seq);
    sensitive << clk.pos();
    NVHLS_NEG_RESET_SIGNAL_IS(rst);

    // Needed so that DeqMsg always has a good tail value
    tail.write(0);
  }

  // Combinational logic

  void FilledFree() {
   int diff = head_next.read() - tail_next.read();

   if (diff < 0)
      diff = -diff;

   num_free = NumEntries - diff;
   num_filled = diff;
  }

  // Enqueue ready
  void EnqRdy() {
     enq.rdy.write(!full.read()); 
     is_full = full;
  }

  // Dequeue valid
  void DeqVal() {
    bool empty = (!full.read() && (head.read() == tail.read()));
    deq.vld.write(!empty);
    is_empty = empty;
  }

  // Dequeue messsage
  void DeqMsg() {
#ifndef __SYNTHESIS__
    bool empty = (!full.read() && (head.read() == tail.read()));
    bool do_deq = !empty;
    if (do_deq) {
#endif
      deq.dat.write(buffer[tail.read()].msg.read());
#ifndef __SYNTHESIS__
    } else {
      deq.dat.write(0);
    }
#endif
  }

  // Head next calculations
  void HeadNext() {
    bool do_enq = (enq.vld.read() && !full.read());
    BuffIdx head_inc;
    if ((head.read() + 1) == NumEntries)
      head_inc = 0;
    else
      head_inc = head.read() + 1;

    if (do_enq)
      head_next.write(head_inc);
    else
      head_next.write(head.read());
  }

  // Tail next calculations
  void TailNext() {
    bool empty = (!full.read() && (head.read() == tail.read()));
    bool do_deq = (deq.rdy.read() && !empty);
    BuffIdx tail_inc;
    if ((tail.read() + 1) == NumEntries)
      tail_inc = 0;
    else
      tail_inc = tail.read() + 1;

    if (do_deq)
      tail_next.write(tail_inc);
    else
      tail_next.write(tail.read());
  }

  // Full next calculations
  void FullNext() {
    bool empty = (!full.read() && (head.read() == tail.read()));
    bool do_enq = (enq.vld.read() && !full.read());
    bool do_deq = (deq.rdy.read() && !empty);

    BuffIdx head_inc;
    if ((head.read() + 1) == NumEntries)
      head_inc = 0;
    else
      head_inc = head.read() + 1;

    if (do_enq && !do_deq && (head_inc == tail.read()))
      full_next.write(1);
    else if (do_deq && full.read())
      full_next.write(0);
    else
      full_next.write(full.read());
  }

  // Sequential logic
  void Seq() {
    // Reset state
    full.write(0);
    head.write(0);
    tail.write(0);
#pragma hls_unroll yes
    for (unsigned int i = 0; i < NumEntries; ++i)
      buffer[i].reset_state();

    wait();

    while (1) {
      // Head update
      head.write(head_next);

      // Tail update
      tail.write(tail_next);

      // Full update
      full.write(full_next);

      // Enqueue message
      if (enq.vld.read() && !full.read()) {
        buffer[head.read()].msg.write(enq.dat.read());
      }

      wait();
    }
  }

#ifndef __SYNTHESIS__
 public:
  void line_trace() {
    if (rst.read()) {
      unsigned int width = (Message().length() / 4);
      // Enqueue port
      if (enq.vld.read() && enq.rdy.read()) {
        std::cout << std::hex << std::setw(width) << enq.msg.read();
      } else {
        std::cout << std::setw(width + 1) << " ";
      }

      std::cout << " ( " << full.read() << " ) ";

      // Dequeue port
      if (deq.vld.read() && deq.rdy.read()) {
        std::cout << std::hex << std::setw(width) << deq.msg.read();
      } else {
        std::cout << std::setw(width + 1) << " ";
      }
      std::cout << " | ";
    }
  }
#endif
};

} // namespace Connections


typedef ac_int<16,true> STATUS_TYPE;

template <class T, unsigned N, class STATUS_TYPE = ac_int<16,true>>
class FifoChannelStatus : public sc_module {
public:
  sc_in<bool>         CCS_INIT_S1(clk);
  sc_in<bool>         CCS_INIT_S1(rst_bar);
  sc_out<bool>        CCS_INIT_S1(is_full);
  sc_out<bool>        CCS_INIT_S1(is_empty);
  sc_out<STATUS_TYPE> CCS_INIT_S1(num_filled);
  sc_out<STATUS_TYPE> CCS_INIT_S1(num_free);
  Connections::Out<T, AUTO_PORT> CCS_INIT_S1(out1);
  Connections::In <T, AUTO_PORT> CCS_INIT_S1(in1);

  Connections::BufferStatus<T, N, STATUS_TYPE> fifo1;

  SC_CTOR(FifoChannelStatus) {
    fifo1.clk(clk);
    fifo1.rst(rst_bar);
    fifo1.enq(in1);
    fifo1.deq(out1);
    fifo1.is_full(is_full);
    fifo1.is_empty(is_empty);
    fifo1.num_filled(num_filled);
    fifo1.num_free(num_free);
  }
};

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);
  sc_out<bool>        CCS_INIT_S1(is_full);
  sc_out<bool>        CCS_INIT_S1(is_empty);
  sc_out<STATUS_TYPE> CCS_INIT_S1(num_filled);
  sc_out<STATUS_TYPE> CCS_INIT_S1(num_free);

  Connections::Out<uint32> CCS_INIT_S1(out1);
  Connections::In <uint32> CCS_INIT_S1(in1);
 
  FifoChannelStatus<uint32, 4, STATUS_TYPE> CCS_INIT_S1(fifo_status);

  SC_CTOR(dut) {
    fifo_status.in1(in1);
    fifo_status.out1(out1);
    fifo_status.clk(clk);
    fifo_status.rst_bar(rst_bar);
    fifo_status.is_full(is_full);
    fifo_status.is_empty(is_empty);
    fifo_status.num_filled(num_filled);
    fifo_status.num_free(num_free);
  }

private:
};

