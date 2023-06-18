HDL_DIR := ../../hdl

SIM ?= verilator

# Extra arguments to pass to verilator
EXTRA_ARGS += --trace --trace-structs

# RTL language for the top level module.
#
# NOTE: use verilog for SystemVerilog
TOPLEVEL_LANG ?= verilog

# List of .v and .sv files
VERILOG_SOURCES := $(HDL_DIR)/task_icd_pkg.sv
VERILOG_SOURCES += $(HDL_DIR)/cmd_icd_pkg.sv
VERILOG_SOURCES += $(HDL_DIR)/out_cmd.sv

# Top level module for simulation
TOPLEVEL = out_cmd

# Name of the python test bench
MODULE = testbench

# GTK Wave display file
GTWK := $(TOPLEVEL)_dump.gtkw

# Verilator linting flags
LINT_FLAGS := -Wpedantic
LINT_FLAGS += -Wwarn-lint
LINT_FLAGS += -Wwarn-style

all: sim

.PHONY: lint
lint:
	@verilator --lint-only $(LINT_FLAGS) $(VERILOG_SOURCES)

.PHONY:wave
wave: dump.vcd
	gtkwave $(GTWK)

dump.vcd: sim

include $(shell cocotb-config --makefiles)/Makefile.sim
