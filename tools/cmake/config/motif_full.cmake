# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# Full build configuration for Motif
# This configuration enables all supported dependencies and options

# Set build type to Release
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")

# Enable all features
set(WITH_TESTS ON CACHE BOOL "Build tests")
set(WITH_DEMOS ON CACHE BOOL "Build examples")

# Enable all optional features
set(WITH_UTF8 ON CACHE BOOL "Enable UTF-8 support")
set(WITH_MESSAGE_CATALOG ON CACHE BOOL "Enable message catalog support")
set(WITH_PRINTING ON CACHE BOOL "Enable printing support")
set(WITH_JPEG ON CACHE BOOL "Enable JPEG support")
set(WITH_PNG ON CACHE BOOL "Enable PNG support")
set(WITH_XFT ON CACHE BOOL "Enable Xft support")

# Enable documentation and tools
set(WITH_DOCS ON CACHE BOOL "Enable documentation installation")
set(WITH_WML_TOOLS ON CACHE BOOL "Enable WML tools")

# Build both shared and static libraries
set(WITH_SHARED_LIBS ON CACHE BOOL "Build shared libraries")
set(WITH_STATIC_LIBS ON CACHE BOOL "Build static libraries")

# Enable all compiler optimizations (coverage instrumentation is not one)
set(WITH_COMPILER_CODE_COVERAGE OFF CACHE BOOL "Enable code coverage")
set(WITH_COMPILER_CCACHE ON CACHE BOOL "Enable ccache")

# Enable advanced features
set(WITH_NINJA_POOL_JOBS ON CACHE BOOL "Enable Ninja pool jobs")

# Optimize for performance
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release C flags")

# Enable link-time optimization (applied in CMakeLists.txt once the compiler is known)
set(WITH_LTO ON CACHE BOOL "Enable link-time optimization")

# Optimize for the build machine's CPU (not portable to older CPUs)
set(WITH_CPU_NATIVE ON CACHE BOOL "Optimize for the build machine's CPU")
