# Makefile for example 04_ready_valid

CXXFLAGS += -g -std=c++11 -Wall -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-label

# =====================================================================
# ENVIRONMENT VARIABLES
#
# The following environment variables will specify paths
# to open-source repositories that are also included in
# a Catapult install tree.
# If you are using Catapult (i.e. if CATAPULT_HOME or MGC_HOME is set)
# then you do not need to define these environment variables.
# If, however, you wish to point to your own github clone
# of any of these repositories, then define the appropriate
# environment variable.

# If CATAPULT_HOME not set, use value of MGC_HOME for backward compatibility.
CATAPULT_HOME ?= $(MGC_HOME)

ifneq "$(CATAPULT_HOME)" ""

# Pick up SystemC via "SYSTEMC_HOME"
ifneq "$(SYSTEMC_HOME)" ""
$(warning - Warning: SYSTEMC_HOME and MGC_HOME/CATAPULT_HOME are both set. Using SystemC from MGC_HOME/CATAPULT_HOME)
endif
SYSTEMC_HOME := $(CATAPULT_HOME)/shared

# Pick up Connections via "CONNECTIONS_HOME"
CONNECTIONS_HOME ?= $(CATAPULT_HOME)/shared

# Pick up MatchLib via "MATCHLIB_HOME"
MATCHLIB_HOME ?= $(CATAPULT_HOME)/shared/pkgs/matchlib

# Pick up Boost Preprocessor via "BOOST_HOME"
BOOST_HOME ?= $(CATAPULT_HOME)/shared/pkgs/boostpp/pp

# Pick up RapidJSON via "RAPIDJSON_HOME"
RAPIDJSON_HOME ?= $(CATAPULT_HOME)/shared

# Pick up AC Datatypes via "AC_TYPES"
AC_TYPES ?= $(CATAPULT_HOME)/shared

# Pick up AC Simutils via "AC_SIMUTILS"
AC_SIMUTILS ?= $(CATAPULT_HOME)/shared

# Pick up C++ compiler
CXX := $(CATAPULT_HOME)/bin/g++
LD_LIBRARY_PATH := $(if $(LD_LIBRARY_PATH),$(LD_LIBRARY_PATH):$(CATAPULT_HOME)/lib:$(CATAPULT_HOME)/shared/lib,$(CATAPULT_HOME)/lib:$(CATAPULT_HOME)/shared/lib)
export LD_LIBRARY_PATH
LIBDIRS += -L$(CATAPULT_HOME)/lib -L$(CATAPULT_HOME)/shared/lib

else

# CATAPULT_HOME appears to not be set. Make sure required variables are defined

ifndef SYSTEMC_HOME
$(error - Environment variable SYSTEMC_HOME must be defined)
endif
ifndef CONNECTIONS_HOME
$(error - Environment variable CONNECTIONS_HOME must be defined)
endif
ifndef MATCHLIB_HOME
$(error - Environment variable MATCHLIB_HOME must be defined)
endif
ifndef BOOST_HOME
$(error - Environment variable BOOST_HOME must be defined)
endif
ifndef RAPIDJSON_HOME
$(error - Environment variable RAPIDJSON_HOME must be defined)
endif
ifndef AC_TYPES
$(error - Environment variable AC_TYPES must be defined)
endif
ifndef AC_SIMUTILS
$(error - Environment variable AC_SIMUTILS must be defined)
endif

# Default to the compiler installed on the machine
CXX ?= g++
LD_LIBRARY_PATH := $(if $(LD_LIBRARY_PATH),$(LD_LIBRARY_PATH):$(SYSTEMC_HOME)/lib:$(SYSTEMC_HOME)/lib-linux64,$(SYSTEMC_HOME)/lib:$(SYSTEMC_HOME)/lib-linux64)
export LD_LIBRARY_PATH
LIBDIRS += -L$(SYSTEMC_HOME)/lib -L$(SYSTEMC_HOME)/lib-linux64

endif

# ---------------------------------------------------------------------

# Check: $(SYSTEMC_HOME)/include/systemc.h must exist
checkvar_SYSTEMC_HOME: $(SYSTEMC_HOME)/include/systemc.h

# Check: $(CONNECTIONS_HOME)/include/connections/connections.h must exist
checkvar_CONNECTIONS_HOME: $(CONNECTIONS_HOME)/include/connections/connections.h

# Check: $(MATCHLIB_HOME)/cmod/include/nvhls_marshaller.h
checkvar_MATCHLIB_HOME: $(MATCHLIB_HOME)/cmod/include/nvhls_marshaller.h

# Check: $(BOOST_HOME)/include/boost/preprocessor/arithmetic/add.hpp
checkvar_BOOST_HOME: $(BOOST_HOME)/include/boost/preprocessor/arithmetic/add.hpp

# Check: $(RAPIDJSON_HOME)/include/rapidjson/document.h
checkvar_RAPIDJSON_HOME: $(RAPIDJSON_HOME)/include/rapidjson/document.h

