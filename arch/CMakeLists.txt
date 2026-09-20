# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: arch/CMakeLists.txt
# Brief: Select the architecture implementation.

# Subdirectory selection -----------------------------------------------------
add_subdirectory_if_enabled(CONFIG_ARCH_AARCH64 aarch64)
add_subdirectory_if_enabled(CONFIG_ARCH_AARCH32 aarch32)
add_subdirectory_if_enabled(CONFIG_ARCH_CORTEX_M cortex-m)
