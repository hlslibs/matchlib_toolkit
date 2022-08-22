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


// The NO_DEFAULT_CONNECTIONS_CLOCK needs to be used here to eliminate the default 1 NS clock
// in the connections library, since the main clock is 1 second. However, in general, this
// macro should not be used in real designs.
#define NO_DEFAULT_CONNECTIONS_CLOCK 1

#include "car_factory.h"
#include <mc_scverify.h>

class spark_plug_producer : public sc_module
{
public:
  sc_in<bool>                    CCS_INIT_S1(clk);
  sc_in<bool>                    CCS_INIT_S1(rst_bar);
  Connections::Out<spark_plug_t> CCS_INIT_S1(spark_plugs);

  SC_CTOR(spark_plug_producer) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    spark_plugs.Reset();
    int count=0;
    wait();

    while (1) {
      spark_plug_t spark_plug;
      spark_plug.spark_plug = count++;
      spark_plugs.Push(spark_plug);
      wait(3);
      if (rand() & 1) {
        wait(3);
      }
    }
  }
};

class engine_producer : public sc_module
{
public:
  sc_in<bool>                CCS_INIT_S1(clk);
  sc_in<bool>                    CCS_INIT_S1(rst_bar);
  Connections::Out<engine_t> CCS_INIT_S1(engines);

  SC_CTOR(engine_producer) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    engines.Reset();
    int count=0;
    wait();

    while (1) {
      engine_t engine;
      engine.engine = count++;
      engines.Push(engine);
      wait(20);
    }
  }
};

class chassis_producer : public sc_module
{
public:
  sc_in<bool>                   CCS_INIT_S1(clk);
  sc_in<bool>                   CCS_INIT_S1(rst_bar);
  Connections::Out<chassis_t>   CCS_INIT_S1(chassis_out);

  SC_CTOR(chassis_producer) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    chassis_out.Reset();
    int count=0;
    wait();

    while (1) {
      chassis_t chassis;
      chassis.chassis = count++;
      chassis_out.Push(chassis);
      wait(25);
    }
  }
};

class car_consumer : public sc_module
{
public:
  sc_in<bool>             CCS_INIT_S1(clk);
  sc_in<bool>             CCS_INIT_S1(rst_bar);
  Connections::In<car_t>  CCS_INIT_S1(cars);

  SC_CTOR(car_consumer) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    cars.Reset();
    int count = 0;
    wait();
    while (1) {
      cars.Pop();
      ++count;
      CCS_LOG("got car # " << count);
      if (count == 10) {
        CCS_LOG("total cars produced: " << count);
        CCS_LOG("time per car: " << sc_time_stamp() / count);
        sc_stop();
      }
    }
  }
};

class Top : public sc_module
{
public:
  CCS_DESIGN(car_factory) CCS_INIT_S1(car_factory1);

  sc_clock clk;
  SC_SIG(bool, rst_bar);

  Connections::Combinational<spark_plug_t> CCS_INIT_S1(spark_plugs);
  Connections::Combinational<engine_t>     CCS_INIT_S1(engines);
  Connections::Combinational<chassis_t>    CCS_INIT_S1(chassis);
  Connections::Combinational<car_t>        CCS_INIT_S1(cars);

  engine_producer     CCS_INIT_S1(engine_producer1);
  chassis_producer    CCS_INIT_S1(chassis_producer1);
  spark_plug_producer CCS_INIT_S1(spark_plug_producer1);
  car_consumer        CCS_INIT_S1(car_consumer1);

  SC_CTOR(Top)
    :   clk("clk", 1, SC_SEC, 0.5,0,SC_SEC,true) {
    Connections::set_sim_clk(&clk);
    sc_object_tracer<sc_clock> trace_clk(clk);

    car_factory1.clk(clk);
    car_factory1.rst_bar(rst_bar);
    car_factory1.spark_plugs(spark_plugs);
    car_factory1.engines(engines);
    car_factory1.chassis(chassis);
    car_factory1.cars(cars);

    spark_plug_producer1.clk(clk);
    spark_plug_producer1.rst_bar(rst_bar);
    spark_plug_producer1.spark_plugs(spark_plugs);

    engine_producer1.clk(clk);
    engine_producer1.rst_bar(rst_bar);
    engine_producer1.engines(engines);

    chassis_producer1.clk(clk);
    chassis_producer1.rst_bar(rst_bar);
    chassis_producer1.chassis_out(chassis);

    car_consumer1.clk(clk);
    car_consumer1.rst_bar(rst_bar);
    car_consumer1.cars(cars);

    SC_CTHREAD(reset, clk);
  }

  void reset() {
    rst_bar.write(0);
    wait(1);
    rst_bar.write(1);
    wait();
  }

};

int sc_main(int argc, char **argv)
{
  sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
  sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
  sc_trace_file *trace_file_ptr = sc_trace_static::setup_trace_file("trace");
  trace_file_ptr->set_time_unit(0.1, SC_SEC);

  Top top("top");
  trace_hierarchy(&top, trace_file_ptr);

  channel_logs logs;
  logs.enable("log_chan");
  logs.log_hierarchy(top);

  sc_start();
  if (sc_report_handler::get_count(SC_ERROR) > 0) {
    std::cout << "Simulation FAILED" << std::endl;
    return -1;
  }
  std::cout << "Simulation PASSED" << std::endl;
  return 0;
}

