
#pragma once

#include "ac_int.h"

template<typename T>
inline T keep_leading_one_internal(T valid){
  constexpr unsigned half = 1 << (ac::log2_ceil<T::width>::val - 1);
  ac_int<T::width - half, false> upper = valid.template slc<T::width - half>(half);
  ac_int<half, false>lower  = valid.template slc<half>(0);

  upper = keep_leading_one_internal(upper);
  lower = upper.or_reduce() ? ac_int<half, false>(0) : keep_leading_one_internal(lower);

  ac_int<T::width, false> tmp;
  tmp.set_slc(0, lower);
  tmp.set_slc(half, upper);
  return tmp;
}

template<>
inline ac_int<1, false> keep_leading_one_internal(ac_int<1, false> valid){
  return valid;
}

#pragma hls_design ccore
#pragma hls_ccore_type combinational
template<typename T>
T keep_leading_one(T valid){
  return keep_leading_one_internal(valid);
}

