set sfd [file dir [info script]]

options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags " "

project new

flow package require /SCVerify
flow package option set /SCVerify/INVOKE_ARGS "trace direct_input_sync.log $sfd/run_logs"
flow package require /QuestaSIM
flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true
flow package option set /QuestaSIM/MSIM_DOFILE msim.do

solution file add "$sfd/testbench.cpp" -type C++

go analyze
directive set -DESIGN_HIERARCHY dut

go compile
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm
directive set /dut/direct_inputs:rsc -MAP_TO_MODULE {[DirectInput]}

go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 2.0}}


go assembly

# NOTE: following directive REQUIRED for using direct_input_sync in C++ flow
directive set STRICT_IO_SCHEDULING=STRICT

directive set /dut/main/ac_math::ac_sqrt<32,32,false>:for -UNROLL yes
go architect
go allocate
go extract

