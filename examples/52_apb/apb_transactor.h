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
 *  Copyright 2020 Siemens                                                *
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

// Include matchlib/cmod/include/axi/axi4.h for
// standard AXI4 bus configurations.
#include "axi/axi4.h"

namespace apb
{

  template <typename Cfg>
  class apb_transactor : public axi::axi4<Cfg>
  {
  public:

    // --------------------------------------------------------------------------------
    // CONVENIENCE TYPEDEFS - Used in model that instantiates this

    typedef Cfg axi_cfg;

    typedef typename axi::axi4<Cfg>::AddrPayload    addr_payload;
    typedef typename axi::axi4<Cfg>::ReadPayload    r_payload;
    typedef typename axi::axi4<Cfg>::WritePayload   w_payload;
    typedef typename axi::axi4<Cfg>::WRespPayload   b_payload;

    typedef sc_lv<axi::axi4<Cfg>::ADDR_WIDTH>       Addr;
    typedef sc_lv<axi::axi4<Cfg>::DATA_WIDTH>       Data;
    typedef sc_lv<axi::axi4<Cfg>::WSTRB_WIDTH>      Wstrb;
    typedef sc_lv<3>                                Prot_t;

    // local configuration settings - compile-type value computation
    static const int bytesPerBeat = axi::axi4<Cfg>::DATA_WIDTH >> 3;

    // --------------------------------------------------------------------------------
    // MESSAGE STRUCTURES

    // Define the "request" message
    struct apb_req : public nvhls_message {
      bool         is_write { false };
      addr_payload addr;
      w_payload    w;

      static const unsigned int width = 1 + addr_payload::width + w_payload::width;
      template <unsigned int Size> void Marshall(Marshaller<Size> &m) {
        m &is_write;
        m &addr;
        m &w;
      }
      inline friend void sc_trace(sc_trace_file *tf, const apb_req &v, const std::string &NAME ) {
        sc_trace(tf,v.is_write,  NAME + ".is_write");
        sc_trace(tf,v.addr,  NAME + ".addr");
        sc_trace(tf,v.w,  NAME + ".w");
      }
      inline friend std::ostream &operator<<(ostream &os, const apb_req &rhs) {
        os << rhs.is_write << " ";
        os << rhs.addr << " ";
        os << rhs.w << " ";
        return os;
      }
    };

    // Define the "response" message
    struct apb_rsp : public nvhls_message {
      r_payload    r; // if req was a write, then write resp will be stored in r.resp

      static const unsigned int width = r_payload::width;
      template <unsigned int Size> void Marshall(Marshaller<Size> &m) {
        m &r;
      }
      inline friend void sc_trace(sc_trace_file *tf, const apb_rsp &v, const std::string &NAME ) {
        sc_trace(tf,v.r,  NAME + ".r");
      }
      inline friend std::ostream &operator<<(ostream &os, const apb_rsp &rhs) {
        os << rhs.r << " ";
        return os;
      }
    };

    // External typedefs - to simplify usage of these message types
    //  These let you declare the communication channel in the model that uses this APB as
    //   apb_req_chan                         CCS_INIT_S1(apb_msg_req_chan);  // Connections channel for the apb_req message
    //  Instead of the direct form
    //   Connections::Combinational<apb_req>  CCS_INIT_S1(apb_msg_req_chan);  // Connections channel for the apb_req message
    //  But note that the direct form make the code similar to the C++ ac_channel style:
    //   ac_channel<type>                     object
    //
    typedef Connections::Combinational<apb_req> apb_req_chan;
    typedef Connections::Combinational<apb_rsp> apb_rsp_chan;

    // --------------------------------------------------------------------------------
    // INTERFACES

