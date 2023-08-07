// INSERT_EULA_COPYRIGHT: 2020
#include "dut.h"
#include <mc_scverify.h>

using namespace::std;

class Top : public sc_module
{
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);

  SC_SIG(packet, in1_data);
  SC_SIG(bool,   in1_vld);
  SC_SIG(packet, out1_data);
  SC_SIG(bool,   out1_vld);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    Connections::set_sim_clk(&clk);
    sc_object_tracer<sc_clock> trace_clk(clk);

    dut1.clk(clk);
    dut1.rst_bar(rst_bar);
    dut1.out1_data(out1_data);
    dut1.out1_vld(out1_vld);
    dut1.in1_data(in1_data);
    dut1.in1_vld(in1_vld);

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
    in1_data = packet();
    in1_vld = false;
    wait();

    packet p;

    for (int i=0; i < packet::data_len; i++) {
      p.data[i] = i;
    }

    for (int i = 0; i < num_packets; i++) {
      p.coeff = i;
      in1_data = p;
      in1_vld = true;
      wait();  // need wait statement for data/valid protocol
    }
  }

  void resp() {
    wait();

    sc_time start_time = sc_time_stamp();

    for (int i = 0; i < 50; i++) {
      do {
        wait();
      } while (!out1_vld);
      packet p = out1_data;
      CCS_LOG("TB resp sees: " << std::hex << p.coeff << " " << p.data[1]);
    }

    sc_time end_time = sc_time_stamp();
    CCS_LOG("Throughput is: " << (end_time - start_time) / num_packets);

    sc_stop();
    wait();
  }

  void reset() {
    rst_bar.write(0);
    wait(5);
    rst_bar.write(1);
    wait();
  }

  const int num_packets = 10;
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

