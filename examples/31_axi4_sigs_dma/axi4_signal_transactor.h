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
 *  Copyright 2022 Siemens                                                *
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

#include <mc_connections.h>

// Prevent redefine warnings from NVHLS
#undef CONNECTIONS_ASSERT_MSG
#undef CONNECTIONS_SIM_ONLY_ASSERT_MSG

#include "axi4_segment.h"

/*
In effect this code below is just "wires" (or effectively Verilog "assign" statements)
that rename the ports on modules. During HLS, this code will not add any real logic.
*/

namespace axi
{

  template <typename Cfg>
  class axi4_signal_transactor : public axi::axi4_segment<Cfg>
  {
  public:

    typedef Cfg axi_cfg;

    typedef typename axi::axi4<Cfg>::AddrPayload addr_payload;
    typedef typename axi::axi4<Cfg>::AddrPayload aw_payload;
    typedef typename axi::axi4<Cfg>::AddrPayload ar_payload;
    typedef typename axi::axi4<Cfg>::ReadPayload r_payload;
    typedef typename axi::axi4<Cfg>::WritePayload w_payload;
    typedef typename axi::axi4<Cfg>::WRespPayload b_payload;

    typedef sc_lv<axi::axi4<Cfg>::ADDR_WIDTH> Addr;
    typedef sc_lv<axi::axi4<Cfg>::DATA_WIDTH> Data;
    typedef sc_lv<axi::axi4<Cfg>::WSTRB_WIDTH> Wstrb;

    static const int bytesPerBeat = axi::axi4<Cfg>::DATA_WIDTH >> 3;

    template <class T>
    T
    static convert_from_lv(sc_lv<Wrapped<T>::width> lv) {
      Wrapped<T> result;
      Marshaller<Wrapped<T>::width> marshaller(lv);
      result.Marshall(marshaller);
      return result.val;
    }

    template <class T>
    sc_lv<Wrapped<T>::width>
    static convert_to_lv(T v) {
      Marshaller<Wrapped<T>::width> marshaller;
      Wrapped<T> wm(v);
      wm.Marshall(marshaller);
      return marshaller.GetResult();
    }

    struct axi4_signal_w_chan {
      axi4_signal_w_chan(const char *name)
        : AWVALID(nvhls_concat(name, "_AWVALID"))
        , AWREADY(nvhls_concat(name, "_AWREADY"))
        , AWLEN  (nvhls_concat(name, "_AWLEN"))
        , AWADDR (nvhls_concat(name, "_AWADDR"))

        , WVALID(nvhls_concat(name, "_WVALID"))
        , WREADY(nvhls_concat(name, "_WREADY"))
        , WDATA (nvhls_concat(name, "_WDATA"))
        , WSTRB (nvhls_concat(name, "_WSTRB"))
        , WLAST (nvhls_concat(name, "_WLAST"))

        , BVALID(nvhls_concat(name, "_BVALID"))
        , BREADY(nvhls_concat(name, "_BREADY"))
        , BRESP (nvhls_concat(name, "_BRESP"))
      {}

      SC_SIG(bool,     AWVALID);
      SC_SIG(bool,     AWREADY);
      SC_SIG(uint8,    AWLEN);
      SC_SIG(Addr,     AWADDR);

      SC_SIG(bool,     WVALID);
      SC_SIG(bool,     WREADY);
      SC_SIG(Data,     WDATA);
      SC_SIG(Wstrb,    WSTRB);
      SC_SIG(bool,     WLAST);

      SC_SIG(bool,     BVALID);
      SC_SIG(bool,     BREADY);
      SC_SIG(bool,     BRESP);
    };

    struct axi4_signal_r_chan {
      axi4_signal_r_chan(const char *name)
        : ARVALID(nvhls_concat(name, "_ARVALID"))
        , ARREADY(nvhls_concat(name, "_ARREADY"))
        , ARADDR (nvhls_concat(name, "_ARADDR"))
        , ARLEN  (nvhls_concat(name, "_ARLEN"))

