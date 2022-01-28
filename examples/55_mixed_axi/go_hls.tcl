set sfd [file dir [info script]]

options defaults
options set /Input/CppStandard c++11
options set /Input/CompilerFlags {-DCONNECTIONS_ACCURATE_SIM -DSEGMENT_BURST_SIZE=16 }
options set /Input/SearchPath [file normalize [file join $sfd ../../include]]
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append

project new

flow package require /SCVerify

flow package require /QuestaSIM
flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true
flow package option set /QuestaSIM/MSIM_DOFILE $sfd/msim.do

solution file add "$sfd/mixed_dma.h" -type CHEADER
solution file add "$sfd/mixed_ram.h" -type CHEADER
solution file add "$sfd/testbench.cpp" -type C++ -exclude true

go analyze
directive set -DESIGN_HIERARCHY dma

go compile
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm

go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 2.0}}

go assembly
go architect
go allocate
go extract

