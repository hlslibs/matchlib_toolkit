// INSERT_EULA_COPYRIGHT: 2020


#include "and_gate.h"
#include <mc_scverify.h>
using namespace::std;

class Top : public sc_module
{
public:
  CCS_DESIGN(and_gate) CCS_INIT_S1(and_gate1);

  SC_SIG(bool, in1);
  SC_SIG(bool, in2);
  SC_SIG(bool, out1);

  SC_CTOR(Top) {
    and_gate1.in1(in1);
    and_gate1.in2(in2);
    and_gate1.out1(out1);

    SC_THREAD(stim);
  }

  void stim() {
    CCS_LOG("Stimulus started");
    in1 = 0;
    in2 = 0;
    wait(5, SC_NS);
    in1 = 1;
    in2 = 1;
    wait(1, SC_NS);
    in2 = 0;
    wait(1, SC_NS);
    in2 = 1;
    wait(5, SC_NS);
    in1 = 0;
    in2 = 0;
    CCS_LOG("stopping sim at end of stimulus");
    sc_stop();
    wait(5, SC_NS);
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

