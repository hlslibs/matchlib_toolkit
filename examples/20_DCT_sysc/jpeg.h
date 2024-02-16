/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2.1                                                 *
 *                                                                        *
 *  Release Date    : Mon Jan 15 20:15:38 PST 2024                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 2.1.1                                               *
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

#include "huff.h"
#include "tables.h"
#include "bmp.h"
#include "bitstream.h"
#include <ac_int.h>
#include <ac_fixed.h>
#include <mc_connections.h>
#include "auto_gen_fields.h"

// ----------------------------------------------------------------------------------------------
//  Data types
// ----------------------------------------------------------------------------------------------

// Huffman Codes
// DC code is index 0, AC codes 1..63
struct codes_t {
  uint6   size;
  uint32  code;

  codes_t() {
    size=0;
    code=0;
  };

  codes_t(uint6 s, uint32 c) {
    size=s;
    code=c;
  }

  AUTO_GEN_FIELD_METHODS(codes_t, ( \
     size \
   , code \
  ) )
  //
};

template <> struct mc_typedef_T_traits< codes_t > {
  enum { bitwidth = 38, issigned = 0 };
};

template<>
inline void type_to_vector(const codes_t &in, int length, sc_lv<38> &rvec)
{
  rvec.range(37,32) = in.size;
  rvec.range(31,0) = in.code;
}

template<>
inline void vector_to_type(const sc_lv<38> &in, bool issigned, codes_t *result)
{
  sc_biguint<38> temp = in;
  ac_int<38> temp2 = to_ac(temp);
  result->size = temp2.slc<6>(32);
  result->code = temp2.slc<32>(0);
}

struct rgb_t {
  uint8 r;
  uint8 g;
  uint8 b;


  AUTO_GEN_FIELD_METHODS(rgb_t, (\
     r \
   , g \
   , b \
  ) )
  //
};

template <> struct mc_typedef_T_traits< rgb_t > {
  enum { bitwidth = 24, issigned = 0 };
};

template<>
inline void type_to_vector(const rgb_t &in, int length, sc_lv<24> &rvec)
{
  rvec.range(23,16) = in.r;
  rvec.range(15,8) = in.g;
  rvec.range(7,0) = in.b;
}

template<>
inline void vector_to_type(const sc_lv<24> &in, bool issigned, rgb_t *result)
{
  sc_biguint<24> temp = in;
  ac_int<24> temp2 = to_ac(temp);
  result->r = temp2.slc<8>(16);
  result->g = temp2.slc<8>(8);
  result->b = temp2.slc<8>(0);
}

