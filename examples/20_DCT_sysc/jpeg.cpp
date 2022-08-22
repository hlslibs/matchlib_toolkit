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

#include "jpeg.h"
#include "pixelpipe_mod.h"
// ----------------------------------------------------------------------------------------------

// ---------------------------------------------------
// Convert a 8x8 RGB block to a 8x8 YCbCr block
// ---------------------------------------------------
void pixelpipe_mod::convert()
{
  ac_fixed<16,1,true> coeffs[3][3] = {
    { 0.299,  0.587,  0.114    },
    {-0.168736, -0.331264,  0.5      },
    { 0.5, -0.418688, -0.081312 }
  };
  ac_fixed<8,8,false> tmp;

  uint2 k = param1_out.Pop();

  // normalize values by subtracting 128 and convert to YCbCr
  unsigned char r, g, b;
  convert2ycbcr:
  for (unsigned int i=0; i<8; i++) {
    for (unsigned int j=0; j<8; j++) {
      rgb_t datai = rgb.Pop();
      r = datai.r;
      g = datai.g;
      b = datai.b;
      tmp = coeffs[k][0]*r + coeffs[k][1]*g + coeffs[k][2]*b + ((k)?128:0);
      tmp -= 128;
      converted.Push( tmp.to_uint() );
    }
  }
}

void pixelpipe_mod::quantize(bool &pp) // output  index of the last non-zero value in the output block
{
  uint2   type     = param2_out.Pop();
  uint1   LC       = (type==LUMA)?LUMA:CHROMA;
  uint6   last_nzi = 0;
  uint6   addr;
  const unsigned zigzagpath_vector[64] = {
    0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
  };

  sync_out.sync_in();
  for (unsigned short i=0; i<64; i++) {
    // Zig-Zag encoding
    addr = zigzagpath_vector[i];

    int16 vectorized = transformed[addr.slc<3>(3)*8 + addr.slc<3>(0) + (pp ? 64 : 0)];

    // stuart swan: this wait() currently needed to resync to clk since mem access above
    // gets us 300 ps off clock
    wait();

    // Quantization
    int16 quantized_dat  = (short)(vectorized / qvector[LC][i]);

    // Find last non-zero value
    if (quantized_dat) {
      last_nzi = i;
    }

    // Write stream of quantized values
    quantized_in.Push(quantized_dat);
  }

  // Write out index of last non-zero value
  last_non_zero_index.Push(last_nzi);
  pp = !pp;
}

// ---------------------------------------------------
// Huffman encode
void pixelpipe_mod::encode()                  // output huffman codes
{

  // previous dc coefficient values for the 3 picture elements from last block
  static int dc[3] = {0, 0, 0};

  uint6  num_zeros = 0;
  bool   new_code;
  uint6  huf_size;
  uint32 huf_code;

  uint6  huf_lnzi = last_non_zero_index.Pop();
  uint2  type = param3_out.Pop();

  // Luma/Chroma block
  uint1 LC = (type==LUMA)?LUMA:CHROMA;


  // ---------------------------------------------
  // Huffman encoding
  huffmanization:
  for (unsigned short i=0; i<64; i++) {

    int value = quantized_out.Pop();
    if (i==0) {
      // ---------------------------------------------
      // Huffman encode the DC Coefficient
      int dcval         = value;
      int diff          = dcval - dc[type];
      dc[type]   = dcval;

      huf_code = huffencode(huffcodes[LC][DC],diff);
      huf_size = huffencode(huffsizes[LC][DC],diff);
    } else {
      // ---------------------------------------------
      // Huffman encode the AC Coefficient
      if (value) {
        // if data is not 0...
        // ...insert the (num_zeros,block[i]) code
        new_code   = true;
      } else {
        // if data is 0...
        if (num_zeros==15 && i<huf_lnzi) {
          // if we found 16 consecutive zeros
          // and if we are not yet in the last block of zero's
          // ...insert the special (15,0) code
          new_code   = true;
        } else if (i==63) {
          // if we reached the end of the last block of zero's
          // ...insert the special (0,0) code
          new_code   = true;
        } else {
          // else keep counting zero's !
          new_code   = false;
        }
      }
      huf_code  = (new_code) ? huffencode(huffcodes[LC][AC],num_zeros,value) : 0;
      huf_size  = (new_code) ? huffencode(huffsizes[LC][AC],num_zeros,value) : 0;
      num_zeros = (new_code || (i>=huf_lnzi)) ? 0 : (int)num_zeros+1;
    }

    //  huf_code : the code        (the bits)
    //  huf_size : the code length (the number of bits)
    codes.Push(codes_t(huf_size, huf_code));
  }
}

void pixelpipe_mod::broadcast()
{
  uint2 type = blocktype.Pop();
  param1_in.Push(type);
  param2_in.Push(type);
  param3_in.Push(type);
}

