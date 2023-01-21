# The build tools
CC    := gcc -c -xc
CXX   := gcc -c -xc++
LD    := gcc
AR    := ar
RM    := rm -rf
MKDIR := mkdir -p

# Determine the OS the Makefile is
# running on.
ifneq ($(OS),Windows_NT)
OS := $(shell uname -s)
endif

# The module under test
MODULE := out_cmd

# The output test bench binary
BIN_DIR := bin
BIN     := tb_$(MODULE)

# This is the relative path to the verilator
# support library.
VERI_SUPPORT_ROOT := ../verilator_support
VERI_SUPPORT_LIB  := $(VERI_SUPPORT_ROOT)/lib/libverilator_support.a

# Verilator does not choose very good default
# names for its outputs. To make Verilator
# builds easier to work with, the output
# directory and output file prefix are defined
# with the following variables. The generic name
# "design" was chosen to allow this Makefile to
# be dropped into different projects.
VERI_DIR    := verilate_obj
VERI_PREFIX := tb_$(MODULE)
DUT_LIB     := $(VERI_DIR)/$(VERI_PREFIX)__ALL.a

# C++ include directories and compiler include
# (-I) argument generation.
INC_DIRS :=
INC_DIRS += include
INC_DIRS += $(VERI_SUPPORT_ROOT)/include
INC_DIRS += $(VERI_SUPPORT_ROOT)/include/vltstd
INC_DIRS += $(VERI_DIR)

INC_FLAGS := $(foreach dir, $(INC_DIRS), $(addprefix -I, $(dir)))

# C++ source directory list and source file
# generation. The SRC_FILE variable is created
# by recursively searching the SRC_DIRS for *.c
# and *.cpp files.
SRC_DIRS :=
SRC_DIRS += src

SRC_FILES := $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.cpp'))
SRC_FILES += $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.c'))

# C++ compiled object directory and object file
# list generation.
OBJ_DIR := obj
OBJS    := $(foreach obj, $(SRC_FILES:=.o), $(addprefix $(OBJ_DIR)/, $(obj)))

# This list will be non-empty on subsequent compiles
# assuming a clean was not done.
DEPS := $(shell find . -type f -name '*.d')

# Compiler flags that are common to both
# C and C++.
COMMON_FLAGS :=
COMMON_FLAGS += -Wall
COMMON_FLAGS += -Wextra
COMMON_FLAGS += -Wshadow
COMMON_FLAGS += -Wno-unused-parameter
COMMON_FLAGS += -ffunction-sections
COMMON_FLAGS += -fdata-sections
COMMON_FLAGS += -MMD
COMMON_FLAGS += -MP
COMMON_FLAGS += $(INC_FLAGS)
COMMON_FLAGS += -O2

# C compiler specific flags
CFLAGS := $(COMMON_FLAGS)
CFLAGS += -std=gnu99

# C++ compiler specific flags
CXXFLAGS := $(COMMON_FLAGS)
CXXFLAGS += -std=gnu++14

# C++ linker flags
LDFLAGS := -lstdc++
LDFLAGS += -lm

# The dead code removal linker flag is
# named different on MacOS because Apple.
ifeq ($(OS), Darwin)
	LDFLAGS += -Wl,-dead_strip
else
	LDFLAGS += -Wl,--gc-sections
endif

# The list of SystemVerilog packages.
# Be mindful of any ordering requirements
SV_PKGS := ../../hdl/task_icd_pkg.sv
SV_PKGS += ../../hdl/cmd_icd_pkg.sv

# The list of all SystemVerilog source files. The
# packages are listed first since that is typically
# what's required in a verilator compile. Be mindful
# of module dependencies. A module's dependency should
# come before the module itself.
SV_SRCS := $(SV_PKGS)
SV_SRCS += ../../hdl/$(MODULE).sv

# The list of directories that contain the
# SystemVerilog sources. These are prepended with
# the -I flag for linting and verilation.
SV_DIRS := $(sort $(foreach src, $(SV_SRCS), $(shell dirname $(src))))
SV_INC_DIRS := $(foreach dir, $(SV_DIRS), $(addprefix -I, $(dir)))

