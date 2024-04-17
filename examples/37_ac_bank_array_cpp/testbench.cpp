// INSERT_EULA_COPYRIGHT: 2020

#include <mc_scverify.h>

#include "dut.h"

unsigned rand_up_to(unsigned n) {
   return rand() & (n-1);
}

CCS_MAIN(int argc, char **argv)
{
  dut dut1;

  ac_channel<mem_req> in1_chan;
  ac_channel<uint16> out1_chan;

  static constexpr unsigned test_cnt = 1000;
  mem_req writes[test_cnt];
  uint16 ref_mem[DimSize[0]][DimSize[1]];

  // First write to the memory in DUT
  for (unsigned i=0; i < test_cnt; i++) {
      mem_req req1;
      req1.is_write = 1;
      req1.index[0] = rand_up_to(DimSize[0]);
      req1.index[1] = rand_up_to(DimSize[1]);
      req1.data  = rand();
      writes[i] = req1;
      ref_mem[req1.index[0]][req1.index[1]] = req1.data;
      in1_chan.write(req1);
      dut1.run(in1_chan, out1_chan);
  }

  // Now read the memory in DUT
  for (unsigned i=0; i < test_cnt; i++) {
      mem_req req1 = writes[i];
      req1.is_write = 0;
      in1_chan.write(req1);
      dut1.run(in1_chan, out1_chan);
  }

  // Now check the DUT output data
  for (unsigned i=0; i < test_cnt; i++) {
      uint16 r = out1_chan.read();
      if (r != ref_mem[writes[i].index[0]][writes[i].index[1]]) {
        std::cout << "ERROR: index: " << i << "\n";
        return 1;
      }

      std::cout << "TB response: " << r << "\n";
  }

  std::cout << "Test passed" << "\n";

  return 0;
}
