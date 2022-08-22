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

