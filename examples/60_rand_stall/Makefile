# Makefile for example 60_rand_stall
CXX = g++

CXXFLAGS += -g -std=c++11 -Wall -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-label -Wno-maybe-uninitialized

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

# Determine the director containing the source files from the path to this Makefile
PWD := $(shell pwd)
SOURCE_DIR1 = $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
SOURCE_DIR = $(if $(subst ./,,$(SOURCE_DIR1)),$(SOURCE_DIR1),$(PWD)/)

INCDIRS := -I$(SOURCE_DIR) -I$(SOURCE_DIR)/../../include
INCDIRS += -I$(SYSTEMC_HOME)/include -I$(SYSTEMC_HOME)/src
INCDIRS += -I$(CONNECTIONS_HOME)/include
INCDIRS += -I$(MATCHLIB_HOME)/cmod/include
INCDIRS += -I$(BOOST_HOME)/include
INCDIRS += -I$(RAPIDJSON_HOME)/include
INCDIRS += -I$(AC_TYPES)/include
INCDIRS += -I$(AC_SIMUTILS)/include

CPPFLAGS += $(INCDIRS)
CPPFLAGS += -DCONNECTIONS_ACCURATE_SIM -DSC_INCLUDE_DYNAMIC_PROCESSES -DCONNECTIONS_NAMING_ORIGINAL
CPPFLAGS += -DSEGMENT_BURST_SIZE=2

LIBS += -lsystemc -lpthread

.PHONY: all build run clean sim_clean
build: sim_stall sim_no_stall

all: build run diff

run: stall no_stall

sim_stall: CPPFLAGS += -DCONN_RAND_STALL

sim_%: $(wildcard $(SOURCE_DIR)*.h) $(wildcard $(SOURCE_DIR)*.cpp)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LIBDIRS) $(wildcard $(SOURCE_DIR)*.cpp) -o $@ $(LIBS)

.PHONY: stall no_stall

stall: sim_stall
	$(runit)

no_stall: sim_no_stall
	$(runit)

run_ddd_no_stall:
	ddd sim_no_stall

run_ddd_stall:
	ddd sim_stall

define runit
	@rm -rf $@_log; mkdir $@_log
	./sim_$@ && mv trace.vcd $@_log/trace.vcd
	/bin/sh $(SOURCE_DIR)../bin/gen_logs.sh $@_log && mv $@*.* $@_log
endef

sim_clean:
	@rm -f sim_no_stall sim_stall stall_log_data.txt stall_log_names.txt
	@rm -rf stall_log no_stall_log

clean: sim_clean

stall_wave: 
	@/bin/sh $(SOURCE_DIR)../bin/vsim_vcd stall_log/trace.vcd
	vsim stall_log/out.wlf -nolog -do "add wave -r out:/SystemC/*" -do "wave zoom full"

no_stall_wave:
	@/bin/sh $(SOURCE_DIR)../bin/vsim_vcd no_stall_log/trace.vcd
	vsim  no_stall_log/out.wlf -nolog -do "add wave -r out:/SystemC/*" -do "wave zoom full"

diff:
	@/bin/bash $(SOURCE_DIR)../bin/earliest_diff.sh no_stall_log stall_log
	@echo Finished difference summary

help: checkvars
	-@echo "Makefile targets:"
	-@echo "  clean     - Clean up from previous make runs"
	-@echo "  all       - Perform all of the targets below"
	-@echo "  sim_sc    - Compile SystemC design"
	-@echo "  run       - Execute SystemC design and generate trace.vcd"
	-@echo "  view_wave - Convert trace.vcd to QuestaSim wlf file and view in QuestaSim"
	-@echo ""
	-@echo "SOURCE_DIR = $(SOURCE_DIR)"
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
	-@echo ""

