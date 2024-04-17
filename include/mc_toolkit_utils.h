
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

