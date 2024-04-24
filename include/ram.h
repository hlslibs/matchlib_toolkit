// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include "axi4_segment.h"
#include "auto_gen_port_info.h"

typedef axi::axi4_segment<axi::cfg::standard> local_axi;

#ifdef USE_EXTENDED_ARRAY
#include <extended_array.h>
#endif

#include <ac_array_1D.h>

/**
 *  \brief A simple RAM module with 1 axi4 read slave and 1 axi4 write slave
*/

class ram : public sc_module, public local_axi
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);
  r_slave<AUTO_PORT>     CCS_INIT_S1(r_slave0);
  w_slave<AUTO_PORT>     CCS_INIT_S1(w_slave0);

  AUTO_GEN_PORT_INFO(ram, ( \
    clk \
  , rst_bar \
  , r_slave0 \
  , w_slave0 \
  ) )
  //

  static const int sz = 0x10000; // size in axi_cfg::dataWidth words

  typedef NVUINTW(axi_cfg::dataWidth) arr_t;
#ifdef USE_EXTENDED_ARRAY
  extended_array<arr_t, sz>* array{0};
#else
  ac_array_1D<arr_t, sz>* array{0};
#endif

  SC_HAS_PROCESS(ram);
  ram(sc_module_name _nm, std::string log_nm="", bool use_time_stamp=0) {
#ifdef USE_EXTENDED_ARRAY
    if (log_nm != "")
      log_nm = std::string(this->name()) + "_" + log_nm;
    
    array = new extended_array<arr_t, sz>(log_nm, use_time_stamp);
#else
    array = new ac_array_1D<arr_t, sz>();
#endif

    SC_THREAD(slave_r_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    SC_THREAD(slave_w_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    for (int i=0; i < sz; i++)
    { (*array)[i] = arr_t(i * bytesPerBeat); }
  }

  ~ram() {
    delete array;
  }

  NVUINTW(axi_cfg::dataWidth) debug_read_addr(uint32_t addr) {
    if (addr >= (sz * bytesPerBeat)) {
      SC_REPORT_ERROR("ram", "invalid addr");
      return 0;
    }

    return ((*array)[addr / bytesPerBeat]);
  }

  void slave_r_process() {
    r_slave0.reset();

    wait();

    while (1) {
      ar_payload ar;
      r_slave0.start_multi_read(ar);

      CCS_LOG("ram read  addr: " << std::hex << ar.addr << " len: " << ar.len);

      while (1) {
        r_payload r;

        if (ar.addr >= (sz * bytesPerBeat)) {
          SC_REPORT_ERROR("ram", "invalid addr");
          r.resp = Enc::XRESP::SLVERR;
        } else {
          r.data = (*array)[ar.addr / bytesPerBeat];
        }

        if (!r_slave0.next_multi_read(ar, r)) { break; }
      }
    }
  }

  void slave_w_process() {
    w_slave0.reset();
    wait();

    while (1) {
      aw_payload aw;
      b_payload b;

      w_slave0.start_multi_write(aw, b);

      CCS_LOG("ram write addr: " << std::hex << aw.addr << " len: " << aw.len);

      while (1) {
        w_payload w = w_slave0.w.Pop();

        if (aw.addr >= (sz * bytesPerBeat)) {
          SC_REPORT_ERROR("ram", "invalid addr");
          b.resp = Enc::XRESP::SLVERR;
        } else {
          decltype(w.wstrb) all_on{~0};

          if (w.wstrb == all_on) {
            (*array)[aw.addr / bytesPerBeat] = w.data.to_uint64();
          } else {
            CCS_LOG("write strobe enabled");
            arr_t orig  = (*array)[aw.addr / bytesPerBeat];
            arr_t wdata = w.data.to_uint64();

#pragma hls_unroll
            for (int i=0; i<WSTRB_WIDTH; i++)
              if (w.wstrb[i]) {
                orig = nvhls::set_slc(orig, nvhls::get_slc<8>(wdata, (i*8)), (i*8));
              }

            (*array)[aw.addr / bytesPerBeat] = orig;
          }
        }

        if (!w_slave0.next_multi_write(aw)) { break; }
      }

      w_slave0.b.Push(b);
    }
  }
};

