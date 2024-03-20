
#pragma once

#include <mc_connections.h>
#include <ac_int.h> 
#include "array_t.h"
#include "ac_shared_bank_array.h"
#include <string>


#pragma hls_design top
class matrixMultiply  : public sc_module {
 public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rstn);

  Connections::In <ac_int<8>> CCS_INIT_S1(A);
  Connections::In <ac_int<8>> CCS_INIT_S1(B);
  Connections::Out<ac_int<8+8+3>> CCS_INIT_S1(C);

  ac_shared_bank_array_2D<ac_int<8>, 8, 8*2> B_transpose;
  Connections::SyncChannel sync; // memory synchronization between threads
  Connections::Combinational <array_t<ac_int<8>,8>> CCS_INIT_S1(A_row);

  SC_CTOR(matrixMultiply) {
    SC_THREAD(pack_A);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);

    SC_THREAD(transpose);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);

    SC_THREAD(mac);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
  }

  void pack_A() {
    A.Reset();
    A_row.ResetWrite();
    wait();

    #pragma hls_pipeline_init_interval 1
    #pragma pipeline_stall_mode flush
    while (1) {
      array_t<ac_int<8>,8> A_dat;
      #pragma hls_unroll yes
      for (int i=0; i<8; i++) {
        A_dat.data[i] = 0;
      }
      for (int i=0; i<8; i++) {
        A_dat.data[i] = A.Pop();
      }
      A_row.Push(A_dat);
    }
  }

  void transpose() {
    B.Reset();
    sync.reset_sync_out();
    bool ping_pong = false;
    wait();

    while (1) {
      #pragma hls_pipeline_init_interval 1
      #pragma pipeline_stall_mode flush
      TRANSPOSEB_ROW0:for (int i=0; i<8; i++) { // Transpose operation must complete first
        TRANSPOSEB_COL0:for (int j=0; j<8; j++) {
          B_transpose[i][j + 8*ping_pong] = B.Pop();
        }
      }
      ping_pong = !ping_pong;
      sync.sync_out();
    }
  }

  void mac() {
    C.Reset();
    A_row.ResetRead();
    sync.reset_sync_in();
    array_t<ac_int<8>,8> A_dat;
    ac_int<8> B_dat;
    bool ping_pong = false;
    wait();

    while (1) {
      ac_int<8+8+3> acc = 0;
      sync.sync_in();
      #pragma hls_pipeline_init_interval 1
      #pragma pipeline_stall_mode flush
      ROW:for (int i = 0; i < 8; i++) {
        A_dat = A_row.Pop();
        COL:for (int j = 0; j < 8; j++) {
          acc = 0;
          #pragma hls_unroll yes
          MAC:for (int k = 0; k < 8; k++) { // loop can be unrolled without mem port contention..
            B_dat = B_transpose[k][j + 8*ping_pong];
            acc += A_dat.data[k] * B_dat;
          }
          C.Push(acc);
        }
      }
      ping_pong = !ping_pong;
    }
  }
};
