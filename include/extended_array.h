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

#include <iostream>
#include <iomanip>
#include <array>

template <typename T, unsigned N>
class extended_array : public std::array<T,N> {
public:

  extended_array(std::string _nm = "", bool _add_time_stamp=0) 
  {
#ifndef __SYNTHESIS__
    name = _nm;
    add_time_stamp = _add_time_stamp;
    if (name != "") {
      mem_read_log.open(name + "_read.log");
      mem_write_log.open(name + "_write.log");
    }
    Reset();
#endif
  }

#ifndef __SYNTHESIS__
  void Reset() {
    for (unsigned i=0; i < N; i++) {
      write_cnt[i] = 0;
      std::array<T,N>& tmp = *this; 
      tmp[i] = 0;
    }
  }

  //use:  
  //   sort -k1 *read.log > xxr 
  //   sort -k1 *write.log > xxw 
  //   diff -d -y xxw xxr

  static const int elem_width{Wrapped<T>::width};
  ofstream mem_read_log;
  ofstream mem_write_log;
  std::string name;
  uint32_t write_cnt[N];
  bool add_time_stamp;


  struct elem_proxy {
    std::array<T,N>& array;
    extended_array& ext_array;
    unsigned idx;

    elem_proxy(extended_array& _array, unsigned _idx) : array(_array), ext_array(_array), idx(_idx) {}

    operator T () { 
      std::ostringstream os;
      if (ext_array.add_time_stamp)
        os << sc_time_stamp();
      else
        os << "";
      sc_lv_base val(elem_width);
      assert(idx < N);
      val = array[idx];
      ext_array.mem_read_log << std::setfill('0') << std::setw(8) << std::hex << idx 
           << " " << ext_array.write_cnt[idx] << " " << val << " " << os.str() << "\n";
      return array[idx]; 
    }

    const T& operator=(const T& v) { 
      std::ostringstream os;
      if (ext_array.add_time_stamp)
        os << sc_time_stamp();
      else
        os << "";
      sc_lv_base val(elem_width);
      assert(idx < N);
      ++ext_array.write_cnt[idx];
      val = array[idx] = v; 
      ext_array.mem_write_log << std::setfill('0') << std::setw(8) << std::hex << idx 
           << " " << ext_array.write_cnt[idx] << " " << val << " " << os.str() << "\n";
      return array[idx]; 
    }
  };

  elem_proxy operator[](unsigned idx) { return elem_proxy(*this, idx); }
  const elem_proxy operator[](unsigned idx) const { return elem_proxy(*this, idx); }
#endif
};
