/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 28 15:18:06 PST 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.7                                               *
 *                                                                        *
 *  Unpublished work. Copyright 2020 Siemens                              *
 *                                                                        *
 **************************************************************************/

// This document contains information that is proprietary to Siemens
// The original recipient of this document may duplicate this  
// document in whole or in part for internal business purposes only, provided  
// that this entire notice appears in all copies. In duplicating any part of  
// this document, the recipient agrees to make every reasonable effort to  
// prevent the unauthorized use and distribution of the proprietary information.

#pragma once

#include "huff.h"
#include "tables.h"
#include "bmp.h"
#include "bitstream.h"
#include <ac_int.h>
#include <ac_fixed.h>
#include <mc_connections.h>

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

  static const unsigned int width = 6  + 32;
  template <unsigned int Size> void Marshall(Marshaller<Size> &m) {
    m &size;
    m &code;
  }
  inline friend void sc_trace(sc_trace_file *tf, const codes_t &v, const std::string &NAME ) {
    sc_trace(tf,v.size,  NAME + ".size");
    sc_trace(tf,v.code,  NAME + ".code");
  }

  inline friend std::ostream &operator<<(ostream &os, const codes_t &rhs) {
    os << std::hex << rhs.size << " ";
    os << std::hex << rhs.code << " ";
    return os;
  }

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

  static const unsigned int width = 3 * 8;
  template <unsigned int Size> void Marshall(Marshaller<Size> &m) {
    m &r;
    m &g;
    m &b;
  }
  inline friend void sc_trace(sc_trace_file *tf, const rgb_t &v, const std::string &NAME ) {
    sc_trace(tf,v.r,  NAME + ".r");
    sc_trace(tf,v.g,  NAME + ".g");
    sc_trace(tf,v.b,  NAME + ".b");
  }

  inline friend std::ostream &operator<<(ostream &os, const rgb_t &rhs) {
    os << std::hex << rhs.r << " ";
    os << std::hex << rhs.g << " ";
    os << std::hex << rhs.b << " ";
    return os;
  }
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
