# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# Developer build configuration for Motif
# This configuration enables faster builds, error checking and tests, recommended for developers

# Set build type to Debug for development
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type")

# Enable all development features
set(WITH_TESTS ON CACHE BOOL "Build tests")
set(WITH_DEMOS ON CACHE BOOL "Build examples")

# Enable development tools
set(WITH_COMPILER_CODE_COVERAGE ON CACHE BOOL "Enable code coverage")
set(WITH_UIL_DEBUG ON CACHE BOOL "Enable UIL debug support")

# Enable all optional features for comprehensive testing
set(WITH_UTF8 ON CACHE BOOL "Enable UTF-8 support")
set(WITH_MESSAGE_CATALOG ON CACHE BOOL "Enable message catalog support")
set(WITH_PRINTING OFF CACHE BOOL "Enable printing support")
set(WITH_JPEG ON CACHE BOOL "Enable JPEG support")
set(WITH_PNG ON CACHE BOOL "Enable PNG support")
set(WITH_XFT ON CACHE BOOL "Enable Xft support")

# Enable documentation
set(WITH_DOCS ON CACHE BOOL "Enable documentation installation")

# Build shared libraries
set(WITH_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

# Enable developer tools
set(WITH_COMPILER_CCACHE ON CACHE BOOL "Enable ccache")
set(WITH_NINJA_POOL_JOBS ON CACHE BOOL "Enable Ninja pool jobs")

# Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose makefile output")

# Enable debug information with some optimization
set(CMAKE_C_FLAGS_DEBUG "-g -O1 -DDEBUG" CACHE STRING "Debug C flags")

# Enable the address sanitizer for development builds.  This is an
# initial-cache script that runs before the compiler is known, so the
# top-level CMakeLists.txt checks that the compiler supports it.
set(WITH_COMPILER_ASAN ON CACHE BOOL "Enable address sanitizer")
