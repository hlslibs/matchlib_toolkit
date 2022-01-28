# Makefile for example 45_vlog_tb_dma_dut

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

# Override CXX to use QuestaSim compiler
CXX = $(MODEL_TECH)/sccom

INCDIRS := -I$(SOURCE_DIR) -I$(SOURCE_DIR)/../../include
# QuestaSim does not like overriding the location of SystemC in the compiler search path
#INCDIRS += -I$(SYSTEMC_HOME)/include -I$(SYSTEMC_HOME)/src
INCDIRS += -I$(CONNECTIONS_HOME)/include
INCDIRS += -I$(MATCHLIB_HOME)/cmod/include
INCDIRS += -I$(BOOST_HOME)/include
INCDIRS += -I$(RAPIDJSON_HOME)/include
INCDIRS += -I$(AC_TYPES)/include
INCDIRS += -I$(AC_SIMUTILS)/include

CPPFLAGS += $(INCDIRS)
CPPFLAGS += -DCONNECTIONS_ACCURATE_SIM -DSC_INCLUDE_DYNAMIC_PROCESSES -DCONNECTIONS_NAMING_ORIGINAL
CPPFLAGS += -DSEGMENT_BURST_SIZE=16 -DSC_INCLUDE_MTI_AC

LIBS += -lsystemc -lpthread

.PHONY: all build run clean sim_clean

# build and run targets depend on simulator
SIM_TARGET = vsim
BUILD_TARGET = sim_sc
ifeq ($(MODEL_TECH),)
ifneq ($(VCS_HOME),)
SIM_TARGET = vcs_sim
BUILD_TARGET = simv
endif
endif

build: $(BUILD_TARGET)

run: $(SIM_TARGET)

all: run

sim_sc: $(wildcard $(SOURCE_DIR)*.h) $(wildcard $(SOURCE_DIR)*.cpp)
ifeq ($(MODEL_TECH),)
	$(error Test requires MODEL_TECH for Questa simulation)
endif
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(LIBDIRS) $(wildcard $(SOURCE_DIR)*.cpp) $(LIBS) \
	&& /bin/date > $@

help: checkvars
	-@echo "Makefile targets:"
	-@echo "  clean     - Clean up from previous make runs"
	-@echo "  all       - Perform all of the targets below"
	-@echo "  build     - Compile SystemC design"
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
	-@echo "  AC_SIMUTILS        = $(AC_SIMUTILS)"
	-@echo "  CXX                = $(CXX)"
	-@echo "  LIBDIRS            = $(LIBDIRS)"
	-@echo "  LD_LIBRARY_PATH    = $(LD_LIBRARY_PATH)"
	-@echo "  MODEL_TECH         = $(MODEL_TECH)"
	-@echo "  VCS_HOME           = $(VCS_HOME)"
	-@echo "  VG_GNU_PACKAGE     = $(VG_GNU_PACKAGE)"
	-@echo ""

sim_clean:
	@rm -rf sim_* work DVEfiles inter.vpd AN.DB simv simv.daidir ucli.key vcs_command.tcl csrc

clean: sim_clean
	@rm -rf *.vcd *.wlf transcript 

%.wlf:
	/bin/sh ../bin/vsim_vcd trace.vcd

view_wave: out.wlf
	$(MODEL_TECH)/vsim $< -nolog -do "add wave -r out:/SystemC/*" -do "wave zoom full"

.PHONY: vsim vsim_gui
vsim: sim_sc
	$(MODEL_TECH)/vlib work
	$(MODEL_TECH)/vlog $(wildcard $(SOURCE_DIR)*.v)
	$(MODEL_TECH)/sccom -link
	$(MODEL_TECH)/vsim -c -do "do $(SOURCE_DIR)run.do"

vsim_gui: sim_sc
	$(MODEL_TECH)/vlib work
	$(MODEL_TECH)/vlog $(wildcard $(SOURCE_DIR)*.v)
	$(MODEL_TECH)/sccom -link
	$(MODEL_TECH)/vsim -gui -do "do $(SOURCE_DIR)run.do"

# VCS-MX Support
simv:
ifeq ($(VCS_HOME),)
	$(error Test requires VCS_HOME for VCS-MX simulation)
endif
ifeq ($(VG_GNU_PACKAGE),)
	$(error Test requires VG_GNU_PACKAGE for VCS-MX simulation)
endif
	@echo "Note: VCS-MX compilation will use the g++ compiler from VG_GNU_PACKAGE=$(VG_GNU_PACKAGE)"
	. $(VG_GNU_PACKAGE)/source_me_64.sh; $(VCS_HOME)/bin/syscan -cpp g++ -cflags "$(CXXFLAGS) $(CPPFLAGS)"  $(SOURCE_DIR)dma_wrap.cpp:dma_wrap -debug_access+r
	. $(VG_GNU_PACKAGE)/source_me_64.sh; $(VCS_HOME)/bin/syscan -cpp g++ -cflags "$(CXXFLAGS) $(CPPFLAGS)"  $(SOURCE_DIR)ram_wrap.cpp:ram_wrap -debug_access+r
	. $(VG_GNU_PACKAGE)/source_me_64.sh; $(VCS_HOME)/bin/vcs -cpp g++ -sysc $(SOURCE_DIR)testbench.v -debug_access+r +vcs+lic+wait -licqueue -debug_all

.PHONY: vcs_sim
vcs_sim: simv
	@echo "run" >vcs_command.tcl
	@echo "quit" >>vcs_command.tcl
	./simv -ucli -i vcs_command.tcl

.PHONY: vcs_sim_gui
vcs_sim_gui: simv
	./simv -gui


