// INSERT_EULA_COPYRIGHT: 2020

#include "dut.h"
#include <mc_scverify.h>

#define USE_GATED_CLOCK 1
#define SAMPLE_COUNT  300
#define USE_RANDOM 1


class Top : public sc_module
{
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk;
  gated_clock gated_clock1;
  SC_SIG(bool, rst_bar);
  SC_SIG(bool, idle);
  SC_SIG(bool, gated_clock1_clk);

  Connections::Combinational<uint32>        CCS_INIT_S1(out1);
  Connections::Combinational<uint32>        CCS_INIT_S1(in1);
  Connections::Combinational<uint32>        CCS_INIT_S1(in2);

 virtual void start_of_simulation() {
    Connections::get_sim_clk().add_clock_alias(
      clk.posedge_event(), gated_clock1_clk.posedge_event());
  }

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true)
    ,   gated_clock1("gated_clock1", clk.period())
  {
    sc_object_tracer<sc_clock> trace_clk(clk);

    gated_clock1.idle(idle);
    gated_clock1.clk_out(gated_clock1_clk);

#ifdef USE_GATED_CLOCK
    dut1.clk(gated_clock1_clk);
#else
    dut1.clk(clk);
#endif
    dut1.rst_bar(rst_bar);
    dut1.out1(out1);
    dut1.in1(in1);
    dut1.in2(in2);
    dut1.idle(idle);

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
    in2.ResetWrite();
    wait();

    for (int i = 0; i < SAMPLE_COUNT; i++) {
      in1.Push(i);
      in2.Push(i);
      if ((i % 10) == 0)
      {
#ifdef USE_RANDOM
        uint32 t = rand() & 0xf;
        for (int i=0; i < t; i++)
          wait();
#else
        wait(8);
#endif
      }
    }

    sc_stop();
    wait();
  }

  void resp() {
    out1.ResetRead();
    wait();

    while (1) {
      CCS_LOG("TB resp sees: " << std::hex << out1.Pop());
#ifdef USE_RANDOM
      if ((rand() & 0xf) == 0)
        wait();
#endif
    }
  }

  void reset() {
    rst_bar.write(0);
    wait(5);
    rst_bar.write(1);
    wait();
    wait(SAMPLE_COUNT * 8);
    CCS_LOG("stopping due to timeout");
    sc_stop();
  }
};

int sc_main(int argc, char **argv)
{
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file("trace");

  Top top("top");
  trace_hierarchy(&top, trace_file_ptr);

  channel_logs logs;
#ifdef USE_GATED_CLOCK
  logs.enable("gated_clock_log");
#else
  logs.enable("regular_clock_log");
#endif
  logs.log_hierarchy(top);

  sc_start();

  if (sc_report_handler::get_count(SC_ERROR) > 0) {
    std::cout << "Simulation FAILED" << std::endl;
    return -1;
  }
  std::cout << "Simulation PASSED" << std::endl;
  return 0;
}

