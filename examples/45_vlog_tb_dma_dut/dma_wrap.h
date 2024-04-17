// Auto generated on: Tue Oct 10 15:48:02 2023

#include "dma.h"

extern sc_trace_file* trace_file_ptr;

class dma_wrap : public sc_module {
public:
  dma CCS_INIT_S1(dma_inst);

  decltype(dma_inst.clk)   CCS_INIT_S1(clk);
  decltype(dma_inst.rst_bar)   CCS_INIT_S1(rst_bar);
  decltype(dma_inst.r_master0.ar.rdy)   CCS_INIT_S1(r_master0_ar_rdy);
  decltype(dma_inst.r_master0.ar.vld)   CCS_INIT_S1(r_master0_ar_vld);
  decltype(dma_inst.r_master0.ar.dat)   CCS_INIT_S1(r_master0_ar_dat);
  decltype(dma_inst.r_master0.r.rdy)   CCS_INIT_S1(r_master0_r_rdy);
  decltype(dma_inst.r_master0.r.vld)   CCS_INIT_S1(r_master0_r_vld);
  decltype(dma_inst.r_master0.r.dat)   CCS_INIT_S1(r_master0_r_dat);
  decltype(dma_inst.w_master0.aw.rdy)   CCS_INIT_S1(w_master0_aw_rdy);
  decltype(dma_inst.w_master0.aw.vld)   CCS_INIT_S1(w_master0_aw_vld);
  decltype(dma_inst.w_master0.aw.dat)   CCS_INIT_S1(w_master0_aw_dat);
  decltype(dma_inst.w_master0.w.rdy)   CCS_INIT_S1(w_master0_w_rdy);
  decltype(dma_inst.w_master0.w.vld)   CCS_INIT_S1(w_master0_w_vld);
  decltype(dma_inst.w_master0.w.dat)   CCS_INIT_S1(w_master0_w_dat);
  decltype(dma_inst.w_master0.b.rdy)   CCS_INIT_S1(w_master0_b_rdy);
  decltype(dma_inst.w_master0.b.vld)   CCS_INIT_S1(w_master0_b_vld);
  decltype(dma_inst.w_master0.b.dat)   CCS_INIT_S1(w_master0_b_dat);
  decltype(dma_inst.r_slave0.ar.rdy)   CCS_INIT_S1(r_slave0_ar_rdy);
  decltype(dma_inst.r_slave0.ar.vld)   CCS_INIT_S1(r_slave0_ar_vld);
  decltype(dma_inst.r_slave0.ar.dat)   CCS_INIT_S1(r_slave0_ar_dat);
  decltype(dma_inst.r_slave0.r.rdy)   CCS_INIT_S1(r_slave0_r_rdy);
  decltype(dma_inst.r_slave0.r.vld)   CCS_INIT_S1(r_slave0_r_vld);
  decltype(dma_inst.r_slave0.r.dat)   CCS_INIT_S1(r_slave0_r_dat);
  decltype(dma_inst.w_slave0.aw.rdy)   CCS_INIT_S1(w_slave0_aw_rdy);
  decltype(dma_inst.w_slave0.aw.vld)   CCS_INIT_S1(w_slave0_aw_vld);
  decltype(dma_inst.w_slave0.aw.dat)   CCS_INIT_S1(w_slave0_aw_dat);
  decltype(dma_inst.w_slave0.w.rdy)   CCS_INIT_S1(w_slave0_w_rdy);
  decltype(dma_inst.w_slave0.w.vld)   CCS_INIT_S1(w_slave0_w_vld);
  decltype(dma_inst.w_slave0.w.dat)   CCS_INIT_S1(w_slave0_w_dat);
  decltype(dma_inst.w_slave0.b.rdy)   CCS_INIT_S1(w_slave0_b_rdy);
  decltype(dma_inst.w_slave0.b.vld)   CCS_INIT_S1(w_slave0_b_vld);
  decltype(dma_inst.w_slave0.b.dat)   CCS_INIT_S1(w_slave0_b_dat);
  decltype(dma_inst.dma_done.rdy)   CCS_INIT_S1(dma_done_rdy);
  decltype(dma_inst.dma_done.vld)   CCS_INIT_S1(dma_done_vld);
  decltype(dma_inst.dma_done.dat)   CCS_INIT_S1(dma_done_dat);
  decltype(dma_inst.dma_dbg.rdy)   CCS_INIT_S1(dma_dbg_rdy);
  decltype(dma_inst.dma_dbg.vld)   CCS_INIT_S1(dma_dbg_vld);
  decltype(dma_inst.dma_dbg.dat)   CCS_INIT_S1(dma_dbg_dat);

