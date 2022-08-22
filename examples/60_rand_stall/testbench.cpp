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

#include "fabric.h"
#include "ram.h"
#include "ac_gdb_helpers.cpp"
#include <mc_scverify.h>

typedef axi::axi4_segment<axi::cfg::standard> local_axi;

static const int scenarios = 4;

class Top : public sc_module, public local_axi
{
public:
  ram               CCS_INIT_S1(ram0);
  ram               CCS_INIT_S1(ram1);
  CCS_DESIGN(fabric) CCS_INIT_S1(fabric1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);

  w_chan<> CCS_INIT_S1(fabric_w_slave0);
  r_chan<> CCS_INIT_S1(fabric_r_slave0);
  w_chan<> CCS_INIT_S1(fabric_w_master0);
  r_chan<> CCS_INIT_S1(fabric_r_master0);
  w_chan<> CCS_INIT_S1(fabric_w_master1);
  r_chan<> CCS_INIT_S1(fabric_r_master1);
  Connections::Combinational<bool>        CCS_INIT_S1(dma0_done);
  Connections::Combinational<bool>        CCS_INIT_S1(dma1_done);
  Connections::Combinational<sc_uint<32>> CCS_INIT_S1(dma0_dbg);
  Connections::Combinational<sc_uint<32>> CCS_INIT_S1(dma1_dbg);

  w_master<>    CCS_INIT_S1(tb_w_master);
  r_master<>    CCS_INIT_S1(tb_r_master);

  int test_num;

