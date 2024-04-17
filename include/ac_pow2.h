
#pragma once

// local helper class for maximum power of 2 <= W

template <size_t W>
struct ac_pow2;

template <>
struct ac_pow2<1>
{
  static const size_t P = 1;
};

template <size_t W>
struct ac_pow2
{
  typedef ac_pow2<(W>>1)> SUB;
  static const size_t P = SUB::P << 1;
};
