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

#include "huff.h"
#include "ac_int.h"

// Figure F.1 DC Difference magnitude categories
unsigned char SSSS(int diff)
{
  unsigned char r = 0;
  diff = (diff>=0)?diff:-diff;
#pragma unroll yes
  for (int i=11; i>=0; i--) {
    if ((diff>>i)==0) {
      r=i;
    }
  }
  return r;
}

// returns RRRRSSSS byte, R is run length up to 15, S is category of coefficient
// part of AC encoding
unsigned char RS(unsigned char r, int coefficient)
{
#ifndef __SYNTHESIS__
  assert(r <= 15);
#endif
  return (r << 4) | SSSS(coefficient);
}

// Concatinate bits and return them (with result msbs 0 if unfilled)
unsigned int concat_bits(unsigned int msbs, int lsbs, unsigned char nlsbs)
{
  lsbs &= (1<<(nlsbs))-1;
  return (msbs << nlsbs) | lsbs;
}

unsigned char huffencode(huff_sizes_t t, int diff)
{
  unsigned char magnitude = SSSS(diff);
  unsigned char huffsize  = t[magnitude];
  return magnitude + huffsize;
}

unsigned int huffencode(huff_codes_t t, int diff)
{
  unsigned char magnitude = SSSS(diff);
  unsigned int  huffcode  = t[magnitude];
  if (diff < 0) {
    diff = diff - 1;
  }
  return concat_bits(huffcode, diff, magnitude);
}

unsigned int huffencode(huff_codes_t t, unsigned char r, int coeff)
{
  unsigned char magnitude = RS(r, coeff);
  unsigned int  huffcode  = t[magnitude];
  return concat_bits(huffcode, (coeff < 0) ? coeff - 1 : coeff, SSSS(coeff));
}

unsigned char huffencode(huff_sizes_t t, unsigned char r, int coeff)
{
  unsigned char magnitude = RS(r, coeff);
  unsigned char huffsize  = t[magnitude];
  return huffsize + SSSS(coeff);
}

