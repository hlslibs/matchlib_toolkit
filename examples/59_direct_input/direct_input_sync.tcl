set sfd [file dir [info script]]

options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags "-DHLS_DIRECT_INPUT -DCONNECTIONS_ACCURATE_SIM -DCONNECTIONS_NAMING_ORIGINAL"

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

go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 2.0}}


go assembly
directive set /dut/main/while/sync_in.sync_in() -DIRECT_INPUT_SYNC all
go architect
go allocate
go extract

