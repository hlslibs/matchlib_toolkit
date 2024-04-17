
// ac_array_1D.h
// Stuart Swan, Platform Architect, Siemens EDA
// 11 April 2024
//
// ac_array_1D<> is a 1 dimensional array that provides
// assertion checking on index violations in both the pre-HLS model and in the post-HLS RTL
//
// usage:
//   instead of :
//       uint16 my_array[0x1000];
//   use:
//       ac_array_1D<uint16, 0x1000> my_array;
  

#pragma once

#include <cstddef>
#include <ac_assert.h>

template <typename T, size_t D1>
class ac_array_1D 
{
  T data[D1];
public:
  T &operator[](size_t idx) { 
    assert(idx < D1);
    return data[idx];
  }

  const T &operator[](size_t idx) const {
    assert(idx < D1);
    return data[idx];
  }
};
