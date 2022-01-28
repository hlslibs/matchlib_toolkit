
set sfd [file dir [info script]]

options defaults
options set /Input/CppStandard c++11
options set /Input/CompilerFlags {-DCONNECTIONS_ACCURATE_SIM -DSC_INCLUDE_DYNAMIC_PROCESSES}
options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append

project new

# CDesignChecker flow
# ===========
flow package require /CDesignChecker

# CCOV flow
# ===========
flow package require /CCOV
flow package option set /CCOV/EXCLUDE_TB_FILES true
flow package option set /CCOV/CCOV_EXCLUDE_FILES { apb_ram.h }
flow package option set /CCOV/CCOV_HOME $env(CCOV_HOME)

# SCVerify flow
# ===========
flow package require /SCVerify

# QuestaSim
# ===========
flow package require /QuestaSIM
flow package option set /QuestaSIM/ENABLE_CODE_COVERAGE true
flow package option set /QuestaSIM/MSIM_DOFILE $sfd/msim.do

# solution file add
# ===========
solution file add "$sfd/apb_dma.h" -type CHEADER
solution file add "$sfd/testbench.cpp" -type C++ -exclude true

go analyze
directive set -DESIGN_HIERARCHY dma

go compile

# Specify Technology Library
# ===========
solution library add nangate-45nm_beh -- -rtlsyntool OasysRTL -vendor Nangate -technology 045nm

go libraries
directive set -CLOCKS {clk {-CLOCK_PERIOD 2.0}}

go assembly
go architect
go allocate
go extract

