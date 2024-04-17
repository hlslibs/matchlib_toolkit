// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include "ac_int.h"
#include "ac_channel.h"
#include "mc_scverify.h"
#include "ac_bank_array.h"


static const int NumDims = 2;
constexpr int DimSize[NumDims] = {0x10, 0x1000};

struct mem_req {
  bool is_write{0};
  uint16 index[NumDims];
  uint16 data;
};


class dut 
{
public:
  dut() {}

  // uint16 mem[0x10][0x1000];
  // ac_bank_array_vary<uint16, 0x10, 0x10> mem;
  // ac_bank_array_3D<uint16, 4, 4, 0x1000> mem;
  ac_bank_array_2D<uint16, DimSize[0], DimSize[1]> mem;

#pragma hls_design interface top
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
  void CCS_BLOCK(run)(ac_channel<mem_req>& in1, ac_channel<uint16>& out1)
  {
      mem_req req1 = in1.read();

      if (req1.is_write) {
        mem[req1.index[0]][req1.index[1]] = req1.data;
      } else {
        uint16 rdata;
        rdata = mem[req1.index[0]][req1.index[1]];
        out1.write(rdata);
      }
  }
};
