
# Establish the location of this script and use it to reference all
# other files in this example
set sfd [file dirname [info script]]

# Reset the options to the factory defaults
options defaults

# Set simulation options (no VHDL model for DPRAM)
options set /Input/CppStandard c++11
options set /Input/CompilerFlags {-DMULTI_PROCESS -DSC_INCLUDE_DYNAMIC_PROCESSES }
options set /Input/SearchPath {$MGC_HOME/shared/examples/matchlib/toolkit/include} -append
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/boostpp/pp/include} -append
options set /Input/SearchPath $sfd -append

project new

flow package require /SCVerify

# Read Design and Libraries
solution file add ${sfd}/testbench.cpp -type C++
solution library add nangate-45nm_beh
solution library add ccs_sample_mem
go libraries

# Set clock period
directive set -CLOCKS {clk {-CLOCK_PERIOD 2}}
go assembly

# Generate RTL
go extract

