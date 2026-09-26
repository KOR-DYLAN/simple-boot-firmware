# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: Makefile
# Brief: Orchestrate independent CMake configurations.

.DEFAULT_GOAL := build

SOURCE_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ARCH ?= aarch64
PLATFORM ?=
CONFIG ?=
CONFIGS ?= bl1 bl2
BUILD_CONFIG ?= $(firstword $(CONFIGS))
FIRMWARE_CONFIGS ?= bl1 bl2
FRAGMENTS ?=
BUILD_DIR ?= $(SOURCE_DIR)/build/$(ARCH)
MULTIBUILD_ROOT := $(abspath $(BUILD_DIR))
BUILD_TYPE ?= Debug
GENERATOR ?= Ninja
CMAKE ?= cmake
PYTHON ?= python3
CMAKE_ARGS ?=
BUILD_ARGS ?=
TARGET ?= boot
JOBS ?=

EMPTY :=
SPACE := $(EMPTY) $(EMPTY)
COMMA := ,
join_configs = $(subst $(SPACE),$(COMMA),$(strip $(1)))
config_file = $(1).config
config_build_dir = $(if $(BUILD_DIR_$(1)),$(BUILD_DIR_$(1)),$(BUILD_DIR)/$(1))
config_fragments = $(call join_configs,$(call config_file,$(1)) \
	$(FRAGMENTS) $(FRAGMENTS_$(1)))

FIRST_FIRMWARE_CONFIG := $(word 1,$(FIRMWARE_CONFIGS))
SECOND_FIRMWARE_CONFIG := $(word 2,$(FIRMWARE_CONFIGS))
FIRST_FIRMWARE_BUILD_DIR := $(call config_build_dir,$(FIRST_FIRMWARE_CONFIG))
SECOND_FIRMWARE_BUILD_DIR := $(call config_build_dir,$(SECOND_FIRMWARE_CONFIG))
BUILD_CONFIG_DIR := $(call config_build_dir,$(BUILD_CONFIG))
RULE_CONFIGS := $(sort $(CONFIGS) $(BUILD_CONFIG) $(FIRMWARE_CONFIGS))

ifeq ($(strip $(CONFIGS)),)
$(error CONFIGS must contain at least one configuration name)
endif
ifneq ($(words $(FIRMWARE_CONFIGS)),2)
$(error FIRMWARE_CONFIGS must contain exactly two configuration names)
endif

COMMON_CONFIGURE_ARGS = \
	-DBOOT_ARCH="$(ARCH)" \
	$(if $(PLATFORM),-DBOOT_PLATFORM="$(PLATFORM)") \
	$(if $(CONFIG),-DBOOT_CONFIG="$(CONFIG)") \
	-DBOOT_MULTIBUILD_ROOT="$(MULTIBUILD_ROOT)" \
	-DBOOT_BL1_BUILD_DIR="$(abspath $(FIRST_FIRMWARE_BUILD_DIR))" \
	-DBOOT_BL2_BUILD_DIR="$(abspath $(SECOND_FIRMWARE_BUILD_DIR))" \
	-DCMAKE_BUILD_TYPE="$(BUILD_TYPE)" \
	$(CMAKE_ARGS)

CONFIGURE_TARGETS := $(addprefix configure-,$(CONFIGS))
BUILD_TARGETS := $(addprefix build-,$(CONFIGS))
LAYOUT_TARGETS := $(addprefix layout-,$(CONFIGS))
CLEAN_TARGETS := $(addprefix clean-,$(CONFIGS))
DISTCLEAN_TARGETS := $(addprefix distclean-,$(CONFIGS))

.PHONY: all configure build firmware \
	defconfig menuconfig savedefconfig savefragmentconfig fragmentconfig \
	showconfig target run debug gdb layout clean distclean targets help \
	$(addprefix configure-,$(RULE_CONFIGS)) \
	$(addprefix build-,$(RULE_CONFIGS)) \
	$(addprefix layout-,$(RULE_CONFIGS)) \
	$(addprefix clean-,$(RULE_CONFIGS)) \
	$(addprefix distclean-,$(RULE_CONFIGS))

all: build

# Every named configuration owns a CMake cache, generated config, and objects.
define CONFIG_RULES
configure-$(1):
	$$(CMAKE) -S "$$(SOURCE_DIR)" \
		-B "$$(call config_build_dir,$(1))" -G "$$(GENERATOR)" \
		$$(COMMON_CONFIGURE_ARGS) \
		-DBOOT_CONFIG_APPEND="$$(call config_fragments,$(1))" \
		$$(CMAKE_ARGS_$(1))

build-$(1): configure-$(1)
	+$$(CMAKE) --build "$$(call config_build_dir,$(1))" \
		$$(if $$(JOBS),--parallel $$(JOBS)) $$(BUILD_ARGS)

