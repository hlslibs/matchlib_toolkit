// INSERT_EULA_COPYRIGHT: 2020

#include "dut.h"


#include <mc_scverify.h>

ofstream log_stream;

class Top : public sc_module
{
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);

  Connections::Combinational<dut::T>        CCS_INIT_S1(out1);
  Connections::Combinational<dut::T>        CCS_INIT_S1(in1);


  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    sc_object_tracer<sc_clock> trace_clk(clk);

    log_stream.open("resp_log.txt");

    dut1.clk(clk);
    dut1.rst_bar(rst_bar);
    dut1.out1(out1);
    dut1.in1(in1);

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
    in1.ResetWrite();
    wait();

    log_stream << "START_OF_SIMULATION\n";

    for (int i = 0; i < 200; i++) {
      in1.Push(i);
    }

    wait(100);
    sc_stop();
 
    log_stream << "END_OF_SIMULATION\n";
    wait();
  }

  void resp() {
    out1.ResetRead();
    wait();

    while (1) {
      int t = out1.Pop();
      log_stream << std::hex << t << "\n";
      // CCS_LOG("TB resp sees: " << std::hex << t);
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

/*
  channel_logs logs;
  logs.enable("chan_log");
  logs.log_hierarchy(top);
*/

  sc_start();
  if (sc_report_handler::get_count(SC_ERROR) > 0) {
    std::cout << "Simulation FAILED" << std::endl;
    return -1;
  }
  std::cout << "Simulation PASSED" << std::endl;
  return 0;
}

