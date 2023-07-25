# GCC tool prefix (if there is one)
#
TOOL_PREFIX ?=

# The build tools
#
CC    := $(TOOL_PREFIX)gcc -c -xc
CXX   := $(TOOL_PREFIX)gcc -c -xc++
LD    := $(TOOL_PREFIX)gcc
AR    := $(TOOL_PREFIX)ar
RM    := rm -rf
MKDIR := mkdir -p

# Compiled library directory, name, and
# full lib*.a name
#
LIB_DIR  := lib
LIB_NAME := ticd
LIB      := lib$(LIB_NAME).a

# Doxygen configuration and output directory
#
DOXY_CONF := doxy.conf
DOXY_DIR  := doxy_output

# The output name and location of the TICD
# documentation.
DOC_DIR  := doc
DOC_NAME := ticd_api.pdf

# Include directories and compiler
# include flags
#
INC_DIRS  := include
INC_FLAGS := $(foreach dir, $(INC_DIRS), $(addprefix -I, $(dir)))

# Source directories and source file
# list
# 
SRC_DIRS  := src
SRC_FILES := $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.cpp'))
SRC_FILES += $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.c'))

# Object directory and file names
#
OBJ_DIR := obj
OBJS    := $(SRC_FILES:=.o)
OBJS    := $(foreach obj, $(OBJS), $(addprefix $(OBJ_DIR)/, $(obj)))

# Compiler warning flags
#
WARN_FLAGS := -Wall
WARN_FLAGS += -Wextra
WARN_FLAGS += -Wshadow

# C and C++ shared compiler flags
#
COMMON_FLAGS := $(WARN_FLAGS)
COMMON_FLAGS += -O2
COMMON_FLAGS += -ffunction-sections
COMMON_FLAGS += -fdata-sections
COMMON_FLAGS += -MMD
COMMON_FLAGS += -MP
COMMON_FLAGS += $(INC_FLAGS)

# C compiler flags
#
CFLAGS := $(COMMON_FLAGS)
CFLAGS += -std=c99

# C++ compiler flags
#
CXXFLAGS := $(COMMON_FLAGS)
CXXFLAGS += -std=c++11

# Archiver flags
#
ARFLAGS := -crs

# This variable is used by the dump
# target to format the output of Make
# variables. Notice how it uses the
# regular equal "=" instead of the
# colon equal ":=". This is done so
# that variable expansion is defered
# until this variable is used.
DUMP_FMT := tr " " "\n" | sed 's/^\(\w\|-\)/    \1/; /^$$/d'

# Build static archive
#
all: $(LIB_DIR)/$(LIB)

# Archive object files
#
$(LIB_DIR)/$(LIB): $(OBJS)
	@$(MKDIR) $$(dirname $@)
	$(AR) $(ARFLAGS) $@ $^

# Compile C files
#
$(OBJ_DIR)/%.c.o: %.c
	@$(MKDIR) $$(dirname $@)
	$(CC) $(CFLAGS) -o $@ $<

# Compile C++ files
#
$(OBJ_DIR)/%.cpp.o: %.cpp
	@$(MKDIR) $$(dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Error check doxygen comments
#
.PHONY: doc-check
doc-check:
	@doxygen $(DOXY_CONF)

# Generate API document
#
.PHONY: doc
doc: doc-check
	@$(MAKE) -C $(DOXY_DIR)/latex
	@mkdir -p $(DOC_DIR)
	@cp -f $(DOXY_DIR)/latex/refman.pdf $(DOC_DIR)/$(DOC_NAME)

# Remove build files and doxygen output
#
.PHONY: clean
clean:
	$(RM) $(OBJ_DIR)
	$(RM) $(LIB_DIR)
	$(RM) $(DOXY_DIR)
	$(RM) $(DOC_DIR)/$(DOC_NAME)

# Pretty print the build configuration
#
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

# Include dependency files
#
DEPS := $(shell find $(OBJDIR) -name '*.d')
include $(DEPS)
