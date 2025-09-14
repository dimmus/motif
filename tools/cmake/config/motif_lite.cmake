# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# Lite build configuration for Motif
# This configuration disables non-essential features for a smaller binary and faster build

# Set build type to Release for smaller size
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)

# Disable debug features
set(WITH_DEBUG OFF CACHE BOOL "Enable debug build" FORCE)

# Disable non-essential features
set(WITH_TESTS OFF CACHE BOOL "Build tests" FORCE)
set(WITH_DEMOS OFF CACHE BOOL "Build examples" FORCE)

# Disable development features
set(WITH_COMPILER_CODE_COVERAGE OFF CACHE BOOL "Enable code coverage" FORCE)
set(WITH_UIL_DEBUG OFF CACHE BOOL "Enable UIL debug support" FORCE)

# Disable optional features to reduce size
set(WITH_UTF8 OFF CACHE BOOL "Enable UTF-8 support" FORCE)
set(WITH_MESSAGE_CATALOG OFF CACHE BOOL "Enable message catalog support" FORCE)
set(WITH_PRINTING OFF CACHE BOOL "Enable printing support" FORCE)
set(WITH_JPEG OFF CACHE BOOL "Enable JPEG support" FORCE)
set(WITH_PNG OFF CACHE BOOL "Enable PNG support" FORCE)
set(WITH_XFT OFF CACHE BOOL "Enable Xft support" FORCE)

# Disable documentation
set(WITH_DOCS OFF CACHE BOOL "Enable documentation installation" FORCE)
set(WITH_WML_TOOLS OFF CACHE BOOL "Enable WML tools" FORCE)

# Build only shared libraries
set(WITH_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
set(WITH_STATIC_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

# Optimize for size
set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG" CACHE STRING "Release C flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG" CACHE STRING "Release C++ flags" FORCE)

# Enable size optimizations
add_compile_definitions(NDEBUG=1)

# Strip symbols for smaller binary
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s" CACHE STRING "Release exe linker flags" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-s" CACHE STRING "Release shared linker flags" FORCE)