# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= wildstorage
GAME_TITLE	:= Wild Storage
GAME_SUBTITLE := ACWW save editor tool
GAME_AUTHOR   := by NovaSquirrel - v1.0.2
GAME_ICON     := $(CURDIR)/icon.png

# Disable floating-point printf/scanf to reduce ROM usage.
DEFINES     := -DPICOLIBC_LONG_LONG_PRINTF_SCANF

# Source code paths
# -----------------

GFXDIRS		:= graphics

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
