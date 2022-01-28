/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan 28 15:07:52 PST 2022                        *
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

template <unsigned MAX_W, unsigned MAX_H>
class EdgeAlg
{
  int Clip(int i,int bound) {
    if (i<0) {
      return 0;
    } else if (i>=bound) {
      return bound;
    } else {
      return i;
    }
  }

  void VerticalGradient(double *dy, unsigned char *dat_in) {
    // Vertical gradient
    for (unsigned i = 0; i < MAX_H; i++) {
      for (unsigned j = 0; j < MAX_W; j++) {
        *(dy + i * MAX_W + j) =
          dat_in[Clip(i - 1, MAX_H-1) * MAX_W + j] * kernel[0] +
          dat_in[i * MAX_W + j] * kernel[1] +
          dat_in[Clip(i + 1, MAX_H-1) * MAX_W + j] * kernel[2];
      }
    }
  }

  void HorizintalGradient(double *dx, unsigned char *dat_in) {
    // Horizontal gradient
    for (unsigned i = 0; i < MAX_H; i++) {
      for (unsigned j = 0; j < MAX_W; j++) {
        *(dx + i * MAX_W + j) =
          dat_in[i * MAX_W + Clip(j - 1, MAX_W-1)] * kernel[0] +
          dat_in[i * MAX_W + j] * kernel[1] +
          dat_in[i * MAX_W + Clip(j + 1, MAX_W-1)] * kernel[2];
      }
    }
  }

  void MagnitudeAngle( double *dx,
                       double *dy, double *magn, double *angle) {
    double dx_sq, dy_sq, sum;
    for (unsigned i = 0; i < MAX_H; i++) {
      for (unsigned j = 0; j < MAX_W; j++) {
        dx_sq = *(dx + i * MAX_W + j) * *(dx + i * MAX_W + j);
        dy_sq = *(dy + i * MAX_W + j) * *(dy + i * MAX_W + j);
        sum = dx_sq + dy_sq;
        *(magn + i * MAX_W + j) = sqrt(sum);
        *(angle + i * MAX_W + j) = atan2(dy[i * MAX_W + j], dx[i * MAX_W + j]);
      }
    }
  }

public:

  void run(unsigned char *dat_in, double *magn, double *angle) {
    double *dx, *dy;

    dy = (double *)malloc(MAX_H*MAX_W*sizeof(double));
    dx = (double *)malloc(MAX_H*MAX_W*sizeof(double));

    VerticalGradient(dy, dat_in);
    HorizintalGradient(dx, dat_in);
    MagnitudeAngle(dx, dy, magn, angle);

    free(dy);
    free(dx);
  }
};

