# The build tools
CC    := gcc -c -xc
CXX   := gcc -c -xc++
LD    := gcc
AR    := ar
RM    := rm -rf
CP    := cp -rf
MKDIR := mkdir -p

# Determine the OS the Makefile is running on.
ifneq ($(OS),Windows_NT)
OS := $(shell uname -s)
endif

# When using verilator to build test benches,
# there are several files from the install
# that need to be compiled for the final
# executable. This Makefile encapsulates all
# of that into a static archive.
LIB_DIR := lib
LIB     := libverilator_support.a

# Use a combination of Verilator and shell
# commands to get the install directory of
# Verilator
VERI_INSTALL := $(shell verilator -V | grep -E "VERILATOR_ROOT\W*=.*/.*" | cut -d = -f 2 | xargs)

# During the build, we will copy all the
# header files from the install directory
# to an include directory in this project.
# Just to make it easier for simulations
# to use this library.
INC_DIR := include

# Verilator install C++ source files.
SRC_FILES := $(VERI_INSTALL)/include/verilated.cpp
SRC_FILES += $(VERI_INSTALL)/include/verilated_vcd_c.cpp
SRC_FILES += $(VERI_INSTALL)/include/verilated_threads.cpp

# Object directory and object file list
# generation.
OBJ_DIR := obj

OBJS := $(SRC_FILES:=.o)
OBJS := $(foreach obj, $(OBJS), $(addprefix $(OBJ_DIR)/, $(obj)))

# Since this Makefile is intended to be
# called from another Makefile, we need
# a way to change the optimizaiton level.
OPT_LVL ?= -O2

# Compiler flags that are common to both
# C and C++. For the verilator install
# sources we don't really care about warnings.
COMMON_FLAGS := $(OPT_LVL)
COMMON_FLAGS += -ffunction-sections
COMMON_FLAGS += -fdata-sections
COMMON_FLAGS += -I$(VERI_INSTALL)/include

# C compiler specific flags
CFLAGS := $(COMMON_FLAGS)
CFLAGS += -std=gnu99

# C++ compiler specific flags
CXXFLAGS := $(COMMON_FLAGS)
CXXFLAGS += -std=gnu++14

# archiver flags
ARFLAGS := -crs

# The dead code removal linker flag is
# named different on MacOS because Apple.
ifeq ($(OS), Darwin)
	LDFLAGS += -Wl,-dead_strip
else
	LDFLAGS += -Wl,--gc-sections
endif

# This variable is used by the dump
# target to format the output of Make
# variables. Notice how it uses the
# regular equal "=" instead of the
# colon equal ":=". This is done so
# that variable expansion is defered
# until this variable is used.
DUMP_FMT := tr " " "\n" | sed 's/^\(\w\|-\)/    \1/; /^$$/d'

# Bundle the compiled objects into a
# static archive. The header files from
# the verilator install are also copied
# to directory to make it easier to use
# this lib. All non-header files will
# have to be cleaned out after the copy
$(LIB_DIR)/$(LIB): $(DUT_LIB) $(VERILIB) $(OBJS)
	@$(CP) $(VERI_INSTALL)/include .
	@$(RM) $(INC_DIR)/**/*.c
	@$(RM) $(INC_DIR)/**/*.cpp
	@$(RM) $(INC_DIR)/**/*.sv
	@$(RM) $(INC_DIR)/**/*.in
	@$(RM) $(INC_DIR)/*.c
	@$(RM) $(INC_DIR)/*.cpp
	@$(RM) $(INC_DIR)/*.sv
	@$(RM) $(INC_DIR)/*.in
	@$(MKDIR) $$(dirname $@)
	$(AR) $(ARFLAGS) $@ $^

# C compilations
$(OBJ_DIR)/%.c.o: %.c
	@$(MKDIR) $$(dirname $@)
	$(CC) $(CFLAGS) -o $@ $<

# C++ compilations
$(OBJ_DIR)/%.cpp.o: %.cpp
	@$(MKDIR) $$(dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Clean up the project folder
.PHONY: clean
clean:
	$(RM) $(INC_DIR)
	$(RM) $(OBJ_DIR)
	$(RM) $(LIB_DIR)

# This target is mostly used for
# debugging. It pretty prints the
# contents of Make variables.
.PHONY: dump
dump:
	@echo "OS          : $(OS)"
	@echo "VERI_INSTALL: $(VERI_INSTALL)"
	@echo "CC          : $(CC)"
	@echo "CXX         : $(CXX)"
	@echo "LD          : $(LD)"
	@echo "AR          : $(AR)"
	@echo "RM          : $(RM)"
	@echo "MKDIR       : $(MKDIR)"
	@echo "LIB         : $(LIB_DIR)/$(LIB)"
	@echo "INC_DIR     : $(INC_DIR)"
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
	@echo "ARFLAGS"
	@echo "$$(echo "$(ARFLAGS)" | $(DUMP_FMT))"

