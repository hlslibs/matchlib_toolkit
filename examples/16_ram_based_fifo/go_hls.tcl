set sfd [file dir [info script]]

options defaults
options set /Input/CppStandard c++11
options set /Input/CompilerFlags -DCONNECTIONS_ACCURATE_SIM
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append

project new

flow package require /SCVerify

flow package require /QuestaSIM
flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true
flow package option set /QuestaSIM/MSIM_DOFILE $sfd/msim.do

solution file add "$sfd/dut.h" -type CHEADER
solution file add "$sfd/testbench.cpp" -type C++ -exclude true

go analyze
solution design set dut -top
go compile
solution library add nangate-45nm_beh -- -rtlsyntool DesignCompiler -vendor Nangate -technology 045nm
solution library add ram_nangate-45nm_pipe_beh

go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 2.0}}

go assembly
directive set /dut/mm_fifo<dut::T,4096>/fifo0:cns -MAP_TO_MODULE ram_nangate-45nm_pipe_beh.mgc_pipe_mem
go architect
go allocate
go extract

