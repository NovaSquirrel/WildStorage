# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= wildstorage
GAME_TITLE	:= Wild Storage

# Source code paths
# -----------------

GFXDIRS		:= graphics

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
