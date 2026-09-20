# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/qemu/CMakeLists.txt
# Brief: Select the configured QEMU board.

# Board selection ------------------------------------------------------------
add_subdirectory_if_enabled(CONFIG_PLATFORM_QEMU_VIRT virt)
add_subdirectory_if_enabled(CONFIG_PLATFORM_QEMU_VIRT_SECURE virt-secure)
add_subdirectory_if_enabled(CONFIG_PLATFORM_QEMU_MPS2_AN385 mps2-an385)
