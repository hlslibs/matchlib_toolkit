// INSERT_EULA_COPYRIGHT: 2020

#include "mc_connections.h"
#include "auto_gen_fields.h"
#include "apb_ram.h"
#include "apb_dma.h"
#include <mc_scverify.h>

// Create a typedef for the configuration of this local APB bus
typedef apb::apb_transactor<axi::cfg::lite> local_apb;

class Top : public sc_module, public local_apb
{
public:

  // Hierarchy Instances
  CCS_DESIGN(dma)                         CCS_INIT_S1(dma_inst);   // the DUT
  ram                                     CCS_INIT_S1(ram_inst);   // slave RAM to talk to
  apb_master_xactor<>                     CCS_INIT_S1(cpu_inst);   // simple "CPU"

  // Local signal/Connections Declarations
  sc_clock                                clk;
  sc_signal<bool>                         rst_bar;

  Connections::Combinational<bool>        CCS_INIT_S1(dma_done);
  Connections::Combinational<uint32_t>    CCS_INIT_S1(dma_dbg);

  apb_signals_chan                        CCS_INIT_S1(dma_master0_apb_signals);
  apb_signals_chan                        CCS_INIT_S1(dma_slave0_apb_signals);

  apb_master_ports<>                      CCS_INIT_S1(cpu_master_ports);

  Connections::Combinational<apb_req>     CCS_INIT_S1(cpu_master_req);  // Connections channel for the apb_req message
  Connections::Combinational<apb_rsp>     CCS_INIT_S1(cpu_master_rsp);  // Connections channel for the apb_rsp message

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {

    // Connect up the testbench master that will generate requests to the DUT "dma_inst"
    cpu_inst.clk(clk);
    cpu_inst.rst_bar(rst_bar);
    cpu_inst.req_port(cpu_master_req);
    cpu_inst.rsp_port(cpu_master_rsp);
    // Connect testbench master to the slave port of the DUT "dma" via cpu_master_ports->dma_slave0_apb_signals
    cpu_inst(cpu_master_ports);
    cpu_master_ports(dma_slave0_apb_signals);

    // Connect up the DUT "dma" instance "dma_inst"
    dma_inst.clk(clk);
    dma_inst.rst_bar(rst_bar);
    dma_inst.dma_done(dma_done);
    dma_inst.dma_dbg(dma_dbg);
    // Connection from cpu_inst
    dma_inst.slave0_ports(dma_slave0_apb_signals);
    // Connection to "ram"
    dma_inst.master0_ports(dma_master0_apb_signals);

    // Connect up the slave RAM instance "ram_inst"
    ram_inst.clk(clk);
    ram_inst.rst_bar(rst_bar);
    ram_inst.slave0_ports(dma_master0_apb_signals);

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
    cpu_master_req.ResetWrite();
    cpu_master_rsp.ResetRead();
    wait();

    apb_req req;
    apb_rsp rsp;

    // Program DMA ar_addr with source_addr
    req.is_write = true;
    req.addr.addr = offsetof(dma_configuration_regs, ar_addr);
    req.w.data = source_addr;
    cpu_master_req.Push(req);
    rsp = cpu_master_rsp.Pop();

    // Program DMA aw_addr with target_addr
    req.is_write = true;
    req.addr.addr = offsetof(dma_configuration_regs, aw_addr);
    req.w.data = target_addr;
    cpu_master_req.Push(req);
    rsp = cpu_master_rsp.Pop();

    // Program DMA len with beats-1
    req.is_write = true;
    req.addr.addr = offsetof(dma_configuration_regs, len);
    req.w.data = beats - 1; // axi encoding: 0 means 1 beat
    cpu_master_req.Push(req);
    rsp = cpu_master_rsp.Pop();

    // Program DMA to start
    req.is_write = true;
    req.addr.addr = offsetof(dma_configuration_regs, start);
    req.w.data = 0x1;
    cpu_master_req.Push(req);
    rsp = cpu_master_rsp.Pop();

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
        int s = ram_inst.debug_read_addr(source_addr + (i * axi_cfg::dataWidth/8));
        int t = ram_inst.debug_read_addr(target_addr + (i * axi_cfg::dataWidth/8));
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

int sc_main(int argc, char **argv)
{
  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file("trace");

  Top top("top");
  trace_hierarchy(&top, trace_file_ptr);

  channel_logs logs;
  logs.enable("chan_log");
  logs.log_hierarchy(top);

  sc_start();
  return 0;
}