    // Define the signal interface for an APB connection channel (similar to a SystemVerilog Interface)
    struct apb_signals_ifc {
      apb_signals_ifc(const char *name)
        : PSEL(nvhls_concat(name, "_PSEL"))
        , PADDR(nvhls_concat(name, "_PADDR"))
        , PWRITE(nvhls_concat(name, "_PWRITE"))
        , PENABLE(nvhls_concat(name, "_PENABLE"))
        , PWDATA(nvhls_concat(name, "_PWDATA"))
        , PSTRB(nvhls_concat(name, "_PSTRB"))
        , PPROT(nvhls_concat(name, "_PPROT"))
        , PRDATA(nvhls_concat(name, "_PRDATA"))
        , PSLVERR(nvhls_concat(name, "_PSLVERR"))
        , PREADY(nvhls_concat(name, "_PREADY"))
      {}

      SC_SIG(bool,     PSEL);
      SC_SIG(Addr,     PADDR);
      SC_SIG(bool,     PWRITE);
      SC_SIG(bool,     PENABLE);
      SC_SIG(Data,     PWDATA);
      SC_SIG(Wstrb,    PSTRB);
      SC_SIG(Prot_t,   PPROT);
      SC_SIG(Data,     PRDATA);
      SC_SIG(bool,     PSLVERR);
      SC_SIG(bool,     PREADY);
    };

    // Define the ports (directionality) for an APB Master
    template <Connections::connections_port_t PortType = AUTO_PORT>
    struct apb_master_ports {
      apb_master_ports(const char *name)
        : PSEL(nvhls_concat(name, "_PSEL"))
        , PADDR(nvhls_concat(name, "_PADDR"))
        , PWRITE(nvhls_concat(name, "_PWRITE"))
        , PENABLE(nvhls_concat(name, "_PENABLE"))
        , PWDATA(nvhls_concat(name, "_PWDATA"))
        , PSTRB(nvhls_concat(name, "_PSTRB"))
        , PPROT(nvhls_concat(name, "_PPROT"))
        , PRDATA(nvhls_concat(name, "_PRDATA"))
        , PSLVERR(nvhls_concat(name, "_PSLVERR"))
        , PREADY(nvhls_concat(name, "_PREADY"))
      {}

      sc_out<bool> PSEL;
      sc_out<Addr> PADDR;
      sc_out<bool> PWRITE;
      sc_out<bool> PENABLE;
      sc_out<Data> PWDATA;
      sc_out<Wstrb> PSTRB;
      sc_out<Prot_t> PPROT;
      sc_in<Data>  PRDATA;
      sc_in<bool>  PSLVERR;
      sc_in<bool>  PREADY;

      template <class C>
      void operator()(C &c) {
        PADDR(c.PADDR);
        PWRITE(c.PWRITE);
        PENABLE(c.PENABLE);
        PSEL(c.PSEL);
        PWDATA(c.PWDATA);
        PSTRB(c.PSTRB);
        PPROT(c.PPROT);
        PRDATA(c.PRDATA);
        PSLVERR(c.PSLVERR);
        PREADY(c.PREADY);
      }
    };

    // Define the ports (directionality) for an APB Slave
    template <Connections::connections_port_t PortType = AUTO_PORT>
    struct apb_slave_ports {
      apb_slave_ports(const char *name)
        : PADDR(nvhls_concat(name, "_PADDR"))
        , PWRITE(nvhls_concat(name, "_PWRITE"))
        , PENABLE(nvhls_concat(name, "_PENABLE"))
        , PSEL(nvhls_concat(name, "_PSEL"))
        , PWDATA(nvhls_concat(name, "_PWDATA"))
        , PSTRB(nvhls_concat(name, "_PSTRB"))
        , PPROT(nvhls_concat(name, "_PPROT"))
        , PRDATA(nvhls_concat(name, "_PRDATA"))
        , PSLVERR(nvhls_concat(name, "_PSLVERR"))
        , PREADY(nvhls_concat(name, "_PREADY"))
      {}

      sc_in<Addr> PADDR;
      sc_in<bool> PWRITE;
      sc_in<bool> PENABLE;
      sc_in<bool> PSEL;
      sc_in<Data> PWDATA;
      sc_in<Wstrb> PSTRB;
      sc_in<Prot_t> PPROT;
      sc_out<Data> PRDATA;
      sc_out<bool> PSLVERR;
      sc_out<bool> PREADY;

