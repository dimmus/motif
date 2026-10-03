# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# Release build configuration for Motif
# This configuration optimizes for performance and production use

# Set build type to Release
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")

# Enable essential features
set(WITH_TESTS OFF CACHE BOOL "Build tests")
set(WITH_DEMOS ON CACHE BOOL "Build examples")

# Disable development features
set(WITH_COMPILER_CODE_COVERAGE OFF CACHE BOOL "Enable code coverage")
set(WITH_UIL_DEBUG OFF CACHE BOOL "Enable UIL debug support")

# Enable all optional features for full functionality
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

# Optimize for performance
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release C flags")

# Enable link-time optimization (applied in CMakeLists.txt once the compiler is known)
set(WITH_LTO ON CACHE BOOL "Enable link-time optimization")
