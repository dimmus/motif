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
set(WITH_DEMOS ON CACHE BOOL "Build examples" FORCE)

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

# Enable all compiler optimizations (coverage instrumentation is not one)
set(WITH_COMPILER_CODE_COVERAGE OFF CACHE BOOL "Enable code coverage" FORCE)
set(WITH_COMPILER_CCACHE ON CACHE BOOL "Enable ccache" FORCE)

# Enable advanced features
set(WITH_NINJA_POOL_JOBS ON CACHE BOOL "Enable Ninja pool jobs" FORCE)

# Optimize for performance
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release C flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release C++ flags" FORCE)

# Enable optimizations
add_compile_definitions(NDEBUG=1)

# Enable link-time optimization (applied in CMakeLists.txt once the compiler is known)
set(WITH_LTO ON CACHE BOOL "Enable link-time optimization" FORCE)

# Optimize for the build machine's CPU (not portable to older CPUs)
set(WITH_CPU_NATIVE ON CACHE BOOL "Optimize for the build machine's CPU" FORCE)