      template <class C>
      void operator()(C &c) {
        PADDR(c.PADDR);
        PWRITE(c.PWRITE);
        PENABLE(c.PENABLE);
        PSEL(c.PSEL);
        PWDATA(c.PWDATA);
        PSTRB(c.PSTRB);
        PPROT(c.PPROT);
        PRDATA(c.PRDATA);
        PSLVERR(c.PSLVERR);
        PREADY(c.PREADY);
      }
    };

    // --------------------------------------------------------------------------------
    // APB Master Interface

    template <Connections::connections_port_t PortType = AUTO_PORT>
    class apb_master_xactor : public sc_module
    {
    public:
      sc_in<bool> CCS_INIT_S1(clk);
      sc_in<bool> CCS_INIT_S1(rst_bar);

      Connections::In<apb_req, PortType>   CCS_INIT_S1(req_port);
      Connections::Out<apb_rsp, PortType>  CCS_INIT_S1(rsp_port);
      sc_out<Addr>     CCS_INIT_S1(PADDR);
      sc_out<bool>     CCS_INIT_S1(PWRITE);
      sc_out<bool>     CCS_INIT_S1(PENABLE);
      sc_out<bool>     CCS_INIT_S1(PSEL);
      sc_out<Data>     CCS_INIT_S1(PWDATA);
      sc_out<Wstrb>    CCS_INIT_S1(PSTRB);
      sc_out<Prot_t>   CCS_INIT_S1(PPROT);
      sc_in<Data>      CCS_INIT_S1(PRDATA);
      sc_in<bool>      CCS_INIT_S1(PSLVERR);
      sc_in<bool>      CCS_INIT_S1(PREADY);

      SC_CTOR(apb_master_xactor) {
        SC_THREAD(main);
        sensitive << clk.pos();
        async_reset_signal_is(rst_bar, false);
      }

      template <class C>
      void operator()(C &c) {
        PADDR(c.PADDR);
        PSEL(c.PSEL);
        PWRITE(c.PWRITE);
        PENABLE(c.PENABLE);
        PWDATA(c.PWDATA);
        PSTRB(c.PSTRB);
        PPROT(c.PPROT);
        PRDATA(c.PRDATA);
        PSLVERR(c.PSLVERR);
        PREADY(c.PREADY);
      }

      void main() {
        req_port.Reset();
        rsp_port.Reset();
        PADDR = 0;
        PWRITE = 0;
        PSTRB = 0;
        PPROT = 0;
        PENABLE = 0;
        PWDATA = 0;
        PSEL = 0;
        wait();

        bool got_req = false;
        bool got_rsp = false;
        apb_req req;
        apb_rsp rsp;

        while (1) {
          // See ARM APB spec for state description.
          do {
            // IDLE state
            wait();
            if (!got_req) {
              got_req = req_port.PopNB(req);
              /*
              if (got_req)
                CCS_LOG("master pop req: " << std::hex << req.addr.addr << " " << req.is_write << " " << req.w.data);
              */
            }

            if (got_rsp)
              if (rsp_port.PushNB(rsp)) {
                got_rsp = 0;
                // CCS_LOG("master pushed resp: " << std::hex << rsp.r.data);
              }

          } while (got_rsp || !got_req);

          do {
            wait();
            if (!got_req) {
              got_req = req_port.PopNB(req);
              /*
              if (got_req)
                CCS_LOG("master pop req: " << std::hex << req.addr.addr << " " << req.is_write << " " << req.w.data);
              */
            }

            if (got_rsp) {
              if (rsp_port.PushNB(rsp)) {
                got_rsp = 0;
                // CCS_LOG("master pushed resp: " << std::hex << rsp.r.data);
              }
            }
            // SETUP state
            PSEL = 1;
            PENABLE = 0;
            PADDR = req.addr.addr.to_uint64();
            PWRITE = req.is_write;
            if (req.is_write) {
              PWDATA = req.w.data.to_uint64();
              PSTRB = req.w.wstrb.to_uint64();
            } else {
              PWDATA = 0;
              PSTRB = 0;
            }

            wait();

            got_req = 0;
            PSEL = 1;
            PENABLE = 1;

            do {
              wait();
            } while (PREADY == 0);
            // ACCESS state

            rsp.r.data = PRDATA.read().to_uint64();
            rsp.r.resp = PSLVERR.read();
            got_rsp = 1;
            PSEL = 0;
            PENABLE = 0;
          } while (!got_rsp && got_req);
        }
      }
    };

