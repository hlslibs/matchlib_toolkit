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

#include "scatter_gather_dma.h"
#include "ram.h"
#include <mc_scverify.h>
#include <time.h>

typedef axi::axi4_segment<axi::cfg::standard> local_axi;

class Top : public sc_module, public local_axi
{
public:
  ram             CCS_INIT_S1(ram1);
  CCS_DESIGN(scatter_gather_dma) CCS_INIT_S1(dma1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);

  w_chan<> CCS_INIT_S1(dma_w_slave);
  r_chan<> CCS_INIT_S1(dma_r_slave);
  w_chan<> CCS_INIT_S1(dma_w_master);
  r_chan<> CCS_INIT_S1(dma_r_master);
  w_master<>    CCS_INIT_S1(tb_w_master);
  r_master<>    CCS_INIT_S1(tb_r_master);

  Connections::Combinational<bool> CCS_INIT_S1(dma_done);
  Connections::Combinational<bool> CCS_INIT_S1(started_fifo);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    Connections::set_sim_clk(&clk);

    tb_w_master(dma_w_slave);
    tb_r_master(dma_r_slave);

    ram1.clk(clk);
    ram1.rst_bar(rst_bar);
    ram1.r_slave0(dma_r_master);
    ram1.w_slave0(dma_w_master);

    dma1.clk(clk);
    dma1.rst_bar(rst_bar);
    dma1.r_master0(dma_r_master);
    dma1.w_master0(dma_w_master);
    dma1.r_slave0(dma_r_slave);
    dma1.w_slave0(dma_w_slave);
    dma1.dma_done(dma_done);

    SC_CTHREAD(reset, clk);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(checker);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    sc_object_tracer<sc_clock> trace_clk(clk);
  }

  int test_iterations = 1;
  bool copy_mode = false;
  int total_len = 64 * bytesPerBeat;
  int scatter_groups = 4;
  int scatter_len = total_len / scatter_groups;
  int scatter_stride = scatter_len * 2;
  int source_addr = 0x1000;
  int target1_addr = 0x4000;
  int target2_addr = 0x8000;
  sc_time start_time, end_time;

  void stim() {
    CCS_LOG("Stimulus started");
    tb_w_master.reset();
    tb_r_master.reset();
    started_fifo.ResetWrite();
    wait();

    for (int i=0; i < test_iterations; i++) {
      // Step 1: do SCATTER DMA operation
      tb_w_master.single_write(offsetof(dma_address_map, ar_addr),        source_addr);
      tb_w_master.single_write(offsetof(dma_address_map, aw_addr),        target1_addr);
      tb_w_master.single_write(offsetof(dma_address_map, dma_mode),
                               (copy_mode? dma_mode_t::COPY : dma_mode_t::SCATTER));
      tb_w_master.single_write(offsetof(dma_address_map, total_len),      total_len);
      tb_w_master.single_write(offsetof(dma_address_map, scatter_stride), scatter_stride);
      tb_w_master.single_write(offsetof(dma_address_map, scatter_len),    scatter_len);
      tb_w_master.single_write(offsetof(dma_address_map, scatter_groups), scatter_groups);
      b_payload b = tb_w_master.single_write(offsetof(dma_address_map, start),   0x1);

      started_fifo.Push(b.resp == Enc::XRESP::OKAY);

      // Step 2: do GATHER DMA operation to collect the data that was just scattered

      tb_w_master.single_write(offsetof(dma_address_map, ar_addr),        target1_addr);
      tb_w_master.single_write(offsetof(dma_address_map, aw_addr),        target2_addr);
      tb_w_master.single_write(offsetof(dma_address_map, dma_mode),
                               (copy_mode? dma_mode_t::COPY : dma_mode_t::GATHER));
      tb_w_master.single_write(offsetof(dma_address_map, total_len),      total_len);
      tb_w_master.single_write(offsetof(dma_address_map, scatter_stride), scatter_stride);
      tb_w_master.single_write(offsetof(dma_address_map, scatter_len),    scatter_len);
      tb_w_master.single_write(offsetof(dma_address_map, scatter_groups), scatter_groups);
      b_payload b2 = tb_w_master.single_write(offsetof(dma_address_map, start),   0x1);

      started_fifo.Push(b2.resp == Enc::XRESP::OKAY);
    }
  }

  void checker() {
    started_fifo.ResetRead();
    dma_done.ResetRead();
    wait();

    for (int i=0; i < test_iterations; i++) {
      bool s1 = started_fifo.Pop();
      if (i == 0) {
        start_time = sc_time_stamp();
      }

      if (s1) {
        dma_done.Pop();
      }

      bool s2 = started_fifo.Pop();
      if (s2) {
        dma_done.Pop();
      }

      if (!s1 || !s2) {
        CCS_LOG("bad dma start!");
        continue;
      }

      // check that gathered data matches the original data (from before step 1)

      for (int i=0; i < (total_len / bytesPerBeat); i++) {
        int s = ram1.debug_read_addr(source_addr + (i * axi_cfg::dataWidth/8));
        int t = ram1.debug_read_addr(target2_addr + (i * axi_cfg::dataWidth/8));
        if (s != t) {
          CCS_LOG("ram source and target data mismatch! Beat#: " << i << " " <<  std::hex << " s:" << s << " t: " << t);
        }
      }
    }

    end_time = sc_time_stamp();

    CCS_LOG("start_time: " << start_time << " end_time: " << end_time);
    int beats = 2 * (total_len / bytesPerBeat) * test_iterations;
    CCS_LOG("axi beats (dec): " << std::dec << beats);
    sc_time elapsed =  end_time - start_time;
    CCS_LOG("elapsed time: " << elapsed);
    CCS_LOG("beat rate: " << (elapsed / beats));
    CCS_LOG("clock period: " << sc_time(1, SC_NS));

    clock_t usage = clock();
    CCS_LOG("CPU time used: " << (double)usage / (double)CLOCKS_PER_SEC << " seconds");

    sc_stop();
    wait();
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

