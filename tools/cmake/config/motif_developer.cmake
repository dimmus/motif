# SPDX-FileCopyrightText: 2024 Motif Authors
#
# SPDX-License-Identifier: MIT

# Developer build configuration for Motif
# This configuration enables faster builds, error checking and tests, recommended for developers

# Set build type to Debug for development
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)

# Enable debug features
set(WITH_DEBUG ON CACHE BOOL "Enable debug build" FORCE)

# Enable all development features
set(WITH_TESTS ON CACHE BOOL "Build tests" FORCE)
set(WITH_DEMOS ON CACHE BOOL "Build demos" FORCE)

# Enable development tools
set(WITH_COMPILER_CODE_COVERAGE ON CACHE BOOL "Enable code coverage" FORCE)
set(WITH_UIL_DEBUG ON CACHE BOOL "Enable UIL debug support" FORCE)

# Enable all optional features for comprehensive testing
set(WITH_UTF8 ON CACHE BOOL "Enable UTF-8 support" FORCE)
set(WITH_MESSAGE_CATALOG ON CACHE BOOL "Enable message catalog support" FORCE)
set(WITH_PRINTING OFF CACHE BOOL "Enable printing support" FORCE)
set(WITH_JPEG ON CACHE BOOL "Enable JPEG support" FORCE)
set(WITH_PNG ON CACHE BOOL "Enable PNG support" FORCE)
set(WITH_XFT ON CACHE BOOL "Enable Xft support" FORCE)

# Enable documentation and tools
set(WITH_DOCS ON CACHE BOOL "Enable documentation installation" FORCE)
set(WITH_WML_TOOLS ON CACHE BOOL "Enable WML tools" FORCE)

# Build both shared and static libraries for testing
set(WITH_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
set(WITH_STATIC_LIBS ON CACHE BOOL "Build static libraries" FORCE)

# Enable developer tools
set(WITH_COMPILER_CCACHE ON CACHE BOOL "Enable ccache" FORCE)
set(WITH_NINJA_POOL_JOBS ON CACHE BOOL "Enable Ninja pool jobs" FORCE)

# Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Enable verbose makefile output" FORCE)

# Enable debug information with some optimization
set(CMAKE_C_FLAGS_DEBUG "-g -O1 -DDEBUG" CACHE STRING "Debug C flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O1 -DDEBUG" CACHE STRING "Debug C++ flags" FORCE)

# Enable assertions and debug macros
add_compile_definitions(DEBUG=1)
add_compile_definitions(NDEBUG=0)

# Enable additional debugging features
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Enable address sanitizer for development builds
    set(WITH_COMPILER_ASAN ON CACHE BOOL "Enable address sanitizer" FORCE)
    
    # Enable additional warnings
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -Wpedantic" CACHE STRING "Debug C flags" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic" CACHE STRING "Debug C++ flags" FORCE)
endif()

# Enable parallel builds for faster development
set(CMAKE_BUILD_PARALLEL_LEVEL "" CACHE STRING "Number of parallel jobs" FORCE)