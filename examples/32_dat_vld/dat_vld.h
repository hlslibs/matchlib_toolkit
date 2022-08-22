/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Thu Aug 11 16:24:59 PDT 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.9                                               *
 *                                                                        *
 *  Copyright  Siemens                                                *
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

template <class T>
T
static convert_from_lv(sc_lv<Wrapped<T>::width> lv)
{
  Wrapped<T> result;
  Marshaller<Wrapped<T>::width> marshaller(lv);
  result.Marshall(marshaller);
  return result.val;
}

template <class T>
sc_lv<Wrapped<T>::width>
static convert_to_lv(T v)
{
  Marshaller<Wrapped<T>::width> marshaller;
  Wrapped<T> wm(v);
  wm.Marshall(marshaller);
  return marshaller.GetResult();
}

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
    T t = convert_from_lv<T>(in1.dat);
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
    if (sc_time_stamp() > sc_time(100, SC_PS)) {
      CCS_LOG("InFromDatVld rdy is: " << out1.rdy);
      sc_assert(0);
    }
    #endif
  }

  void drive_dat_vld() {
    out1.vld = vld;
    out1.dat.write(convert_to_lv(dat.read()));
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

