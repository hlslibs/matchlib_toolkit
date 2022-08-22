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

