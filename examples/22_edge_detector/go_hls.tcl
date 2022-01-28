set sfd [file dir [info script]]

options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags {-DCONNECTIONS_ACCURATE_SIM -DNO_DISPLAY -DCONNECTIONS_NAMING_ORIGINAL}
options set /Input/SearchPath $sfd -append
options set /Input/SearchPath $sfd/../../include -append

# Workaround for CCOV bug - it doesn't expand out MGC_HOME, so do it literally
#options set /Input/SearchPath {$MGC_HOME/shared/include/bmpUtil} -append
#options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append
set MGC_HOME [application get /SYSTEM/ENV_MGC_HOME]
options set /Input/SearchPath $MGC_HOME/shared/include/bmpUtil -append
options set /Input/SearchPath $MGC_HOME/shared/pkgs/matchlib/cmod/include -append

project new

# SCVerify Flow
# ==========================
flow package require /SCVerify 
flow package option set /SCVerify/INVOKE_ARGS "$sfd/people_gray.bmp trace"

# CCoverage Flow
# ==========================
flow package require /CCOV
flow package option set /CCOV/TB_INVOKE_ARGS "$sfd/people_gray.bmp trace"
# This should have been implied
flow package option set /CCOV/COMP_FLAGS {-DSC_INCLUDE_DYNAMIC_PROCESSES}

# CDesignChecker Flow
# ==========================
flow package require /CDesignChecker
flow package option set /CDesignChecker/VERIFICATION_MODE code_checks

# UVM Flow
# ==========================
flow package require /UVM

# QuestaSim Flow
# ==========================
flow package require /QuestaSIM
flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true

# Compile Design
# ==========================
solution file add "$sfd/edge_hierarchy.h" -type CHEADER
solution file add "$sfd/sc_main.cpp" -type C++ -exclude true
solution file add {$MGC_HOME/shared/include/bmpUtil/bmp_io.cpp} -type C++ -exclude true

go analyze
directive set -DESIGN_HIERARCHY {EdgeHierarchy}

go compile

# Run CDesignChecker 
# ==========================
# flow run /CDesignChecker/launch_sleccpc_sh ./CDesignChecker/design_checker.sh -code_checks 

# Finish Synthesis
# ==========================
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm
solution library add ccs_sample_mem

go libraries

# Clock Name/Polarity and Reset Name/Kind/Polarity are hard-coded in the SystemC design
#   'clk' rising edge
#   'rst_bar' active-low asynchronous reset
directive set -CLOCKS {clk {-CLOCK_PERIOD 2.0}}

go assembly
directive set /EdgeHierarchy/MagnitudeAngle -DESIGN_GOAL latency
directive set /EdgeHierarchy/MagnitudeAngle/while -PIPELINE_INIT_INTERVAL 0
directive set /EdgeHierarchy/MagnitudeAngle/ac_math::ac_atan2_cordic<9,9,AC_TRN,AC_WRAP,9,9,AC_TRN,AC_WRAP,8,3,AC_TRN,AC_WRAP>:for -UNROLL yes
directive set /EdgeHierarchy/MagnitudeAngle/while -PIPELINE_STALL_MODE flush
directive set /EdgeHierarchy/VerticalGradient/line_buf1:rsc -MAP_TO_MODULE ccs_sample_mem.ccs_ram_sync_singleport
directive set /EdgeHierarchy/VerticalGradient/line_buf0:rsc -MAP_TO_MODULE ccs_sample_mem.ccs_ram_sync_singleport
directive set /EdgeHierarchy/VerticalGradient/VROW -PIPELINE_INIT_INTERVAL 1
directive set /EdgeHierarchy/HorizontalGradient/VROW -PIPELINE_INIT_INTERVAL 1
directive set /EdgeHierarchy/MagnitudeAngle/VROW -PIPELINE_INIT_INTERVAL 1

go assembly
go architect
go allocate
go extract

# Run SCVerify verification using QuestaSim
# ==========================
# flow run /SCVerify/launch_make scverify/Verify_rtl_v_msim.mk sim

# Run UVM simulation (random sequences, no predictor)
# ==========================
# flow run /UVM/launch_make ../../UVM/project_benches/EdgeHierarchy_bench/sim/Makefile cli
