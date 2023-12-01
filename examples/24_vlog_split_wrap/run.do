
onbreak {resume}

vlib work

# compile the Verilog source files
vlog testbench.v dut_split_wrap.v Catapult/dut.v1/concat_rtl.v

# open debugging windows
quietly view *

# start and run simulation
vopt +acc testbench -o testbench_opt -enablescstdout -undefsyms=verbose
vsim testbench_opt
add wave sim:/testbench/dut_wrap_inst/*

run 5000 ns
if { [batch_mode] } {
  quit -f
}
wave zoom full
