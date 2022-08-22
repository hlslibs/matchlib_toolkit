/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Thu Aug 11 16:24:59 PDT 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.9                                               *
 *                                                                        *
 *  Copyright 2020 Siemens                                                *
 *                                                                        *
 **************************************************************************
 *  Licensed under the Apache License, Version 2.0 (the "License");       *
 *  you may not use this file except in compliance with the License.      * 
 *  You may obtain a copy of the License at                               *
 *                                                                        *
 *      http://www.apache.org/licenses/LICENSE-2.0                        *
 *                                                                        *
 *  Unless required by applicable law or agreed to in writing, software   * 
 *  distributed under the License is distributed on an "AS IS" BASIS,     * 
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or       *
 *  implied.                                                              * 
 *  See the License for the specific language governing permissions and   * 
 *  limitations under the License.                                        *
 **************************************************************************
 *                                                                        *
 *  The most recent version of this package is available at github.       *
 *                                                                        *
 *************************************************************************/

#pragma once

#include "jpeg.h"
#include "dct.h"
#include "mc_fifo.h"
#include <mc_scverify.h>

SC_MODULE(pixelpipe_mod)
{
  public:
  sc_in <bool>                        CCS_INIT_S1(clk);
  sc_in <bool>                        CCS_INIT_S1(rstn);
  Connections::In<ac_int<2, false > > CCS_INIT_S1(blocktype);
  Connections::In<rgb_t >             CCS_INIT_S1(rgb);
  Connections::Out<codes_t>           CCS_INIT_S1(codes);

#ifdef TOP_PIXELPIPE_MOD
  dct CCS_INIT_S1(dct_inst);
#else
  CCS_DESIGN(dct) CCS_INIT_S1(dct_inst);
#endif

  SC_CTOR(pixelpipe_mod) {
    SC_THREAD(broadcast_exec);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
    SC_THREAD(convert_exec);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
    SC_THREAD(quantize_exec);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);
    SC_THREAD(encode_exec);
    sensitive << clk.pos();
    async_reset_signal_is(rstn, false);

    //DCT instance port bindings
    dct_inst.clk(clk);
    dct_inst.rstn(rstn);
    dct_inst.input(converted);
    dct_inst.output(transformed);
    dct_inst.sync_out(sync_out);

    //DCT output memory port bindings
    transformed.CK(clk);

    //fifo clock and reset port bindings

    quantized.clk(clk);
    quantized.rst_bar(rstn);
    quantized.in1(quantized_in);
    quantized.out1(quantized_out);

    param1.clk(clk);
    param1.rst_bar(rstn);
    param1.in1(param1_in);
    param1.out1(param1_out);

    param2.clk(clk);
    param2.rst_bar(rstn);
    param2.in1(param2_in);
    param2.out1(param2_out);

    param3.clk(clk);
    param3.rst_bar(rstn);
    param3.in1(param3_in);
    param3.out1(param3_out);
  }

  void broadcast();
  void convert();
  void quantize(bool &pp);
  void encode();

  private:

  void broadcast_exec() {
    blocktype.Reset();
    param1_in.ResetWrite();
    param2_in.ResetWrite();
    param3_in.ResetWrite();
    wait();                                 // WAIT
    while (1) {
      broadcast();
    }
  }

  // convert 8x8 RGB block to 8x8 YCbCr
  void convert_exec() {
    rgb.Reset();
    param1_out.ResetRead();
    converted.ResetWrite();
    wait();                                 // WAIT
    while (1) { convert(); }
  }

  // zig-zag and quantize the results
  void quantize_exec() {
    bool pp = false;
    param2_out.ResetRead();
    transformed.reset_read();
    quantized_in.ResetWrite();
    sync_out.reset_sync_in();
    last_non_zero_index.ResetWrite();
    wait();                                 // WAIT
    while (1) { quantize(pp); }
  }

  // run-length and Huffman encode
  void encode_exec() {
    codes.Reset();
    quantized_out.ResetRead();
    param3_out.ResetRead();
    last_non_zero_index.ResetRead();
    wait();                                 // WAIT
    while (1) { encode(); }
  }

  Connections::Combinational<ac_int<8>> CCS_INIT_S1(converted);
  RAM_1R1W_model<>::mem<ac_int<16>,128>  CCS_INIT_S1(transformed);
  Connections::SyncChannel CCS_INIT_S1(sync_out);
  Connections::Combinational<uint6> CCS_INIT_S1(last_non_zero_index);

  Connections::FifoModule<ac_int<16, true>, 65> CCS_INIT_S1(quantized);
  Connections::Combinational<ac_int<16, true>> CCS_INIT_S1(quantized_in);
  Connections::Combinational<ac_int<16, true>> CCS_INIT_S1(quantized_out);

  Connections::FifoModule<uint2, 8>  CCS_INIT_S1(param1);
  Connections::Combinational<uint2>  CCS_INIT_S1(param1_in);
  Connections::Combinational<uint2>  CCS_INIT_S1(param1_out);

  Connections::FifoModule<uint2, 8>  CCS_INIT_S1(param2);
  Connections::Combinational<uint2>  CCS_INIT_S1(param2_in);
  Connections::Combinational<uint2>  CCS_INIT_S1(param2_out);

  Connections::FifoModule<uint2, 8>  CCS_INIT_S1(param3);
  Connections::Combinational<uint2>  CCS_INIT_S1(param3_in);
  Connections::Combinational<uint2>  CCS_INIT_S1(param3_out);

};

