# GCC tool prefix (if there is one)
TOOL_PREFIX ?=

# The build tools used in the targets.
CC    := $(TOOL_PREFIX)gcc -c -xc
CXX   := $(TOOL_PREFIX)gcc -c -xc++
LD    := $(TOOL_PREFIX)gcc
AR    := $(TOOL_PREFIX)ar
RM    := rm -rf
MKDIR := mkdir -p

# The output library directory and name.
LIB_DIR  := lib
LIB_NAME := ticd
LIB      := lib$(LIB_NAME).a

# Variables related to doxygen and its output.
DOXY_CONF := doxy.conf
DOXY_DIR  := doxy_output

# The output name and location of the TICD
# documentation.
DOC_DIR := doc
DOC_NAME := ticd_api.pdf

# Include directories and compiler
# include (-I) argument creation. For
# ease of addition, put each new
# include directory on its own line
# with the += assignment.
INC_DIRS :=
INC_DIRS += include

INC_FLAGS := $(foreach dir, $(INC_DIRS), $(addprefix -I, $(dir)))

# Source directory list and source file
# list generation. For ease of addition,
# put each new source directory on its
# own line. The SRC_FILE variable is
# created by recursively searching
# these directories for *.c and *.cpp
# files.
SRC_DIRS :=
SRC_DIRS += src

SRC_FILES :=
SRC_FILES += $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.cpp'))
SRC_FILES += $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.c'))

# The object directory and object file
# list generation. These shouldn't
# really change. Object files under the
# $(OBJ_DIR) will mirror the path of
# source files under the $(SRC_DIRS).
OBJ_DIR  := obj

OBJS := $(SRC_FILES:=.o)
OBJS := $(foreach obj, $(OBJS), $(addprefix $(OBJ_DIR)/, $(obj)))

# Warning flags are applied to both
# CFLAGS and CXXFLAGS through the
# COMFLAGS variable. If a certain
# warning only applies to one type of
# compiler, it must be placed in the
# compiler specific flags variable
WARN_FLAGS :=
WARN_FLAGS += -Wall
WARN_FLAGS += -Wextra
WARN_FLAGS += -Wshadow

# Common compiler flags are passed to
# both the CC and CXX compilers.
COMMON_FLAGS := $(WARN_FLAGS)
COMMON_FLAGS += -ffunction-sections
COMMON_FLAGS += -O2
COMMON_FLAGS += -fdata-sections
COMMON_FLAGS += -MMD
COMMON_FLAGS += -MP
COMMON_FLAGS += $(INC_FLAGS)

# CFLAGS are C compiler specific flags.
# These flags are NOT passed to CXX
CFLAGS := $(COMMON_FLAGS)
CFLAGS += -std=c99

# CXXFLAGS are C++ compiler specific
# flags. These flags are not passed to
# CC.
CXXFLAGS := $(COMMON_FLAGS)
CXXFLAGS += -std=c++11

# Flags passed to the archiver after
# compilation.
ARFLAGS := -crs

# This variable is used by the dump
# target to format the output of Make
# variables. Notice how it uses the
# regular equal "=" instead of the
# colon equal ":=". This is done so
# that variable expansion is defered
# until this variable is used.
DUMP_FMT := tr " " "\n" | sed 's/^\(\w\|-\)/    \1/; /^$$/d'

# This is the default target (commonly
# called all). It in turn triggers the
# final binary linker target.
#
all: $(LIB_DIR)/$(LIB)

# This is the archive target that uses
# all of the compiled OBJS files and
# ARFLAGS to create the final lib.
#
$(LIB_DIR)/$(LIB): $(OBJS)
	@$(MKDIR) $$(dirname $@)
	$(AR) $(ARFLAGS) $@ $^

# This is the target that compiles all
# C files into object files under the
# $(OBJ_DIR).
$(OBJ_DIR)/%.c.o: %.c
	@$(MKDIR) $$(dirname $@)
	$(CC) $(CFLAGS) -o $@ $<

# This is the target that compiles all
# C++ files into object files under the
# $(OBJ_DIR).
$(OBJ_DIR)/%.cpp.o: %.cpp
	@$(MKDIR) $$(dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ $<

# This target uses doxygen to create the
# API documentation from annotated source
# files.
.PHONY: doc
doc:
	@doxygen $(DOXY_CONF)
	@$(MAKE) -C $(DOXY_DIR)/latex
	@mkdir -p $(DOC_DIR)
	@cp -f $(DOXY_DIR)/latex/refman.pdf $(DOC_DIR)/$(DOC_NAME)

# This target removes all build files
# and the final binary.
.PHONY: clean
clean:
	$(RM) $(OBJ_DIR)
	$(RM) $(LIB_DIR)
	$(RM) $(DOXY_DIR)

# This target is mostly used for
# debugging. It pretty prints the
# contents of Make variables.
.PHONY: dump
dump:
	@echo "CC    : $(CC)"
	@echo "CXX   : $(CXX)"
	@echo "LD    : $(LD)"
	@echo "AR    : $(AR)"
	@echo "RM    : $(RM)"
	@echo "MKDIR : $(MKDIR)"
	@echo "LIB   : $(LIB_DIR)/$(LIB)"
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
	@echo "ARFLAGS"
	@echo "$$(echo "$(ARFLAGS)" | $(DUMP_FMT))"

# Not quite sure how they work but .d
# (a.k.a dep files) allow rebuilds to
# happen if header files changes.
DEPS := $(shell find $(OBJDIR) -name '*.d')
include $(DEPS)
