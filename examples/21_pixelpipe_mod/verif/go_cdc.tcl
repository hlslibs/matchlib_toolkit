set sfd [file normalize [file join [file dir [info script]] ../../20_DCT_sysc]]

options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags {-DCONNECTIONS_ACCURATE_SIM -DTOP_PIXELPIPE_MOD}
options set /Input/SearchPath {$MGC_HOME/shared/examples/matchlib/toolkit/include} -append
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append
options set /Input/SearchPath "$sfd/RAM_lib" -append
options set /ComponentLibs/SearchPath "$sfd/RAM_lib" -append

project new

flow package require /SCVerify 
flow package option set /SCVerify/INVOKE_ARGS "$sfd/sample.bmp trace"
# Set to a non-zero number to enable automatic random stall injection on handshake interfaces
#flow package option set /SCVerify/AUTOWAIT 0
# Allow initial toggle of reset
#flow package option set /SCVerify/ENABLE_RESET_TOGGLE true
# Turn on to enable systematic STALL_FLAG toggling (requires STALL_FLAG directive to be set)
#flow package option set /SCVerify/ENABLE_STALL_TOGGLE true

flow package require /QuestaSIM
flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true
flow package option set /QuestaSIM/MSIM_DOFILE $sfd/msim.do


solution file add "$sfd/dct.cpp"
solution file add "$sfd/jpeg.cpp"
solution file add "$sfd/huff.cpp"
solution file add "$sfd/bmp.cpp" -exclude true
solution file add "$sfd/bitstream.cpp" -exclude true
solution file add "$sfd/testbench.cpp" -exclude true
solution file add "$sfd/sc_main.cpp" -exclude true

directive set -STRICT_MIO_SCHEDULING false

go analyze
solution design set pixelpipe_mod -top

go compile
flow run /CDesignChecker/launch_sleccpc_sh ./CDesignChecker/design_checker.sh Custom
