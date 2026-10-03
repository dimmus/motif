# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# Lite build configuration for Motif
# This configuration disables non-essential features for a smaller binary and faster build

# Set build type to Release for smaller size
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")

# Disable non-essential features
set(WITH_TESTS OFF CACHE BOOL "Build tests")
set(WITH_DEMOS OFF CACHE BOOL "Build examples")

# Disable development features
set(WITH_COMPILER_CODE_COVERAGE OFF CACHE BOOL "Enable code coverage")
set(WITH_UIL_DEBUG OFF CACHE BOOL "Enable UIL debug support")

# Disable optional features to reduce size
set(WITH_UTF8 OFF CACHE BOOL "Enable UTF-8 support")
set(WITH_MESSAGE_CATALOG OFF CACHE BOOL "Enable message catalog support")
set(WITH_PRINTING OFF CACHE BOOL "Enable printing support")
set(WITH_JPEG OFF CACHE BOOL "Enable JPEG support")
set(WITH_PNG OFF CACHE BOOL "Enable PNG support")
set(WITH_XFT OFF CACHE BOOL "Enable Xft support")

# Disable documentation
set(WITH_DOCS OFF CACHE BOOL "Enable documentation installation")
set(WITH_WML_TOOLS OFF CACHE BOOL "Enable WML tools")

# Build only shared libraries
set(WITH_SHARED_LIBS ON CACHE BOOL "Build shared libraries")
set(WITH_STATIC_LIBS OFF CACHE BOOL "Build static libraries")

# Optimize for size
set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG" CACHE STRING "Release C flags")

# Strip symbols for smaller binary
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s" CACHE STRING "Release exe linker flags")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-s" CACHE STRING "Release shared linker flags")
