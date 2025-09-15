set sfd [file dir [info script]]

options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags "-DCONNECTIONS_ACCURATE_SIM "
options set /Input/SearchPath {$MGC_HOME/shared/examples/matchlib/toolkit/include} -append
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append

project new

flow package require /SCVerify
# Set to a non-zero number to enable automatic random stall injection on handshake interfaces
#flow package option set /SCVerify/AUTOWAIT 0
# Allow initial toggle of reset
#flow package option set /SCVerify/ENABLE_RESET_TOGGLE true
# Turn on to enable systematic STALL_FLAG toggling (requires STALL_FLAG directive to be set)
#flow package option set /SCVerify/ENABLE_STALL_TOGGLE true

flow package require /QuestaSIM
flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true
flow package option set /QuestaSIM/MSIM_DOFILE $sfd/../msim.do

solution file add "$sfd/../dut.h" -type CHEADER
solution file add "$sfd/../testbench.cpp" -type C++ -exclude true

go analyze
solution design set dut -top

go compile
flow run /CDesignChecker/launch_sleccpc_sh ./CDesignChecker/design_checker.sh Custom