# Verilator flags
VERI_FLAGS :=
VERI_FLAGS += -Wall
VERI_FLAGS += -Wpedantic
VERI_FLAGS += -Wwarn-lint
VERI_FLAGS += -Wwarn-style
VERI_FLAGS += $(SV_INC_DIRS) 
VERI_FLAGS += --x-assign unique
VERI_FLAGS += --x-initial unique
VERI_FLAGS += --trace
VERI_FLAGS += --cc
VERI_FLAGS += --Mdir $(VERI_DIR)
VERI_FLAGS += --prefix $(VERI_PREFIX)
VERI_FLAGS += --MMD
VERI_FLAGS += --MP

# This variable is used by the dump
# target to format the output of Make
# variables. Notice how it uses the
# regular equal "=" instead of the
# colon equal ":=". This is done so
# that variable expansion is defered
# until this variable is used.
DUMP_FMT := tr " " "\n" | sed 's/^\(\w\|-\)/    \1/; /^$$/d'

# Build the entire test bench. The
# all target buids the Verilator
# support library and the DUT library
# before building the final test bench
# executable.
all: $(VERI_SUPPORT_LIB) $(DUT_LIB) $(BIN_DIR)/$(BIN) 

# Run the executable
.PHONY: run
run: all
	@$(BIN_DIR)/$(BIN) +verilator+rand+reset+2

# Verilate HDL
.PHONY: verilate
verilate: $(DUT_LIB)

# C++ application link target. The
# ordering of the target dependencies
# matters. 
$(BIN_DIR)/$(BIN): $(OBJS) $(DUT_LIB) $(VERI_SUPPORT_LIB)
	@$(MKDIR) $$(dirname $@)
	$(LD) $(LDFLAGS) $^ -o $@

# C compilations
$(OBJ_DIR)/%.c.o: %.c
	@$(MKDIR) $$(dirname $@)
	$(CC) $(CFLAGS) -o $@ $<

# C++ compilations
$(OBJ_DIR)/%.cpp.o: %.cpp
	@$(MKDIR) $$(dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Verilate target
$(DUT_LIB): $(SV_SRCS)
	verilator $(VERI_FLAGS) $^
	make -C $(VERI_DIR) -f $(VERI_PREFIX).mk

# Build verilator support library
$(VERI_SUPPORT_LIB):
	make -j -C $(VERI_SUPPORT_ROOT)

# Clean up the project folder
.PHONY: clean
clean:
	$(RM) $(VERI_DIR)
	$(RM) $(OBJ_DIR)
	$(RM) $(BIN_DIR)
	$(RM) *.vcd

# Clean up the project folder and support
# library
.PHONY: clean_all
clean_all: clean
	make -C $(VERI_SUPPORT_ROOT) clean

# HDL linting target
.PHONY: lint
lint:
	@verilator --lint-only $(LINT_FLAGS) $(SV_SRCS)

# This target is mostly used for
# debugging. It pretty prints the
# contents of Make variables.
.PHONY: dump
dump:
	@echo "OS    : $(OS)"
	@echo "CC    : $(CC)"
	@echo "CXX   : $(CXX)"
	@echo "LD    : $(LD)"
	@echo "AR    : $(AR)"
	@echo "RM    : $(RM)"
	@echo "MKDIR : $(MKDIR)"
	@echo "BIN   : $(BIN_DIR)/$(BIN)"
	@echo ""
	@echo "VERI_INSTALL: $(VERI_INSTALL)"
	@echo "VERI_DIR    : $(VERI_DIR)"
	@echo "VERI_PREFIX : $(VERI_PREFIX)"
	@echo "DUT_LIB     : $(DUB_LIB)"
	@echo ""
	@echo "INC_DIRS:"
	@echo "$$(echo "$(INC_DIRS)" | $(DUMP_FMT))"
	@echo ""
	@echo "SRC_DIRS:"
	@echo "$$(echo "$(SRC_DIRS)" | $(DUMP_FMT))"
	@echo ""
	@echo "SRC_FILES:"
	@echo "$$(echo "$(SRC_FILES)" | $(DUMP_FMT))"
	@echo ""
	@echo "OBJS:"
	@echo "$$(echo "$(OBJS)" | $(DUMP_FMT))"
	@echo ""
	@echo "CFLAGS:"
	@echo "$$(echo "$(CFLAGS)" | $(DUMP_FMT))"
	@echo ""
	@echo "CXXFLAGS:"
	@echo "$$(echo "$(CXXFLAGS)" | $(DUMP_FMT))"
	@echo ""
	@echo "LDFLAGS"
	@echo "$$(echo "$(LDFLAGS)" | $(DUMP_FMT))"

# Handle and .d files if thte list is not empty
ifneq ($(DEPS),)
include $(DEPS)
endif
