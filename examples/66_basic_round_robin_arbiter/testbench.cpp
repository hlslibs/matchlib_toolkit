/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 2.3                                                 *
 *                                                                        *
 *  Release Date    : Tue May 13 15:55:46 PDT 2025                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 2.3.0                                               *
 *                                                                        *
 *  Copyright 2023 Siemens                                                *
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


#include "mc_scverify.h"
#include "dut.h"

SC_MODULE(driver){
  sc_in<bool> clk{"clk"};
  sc_in<bool> rst{"rst"};
  Connections::Out<data_t> in[NUM_INPUTS];
   
  SC_CTOR(driver){
    SC_THREAD(run);
    sensitive << clk.pos();
    async_reset_signal_is(rst, false);
    #ifdef CONNECTIONS_SIM_ONLY
    for(unsigned i=0; i<NUM_INPUTS; i++){
      in[i].disable_spawn();
    }
    #endif
  }
   
  void run(){
    for(unsigned i=0; i<NUM_INPUTS; i++){
      in[i].dat = 0;
      in[i].vld = false;
    }
    wait();
    while(true){
      //All inputs have 1-packet at the same time
      for(unsigned i=0; i<NUM_INPUTS; i++){
        in[i].dat = i;
        in[i].vld = 1;
      }
      wait();
      ac_int<NUM_INPUTS, false> valid;
      do{
        
        for(unsigned i=0; i<NUM_INPUTS; i++){
          if(in[i].rdy){
            in[i].vld = 0;
            break;
          }
        }
        wait();
        for(unsigned i=0; i<NUM_INPUTS; i++){
          valid[i] = in[i].vld;
        }
      } while(valid != 0);
      wait(5);
      
      //One-by-one starting at 0
      for(unsigned i=0; i<NUM_INPUTS; i++){
        in[i].vld = true;
        do{
          wait();  
        }while(!in[i].rdy);
        in[i].vld = 0;
      }
      wait(5);
      
      //All inputs have 2-packet at the same time
      unsigned cnt[NUM_INPUTS];
      for(unsigned i=0; i<NUM_INPUTS; i++){
        in[i].dat = i;
        in[i].vld = 1;
	cnt[i] = 2;
      }
      wait();
      do{
        
        for(unsigned i=0; i<NUM_INPUTS; i++){
          if(in[i].rdy){
            cnt[i]--;
	    if(cnt[i] == 0){
              in[i].vld = 0;
	    }
            break;
          }
        }
        wait();
        for(unsigned i=0; i<NUM_INPUTS; i++){
          valid[i] = in[i].vld;
        }
      } while(valid != 0);
      wait(5);
      
      sc_stop();
    }
  }
};

SC_MODULE(monitor){
  sc_in<bool> clk{"clk"};
  sc_in<bool> rst{"rst"};
  Connections::In<data_t> out{"out"};
   
  SC_CTOR(monitor){
    SC_THREAD(run);
    sensitive << clk.pos();
    async_reset_signal_is(rst, false);
    #ifdef CONNECTIONS_SIM_ONLY
      out.disable_spawn();
    #endif
  }
  
  void run(){
    out.rdy = 0;
    wait();
    while(true){
      wait(NUM_INPUTS/2);  
      out.rdy = true;
      wait();
    }
  }
};


SC_MODULE(testbench) {
  typedef rr_arbiter<NUM_INPUTS, data_t> dut_t;
  CCS_DESIGN(dut_t) dut{"dut"};
  driver driver_i{"driver_i"};
  monitor monitor_i{"monitor_i"};

  sc_clock clk{"clk", 1, SC_NS};
  sc_signal<bool> rst{"rst"};

  Connections::Combinational<data_t> in[NUM_INPUTS];
  Connections::Combinational<data_t> out{"out"};

  SC_CTOR(testbench){
    dut.clk(clk);
    dut.rst(rst);
    for(unsigned i=0; i<NUM_INPUTS; i++){
      dut.in[i](in[i]);
    }
    dut.out(out);

    driver_i.clk(clk);
    driver_i.rst(rst);
    for(unsigned i=0; i<NUM_INPUTS; i++){
      driver_i.in[i](in[i]);
    }

    monitor_i.clk(clk);
    monitor_i.rst(rst);
    monitor_i.out(out);
      
    SC_THREAD(reset);
    sensitive << clk.posedge_event();

  }

  void reset(){
    rst = false;
    wait(3);
    rst = true;
  }

};

int sc_main(int argc, char **argv) {
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ID_LOGIC_X_TO_BOOL_, SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file("trace");

  testbench tb("tb");
  trace_hierarchy(&tb, trace_file_ptr);
  sc_trace(trace_file_ptr, tb.clk, "tb.clk");
  sc_trace(trace_file_ptr, tb.rst, "tb.rst");
  sc_start();

  return 0;
}
