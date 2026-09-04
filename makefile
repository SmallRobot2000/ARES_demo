# ============================================================
# ARES Demo Makefile
#
#   make hw       Build RISC-V hardware version
#   make sim      Build x86 Linux simulator
#   make          Build both
#   make clean    Remove generated files
# ============================================================


# ------------------------------------------------------------
# Directories
# ------------------------------------------------------------

SRC_DIR     := src
INC_DIR     := include
BUILD_DIR   := build

COMMON_DIR  := $(SRC_DIR)/common
CPP_DIR     := $(SRC_DIR)/cpp
HW_DIR      := $(SRC_DIR)/hw
SIM_DIR     := $(SRC_DIR)/sim

HW_BUILD_DIR  := $(BUILD_DIR)/hw
SIM_BUILD_DIR := $(BUILD_DIR)/sim


# ------------------------------------------------------------
# Compilers
# ------------------------------------------------------------

PREFIX := /home/lovro/ARES/buildroot/output/host/bin/riscv32-buildroot-linux-gnu-

HW_CC   := $(PREFIX)gcc
HW_CXX  := $(PREFIX)g++

SIM_CC  := gcc
SIM_CXX := g++


# ------------------------------------------------------------
# Preprocessor flags
# ------------------------------------------------------------

HW_CPPFLAGS := \
	-DARES_HW \
	-I$(INC_DIR)

SIM_CPPFLAGS := \
	-DARES_SIM \
	-I$(INC_DIR)


# ------------------------------------------------------------
# C compiler flags
# ------------------------------------------------------------

HW_CFLAGS := \
	-O2 \
	-Wall \
	-Wextra \
	-MMD \
	-MP \
	-march=rv32imafdc_zicsr_zifencei_zicbom \
	-mabi=ilp32d

SIM_CFLAGS := \
	-O2 \
	-Wall \
	-Wextra \
	-std=c11 \
	-MMD \
	-MP \
	-pthread


# ------------------------------------------------------------
# C++ compiler flags
# ------------------------------------------------------------

HW_CXXFLAGS := \
	-O2 \
	-Wall \
	-Wextra \
	-std=c++17 \
	-MMD \
	-MP \
	-march=rv32imafdc_zicsr_zifencei_zicbom \
	-mabi=ilp32d

SIM_CXXFLAGS := \
	-O2 \
	-Wall \
	-Wextra \
	-std=c++17 \
	-MMD \
	-MP \
	-pthread


# ------------------------------------------------------------
# Libraries
# ------------------------------------------------------------

SIM_LDLIBS := \
	-lX11 \
	-pthread


# ------------------------------------------------------------
# Find source files recursively
# ------------------------------------------------------------

MAIN_SOURCE := $(SRC_DIR)/main.c

COMMON_SOURCES := \
	$(shell find $(COMMON_DIR) -type f -name '*.c' 2>/dev/null)

HW_SOURCES := \
	$(shell find $(HW_DIR) -type f -name '*.c' 2>/dev/null)

SIM_SOURCES := \
	$(shell find $(SIM_DIR) -type f -name '*.c' 2>/dev/null)

CPP_SOURCES := \
	$(shell find $(CPP_DIR) -type f -name '*.cpp' 2>/dev/null)


# ------------------------------------------------------------
# C source lists
# ------------------------------------------------------------

HW_C_SOURCES := \
	$(MAIN_SOURCE) \
	$(COMMON_SOURCES) \
	$(HW_SOURCES)

SIM_C_SOURCES := \
	$(MAIN_SOURCE) \
	$(COMMON_SOURCES) \
	$(SIM_SOURCES)


# ------------------------------------------------------------
# Object files
# ------------------------------------------------------------

HW_C_OBJECTS := \
	$(patsubst $(SRC_DIR)/%.c,$(HW_BUILD_DIR)/%.o,$(HW_C_SOURCES))

HW_CPP_OBJECTS := \
	$(patsubst $(SRC_DIR)/%.cpp,$(HW_BUILD_DIR)/%.o,$(CPP_SOURCES))

SIM_C_OBJECTS := \
	$(patsubst $(SRC_DIR)/%.c,$(SIM_BUILD_DIR)/%.o,$(SIM_C_SOURCES))

SIM_CPP_OBJECTS := \
	$(patsubst $(SRC_DIR)/%.cpp,$(SIM_BUILD_DIR)/%.o,$(CPP_SOURCES))


HW_OBJECTS := \
	$(HW_C_OBJECTS) \
	$(HW_CPP_OBJECTS)

SIM_OBJECTS := \
	$(SIM_C_OBJECTS) \
	$(SIM_CPP_OBJECTS)


# ------------------------------------------------------------
# Automatic dependency files
# ------------------------------------------------------------

HW_DEPS := $(HW_OBJECTS:.o=.d)
SIM_DEPS := $(SIM_OBJECTS:.o=.d)


# ------------------------------------------------------------
# Output files
# ------------------------------------------------------------

HW_TARGET  := main_hw
SIM_TARGET := main_sim


# ------------------------------------------------------------
# Main targets
# ------------------------------------------------------------

.PHONY: all hw sim clean

all: hw sim

hw: $(HW_TARGET)

sim: $(SIM_TARGET)


# ------------------------------------------------------------
# Hardware link
#
# Link with g++ because the program contains C++ objects.
# ------------------------------------------------------------

$(HW_TARGET): $(HW_OBJECTS)
	$(HW_CXX) \
		-march=rv32imafdc_zicsr_zifencei_zicbom \
		-mabi=ilp32d \
		$^ \
		-o $@


# ------------------------------------------------------------
# Simulator link
#
# Link with g++ because the program contains C++ objects.
# ------------------------------------------------------------

$(SIM_TARGET): $(SIM_OBJECTS)
	$(SIM_CXX) \
		$^ \
		-o $@ \
		$(SIM_LDLIBS)


# ------------------------------------------------------------
# Hardware C compile rule
# ------------------------------------------------------------

$(HW_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(HW_CC) $(HW_CPPFLAGS) $(HW_CFLAGS) \
		-c $< \
		-o $@


# ------------------------------------------------------------
# Hardware C++ compile rule
# ------------------------------------------------------------

$(HW_BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(HW_CXX) $(HW_CPPFLAGS) $(HW_CXXFLAGS) \
		-c $< \
		-o $@


# ------------------------------------------------------------
# Simulator C compile rule
# ------------------------------------------------------------

$(SIM_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(SIM_CC) $(SIM_CPPFLAGS) $(SIM_CFLAGS) \
		-c $< \
		-o $@


# ------------------------------------------------------------
# Simulator C++ compile rule
# ------------------------------------------------------------

$(SIM_BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(SIM_CXX) $(SIM_CPPFLAGS) $(SIM_CXXFLAGS) \
		-c $< \
		-o $@


# ------------------------------------------------------------
# Include automatically generated header dependencies
# ------------------------------------------------------------

-include $(HW_DEPS)
-include $(SIM_DEPS)


# ------------------------------------------------------------
# Clean
# ------------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(HW_TARGET) $(SIM_TARGET)