// INSERT_EULA_COPYRIGHT: 2020
#ifndef __INCLUDED_RAM_1R1W_trans_rsc_H__
#define __INCLUDED_RAM_1R1W_trans_rsc_H__

#include <mc_transactors.h>

template <
  int words
  ,int width
  ,int addr_width
  >
class RAM_1R1W_trans_rsc : public mc_wire_trans_rsc_base<width,words>
{
public:
  sc_in< bool >   CK;
  sc_in< sc_logic >   RCSN;
  sc_in< sc_logic >   WCSN;
  sc_in< sc_lv<addr_width> >   RA;
  sc_in< sc_lv<addr_width> >   WA;
  sc_in< sc_lv<width> >   D;
  sc_out< sc_lv<width> >   Q;

  typedef mc_wire_trans_rsc_base<width,words> base;
  MC_EXPOSE_NAMES_OF_BASE(base);

  SC_HAS_PROCESS( RAM_1R1W_trans_rsc );
  RAM_1R1W_trans_rsc(const sc_module_name &name, bool phase, double clk_skew_delay=0.0)
    : base(name, phase, clk_skew_delay)
    ,CK("CK")
    ,RCSN("RCSN")
    ,WCSN("WCSN")
    ,RA("RA")
    ,WA("WA")
    ,D("D")
    ,Q("Q")
    ,_is_connected_wr1(true)
    ,_is_connected_wr1_messaged(false) {
    SC_METHOD(at_active_clock_edge);
    sensitive << (phase ? CK.pos() : CK.neg());
    this->dont_initialize();

    MC_METHOD(clk_skew_delay);
    this->sensitive << this->_clk_skew_event;
    this->dont_initialize();
  }

  virtual void start_of_simulation() {
    if ((base::_holdtime == 0.0) && this->get_attribute("CLK_SKEW_DELAY")) {
      base::_holdtime = ((sc_attribute<double> *)(this->get_attribute("CLK_SKEW_DELAY")))->value;
    }
    if (base::_holdtime > 0) {
      std::ostringstream msg;
      msg << "RAM_1R1W_trans_rsc CLASS_STARTUP - CLK_SKEW_DELAY = "
          << base::_holdtime << " ps @ " << sc_time_stamp();
      SC_REPORT_INFO(this->name(), msg.str().c_str());
    }
    reset_memory();
  }

  virtual void inject_value(int addr, int idx_lhs, int mywidth, sc_lv_base &rhs, int idx_rhs) {
    this->set_value(addr, idx_lhs, mywidth, rhs, idx_rhs);
  }

  virtual void extract_value(int addr, int idx_rhs, int mywidth, sc_lv_base &lhs, int idx_lhs) {
    this->get_value(addr, idx_rhs, mywidth, lhs, idx_lhs);
  }

private:
  void at_active_clock_edge() {
    base::at_active_clk();
  }

  void clk_skew_delay() {
    this->exchange_value(0);
    if (RCSN.get_interface()) { _RCSN = RCSN.read(); }
    if (WCSN.get_interface()) { _WCSN = WCSN.read(); }
    if (RA.get_interface()) { _RA = RA.read(); }
    if (WA.get_interface()) { _WA = WA.read(); }
    else { _is_connected_wr1 = false; }
    if (D.get_interface()) { 
	 _D = D.read(); 
	 } else { 
	 _is_connected_wr1 = false;
    }

    //  Write
    int _w_addr_wr1 = -1;
    if ( _is_connected_wr1 && (_WCSN==0)) {
      _w_addr_wr1 = get_addr(_WA, "WA");
      if (_w_addr_wr1 >= 0) { 
		inject_value(_w_addr_wr1, 0, width, _D, 0); 
		}
    }
    if ( !_is_connected_wr1 && !_is_connected_wr1_messaged) {
      std::ostringstream msg;
      msg << "wr1 is not fully connected and writes on it will be ignored";
      SC_REPORT_WARNING(this->name(), msg.str().c_str());
      _is_connected_wr1_messaged = true;
    }

    //  Sync Read
    if ((_RCSN==0)) {
      const int addr = get_addr(_RA, "RA");
      if (addr >= 0) {
        if (addr==_w_addr_wr1) {
          sc_lv<width> dc; // X
          _Q = dc;
        } else { 
		  extract_value(addr, 0, width, _Q, 0); 
		  }
      } else {
        sc_lv<width> dc; // X
        _Q = dc;
      }
    }
    if (Q.get_interface()) { Q = _Q; }
    this->_value_changed.notify(SC_ZERO_TIME);
  }

  int get_addr(const sc_lv<addr_width> &addr, const char *pin_name) {
    if (addr.is_01()) {
      const int cur_addr = addr.to_uint();
      if (cur_addr < 0 || cur_addr >= words) {
#ifdef CCS_SYSC_DEBUG
        std::ostringstream msg;
        msg << "Invalid address '" << cur_addr << "' out of range [0:" << words-1 << "]";
        SC_REPORT_WARNING(pin_name, msg.str().c_str());
#endif
        return -1;
      } else {
        return cur_addr;
      }
    } else {
#ifdef CCS_SYSC_DEBUG
      std::ostringstream msg;
      msg << "Invalid Address '" << addr << "' contains 'X' or 'Z'";
      SC_REPORT_WARNING(pin_name, msg.str().c_str());
#endif
      return -1;
    }
  }

  void reset_memory() {
    this->zero_data();
    _RCSN = SC_LOGIC_X;
    _WCSN = SC_LOGIC_X;
    _RA = sc_lv<addr_width>();
    _WA = sc_lv<addr_width>();
    _D = sc_lv<width>();
    _is_connected_wr1 = true;
    _is_connected_wr1_messaged = false;
  }

  sc_logic _RCSN;
  sc_logic _WCSN;
  sc_lv<addr_width>  _RA;
  sc_lv<addr_width>  _WA;
  sc_lv<width>  _D;
  sc_lv<width>  _Q;
  bool _is_connected_wr1;
  bool _is_connected_wr1_messaged;
};

#endif // ifndef __INCLUDED_RAM_1R1W_trans_rsc_H__