  SC_HAS_PROCESS(Top);
  Top(sc_module_name nm, int test_num_)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true)
    ,   test_num(test_num_) {
    Connections::set_sim_clk(&clk);

    tb_w_master(fabric_w_slave0);
    tb_r_master(fabric_r_slave0);

    ram0.clk(clk);
    ram0.rst_bar(rst_bar);
    ram0.r_slave0(fabric_r_master0);
    ram0.w_slave0(fabric_w_master0);

    ram1.clk(clk);
    ram1.rst_bar(rst_bar);
    ram1.r_slave0(fabric_r_master1);
    ram1.w_slave0(fabric_w_master1);

    fabric1.clk(clk);
    fabric1.rst_bar(rst_bar);
    fabric1.r_master0(fabric_r_master0);
    fabric1.w_master0(fabric_w_master0);
    fabric1.r_master1(fabric_r_master1);
    fabric1.w_master1(fabric_w_master1);
    fabric1.r_slave0(fabric_r_slave0);
    fabric1.w_slave0(fabric_w_slave0);
    fabric1.dma0_done(dma0_done);
    fabric1.dma0_dbg(dma0_dbg);
    fabric1.dma1_done(dma1_done);
    fabric1.dma1_dbg(dma1_dbg);

    SC_CTHREAD(reset, clk);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(dbg_mon);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(done_mon);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    sc_object_tracer<sc_clock> trace_clk(clk);
  }

  sc_time start_time, end_time;
  int timed_beats;
  static const int ram1_offset = 0x80000;

  struct dma_command {
    int source_addr;
    int target_addr;
    int beats;
  };

  static const int beat_len = 0x10;

  dma_command commands[scenarios][2] = {
    { { 0x0000,  0x2000, beat_len}, { 0x2000, ram1_offset + 0x2000, beat_len} },
    { { 0x0000,  0x2000, beat_len}, { ram1_offset + 0, 0x6000, beat_len} },
    { { ram1_offset + 0x0000, ram1_offset + 0x2000, beat_len}, { 0x0000, 0x2000, beat_len } },
    { { 0x0000,  0x2000, beat_len}, { 0x2000,  ram1_offset + 0x2000, beat_len} },// intentional race condition here!!
  };

  void stim() {
    CCS_LOG("Stimulus started");
    tb_w_master.reset();
    tb_r_master.reset();
    wait();

    dma_command dma0_cmd = commands[test_num][0];
    dma_command dma1_cmd = commands[test_num][1];
    timed_beats = dma0_cmd.beats;

    int dma_addr = 0;
    tb_w_master.single_write(dma_addr + offsetof(dma_address_map, ar_addr), dma0_cmd.source_addr);
    tb_w_master.single_write(dma_addr + offsetof(dma_address_map, aw_addr), dma0_cmd.target_addr);
    tb_w_master.single_write(dma_addr + offsetof(dma_address_map, len), dma0_cmd.beats-1);// axi encoding: 0 means 1 beat

    dma_addr = ram1_offset;
    tb_w_master.single_write(dma_addr + offsetof(dma_address_map, ar_addr), dma1_cmd.source_addr);
    tb_w_master.single_write(dma_addr + offsetof(dma_address_map, aw_addr), dma1_cmd.target_addr);
    tb_w_master.single_write(dma_addr + offsetof(dma_address_map, len), dma1_cmd.beats-1);// axi encoding: 0 means 1 beat

    dma_addr = 0;
    tb_w_master.single_write(dma_addr + offsetof(dma_address_map, start),   0x1);
    wait(12); // allow DMA0 to do transfers before DMA1 starts
    dma_addr = ram1_offset;
    tb_w_master.single_write(dma_addr + offsetof(dma_address_map, start),   0x1);

    start_time = sc_time_stamp();

    wait(40000, SC_NS);
    CCS_LOG("stopping sim due to testbench timeout");
    sc_stop();
    wait();
  }

  uint64_t debug_read_ram(int addr) {
    if (addr < ram1_offset) {
      return ram0.debug_read_addr(addr);
    } else {
      return ram1.debug_read_addr(addr - ram1_offset);
    }
  }

  void done_mon() {
    dma0_done.ResetRead();
    dma1_done.ResetRead();
    wait();

    while (1) {
      bool d0 = dma0_done.Pop();
      bool d1 = dma1_done.Pop();
      end_time = sc_time_stamp();

      CCS_LOG("dma_done detected. " << d0 << " " << d1);
      CCS_LOG("start_time: " << start_time << " end_time: " << end_time);
      CCS_LOG("axi beats (dec): " << std::dec << timed_beats );
      sc_time elapsed =  end_time - start_time;
      CCS_LOG("elapsed time: " << elapsed);
      CCS_LOG("beat rate: " << (elapsed / (timed_beats)));
      CCS_LOG("clock period: " << sc_time(1, SC_NS));

      wait(40, SC_NS);

      for (int dma=0; dma<2; dma++) {
        dma_command cmd = commands[test_num][dma];

        for (int i=0; i < cmd.beats; i++) {
          uint64_t s = debug_read_ram(cmd.source_addr + (i * bytesPerBeat));
          uint64_t t = debug_read_ram(cmd.target_addr + (i * bytesPerBeat));
          if (s != t) {
            CCS_LOG("source and target data mismatch! DMA#: " << dma << " Beat#: " << std::hex << i <<
                " " <<  std::hex << " s:" << s << " t: " << t);
          }
        }
      }

      CCS_LOG("finished checking memory contents");

      sc_stop();
    }
  }

  void dbg_mon() {
    dma0_dbg.ResetRead();
    dma1_dbg.ResetRead();
    wait();

    while (1) {
      uint32_t v = dma0_dbg.Pop();
      CCS_LOG("dma0_dbg val: " << std::hex << v);
    }
  }

  void reset() {
    rst_bar.write(0);
    wait(5);
    rst_bar.write(1);
    wait();
    CCS_LOG("Running FABRIC_TEST # : " << test_num);
  }
};

int sc_main(int argc, char **argv)
{
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  char *test_str = getenv("FABRIC_TEST");
  unsigned test_num = 0;
  if (test_str) {
    test_num = atoi(test_str);
  }

  test_num = 0;

  if (test_num >= scenarios) {
    test_num = 0;
  }

  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file("trace");
  Top top("top", test_num);

  channel_logs logs;
#ifdef CONN_RAND_STALL
  logs.enable("stall_log");
#else
  logs.enable("no_stall_log");
#endif
  logs.log_hierarchy(top);

  trace_hierarchy(&top, trace_file_ptr);

  sc_start();
  if (sc_report_handler::get_count(SC_ERROR) > 0) {
    std::cout << "Simulation FAILED" << std::endl;
    return -1;
  }
  std::cout << "Simulation PASSED" << std::endl;
  return 0;
}

