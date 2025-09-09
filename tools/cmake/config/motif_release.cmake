# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# Release build configuration for Motif
# This configuration optimizes for performance and production use

# Set build type to Release
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)

# Disable debug features
set(WITH_DEBUG OFF CACHE BOOL "Enable debug build" FORCE)

# Enable essential features
set(WITH_TESTS OFF CACHE BOOL "Build tests" FORCE)
set(WITH_DEMOS ON CACHE BOOL "Build demos" FORCE)

# Disable development features
set(WITH_COMPILER_CODE_COVERAGE OFF CACHE BOOL "Enable code coverage" FORCE)
set(WITH_UIL_DEBUG OFF CACHE BOOL "Enable UIL debug support" FORCE)

# Enable all optional features for full functionality
set(WITH_UTF8 ON CACHE BOOL "Enable UTF-8 support" FORCE)
set(WITH_MESSAGE_CATALOG ON CACHE BOOL "Enable message catalog support" FORCE)
set(WITH_PRINTING ON CACHE BOOL "Enable printing support" FORCE)
set(WITH_JPEG ON CACHE BOOL "Enable JPEG support" FORCE)
set(WITH_PNG ON CACHE BOOL "Enable PNG support" FORCE)
set(WITH_XFT ON CACHE BOOL "Enable Xft support" FORCE)

# Enable documentation
set(WITH_DOCS ON CACHE BOOL "Enable documentation installation" FORCE)
set(WITH_WML_TOOLS ON CACHE BOOL "Enable WML tools" FORCE)

# Build shared libraries by default
set(WITH_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
set(WITH_STATIC_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

# Optimize for performance
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release C flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release C++ flags" FORCE)

# Enable optimizations
add_compile_definitions(NDEBUG=1)

# Enable link-time optimization if available
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON CACHE BOOL "Enable LTO" FORCE)
endif()