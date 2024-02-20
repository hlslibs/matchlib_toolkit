// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>
#include "auto_gen_fields.h"
#include "auto_gen_port_info.h"

template <int X, int Y>
struct sub_trans_t {
  uint16_t array[X][Y];

  AUTO_GEN_FIELD_METHODS(sub_trans_t, ( \
     array \
  ) )
  //
};

struct transaction_t {
  uint16 value;
  sub_trans_t<5,2> sub;

  AUTO_GEN_FIELD_METHODS(transaction_t, ( \
     value \
  ,  sub   \
  ) )
  //
};



#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::Out<transaction_t> CCS_INIT_S1(out1);
  Connections::In <transaction_t> CCS_INIT_S1(in1);

  AUTO_GEN_PORT_INFO(dut, ( \
    clk \
  , rst_bar \
  , out1 \
  , in1 \
  ) )
  //

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

private:

  void main() {
    out1.Reset();
    in1.Reset();
    wait();                                 // WAIT
#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      transaction_t t = in1.Pop();
      t.value += 0x100;
      out1.Push(t);
    }
  }
};
