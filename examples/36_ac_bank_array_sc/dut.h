// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>
#include "ac_bank_array.h"
#include "auto_gen_fields.h"


static const int NumDims = 2;
constexpr int DimSize[NumDims] = {0x10, 0x1000};

struct mem_req {
  bool is_write{0};
  uint16 index[NumDims];
  uint16 data;

  AUTO_GEN_FIELD_METHODS(mem_req, ( \
     is_write \
   , index \
   , data \
  ) )
  //
};


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool>                             CCS_INIT_S1(clk);
  sc_in<bool>                             CCS_INIT_S1(rst_bar);
  Connections::Out<uint16>                CCS_INIT_S1(out1);
  Connections::In<mem_req>                CCS_INIT_S1(in1);

  // uint16 mem[0x10][0x1000];
  // ac_bank_array_3D<uint16, 4, 4, 0x1000> mem;
  // ac_bank_array_2D<uint16, DimSize[0], DimSize[1]> mem;
  // ac_bank_array_vary<uint16 [0x1000], 0x10> mem;
  ac_bank_array_2D<uint16, DimSize[0], DimSize[1]> mem;

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  void main() {
    in1.Reset();
    out1.Reset();
    wait();

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      mem_req req1 = in1.Pop();

      if (req1.is_write) {
        mem[req1.index[0]][req1.index[1]] = req1.data;
      } else {
        uint16 rdata;
        rdata = mem[req1.index[0]][req1.index[1]];
        out1.Push(rdata);
      }
    }
  }
};
