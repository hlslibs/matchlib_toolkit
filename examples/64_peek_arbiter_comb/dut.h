// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>
#include "auto_gen_fields.h"

// Prevent redefine warnings from NVHLS
#undef CONNECTIONS_ASSERT_MSG
#undef CONNECTIONS_SIM_ONLY_ASSERT_MSG

#include "mc_toolkit_utils.h"

typedef uint32 DTYPE;

struct packet {
static const int SIZE = 64;
  DTYPE data[SIZE];

  AUTO_GEN_FIELD_METHODS(packet, ( \
     data \
  ) )
};

class arbiter_comb : public sc_module {
public:
  Connections::In <packet> CCS_INIT_S1(in1);
  Connections::In <packet> CCS_INIT_S1(in2);
  Connections::Out<packet> CCS_INIT_S1(out1);

  // This module is entirely combinational logic (no sequential logic).

  sc_signal<ac_int<2, false>> CCS_INIT_S1(winner);

  SC_CTOR(arbiter_comb) {
    SC_METHOD(out1_vld_method);
    dont_initialize();
    sensitive << in1.vld << in2.vld;

    SC_METHOD(winner_method);
    dont_initialize();
    sensitive << in1.vld << in2.vld << in1.dat << in2.dat;

    SC_METHOD(in1_rdy_method);
    dont_initialize();
    sensitive << winner << out1.rdy;

    SC_METHOD(in2_rdy_method);
    dont_initialize();
    sensitive << winner << out1.rdy;

#ifdef CONNECTIONS_SIM_ONLY
    in1.disable_spawn();
    in2.disable_spawn();
    out1.disable_spawn();
#endif
  }

  void in1_rdy_method() {
    in1.rdy = (winner.read() == 1) && out1.rdy;
  }

  void in2_rdy_method() {
    in2.rdy = (winner.read() == 2) && out1.rdy;
  }

  void out1_vld_method() {
    out1.vld = in1.vld.read() || in2.vld.read();
  }

  void winner_method() {
    packet p1;
    packet p2;
    bits_to_type_if_needed(p1, in1.dat);
    bits_to_type_if_needed(p2, in2.dat);

    ac_int<2, false> winval = 0;

    if (in1.vld) winval = 1;

    if (in2.vld) winval = 2;

    if (in1.vld && in2.vld) {
      if (p1.data[0] >= p2.data[0])
        winval = 1;
      else
        winval = 2;
    }

    winner = winval;

    if (winval == 1)
      out1.dat = in1.dat;
    else if (winval == 2)
      out1.dat = in2.dat;
    else
      out1.dat = in1.dat;
  }
};


#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::In <packet> CCS_INIT_S1(in1);
  Connections::In <packet> CCS_INIT_S1(in2);
  Connections::Out<packet> CCS_INIT_S1(out1);
  Connections::Combinational<packet> CCS_INIT_S1(arb_comb_out1);

  arbiter_comb CCS_INIT_S1(arbiter_comb1);

  SC_CTOR(dut) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    arbiter_comb1.in1(in1);
    arbiter_comb1.in2(in2);
    arbiter_comb1.out1(arb_comb_out1);
  }

private:

  void main() {
    out1.Reset();
    arb_comb_out1.ResetRead();
    wait();

    // This sequential process simply copies arb_comb output to the output of the DUT.
    // In a real design, this process would contain the primary functionality that Catapult
    // is synthesizing..

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      out1.Push(arb_comb_out1.Pop());
    }
  }
};
