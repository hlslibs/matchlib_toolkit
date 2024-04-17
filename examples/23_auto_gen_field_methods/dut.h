// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

#include "auto_gen_port_info.h"
#include "ac_enum.h"

enum ColorBase { red, green, blue};  // C-style enum
enum class SizeBase { small, medium, large }; // C++ "enum class"

// See: 
// https://stackoverflow.com/questions/11363737/enum-value-collision-with-enum-name
// for discussion of differences between C-style enum and C++ "enum class"

typedef ac_enum<ColorBase, 2> Color_t;
typedef ac_enum<SizeBase, 2>  Size_t;



template <int X, int Y>
struct sub_trans_t {
  uint16_t field1;
  ac_int<8, false> field2;
  Color_t enum3;
  uint16_t field4[X][Y];

  AUTO_GEN_FIELD_METHODS(sub_trans_t, ( \
     field1 \
   , field2 \
   , enum3 \
   , field4 \
  ) )
  //
};

struct transaction_t {
  ac_int<32, false> field1;
  sc_uint<32> field2;
  sub_trans_t<3,3> field3;
  uint16_t array[2];
  Size_t enum4;
  bool b;

  AUTO_GEN_FIELD_METHODS(transaction_t, ( \
     field1 \
   , field2 \
   , field3 \
   , array  \
   , enum4  \
   , b \
  ) )
  //
};


template <class T>
class copyio : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  Connections::Out<T> CCS_INIT_S1(out1);
  Connections::In <T> CCS_INIT_S1(in1);
  sc_out<T> CCS_INIT_S1(sig1);
  sc_out<Color_t> CCS_INIT_S1(sig2);
  sc_out<Size_t> CCS_INIT_S1(sig3);

  SC_CTOR(copyio) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    out1.Reset();
    in1.Reset();
    wait();                          

#pragma hls_pipeline_init_interval 1
#pragma pipeline_stall_mode flush
    while (1) {
      transaction_t t = in1.Pop();
      out1.Push(t); 
      sig1 = t;
      sig2 = t.field3.enum3;
      sig3 = t.enum4;
    }
  }
};

#pragma hls_design top
class dut : public sc_module
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  static const int N = 8;

  Connections::In <transaction_t> CCS_INIT_S1(in1);
  Connections::Out<transaction_t> CCS_INIT_S1(out1);
  sc_out<transaction_t> CCS_INIT_S1(sig1);
  sc_out<Color_t> CCS_INIT_S1(sig2);
  sc_out<Size_t> CCS_INIT_S1(sig3);
  sc_out<uint16> CCS_INIT_S1(sig4);

  AUTO_GEN_PORT_INFO(dut, ( \
    clk \
  , rst_bar \
  , in1 \
  , out1 \
  , sig1 \
  , sig2 \
  , sig3 \
  , sig4 \
  ) )
  //


  copyio<transaction_t> CCS_INIT_S1(copy1);
  copyio<transaction_t> CCS_INIT_S1(copy2);

  Connections::Fifo<transaction_t, N> CCS_INIT_S1(fifo1);
  Connections::Combinational<transaction_t> CCS_INIT_S1(fifo1_in1);
  Connections::Combinational<transaction_t> CCS_INIT_S1(fifo1_out1);

  sc_signal<transaction_t> CCS_INIT_S1(tie_off1);
  sc_signal<Color_t> CCS_INIT_S1(tie_off2);
  sc_signal<Size_t> CCS_INIT_S1(tie_off3);


  SC_CTOR(dut) {
    copy1.clk(clk);
    copy1.rst_bar(rst_bar);
    copy1.in1(in1);
    copy1.out1(fifo1_in1);
    copy1.sig1(sig1);
    copy1.sig2(sig2);
    copy1.sig3(sig3);

    fifo1.clk(clk);
    fifo1.rst(rst_bar);
    fifo1.enq(fifo1_in1);
    fifo1.deq(fifo1_out1);

    copy2.clk(clk);
    copy2.rst_bar(rst_bar);
    copy2.in1(fifo1_out1);
    copy2.out1(out1);
    copy2.sig1(tie_off1);
    copy2.sig2(tie_off2);
    copy2.sig3(tie_off3);
  }
};

