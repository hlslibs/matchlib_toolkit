// INSERT_EULA_COPYRIGHT: 2020

#include "dut.h"
#include <mc_scverify.h>

CCS_MAIN(int argc, char **argv)
{
  dut dut1;

  local_mem::word_type*  ref_mem;
  bool* ref_mem_valid;
  unsigned match_count{0};
  unsigned mismatch_count{0};

  ac_channel<local_mem::word_type> out1;
  ac_channel<dut_in_t> in1;
  ac_channel<uint64> read_fifo1;

  ref_mem = new local_mem::word_type[local_mem::capacity_in_words];
  ref_mem_valid = new bool[local_mem::capacity_in_words];

  for (unsigned i=0; i < local_mem::capacity_in_words; i++)
      ref_mem_valid[i] = 0;

  static const int test_count = 1000;

  dut_in_t dut_in1;

  // Write random coefficient values to ascending memory locations:
  unsigned addr = 0;
  for (unsigned r = 0; r < local_mem::words_per_bank; r++) {
      dut_in1.is_load = 0;
      dut_in1.addr = addr;
      for (unsigned i=0; i < local_mem::num_inputs; i++) {
        dut_in1.data[i] = rand();
        ++addr;
        ref_mem_valid[addr] = 1;
        ref_mem[addr] = dut_in1.data[i];
      }
      in1.write(dut_in1);
      dut1.run(in1, out1);
  }

  // Do multiply-accumulate operations using coefficients in scratchpad and input data 
  // weights sent here.
  // starting address for the coefficient reads is randomly selected here
  for (unsigned r = 0; r < test_count; r++) {
      unsigned addr;
      while (1) {
        addr = rand() &  (( 1 << local_mem::addr_width ) - 1);
        if (addr < local_mem::capacity_in_words - local_mem::num_inputs)
          break;
      }
      dut_in1.is_load = 1;
      dut_in1.addr = addr;

      // compute MAC for ref model checking here:
      unsigned sum = 0;
      for (unsigned i=0; i < local_mem::num_inputs; i++) {
        dut_in1.data[i] = rand();
        ++addr;
        sum += dut_in1.data[i] * ref_mem[addr];
      }

      in1.write(dut_in1);
      read_fifo1.write(sum);
      dut1.run(in1, out1);
  }

  for (unsigned i=0; i < test_count; i++) {
          uint64 ref = read_fifo1.read();
          local_mem::word_type rsp = out1.read();
          if (ref != rsp) {
            ++mismatch_count;
          }
          else {
            ++match_count;
          }
  }

  std::cout << "MATCH    COUNT: " << std::dec << match_count << "\n";
  std::cout << "MISMATCH COUNT: " << std::dec << mismatch_count << "\n";

  CCS_RETURN(0);
}

#ifndef __SYNTHESIS__
int sc_main(int argc, char **argv) { return 0;}
#endif
