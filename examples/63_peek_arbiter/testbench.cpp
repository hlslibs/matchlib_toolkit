// INSERT_EULA_COPYRIGHT: 2020

#include "dut.h"
#include <mc_scverify.h>

class Top : public sc_module
{
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);

  Connections::Combinational<DTYPE>        CCS_INIT_S1(in1_peek);
  Connections::Combinational<DTYPE>        CCS_INIT_S1(in2_peek);
  Connections::Combinational<packet>       CCS_INIT_S1(in1_dat);
  Connections::Combinational<packet>       CCS_INIT_S1(in2_dat);
  Connections::Combinational<packet>       CCS_INIT_S1(out1);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    sc_object_tracer<sc_clock> trace_clk(clk);

    dut1.clk(clk);
    dut1.rst_bar(rst_bar);
    dut1.out1(out1);
    dut1.in1_peek(in1_peek);
    dut1.in2_peek(in2_peek);
    dut1.in1_dat(in1_dat);
    dut1.in2_dat(in2_dat);

    SC_CTHREAD(reset, clk);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(resp);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);
  }


  void stim() {
    CCS_LOG("Stimulus started");
    in1_peek.ResetWrite();
    in2_peek.ResetWrite();
    in1_dat.ResetWrite();
    in2_dat.ResetWrite();
    wait();

    packet pkt;

    for (int i = 0; i < 30; ) {
      wait();
      if (rand() & 1) {
        in1_peek.Push(i);
        for (int z=0; z < packet::SIZE; z++) 
          pkt.data[z] = i;
        in1_dat.Push(pkt);
        ++i;
      }
      if (rand() & 1) {
        in2_peek.Push(i);
        for (int z=0; z < packet::SIZE; z++) 
          pkt.data[z] = i;
        in2_dat.Push(pkt);
        ++i;
      }
    }

    sc_stop();
    wait();
  }

  void resp() {
    out1.ResetRead();
    wait();

    while (1) {
      wait();
      if (rand() & 1) {
        packet pkt = out1.Pop();
        uint32 t = pkt.data[0];
        CCS_LOG("TB resp sees: " << std::hex << t);
      }
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
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file("trace");

  Top top("top");
  trace_hierarchy(&top, trace_file_ptr);
  sc_start();
  if (sc_report_handler::get_count(SC_ERROR) > 0) {
    std::cout << "Simulation FAILED" << std::endl;
    return -1;
  }
  std::cout << "Simulation PASSED" << std::endl;
  return 0;
}

