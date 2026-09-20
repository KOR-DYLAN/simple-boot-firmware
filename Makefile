# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: Makefile
# Brief: Provide shortcuts for CMake configuration, builds, and targets.

.DEFAULT_GOAL := build

SOURCE_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ARCH ?= aarch64
PLATFORM ?=
CONFIG ?=
BUILD_DIR ?= $(SOURCE_DIR)/build/$(ARCH)
BUILD_TYPE ?= Debug
GENERATOR ?= Ninja
CMAKE ?= cmake
PYTHON ?= python3
CMAKE_ARGS ?=
BUILD_ARGS ?=
TARGET ?= boot
JOBS ?=

.PHONY: all configure defconfig menuconfig savedefconfig savefragmentconfig fragmentconfig showconfig build target run debug gdb layout clean distclean targets help

all: build

configure:
	$(CMAKE) -S "$(SOURCE_DIR)" -B "$(BUILD_DIR)" -G "$(GENERATOR)" \
		-DBOOT_ARCH="$(ARCH)" $(if $(PLATFORM),-DBOOT_PLATFORM="$(PLATFORM)") \
		$(if $(CONFIG),-DBOOT_CONFIG="$(CONFIG)") \
		-DCMAKE_BUILD_TYPE="$(BUILD_TYPE)" $(CMAKE_ARGS)

defconfig menuconfig savedefconfig savefragmentconfig fragmentconfig: configure
	+$(CMAKE) --build "$(BUILD_DIR)" --target $@ $(BUILD_ARGS)

showconfig: configure
	@$(CMAKE) -E cat "$(BUILD_DIR)/.config"

build: configure
	+$(CMAKE) --build "$(BUILD_DIR)" $(if $(JOBS),--parallel $(JOBS)) $(BUILD_ARGS)

target: configure
	+$(CMAKE) --build "$(BUILD_DIR)" --target "$(TARGET)" $(if $(JOBS),--parallel $(JOBS)) $(BUILD_ARGS)

run debug gdb layout: configure
	+$(CMAKE) --build "$(BUILD_DIR)" --target $@ $(BUILD_ARGS)

clean:
	+$(CMAKE) --build "$(BUILD_DIR)" --target clean

distclean:
	$(PYTHON) "$(SOURCE_DIR)/cmake/distclean.py" \
		--source-dir "$(SOURCE_DIR)" --build-dir "$(BUILD_DIR)"

targets: configure
	+$(CMAKE) --build "$(BUILD_DIR)" --target help

help:
	@printf '%s\n' \
		'Usage: make [target] [VARIABLE=value ...]' \
		'' \
		'Targets:' \
		'  build       Configure and build (default).' \
		'  configure   Run CMake configure only.' \
		'  defconfig   Generate .config from the selected board configuration.' \
		'  menuconfig  Edit the merged configuration in a terminal menu.' \
		'  savedefconfig Save BUILD_DIR/defconfig relative to KConfig defaults.' \
		'  savefragmentconfig Save BUILD_DIR/fragment.config relative to board configs.' \
		'  fragmentconfig Alias for savefragmentconfig.' \
		'  showconfig  Print the normalized generated .config.' \
		'  run         Run the platform execution target.' \
		'  debug       Start the platform debug server.' \
		'  gdb         Connect GDB to the running debug target.' \
		'  layout      Print ELF sections and program headers.' \
		'  target      Build the CMake target named by TARGET.' \
		'  targets     List available CMake targets.' \
		'  clean       Remove build outputs from BUILD_DIR.' \
		'  distclean   Remove BUILD_DIR and its active generated editor configuration.' \
		'' \
		'Variables:' \
		'  ARCH=aarch64|aarch32|cortex-m  PLATFORM=vendor/board' \
		'  CONFIG=config-file-name' \
		'  BUILD_DIR=path  BUILD_TYPE=Debug|Release  GENERATOR=Ninja' \
		'  JOBS=count  TARGET=boot  CMAKE=cmake  PYTHON=python3' \
		'  CMAKE_ARGS="configure options"  BUILD_ARGS="build options"'
