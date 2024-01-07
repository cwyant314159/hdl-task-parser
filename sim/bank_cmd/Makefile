# Top level entity under test 
TOPLEVEL := bank_cmd

# Name of the cocotb testbench python script
MODULE   := testbench

# Both the Verilog and VHDL source code for
# this module.
HDL_DIR := ../../hdl

VHDL_FILES := $(HDL_DIR)/vhdl/task_icd_pkg.vhd
VHDL_FILES += $(HDL_DIR)/vhdl/bank_cmd.vhd

VERILOG_FILES := $(HDL_DIR)/sv/task_icd_pkg.sv
VERILOG_FILES += $(HDL_DIR)/sv/cmd_icd_pkg.sv
VERILOG_FILES += $(HDL_DIR)/sv/bank_cmd.sv

# We have to do some monkey business to use this 
# same Makefile for both the VHDL and Verilog 
# versions of the code. To run the simulation with 
# the VHDL source code, the environment variable 
# VHDL must be set to 1 (VHDL=1). If this variable 
# is not set, the Verilog source code will be used 
# during the simulation.
ifeq ($(VHDL),1)
WAVE_FILE     := dump.ghw
SIM           := ghdl
TOPLEVEL_LANG := vhdl
SIM_ARGS      := --wave=$(WAVE_FILE)
EXTRA_ARGS    += --std=08
VHDL_SOURCES  := $(VHDL_FILES)
WAVE_FILE     := 
else
WAVE_FILE       := dump.vcd
SIM             := verilator
TOPLEVEL_LANG   := verilog
EXTRA_ARGS      += --trace --trace-structs
VERILOG_SOURCES := $(VERILOG_FILES)
endif

# Verilator linting flags
VERILATOR_LINT_FLAGS := -Wpedantic
VERILATOR_LINT_FLAGS += -Wwarn-lint
VERILATOR_LINT_FLAGS += -Wwarn-style

# GHDL linting flags 
GHDL_LINT_FLAGS := -a 
GHDL_LINT_FLAGS += --std=08 

all: sim

.PHONY: lint
lint:
	@verilator --lint-only $(VERILATOR_LINT_FLAGS) $(VERILOG_FILES)
	@ghdl $(GHDL_LINT_FLAGS) $(VHDL_FILES)

$(WAVE_FILE): sim

.PHONY:wave
wave: $(WAVE_FILE)
	gtkwave $<

# Add the packages directory to the PYTHONPATH variable before running
# simulation.
sim: export PYTHONPATH := ../packages:$$PYTHONPATH

include $(shell cocotb-config --makefiles)/Makefile.sim
