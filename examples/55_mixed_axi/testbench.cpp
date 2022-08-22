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


#include "mixed_dma.h"
#include "mixed_ram.h"

#include <mc_scverify.h>

class Top : public sc_module
{
public:
  // Hierarchy Instances
  CCS_DESIGN(dma)                         CCS_INIT_S1(dma_inst);      // the DUT
  ram<local_axi_16>                       CCS_INIT_S1(ram_inst);      // slave RAM to talk to

  // Local signal/Connections Declarations
  sc_clock                                clk;
  sc_signal<bool>                         rst_bar;

  Connections::Combinational<bool>        CCS_INIT_S1(dma_done);
  Connections::Combinational<sc_uint<32>> CCS_INIT_S1(dma_dbg);

  local_axi_64::w_chan<>                  CCS_INIT_S1(dma_w_slave);
  local_axi_64::r_chan<>                  CCS_INIT_S1(dma_r_slave);
  local_axi_16::w_chan<>                  CCS_INIT_S1(dma_w_master);
  local_axi_16::r_chan<>                  CCS_INIT_S1(dma_r_master);
  local_axi_64::w_master<>                CCS_INIT_S1(tb_w_master);
  local_axi_64::r_master<>                CCS_INIT_S1(tb_r_master);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    Connections::set_sim_clk(&clk);

    // Connect up the testbench master that will generate requests to the DUT "dma_inst"
    tb_w_master(dma_w_slave);
    tb_r_master(dma_r_slave);

    // Connect up the DUT "dma" instance "dma_inst"
    dma_inst.clk(clk);
    dma_inst.rst_bar(rst_bar);
    dma_inst.r_master0(dma_r_master);
    dma_inst.w_master0(dma_w_master);
    dma_inst.r_slave0(dma_r_slave);
    dma_inst.w_slave0(dma_w_slave);
    dma_inst.dma_done(dma_done);
    dma_inst.dma_dbg(dma_dbg);

    // Connect up the slave RAM instance "ram_inst"
    ram_inst.clk(clk);
    ram_inst.rst_bar(rst_bar);
    ram_inst.r_slave0(dma_r_master);
    ram_inst.w_slave0(dma_w_master);

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

  //  Configuration of the test sequence
  int beats = 0x40;
  int source_addr = 0x1000;
  int target_addr = 0x4000;

  void stim() {
    CCS_LOG("Stimulus started");
    tb_w_master.reset();
    tb_r_master.reset();
    wait();

    tb_w_master.single_write(offsetof(dma_address_map, ar_addr), source_addr);
    tb_w_master.single_write(offsetof(dma_address_map, aw_addr), target_addr);
    tb_w_master.single_write(offsetof(dma_address_map, len),     beats - 1);  // axi encoding: 0 means 1 beat
    tb_w_master.single_write(offsetof(dma_address_map, start),   0x1);
    start_time = sc_time_stamp();

    wait(2000, SC_NS);
    CCS_LOG("stopping sim due to testbench timeout");
    sc_stop();
    wait();
  }

  void done_mon() {
    dma_done.ResetRead();
    wait();

    while (1) {
      bool v = dma_done.Pop();
      end_time = sc_time_stamp();

      CCS_LOG("dma_done detected. " << v);
      CCS_LOG("start_time: " << start_time << " end_time: " << end_time);
      CCS_LOG("axi beats (dec): " << std::dec << beats );
      sc_time elapsed =  end_time - start_time;
      CCS_LOG("elapsed time: " << elapsed);
      CCS_LOG("beat rate: " << (elapsed / beats));
      CCS_LOG("clock period: " << sc_time(1, SC_NS));

      for (int i=0; i < beats; i++) {
        int s = ram_inst.debug_read_addr(source_addr + (i * local_axi_16::axi_cfg::dataWidth/8));
        int t = ram_inst.debug_read_addr(target_addr + (i * local_axi_16::axi_cfg::dataWidth/8));
        if (s != t) {
          CCS_LOG("ram source and target data mismatch! Beat#: " << i << " " <<  std::hex << " s:" << s << " t: " << t);
        }
      }

      sc_stop();
    }
  }

  void dbg_mon() {
    dma_dbg.ResetRead();
    wait();

    while (1) {
      uint32_t v = dma_dbg.Pop();
      CCS_LOG("dma_dbg val: " << std::hex << v);
    }
  }

  void reset() {
    rst_bar.write(0);
    wait(5);
    rst_bar.write(1);
    wait();
  }
};


sc_trace_file *trace_file_ptr;

int sc_main(int argc, char **argv)
{
  trace_file_ptr = sc_create_vcd_trace_file("trace");

  Top top("top");
  trace_hierarchy(&top, trace_file_ptr);

  channel_logs logs;
  logs.enable("chan_log");
  logs.log_hierarchy(top);

  sc_start();
  return 0;
}