        , RVALID(nvhls_concat(name, "_RVALID"))
        , RREADY(nvhls_concat(name, "_RREADY"))
        , RRESP (nvhls_concat(name, "_RRESP"))
        , RDATA (nvhls_concat(name, "_RDATA"))
      {}

      SC_SIG(bool,     ARVALID);
      SC_SIG(bool,     ARREADY);
      SC_SIG(Addr,     ARADDR);
      SC_SIG(uint8,    ARLEN);

      SC_SIG(bool,     RVALID);
      SC_SIG(bool,     RREADY);
      SC_SIG(bool,     RRESP);
      SC_SIG(Data,     RDATA);
    };

    template <Connections::connections_port_t PortType = AUTO_PORT>
    struct axi4_signal_w_master_ports {
      axi4_signal_w_master_ports(const char *name)
        : AWVALID(nvhls_concat(name, "_AWVALID"))
        , AWREADY(nvhls_concat(name, "_AWREADY"))
        , AWADDR (nvhls_concat(name, "_AWADDR"))
        , AWLEN  (nvhls_concat(name, "_AWLEN"))

        , WVALID(nvhls_concat(name, "_WVALID"))
        , WREADY(nvhls_concat(name, "_WREADY"))
        , WDATA (nvhls_concat(name, "_WDATA"))
        , WSTRB (nvhls_concat(name, "_WSTRB"))
        , WLAST (nvhls_concat(name, "_WLAST"))

        , BVALID(nvhls_concat(name, "_BVALID"))
        , BREADY(nvhls_concat(name, "_BREADY"))
        , BRESP (nvhls_concat(name, "_BRESP"))
      {}

      sc_out<bool>     CCS_INIT_S1(AWVALID);
      sc_in <bool>     CCS_INIT_S1(AWREADY);
      sc_out<Addr>     CCS_INIT_S1(AWADDR);
      sc_out<uint8>  CCS_INIT_S1(AWLEN);

      sc_out<bool>     CCS_INIT_S1(WVALID);
      sc_in <bool>     CCS_INIT_S1(WREADY);
      sc_out<Data>     CCS_INIT_S1(WDATA);
      sc_out<Wstrb>    CCS_INIT_S1(WSTRB);
      sc_out<bool>     CCS_INIT_S1(WLAST);

      sc_in <bool>     CCS_INIT_S1(BVALID);
      sc_out<bool>     CCS_INIT_S1(BREADY);
      sc_in <bool>     CCS_INIT_S1(BRESP);

      template <class C>
      void operator()(C &c) {
        AWVALID(c.AWVALID);
        AWREADY(c.AWREADY);
        AWADDR(c.AWADDR);
        AWLEN(c.AWLEN);

        WVALID(c.WVALID);
        WREADY(c.WREADY);
        WDATA(c.WDATA);
        WSTRB(c.WSTRB);
        WLAST(c.WLAST);

        BVALID(c.BVALID);
        BREADY(c.BREADY);
        BRESP(c.BRESP);
      }
    };

    template <Connections::connections_port_t PortType = AUTO_PORT>
    struct axi4_signal_w_slave_ports {
      axi4_signal_w_slave_ports(const char *name)
        : AWVALID(nvhls_concat(name, "_AWVALID"))
        , AWREADY(nvhls_concat(name, "_AWREADY"))
        , AWADDR (nvhls_concat(name, "_AWADDR"))
        , AWLEN  (nvhls_concat(name, "_AWLEN"))

        , WVALID(nvhls_concat(name, "_WVALID"))
        , WREADY(nvhls_concat(name, "_WREADY"))
        , WDATA (nvhls_concat(name, "_WDATA"))
        , WSTRB (nvhls_concat(name, "_WSTRB"))
        , WLAST (nvhls_concat(name, "_WLAST"))

        , BVALID(nvhls_concat(name, "_BVALID"))
        , BREADY(nvhls_concat(name, "_BREADY"))
        , BRESP (nvhls_concat(name, "_BRESP"))
      {}

      sc_in <bool>     CCS_INIT_S1(AWVALID);
      sc_out<bool>     CCS_INIT_S1(AWREADY);
      sc_in <Addr>     CCS_INIT_S1(AWADDR);
      sc_in <uint8>  CCS_INIT_S1(AWLEN);