# Check: $(AC_TYPES)/include/ac_int.h
checkvar_AC_TYPES: $(AC_TYPES)/include/ac_int.h

# Check: $(AC_SIMUTILS)/include/mc_scverify.h
checkvar_AC_SIMUTILS: $(AC_SIMUTILS)/include/mc_scverify.h

# Rule to check that environment variables are set correctly
checkvars: checkvar_SYSTEMC_HOME checkvar_CONNECTIONS_HOME checkvar_MATCHLIB_HOME checkvar_BOOST_HOME checkvar_RAPIDJSON_HOME checkvar_AC_TYPES checkvar_AC_SIMUTILS
# =====================================================================

export CATAPULT_HOME SYSTEMC_HOME CONNECTIONS_HOME MATCHLIB_HOME BOOST_HOME RAPIDJSON_HOME AC_TYPES AC_SIMUTILS
export DUT    := dut
# Determine the director containing the source files from the path to this Makefile
PWD := $(shell pwd)
SOURCE_DIR1 = $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
SOURCE_DIR = $(if $(subst ./,,$(SOURCE_DIR1))../,$(SOURCE_DIR1)../,$(PWD)/../)

INCDIRS := -I$(SOURCE_DIR) -I$(SOURCE_DIR)/../../include
INCDIRS += -I$(SYSTEMC_HOME)/include -I$(SYSTEMC_HOME)/src
INCDIRS += -I$(CONNECTIONS_HOME)/include
INCDIRS += -I$(MATCHLIB_HOME)/cmod/include
INCDIRS += -I$(BOOST_HOME)/include
INCDIRS += -I$(RAPIDJSON_HOME)/include
INCDIRS += -I$(AC_TYPES)/include
INCDIRS += -I$(AC_SIMUTILS)/include

CPPFLAGS += $(INCDIRS)
CPPFLAGS += -DSC_INCLUDE_DYNAMIC_PROCESSES

LIBS += -lsystemc -lpthread

.PHONY: all build run clean
build: sim_sc

all: run cat_cdc run_ccov run_ccovercheck

run: trace.vcd

trace.vcd: sim_sc
	-@echo "Starting execution in directory `pwd`"
	./$^

sim_sc: $(wildcard $(SOURCE_DIR)*.h) $(wildcard $(SOURCE_DIR)*.cpp)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIRS) $(wildcard $(SOURCE_DIR)*.cpp) -o $@ $(LIBS)

# These two targets assume that the QuestaSim utilities 'vcd2wlf' and 'vsim'
# are found in your PATH
%.wlf: %.vcd
	vcd2wlf $< $@

view_wave: trace.wlf
	vsim $< -nolog -do "add wave -r trace:/SystemC/*" -do "wave zoom full"

###################
###  Catapult   ###
###################
cat_cdc_all: cat_clean cat_cli

cat_cdc:
	$(CATAPULT_HOME)/bin/catapult -product cpc -file go_cdc.tcl -logfile catapult.log -shell
cat_clean:
	@rm -rf Catapult catapult.log design_checker_constraints.tcl design_checker_pre_build.tcl design_checker_waivers.tcl

###################
###    CCOV     ###
###################
export CCOV_CONSTRAINTS_FILE := $(wildcard $(SOURCE_DIR1)/cov_files/ccov*.xml)

ABS_CATAPULT_HOME   := $(realpath $(CATAPULT_HOME))
TESTNAME	    ?= test_top
VCOV_HTML_REPORT_OPTS ?= -details -annotate

sim_ccov: $(CCOV_CONSTRAINTS_FILE) $(wildcard $(SOURCE_DIR)*.h) $(wildcard $(SOURCE_DIR)*.cpp)
	@-echo "Compiling design instrumented for CCOV"
	$(CCOV_HOME)/bin/ccov $(CXXFLAGS) $(CPPFLAGS) $(LIBDIRS) $(wildcard $(SOURCE_DIR)*.cpp) -o $@ $(LIBS)

$(TESTNAME).ucdb: sim_ccov
	@-echo "Generating coverage data"
	./$^ 
	$(CCOV_HOME)/bin/ccov --gen_ucdb -testname $(TESTNAME) -output $(TESTNAME).ucdb $(DUT_MERGE_OPTS)

ifneq "$(QUESTA_HOME)" ""
$(TESTNAME).rpt: $(TESTNAME).ucdb
	$(QUESTA_HOME)/bin/vcover report -output $(TESTNAME).rpt $(TESTNAME).ucdb
	$(QUESTA_HOME)/bin/vcover report -html -output html_report $(VCOV_HTML_REPORT_OPTS) $(TESTNAME).ucdb
	$(QUESTA_HOME)/bin/vcover report -summary $(TESTNAME).ucdb

ccov_viewcov:
	$(QUESTA_HOME)/bin/vis -ucdbfile $(TESTNAME).ucdb

ccov_viewcov_html:
	firefox html_report/index.html
