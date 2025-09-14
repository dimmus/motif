# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# Debug build configuration for Motif
# This configuration enables debug symbols, assertions, and debugging features

# Set build type to Debug
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)

# Enable debug features
set(WITH_DEBUG ON CACHE BOOL "Enable debug build" FORCE)

# Enable all debugging and testing features
set(WITH_TESTS ON CACHE BOOL "Build tests" FORCE)
set(WITH_DEMOS ON CACHE BOOL "Build examples" FORCE)

# Enable developer-friendly features
set(WITH_COMPILER_CODE_COVERAGE ON CACHE BOOL "Enable code coverage" FORCE)
set(WITH_UIL_DEBUG ON CACHE BOOL "Enable UIL debug support" FORCE)

# Enable all optional features for comprehensive testing
set(WITH_UTF8 ON CACHE BOOL "Enable UTF-8 support" FORCE)
set(WITH_MESSAGE_CATALOG ON CACHE BOOL "Enable message catalog support" FORCE)
set(WITH_PRINTING ON CACHE BOOL "Enable printing support" FORCE)
set(WITH_JPEG ON CACHE BOOL "Enable JPEG support" FORCE)
set(WITH_PNG ON CACHE BOOL "Enable PNG support" FORCE)
set(WITH_XFT ON CACHE BOOL "Enable Xft support" FORCE)

# Enable documentation
set(WITH_DOCS ON CACHE BOOL "Enable documentation installation" FORCE)
set(WITH_WML_TOOLS ON CACHE BOOL "Enable WML tools" FORCE)

# Build both shared and static libraries for testing
set(WITH_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
set(WITH_STATIC_LIBS ON CACHE BOOL "Build static libraries" FORCE)

# Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose makefile output" FORCE)

# Enable debug information
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG" CACHE STRING "Debug C flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG" CACHE STRING "Debug C++ flags" FORCE)

# Enable assertions
add_compile_definitions(DEBUG=1)
add_compile_definitions(NDEBUG=0)
