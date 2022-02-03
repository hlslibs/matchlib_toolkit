set sfd [file normalize [file join [file dir [info script]] ../20_DCT_sysc]]

options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags {-DCONNECTIONS_NAMING_ORIGINAL -DCONNECTIONS_ACCURATE_SIM -DTOP_PIXELPIPE_MOD}
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


solution file add "$sfd/dct.cpp"
solution file add "$sfd/jpeg.cpp"
solution file add "$sfd/huff.cpp"
solution file add "$sfd/bmp.cpp" -exclude true
solution file add "$sfd/bitstream.cpp" -exclude true
solution file add "$sfd/testbench.cpp" -exclude true
solution file add "$sfd/sc_main.cpp" -exclude true

directive set -STRICT_MIO_SCHEDULING false

go analyze
directive set -DESIGN_HIERARCHY pixelpipe_mod

go compile
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm
solution library add RAM_1R1W

go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 5.0}}

go assembly
directive set /pixelpipe_mod/dct/dct_h/while -PIPELINE_INIT_INTERVAL 8
directive set /pixelpipe_mod/dct/dct_h/COPY_ROW0 -UNROLL yes
directive set /pixelpipe_mod/dct/dct_h/COL0 -UNROLL yes
directive set /pixelpipe_mod/dct/dct_h/PRE_ADD -UNROLL yes
directive set /pixelpipe_mod/dct/dct_h/MAC -UNROLL yes
directive set /pixelpipe_mod/dct/dct_v/while -PIPELINE_INIT_INTERVAL 8
directive set /pixelpipe_mod/dct/dct_v/COPY_ROW1 -UNROLL yes
directive set /pixelpipe_mod/dct/dct_v/ROW1 -UNROLL yes
directive set /pixelpipe_mod/dct/dct_v/PRE_ADD -UNROLL yes
directive set /pixelpipe_mod/dct/dct_v/MAC -UNROLL yes
directive set /pixelpipe_mod/quantize_exec -DESIGN_GOAL latency

go architect
go allocate
go extract

