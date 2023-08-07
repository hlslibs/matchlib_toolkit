
#pragma once

// utilities for generation idle signals in matchlib connections models.


#define COUNTER_THREAD(x) \
  void x ## _thread () {  \
    x ## _cnt = 0;        \
    wait();               \
    while (1) {           \
      if (x .rdy.read() && x .vld.read())        \
        x ## _cnt = x ## _cnt.read() + 1;        \
      wait();                                    \
    }                                            \
  }

#define IN_STALLED(x) ((x.rdy.read() == 1) && (x.vld.read() == 0))
#define OUT_STALLED(x) ((x.rdy.read() == 0) && (x.vld.read() == 1))
#define EQUAL_COUNTS(x, y) (x ## _cnt.read() == y ## _cnt.read())


class gated_clock : public sc_module {
public:
   sc_in<bool> CCS_INIT_S1(idle);
   sc_out<bool> CCS_INIT_S1(clk_out);

   sc_time period;
   sc_time period_div_2;

   SC_HAS_PROCESS(gated_clock);

   gated_clock(sc_module_name nm, sc_time period_) : sc_module(nm) {
     period = period_;
     period_div_2 = period / 2;
     SC_THREAD(main);
   }

   void main() {
     while (1) {
       clk_out = 1;
       wait(period_div_2);
       // this may look wrong, but goal is to force the clk "high" when idle=1
       if (!idle.read())
         clk_out = 0;
       wait(period_div_2);
     }
   }

};

/**
class clock_gate 
  : public sc_signal<bool,SC_ONE_WRITER>
  , public sc_module
{
 public:
   sc_in<bool> CCS_INIT_S1(clk_in);
   sc_in<bool> CCS_INIT_S1(idle);

   // this works for pre-HLS dut but for some reason Questa RTL does not work with it..

   SC_CTOR(clock_gate) {
     SC_METHOD(main);
     sensitive << clk_in;
   }

   void main() {
     bool v = clk_in.read() | idle.read();
     m_new_val = v;
     m_cur_val = v;
     if (!idle.read()) {
      if (v) {
       const_cast<sc_event&>(posedge_event()).notify();
       const_cast<sc_event&>(default_event()).notify();
      } else {
       const_cast<sc_event&>(negedge_event()).notify();
      }
     }
   }
};
**/
