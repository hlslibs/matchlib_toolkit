
#pragma once

#include <iostream>
#include <iomanip>


class block_data_abs;
extern std::vector<block_data_abs*> block_data_vec;

class observe_block_data_read_write_abs {
public:
  virtual void write_cb(int rsc_num, int row, sc_lv_base& rhs) = 0;
  virtual void read_cb (int rsc_num, int row, sc_dt::sc_subref<sc_lv_base> lhs) = 0;
};

class block_data_abs {
public:
  block_data_abs() {
    block_data_vec.push_back(this);
  }

  std::vector<observe_block_data_read_write_abs*> observers;
  int rsc_num{0};

  virtual void set_value(int row, int idx_lhs, int width, sc_lv_base& rhs, int idx_rhs, bool silent=false) = 0;
  virtual void get_value(int row, int idx_rhs, int width, sc_lv_base& lhs, int idx_lhs) = 0;
  virtual void get_dimensions(int& rows, int& width) = 0;
  void add_observer(observe_block_data_read_write_abs* p) { observers.push_back(p); }
};

template <class T, int N> 
class block_data_proxy {
public:
  std::vector<block_data_abs*> local_block_data_vec;
  static const int elem_width{Wrapped<T>::width};

  uint32_t write_cnt[N];

  block_data_proxy(std::string _name="", bool _add_time_stamp=0) 
   : obs1(_name, _add_time_stamp, write_cnt)
  {
    for (unsigned i=0; i < N; i++)
       write_cnt[i] = 0;
  }
 
  void set_block_data(block_data_abs* abs) {
    local_block_data_vec.push_back(abs);
  }

  T read(int idx) const {
      assert(idx < N);
      sc_lv_base lv;
      assert(local_block_data_vec.size());
      local_block_data_vec[0]->get_value(idx, 0, elem_width, lv, 0);
      sc_uint<elem_width> t = lv;
      return t.to_uint64();
  }

  void write(int idx, T v) {
      assert(idx < N);
      sc_uint<elem_width> t = v.to_uint64();
      sc_lv<elem_width> lv = t.to_uint64();
      assert(local_block_data_vec.size());
      local_block_data_vec[0]->set_value(idx, 0, elem_width, lv, 0);
  }

  class obs : public observe_block_data_read_write_abs{
   public:
      obs(std::string _name, bool _use_ts, uint32_t* _write_cnt) 
        : name(_name) 
        , use_ts(_use_ts)
        , write_cnt(_write_cnt)
      {
        if (name != "") {
          mem_read_log.rdbuf()->pubsetbuf(0, 0);
          mem_write_log.rdbuf()->pubsetbuf(0, 0);
          mem_read_log.open(name + "_read.log");
          mem_write_log.open(name + "_write.log");
        }
      }
      bool use_ts;
      std::string name;
      uint32_t* write_cnt;
      ofstream mem_read_log;
      ofstream mem_write_log;

      void write_cb(int rsc_num, int row, sc_lv_base& rhs) {
        std::ostringstream os;
        if (use_ts)
          os << sc_time_stamp();
        else
          os << "";

        ++write_cnt[row];
        mem_write_log << std::setfill('0') << std::setw(8) << std::hex << row 
           << " " << write_cnt[row] << " " << rhs << " " << os.str() << "\n";
      }

      void read_cb(int rsc_num, int row, sc_dt::sc_subref<sc_lv_base> lhs) {
        std::ostringstream os;
        if (use_ts)
          os << sc_time_stamp();
        else
          os << "";

        mem_read_log << std::setfill('0') << std::setw(8) << std::hex << row 
             << " " << write_cnt[row] << " " << lhs << " " << os.str() << "\n";
      }
  };

  obs obs1;

  void sync_with_all_block_data() 
  {
    // add and initialize block_data observers, set_block_data for block_data_proxy
    if (block_data_vec.size()) {
     for (unsigned v = 0; v < block_data_vec.size(); v++) {
      block_data_abs* bd_abs = block_data_vec[v];
      bd_abs->rsc_num = v;
      bd_abs->add_observer(&obs1);
      set_block_data(bd_abs);

      int rows, width;
      bd_abs->get_dimensions(rows, width);
      // log_stream << "START data vec# " << std::dec << v << " rows: " << rows << " width: " << width << "\n";

     }
    }
  }
};
