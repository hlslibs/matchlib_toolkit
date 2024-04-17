// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

// Prevent redefine warnings from NVHLS
#undef CONNECTIONS_ASSERT_MSG
#undef CONNECTIONS_SIM_ONLY_ASSERT_MSG

#include "axi4_new.h"

#include "auto_gen_fields.h"
#include "mc_toolkit_utils.h"



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

      AUTO_GEN_FIELD_METHODS(apb_req, ( \
         is_write \
       , addr \
       , w \
      ) )
      //
    };

    // Define the "response" message
    struct apb_rsp : public nvhls_message {
      r_payload    r; // if req was a write, then write resp will be stored in r.resp

      AUTO_GEN_FIELD_METHODS(apb_rsp, ( \
         r \
      ) )
      //
    };

    // External typedefs - to simplify usage of these message types
    //  These let you declare the communication channel in the model that uses this APB as
    //   apb_req_chan                         CCS_INIT_S1(apb_msg_req_chan);  // Connections channel for the apb_req message
    //  Instead of the direct form
    //   Connections::Combinational<apb_req>  CCS_INIT_S1(apb_msg_req_chan);  // Connections channel for the apb_req message
    //
    typedef Connections::Combinational<apb_req> apb_req_chan;
    typedef Connections::Combinational<apb_rsp> apb_rsp_chan;

    // --------------------------------------------------------------------------------
    // INTERFACES

    // Define the signals for APB channel 
    struct apb_signals_chan{
      apb_signals_chan(const char *name)
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

#ifdef CONNECTIONS_SIM_ONLY
        req_port.disable_spawn();
        rsp_port.disable_spawn();
#endif
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
        req_port.rdy = 0;
        rsp_port.vld = 0;
        PADDR = 0;
        PWRITE = 0;
        PSTRB = 0;
        PPROT = 0;
        PENABLE = 0;
        PWDATA = 0;
        PSEL = 0;
        wait();

        apb_req req;
        apb_rsp rsp;

        typedef enum {APB_IDLE, APB_GET_RSP} apb_state_t;
        apb_state_t state = APB_IDLE;

        req_port.rdy = 1;

        while (1) {
         wait();

         switch (state) {
          case APB_IDLE:
            state = APB_IDLE;
            PSEL = 0;
            PENABLE = 0;
            if (rsp_port.rdy) {
             rsp_port.vld = 0;
            }
            if (req_port.vld) {
              req_port.rdy = 0;
              state = APB_GET_RSP;
              bits_to_type_if_needed(req, req_port.dat);
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
            }
            break;

          case APB_GET_RSP:
            state = APB_GET_RSP;
            PENABLE = 1;

            if (PREADY == 1) {
              state = APB_IDLE;
              rsp.r.data = PRDATA.read().to_uint64();
              rsp.r.resp = PSLVERR.read();
              PSEL = 0;
              PENABLE = 0;
              rsp_port.vld = 1;
              type_to_bits_if_needed(rsp_port.dat, rsp);
              req_port.rdy = 1;
            } else {
              state = APB_GET_RSP;
            }
        }
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
#ifdef CONNECTIONS_SIM_ONLY
        req_port.disable_spawn();
        rsp_port.disable_spawn();
#endif
        SC_THREAD(main);
        sensitive << clk.pos();
        async_reset_signal_is(rst_bar, false);
      }

//  #pragma hls_implicit_fsm true
      void main() {
        PRDATA = 0;
        PSLVERR = 0;
        PREADY = 0;
        req_port.vld = 0;
        rsp_port.rdy = 0;

        bool pending_read = false;
        apb_req req;
        apb_rsp rsp;

        wait();

        typedef enum {APB_IDLE, APB_GET_RSP, APB_DONE} apb_state_t;
        apb_state_t state = APB_IDLE;

        while (1) {
          wait();

          switch (state) {
          case APB_IDLE:
           if (PSEL == 0)
             state = APB_IDLE;
           else {
             req.is_write = PWRITE.read();
             req.w.data = PWDATA.read().to_uint64();
             req.w.wstrb = PSTRB.read().to_uint64();
             req.addr.addr = PADDR.read().to_uint64();
             if (req.is_write) {
              pending_read = false;
             } else {
              pending_read = true;
             }
             type_to_bits_if_needed(req_port.dat, req);
             req_port.vld = 1;
             rsp_port.rdy = 1;
             state = APB_GET_RSP;
           }
           break;
 
          case APB_GET_RSP:
            if (req_port.rdy)
              req_port.vld = 0;

            if (rsp_port.vld) {
              rsp_port.rdy = 0;
              bits_to_type_if_needed(rsp, rsp_port.dat);
            }

            if ((!rsp_port.vld) || (PENABLE == 0)) {
              state = APB_GET_RSP;
            } else {
              state = APB_DONE;
              PREADY = 1;
              if (pending_read) {
                PRDATA = rsp.r.data.to_uint64();
                pending_read = false;
              }
              PSLVERR = rsp.r.resp;  // works for both reads and writes..
            }
            break;

          case APB_DONE:
           PREADY = 0;
           PSLVERR = 0;
           state = APB_IDLE;
           break;
        }
       }
      }
    };

    template <class C>
    static void apb_master_rw_reset(C& chan) {
        chan.PADDR = 0;
        chan.PWRITE = 0;
        chan.PSTRB = 0;
        chan.PPROT = 0;
        chan.PENABLE = 0;
        chan.PWDATA = 0;
        chan.PSEL = 0;
    }

// #pragma design modulario <out> 
    template <class C>
    void apb_master_rw(C& chan, const apb_req& req, apb_rsp& rsp) {
      apb_master_rw_reset(chan);

      // SETUP state
      chan.PSEL = 1; 
      chan.PENABLE = 0;
      chan.PADDR = req.addr.addr.to_uint64();
      chan.PWRITE = req.is_write;
      if (req.is_write) {
        chan.PWDATA = req.w.data.to_uint64();
        chan.PSTRB = req.w.wstrb.to_uint64();
      } else {
        chan.PWDATA = 0;
        chan.PSTRB = 0;
      }

      while (1) {
        wait();
        chan.PENABLE = 1;

        if (chan.PREADY == 1) {
              rsp.r.data = chan.PRDATA.read().to_uint64();
              rsp.r.resp = chan.PSLVERR.read();
              chan.PSEL = 0; 
              chan.PENABLE = 0;
              break;
        }
      }
    }
  };  // apb_transactor
};  // namespace apb
