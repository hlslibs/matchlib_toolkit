
#pragma once

#include "mc_connections.h"
#include "keep_leading_one.h"
#include "propagate_leading_one.h"

#undef CONNECTIONS_ASSERT_MSG
#undef CONNECTIONS_SIM_ONLY_ASSERT_MSG
#include "mc_toolkit_utils.h"

constexpr unsigned NUM_INPUTS = 9;
typedef ac_int<8, false> data_t;


#pragma hls_design top
template<unsigned size, typename T> // Use AC data types to comply with T::width
SC_MODULE(rr_arbiter){
  sc_in<bool> clk{"clk"};
  sc_in<bool> rst{"rst"};
  Connections::In<T>  in[size];
  Connections::Out<T> out{"out"};

  sc_signal<ac_int<size, false>> winner_sig{"winner_sig"};
  sc_signal<ac_int<size, false>> mask_sig{"mask_sig"};

  SC_CTOR(rr_arbiter){
    SC_METHOD(run);
    for(unsigned i=0; i<size; i++){
      sensitive << in[i].vld << in[i].dat;
    }
    sensitive << out.rdy << mask_sig;

    SC_THREAD(mask_th);
    sensitive << clk.pos();
    async_reset_signal_is(rst, false);
    
    #ifdef CONNECTIONS_SIM_ONLY
    for(unsigned i=0; i<NUM_INPUTS; i++){
      in[i].disable_spawn();
    }
    out.disable_spawn();
    #endif
  }

  void run(){
    ac_int<2*size, false> valid_long = 0; // 2*size-1 should be enough but 2*size is simpler
    #pragma hls_unroll yes
    for(unsigned i=0; i<size; i++){
      valid_long[i] = in[i].vld;
      valid_long[size+i] = in[i].vld;
    }

    ac_int<2*size, false> mask_long;
    mask_long.set_slc(0, mask_sig.read().bit_complement());
    mask_long.set_slc(size, mask_sig.read());

    ac_int<2*size, false> valid_long_masked = valid_long & mask_long;
    ac_int<2*size, false> winner_long = keep_leading_one(valid_long_masked);
    ac_int<size, false> winner = winner_long.template slc<size>(0) | winner_long.template slc<size>(size);
    winner_sig = winner;

    T dat = 0;
    bool vld = 0;
    #pragma hls_unroll yes
    for(unsigned i=0; i<size; i++){
      T tmp;
      bits_to_type_if_needed(tmp, in[i].dat);
      dat |= winner[i] ? tmp : T(0);
      vld |= winner[i] ? in[i].vld : 0;
      in[i].rdy = winner[i] & out.rdy;
    }
    type_to_bits_if_needed(out.dat, dat);
    out.vld = vld;
  }

  void mask_th(){
    mask_sig = ~ac_int<size, false>(0);
    wait();
    #pragma hls_pipeline_init_interval 1
    while(true){
      if(out.rdy && out.vld){
        ac_int<size, false> mask = propagate_leading_one(winner_sig.read() >> 1);
        mask_sig = mask.or_reduce() ? mask : mask.bit_complement();
      }
      wait();
    }
  }
};

template struct rr_arbiter<NUM_INPUTS, data_t>;

