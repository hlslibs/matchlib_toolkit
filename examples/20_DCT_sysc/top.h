// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include "testbench.h"
#include "pixelpipe_mod.h"
#include <mc_scverify.h>

SC_MODULE(top)
{
  sc_clock        clk;
  sc_signal<bool> CCS_INIT_S1(rst);

  Connections::Combinational<ac_int<2, false>> CCS_INIT_S1(blocktype);
  Connections::Combinational<rgb_t> CCS_INIT_S1(rgbstream);
  Connections::Combinational<codes_t> CCS_INIT_S1(hufstream);

  testbench testbench_INST; // Instance of testbench

#ifdef TOP_PIXELPIPE_MOD
  CCS_DESIGN(pixelpipe_mod) CCS_INIT_S1(p_inst);
#else
  pixelpipe_mod CCS_INIT_S1(p_inst);
#endif

  SC_HAS_PROCESS(top);
  top(const sc_module_name& name, const std::string &input_image_bmp) :
    clk("clk", 5.00, SC_NS, 0.5, 0, SC_NS, true),
    testbench_INST("testbench_INST",input_image_bmp) {
    sc_object_tracer<sc_clock> trace_clk(clk);

    testbench_INST.clk(clk);
    testbench_INST.rst(rst);
    testbench_INST.blocktype(blocktype);
    testbench_INST.rgbstream(rgbstream);
    testbench_INST.hufstream(hufstream);

    p_inst.clk(clk);
    p_inst.rstn(rst);
    p_inst.blocktype(blocktype);
    p_inst.rgb(rgbstream);
    p_inst.codes(hufstream);

    SC_THREAD(reset);
  }

  void reset() {
    rst.write(0);
    wait(50, SC_NS);                        // WAIT
    rst.write(1);
  }
};