    // --------------------------------------------------------------------------------
    // APB Slave Interface

    template <Connections::connections_port_t PortType = AUTO_PORT>
    class apb_slave_xactor : public sc_module
    {
    public:
      sc_in<bool> CCS_INIT_S1(clk);
      sc_in<bool> CCS_INIT_S1(rst_bar);

      Connections::Out<apb_req, PortType> CCS_INIT_S1(req_port);
      Connections::In<apb_rsp, PortType>  CCS_INIT_S1(rsp_port);
      sc_in<Addr>       CCS_INIT_S1(PADDR);
      sc_in<bool>       CCS_INIT_S1(PWRITE);
      sc_in<bool>       CCS_INIT_S1(PENABLE);
      sc_in<bool>       CCS_INIT_S1(PSEL);
      sc_in<Data>       CCS_INIT_S1(PWDATA);
      sc_in<Wstrb >     CCS_INIT_S1(PSTRB);
      sc_in<Prot_t >    CCS_INIT_S1(PPROT);
      sc_out<Data>      CCS_INIT_S1(PRDATA);
      sc_out<bool>      CCS_INIT_S1(PSLVERR);
      sc_out<bool>      CCS_INIT_S1(PREADY);

      template <class C>
      void operator()(C &c) {
        PADDR(c.PADDR);
        PWRITE(c.PWRITE);
        PENABLE(c.PENABLE);
        PSEL(c.PSEL);
        PWDATA(c.PWDATA);
        PSTRB(c.PSTRB);
        PPROT(c.PPROT);
        PRDATA(c.PRDATA);
        PSLVERR(c.PSLVERR);
        PREADY(c.PREADY);
      }

      SC_CTOR(apb_slave_xactor) {
        SC_THREAD(main);
        sensitive << clk.pos();
        async_reset_signal_is(rst_bar, false);
      }

      void main() {
        req_port.Reset();
        rsp_port.Reset();
        PRDATA = 0;
        PSLVERR = 0;
        PREADY = 0;

        bool got_req = false;
        bool got_rsp = false;
        bool pending_read = false;
        apb_req req;
        apb_rsp rsp;

        wait();

        while (1) {
          // See ARM APB spec for state description.

          // State: apb_idle
          do {
            wait();
          } while (PSEL == 0);

          req.is_write = PWRITE.read();
          req.w.data = PWDATA.read().to_uint64();
          req.w.wstrb = PSTRB.read().to_uint64();
          req.addr.addr = PADDR.read().to_uint64();
          got_req = 1;

          if (req.is_write) {
            pending_read = false;
          } else {
            pending_read = true;
          }

          do {
            if (got_req)
              if (req_port.PushNB(req)) {
                // CCS_LOG("slave push req " << std::hex << req.addr.addr << " " << req.is_write << " " << req.w.data);
                got_req = 0;
              }
            wait();
          } while (got_req);

          // State: apb_setup
          do {
            wait();
            if (!got_rsp) {
              got_rsp = rsp_port.PopNB(rsp);
              // if (got_rsp) CCS_LOG("slave pop rsp data " << std::hex << rsp.r.data);
            }
          } while ((PENABLE == 0) || !got_rsp);

          // State: apb_access
          PREADY = 1;
          if (pending_read) {
            PRDATA = rsp.r.data.to_uint64();
            pending_read = false;
          }

          PSLVERR = rsp.r.resp;  // works for both reads and writes..
          wait();
          PREADY = 0;
          PSLVERR = 0;
          got_rsp = 0;
        }
      }
    };

  };  // apb_transactor
};  // namespace apb