      sc_in <bool>     CCS_INIT_S1(WVALID);
      sc_out<bool>     CCS_INIT_S1(WREADY);
      sc_in <Data>     CCS_INIT_S1(WDATA);
      sc_in <Wstrb>    CCS_INIT_S1(WSTRB);
      sc_in <bool>     CCS_INIT_S1(WLAST);

      sc_out<bool>     CCS_INIT_S1(BVALID);
      sc_in <bool>     CCS_INIT_S1(BREADY);
      sc_out<bool>     CCS_INIT_S1(BRESP);

      template <class C>
      void operator()(C &c) {
        AWVALID(c.AWVALID);
        AWREADY(c.AWREADY);
        AWADDR(c.AWADDR);
        AWLEN(c.AWLEN);

        WVALID(c.WVALID);
        WREADY(c.WREADY);
        WDATA(c.WDATA);
        WSTRB(c.WSTRB);
        WLAST(c.WLAST);

        BVALID(c.BVALID);
        BREADY(c.BREADY);
        BRESP(c.BRESP);
      }
    };

    template <Connections::connections_port_t PortType = AUTO_PORT>
    struct axi4_signal_r_master_ports {
      axi4_signal_r_master_ports(const char *name)
        : ARVALID(nvhls_concat(name, "_ARVALID"))
        , ARREADY(nvhls_concat(name, "_ARREADY"))
        , ARADDR (nvhls_concat(name, "_ARADDR"))
        , ARLEN  (nvhls_concat(name, "_ARLEN"))

        , RVALID(nvhls_concat(name, "_RVALID"))
        , RREADY(nvhls_concat(name, "_RREADY"))
        , RRESP (nvhls_concat(name, "_RRESP"))
        , RDATA (nvhls_concat(name, "_RDATA"))
      {}

      sc_out<bool>     ARVALID;
      sc_in <bool>     ARREADY;
      sc_out<Addr>     ARADDR;
      sc_out<uint8>    ARLEN;

      sc_in <bool>     RVALID;
      sc_out<bool>     RREADY;
      sc_in <bool>     RRESP;
      sc_in <Data>     RDATA;

      template <class C>
      void operator()(C &c) {
        ARVALID(c.ARVALID);
        ARREADY(c.ARREADY);
        ARADDR (c.ARADDR);
        ARLEN  (c.ARLEN );

        RVALID(c.RVALID);
        RREADY(c.RREADY);
        RRESP (c.RRESP );
        RDATA (c.RDATA );
      }
    };

    template <Connections::connections_port_t PortType = AUTO_PORT>
    struct axi4_signal_r_slave_ports {
      axi4_signal_r_slave_ports(const char *name)
        : ARVALID(nvhls_concat(name, "_ARVALID"))
        , ARREADY(nvhls_concat(name, "_ARREADY"))
        , ARADDR (nvhls_concat(name, "_ARADDR"))
        , ARLEN  (nvhls_concat(name, "_ARLEN"))

        , RVALID(nvhls_concat(name, "_RVALID"))
        , RREADY(nvhls_concat(name, "_RREADY"))
        , RRESP (nvhls_concat(name, "_RRESP"))
        , RDATA (nvhls_concat(name, "_RDATA"))
      {}

      sc_in <bool>     ARVALID;
      sc_out<bool>     ARREADY;
      sc_in <Addr>     ARADDR;
      sc_in <uint8>    ARLEN;

      sc_out<bool>     RVALID;
      sc_in <bool>     RREADY;
      sc_out<bool>     RRESP;
      sc_out<Data>     RDATA;

      template <class C>
      void operator()(C &c) {
        ARVALID(c.ARVALID);
        ARREADY(c.ARREADY);
        ARADDR (c.ARADDR);
        ARLEN  (c.ARLEN );

        RVALID(c.RVALID);
        RREADY(c.RREADY);
        RRESP (c.RRESP );
        RDATA (c.RDATA );
      }
    };

