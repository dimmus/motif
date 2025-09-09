# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# Full build configuration for Motif
# This configuration enables all supported dependencies and options

# Set build type to Release
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)

# Disable debug features
set(WITH_DEBUG OFF CACHE BOOL "Enable debug build" FORCE)

# Enable all features
set(WITH_TESTS ON CACHE BOOL "Build tests" FORCE)
set(WITH_DEMOS ON CACHE BOOL "Build demos" FORCE)

# Enable all optional features
set(WITH_UTF8 ON CACHE BOOL "Enable UTF-8 support" FORCE)
set(WITH_MESSAGE_CATALOG ON CACHE BOOL "Enable message catalog support" FORCE)
set(WITH_PRINTING ON CACHE BOOL "Enable printing support" FORCE)
set(WITH_JPEG ON CACHE BOOL "Enable JPEG support" FORCE)
set(WITH_PNG ON CACHE BOOL "Enable PNG support" FORCE)
set(WITH_XFT ON CACHE BOOL "Enable Xft support" FORCE)

# Enable documentation and tools
set(WITH_DOCS ON CACHE BOOL "Enable documentation installation" FORCE)
set(WITH_WML_TOOLS ON CACHE BOOL "Enable WML tools" FORCE)

# Build both shared and static libraries
set(WITH_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
set(WITH_STATIC_LIBS ON CACHE BOOL "Build static libraries" FORCE)

# Enable all compiler optimizations
set(WITH_COMPILER_CODE_COVERAGE ON CACHE BOOL "Enable code coverage" FORCE)
set(WITH_COMPILER_CCACHE ON CACHE BOOL "Enable ccache" FORCE)

# Enable advanced features
set(WITH_NINJA_POOL_JOBS ON CACHE BOOL "Enable Ninja pool jobs" FORCE)

# Optimize for performance
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release C flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release C++ flags" FORCE)

# Enable optimizations
add_compile_definitions(NDEBUG=1)

# Enable link-time optimization if available
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON CACHE BOOL "Enable LTO" FORCE)
endif()

# Enable all available compiler features
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native" CACHE STRING "Release C flags" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native" CACHE STRING "Release C++ flags" FORCE)
endif()