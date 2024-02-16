/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2.1                                                 *
 *                                                                        *
 *  Release Date    : Mon Jan 15 20:15:38 PST 2024                        *
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
