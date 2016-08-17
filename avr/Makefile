#
#             LUFA Library
#     Copyright (C) Dean Camera, 2015.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

# Run "make help" for target help.

MCU = atmega32u2
ARCH = AVR8
BOARD = NONE
F_CPU = 8000000
F_USB = $(F_CPU)
OPTIMIZATION = s
OBJDIR = build
TARGET = $(OBJDIR)/gcipl
SRCDIR = src
SRC = $(wildcard $(SRCDIR)/*.c) $(LUFA_SRC_USB)
LUFA_PATH = lufa/LUFA
CC_FLAGS = -Wno-strict-prototypes -DUSE_LUFA_CONFIG_HEADER -I$(SRCDIR) -I$(SRCDIR)/config -flto
LD_FLAGS = -flto

# Default target
all:

program: $(TARGET).hex
	sudo dfu-programmer $(MCU) erase
	sudo dfu-programmer $(MCU) flash $(TARGET).hex
	sudo dfu-programmer $(MCU) launch

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
