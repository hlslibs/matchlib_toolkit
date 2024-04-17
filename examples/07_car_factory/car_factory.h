// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <mc_connections.h>

#include "auto_gen_fields.h"

struct chassis_t {
  sc_uint<16> chassis;

  AUTO_GEN_FIELD_METHODS(chassis_t, ( \
   chassis \
  ) )
  //
};

struct spark_plug_t {
  sc_uint<16> spark_plug;

  AUTO_GEN_FIELD_METHODS(spark_plug_t, ( \
    spark_plug \
  ) )
  //
};

struct engine_t {
  static const int plugs = 4;
  sc_uint<16> engine;
  spark_plug_t spark_plugs[plugs];

  AUTO_GEN_FIELD_METHODS(engine_t, ( \
      engine \
    , spark_plugs \
  ) )
  //
};

struct car_t {
  engine_t  engine;
  chassis_t chassis;

  AUTO_GEN_FIELD_METHODS(car_t, ( \
     engine \
   , chassis \
  ) )
  //
};


class spark_plug_robot : public sc_module
{
public:
  sc_in<bool>                     CCS_INIT_S1(clk);
  sc_in<bool>                     CCS_INIT_S1(rst_bar);
  Connections::In<spark_plug_t>   CCS_INIT_S1(spark_plugs);
  Connections::In<engine_t>       CCS_INIT_S1(engines_in);
  Connections::Out<engine_t>      CCS_INIT_S1(engines_out);
  SC_SIG(bool, busy);
  SC_SIG(bool, maintenance);

  SC_CTOR(spark_plug_robot) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }


  void main() {
    engines_in.Reset();
    spark_plugs.Reset();
    engines_out.Reset();
    int count = 0;
    wait();                                 // WAIT

    while (1) {
      engine_t engine_in = engines_in.Pop();
      for (int i=0; i < engine_t::plugs; i++) {
        engine_in.spark_plugs[i] = spark_plugs.Pop();
      }
      busy = 1;
      wait(60);                             // WAIT
      busy = 0;
      engines_out.Push(engine_in);
      if ((count++ & 1) && (rand() & 3)) {
        maintenance = 1;
        wait(60);                           // WAIT
        maintenance = 0;
      }
    }
  }
};

class engine_install_robot : public sc_module
{
public:
  sc_in<bool>                  CCS_INIT_S1(clk);
  sc_in<bool>                  CCS_INIT_S1(rst_bar);
  Connections::In<chassis_t>   CCS_INIT_S1(chassis);
  Connections::In<engine_t>    CCS_INIT_S1(engines);
  Connections::Out<car_t>      CCS_INIT_S1(cars);
  SC_SIG(bool, busy);

  SC_CTOR(engine_install_robot) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    chassis.Reset();
    engines.Reset();
    cars.Reset();
    wait();                                 // WAIT
    while (1) {
      car_t car;
      car.chassis = chassis.Pop();
      car.engine = engines.Pop();
      busy = 1;
      wait(30);                             // WAIT
      busy = 0;
      cars.Push(car);
    }
  }
};

#define SIMPLE

// #define SEQUENTIAL

// #define CONCURRENT

#if defined(SIMPLE)

class car_factory : public sc_module
{
public:
  sc_in<bool>                   CCS_INIT_S1(clk);
  sc_in<bool>                   CCS_INIT_S1(rst_bar);
  Connections::In<spark_plug_t> CCS_INIT_S1(spark_plugs);
  Connections::In<engine_t>     CCS_INIT_S1(engines);
  Connections::In<chassis_t>    CCS_INIT_S1(chassis);
  Connections::Out<car_t>       CCS_INIT_S1(cars);

  Connections::Combinational<engine_t> CCS_INIT_S1(finished_engines);

  spark_plug_robot      CCS_INIT_S1(spark_plug_robot1);
  engine_install_robot  CCS_INIT_S1(engine_install_robot1);

  SC_CTOR(car_factory) {
    spark_plug_robot1.clk(clk);
    spark_plug_robot1.rst_bar(rst_bar);
    spark_plug_robot1.spark_plugs(spark_plugs);
    spark_plug_robot1.engines_in(engines);
    spark_plug_robot1.engines_out(finished_engines);

    engine_install_robot1.clk(clk);
    engine_install_robot1.rst_bar(rst_bar);
    engine_install_robot1.chassis(chassis);
    engine_install_robot1.engines(finished_engines);
    engine_install_robot1.cars(cars);
  }
};

#elif defined(SEQUENTIAL)

