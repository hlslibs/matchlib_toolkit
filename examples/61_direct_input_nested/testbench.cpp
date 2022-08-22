/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Thu Aug 11 16:24:59 PDT 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.9                                               *
 *                                                                        *
 *  Copyright 2020 Siemens                                                *
 *                                                                        *
 **************************************************************************
 *  Licensed under the Apache License, Version 2.0 (the "License");       *
 *  you may not use this file except in compliance with the License.      * 
 *  You may obtain a copy of the License at                               *
 *                                                                        *
 *      http://www.apache.org/licenses/LICENSE-2.0                        *
 *                                                                        *
 *  Unless required by applicable law or agreed to in writing, software   * 
 *  distributed under the License is distributed on an "AS IS" BASIS,     * 
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or       *
 *  implied.                                                              * 
 *  See the License for the specific language governing permissions and   * 
 *  limitations under the License.                                        *
 **************************************************************************
 *                                                                        *
 *  The most recent version of this package is available at github.       *
 *                                                                        *
 *************************************************************************/

#include "dut.h"
#include <mc_scverify.h>

int retcode = 0;

class testbench : public sc_module
{
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);

  Connections::Combinational<uint32>        CCS_INIT_S1(out1);
  Connections::Combinational<uint32>        sample_in[num_samples];
  Connections::SyncChannel                  CCS_INIT_S1(sync_chan);
  sc_signal<uint32_t>                       direct_inputs[num_direct_inputs];
  ofstream log_stream;

  SC_HAS_PROCESS(testbench);
  testbench(const sc_module_name& name, const std::string &resp_log)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    sc_object_tracer<sc_clock> trace_clk(clk);

    log_stream.open(resp_log);
    dut1.clk(clk);
    dut1.rst_bar(rst_bar);
    dut1.out1(out1);
    dut1.sync_in(sync_chan);

    for (int i=0; i < num_samples; i++) {
      dut1.sample_in[i](sample_in[i]);
    }

    for (int y=0; y < num_direct_inputs; y++) {
      dut1.direct_inputs[y](direct_inputs[y]);
      sc_object_tracer<sc_signal<uint32_t>> trace_sig(direct_inputs[y]);
    }

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
    sync_chan.ResetWrite();

    for (int i=0; i < num_samples; i++) {
      sample_in[i].ResetWrite();
    }

    for (int y=0; y < num_direct_inputs; y++) {
      direct_inputs[y] = y;
    }

    direct_inputs[0] = 5;
    direct_inputs[1] = 5;
    direct_inputs[7] = 4;

    wait();

    // CCS_LOG("START_OF_SIMULATION");
    log_stream << "START_OF_SIMULATION\n";

    for (int i = 0; i < 10; i++) {
      do {
        wait();
      } while (sync_chan.rdy == 0);

      // update dir inputs here
      direct_inputs[2] = 1 + direct_inputs[2];
      direct_inputs[3] = direct_inputs[4];
      direct_inputs[4] = direct_inputs[3];
      direct_inputs[7] = 4 + (i&3);
      sync_chan.vld = 1;

      wait();
      sync_chan.vld = 0;

      if (i == 9) {
        // DUT pipeline should be ramped down at end of sync above, but isn't currently due to catapult bug
        wait(100);
        // CCS_LOG("END_OF_SIMULATION");
        log_stream << "END_OF_SIMULATION\n";
        log_stream.close();
        sc_stop();
        wait();
      }

      for (uint32_t x=0; x < direct_inputs[0]; x++) {
        for (uint32_t y=0; y < direct_inputs[1]; y++) {
          for (uint32_t s=0; s < num_samples; s++) {
            sample_in[s].Push((x*y) + i + s);
          }
        }
      }
    }

    wait(50);

    wait();
  }

  void resp() {
    out1.ResetRead();
    wait();

    while (1) {
      uint32_t t = out1.Pop();
      // std::cout << "TB resp sees: " << std::hex << t  << "\n";
      log_stream << "TB resp sees: " << std::hex << t << "\n";
    }
  }

  void end_of_simulation() {}

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
  std::string trace_file = "trace";
  std::string resp_log = "resp.log";
  std::string run_logs = "run_logs";
  if (argc > 1) { trace_file = argv[1]; }
  if (argc > 2) { resp_log = argv[2]; }
  if (argc > 3) { run_logs = argv[3]; }

  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file(trace_file.c_str());

  testbench top("top", resp_log);
  channel_logs logs;
  logs.enable("chan_logs", true);
  logs.log_hierarchy(top);
  trace_hierarchy(&top, trace_file_ptr);

  sc_start();
  std::string diff_cmd;
  diff_cmd += "diff " + resp_log + " " + run_logs + "/" + resp_log;
  std::cout << "Comparing against reference file: " << diff_cmd << std::endl;
  retcode = system(diff_cmd.c_str());
  if (retcode || sc_report_handler::get_count(SC_ERROR) > 0) {
    std::cout << "Simulation FAILED" << std::endl;
    return -1;
  } else {
    std::cout << "Simulation PASSED" << std::endl;
  }
  return retcode;
}

