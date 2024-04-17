// INSERT_EULA_COPYRIGHT: 2020

#include <mc_scverify.h>

#include "dut.h"


class Top : public sc_module
{
public:
  CCS_DESIGN(dut)                         CCS_INIT_S1(dut1);   // the DUT
  sc_clock                                clk;
  sc_signal<bool>                         rst_bar;

  Connections::Combinational<mem_req>     CCS_INIT_S1(in1_chan);
  Connections::Combinational<uint16>      CCS_INIT_S1(out1_chan);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {

    dut1.clk(clk);
    dut1.rst_bar(rst_bar);
    dut1.in1(in1_chan);
    dut1.out1(out1_chan);

    SC_CTHREAD(reset, clk);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(resp);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    sc_object_tracer<sc_clock> trace_clk(clk);
  }

  sc_time start_time, end_time;

  unsigned rand_up_to(unsigned n) {
   return rand() & (n-1);
  }

  static constexpr unsigned test_cnt = 1000;
  mem_req writes[test_cnt];
  uint16 ref_mem[DimSize[0]][DimSize[1]];

  void stim() {
    CCS_LOG("Stimulus started");
    in1_chan.ResetWrite();
    wait();

    start_time = sc_time_stamp();

    // First write to the memory in DUT
    for (unsigned i=0; i < test_cnt; i++) {
      mem_req req1;
      req1.is_write = 1;
      req1.index[0] = rand_up_to(DimSize[0]);
      req1.index[1] = rand_up_to(DimSize[1]);
      req1.data  = rand();
      writes[i] = req1;
      ref_mem[req1.index[0]][req1.index[1]] = req1.data;
      in1_chan.Push(req1);
    }

    // Now read the memory in DUT
    for (unsigned i=0; i < test_cnt; i++) {
      mem_req req1 = writes[i];
      req1.is_write = 0;
      in1_chan.Push(req1);
    }

    wait(20, SC_NS);
    sc_stop();
    wait();
  }

  void resp() {
    out1_chan.ResetRead();
    wait();

    for (unsigned i=0; i < test_cnt; i++) {
      uint16 r = out1_chan.Pop();
      if (r != ref_mem[writes[i].index[0]][writes[i].index[1]]) {
        CCS_LOG("ERROR: index: " << i);
        exit(1);
      }

      CCS_LOG("TB response: " << r);
    }
  }

  void reset() {
    rst_bar.write(0);
    wait(5);
    rst_bar.write(1);
    wait();
  }
};

int sc_main(int argc, char **argv)
{
  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file("trace");

  Top* top = new Top("top");
  trace_hierarchy(top, trace_file_ptr);

  sc_start();
  return 0;
}

