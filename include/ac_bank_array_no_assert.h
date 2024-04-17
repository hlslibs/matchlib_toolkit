
// ac_bank_array_no_assert.h
// Stuart Swan, Platform Architect, Siemens EDA
// 11 April 2024
//
// ac_bank_array_no_assert classes support banked memory modeling in HLS.
//
// Consider the following memory in an HLS model:
//   uint32 mem[15][333];
// In the C language, mem occupies a contiguous region of memory. 
// For HW implementation thru HLS,
// typically we would want 15 banks of HW RAMs, each containing 333 elements.
// The ac_bank_array_no_assert classes are "plug compatible" with normal C arrays, but explicitly
// make each bank separate C arrays so that HLS sees the proper HW structure and synthesizes
// more quickly and to more efficient HW.
//
// ac_bank_array_no_assert is same as ac_bank_array, however assertions are removed to
// handle legacy code that needs these classes for use cases that do not model memories/RAMs
// (assertions must be removed in those cases to avoid errors in Catapult in some cases
//  relating to synthesis of assertions into the RTL).

#pragma once

#include <cstddef>
#include <ac_assert.h>

#include <ac_pow2.h>

//==========================================================================


// ac_bank_array_no_assert_base is the base class for banked arrays, 
// and typically is not directly used in user models.

template <typename B, size_t C>
class ac_bank_array_no_assert_base;

template <typename B>
class ac_bank_array_no_assert_base<B, 1>
{
  B a;
public:
  B &operator[](size_t idx) { return a; }
  const B &operator[](size_t idx) const { return a; }
};


template <typename B, size_t C>
class ac_bank_array_no_assert_base
{
  static const size_t W = ac_pow2<C-1>::P;
  ac_bank_array_no_assert_base<B, W  > a0;
  ac_bank_array_no_assert_base<B, C-W> a1;
public:
  B &operator[](size_t idx) { 
#ifndef __SYNTHESIS__
    assert(idx < C);
#endif
    size_t aidx = idx & (W-1); return idx&W ? a1[aidx] : a0[aidx]; 
  }

  const B &operator[](size_t idx) const {
#ifndef __SYNTHESIS__
    assert(idx < C);
#endif
    size_t aidx = idx & (W-1); return idx&W ? a1[aidx] : a0[aidx]; 
  }
};

// ac_bank_array_no_assert_vary<> supports variable number of array dimensions for a banked array
//
// T can be any type: for example:  int, int [0x1000], int [0x10][0x1000]
// Most typically T would be a single dimensional C array type (e.g. int [0x1000])
// However, a wide variety of structures can be modeled, for example:
//  ac_bank_array_no_assert_vary<int[7][9], 5>  // 5 banks of 63 element 2D arrays
//  ac_bank_array_no_assert_vary<int[9], 5, 7>  // 35 banks of 9 element 1D arrays
//  ac_bank_array_no_assert_vary<int, 5, 7, 9>  // 315 separate registers

// variadic templates for ac_bank_array_no_assert_vary<> requires -std=c++11 or greater.
// TODO : add a compiler language version check here to error out if not c++11 at least.


template<typename T, size_t FirstDim, size_t... RestDims>
class ac_bank_array_no_assert_vary : 
 public ac_bank_array_no_assert_base<ac_bank_array_no_assert_vary<T, RestDims...>, FirstDim>
{
};

template<typename T, size_t FirstDim>
class ac_bank_array_no_assert_vary<T, FirstDim> :
  public ac_bank_array_no_assert_base<T, FirstDim> 
{
};

// ac_bank_array_no_assert_2D supports common 2D array case where FirstDim is the number of banks
// and SecondDim is the number of elements in each bank.
// T is typically a simple scalar type, e.g. uint64

template <typename T, size_t FirstDim, size_t SecondDim>
class ac_bank_array_no_assert_2D : public ac_bank_array_no_assert_base<T [SecondDim], FirstDim> {};

// ac_bank_array_no_assert_3D supports common 3D array case where FirstDim and SecondDim determine
// the number of banks and ThirdDim is the number of elements in each bank.
// T is typically a simple scalar type, e.g. uint64

template <typename T, size_t FirstDim, size_t SecondDim, size_t ThirdDim>
class ac_bank_array_no_assert_3D : 
 public ac_bank_array_no_assert_base<ac_bank_array_no_assert_base<T [ThirdDim], SecondDim>, FirstDim> {};

