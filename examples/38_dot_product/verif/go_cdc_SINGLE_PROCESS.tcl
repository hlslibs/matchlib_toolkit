
# Establish the location of this script and use it to reference all
# other files in this example
set sfd [file dirname [info script]]

# Reset the options to the factory defaults
options defaults

# Set simulation options
options set /Input/CppStandard c++11
options set /Input/CompilerFlags {-DSINGLE_PROCESS -DSC_INCLUDE_DYNAMIC_PROCESSES}
options set /Input/SearchPath {$MGC_HOME/shared/examples/matchlib/toolkit/include} -append
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/boostpp/pp/include} -append
options set /Input/SearchPath $sfd/.. -append

project new

flow package require /SCVerify

# Read Design and Libraries
solution file add ${sfd}/../testbench.cpp -type C++
go compile
flow run /CDesignChecker/launch_sleccpc_sh ./CDesignChecker/design_checker.sh Custom
