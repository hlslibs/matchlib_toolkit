// INSERT_EULA_COPYRIGHT: 2020

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

