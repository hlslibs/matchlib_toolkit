set sfd [file dir [info script]]

options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags "-DCONNECTIONS_ACCURATE_SIM "

project new

flow package require /SCVerify
flow package require /QuestaSIM
flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true
flow package option set /QuestaSIM/MSIM_DOFILE $sfd/msim.do

solution file add "$sfd/testbench.cpp" -type C++

go analyze
directive set -DESIGN_HIERARCHY dut

go compile
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm

go libraries
directive set -CLOCKS {clk1 {-CLOCK_PERIOD 2.0} clk2 {-CLOCK_PERIOD 2.0}}

go assembly
go architect
go allocate
go extract

