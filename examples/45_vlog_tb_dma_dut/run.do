# Copyright 1991-2019 Mentor Graphics Corporation
#
# All Rights Reserved.
#
# THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION
# WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION
# OR ITS LICENSORS AND IS SUBJECT TO LICENSE TERMS.

# Use this run.do file to run this example.
# Either bring up ModelSim and type the following at the "ModelSim>" prompt:
#     do run.do
# or, to run from a shell, type the following at the shell prompt:
#     vsim -do run.do -c
# (omit the "-c" to see the GUI while running from the shell)

onbreak {resume}

# vlib work

# compile the Verilog source files
# vlog *.v

# compile and link C source files
# sccom -link

# open debugging windows
quietly view *

# start and run simulation
vopt testbench -o testbench_opt -enablescstdout -undefsyms=verbose
vsim testbench_opt
add wave sim:/testbench/dma_INST/*

run 5000 ns
echo "Finished DMA wrap test"
if { [batch_mode] } {
  quit -f
}
wave zoom full