    template <Connections::connections_port_t PortType = AUTO_PORT>
    class axi4_signal_w_master_xactor : public sc_module
    {
    public:
      Connections::In<aw_payload, PortType>   CCS_INIT_S1(aw_port);
      Connections::In<w_payload,  PortType>   CCS_INIT_S1(w_port);
      Connections::Out<b_payload,  PortType>  CCS_INIT_S1(b_port);

      // reads aw, w payloads, writes AW and W sigs
      // reads B sigs, writes b payload

      sc_out<bool>     CCS_INIT_S1(AWVALID);
      sc_in <bool>     CCS_INIT_S1(AWREADY);
      sc_out<Addr>     CCS_INIT_S1(AWADDR);
      sc_out<uint8>  CCS_INIT_S1(AWLEN);

      sc_out<bool>     CCS_INIT_S1(WVALID);
      sc_in <bool>     CCS_INIT_S1(WREADY);
      sc_out<Data>     CCS_INIT_S1(WDATA);
      sc_out<Wstrb>    CCS_INIT_S1(WSTRB);
      sc_out<bool>     CCS_INIT_S1(WLAST);

      sc_in <bool>     CCS_INIT_S1(BVALID);
      sc_out<bool>     CCS_INIT_S1(BREADY);
      sc_in <bool>     CCS_INIT_S1(BRESP);

      SC_CTOR(axi4_signal_w_master_xactor) {
        SC_METHOD(aw_fwd);
        dont_initialize();
        sensitive << aw_port.dat << aw_port.vld;
        SC_METHOD(aw_bwd);
        dont_initialize();
        sensitive << AWREADY;
        SC_METHOD(w_fwd);
        dont_initialize();
        sensitive << w_port.dat << w_port.vld;
        SC_METHOD(w_bwd);
        dont_initialize();
        sensitive << WREADY;
        SC_METHOD(b_fwd);
        dont_initialize();
        sensitive << b_port.rdy;
        SC_METHOD(b_bwd);
        dont_initialize();
        sensitive << BVALID << BRESP;

        #ifdef CONNECTIONS_SIM_ONLY
        aw_port.disable_spawn();
        w_port.disable_spawn();
        b_port.disable_spawn();
        #endif
      }

      template <class C>
      void operator()(C &c) {
        AWVALID(c.AWVALID);
        AWREADY(c.AWREADY);
        AWADDR(c.AWADDR);
        AWLEN(c.AWLEN);

        WVALID(c.WVALID);
        WREADY(c.WREADY);
        WDATA(c.WDATA);
        WSTRB(c.WSTRB);
        WLAST(c.WLAST);

        BVALID(c.BVALID);
        BREADY(c.BREADY);
        BRESP(c.BRESP);
      }

      void aw_fwd() {
        AWVALID = aw_port.vld;
        aw_payload aw = convert_from_lv<aw_payload>(aw_port.dat);
        AWADDR = aw.addr.to_uint64();
        AWLEN  = aw.len.to_uint64();
      }
      void aw_bwd() {
        aw_port.rdy = AWREADY;
      }
      void w_fwd() {
        WVALID = w_port.vld;
        w_payload w = convert_from_lv<w_payload>(w_port.dat);

        WDATA  = w.data.to_uint64();
        WSTRB  = w.wstrb.to_uint64();
        WLAST  = w.last;
      }
      void w_bwd() {
        w_port.rdy = WREADY;
      }
      void b_fwd() {
        BREADY = b_port.rdy;
      }
      void b_bwd() {
        b_port.vld = BVALID;
        b_payload b;
        b.resp = BRESP.read();
        b_port.dat = convert_to_lv(b);
      }
    };

    template <Connections::connections_port_t PortType = AUTO_PORT>
    class axi4_signal_w_slave_xactor : public sc_module
    {
    public:
      Connections::Out<aw_payload, PortType>   CCS_INIT_S1(aw_port);
      Connections::Out<w_payload,  PortType>   CCS_INIT_S1(w_port);
      Connections::In <b_payload,  PortType>   CCS_INIT_S1(b_port);

      // reads AW and W sigs, writes aw, w payloads
      // reads b payload, writes B sigs

