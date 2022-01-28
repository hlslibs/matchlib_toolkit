# Makefile for example 22_edge_detector

CXXFLAGS += -O2 -g -std=c++11 -Wall -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-label -Wno-write-strings -Wno-reorder -Wno-maybe-uninitialized

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

# Pick up AC Datatypes via "AC_MATH"
AC_MATH ?= $(CATAPULT_HOME)/shared

# Pick up AC Simutils via "AC_SIMUTILS"
AC_SIMUTILS ?= $(CATAPULT_HOME)/shared

# Pick up bitmap sources via "BMP_IO"
BMP_IO ?= $(CATAPULT_HOME)/shared/include/bmpUtil

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
ifndef AC_MATH
$(error - Environment variable AC_MATH must be defined)
endif
ifndef AC_SIMUTILS
$(error - Environment variable AC_SIMUTILS must be defined)
endif
ifndef BMP_IO
$(error - Environment variable BMP_IO must be defined)
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

# Check: $(AC_MATH)/include/ac_math.h
checkvar_AC_MATH: $(AC_MATH)/include/ac_math.h

# Check: $(AC_SIMUTILS)/include/mc_scverify.h
checkvar_AC_SIMUTILS: $(AC_SIMUTILS)/include/mc_scverify.h

# Check: $(BMP_IO)/bmp_io.cpp
checkvar_BMP_IO: $(BMP_IO)/bmp_io.cpp

# Rule to check that environment variables are set correctly
checkvars: checkvar_SYSTEMC_HOME checkvar_CONNECTIONS_HOME checkvar_MATCHLIB_HOME checkvar_BOOST_HOME checkvar_RAPIDJSON_HOME checkvar_AC_TYPES checkvar_AC_MATH checkvar_AC_SIMUTILS checkvar_BMP_IO
# =====================================================================

export CATAPULT_HOME SYSTEMC_HOME CONNECTIONS_HOME MATCHLIB_HOME BOOST_HOME RAPIDJSON_HOME AC_TYPES AC_MATH AC_SIMUTILS

# Determine the director containing the source files from the path to this Makefile
PWD := $(shell pwd)
SOURCE_DIR1 = $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
SOURCE_DIR = $(if $(subst ./,,$(SOURCE_DIR1)),$(SOURCE_DIR1),$(PWD)/)

BMP_SRC = $(BMP_IO)/bmp_io.cpp

INCDIRS := -I$(SOURCE_DIR) -I$(SOURCE_DIR)/../../include
INCDIRS += -I$(SYSTEMC_HOME)/include -I$(SYSTEMC_HOME)/src
INCDIRS += -I$(CONNECTIONS_HOME)/include
INCDIRS += -I$(MATCHLIB_HOME)/cmod/include
INCDIRS += -I$(BOOST_HOME)/include
INCDIRS += -I$(RAPIDJSON_HOME)/include
INCDIRS += -I$(AC_TYPES)/include
INCDIRS += -I$(AC_MATH)/include
INCDIRS += -I$(AC_SIMUTILS)/include
INCDIRS += -I$(BMP_IO)

CPPFLAGS += $(INCDIRS)
CPPFLAGS += -DCONNECTIONS_ACCURATE_SIM -DSC_INCLUDE_DYNAMIC_PROCESSES -DCONNECTIONS_NAMING_ORIGINAL

# Uncomment this flag if you want to see the input and output images displayed
#CPPFLAGS += -DHAVE_DISPLAY

LIBS += -lsystemc -lpthread

.PHONY: all build run clean sim_clean
build: sim_sc

all: run

run: trace.vcd

trace.vcd: sim_sc
	-@echo "Starting execution in directory `pwd`"
	./$^ $(SOURCE_DIR)people_gray.bmp trace

sim_sc: $(wildcard $(SOURCE_DIR)*.h) $(wildcard $(SOURCE_DIR)*.cpp)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIRS) $(wildcard $(SOURCE_DIR)*.cpp) $(BMP_SRC) -o $@ $(LIBS)

#============================================================================================================
# These variables/targets allow running CCoverage on the HLS SystemC MatchLib model
export CCOV_COMPILE_DIR       := ccov/CCOV_COMPILE
export CCOV_STATS_DIR         := ccov/CCOV_STATS
export CCOV_CONSTRAINTS_FILE  := ccov_constraints.xml
export CCOV_USER_SYSTEMC_HOME := $(SYSTEMC_HOME)
export CCOV_USE_USER_GCC
export CCOV_COVER_FEC_EXPRS
# note: following is temporary
export CCOV_KEEP_INSTRUMENTED_FILES
ABS_CATAPULT_HOME := $(realpath $(CATAPULT_HOME))
TEST_NAME         ?= test_top
CCOV_DATA         := $(CCOV_STATS_DIR)/-1.ccov.cda
VCOV_HTML_REPORT_OPTS ?= -details -annotate -code sbet -cvg
SRC_FILES         := .