  sc_clock connections_clk;
  sc_event check_event;

  virtual void start_of_simulation() {
    Connections::get_sim_clk().add_clock_alias(
      connections_clk.posedge_event(), clk.posedge_event());
  }

  SC_CTOR(dma_wrap) 
  : connections_clk("connections_clk", 10, SC_NS, 0.5,0,SC_NS,true)
  {
    SC_METHOD(check_clock);
    sensitive << connections_clk << clk;

    SC_METHOD(check_event_method);
    sensitive << check_event;

    trace_file_ptr = sc_create_vcd_trace_file("trace");
    trace_hierarchy(this, trace_file_ptr);

    dma_inst.clk(clk);
    dma_inst.rst_bar(rst_bar);
    dma_inst.r_master0.ar.rdy(r_master0_ar_rdy);
    dma_inst.r_master0.ar.vld(r_master0_ar_vld);
    dma_inst.r_master0.ar.dat(r_master0_ar_dat);
    dma_inst.r_master0.r.rdy(r_master0_r_rdy);
    dma_inst.r_master0.r.vld(r_master0_r_vld);
    dma_inst.r_master0.r.dat(r_master0_r_dat);
    dma_inst.w_master0.aw.rdy(w_master0_aw_rdy);
    dma_inst.w_master0.aw.vld(w_master0_aw_vld);
    dma_inst.w_master0.aw.dat(w_master0_aw_dat);
    dma_inst.w_master0.w.rdy(w_master0_w_rdy);
    dma_inst.w_master0.w.vld(w_master0_w_vld);
    dma_inst.w_master0.w.dat(w_master0_w_dat);
    dma_inst.w_master0.b.rdy(w_master0_b_rdy);
    dma_inst.w_master0.b.vld(w_master0_b_vld);
    dma_inst.w_master0.b.dat(w_master0_b_dat);
    dma_inst.r_slave0.ar.rdy(r_slave0_ar_rdy);
    dma_inst.r_slave0.ar.vld(r_slave0_ar_vld);
    dma_inst.r_slave0.ar.dat(r_slave0_ar_dat);
    dma_inst.r_slave0.r.rdy(r_slave0_r_rdy);
    dma_inst.r_slave0.r.vld(r_slave0_r_vld);
    dma_inst.r_slave0.r.dat(r_slave0_r_dat);
    dma_inst.w_slave0.aw.rdy(w_slave0_aw_rdy);
    dma_inst.w_slave0.aw.vld(w_slave0_aw_vld);
    dma_inst.w_slave0.aw.dat(w_slave0_aw_dat);
    dma_inst.w_slave0.w.rdy(w_slave0_w_rdy);
    dma_inst.w_slave0.w.vld(w_slave0_w_vld);
    dma_inst.w_slave0.w.dat(w_slave0_w_dat);
    dma_inst.w_slave0.b.rdy(w_slave0_b_rdy);
    dma_inst.w_slave0.b.vld(w_slave0_b_vld);
    dma_inst.w_slave0.b.dat(w_slave0_b_dat);
    dma_inst.dma_done.rdy(dma_done_rdy);
    dma_inst.dma_done.vld(dma_done_vld);
    dma_inst.dma_done.dat(dma_done_dat);
    dma_inst.dma_dbg.rdy(dma_dbg_rdy);
    dma_inst.dma_dbg.vld(dma_dbg_vld);
    dma_inst.dma_dbg.dat(dma_dbg_dat);
  }

  void check_clock() { check_event.notify(2, SC_PS);} // Let SC and Vlog delta cycles settle.

  void check_event_method() {
    if (connections_clk.read() == clk.read()) return;
    CCS_LOG("clocks misaligned!:"  << connections_clk.read() << " " << clk.read());
  }
};
