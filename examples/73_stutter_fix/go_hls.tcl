
# use AXI_MEM rather than local RAM in DUT
set use_axi_mem 0

# use cycle set directives to improve thruput in RTL when using AXI MEM
set use_cycle_set 0

set sfd [file dir [info script]]

options defaults
options set /Input/CppStandard c++11

if {$use_axi_mem} { 
  options set /Input/CompilerFlags {-DUSE_AXI_MEM=1}
}
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append
options set /Input/SearchPath {../../include} -append
options set /Input/SearchPath {.} -append

project new

# CDesignChecker flow
# ===========
flow package require /CDesignChecker

# CCOV flow
# ===========
flow package require /CCOV
flow package option set /CCOV/EXCLUDE_TB_FILES true
flow package option set /CCOV/CCOV_EXCLUDE_FILES { apb_ram.h }
flow package option set /CCOV/CCOV_HOME $env(CCOV_HOME)

# SCVerify flow
# ===========
flow package require /SCVerify

# solution file add
# ===========
solution file add "$sfd/dut.h" -type CHEADER
solution file add "$sfd/testbench.cpp" -type C++ -exclude true

go analyze
directive set -DESIGN_HIERARCHY dut
#directive set -CHAN_IO_PROTOCOL coupled

go compile

# Specify Technology Library
# ===========
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm
solution library add ccs_sample_mem


go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 2.0}}

go assembly
directive set /dut/main/ac_math::ac_sqrt<32,32,false>:for -UNROLL yes
go architect

if {$use_cycle_set} { 
 cycle set r_master0.r.Pop() -from r_segment0_ex_ar_chan.Push() -equal 2
 cycle set w_segment0_b_chan.Pop() -from w_segment0_ex_aw_chan.Push() -equal 4
 cycle set w_segment0_w_chan.Push() -from w_segment0_ex_aw_chan.Push() -equal 1
}

go allocate
go extract
