# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/EditorConfig.cmake
# Brief: Select the active compilation database for clangd.

# Artifact generator ---------------------------------------------------------
find_package(Python3 3.6 REQUIRED COMPONENTS Interpreter)
set(BOOT_ARTIFACT_GENERATOR "${CMAKE_CURRENT_LIST_DIR}/generate_artifacts.py")

# Editor configuration -------------------------------------------------------
execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${BOOT_ARTIFACT_GENERATOR}"
            --source-dir "${PROJECT_SOURCE_DIR}" --build-dir "${PROJECT_BINARY_DIR}"
            --editor-only
    COMMAND_ERROR_IS_FATAL ANY
)
