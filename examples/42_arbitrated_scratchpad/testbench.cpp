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

  Connections::Combinational<local_mem::rsp_t>        CCS_INIT_S1(out1);
  Connections::Combinational<local_mem::req_t>        CCS_INIT_S1(in1);

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

    local_mem::req_t req;

    static const int test_count = 1000;

    // Write random values to random memory locations:
    for (unsigned r = 0; r < test_count; r++) {
      for (unsigned i=0; i < local_mem::num_inputs; i++) {
        req.type.val = CLITYPE_T::STORE;
        req.valids[i] = rand() & 1;
        if (req.valids[i] == 0)
          continue;

        // find an address we haven't yet written to
        bool success=0;
        unsigned addr;
        do {
          addr = rand() &  (( 1 << local_mem::addr_width ) - 1);
          if (!ref_mem_valid[addr]) {
            ref_mem_valid[addr] = 1;
            ref_mem[addr] = rand();
            success = 1;
          }
        } while (!success);

        // CCS_LOG("WRITE ADDR:" << std::hex << addr);
        req.addr[i] = addr;
        req.data[i] = ref_mem[addr];
      }
      in1.Push(req);
    }

    // Generate empty writes (all valids false) to flush all pending writes thru the scratchpad
    // this insures none of the reads that follow will overtake any of the writes
    for (unsigned z = 0; z < (local_mem::num_inputs * local_mem::input_queue_len); z++) {
      local_mem::req_t req;
      req.type.val = CLITYPE_T::STORE;
      // leave all the valids disabled
      in1.Push(req);
    }

    // Generate Read requests for random memory locations:
    for (unsigned r = 0; r < test_count; r++) {
      for (unsigned i=0; i < local_mem::num_inputs; i++) {
        req.type.val = CLITYPE_T::LOAD;
        req.valids[i] = rand() & 1;
        if (req.valids[i] == 0)
          continue;

        // find an address we did write to in the stimulus thread
        bool success=0;
        unsigned addr;
        do {
          addr = rand() &  (( 1 << local_mem::addr_width ) - 1);
          if (ref_mem_valid[addr]) {
            success = 1;
          }
        } while (!success);
        // CCS_LOG("READ ADDR:" << std::hex << addr);
        req.addr[i] = addr;
        // put the read address into the read fifos so we can check if the response is correct
        read_fifos[i].put(addr);
      }
      in1.Push(req);
    }

    // Generate empty writes (all valids false) to flush all pending reads thru the scratchpad
    for (unsigned z = 0; z < (local_mem::num_inputs * local_mem::input_queue_len); z++) {
      local_mem::req_t req;
      req.type.val = CLITYPE_T::STORE;
      // leave all the valids disabled
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
      local_mem::rsp_t rsp = out1.Pop();
     
      for (unsigned i=0; i < local_mem::num_inputs; i++)
        if (rsp.valids[i]) {
          uint64 ref_addr = read_fifos[i].get();
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