      sc_in <bool>     CCS_INIT_S1(AWVALID);
      sc_out<bool>     CCS_INIT_S1(AWREADY);
      sc_in <Addr>     CCS_INIT_S1(AWADDR);
      sc_in <uint8>    CCS_INIT_S1(AWLEN);

      sc_in <bool>     CCS_INIT_S1(WVALID);
      sc_out<bool>     CCS_INIT_S1(WREADY);
      sc_in <Data>     CCS_INIT_S1(WDATA);
      sc_in <Wstrb>    CCS_INIT_S1(WSTRB);
      sc_in <bool>     CCS_INIT_S1(WLAST);

      sc_out<bool>     CCS_INIT_S1(BVALID);
      sc_in <bool>     CCS_INIT_S1(BREADY);
      sc_out<bool>     CCS_INIT_S1(BRESP);

      SC_CTOR(axi4_signal_w_slave_xactor) {
        SC_METHOD(aw_fwd);
        dont_initialize();
        sensitive << AWVALID << AWADDR << AWLEN;
        SC_METHOD(aw_bwd);
        dont_initialize();
        sensitive << aw_port.rdy;
        SC_METHOD(w_fwd);
        dont_initialize();
        sensitive << WVALID << WDATA << WSTRB << WLAST;
        SC_METHOD(w_bwd);
        dont_initialize();
        sensitive << w_port.rdy;
        SC_METHOD(b_fwd);
        dont_initialize();
        sensitive << BREADY;
        SC_METHOD(b_bwd);
        dont_initialize();
        sensitive << b_port.dat << b_port.vld;

        #ifdef CONNECTIONS_SIM_ONLY
        aw_port.disable_spawn();
        w_port.disable_spawn();
        b_port.disable_spawn();
        #endif
      }

      template <class C>
      void operator()(C &c) {
        AWVALID(c.AWVALID);
        AWREADY(c.AWREADY);
        AWADDR(c.AWADDR);
        AWLEN(c.AWLEN);

        WVALID(c.WVALID);
        WREADY(c.WREADY);
        WDATA(c.WDATA);
        WSTRB(c.WSTRB);
        WLAST(c.WLAST);

        BVALID(c.BVALID);
        BREADY(c.BREADY);
        BRESP(c.BRESP);
      }

      void aw_fwd() {
        aw_port.vld = AWVALID;

        aw_payload aw;
        aw.addr = AWADDR.read().to_uint64();
        aw.len = AWLEN.read().to_uint64();
        aw_port.dat = convert_to_lv(aw);
      }
      void aw_bwd() {
        AWREADY = aw_port.rdy;
      }
      void w_fwd() {
        w_port.vld = WVALID;
        w_payload w;
        w.data = WDATA.read().to_uint64();
        w.wstrb = WSTRB.read().to_uint64();
        w.last = WLAST.read();
        w_port.dat = convert_to_lv(w);
      }
      void w_bwd() {
        WREADY = w_port.rdy;
      }
      void b_fwd() {
        b_port.rdy = BREADY;
      }
      void b_bwd() {
        BVALID = b_port.vld;

        b_payload b = convert_from_lv<b_payload>(b_port.dat);
        BRESP = b.resp;
      }
    };

    template <Connections::connections_port_t PortType = AUTO_PORT>
    class axi4_signal_r_master_xactor : public sc_module
    {
    public:
      Connections::In<ar_payload, PortType>   CCS_INIT_S1(ar_port);
      Connections::Out<r_payload,  PortType>  CCS_INIT_S1(r_port);

      // reads ar payload writes AR sigs
      // reads R sigs, writes r payload

      SC_CTOR(axi4_signal_r_master_xactor) {
        SC_METHOD(ar_fwd);
        dont_initialize();
        sensitive << ar_port.dat << ar_port.vld;
        SC_METHOD(ar_bwd);
        dont_initialize();
        sensitive << ARREADY;
        SC_METHOD(r_fwd);
        dont_initialize();
        sensitive << r_port.rdy;
        SC_METHOD(r_bwd);
        dont_initialize();
        sensitive << RVALID << RRESP << RDATA;

        #ifdef CONNECTIONS_SIM_ONLY
        ar_port.disable_spawn();
        r_port.disable_spawn();
        #endif
      }