class car_factory : public sc_module
{
public:
  sc_in<bool>                   CCS_INIT_S1(clk);
  sc_in<bool>                   CCS_INIT_S1(rst_bar);
  Connections::In<spark_plug_t> CCS_INIT_S1(spark_plugs);
  Connections::In<engine_t>     CCS_INIT_S1(engines);
  Connections::In<chassis_t>    CCS_INIT_S1(chassis);
  Connections::Out<car_t>       CCS_INIT_S1(cars);

  SC_CTOR(car_factory) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  SC_SIG(bool, spark_plug_robot_busy);
  SC_SIG(bool, spark_plug_robot_maintenance);
  SC_SIG(bool, engine_install_robot_busy);

  void main() {
    spark_plug_t plugs[engine_t::plugs];
    int plug_count = 0;

    engine_t unfinished_engine;
    int unfinished_engine_count = 0;
    engine_t finished_engine;
    int finished_engine_count = 0;
    chassis_t chassis_inst;
    int chassis_count = 0;
    int spark_plug_robot_count = 0;

    spark_plugs.Reset();
    engines.Reset();
    chassis.Reset();
    cars.Reset();
    wait();                                 // WAIT

    while (1) {
      if (plug_count < engine_t::plugs) {
        if (spark_plugs.PopNB(plugs[plug_count])) { ++plug_count; }
      }
      if (unfinished_engine_count == 0) {
        if (engines.PopNB(unfinished_engine)) { ++unfinished_engine_count; }
      }
      if (chassis_count == 0) {
        if (chassis.PopNB(chassis_inst)) { ++chassis_count; }
      }
      if ((unfinished_engine_count == 1) && (plug_count == engine_t::plugs)
          && (finished_engine_count == 0)) {
        finished_engine = unfinished_engine;
        for (int i=0; i < engine_t::plugs; i++) {
          finished_engine.spark_plugs[i] = plugs[i];
        }
        spark_plug_robot_busy = 1;
        wait(60);                           // WAIT
        spark_plug_robot_busy = 0;
        if ((spark_plug_robot_count++ & 1) && (rand() & 3)) {
          spark_plug_robot_maintenance = 1;
          wait(60);                         // WAIT
          spark_plug_robot_maintenance = 0;
        }
        finished_engine_count = 1;
        plug_count = 0;
        unfinished_engine_count = 0;
      }

      if ((finished_engine_count == 1) && (chassis_count == 1)) {
        car_t car;
        car.chassis = chassis_inst;
        car.engine = finished_engine;
        engine_install_robot_busy = 1;
        wait(30);                           // WAIT
        engine_install_robot_busy = 0;
        cars.Push(car);
        finished_engine_count = 0;
        chassis_count = 0;
      }

      wait();                               // WAIT
    }
  }
};

#elif defined(CONCURRENT)

class spark_plugs_split : public sc_module
{
public:
  sc_in<bool>                        CCS_INIT_S1(clk);
  sc_in<bool>                        CCS_INIT_S1(rst_bar);
  Connections::In<spark_plug_t>      CCS_INIT_S1(spark_plugs_in);
  Connections::Out<spark_plug_t>     CCS_INIT_S1(spark_plugs_out1);
  Connections::Out<spark_plug_t>     CCS_INIT_S1(spark_plugs_out2);

  SC_CTOR(spark_plugs_split) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    spark_plugs_in.Reset();
    spark_plugs_out1.Reset();
    spark_plugs_out2.Reset();
    wait();                                 // WAIT
    while (1) {
      spark_plug_t spark_plug = spark_plugs_in.Pop();
      while (1) {
        if (spark_plugs_out1.PushNB(spark_plug)) { break; }
        if (spark_plugs_out2.PushNB(spark_plug)) { break; }
        wait();                             // WAIT
      }
    }
  }
};

class engines_split : public sc_module
{
public:
  sc_in<bool>                    CCS_INIT_S1(clk);
  sc_in<bool>                    CCS_INIT_S1(rst_bar);
  Connections::In<engine_t>      CCS_INIT_S1(engines_in);
  Connections::Out<engine_t>     CCS_INIT_S1(engines_out1);
  Connections::Out<engine_t>     CCS_INIT_S1(engines_out2);

  SC_CTOR(engines_split) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    engines_in.Reset();
    engines_out1.Reset();
    engines_out2.Reset();
    wait();                                 // WAIT
    while (1) {
      engine_t engine = engines_in.Pop();
      while (1) {
        if (engines_out1.PushNB(engine)) { break; }
        if (engines_out2.PushNB(engine)) { break; }
        wait();                             // WAIT
      }
    }
  }
};

