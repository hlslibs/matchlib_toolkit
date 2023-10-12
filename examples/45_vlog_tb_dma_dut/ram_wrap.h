// Auto generated on: Tue Oct 10 15:48:02 2023

#include "ram.h"

class ram_wrap : public sc_module {
public:
  ram CCS_INIT_S1(ram_inst);

  decltype(ram_inst.clk)   CCS_INIT_S1(clk);
  decltype(ram_inst.rst_bar)   CCS_INIT_S1(rst_bar);
  decltype(ram_inst.r_slave0.ar.rdy)   CCS_INIT_S1(r_slave0_ar_rdy);
  decltype(ram_inst.r_slave0.ar.vld)   CCS_INIT_S1(r_slave0_ar_vld);
  decltype(ram_inst.r_slave0.ar.dat)   CCS_INIT_S1(r_slave0_ar_dat);
  decltype(ram_inst.r_slave0.r.rdy)   CCS_INIT_S1(r_slave0_r_rdy);
  decltype(ram_inst.r_slave0.r.vld)   CCS_INIT_S1(r_slave0_r_vld);
  decltype(ram_inst.r_slave0.r.dat)   CCS_INIT_S1(r_slave0_r_dat);
  decltype(ram_inst.w_slave0.aw.rdy)   CCS_INIT_S1(w_slave0_aw_rdy);
  decltype(ram_inst.w_slave0.aw.vld)   CCS_INIT_S1(w_slave0_aw_vld);
  decltype(ram_inst.w_slave0.aw.dat)   CCS_INIT_S1(w_slave0_aw_dat);
  decltype(ram_inst.w_slave0.w.rdy)   CCS_INIT_S1(w_slave0_w_rdy);
  decltype(ram_inst.w_slave0.w.vld)   CCS_INIT_S1(w_slave0_w_vld);
  decltype(ram_inst.w_slave0.w.dat)   CCS_INIT_S1(w_slave0_w_dat);
  decltype(ram_inst.w_slave0.b.rdy)   CCS_INIT_S1(w_slave0_b_rdy);
  decltype(ram_inst.w_slave0.b.vld)   CCS_INIT_S1(w_slave0_b_vld);
  decltype(ram_inst.w_slave0.b.dat)   CCS_INIT_S1(w_slave0_b_dat);

  sc_clock connections_clk;
  sc_event check_event;

  virtual void start_of_simulation() {
    Connections::get_sim_clk().add_clock_alias(
      connections_clk.posedge_event(), clk.posedge_event());
  }

  SC_CTOR(ram_wrap) 
  : connections_clk("connections_clk", 10, SC_NS, 0.5,0,SC_NS,true)
  {
    SC_METHOD(check_clock);
    sensitive << connections_clk << clk;

    SC_METHOD(check_event_method);
    sensitive << check_event;

    ram_inst.clk(clk);
    ram_inst.rst_bar(rst_bar);
    ram_inst.r_slave0.ar.rdy(r_slave0_ar_rdy);
    ram_inst.r_slave0.ar.vld(r_slave0_ar_vld);
    ram_inst.r_slave0.ar.dat(r_slave0_ar_dat);
    ram_inst.r_slave0.r.rdy(r_slave0_r_rdy);
    ram_inst.r_slave0.r.vld(r_slave0_r_vld);
    ram_inst.r_slave0.r.dat(r_slave0_r_dat);
    ram_inst.w_slave0.aw.rdy(w_slave0_aw_rdy);
    ram_inst.w_slave0.aw.vld(w_slave0_aw_vld);
    ram_inst.w_slave0.aw.dat(w_slave0_aw_dat);
    ram_inst.w_slave0.w.rdy(w_slave0_w_rdy);
    ram_inst.w_slave0.w.vld(w_slave0_w_vld);
    ram_inst.w_slave0.w.dat(w_slave0_w_dat);
    ram_inst.w_slave0.b.rdy(w_slave0_b_rdy);
    ram_inst.w_slave0.b.vld(w_slave0_b_vld);
    ram_inst.w_slave0.b.dat(w_slave0_b_dat);
  }

  void check_clock() { check_event.notify(2, SC_PS);} // Let SC and Vlog delta cycles settle.

  void check_event_method() {
    if (connections_clk.read() == clk.read()) return;
    CCS_LOG("clocks misaligned!:"  << connections_clk.read() << " " << clk.read());
  }
};