else
$(TESTNAME).rpt:
	@-echo "To generated CCOV reports and UCDB requires a QuestaSim install. Set the QUESTA_HOME environment variable"
ccov_viewcov:
	@-echo "Viewing CCOV results within Visualizer requires a Visualizer install. Set the QUESTA_HOME environment variable"

ccov_viewcov_html:
	@-echo "Viewing CCOV results requires a QuestaSim install. Set the QUESTA_HOME environment variable"
endif

ifneq "$(CCOV_HOME)" ""
run_ccov: $(TESTNAME).rpt
else
run_ccov:
	@-echo "Running CCOV requires a Catapult Coverage (CCOV) install. Set the CCOV_HOME environment variable"
endif

ccov_clean:
	@rm -rf cov_data $(TESTNAME).ucdb $(TESTNAME).rpt sysinfo.log visualizer.log html_report sim_ccov .visualizer

###################
### CCOVERCHECK ###
###################

run_ccovercheck: ccovercheck_clean ccovercheck_run ccovercheck_apply 

ifneq "$(SLEC_HOME)" ""
ccovercheck_run:
	$(SLEC_HOME)/bin/ccovercheck -u $(TESTNAME).ucdb \
     -cat_db ./Catapult/$(DUT).v1/CDesignChecker/.design_checker_db
else
ccovercheck_run:
	@-echo "Running CCoverCheck requires a Catapult Formal CoverCheck (CCoverCheck) install. Set the SLEC_HOME environment variable"
endif

ifneq "$(QUESTA_HOME)" ""
ccovercheck_apply:
	$(QUESTA_HOME)/bin/vsim -l questa_transcript -c -viewcov $(TESTNAME).ucdb -do "do CCoverCheck/calypto/ccheck_exclusions.do; coverage save $(TESTNAME)_ccovercheck_exclusions.ucdb; exit"
	$(QUESTA_HOME)/bin/vcover report -summary $(TESTNAME)_ccovercheck_exclusions.ucdb

ccovercheck_viewcov:
	$(QUESTA_HOME)/bin/vis -ucdbfile $(TESTNAME)_ccovercheck_exclusions.ucdb
else
ccovercheck_apply:
	@-echo "Applying exclusions requires a QuestaSim install. Set the QUESTA_HOME environment variable"

ccovercheck_viewcov:
	@-echo "Viewing CCoverCheck results within Visualizer requires a Visualizer install. Set the QUESTA_HOME environment variable"
endif

ccovercheck_clean:
	@rm -rf CCoverCheck CCoverCheck_1 $(TESTNAME)_ccovercheck_exclusions.ucdb questa_transcript 

#============================================================================================================

help: checkvars
	-@echo "Makefile targets:"
	-@echo "  clean     - Clean up from previous make runs"
	-@echo "  all       - Perform all of the targets below"
	-@echo "  sim_sc    - Compile SystemC design"
	-@echo "  run       - Execute SystemC design and generate trace.vcd"
	-@echo "  view_wave - Convert trace.vcd to QuestaSim wlf file and view in QuestaSim"
	-@echo "  run_ccov           : Compile design with CCOV and run instrumented design"
	-@echo "  ccov_viewcov       : View UCDB file in Visualizer."
	-@echo "  ccov_viewcov_html  : View UCDB file in browser."
	-@echo "  cat_cdc            : invoke catapult in batch mode and execute the provided go_cdc.tcl directives file "
	-@echo "  run_ccovercheck        : Run CCoverCheck and apply exclusions."
	-@echo "  ccovercheck_viewcov    : View UCDB file in Visualizer after applying exclusions."
	-@echo ""
	-@echo "Environment/Makefile Variables:"
	-@echo "  CATAPULT_HOME      = $(CATAPULT_HOME)"
	-@echo "  SYSTEMC_HOME       = $(SYSTEMC_HOME)"
	-@echo "  CONNECTIONS_HOME   = $(CONNECTIONS_HOME)"
	-@echo "  MATCHLIB_HOME      = $(MATCHLIB_HOME)"
	-@echo "  BOOST_HOME         = $(BOOST_HOME)"
	-@echo "  RAPIDJSON_HOME     = $(RAPIDJSON_HOME)"
	-@echo "  AC_TYPES           = $(AC_TYPES)"
	-@echo "  AC_SIMUTILS        = $(AC_SIMUTILS)"
	-@echo "  CXX                = $(CXX)"
	-@echo "  LIBDIRS            = $(LIBDIRS)"
	-@echo "  LD_LIBRARY_PATH    = $(LD_LIBRARY_PATH)"
	-@echo "  CCOV_HOME          = $(CCOV_HOME)"
	-@echo "  SLEC_HOME          = $(SLEC_HOME)"
	-@echo "  QUESTA_HOME        = $(QUESTA_HOME)"
	-@echo ""

clean: ccov_clean cat_clean ccovercheck_clean
	@rm -rf sim_sc trace.vcd trace.wlf chan_log_data.txt chan_log_names.txt

