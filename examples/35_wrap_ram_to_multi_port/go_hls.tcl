set sfd [file dir [info script]]

solution new -state initial
solution options defaults
solution options set /Input/CppStandard c++11
solution options set /Input/CompilerFlags {-DCONNECTIONS_ACCURATE_SIM -DSC_INCLUDE_DYNAMIC_PROCESSES}
options set /Input/SearchPath [file normalize [file join $sfd ../../include]]
flow package require /SCVerify
#options set Flows/SCVerify/USE_NCSIM true
#options set Flows/SCVerify/USE_VCS true
solution options set /Output/GenerateCycleNetlist false
solution file add ./testbench.cpp -type C++
go new
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm
solution library add ccs_sample_mem
go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 10 -CLOCK_HIGH_TIME 1 -CLOCK_OFFSET 0.000000 -CLOCK_UNCERTAINTY 0.0}}
go assembly
#directive set /dut/mem_wrapper/main/mem:rsc -MAP_TO_MODULE ccs_sample_mem.ccs_ram_sync_1R1W
#directive set /dut/blk/main/mem:rsc -BLOCK_SIZE 512
#directive set /dut/blk/main/mem:rsc -EXTERNAL_MEMORY true
 go architect
ignore_memory_precedences -from *write_mem* -to *read_mem*
#ignore_memory_precedences -from write_mem(/dut/mem_wrapper/main_thread/mem.rsc.@) -to read_mem(/dut/mem_wrapper/main_thread/mem.rsc.@)
go extract

