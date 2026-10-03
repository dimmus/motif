# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# Debug build configuration for Motif
# This configuration enables debug symbols, assertions, and debugging features
#
# Like the other presets, this is an initial-cache script (cmake -C): it only
# seeds a new cache and does not override values already in an existing one.

# Set build type
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type")

# Enable all debugging and testing features
set(WITH_TESTS ON CACHE BOOL "Build tests")
set(WITH_DEMOS ON CACHE BOOL "Build examples")

# Enable developer-friendly features
set(WITH_COMPILER_CODE_COVERAGE ON CACHE BOOL "Enable code coverage")
set(WITH_UIL_DEBUG ON CACHE BOOL "Enable UIL debug support")

# Enable all optional features for comprehensive testing
set(WITH_UTF8 ON CACHE BOOL "Enable UTF-8 support")
set(WITH_MESSAGE_CATALOG ON CACHE BOOL "Enable message catalog support")
set(WITH_PRINTING ON CACHE BOOL "Enable printing support")
set(WITH_JPEG ON CACHE BOOL "Enable JPEG support")
set(WITH_PNG ON CACHE BOOL "Enable PNG support")
set(WITH_XFT ON CACHE BOOL "Enable Xft support")

# Enable documentation
set(WITH_DOCS ON CACHE BOOL "Enable documentation installation")

# Build shared libraries
set(WITH_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

# Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose makefile output")

# Enable debug information
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG" CACHE STRING "Debug C flags")
