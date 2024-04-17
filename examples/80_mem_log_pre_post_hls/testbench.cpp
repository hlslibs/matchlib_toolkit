// INSERT_EULA_COPYRIGHT: 2020

#include "dut.h"


#ifdef CCS_SYSC
#include "block_data_abs.h"
std::vector<block_data_abs*> block_data_vec;
#include "./mc_transactors.h"
#endif

#include <mc_scverify.h>

ofstream log_stream;

class Top : public sc_module
{
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);
#ifdef CCS_SYSC
  block_data_proxy<dut::elem_type, dut::mem_size> block_data_proxy1{"mem_posthls"};
#endif

  Connections::Combinational<dut::elem_type>        CCS_INIT_S1(data_out);
  Connections::Combinational<dut::elem_type>        CCS_INIT_S1(raddr_in);
  Connections::Combinational<dut::elem_type>        CCS_INIT_S1(waddr_in);
  Connections::Combinational<dut::elem_type>        CCS_INIT_S1(data_in);


  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    sc_object_tracer<sc_clock> trace_clk(clk);

    log_stream.open("resp_log.txt");

    dut1.clk(clk);
    dut1.rst_bar(rst_bar);
    dut1.data_out(data_out);
    dut1.data_in(data_in);
    dut1.raddr_in(raddr_in);
    dut1.waddr_in(waddr_in);

    SC_CTHREAD(reset, clk);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(resp);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

#ifdef CCS_SYSC
    CCS_LOG("block_data_vec size: " << block_data_vec.size() << "\n");
    block_data_proxy1.sync_with_all_block_data();
#endif
  }

  void stim() {
    CCS_LOG("Stimulus started");
    data_in.ResetWrite();
    raddr_in.ResetWrite();
    waddr_in.ResetWrite();

    wait();

    log_stream << "START_OF_SIMULATION\n";

    for (int i=0; i < 40; i++) {
      waddr_in.Push(i);
      data_in.Push(i);

      if (i > 2)  {
        if (i == 10)
          raddr_in.Push(i);  // force an error by making waddr and raddr the same
        else
          raddr_in.Push(i-1);// make sure raddr!=waddr and also only read where already written
      } else {
        raddr_in.Push(100);  // at startup just read a location with a known value
      }
    }

    wait(100);
    sc_stop();
 
    log_stream << "END_OF_SIMULATION\n";
    wait();
  }

  void resp() {
    data_out.ResetRead();
    wait();

    while (1) {
      int t = data_out.Pop();
      log_stream << std::hex << t << "\n";
      CCS_LOG("TB resp sees: " << std::hex << t);
    }
  }

  void reset() {
    rst_bar.write(0);
    wait();
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