class engines_merge : public sc_module
{
public:
  sc_in<bool>                    CCS_INIT_S1(clk);
  sc_in<bool>                    CCS_INIT_S1(rst_bar);
  Connections::In<engine_t>      CCS_INIT_S1(engines_in1);
  Connections::In<engine_t>      CCS_INIT_S1(engines_in2);
  Connections::Out<engine_t>     CCS_INIT_S1(engines_out);

  SC_CTOR(engines_merge) {
    SC_THREAD(main);
    sensitive << clk.pos();
    async_reset_signal_is(rst_bar, false);
  }

  void main() {
    engines_in1.Reset();
    engines_in2.Reset();
    engines_out.Reset();
    wait();                                 // WAIT
    while (1) {
      engine_t engine;
      while (1) {
        if (engines_in1.PopNB(engine)) { break; }
        if (engines_in2.PopNB(engine)) { break; }
        wait();                             // WAIT
      }
      engines_out.Push(engine);
    }
  }
};

class car_factory : public sc_module
{
public:
  sc_in<bool>                   CCS_INIT_S1(clk);
  sc_in<bool>                   CCS_INIT_S1(rst_bar);
  Connections::In<spark_plug_t> CCS_INIT_S1(spark_plugs);
  Connections::In<engine_t>     CCS_INIT_S1(engines);
  Connections::In<chassis_t>    CCS_INIT_S1(chassis);
  Connections::Out<car_t>       CCS_INIT_S1(cars);

  Connections::Combinational<spark_plug_t> CCS_INIT_S1(spark_plugs_stream1);
  Connections::Combinational<spark_plug_t> CCS_INIT_S1(spark_plugs_stream2);
  Connections::Combinational<engine_t> CCS_INIT_S1(unfinished_engines_stream1);
  Connections::Combinational<engine_t> CCS_INIT_S1(unfinished_engines_stream2);
  Connections::Combinational<engine_t> CCS_INIT_S1(finished_engines_stream1);
  Connections::Combinational<engine_t> CCS_INIT_S1(finished_engines_stream2);
  Connections::Combinational<engine_t> CCS_INIT_S1(finished_engines_merged);

  engines_split         CCS_INIT_S1(engines_split1);
  spark_plugs_split     CCS_INIT_S1(spark_plugs_split1);
  spark_plug_robot      CCS_INIT_S1(spark_plug_robot1);
  spark_plug_robot      CCS_INIT_S1(spark_plug_robot2);
  engines_merge         CCS_INIT_S1(engines_merge1);
  engine_install_robot  CCS_INIT_S1(engine_install_robot1);

  SC_CTOR(car_factory) {
    engines_split1.clk(clk);
    engines_split1.rst_bar(rst_bar);
    engines_split1.engines_in(engines);
    engines_split1.engines_out1(unfinished_engines_stream1);
    engines_split1.engines_out2(unfinished_engines_stream2);

    spark_plugs_split1.clk(clk);
    spark_plugs_split1.rst_bar(rst_bar);
    spark_plugs_split1.spark_plugs_in(spark_plugs);
    spark_plugs_split1.spark_plugs_out1(spark_plugs_stream1);
    spark_plugs_split1.spark_plugs_out2(spark_plugs_stream2);

    spark_plug_robot1.clk(clk);
    spark_plug_robot1.rst_bar(rst_bar);
    spark_plug_robot1.spark_plugs(spark_plugs_stream1);
    spark_plug_robot1.engines_in(unfinished_engines_stream1);
    spark_plug_robot1.engines_out(finished_engines_stream1);

    spark_plug_robot2.clk(clk);
    spark_plug_robot2.rst_bar(rst_bar);
    spark_plug_robot2.spark_plugs(spark_plugs_stream2);
    spark_plug_robot2.engines_in(unfinished_engines_stream2);
    spark_plug_robot2.engines_out(finished_engines_stream2);

    engines_merge1.clk(clk);
    engines_merge1.rst_bar(rst_bar);
    engines_merge1.engines_in1(finished_engines_stream1);
    engines_merge1.engines_in2(finished_engines_stream2);
    engines_merge1.engines_out(finished_engines_merged);

    engine_install_robot1.clk(clk);
    engine_install_robot1.rst_bar(rst_bar);
    engine_install_robot1.chassis(chassis);
    engine_install_robot1.engines(finished_engines_merged);
    engine_install_robot1.cars(cars);
  }
};

#endif

