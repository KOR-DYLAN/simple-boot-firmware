# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: platform/CMakeLists.txt
# Brief: Select the platform vendor and board path.

# Vendor selection -----------------------------------------------------------
set_source_target(libdriver)
add_subdirectory_if_enabled(CONFIG_PLATFORM_QEMU qemu)
