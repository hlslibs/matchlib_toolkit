// INSERT_EULA_COPYRIGHT: 2020

#pragma once

// Prevent redefine warnings from NVHLS
#undef CONNECTIONS_ASSERT_MSG
#undef CONNECTIONS_SIM_ONLY_ASSERT_MSG

#include "mc_toolkit_utils.h"

template <class T>
class OutToDatVld : public sc_module
{
public:
  Connections::In <T> CCS_INIT_S1(in1);

  sc_out<bool> CCS_INIT_S1(vld);
  sc_out<T>    CCS_INIT_S1(dat);

  SC_CTOR(OutToDatVld) {
    SC_METHOD(drive_rdy);
    sensitive << in1.rdy;
    
    SC_METHOD(drive_dat_vld);
    dont_initialize();
    sensitive << in1.dat << in1.vld;

#ifdef CONNECTIONS_SIM_ONLY
    in1.disable_spawn();
#endif
  }

  void drive_rdy() {
    in1.rdy = 1;
  }

  void drive_dat_vld() {
    vld = in1.vld;
    T t;
    bits_to_type_if_needed(t, in1.dat);
    dat = t;
  }
};

template <class T>
class InFromDatVld : public sc_module
{
public:
  Connections::Out<T> CCS_INIT_S1(out1);

  sc_in<bool> CCS_INIT_S1(vld);
  sc_in<T>    CCS_INIT_S1(dat);

  SC_CTOR(InFromDatVld) {
    SC_METHOD(test_rdy);
    sensitive << out1.rdy;
    
    SC_METHOD(drive_dat_vld);
    dont_initialize();
    sensitive << vld << dat;

#ifdef CONNECTIONS_SIM_ONLY
    out1.disable_spawn();
#endif
  }

  void test_rdy() {
#ifdef CONNECTIONS_SIM_ONLY
    if (!out1.rdy.read() && (sc_time_stamp() > sc_time(100, SC_PS)))
    {
      CCS_LOG("InFromDatVld rdy is: " << out1.rdy);
      SC_REPORT_ERROR("InFromDatVld-01", "rdy signal is false");
    }
#endif
  }

  void drive_dat_vld() {
    out1.vld = vld;
    auto tmp = dat.read();
    type_to_bits_if_needed(out1.dat, tmp);
  }
};


// Would like to not need these macros and instead uses classes/modules, but
// Catapult limitations currently prevent this.

#define OUT_TO_DAT(n, T) \
  OutToDatVld<T> CCS_INIT_S1(n ## _mod); \
  sc_out<T>      CCS_INIT_S1(n ## _dat); \
  Connections::Combinational<T> CCS_INIT_S1(n ## _out); 

#define IN_FROM_DAT(n, T) \
  InFromDatVld<T> CCS_INIT_S1(n ## _mod); \
  sc_in<T>        CCS_INIT_S1(n ## _dat); \
  Connections::Combinational<T> CCS_INIT_S1(n ## _in);  

#define OUT_TO_VLD(n) \
  sc_out<bool>   CCS_INIT_S1(n ## _vld); 

#define IN_FROM_VLD(n) \
  sc_in<bool>    CCS_INIT_S1(n ## _vld);  


// Would like to combine above 2 IN/OUT macros together, 
// but cannot currently because would cause scverify compile error

#define OUT_TO_DAT_VLD_BIND(n) \
  n ## _mod .in1(n ## _out); \
  n ## _mod .dat(n ## _dat); \
  n ## _mod .vld(n ## _vld);

#define IN_FROM_DAT_VLD_BIND(n) \
  n ## _mod .out1(n ## _in); \
  n ## _mod .dat(n ## _dat); \
  n ## _mod .vld(n ## _vld); 

