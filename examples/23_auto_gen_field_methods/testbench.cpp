// INSERT_EULA_COPYRIGHT: 2020

#include "dut.h"
#include <mc_scverify.h>

class Top : public sc_module
{
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);

  Connections::Combinational<transaction_t>        CCS_INIT_S1(out1);
  Connections::Combinational<transaction_t>        CCS_INIT_S1(in1);
  SC_SIG(transaction_t, sig1);
  SC_SIG(Color_t, sig2);
  SC_SIG(Size_t,  sig3);
  SC_SIG(uint16,  sig4);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    sc_object_tracer<sc_clock> trace_clk(clk);

#ifndef CCS_SYSC
    auto_gen_wrapper wrap0("dut");
    dut1.gen_port_info_vec(wrap0.port_info_vec);
    wrap0.gen_wrappers(10, true);

    auto_gen_split_wrap wrap1("dut");
    dut1.gen_port_info_vec(wrap1.port_info_vec);
    wrap1.gen_wrapper();
#endif

    dut1.clk(clk);
    dut1.rst_bar(rst_bar);
    dut1.out1(out1);
    dut1.in1(in1);
    dut1.sig1(sig1);
    dut1.sig2(sig2);
    dut1.sig3(sig3);
    dut1.sig4(sig4);

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

    for (int i = 0; i < 10; i++) {
      transaction_t t;
      t.field1 = i;
      t.field2 = i + 0x100;
      t.field3.field1 = i + 0x10;
      t.field3.field2 = i + 0x20;
      t.field3.enum3 = green;
      for (int d1=0; d1 < 3; d1++)
       for (int d2=0; d2 < 3; d2++)
         t.field3.field4[d1][d2] = (d1 * 3) + d2 + i;
      t.array[0] = i;
      t.array[1] = i;
      t.enum4 = SizeBase::medium;
      t.b = i & 1;
      in1.Push(t);
    }

    sc_stop();
    wait();
  }

  void resp() {
    out1.ResetRead();
    wait();

    while (1) {
      transaction_t t = out1.Pop();
      CCS_LOG("TB resp sees: " << std::hex << t);
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

  channel_logs logs;
  logs.enable("chan_log");
  logs.log_hierarchy(top);

  sc_start();
  if (sc_report_handler::get_count(SC_ERROR) > 0) {
    std::cout << "Simulation FAILED" << std::endl;
    return -1;
  }
  std::cout << "Simulation PASSED" << std::endl;
  return 0;
}

