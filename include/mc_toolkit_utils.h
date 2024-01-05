/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2.1                                                 *
 *                                                                        *
 *  Release Date    : Fri Jan  5 08:41:49 PST 2024                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 2.1.1                                               *
 *                                                                        *
 *  Copyright 2023 Siemens                                                *
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


#include "TypeToBits.h"

template <class L>
void type_to_bits_if_needed(L& left, L& right)
{
  left = right;
}

template <class L>
void type_to_bits_if_needed(sc_out<L>& left, L& right)
{
  left.write(right);
}

template <int L, class R>
void type_to_bits_if_needed(sc_lv<L>& left, R& right)
{
   left = TypeToBits(right);
}

template <int L, class R>
void type_to_bits_if_needed(sc_out<sc_lv<L>>& left, R& right)
{
   left.write(TypeToBits(right));
}


template <class L>
void bits_to_type_if_needed(L& left, L& right)
{
  left = right;
}

template <class L, class R>
void bits_to_type_if_needed(L& left, sc_in<R>& right)
{
  left = right.read();
}

template <class L, int R>
void bits_to_type_if_needed(L& left, sc_lv<R>& right)
{
   left = BitsToType<L>(right);
}

template <class L, int R>
void bits_to_type_if_needed(L& left, sc_in<sc_lv<R>>& right)
{
   left = BitsToType<L>(right.read());
}

