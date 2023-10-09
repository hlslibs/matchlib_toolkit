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

  ac_channel<local_mem::rsp_t> out1;
  ac_channel<local_mem::req_t> in1;
  ac_channel<uint64> read_fifos[local_mem::num_inputs];

  ref_mem = new local_mem::word_type[local_mem::capacity_in_words];
  ref_mem_valid = new bool[local_mem::capacity_in_words];

  for (unsigned i=0; i < local_mem::capacity_in_words; i++)
      ref_mem_valid[i] = 0;

  local_mem::req_t req;

  const int test_count = 1000;

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
      in1.write(req);
      dut1.run(in1, out1);
  }

  // Generate empty writes (all valids false) to flush all pending writes thru the scratchpad
  // this insures none of the reads that follow will overtake any of the writes
  for (unsigned z = 0; z < (local_mem::num_inputs * local_mem::input_queue_len); z++) {
      local_mem::req_t req;
      req.type.val = CLITYPE_T::STORE;
      // leave all the valids disabled
      in1.write(req);
      dut1.run(in1, out1);
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
        read_fifos[i].write(addr);
      }
      in1.write(req);
      dut1.run(in1, out1);
  }

  // Generate empty writes (all valids false) to flush all pending reads thru the scratchpad
  for (unsigned z = 0; z < (local_mem::num_inputs * local_mem::input_queue_len); z++) {
      local_mem::req_t req;
      req.type.val = CLITYPE_T::STORE;
      // leave all the valids disabled
      in1.write(req);
      dut1.run(in1, out1);
  }

  while (1)
  {
      local_mem::rsp_t rsp = out1.read();
     
      for (unsigned i=0; i < local_mem::num_inputs; i++) {
        if (rsp.valids[i]) {
          uint64 ref_addr = read_fifos[i].read();
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

      if (match_count + mismatch_count >= test_count)
         break;
  }

  std::cout << "MATCH    COUNT: " << std::dec << match_count << "\n";
  std::cout << "MISMATCH COUNT: " << std::dec << mismatch_count << "\n";

  CCS_RETURN(0);
};

#ifndef __SYNTHESIS__
int sc_main(int argc, char **argv) { return 0;}
#endif
