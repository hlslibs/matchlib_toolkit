
# Establish the location of this script and use it to reference all
# other files in this example
set sfd [file dirname [info script]]

# Reset the options to the factory defaults
options defaults
project new

# Set simulation options
options set /Input/CppStandard c++11
options set /Input/CompilerFlags {-DSINGLE_PROCESS -DSC_INCLUDE_DYNAMIC_PROCESSES}
flow package require /SCVerify

# Read Design and Libraries
solution file add ${sfd}/testbench.cpp -type C++
solution library add nangate-45nm_beh
solution library add ccs_sample_mem
go libraries

# Set clock period
directive set -CLOCKS {clk {-CLOCK_PERIOD 2}}
go assembly

# Configure memory
directive set /matrixMultiply/run/B_transpose:rsc -MAP_TO_MODULE ccs_sample_mem.ccs_ram_sync_1R1W
directive set /matrixMultiply/run/B_transpose -WORD_WIDTH 64
go architect

# Ignore schedule dependencies from memory write to read
ignore_memory_precedences -from *write_mem* -to *read_mem*
go allocate

# Generate RTL
go extract