layout-$(1): build-$(1)
	+$$(CMAKE) --build "$$(call config_build_dir,$(1))" \
		--target layout $$(BUILD_ARGS)

clean-$(1):
	+$$(CMAKE) --build "$$(call config_build_dir,$(1))" --target clean

distclean-$(1):
	$$(PYTHON) "$$(SOURCE_DIR)/cmake/distclean.py" \
		--source-dir "$$(SOURCE_DIR)" \
		--build-dir "$$(call config_build_dir,$(1))"
endef

$(foreach config,$(RULE_CONFIGS),$(eval $(call CONFIG_RULES,$(config))))

configure: $(CONFIGURE_TARGETS)

# Multi-config build only builds the requested independent configurations.
build: $(BUILD_TARGETS)

# Firmware composition is an optional two-configuration operation.
firmware: build-$(FIRST_FIRMWARE_CONFIG) build-$(SECOND_FIRMWARE_CONFIG)
	$(PYTHON) "$(SOURCE_DIR)/cmake/combine_images.py" \
		--cmake-cache "$(FIRST_FIRMWARE_BUILD_DIR)/CMakeCache.txt" \
		--first-elf "$(FIRST_FIRMWARE_BUILD_DIR)/bootloader1.elf" \
		--first-bin "$(FIRST_FIRMWARE_BUILD_DIR)/bootloader1.bin" \
		--second-elf "$(SECOND_FIRMWARE_BUILD_DIR)/bootloader2.elf" \
		--second-bin "$(SECOND_FIRMWARE_BUILD_DIR)/bootloader2.bin" \
		--output "$(BUILD_DIR)/firmware.bin"

# Configuration editors operate on one entry selected from the config set.
defconfig menuconfig savedefconfig savefragmentconfig fragmentconfig: \
	configure-$(BUILD_CONFIG)
	+$(CMAKE) --build "$(BUILD_CONFIG_DIR)" --target $@ $(BUILD_ARGS)

showconfig: configure-$(BUILD_CONFIG)
	@$(CMAKE) -E cat "$(BUILD_CONFIG_DIR)/.config"

target: configure-$(BUILD_CONFIG)
	+$(CMAKE) --build "$(BUILD_CONFIG_DIR)" --target "$(TARGET)" \
		$(if $(JOBS),--parallel $(JOBS)) $(BUILD_ARGS)

# The first firmware configuration owns execution and debugger targets.
run debug gdb: firmware
	+$(CMAKE) --build "$(FIRST_FIRMWARE_BUILD_DIR)" --target $@ $(BUILD_ARGS)

layout: $(LAYOUT_TARGETS)

clean: $(CLEAN_TARGETS)
	$(CMAKE) -E rm -f "$(BUILD_DIR)/firmware.bin"

distclean: $(DISTCLEAN_TARGETS)
	$(CMAKE) -E rm -f "$(BUILD_DIR)/firmware.bin"

targets: configure-$(BUILD_CONFIG)
	+$(CMAKE) --build "$(BUILD_CONFIG_DIR)" --target help

help:
	@printf '%s\n' \
		'Usage: make [target] [VARIABLE=value ...]' \
		'' \
		'Targets:' \
		'  build       Configure and build every entry in CONFIGS.' \
		'  configure   Configure every entry in CONFIGS.' \
		'  build-NAME  Configure and build one named configuration.' \
		'  firmware    Build and combine the FIRMWARE_CONFIGS pair.' \
		'  menuconfig  Edit BUILD_CONFIG using the terminal menu.' \
		'  defconfig   Reset BUILD_CONFIG from its selected files.' \
		'  showconfig  Print the normalized BUILD_CONFIG.' \
		'  run/debug   Build firmware and start the platform target.' \
		'  gdb         Connect GDB to the running debug target.' \
		'  layout      Print layouts for every entry in CONFIGS.' \
		'  target      Build TARGET in BUILD_CONFIG.' \
		'  targets     List targets in BUILD_CONFIG.' \
		'  clean       Clean every entry in CONFIGS.' \
		'  distclean   Remove every CONFIGS build directory.' \
		'' \
		'Variables:' \
		'  ARCH=aarch64|aarch32|cortex-m  PLATFORM=vendor/board' \
		'  CONFIG=base-config  CONFIGS="bl1 bl2"' \
		'  BUILD_CONFIG=bl1  FIRMWARE_CONFIGS="bl1 bl2"' \
		'  FRAGMENTS="common.config"  FRAGMENTS_NAME="..."' \
		'  BUILD_DIR=path  BUILD_DIR_NAME=path' \
		'  BUILD_TYPE=Debug|Release  GENERATOR=Ninja  JOBS=count' \
		'  CMAKE_ARGS="..."  CMAKE_ARGS_NAME="..."'
