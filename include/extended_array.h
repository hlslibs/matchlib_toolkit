// INSERT_EULA_COPYRIGHT: 2023


// Author: Stuart Swan, Platform Architect, Siemens EDA
// Date: 19 April 2024

#pragma once

#include <iostream>
#include <iomanip>
#include <ac_array_1D.h>

template <typename T, unsigned N>
class extended_array : public ac_array_1D<T,N> {
public:

  extended_array(std::string _nm = "", bool _add_time_stamp=0, bool _umr_assert=1) 
  {
#ifndef __SYNTHESIS__
    name = _nm;
    add_time_stamp = _add_time_stamp;
    umr_assert = _umr_assert;
    if (name != "") {
      mem_read_log.open(name + "_read.log");
      mem_write_log.open(name + "_write.log");
    }
    Reset();
#endif
  }

#ifndef __SYNTHESIS__
  void Reset() {
    ac_array_1D<T,N>& tmp = *this; 
    for (unsigned i=0; i < N; i++) {
      write_cnt[i] = 0;
      tmp[i] = 0;
    }
  }

  static const int elem_width{Wrapped<T>::width};
  ofstream mem_read_log;
  ofstream mem_write_log;
  std::string name;
  uint32_t write_cnt[N];
  bool add_time_stamp;
  bool umr_assert;


  struct elem_proxy {
    ac_array_1D<T,N>& array;
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
      if (ext_array.umr_assert)
        assert(ext_array.write_cnt[idx] > 0); // assert on uninitialized memory read (UMR)
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
