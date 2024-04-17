
#pragma once

#include <ac_int.h> 
#include "auto_gen_fields.h"

 

template<typename enumT, int W, bool S=false> 
class ac_enum { 
public: 
  ac_int<W,S> _val; 
  ac_enum(enumT c) : _val((int)c) {  
    AC_ASSERT(_val == (int)c,  
      (ac_int<W,S>::type_name() +
          " used for ac_enum not able to capture enumT value").c_str());  
  } 
  ac_enum() : _val(0) {}
  void operator=(enumT c) {_val = (int)c; 
    AC_ASSERT(_val == (int)c,  
      (ac_int<W,S>::type_name() +
          " used for ac_enum not able to capture enumT value").c_str());  
  }
  operator enumT() { return (enumT)(int)_val; }   // restores the value as an enum type 

  AUTO_GEN_FIELD_METHODS(ac_enum, ( \
    _val \
  ) )
  //
}; 

 
/*****************************************
Simple test of ac_enum:

enum Cstate {Cstart, Cstop, Cvalid, Cinvalid}; 
enum Sign {Neg_one=-1, Pos_one=1}; 

int main() { 
  { 
    ac_enum<Sign,2,true> x = Neg_one; 
    std::cout << x << std::endl; 
    x = Pos_one; 
    std::cout << x << std::endl; 
  } 

  { 
    ac_enum<Cstate,2> x = Cstart; 
    std::cout << x << std::endl; 
    x = Cinvalid; 
    std::cout << x << std::endl; 
  } 

  { 
    ac_enum<Cstate,1> x = Cstart; 
    std::cout << x << std::endl; 
    x = Cinvalid; 
    std::cout << x << std::endl; 
  } 
}

*****************/
