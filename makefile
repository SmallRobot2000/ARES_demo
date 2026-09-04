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
HW_DIR      := $(SRC_DIR)/hw
SIM_DIR     := $(SRC_DIR)/sim

HW_BUILD_DIR  := $(BUILD_DIR)/hw
SIM_BUILD_DIR := $(BUILD_DIR)/sim


# ------------------------------------------------------------
# Compilers
# ------------------------------------------------------------

PREFIX := /home/lovro/ARES/buildroot/output/host/bin/riscv32-buildroot-linux-gnu-

HW_CC  := $(PREFIX)gcc
SIM_CC := gcc


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
# Compiler flags
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
	-MP


# ------------------------------------------------------------
# Simulator libraries
# ------------------------------------------------------------

SIM_LDLIBS := \
	-lX11 \
	-pthread


# ------------------------------------------------------------
# Find source files recursively
# ------------------------------------------------------------

MAIN_SOURCE := $(SRC_DIR)/main.c

COMMON_SOURCES := $(shell find $(COMMON_DIR) -type f -name '*.c' 2>/dev/null)
HW_SOURCES     := $(shell find $(HW_DIR)     -type f -name '*.c' 2>/dev/null)
SIM_SOURCES    := $(shell find $(SIM_DIR)    -type f -name '*.c' 2>/dev/null)

HW_ALL_SOURCES := \
	$(MAIN_SOURCE) \
	$(COMMON_SOURCES) \
	$(HW_SOURCES)

SIM_ALL_SOURCES := \
	$(MAIN_SOURCE) \
	$(COMMON_SOURCES) \
	$(SIM_SOURCES)


# ------------------------------------------------------------
# Object files
#
# src/common/game.c
#       ->
# build/hw/common/game.o
#
# and
#
# build/sim/common/game.o
# ------------------------------------------------------------

HW_OBJECTS := \
	$(patsubst $(SRC_DIR)/%.c,$(HW_BUILD_DIR)/%.o,$(HW_ALL_SOURCES))

SIM_OBJECTS := \
	$(patsubst $(SRC_DIR)/%.c,$(SIM_BUILD_DIR)/%.o,$(SIM_ALL_SOURCES))


# ------------------------------------------------------------
# Automatic dependency files
# ------------------------------------------------------------

HW_DEPS  := $(HW_OBJECTS:.o=.d)
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
# ------------------------------------------------------------

$(HW_TARGET): $(HW_OBJECTS)
	$(HW_CC) $(HW_CFLAGS) $^ -o $@


# ------------------------------------------------------------
# Simulator link
# ------------------------------------------------------------

$(SIM_TARGET): $(SIM_OBJECTS)
	$(SIM_CC) $(SIM_CFLAGS) $^ -o $@ $(SIM_LDLIBS)


# ------------------------------------------------------------
# Hardware compile rule
# ------------------------------------------------------------

$(HW_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(HW_CC) $(HW_CPPFLAGS) $(HW_CFLAGS) -c $< -o $@


# ------------------------------------------------------------
# Simulator compile rule
# ------------------------------------------------------------

$(SIM_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(SIM_CC) $(SIM_CPPFLAGS) $(SIM_CFLAGS) -c $< -o $@


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