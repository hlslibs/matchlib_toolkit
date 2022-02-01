/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Tue Feb  1 15:18:00 PST 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.8                                               *
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

