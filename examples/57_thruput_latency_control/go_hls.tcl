set sfd [file dir [info script]]

options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags "-DCONNECTIONS_ACCURATE_SIM -DCONNECTIONS_NAMING_ORIGINAL -DDESIGN_1"
options set /Input/SearchPath {$MGC_HOME/shared/examples/matchlib/toolkit/include} -append
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append

project new

flow package require /SCVerify
# Set to a non-zero number to enable automatic random stall injection on handshake interfaces
#flow package option set /SCVerify/AUTOWAIT 0
# Allow initial toggle of reset
#flow package option set /SCVerify/ENABLE_RESET_TOGGLE true
# Turn on to enable systematic STALL_FLAG toggling (requires STALL_FLAG directive to be set)
#flow package option set /SCVerify/ENABLE_STALL_TOGGLE true

flow package require /QuestaSIM
# flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true
flow package option set /QuestaSIM/MSIM_DOFILE $sfd/msim.do

foreach example {DESIGN_1 DESIGN_2 DESIGN_3 DESIGN_4 DESIGN_5 DESIGN_6 DESIGN_7 DESIGN_8} {
  solution options set /Input/CompilerFlags "-DCONNECTIONS_ACCURATE_SIM -DCONNECTIONS_NAMING_ORIGINAL -D${example}"
  solution file add "$sfd/dut.h" -type CHEADER
  solution file add "$sfd/testbench.cpp" -type C++ -exclude true
  
  solution rename $example
  
  go analyze
  directive set -DESIGN_HIERARCHY dut

  go compile
  solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm

  go libraries
  directive set -CLOCKS {clk {-CLOCK_PERIOD 2.0}}

  go assembly
  go architect
  go allocate
  go extract
  if { $example != "DESIGN_8" } {
    go new
  }
}