# Generate constraints file for CCOV
$(CCOV_CONSTRAINTS_FILE):
	@-echo "Generating CCOV constraints file '$@'"
	@-echo '<constraints>' >$@
	@-echo '<path_cov_constraint include="0" token="$$CATAPULT_HOME/shared/include" />' >>$@
	@-echo '<path_cov_constraint include="0" token="$$CATAPULT_HOME/shared/pkgs/matchlib/cmod/include" />' >>$@
	@-echo '<path_cov_constraint include="0" token="$$CATAPULT_HOME/shared/examples/matchlib/toolkit/include" />' >>$@
	@-echo '<path_cov_constraint include="0" token="$$CATAPULT_HOME/shared/include/bmpUtil" />' >>$@
	@-echo '<path_cov_constraint include="0" token="'$(ABS_CATAPULT_HOME)'/shared/include/bmpUtil" />' >>$@
	@-echo '<path_cov_constraint include="0" token="$$SYSTEMC_HOME/include/systemc" />' >>$@
	@-echo '<path_cov_constraint include="0" token="$$SYSTEMC_HOME/include" />' >>$@
	@-echo '<file_cov_constraint include="1" token="./EdgeHierarchy_wrap.cpp" />' >>$@
	@-echo '<path_cov_constraint include="0" token="'$(realpath $(ABS_CATAPULT_HOME))'/shared/include/bmpUtil/bmp_io.cpp" />' >>$@
	@-echo '<path_cov_constraint include="0" token="'$(realpath $(ABS_CATAPULT_HOME)/shared/include/bmpUtil/bmp_io.cpp)'" />' >>$@
	@-echo '<file_cov_constraint include="0" token="'$(realpath $(SRC_FILES))'/sc_main.cpp" />' >>$@
	@-echo '<file_cov_constraint include="0" token="'$(realpath $(SRC_FILES))'/top.h" />' >>$@
	@-echo '<file_cov_constraint include="0" token="'$(realpath $(SRC_FILES))'/testbench.h" />' >>$@
	@-echo '<file_cov_constraint include="0" token="'$(SRC_FILES)'/edge_alg.h" />' >>$@
	@-echo '</constraints>' >>$@

sim_sc_ccov: $(CCOV_CONSTRAINTS_FILE) $(wildcard $(SOURCE_DIR)*.h) $(wildcard $(SOURCE_DIR)*.cpp)
	@-echo "Compiling design instrumented for CCOV"
	@-mkdir -p $(CCOV_COMPILE_DIR) $(CCOV_STATS_DIR)
	$(CCOV_HOME)/bin/ccov $(CXXFLAGS) $(CPPFLAGS) $(LIBDIRS) $(wildcard $(SOURCE_DIR)*.cpp) $(BMP_SRC) -o $@ $(LIBS)

$(CCOV_DATA): sim_sc_ccov
	@-echo "Generating coverage data"
	$^ $(SOURCE_DIR)people_gray.bmp trace

$(TEST_NAME).ucdb: $(CCOV_DATA)
	$(CCOV_HOME)/bin/ccov --gen_ucdb -testname $(TEST_NAME) -output $(TEST_NAME).ucdb $(DUT_MERGE_OPTS)

$(TEST_NAME).rpt: $(TEST_NAME).ucdb
	$(QUESTA_HOME)/bin/vcover report -output $(TEST_NAME).rpt $(TEST_NAME).ucdb
	$(QUESTA_HOME)/bin/vcover report -html -output html_report $(VCOV_HTML_REPORT_OPTS) $(TEST_NAME).ucdb

ifneq "$(QUESTA_HOME)" ""
run_ccov: $(TEST_NAME).rpt
view_ccov: run_ccov
	firefox html_report/index.html
else
run_ccov:
	@-echo "Running CCOV requires a QuestaSim install. Set the QUESTA_HOME environment variable"
view_ccov:
	@-echo "Viewing CCOV results requires a QuestaSim install. Set the QUESTA_HOME environment variable"
endif
#============================================================================================================

# These two targets assume that the QuestaSim utilities 'vcd2wlf' and 'vsim'
# are found in your PATH
%.wlf: %.vcd
	vcd2wlf $< $@

view_wave: trace.wlf
	vsim $< -nolog -do "add wave -r trace:/SystemC/*" -do "wave zoom full"

sim_clean:
	@rm -rf sim_* Gradient_Magnitude*.bmp

help: checkvars
	-@echo "Makefile targets:"
	-@echo "  clean     - Clean up from previous make runs"
	-@echo "  all       - Perform all of the targets below"
	-@echo "  sim_sc    - Compile SystemC design"
	-@echo "  run       - Execute SystemC design and generate trace.vcd"
	-@echo "  view_wave - Convert trace.vcd to QuestaSim wlf file and view in QuestaSim"
	-@echo ""
	-@echo "Environment/Makefile Variables:"
	-@echo "  CATAPULT_HOME      = $(CATAPULT_HOME)"
	-@echo "  SYSTEMC_HOME       = $(SYSTEMC_HOME)"
	-@echo "  CONNECTIONS_HOME   = $(CONNECTIONS_HOME)"
	-@echo "  MATCHLIB_HOME      = $(MATCHLIB_HOME)"
	-@echo "  BOOST_HOME         = $(BOOST_HOME)"
	-@echo "  RAPIDJSON_HOME     = $(RAPIDJSON_HOME)"
	-@echo "  AC_TYPES           = $(AC_TYPES)"
	-@echo "  AC_MATH            = $(AC_MATH)"
	-@echo "  AC_SIMUTILS        = $(AC_SIMUTILS)"
	-@echo "  BMP_IO             = $(BMP_IO)"
	-@echo "  CXX                = $(CXX)"
	-@echo "  LIBDIRS            = $(LIBDIRS)"
	-@echo "  LD_LIBRARY_PATH    = $(LD_LIBRARY_PATH)"
	-@echo ""

clean: sim_clean
	@rm -rf *.vcd *.wlf ccov $(CCOV_CONSTRAINTS_FILE) $(TEST_NAME).ucdb $(TEST_NAME).rpt html_report



