// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <ac_fixed.h>
#include <mc_connections.h>
#include <ac_math/ac_atan2_cordic.h>
#include <ac_math/ac_sqrt_pwl.h>
#include "edge_defs.h"

using namespace ac_math;
using namespace sc_dt;

struct packed {
  uint9 magn;
  ac_fixed<8,3> angle;

  packed() {}
  packed(const int d) {
    magn = d;
    angle = d;
  }
  inline bool operator == (const packed &rhs) const {
    return (rhs.magn == magn && rhs.angle == angle);
  }
  inline packed &operator = (const packed &rhs) {
    magn = rhs.magn;
    angle = rhs.angle;
    return *this;
  }
  inline friend ostream &operator << ( ostream &os,  packed const &v ) {
#ifndef SC_SYNTHESIS
    os << "(" << v.magn << "," << std::boolalpha << v.angle << ")";
#endif
    return os;
  }
  inline friend void sc_trace(sc_trace_file *tf, const packed &v,
                              const std::string &NAME ) {
#ifndef SC_SYNTHESIS
    sc_trace(tf,v.magn, NAME + ".magn");
    sc_trace(tf,v.angle, NAME + ".angle");
#endif
  }

};

template <> struct mc_typedef_T_traits< packed > {
  enum { bitwidth = 17, issigned = 0 };
};

template<>
inline void type_to_vector(const packed &in, int length, sc_lv<17> &rvec)
{
  rvec.range(16,8) = in.magn;
  rvec.range(7,0) = in.angle.slc<8>(0);//get slice since angle is ac_fixed
}

template<>
inline void vector_to_type(const sc_lv<17> &in, bool issigned, packed *result)
{
  sc_biguint<17> temp = in;
  ac_int<17> temp2 = to_ac(temp);
  result->magn = temp2.slc<9>(8);
  result->angle = temp2.slc<8>(0);
}

