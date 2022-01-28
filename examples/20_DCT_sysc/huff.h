/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 28 15:14:12 PST 2022                        *
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

#include <stdio.h>
#include <assert.h>

typedef unsigned char huff_sizes_t[256];
typedef unsigned int  huff_codes_t[256];

typedef struct {
  huff_sizes_t sizes;
  huff_codes_t codes;
} hufftables_t;

// DC encoding interface, diff is difference from last DC coefficient
// si is size, co is code value
unsigned char huffencode(huff_sizes_t t, int diff);
unsigned int  huffencode(huff_codes_t t, int diff);

// AC encoding interface, r is 0 run length (up to 15), coeff is AC coefficient
// si is size, co is code value
unsigned char huffencode(huff_sizes_t t, unsigned char r, int coeff);
unsigned int  huffencode(huff_codes_t t, unsigned char r, int coeff);