      sc_out<bool>     ARVALID;
      sc_in <bool>     ARREADY;
      sc_out<Addr>     ARADDR;
      sc_out<uint8>  ARLEN;

      sc_in <bool>     RVALID;
      sc_out<bool>     RREADY;
      sc_in <Data>     RDATA;
      sc_in <bool>     RRESP;

      template <class C>
      void operator()(C &c) {
        ARVALID(c.ARVALID);
        ARREADY(c.ARREADY);
        ARADDR (c.ARADDR);
        ARLEN  (c.ARLEN );

        RVALID(c.RVALID);
        RREADY(c.RREADY);
        RRESP (c.RRESP );
        RDATA (c.RDATA );
      }

      void ar_fwd() {
        ARVALID = ar_port.vld;
        ar_payload ar = convert_from_lv<ar_payload>(ar_port.dat);
        ARADDR = ar.addr.to_uint64();
        ARLEN  = ar.len.to_uint64();
      }
      void ar_bwd() {
        ar_port.rdy = ARREADY;
      }
      void r_fwd() {
        RREADY = r_port.rdy;
      }
      void r_bwd() {
        r_port.vld = RVALID;
        r_payload r;
        r.resp = RRESP.read();
        r.data = RDATA.read().to_uint64();
        r_port.dat = convert_to_lv(r);
      }
    };

    template <Connections::connections_port_t PortType = AUTO_PORT>
    class axi4_signal_r_slave_xactor : public sc_module
    {
    public:

      Connections::Out<ar_payload, PortType>   CCS_INIT_S1(ar_port);
      Connections::In <r_payload,  PortType>   CCS_INIT_S1(r_port);

      sc_in <bool>     CCS_INIT_S1(ARVALID);
      sc_out<bool>     CCS_INIT_S1(ARREADY);
      sc_in <Addr>     CCS_INIT_S1(ARADDR);
      sc_in <uint8>    CCS_INIT_S1(ARLEN);

      sc_out<bool>     CCS_INIT_S1(RVALID);
      sc_in <bool>     CCS_INIT_S1(RREADY);
      sc_out<Data>     CCS_INIT_S1(RDATA);
      sc_out<bool>     CCS_INIT_S1(RRESP);


      // reads AR sigs writes ar payload
      // reads r payload writes R sigs

      SC_CTOR(axi4_signal_r_slave_xactor) {
        SC_METHOD(ar_fwd);
        dont_initialize();
        sensitive << ARVALID << ARADDR << ARLEN;
        SC_METHOD(ar_bwd);
        dont_initialize();
        sensitive << ar_port.rdy;
        SC_METHOD(r_fwd);
        dont_initialize();
        sensitive << RREADY;
        SC_METHOD(r_bwd);
        dont_initialize();
        sensitive << r_port.dat << r_port.vld;

        #ifdef CONNECTIONS_SIM_ONLY
        ar_port.disable_spawn();
        r_port.disable_spawn();
        #endif
      }

      template <class C>
      void operator()(C &c) {
        ARVALID(c.ARVALID);
        ARREADY(c.ARREADY);
        ARADDR (c.ARADDR);
        ARLEN  (c.ARLEN );

        RVALID(c.RVALID);
        RREADY(c.RREADY);
        RRESP (c.RRESP );
        RDATA (c.RDATA );
      }

      void ar_fwd() {
        ar_port.vld = ARVALID;

        ar_payload ar;
        ar.addr = ARADDR.read().to_uint64();
        ar.len = ARLEN.read().to_uint64();
        ar_port.dat = convert_to_lv(ar);
      }
      void ar_bwd() {
        ARREADY = ar_port.rdy;
      }
      void r_fwd() {
        r_port.rdy = RREADY;
      }
      void r_bwd() {
        RVALID = r_port.vld;

        r_payload r = convert_from_lv<r_payload>(r_port.dat);
        RRESP = r.resp;
        RDATA = r.data.to_uint64();
      }
    };
  };  // axi4_signal_transactor
};  // namespace axi

