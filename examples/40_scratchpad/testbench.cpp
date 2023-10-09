// INSERT_EULA_COPYRIGHT: 2020

#include "dut.h"
#include <mc_scverify.h>

class Top : public sc_module
{
public:
  CCS_DESIGN(dut) CCS_INIT_S1(dut1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);
  local_mem::word_type*  ref_mem;
  bool* ref_mem_valid;
  unsigned match_count{0};
  unsigned mismatch_count{0};

  Connections::Combinational<local_mem::base_rsp_t>        CCS_INIT_S1(out1);
  Connections::Combinational<local_mem::base_req_t>        CCS_INIT_S1(in1);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_NS, 0.5,0,SC_NS,true) {
    sc_object_tracer<sc_clock> trace_clk(clk);

    ref_mem = new local_mem::word_type[local_mem::capacity_in_words];
    ref_mem_valid = new bool[local_mem::capacity_in_words];

    for (unsigned i=0; i < local_mem::capacity_in_words; i++)
      ref_mem_valid[i] = 0;

    dut1.clk(clk);
    dut1.rst_bar(rst_bar);
    dut1.out1(out1);
    dut1.in1(in1);

    SC_CTHREAD(reset, clk);

    SC_THREAD(stim);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(resp);
    sensitive << clk.posedge_event();
    async_reset_signal_is(rst_bar, false);
  }

  struct my_fifo_t : public tlm::tlm_fifo<uint64> {
    my_fifo_t() : tlm_fifo(32) {}
  };

  my_fifo_t read_fifos[local_mem::num_inputs];


  void stim() {
    CCS_LOG("Stimulus started");
    in1.ResetWrite();
    wait();

    local_mem::scratchpad_req_t req;

    static const int test_count = 1000;

    CCS_LOG("addr_width bank_sel_width capacity_in_words: " << std::dec << local_mem::addr_width << " " << local_mem::bank_sel_width << " " << local_mem::capacity_in_words);

    // Write random values to random memory locations:
    for (unsigned r = 0; r < test_count; r++) {
      for (unsigned i=0; i < local_mem::num_inputs; i++) {
        req.opcode = STORE;
        req.valids[i] = 1;
        bool success=0;
        unsigned addr;
        while (1) {
          addr = rand() &  (( 1 << local_mem::addr_width ) - 1);
          if (ref_mem_valid[addr])
            continue;

          unsigned bank_mask = (1 << local_mem::bank_sel_width) - 1;

          // Scratchpad forbids bank conflicts, so we must make sure there are none..
          bool conflict = 0;
          for (unsigned c=0; c < i; c++)
            if ((bank_mask & req.addr[c]) == (bank_mask & addr))
              conflict = 1;

          if (conflict)
            continue;

          ref_mem_valid[addr] = 1;
          ref_mem[addr] = rand();
          break;
        } 

        req.addr[i] = addr;
        req.data[i] = ref_mem[addr];
        // CCS_LOG("WRITE ADDR DATA:" << std::hex << addr << " " << ref_mem[addr]);
      }
      in1.Push(req);
    }

    // Generate Read requests for random memory locations:
    for (unsigned r = 0; r < test_count; r++) {
      for (unsigned i=0; i < local_mem::num_inputs; i++) {
        req.opcode = LOAD;
        req.valids[i] = 1;
        bool success=0;
        unsigned addr;
        while (1) {
          addr = rand() &  (( 1 << local_mem::addr_width ) - 1);
          if (!ref_mem_valid[addr])
            continue;

          unsigned bank_mask = (1 << local_mem::bank_sel_width) - 1;

          // Scratchpad forbids bank conflicts, so we must make sure there are none..
          bool conflict = 0;
          for (unsigned c=0; c < i; c++)
            if ((bank_mask & req.addr[c]) == (bank_mask & addr))
              conflict = 1;

          if (conflict)
            continue;

          break;
        }
        // CCS_LOG("READ ADDR:" << std::hex << addr);
        req.addr[i] = addr;
        read_fifos[i].put(addr);
      }
      in1.Push(req);
    }

    wait(100);
    CCS_LOG("MATCH    COUNT: " << std::dec << match_count);
    CCS_LOG("MISMATCH COUNT: " << std::dec << mismatch_count);
    sc_stop();
  }

  void resp() {
    out1.ResetRead();
    wait();

    while (1) {
      local_mem::base_rsp_t rsp = out1.Pop();
     
      for (unsigned i=0; i < local_mem::num_inputs; i++)
        if (rsp.valids[i].to_bool()) {
          uint64 ref_addr = read_fifos[i].get();
          // CCS_LOG("READ RESP ADDR:" << std::hex << ref_addr);
          if (ref_mem[ref_addr] != rsp.data[i]) {
            ++mismatch_count;
            // CCS_LOG("DATA MISMATCH: " << std::hex << ref_mem[ref_addr] << " " << rsp.data[i]);
          }
          else {
            ++match_count;
            // CCS_LOG("DATA MATCHES");
          }
        }
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
  sc_start();
  if (sc_report_handler::get_count(SC_ERROR) > 0) {
    std::cout << "Simulation FAILED" << std::endl;
    return -1;
  }
  std::cout << "Simulation PASSED" << std::endl;
  return 0;
}

