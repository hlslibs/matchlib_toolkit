solution new -state initial
solution options defaults
solution options set /Input/CppStandard c++11
solution options set /Input/CompilerFlags {-DCONNECTIONS_ACCURATE_SIM -DSC_INCLUDE_DYNAMIC_PROCESSES}
options set /Input/SearchPath {../../include} -append

flow package require /SCVerify
solution options set /Output/GenerateCycleNetlist false
solution file add ./testbench.cpp -type C++
go new
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm
solution library add ccs_sample_mem
go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 2 -CLOCK_HIGH_TIME 1 -CLOCK_OFFSET 0.000000 -CLOCK_UNCERTAINTY 0.0}}
go assembly
directive set /dut/mem:rsc -MAP_TO_MODULE ccs_sample_mem.ccs_ram_sync_1R1W
directive set /dut/mem:rsc -EXTERNAL_MEMORY true
go architect
ignore_memory_precedences -from *write_mem* -to *read_mem*
go extract

