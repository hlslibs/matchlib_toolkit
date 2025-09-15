set sfd [file dir [info script]]

options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags {-DCONNECTIONS_ACCURATE_SIM -DSEGMENT_BURST_SIZE=16}
options set /Input/SearchPath {$MGC_HOME/shared/examples/matchlib/toolkit/include} -append
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/boostpp/pp/include} -append
options set /Input/SearchPath {../} -append

project new
solution file add "$sfd/../testbench.cpp" -type C++

go analyze
solution design set dma -top

go compile
flow run /CDesignChecker/launch_sleccpc_sh ./CDesignChecker/design_checker.sh Custom
