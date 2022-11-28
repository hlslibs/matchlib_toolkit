/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.3                                                 *
 *                                                                        *
 *  Release Date    : Mon Oct 17 12:31:50 PDT 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.3.0                                               *
 *                                                                        *
 *  Copyright 2022 Siemens                                                *
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


#include "dma_cmd.h"
#include "mixed_dma.h"
#include "mixed_ram.h"

#include <mc_scverify.h>

class Top : public sc_module
{
public:
  // Hierarchy Instances
  CCS_DESIGN(dma)                         CCS_INIT_S1(dma_inst);      // the DUT
  ram<local_axi_64>                       CCS_INIT_S1(ram_inst);      // slave RAM to talk to

  // Local signal/Connections Declarations
  sc_clock                                clk;
  sc_signal<bool>                         rst_bar;

  Connections::Combinational<bool>        CCS_INIT_S1(dma_done);

  // Declare AXI lite connection to the DMA (32bit datapath)
  local_axi_lite::w_chan<>                CCS_INIT_S1(dma_w_slave);
  local_axi_lite::r_chan<>                CCS_INIT_S1(dma_r_slave);

  // Declare the AXI master connection to the RAM (64bit datapath)
  local_axi_64::w_chan<>                  CCS_INIT_S1(dma_w_master);
  local_axi_64::r_chan<>                  CCS_INIT_S1(dma_r_master);

  // Local interfaces
  local_axi_lite::w_master<>              CCS_INIT_S1(tb_w_master);
  local_axi_lite::r_master<>              CCS_INIT_S1(tb_r_master);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {

    // Connect up the testbench master that will generate requests to the DUT "dma_inst"
    tb_w_master(dma_w_slave);
    tb_r_master(dma_r_slave);

    // Connect up the DUT "dma" instance "dma_inst"
    dma_inst.clk(clk);
    dma_inst.rst_bar(rst_bar);
    dma_inst.r_master0(dma_r_master); // connections to the "RAM"
    dma_inst.w_master0(dma_w_master); // connections to the "RAM"
    dma_inst.r_slave0(dma_r_slave);   // connections to the "CPU" (i.e. tb)
    dma_inst.w_slave0(dma_w_slave);   // connections to the "CPU" (i.e. tb)
    dma_inst.dma_done(dma_done);

    // Connect up the slave RAM instance "ram_inst"
    ram_inst.clk(clk);
    ram_inst.rst_bar(rst_bar);
    ram_inst.r_slave0(dma_r_master);  // connection to the "DMA"
    ram_inst.w_slave0(dma_w_master);  // connection to the "DMA"

    SC_CTHREAD(reset, clk);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(done_mon);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    sc_object_tracer<sc_clock> trace_clk(clk);
  }

  sc_time start_time, end_time;

  // Configuration of the test sequence
  dma_cmd test_cmd {0x1000, 0x4000, 0x40};

  void stim() {
    local_axi_lite::WRespPayload b_resp;
    local_axi_lite::ReadPayload  r_resp;

    CCS_LOG("Stimulus started");
    tb_w_master.reset();
    tb_r_master.reset();
    wait();

    // Unit test: Expect an error response trying to read "start" - "start" register is write-only
    r_resp = tb_r_master.single_read(offsetof(dma_address_map, start)); 
    if (r_resp.resp != local_axi_64::Enc::XRESP::SLVERR) {
      CCS_LOG("expected failure on read of write-only register not found");
      sc_stop();
    }

    // Write to the configuration register in the DMA block
    CCS_LOG("  - Programming configuration: " << std::hex << test_cmd );
    b_resp = tb_w_master.single_write(offsetof(dma_address_map, src_addr), test_cmd.src_addr);
    b_resp = tb_w_master.single_write(offsetof(dma_address_map, dst_addr), test_cmd.dst_addr);
    b_resp = tb_w_master.single_write(offsetof(dma_address_map, len),      test_cmd.len);

    // Debug read back of the just written configuration
    dma_cmd tmp;
    r_resp = tb_r_master.single_read(offsetof(dma_address_map, src_addr));
    tmp.src_addr = r_resp.data;
    r_resp = tb_r_master.single_read(offsetof(dma_address_map, dst_addr));
    tmp.dst_addr = r_resp.data;
    r_resp = tb_r_master.single_read(offsetof(dma_address_map, len));
    tmp.len = r_resp.data;
    CCS_LOG("  - Programmed configuration: " << std::hex << tmp);

    CCS_LOG("Starting DMA transfer");
    b_resp = tb_w_master.single_write(offsetof(dma_address_map, start),    0x1);
    start_time = sc_time_stamp();

    // Last ditch timeout control
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
      CCS_LOG("axi beats (dec): " << std::dec << test_cmd.len );
      sc_time elapsed =  end_time - start_time;
      CCS_LOG("elapsed time: " << elapsed);
      CCS_LOG("beat rate: " << (elapsed / test_cmd.len));
      CCS_LOG("clock period: " << sc_time(1, SC_NS));

      for (int i=0; i < test_cmd.len; i++) {
        int s = ram_inst.debug_read_addr(test_cmd.src_addr + (i * local_axi_64::axi_cfg::dataWidth/8));
        int t = ram_inst.debug_read_addr(test_cmd.dst_addr + (i * local_axi_64::axi_cfg::dataWidth/8));
        if (s != t) {
          CCS_LOG("ram source and target data mismatch! Beat#: " << i << " " <<  std::hex << " s:" << s << " t: " << t);
        }
      }

      sc_stop();
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

