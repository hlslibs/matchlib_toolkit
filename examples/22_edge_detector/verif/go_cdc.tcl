set sfd [file dir [info script]]

options defaults

options set /Input/CppStandard c++11
options set /Input/CompilerFlags {-DCONNECTIONS_ACCURATE_SIM -DNO_DISPLAY }
options set /Input/SearchPath $sfd/.. -append
options set /Input/SearchPath {$MGC_HOME/shared/examples/matchlib/toolkit/include} -append

# Workaround for CCOV bug - it doesn't expand out MGC_HOME, so do it literally
#options set /Input/SearchPath {$MGC_HOME/shared/include/bmpUtil} -append
#options set /Input/SearchPath {$MGC_HOME/shared/pkgs/matchlib/cmod/include} -append
set MGC_HOME [application get /SYSTEM/ENV_MGC_HOME]
options set /Input/SearchPath $MGC_HOME/shared/include/bmpUtil -append
options set /Input/SearchPath $MGC_HOME/shared/pkgs/matchlib/cmod/include -append

project new

# CDesignChecker Flow
# ==========================
flow package require /CDesignChecker

# Compile Design
# ==========================
solution file add "$sfd/../edge_hierarchy.h" -type CHEADER
solution file add "$sfd/../sc_main.cpp" -type C++ -exclude true
solution file add {$MGC_HOME/shared/include/bmpUtil/bmp_io.cpp} -type C++ -exclude true

go analyze
solution design set {EdgeHierarchy} -top

go compile
flow run /CDesignChecker/launch_sleccpc_sh ./CDesignChecker/design_checker.sh Custom
