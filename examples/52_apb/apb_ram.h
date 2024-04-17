// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include "apb_transactor.h"

typedef apb::apb_transactor<axi::cfg::lite> local_apb;

class ram : public sc_module, public local_apb
{
public:
  sc_in<bool> CCS_INIT_S1(clk);
  sc_in<bool> CCS_INIT_S1(rst_bar);

  apb_slave_xactor<>   CCS_INIT_S1(slave0_xactor);
  apb_slave_ports<> CCS_INIT_S1(slave0_ports);
  apb_req_chan CCS_INIT_S1(req_chan);
  apb_rsp_chan CCS_INIT_S1(rsp_chan);

  static const int sz = 0x10000; // size in axi_cfg::dataWidth words

  typedef ac_int<axi_cfg::dataWidth, false> arr_t;
  arr_t *array {0};

  SC_CTOR(ram) {
    array = new arr_t[sz];

    SC_THREAD(slave_process);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);

    for (int i=0; i < sz; i++) {
      array[i] = i * bytesPerBeat;
    }

    slave0_xactor.clk(clk);
    slave0_xactor.rst_bar(rst_bar);
    slave0_xactor.req_port(req_chan);
    slave0_xactor.rsp_port(rsp_chan);
    slave0_xactor(slave0_ports);
  }

  ac_int<axi_cfg::dataWidth, false> debug_read_addr(uint32_t addr) {
    if (addr >= (sz * bytesPerBeat)) {
      SC_REPORT_ERROR("ram", "invalid addr");
      return 0;
    }

    return (array[addr / bytesPerBeat]);
  }

  void slave_process() {
    req_chan.ResetRead();
    rsp_chan.ResetWrite();

    wait();

    while (1) {
      apb_req req = req_chan.Pop();

      // CCS_LOG("ram addr: " << std::hex << req.addr.addr << " is_write: " << req.is_write);

      apb_rsp rsp;

      if (req.addr.addr >= (sz * bytesPerBeat)) {
        SC_REPORT_ERROR("ram", "invalid addr");
        rsp.r.resp = Enc::XRESP::SLVERR;
      } else if (!req.is_write) {
        rsp.r.data = array[req.addr.addr / bytesPerBeat];
        // CCS_LOG("read data: " << std::hex << rsp.r.data);
      } else if  (req.is_write) {
        decltype(req.w.wstrb) all_on{~0};

        if (req.w.wstrb == all_on) {
          array[req.addr.addr / bytesPerBeat] = req.w.data.to_uint64();
          // CCS_LOG("write data: " << std::hex << req.w.data.to_uint64());
        } else {
          CCS_LOG("write strobe enabled");
          arr_t orig  = array[req.addr.addr / bytesPerBeat];
          arr_t data = req.w.data.to_uint64();

#pragma unroll
          for (int i=0; i<WSTRB_WIDTH; i++)
            if (req.w.wstrb[i]) {
              orig = nvhls::set_slc(orig, nvhls::get_slc<8>(data, (i*8)), (i*8));
            }

          array[req.addr.addr / bytesPerBeat] = orig;
        }
      }

      rsp_chan.Push(rsp);
    }
  }
};

