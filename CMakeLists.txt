if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  if(NOT DEFINED WITH_IN_SOURCE_BUILD)
    message(FATAL_ERROR
      "CMake generation for Motif is not allowed within the source directory!"
      "\n Remove \"${CMAKE_SOURCE_DIR}/CMakeCache.txt\""
      "\n then try again from another directory, e.g.:"
      "\n "
      "\n rm -rf CMakeCache.txt CMakeFiles"
      "\n cd .."
      "\n mkdir cmake-make"
      "\n cd cmake-make"
      "\n cmake ../xde-motif"
      "\n "
      "\n Alternately define WITH_IN_SOURCE_BUILD to force this option (not recommended!)"
    )
  endif()
endif()

cmake_minimum_required(VERSION 3.16)

# Add UTF-8 encoding support for MSVC compiler.
# This ensures that the MSVC compiler interprets source files as UTF-8 encoded,
# which is useful for projects containing non-ASCII characters in source files.
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if(NOT EXECUTABLE_OUTPUT_PATH)
  set(FIRST_RUN TRUE)
else()
  set(FIRST_RUN FALSE)
endif()

# Avoid having an empty `CMAKE_BUILD_TYPE`.
if(NOT DEFINED CMAKE_BUILD_TYPE_INIT)
  set(CMAKE_BUILD_TYPE_INIT "Release")
  # set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
  # Internal logic caches this variable, avoid showing it by default
  # since it's easy to accidentally set instead of the build type.
endif()
mark_as_advanced(CMAKE_BUILD_TYPE_INIT)

# -----------------------------------------------------------------------------
# Set policy

# see "cmake --help-policy CMP0003"
# So library linking is more sane
cmake_policy(SET CMP0003 NEW)

# So BUILDINFO and BLENDERPATH strings are automatically quoted
cmake_policy(SET CMP0005 NEW)

# So syntax problems are errors
cmake_policy(SET CMP0010 NEW)

# Input directories must have CMakeLists.txt
cmake_policy(SET CMP0014 NEW)

# Silence draco warning on macOS, new policy works fine.
if(POLICY CMP0068)
  cmake_policy(SET CMP0068 NEW)
endif()

# find_package() uses <PackageName>_ROOT variables.
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

# find_package() uses uppercase <PackageName>_ROOT variables.
if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

# Install CODE|SCRIPT allow the use of generator expressions.
if(POLICY CMP0087)
  cmake_policy(SET CMP0087 NEW)
endif()

# Allow to specify language per file.
if(POLICY CMP0119)
  cmake_policy(SET CMP0119 NEW)
endif()

# Install DESTINATION paths are normalized.
if(POLICY CMP0177)
  cmake_policy(SET CMP0177 NEW)
endif()

include(CheckLibraryExists)
include(CheckIncludeFile)
include(CheckLanguage)
include(CheckSymbolExists)
include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckStructHasMember)
include(CMakeDependentOption)
include(CMakeParseArguments)
include(CMakePushCheckState)
# include(GNUInstallDirs)

# -----------------------------------------------------------------------------
# Load Local Macros

include(tools/cmake/motif_macros.cmake)
include(tools/cmake/motif_checks.cmake)
include(tools/cmake/motif_detect_compilers.cmake)
include(tools/cmake/motif_detect_cpu.cmake)
# include(tools/cmake/motif_packaging.cmake)
include(tools/cmake/motif_testing.cmake)
include(tools/cmake/motif_git.cmake)

# -----------------------------------------------------------------------------
# Check for ccache and use it if available

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    message(STATUS "Using ccache for faster builds")
else()
    message(STATUS "ccache not found - install ccache for faster builds")
endif()

# -----------------------------------------------------------------------------
# Initialize Project

motif_project_hack_pre()

project(MOTIF)
# project(MOTIF LANGUAGES CXX VERSION "2.4.1")

# Set install directory variables
include(GNUInstallDirs)

# Installation instructions
message(STATUS "Installation target: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "To install: sudo make install (or use GNUmakefile: make install)")

motif_project_hack_post()

enable_testing()

# -----------------------------------------------------------------------------
# Compilers and flags

motif_detect_compiler()
motif_detect_cpu(MOTIF_CPUS)

# -----------------------------------------------------------------------------
# Test Compiler Support

if(CMAKE_COMPILER_IS_GNUCC)
  if("${CMAKE_C_COMPILER_VERSION}" VERSION_LESS "11.0.0")
    message(FATAL_ERROR
      "The minimum supported version of GCC is 11.0.0, found C compiler: "
      "${CMAKE_C_COMPILER_VERSION}"
    )
  endif()
  if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "11.0.0")
    message(FATAL_ERROR
      "The minimum supported version of GCC is 11.0.0, found C++ compiler: "
      "${CMAKE_CXX_COMPILER_VERSION}"
    )
  endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
  if(CMAKE_COMPILER_IS_GNUCC)
    if("${CMAKE_C_COMPILER_VERSION}" VERSION_LESS "8.0")
      message(FATAL_ERROR
        "The minimum supported version of CLANG is 8.0, found C compiler: "
        "${CMAKE_C_COMPILER_VERSION}"
      )
    endif()
    if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "8.0")
      message(FATAL_ERROR
        "The minimum supported version of CLANG is 8.0, found C++ compiler: "
        "${CMAKE_CXX_COMPILER_VERSION}"
      )
    endif()
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  if(MSVC_VERSION VERSION_LESS "1928")
    # MSVC_VERSION is an internal version number, it doesn't map to something
    # the end user would recognize as a version. Because of this, for MSVC we do
    # not show the found number. When using our make.bat the actual VS version
    # will be displayed on the console before starting the build, anyway.
    message(FATAL_ERROR "The minimum supported version of MSVC is 2019 (16.9.16)")
  endif()
endif()

# -----------------------------------------------------------------------------
# Test Compiler/Library Features

include(tools/cmake/motif_have_features.cmake)

# -----------------------------------------------------------------------------
# Set Default Configuration Options

get_motif_version()

# -----------------------------------------------------------------------------
# Set global definitions

# Version information
set(XM_MAJOR 2)
set(XM_MINOR 4)
set(XM_PATCH 1)
set(XM_VERSION "${XM_MAJOR}.${XM_MINOR}.${XM_PATCH}")

# ABI version for shared libraries
set(ABI_CURRENT 5)
set(ABI_REVISION 0)
set(ABI_AGE 0)
set(ABI_VERSION "${ABI_CURRENT}:${ABI_REVISION}:${ABI_AGE}")

# -----------------------------------------------------------------------------
# Declare Options

set_option(WITH_SHARED_LIBS "Build shared libraries" ON)
set_option(WITH_STATIC_LIBS "Build static libraries" OFF)
set_option(WITH_DEMOS "Build demos" ON)
set_option(WITH_TESTS "Build tests" OFF)
set_option(WITH_DEBUG "Enable debug build" OFF)
set_option(WITH_UTF8 "Enable UTF-8 support" ON)
set_option(WITH_MESSAGE_CATALOG "Enable X/Open message catalog support" OFF)
set_option(WITH_PRINTING "Enable printing support" OFF)
set_option(WITH_UIL_DEBUG "Enable UIL compiler debugging support" OFF)
set_option(WITH_DOCS "Enable documentation installation" ON)
set_option(WITH_WML_TOOLS "Enable WML (Widget Meta-Language) tools" ON)
set_option(WITH_JPEG "Enable JPEG support" ON)
set_option(WITH_PNG "Enable PNG support" ON)
set_option(WITH_XFT "Enable Xft support" ON)

set(MOTIF_VENDOR_INFO "" CACHE STRING "Vendor name and/or version to add to MOTIF_REVISION")

set_option(WITH_COMPILER_CODE_COVERAGE "\
Build and link with code coverage support (only for Debug targets)."
  OFF
)
mark_as_advanced(WITH_COMPILER_CODE_COVERAGE)

set_option(WITH_COMPILER_ASAN "\
Build and link with address sanitizer support (only for Debug targets)."
  OFF
)
mark_as_advanced(WITH_COMPILER_ASAN)

if(WITH_COMPILER_CODE_COVERAGE)
  if(CMAKE_COMPILER_IS_GNUCC)
    set(_code_coverage_defaults "--coverage")
  elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    get_filename_component(COMPILER_DIRECTORY ${CMAKE_CXX_COMPILER} DIRECTORY)
    find_program(LLVM_COV "llvm-cov" ${COMPILER_DIRECTORY} NO_DEFAULT_PATH)
    find_program(LLVM_PROFDATA "llvm-profdata" ${COMPILER_DIRECTORY} NO_DEFAULT_PATH)
    if(NOT LLVM_COV OR NOT LLVM_PROFDATA)
      message(WARNING
        "Could not find code coverage tools, disabling code coverage. "
        "You may explicitly specify LLVM_COV and LLVM_PROFDATA to work around this warning."
      )
      set(WITH_COMPILER_CODE_COVERAGE OFF)
    else()
      set(_code_coverage_defaults "-fprofile-instr-generate -fcoverage-mapping")
    endif()
  else()
    message(WARNING
      "Unsupported compiler ${CMAKE_C_COMPILER_ID} for WITH_COMPILER_CODE_COVERAGE, disabling."
    )
    set(WITH_COMPILER_CODE_COVERAGE OFF)
  endif()

  # The code above could have disabled the feature, so check again.
  if(WITH_COMPILER_CODE_COVERAGE)
    set(COMPILER_CODE_COVERAGE_CFLAGS
      ${_code_coverage_defaults} CACHE STRING
      "C flags for code coverage"
    )
    mark_as_advanced(COMPILER_CODE_COVERAGE_CFLAGS)
    set(COMPILER_CODE_COVERAGE_CXXFLAGS
      ${_code_coverage_defaults} CACHE STRING
      "C++ flags for code coverage"
    )
    mark_as_advanced(COMPILER_CODE_COVERAGE_CXXFLAGS)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
      set(_code_coverage_dir_default "${CMAKE_BINARY_DIR}/coverage")
      set(COMPILER_CODE_COVERAGE_DATA_DIR ${_code_coverage_dir_default} CACHE STRING
        "Directory for code coverage data"
      )
      mark_as_advanced(COMPILER_CODE_COVERAGE_DATA_DIR)
      unset(_code_coverage_dir_default)
    endif()
    unset(_code_coverage_defaults)
  endif()
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  if(WITH_COMPILER_ASAN)
    set(_asan_defaults "\
-fsanitize=address \
-fsanitize=bool \
-fsanitize=bounds \
-fsanitize=enum \
-fsanitize=float-cast-overflow \
-fsanitize=float-divide-by-zero \
-fsanitize=nonnull-attribute \
-fsanitize=returns-nonnull-attribute \
-fsanitize=signed-integer-overflow \
-fsanitize=undefined \
-fsanitize=vla-bound \
-fno-sanitize=alignment \
")

    if(MSVC)
      # clang-cl doesn't support all sanitizers, but leak and object-size give errors/warnings.
      set(_asan_defaults "${_asan_defaults}")
    elseif(APPLE)
      # AppleClang doesn't support all sanitizers, but leak gives error.
      # Build type is not known for multi-config generator, so don't add object-size sanitizer.
      if(CMAKE_BUILD_TYPE MATCHES "Debug" OR GENERATOR_IS_MULTI_CONFIG)
        # Silence the warning that object-size is not effective in -O0.
        set(_asan_defaults "${_asan_defaults}")
      else()
        string(APPEND _asan_defaults " -fsanitize=object-size")
      endif()
    elseif(CMAKE_COMPILER_IS_GNUCC)
      string(APPEND _asan_defaults " -fsanitize=leak -fsanitize=object-size")
    else()
      string(APPEND _asan_defaults " -fsanitize=leak")
    endif()

    set(COMPILER_ASAN_CFLAGS "${_asan_defaults}" CACHE STRING "C flags for address sanitizer")
    mark_as_advanced(COMPILER_ASAN_CFLAGS)
    set(COMPILER_ASAN_CXXFLAGS "${_asan_defaults}" CACHE STRING "C++ flags for address sanitizer")
    mark_as_advanced(COMPILER_ASAN_CXXFLAGS)

    unset(_asan_defaults)

    if(MSVC)
      find_library(
        COMPILER_ASAN_LIBRARY NAMES clang_rt.asan-x86_64
        PATHS
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\LLVM\\LLVM;]/lib/clang/7.0.0/lib/windows
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\LLVM\\LLVM;]/lib/clang/6.0.0/lib/windows
      )
      mark_as_advanced(COMPILER_ASAN_LIBRARY)
    elseif(APPLE)
      execute_process(COMMAND ${CMAKE_CXX_COMPILER}
        -print-file-name=lib
        OUTPUT_VARIABLE CLANG_LIB_DIR
      )
      string(STRIP "${CLANG_LIB_DIR}" CLANG_LIB_DIR)
      find_library(
        COMPILER_ASAN_LIBRARY
        NAMES
          libclang_rt.asan_osx_dynamic.dylib
        PATHS
          "${CLANG_LIB_DIR}/darwin/"
      )
      unset(CLANG_LIB_DIR)
      mark_as_advanced(COMPILER_ASAN_LIBRARY)
    elseif(CMAKE_COMPILER_IS_GNUCC)
      find_library(
        COMPILER_ASAN_LIBRARY asan ${CMAKE_C_IMPLICIT_LINK_DIRECTORIES}
      )
      mark_as_advanced(COMPILER_ASAN_LIBRARY)
    endif()

  endif()
endif()

if(UNIX)
  # See WITH_WINDOWS_SCCACHE for Windows.
  option(WITH_COMPILER_CCACHE "\
Use ccache to improve rebuild times (Works with Ninja, Makefiles and Xcode)"
    OFF
  )
  mark_as_advanced(WITH_COMPILER_CCACHE)
endif()

# The following only works with the Ninja generator in CMake >= 3.0.
if("${CMAKE_GENERATOR}" MATCHES "Ninja")
  option(WITH_NINJA_POOL_JOBS "\
Enable Ninja pools of jobs, to try to ease building on machines with 16GB of RAM or less \
(if not yet defined, will try to set best values based on detected machine specifications)."
    ON
  )
  mark_as_advanced(WITH_NINJA_POOL_JOBS)
  
  if(WITH_NINJA_POOL_JOBS)
    # Set up Ninja job pools for better memory management
    set_property(GLOBAL PROPERTY JOB_POOLS compile_job_pool=4 link_job_pool=2)
    set(CMAKE_JOB_POOL_COMPILE compile_job_pool)
    set(CMAKE_JOB_POOL_LINK link_job_pool)
  endif()
endif()

# -----------------------------------------------------------------------------
# Logging options
set(LOG_LEVEL "INFO" CACHE STRING "Default logging level (DEBUG, INFO, WARN, ERROR, CRITICAL)")
set(LOG_OUTPUT "stderr" CACHE STRING "Default log output destination (stderr, stdout, file)")

# Set feature test macros for compatibility
add_compile_definitions(
    _GNU_SOURCE
    _DEFAULT_SOURCE
    _POSIX_C_SOURCE=200809L
    _BSD_SOURCE
    _XOPEN_SOURCE=600
    _XOPEN_SOURCE_EXTENDED=1
    _SVID_SOURCE
    _ATFILE_SOURCE
)

# Add necessary includes for system functions
include(CheckIncludeFile)
include(CheckFunctionExists)

check_include_file("alloca.h" HAVE_ALLOCA_H)
check_include_file("iconv.h" HAVE_ICONV_H)
check_include_file("wctype.h" HAVE_WCTYPE_H)
check_include_file("fcntl.h" HAVE_FCNTL_H)
check_include_file("sys/poll.h" HAVE_SYS_POLL_H)
check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_include_file("X11/Xos_r.h" HAVE_X11_XOS_R_H)
check_include_file("X11/Xpoll.h" HAVE_X11_XPOLL_H)

check_function_exists(getcwd HAVE_GETCWD)
check_function_exists(getwd HAVE_GETWD)
check_function_exists(iconv HAVE_ICONV)
check_function_exists(nanosleep HAVE_NANOSLEEP)
check_function_exists(putenv HAVE_PUTENV)
check_function_exists(regcomp HAVE_REGCOMP)
check_function_exists(regex HAVE_REGEX)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(strlcat HAVE_STRLCAT)
check_function_exists(getpwnam HAVE_GETPWNAM)
check_function_exists(getpwuid HAVE_GETPWUID)
check_function_exists(re_comp HAVE_RE_COMP)
check_function_exists(re_exec HAVE_RE_EXEC)

# Add compile definitions based on checks
if(HAVE_ALLOCA_H)
    add_compile_definitions(HAVE_ALLOCA_H)
endif()
if(HAVE_ICONV_H)
    add_compile_definitions(HAVE_ICONV_H)
endif()
if(HAVE_WCTYPE_H)
    add_compile_definitions(HAVE_WCTYPE_H)
endif()
if(HAVE_FCNTL_H)
    add_compile_definitions(HAVE_FCNTL_H)
endif()
if(HAVE_SYS_POLL_H)
    add_compile_definitions(HAVE_SYS_POLL_H)
endif()
if(HAVE_SYS_TIME_H)
    add_compile_definitions(HAVE_SYS_TIME_H)
endif()
if(HAVE_X11_XOS_R_H)
    add_compile_definitions(HAVE_X11_XOS_R_H)
endif()
if(HAVE_X11_XPOLL_H)
    add_compile_definitions(HAVE_X11_XPOLL_H)
endif()
if(HAVE_GETCWD)
    add_compile_definitions(HAVE_GETCWD)
endif()
if(HAVE_GETWD)
    add_compile_definitions(HAVE_GETWD)
endif()
if(HAVE_ICONV)
    add_compile_definitions(HAVE_ICONV)
endif()
if(HAVE_NANOSLEEP)
    add_compile_definitions(HAVE_NANOSLEEP)
endif()
if(HAVE_PUTENV)
    add_compile_definitions(HAVE_PUTENV)
endif()
if(HAVE_REGCOMP)
    add_compile_definitions(HAVE_REGCOMP)
endif()
if(HAVE_REGEX)
    add_compile_definitions(HAVE_REGEX)
endif()
if(HAVE_STRCASECMP)
    add_compile_definitions(HAVE_STRCASECMP)
endif()
if(HAVE_STRDUP)
    add_compile_definitions(HAVE_STRDUP)
endif()
if(HAVE_STRLCAT)
    add_compile_definitions(HAVE_STRLCAT)
endif()
if(HAVE_GETPWNAM)
    add_compile_definitions(HAVE_GETPWNAM)
endif()
if(HAVE_GETPWUID)
    add_compile_definitions(HAVE_GETPWUID)
endif()
if(HAVE_RE_COMP)
    add_compile_definitions(HAVE_RE_COMP)
endif()
if(HAVE_RE_EXEC)
    add_compile_definitions(HAVE_RE_EXEC)
endif()

# Compiler flags
if(WITH_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -ggdb -DDEBUG")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -DNDEBUG")
endif()

# -----------------------------------------------------------------------------
# Extra Compile Flags

if(CMAKE_COMPILER_IS_GNUCC)

  # ============================================================================
  # GCC C WARNING FLAGS - TURN WARNINGS ON
  # ============================================================================
  add_check_c_compiler_flags(
    C_WARNINGS_ON

    C_WARN_ALL -Wall
    C_WARN_ERROR_IMPLICIT_FUNCTION_DECLARATION -Werror=implicit-function-declaration
    C_WARN_STRICT_PROTOTYPES -Wstrict-prototypes
    C_WARN_ERROR_RETURN_TYPE -Werror=return-type
    C_WARN_ERROR_VLA -Werror=vla
    C_WARN_MISSING_PROTOTYPES -Wmissing-prototypes
    C_WARN_POINTER_ARITH -Wpointer-arith
    C_WARN_UNUSED_PARAMETER -Wunused-parameter
    C_WARN_WRITE_STRINGS -Wwrite-strings
    C_WARN_LOGICAL_OP -Wlogical-op
    C_WARN_UNDEF -Wundef
    C_WARN_INIT_SELF -Winit-self
    C_WARN_MISSING_INCLUDE_DIRS -Wmissing-include-dirs
    C_WARN_TYPE_LIMITS -Wtype-limits
    C_WARN_FORMAT_SIGN -Wformat-signedness
    C_WARN_RESTRICT -Wrestrict
    C_WARN_NO_NULL -Wnonnull
    C_WARN_ABSOLUTE_VALUE -Wabsolute-value
    C_WARN_UNINITIALIZED -Wuninitialized
    C_WARN_REDUNDANT_DECLS -Wredundant-decls
    C_WARN_SHADOW -Wshadow
    C_WARN_IMPLICIT_FALLTHROUGH -Wimplicit-fallthrough=5
  )

  # ============================================================================
  # GCC C WARNING FLAGS - TURN WARNINGS OFF
  # ============================================================================
  add_check_c_compiler_flags(
    C_WARNINGS_OFF

    C_WARN_NO_CHAR_SUBSCRIPTS -Wno-char-subscripts
    C_WARN_NO_UNKNOWN_PRAGMAS -Wno-unknown-pragmas
    C_WARN_NO_DIV_BY_ZERO -Wno-div-by-zero
    C_WARN_NO_STRINGOP_OVERREAD -Wno-stringop-overread
    C_WARN_NO_STRINGOP_OVERFLOW -Wno-stringop-overflow
  )

  # Combine both categories
  set(C_WARNINGS "${C_WARNINGS_ON} ${C_WARNINGS_OFF}")

  if(NOT APPLE)
    add_check_c_compiler_flags(
      C_WARNINGS
      C_WARN_NO_ERROR_UNUSED_BUT_SET_VARIABLE -Wno-error=unused-but-set-variable
    )
  endif()

  # Add specific warning suppressions for Motif
  add_check_c_compiler_flags(
    C_WARNINGS
    C_WARN_NO_UNUSED_PARAMETER -Wno-unused-parameter
    C_WARN_NO_DISCARDED_QUALIFIERS -Wno-discarded-qualifiers
    C_WARN_NO_MISSING_PROTOTYPES -Wno-missing-prototypes
    C_WARN_NO_REDUNDANT_DECLS -Wno-redundant-decls
    C_WARN_NO_STRICT_PROTOTYPES -Wno-strict-prototypes
    C_WARN_NO_IMPLICIT_FALLTHROUGH -Wno-implicit-fallthrough
    C_WARN_NO_MISLEADING_INDENTATION -Wno-misleading-indentation
    C_WARN_NO_TYPE_LIMITS -Wno-type-limits
  )

  # ============================================================================
  # GCC C++ WARNING FLAGS - TURN WARNINGS ON
  # ============================================================================
  add_check_cxx_compiler_flags(
    CXX_WARNINGS_ON

    CXX_WARN_UNINITIALIZED -Wuninitialized
    CXX_WARN_REDUNDANT_DECLS -Wredundant-decls
    CXX_WARN_ALL -Wall
    CXX_WARN_LOGICAL_OP -Wlogical-op
    CXX_WARN_INIT_SELF -Winit-self
    CXX_WARN_MISSING_INCLUDE_DIRS -Wmissing-include-dirs
    CXX_WARN_TYPE_LIMITS -Wtype-limits
    CXX_WARN_ERROR_RETURN_TYPE -Werror=return-type
    CXX_WARN_POINTER_ARITH -Wpointer-arith
    CXX_WARN_UNUSED_PARAMETER -Wunused-parameter
    CXX_WARN_WRITE_STRINGS -Wwrite-strings
    CXX_WARN_UNDEF -Wundef
    CXX_WARN_COMMA_SUBSCRIPT -Wcomma-subscript
    CXX_WARN_FORMAT_SIGN -Wformat-signedness
    CXX_WARN_RESTRICT -Wrestrict
    CXX_WARN_IMPLICIT_FALLTHROUGH -Wimplicit-fallthrough=5
  )

  # ============================================================================
  # GCC C++ WARNING FLAGS - TURN WARNINGS OFF
  # ============================================================================
  add_check_cxx_compiler_flags(
    CXX_WARNINGS_OFF

    CXX_WARN_NO_INVALID_OFFSETOF -Wno-invalid-offsetof
    CXX_WARN_NO_SIGN_COMPARE -Wno-sign-compare
    CXX_WARN_NO_DIV_BY_ZERO -Wno-div-by-zero
    CXX_WARN_NO_CHAR_SUBSCRIPTS -Wno-char-subscripts
    CXX_WARN_NO_UNKNOWN_PRAGMAS -Wno-unknown-pragmas
    CXX_WARN_NO_SUGGEST_OVERRIDE -Wno-suggest-override
    CXX_WARN_NO_STRINGOP_OVERREAD -Wno-stringop-overread
    CXX_WARN_NO_STRINGOP_OVERFLOW -Wno-stringop-overflow
  )

  # Combine both categories
  set(CXX_WARNINGS "${CXX_WARNINGS_ON} ${CXX_WARNINGS_OFF}")

  # causes too many warnings
  if(NOT APPLE)
    add_check_cxx_compiler_flags(
      CXX_WARNINGS
      CXX_WARN_UNDEF -Wundef
      CXX_WARN_MISSING_DECLARATIONS -Wmissing-declarations
    )
  endif()

  # ---------------------
  # Suppress Strict Flags
  #
  # Exclude the following warnings from this list:
  # - `-Wno-address`:
  #   This can give useful hints that point to bugs/misleading logic.
  # - `-Wno-strict-prototypes`:
  #   No need to support older C-style prototypes.
  #
  # If code in `./extern/` needs to suppress these flags that can be done on a case-by-case basis.

  # flags to undo strict flags
  add_check_c_compiler_flags(
    C_REMOVE_STRICT_FLAGS

    C_WARN_NO_DEPRECATED_DECLARATIONS -Wno-deprecated-declarations
    C_WARN_NO_UNUSED_PARAMETER -Wno-unused-parameter
    C_WARN_NO_UNUSED_FUNCTION -Wno-unused-function
    C_WARN_NO_TYPE_LIMITS -Wno-type-limits
    C_WARN_NO_INT_IN_BOOL_CONTEXT -Wno-int-in-bool-context
    C_WARN_NO_FORMAT -Wno-format
    C_WARN_NO_SWITCH -Wno-switch
    C_WARN_NO_UNUSED_VARIABLE -Wno-unused-variable
    C_WARN_NO_UNUSED_VARIABLE -Wno-uninitialized
    C_WARN_NO_IMPLICIT_FALLTHROUGH -Wno-implicit-fallthrough
  )


  add_check_cxx_compiler_flags(
    CXX_REMOVE_STRICT_FLAGS

    CXX_WARN_NO_CLASS_MEMACCESS -Wno-class-memaccess
    CXX_WARN_NO_COMMENT -Wno-comment
    CXX_WARN_NO_UNUSED_TYPEDEFS -Wno-unused-local-typedefs
    CXX_WARN_NO_UNUSED_VARIABLE -Wno-unused-variable
    CXX_WARN_NO_UNINITIALIZED -Wno-uninitialized
    CXX_WARN_NO_REORDER -Wno-reorder
  )


  if(NOT APPLE)
    add_check_c_compiler_flags(
      C_REMOVE_STRICT_FLAGS
      C_WARN_NO_ERROR_UNUSED_BUT_SET_VARIABLE -Wno-error=unused-but-set-variable
    )
  endif()

elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
  # Matches both "Clang" & "AppleClang" on macOS.

  # ============================================================================
  # CLANG C WARNING FLAGS - TURN WARNINGS ON
  # ============================================================================
  add_check_c_compiler_flags(
    C_WARNINGS_ON

    C_WARN_ALL -Wall
    C_WARN_ERROR_IMPLICIT_FUNCTION_DECLARATION -Werror=implicit-function-declaration
    C_WARN_ERROR_RETURN_TYPE -Werror=return-type
    C_WARN_STRICT_PROTOTYPES -Wstrict-prototypes
    C_WARN_MISSING_PROTOTYPES -Wmissing-prototypes
    C_WARN_UNUSED_PARAMETER -Wunused-parameter
    C_WARN_UNDEF -Wundef
    C_WARN_UNDEF_PREFIX -Wundef-prefix
    C_WARN_ERROR_UNGUARDED_AVAILABILITY_NEW -Werror=unguarded-availability-new
  )

  # ============================================================================
  # CLANG C WARNING FLAGS - TURN WARNINGS OFF
  # ============================================================================
  add_check_c_compiler_flags(
    C_WARNINGS_OFF

    C_WARN_NO_AUTOLOGICAL_COMPARE -Wno-tautological-compare
    C_WARN_NO_UNKNOWN_PRAGMAS -Wno-unknown-pragmas
    C_WARN_NO_CHAR_SUBSCRIPTS -Wno-char-subscripts
  )

  # Combine both categories
  set(C_WARNINGS "${C_WARNINGS_ON} ${C_WARNINGS_OFF}")

  # Add specific warning suppressions for Motif
  add_check_c_compiler_flags(
    C_WARNINGS
    C_WARN_NO_UNUSED_PARAMETER -Wno-unused-parameter
    C_WARN_NO_DISCARDED_QUALIFIERS -Wno-discarded-qualifiers
    C_WARN_NO_MISSING_PROTOTYPES -Wno-missing-prototypes
    C_WARN_NO_REDUNDANT_DECLS -Wno-redundant-decls
    C_WARN_NO_STRICT_PROTOTYPES -Wno-strict-prototypes
    C_WARN_NO_IMPLICIT_FALLTHROUGH -Wno-implicit-fallthrough
    C_WARN_NO_MISLEADING_INDENTATION -Wno-misleading-indentation
    C_WARN_NO_TYPE_LIMITS -Wno-type-limits
  )

  # ============================================================================
  # CLANG C++ WARNING FLAGS - TURN WARNINGS ON
  # ============================================================================
  add_check_cxx_compiler_flags(
    CXX_WARNINGS_ON

    CXX_WARN_ALL -Wall
    CXX_CXX20_DESIGNATOR -Wc++20-designator
    CXX_WARN_UNDEF -Wundef
    CXX_WARN_UNDEF_PREFIX -Wundef-prefix
    CXX_WARN_UNUSED_PARAMETER -Wunused-parameter
    CXX_WARN_MISMATCHED_TAGS -Wmismatched-tags
    CXX_WARN_ERROR_UNGUARDED_AVAILABILITY_NEW -Werror=unguarded-availability-new
  )

  # ============================================================================
  # CLANG C++ WARNING FLAGS - TURN WARNINGS OFF
  # ============================================================================
  add_check_cxx_compiler_flags(
    CXX_WARNINGS_OFF

    CXX_WARN_NO_AUTOLOGICAL_COMPARE -Wno-tautological-compare
    CXX_WARN_NO_UNKNOWN_PRAGMAS -Wno-unknown-pragmas
    CXX_WARN_NO_CHAR_SUBSCRIPTS -Wno-char-subscripts
    CXX_WARN_NO_OVERLOADED_VIRTUAL -Wno-overloaded-virtual
    CXX_WARN_NO_SIGN_COMPARE -Wno-sign-compare
    CXX_WARN_NO_INVALID_OFFSETOF -Wno-invalid-offsetof
    CXX_WARN_NO_SUGGEST_OVERRIDE -Wno-suggest-override
  )

  # Combine both categories
  set(CXX_WARNINGS "${CXX_WARNINGS_ON} ${CXX_WARNINGS_OFF}")
  if(MSVC_CLANG)
    # clang-cl produces an unhealthy amount of warnings in its default
    # configuration as it for reasons unknown decided to enable all
    # warnings known to mankind. Resulting in a 5.5GB build log containing
    # well over 11 million warnings. The code below disables every single
    # one of them indiscriminately. Someone with time on their hands,
    # could/should go over these and either fix them or describe why we
    # would want to disable the warning. The list below contains both C
    # and C++ warnings for all warnings since clang has seemingly no
    # easy way to tell if something is a C or C++ specific warning and
    # manually auditing every single one of them just isn't in the cards
    # right now.

    # /W3 is being removed, then added back again, this is because order
    # matters for clang and these flags are being placed before the
    # CMAKE_[LANGUAGE]_FLAGS which normally contain /W3, so we would
    # disable certain warnings here only for them to be re-enabled by /W3
    # later on.
    remove_cc_flag("/W3")

    add_check_c_compiler_flags(
      C_WARNINGS
      C_WARN_CLANG_CL_W3 /W3
      # The number behind each warn is the number of unique warning were
      # generated on 2024-04-24 (d2be9cecc28a03ff1f799e8c63f1f9f8eda7cce3)
      # especially the ones in the single and low double digits are likely
      # genuine problems that can be investigated.
      C_WARN_CLANG_CL_C++98_COMPAT -Wno-c++98-compat # 352692
      C_WARN_CLANG_CL_OLD_STYLE_CAST -Wno-old-style-cast # 178608
      C_WARN_CLANG_CL_UNSAFE_BUFFER_USAGE -Wno-unsafe-buffer-usage # 89032
      C_WARN_CLANG_CL_MISSING_PROTOTYPES -Wno-missing-prototypes # 25587
      C_WARN_CLANG_CL_SIGN_CONVERSION -Wno-sign-conversion # 20109
      C_WARN_CLANG_CL_MISSING_FIELD_INITIALIZERS -Wno-missing-field-initializers # 20060
      C_WARN_CLANG_CL_EXTRA_SEMI -Wno-extra-semi # 12513
      C_WARN_CLANG_CL_LANGUAGE_EXTENSION_TOKEN -Wno-language-extension-token # 11032
      C_WARN_CLANG_CL_IMPLICIT_FLOAT_CONVERSION -Wno-implicit-float-conversion # 11003
      C_WARN_CLANG_CL_C++98_COMPAT_PEDANTIC -Wno-c++98-compat-pedantic # 10336
      C_WARN_CLANG_CL_IMPLICIT_INT_FLOAT_CONVERSION -Wno-implicit-int-float-conversion # 7354
      C_WARN_CLANG_CL_DOUBLE_PROMOTION -Wno-double-promotion # 7350
      C_WARN_CLANG_CL_PRE_C++17_COMPAT -Wno-pre-c++17-compat # 7303
      C_WARN_CLANG_CL_SHORTEN_64_TO_32 -Wno-shorten-64-to-32 # 7085
      C_WARN_CLANG_CL_C++98_COMPAT_LOCAL_TYPE_TEMPLATE_ARGS -Wno-c++98-compat-local-type-template-args # 6906
      C_WARN_CLANG_CL_RESERVED_IDENTIFIER -Wno-reserved-identifier # 5886
      C_WARN_CLANG_CL_CAST_ALIGN -Wno-cast-align # 5513
      C_WARN_CLANG_CL_DOCUMENTATION -Wno-documentation # 5107
      C_WARN_CLANG_CL_DISABLED_MACRO_EXPANSION -Wno-disabled-macro-expansion # 4449
      C_WARN_CLANG_CL_EXTRA_SEMI_STMT -Wno-extra-semi-stmt # 4349
      C_WARN_CLANG_CL_ZERO_AS_NULL_POINTER_CONSTANT -Wno-zero-as-null-pointer-constant # 3209
      C_WARN_CLANG_CL_FLOAT_CONVERSION -Wno-float-conversion # 2869
      C_WARN_CLANG_CL_RESERVED_MACRO_IDENTIFIER -Wno-reserved-macro-identifier # 2862
      C_WARN_CLANG_CL_CAST_FUNCTION_TYPE_STRICT -Wno-cast-function-type-strict # 2663
      C_WARN_CLANG_CL_FLOAT_EQUAL -Wno-float-equal # 2153
      C_WARN_CLANG_CL_IMPLICIT_INT_CONVERSION -Wno-implicit-int-conversion # 2117
      C_WARN_CLANG_CL_SHADOW -Wno-shadow # 2068
      C_WARN_CLANG_CL_SHADOW_FIELD_IN_CONSTRUCTOR -Wno-shadow-field-in-constructor # 1829
      C_WARN_CLANG_CL_CAST_QUAL -Wno-cast-qual # 1742
      C_WARN_CLANG_CL_PRE_C++14_COMPAT -Wno-pre-c++14-compat # 1569
      C_WARN_CLANG_CL_GLOBAL_CONSTRUCTORS -Wno-global-constructors # 1402
      C_WARN_CLANG_CL_SWITCH_ENUM -Wno-switch-enum # 973
      C_WARN_CLANG_CL_EXIT_TIME_DESTRUCTORS -Wno-exit-time-destructors # 940
      C_WARN_CLANG_CL_CTAD_MAYBE_UNSUPPORTED -Wno-ctad-maybe-unsupported # 891
      C_WARN_CLANG_CL_UNDEFINED_FUNC_TEMPLATE -Wno-undefined-func-template # 863
      C_WARN_CLANG_CL_C++98_COMPAT_EXTRA_SEMI -Wno-c++98-compat-extra-semi # 848
      C_WARN_CLANG_CL_CAST_FUNCTION_TYPE -Wno-cast-function-type # 807
      C_WARN_CLANG_CL_NULLABILITY_EXTENSION -Wno-nullability-extension # 602
      C_WARN_CLANG_CL_SHADOW_FIELD -Wno-shadow-field # 585
      C_WARN_CLANG_CL_CONDITIONAL_UNINITIALIZED -Wno-conditional-uninitialized # 555
      C_WARN_CLANG_CL_UNUSED_PARAMETER -Wno-unused-parameter # 539
      C_WARN_CLANG_CL_SUGGEST_DESTRUCTOR_OVERRIDE -Wno-suggest-destructor-override # 356
      C_WARN_CLANG_CL_SHADOW_UNCAPTURED_LOCAL -Wno-shadow-uncaptured-local # 355
      C_WARN_CLANG_CL_UNUSED_MACROS -Wno-unused-macros # 289
      C_WARN_CLANG_CL_COVERED_SWITCH_DEFAULT -Wno-covered-switch-default # 233
      C_WARN_CLANG_CL_SIGNED_ENUM_BITFIELD -Wno-signed-enum-bitfield # 229
      C_WARN_CLANG_CL_DECLARATION_AFTER_STATEMENT -Wno-declaration-after-statement # 228
      C_WARN_CLANG_CL_IMPLICIT_FALLTHROUGH -Wno-implicit-fallthrough # 164
      C_WARN_CLANG_CL_NON_VIRTUAL_DTOR -Wno-non-virtual-dtor # 161
      C_WARN_CLANG_CL_NESTED_ANON_TYPES -Wno-nested-anon-types # 140
      C_WARN_CLANG_CL_GNU_ZERO_VARIADIC_MACRO_ARGUMENTS -Wno-gnu-zero-variadic-macro-arguments # 132
      C_WARN_CLANG_CL_UNREACHABLE_CODE_BREAK -Wno-unreachable-code-break # 115
      C_WARN_CLANG_CL_INCONSISTENT_MISSING_DESTRUCTOR_OVERRIDE -Wno-inconsistent-missing-destructor-override # 104
      C_WARN_CLANG_CL_FORMAT_PEDANTIC -Wno-format-pedantic # 97
      C_WARN_CLANG_CL_NONPORTABLE_SYSTEM_INCLUDE_PATH -Wno-nonportable-system-include-path # 95
      C_WARN_CLANG_CL_UNDEF -Wno-undef # 94
      C_WARN_CLANG_CL_IGNORED_QUALIFIERS -Wno-ignored-qualifiers # 93
      C_WARN_CLANG_CL_USED_BUT_MARKED_UNUSED -Wno-used-but-marked-unused # 83
      C_WARN_CLANG_CL_HEADER_HYGIENE -Wno-header-hygiene # 79
      C_WARN_CLANG_CL_CHAR_SUBSCRIPTS -Wno-char-subscripts # 76
      C_WARN_CLANG_CL_UNREACHABLE_CODE_RETURN -Wno-unreachable-code-return # 71
      C_WARN_CLANG_CL_UNUSED_TEMPLATE -Wno-unused-template # 66
      C_WARN_CLANG_CL_GNU_ANONYMOUS_STRUCT -Wno-gnu-anonymous-struct # 63
      C_WARN_CLANG_CL_DEPRECATED_COPY_WITH_USER_PROVIDED_DTOR -Wno-deprecated-copy-with-user-provided-dtor # 62
      C_WARN_CLANG_CL_INCONSISTENT_MISSING_OVERRIDE -Wno-inconsistent-missing-override # 54
      C_WARN_CLANG_CL_UNREACHABLE_CODE -Wno-unreachable-code # 52
      C_WARN_CLANG_CL_DEPRECATED_DYNAMIC_EXCEPTION_SPEC -Wno-deprecated-dynamic-exception-spec # 51
      C_WARN_CLANG_CL_BAD_FUNCTION_CAST -Wno-bad-function-cast # 50
      C_WARN_CLANG_CL_MICROSOFT_ENUM_VALUE -Wno-microsoft-enum-value # 47
      C_WARN_CLANG_CL_DEPRECATED_COPY_WITH_USER_PROVIDED_COPY -Wno-deprecated-copy-with-user-provided-copy # 41
      C_WARN_CLANG_CL_ZERO_LENGTH_ARRAY -Wno-zero-length-array # 39
      C_WARN_CLANG_CL_UNUSED_FUNCTION -Wno-unused-function # 38
      C_WARN_CLANG_CL_PEDANTIC -Wno-pedantic # 38
      C_WARN_CLANG_CL_DEPRECATED_COPY_WITH_DTOR -Wno-deprecated-copy-with-dtor # 37
      C_WARN_CLANG_CL_DOCUMENTATION_UNKNOWN_COMMAND -Wno-documentation-unknown-command # 34
      C_WARN_CLANG_CL_UNDEFINED_REINTERPRET_CAST -Wno-undefined-reinterpret-cast # 33
      C_WARN_CLANG_CL_FORMAT_NONLITERAL -Wno-format-nonliteral # 29
      C_WARN_CLANG_CL_COMMA -Wno-comma # 27
      C_WARN_CLANG_CL_DOCUMENTATION_DEPRECATED_SYNC -Wno-documentation-deprecated-sync # 26
      C_WARN_CLANG_CL_SHIFT_SIGN_OVERFLOW -Wno-shift-sign-overflow # 24
      C_WARN_CLANG_CL_PRE_C++17_COMPAT_PEDANTIC -Wno-pre-c++17-compat-pedantic # 24
      C_WARN_CLANG_CL_C++98_COMPAT_UNNAMED_TYPE_TEMPLATE_ARGS -Wno-c++98-compat-unnamed-type-template-args # 22
      C_WARN_CLANG_CL_SIGN_COMPARE -Wno-sign-compare # 21
      C_WARN_CLANG_CL_FORMAT -Wno-format # 21
      C_WARN_CLANG_CL_C++98_COMPAT_BIND_TO_TEMPORARY_COPY -Wno-c++98-compat-bind-to-temporary-copy # 21
      C_WARN_CLANG_CL_ENUM_ENUM_CONVERSION -Wno-enum-enum-conversion # 20
      C_WARN_CLANG_CL_ANON_ENUM_ENUM_CONVERSION -Wno-anon-enum-enum-conversion # 14
      C_WARN_CLANG_CL_RANGE_LOOP_BIND_REFERENCE -Wno-range-loop-bind-reference # 14
      C_WARN_CLANG_CL_ENUM_FLOAT_CONVERSION -Wno-enum-float-conversion # 12
      C_WARN_CLANG_CL_KEYWORD_MACRO -Wno-keyword-macro # 10
      C_WARN_CLANG_CL_DEPRECATED_COPY -Wno-deprecated-copy # 10
      C_WARN_CLANG_CL_UNUSED_MEMBER_FUNCTION -Wno-unused-member-function # 9
      C_WARN_CLANG_CL_MISSING_NORETURN -Wno-missing-noreturn # 8
      C_WARN_CLANG_CL_MISSING_VARIABLE_DECLARATIONS -Wno-missing-variable-declarations # 8
      C_WARN_CLANG_CL_DOCUMENTATION_HTML -Wno-documentation-html # 6
      C_WARN_CLANG_CL_GNU_REDECLARED_ENUM -Wno-gnu-redeclared-enum # 6
      C_WARN_CLANG_CL_DEPRECATED_DECLARATIONS -Wno-deprecated-declarations # 6
      C_WARN_CLANG_CL_OVERLOADED_VIRTUAL -Wno-overloaded-virtual # 5
      C_WARN_CLANG_CL_C++98_C++11_COMPAT_BINARY_LITERAL -Wno-c++98-c++11-compat-binary-literal # 4
      C_WARN_CLANG_CL_DEPRECATED_REDUNDANT_CONSTEXPR_STATIC_DEF -Wno-deprecated-redundant-constexpr-static-def # 4
      C_WARN_CLANG_CL_MISSING_BRACES -Wno-missing-braces # 4
      C_WARN_CLANG_CL_C99_EXTENSIONS -Wno-c99-extensions # 4
      C_WARN_CLANG_CL_STRICT_PROTOTYPES -Wno-strict-prototypes # 4
      C_WARN_CLANG_CL_UNREACHABLE_CODE_LOOP_INCREMENT -Wno-unreachable-code-loop-increment # 4
      C_WARN_CLANG_CL_GNU_CASE_RANGE -Wno-gnu-case-range # 4
      C_WARN_CLANG_CL_DUPLICATE_ENUM -Wno-duplicate-enum # 3
      C_WARN_CLANG_CL_NULL_POINTER_SUBTRACTION -Wno-null-pointer-subtraction # 2
      C_WARN_CLANG_CL_DEPRECATED_LITERAL_OPERATOR -Wno-deprecated-literal-operator # 2
      C_WARN_CLANG_CL_NEWLINE_EOF -Wno-newline-eof # 2
      C_WARN_CLANG_CL_MICROSOFT_CAST -Wno-microsoft-cast # 2
      C_WARN_CLANG_CL_DATE_TIME -Wno-date-time # 2
      C_WARN_CLANG_CL_DELETE_NON_ABSTRACT_NON_VIRTUAL_DTOR -Wno-delete-non-abstract-non-virtual-dtor # 2
      C_WARN_CLANG_CL_UNUSED_PRIVATE_FIELD -Wno-unused-private-field # 2
      C_WARN_CLANG_CL_FLEXIBLE_ARRAY_EXTENSIONS -Wno-flexible-array-extensions # 2
      C_WARN_CLANG_CL_STRING_CONVERSION -Wno-string-conversion # 2
      C_WARN_CLANG_CL_FINAL_DTOR_NON_FINAL_CLASS -Wno-final-dtor-non-final-class # 2
      C_WARN_CLANG_CL_MICROSOFT_UNQUALIFIED_FRIEND -Wno-microsoft-unqualified-friend # 2
      C_WARN_CLANG_CL_INVALID_NORETURN -Wno-invalid-noreturn # 1
      C_WARN_CLANG_CL_INVALID_UTF8 -Wno-invalid-utf8 # 1
      C_WARN_CLANG_CL_FOUR_CHAR_CONSTANTS -Wno-four-char-constants # 1
      C_WARN_CLANG_CL_PARENTHESES -Wno-parentheses # 1
      C_WARN_CLANG_CL_PESSIMIZING_MOVE -Wno-pessimizing-move # 1
      C_WARN_CLANG_CL_DEPRECATED_NON_PROTOTYPE -Wno-deprecated-non-prototype # 1
      C_WARN_CLANG_CL_BITFIELD_ENUM_CONVERSION -Wno-bitfield-enum-conversion # 1
      C_WARN_CLANG_CL_UNUSED_LAMBDA_CAPTURE -Wno-unused-lambda-capture # 1
      C_WARN_CLANG_CL_SHADOW_FIELD_IN_CONSTRUCTOR_MODIFIED -Wno-shadow-field-in-constructor-modified # 1
      # And some additional ones that came up when using LLVM 18.1.8 on Windows ARM64
      C_WARN_CLANG_CL_SWITCH_DEFAULT -Wno-switch-default
      C_WARN_CLANG_CL_NAN_INFINITY_DISABLED -Wno-nan-infinity-disabled
      # And another from 19.1.5
      C_WARN_CLANG_CL_PRE_C11_COMPAT -Wno-pre-c11-compat
    )

    add_check_cxx_compiler_flags(
      CXX_WARNINGS
      CXX_WARN_CLANG_CL_W3 /W3
      CXX_WARN_CLANG_CL_C++98_COMPAT -Wno-c++98-compat # 352692
      CXX_WARN_CLANG_CL_OLD_STYLE_CAST -Wno-old-style-cast # 178608
      CXX_WARN_CLANG_CL_UNSAFE_BUFFER_USAGE -Wno-unsafe-buffer-usage # 89032
      CXX_WARN_CLANG_CL_MISSING_PROTOTYPES -Wno-missing-prototypes # 25587
      CXX_WARN_CLANG_CL_SIGN_CONVERSION -Wno-sign-conversion # 20109
      CXX_WARN_CLANG_CL_MISSING_FIELD_INITIALIZERS -Wno-missing-field-initializers # 20060
      CXX_WARN_CLANG_CL_EXTRA_SEMI -Wno-extra-semi # 12513
      CXX_WARN_CLANG_CL_LANGUAGE_EXTENSION_TOKEN -Wno-language-extension-token # 11032
      CXX_WARN_CLANG_CL_IMPLICIT_FLOAT_CONVERSION -Wno-implicit-float-conversion # 11003
      CXX_WARN_CLANG_CL_C++98_COMPAT_PEDANTIC -Wno-c++98-compat-pedantic # 10336
      CXX_WARN_CLANG_CL_IMPLICIT_INT_FLOAT_CONVERSION -Wno-implicit-int-float-conversion # 7354
      CXX_WARN_CLANG_CL_DOUBLE_PROMOTION -Wno-double-promotion # 7350
      CXX_WARN_CLANG_CL_PRE_C++17_COMPAT -Wno-pre-c++17-compat # 7303
      CXX_WARN_CLANG_CL_SHORTEN_64_TO_32 -Wno-shorten-64-to-32 # 7085
      CXX_WARN_CLANG_CL_C++98_COMPAT_LOCAL_TYPE_TEMPLATE_ARGS -Wno-c++98-compat-local-type-template-args # 6906
      CXX_WARN_CLANG_CL_RESERVED_IDENTIFIER -Wno-reserved-identifier # 5886
      CXX_WARN_CLANG_CL_CAST_ALIGN -Wno-cast-align # 5513
      CXX_WARN_CLANG_CL_DOCUMENTATION -Wno-documentation # 5107
      CXX_WARN_CLANG_CL_DISABLED_MACRO_EXPANSION -Wno-disabled-macro-expansion # 4449
      CXX_WARN_CLANG_CL_EXTRA_SEMI_STMT -Wno-extra-semi-stmt # 4349
      CXX_WARN_CLANG_CL_ZERO_AS_NULL_POINTER_CONSTANT -Wno-zero-as-null-pointer-constant # 3209
      CXX_WARN_CLANG_CL_FLOAT_CONVERSION -Wno-float-conversion # 2869
      CXX_WARN_CLANG_CL_RESERVED_MACRO_IDENTIFIER -Wno-reserved-macro-identifier # 2862
      CXX_WARN_CLANG_CL_CAST_FUNCTION_TYPE_STRICT -Wno-cast-function-type-strict # 2663
      CXX_WARN_CLANG_CL_FLOAT_EQUAL -Wno-float-equal # 2153
      CXX_WARN_CLANG_CL_IMPLICIT_INT_CONVERSION -Wno-implicit-int-conversion # 2117
      CXX_WARN_CLANG_CL_SHADOW -Wno-shadow # 2068
      CXX_WARN_CLANG_CL_SHADOW_FIELD_IN_CONSTRUCTOR -Wno-shadow-field-in-constructor # 1829
      CXX_WARN_CLANG_CL_CAST_QUAL -Wno-cast-qual # 1742
      CXX_WARN_CLANG_CL_PRE_C++14_COMPAT -Wno-pre-c++14-compat # 1569
      CXX_WARN_CLANG_CL_GLOBAL_CONSTRUCTORS -Wno-global-constructors # 1402
      CXX_WARN_CLANG_CL_SWITCH_ENUM -Wno-switch-enum # 973
      CXX_WARN_CLANG_CL_EXIT_TIME_DESTRUCTORS -Wno-exit-time-destructors # 940
      CXX_WARN_CLANG_CL_CTAD_MAYBE_UNSUPPORTED -Wno-ctad-maybe-unsupported # 891
      CXX_WARN_CLANG_CL_UNDEFINED_FUNC_TEMPLATE -Wno-undefined-func-template # 863
      CXX_WARN_CLANG_CL_C++98_COMPAT_EXTRA_SEMI -Wno-c++98-compat-extra-semi # 848
      CXX_WARN_CLANG_CL_CAST_FUNCTION_TYPE -Wno-cast-function-type # 807
      CXX_WARN_CLANG_CL_NULLABILITY_EXTENSION -Wno-nullability-extension # 602
      CXX_WARN_CLANG_CL_SHADOW_FIELD -Wno-shadow-field # 585
      CXX_WARN_CLANG_CL_CONDITIONAL_UNINITIALIZED -Wno-conditional-uninitialized # 555
      CXX_WARN_CLANG_CL_UNUSED_PARAMETER -Wno-unused-parameter # 539
      CXX_WARN_CLANG_CL_SUGGEST_DESTRUCTOR_OVERRIDE -Wno-suggest-destructor-override # 356
      CXX_WARN_CLANG_CL_SHADOW_UNCAPTURED_LOCAL -Wno-shadow-uncaptured-local # 355
      CXX_WARN_CLANG_CL_UNUSED_MACROS -Wno-unused-macros # 289
      CXX_WARN_CLANG_CL_COVERED_SWITCH_DEFAULT -Wno-covered-switch-default # 233
      CXX_WARN_CLANG_CL_SIGNED_ENUM_BITFIELD -Wno-signed-enum-bitfield # 229
      CXX_WARN_CLANG_CL_DECLARATION_AFTER_STATEMENT -Wno-declaration-after-statement # 228
      CXX_WARN_CLANG_CL_IMPLICIT_FALLTHROUGH -Wno-implicit-fallthrough # 164
      CXX_WARN_CLANG_CL_NON_VIRTUAL_DTOR -Wno-non-virtual-dtor # 161
      CXX_WARN_CLANG_CL_NESTED_ANON_TYPES -Wno-nested-anon-types # 140
      CXX_WARN_CLANG_CL_GNU_ZERO_VARIADIC_MACRO_ARGUMENTS -Wno-gnu-zero-variadic-macro-arguments # 132
      CXX_WARN_CLANG_CL_UNREACHABLE_CODE_BREAK -Wno-unreachable-code-break # 115
      CXX_WARN_CLANG_CL_INCONSISTENT_MISSING_DESTRUCTOR_OVERRIDE -Wno-inconsistent-missing-destructor-override # 104
      CXX_WARN_CLANG_CL_FORMAT_PEDANTIC -Wno-format-pedantic # 97
      CXX_WARN_CLANG_CL_NONPORTABLE_SYSTEM_INCLUDE_PATH -Wno-nonportable-system-include-path # 95
      CXX_WARN_CLANG_CL_UNDEF -Wno-undef # 94
      CXX_WARN_CLANG_CL_IGNORED_QUALIFIERS -Wno-ignored-qualifiers # 93
      CXX_WARN_CLANG_CL_USED_BUT_MARKED_UNUSED -Wno-used-but-marked-unused # 83
      CXX_WARN_CLANG_CL_HEADER_HYGIENE -Wno-header-hygiene # 79
      CXX_WARN_CLANG_CL_CHAR_SUBSCRIPTS -Wno-char-subscripts # 76
      CXX_WARN_CLANG_CL_UNREACHABLE_CODE_RETURN -Wno-unreachable-code-return # 71
      CXX_WARN_CLANG_CL_UNUSED_TEMPLATE -Wno-unused-template # 66
      CXX_WARN_CLANG_CL_GNU_ANONYMOUS_STRUCT -Wno-gnu-anonymous-struct # 63
      CXX_WARN_CLANG_CL_DEPRECATED_COPY_WITH_USER_PROVIDED_DTOR -Wno-deprecated-copy-with-user-provided-dtor # 62
      CXX_WARN_CLANG_CL_INCONSISTENT_MISSING_OVERRIDE -Wno-inconsistent-missing-override # 54
      CXX_WARN_CLANG_CL_UNREACHABLE_CODE -Wno-unreachable-code # 52
      CXX_WARN_CLANG_CL_DEPRECATED_DYNAMIC_EXCEPTION_SPEC -Wno-deprecated-dynamic-exception-spec # 51
      CXX_WARN_CLANG_CL_BAD_FUNCTION_CAST -Wno-bad-function-cast # 50
      CXX_WARN_CLANG_CL_MICROSOFT_ENUM_VALUE -Wno-microsoft-enum-value # 47
      CXX_WARN_CLANG_CL_DEPRECATED_COPY_WITH_USER_PROVIDED_COPY -Wno-deprecated-copy-with-user-provided-copy # 41
      CXX_WARN_CLANG_CL_ZERO_LENGTH_ARRAY -Wno-zero-length-array # 39
      CXX_WARN_CLANG_CL_UNUSED_FUNCTION -Wno-unused-function # 38
      CXX_WARN_CLANG_CL_PEDANTIC -Wno-pedantic # 38
      CXX_WARN_CLANG_CL_DEPRECATED_COPY_WITH_DTOR -Wno-deprecated-copy-with-dtor # 37
      CXX_WARN_CLANG_CL_DOCUMENTATION_UNKNOWN_COMMAND -Wno-documentation-unknown-command # 34
      CXX_WARN_CLANG_CL_UNDEFINED_REINTERPRET_CAST -Wno-undefined-reinterpret-cast # 33
      CXX_WARN_CLANG_CL_FORMAT_NONLITERAL -Wno-format-nonliteral # 29
      CXX_WARN_CLANG_CL_COMMA -Wno-comma # 27
      CXX_WARN_CLANG_CL_DOCUMENTATION_DEPRECATED_SYNC -Wno-documentation-deprecated-sync # 26
      CXX_WARN_CLANG_CL_SHIFT_SIGN_OVERFLOW -Wno-shift-sign-overflow # 24
      CXX_WARN_CLANG_CL_PRE_C++17_COMPAT_PEDANTIC -Wno-pre-c++17-compat-pedantic # 24
      CXX_WARN_CLANG_CL_C++98_COMPAT_UNNAMED_TYPE_TEMPLATE_ARGS -Wno-c++98-compat-unnamed-type-template-args # 22
      CXX_WARN_CLANG_CL_SIGN_COMPARE -Wno-sign-compare # 21
      CXX_WARN_CLANG_CL_FORMAT -Wno-format # 21
      CXX_WARN_CLANG_CL_C++98_COMPAT_BIND_TO_TEMPORARY_COPY -Wno-c++98-compat-bind-to-temporary-copy # 21
      CXX_WARN_CLANG_CL_ENUM_ENUM_CONVERSION -Wno-enum-enum-conversion # 20
      CXX_WARN_CLANG_CL_ANON_ENUM_ENUM_CONVERSION -Wno-anon-enum-enum-conversion # 14
      CXX_WARN_CLANG_CL_RANGE_LOOP_BIND_REFERENCE -Wno-range-loop-bind-reference # 14
      CXX_WARN_CLANG_CL_ENUM_FLOAT_CONVERSION -Wno-enum-float-conversion # 12
      CXX_WARN_CLANG_CL_KEYWORD_MACRO -Wno-keyword-macro # 10
      CXX_WARN_CLANG_CL_DEPRECATED_COPY -Wno-deprecated-copy # 10
      CXX_WARN_CLANG_CL_UNUSED_MEMBER_FUNCTION -Wno-unused-member-function # 9
      CXX_WARN_CLANG_CL_MISSING_NORETURN -Wno-missing-noreturn # 8
      CXX_WARN_CLANG_CL_MISSING_VARIABLE_DECLARATIONS -Wno-missing-variable-declarations # 8
      CXX_WARN_CLANG_CL_DOCUMENTATION_HTML -Wno-documentation-html # 6
      CXX_WARN_CLANG_CL_GNU_REDECLARED_ENUM -Wno-gnu-redeclared-enum # 6
      CXX_WARN_CLANG_CL_DEPRECATED_DECLARATIONS -Wno-deprecated-declarations # 6
      CXX_WARN_CLANG_CL_OVERLOADED_VIRTUAL -Wno-overloaded-virtual # 5
      CXX_WARN_CLANG_CL_C++98_C++11_COMPAT_BINARY_LITERAL -Wno-c++98-c++11-compat-binary-literal # 4
      CXX_WARN_CLANG_CL_DEPRECATED_REDUNDANT_CONSTEXPR_STATIC_DEF -Wno-deprecated-redundant-constexpr-static-def # 4
      CXX_WARN_CLANG_CL_MISSING_BRACES -Wno-missing-braces # 4
      CXX_WARN_CLANG_CL_C99_EXTENSIONS -Wno-c99-extensions # 4
      CXX_WARN_CLANG_CL_STRICT_PROTOTYPES -Wno-strict-prototypes # 4
      CXX_WARN_CLANG_CL_UNREACHABLE_CODE_LOOP_INCREMENT -Wno-unreachable-code-loop-increment # 4
      CXX_WARN_CLANG_CL_GNU_CASE_RANGE -Wno-gnu-case-range # 4
      CXX_WARN_CLANG_CL_DUPLICATE_ENUM -Wno-duplicate-enum # 3
      CXX_WARN_CLANG_CL_NULL_POINTER_SUBTRACTION -Wno-null-pointer-subtraction # 2
      CXX_WARN_CLANG_CL_DEPRECATED_LITERAL_OPERATOR -Wno-deprecated-literal-operator # 2
      CXX_WARN_CLANG_CL_NEWLINE_EOF -Wno-newline-eof # 2
      CXX_WARN_CLANG_CL_MICROSOFT_CAST -Wno-microsoft-cast # 2
      CXX_WARN_CLANG_CL_DATE_TIME -Wno-date-time # 2
      CXX_WARN_CLANG_CL_DELETE_NON_ABSTRACT_NON_VIRTUAL_DTOR -Wno-delete-non-abstract-non-virtual-dtor # 2
      CXX_WARN_CLANG_CL_UNUSED_PRIVATE_FIELD -Wno-unused-private-field # 2
      CXX_WARN_CLANG_CL_FLEXIBLE_ARRAY_EXTENSIONS -Wno-flexible-array-extensions # 2
      CXX_WARN_CLANG_CL_STRING_CONVERSION -Wno-string-conversion # 2
      CXX_WARN_CLANG_CL_FINAL_DTOR_NON_FINAL_CLASS -Wno-final-dtor-non-final-class # 2
      CXX_WARN_CLANG_CL_MICROSOFT_UNQUALIFIED_FRIEND -Wno-microsoft-unqualified-friend # 2
      CXX_WARN_CLANG_CL_INVALID_NORETURN -Wno-invalid-noreturn # 1
      CXX_WARN_CLANG_CL_INVALID_UTF8 -Wno-invalid-utf8 # 1
      CXX_WARN_CLANG_CL_FOUR_CHAR_CONSTANTS -Wno-four-char-constants # 1
      CXX_WARN_CLANG_CL_PARENTHESES -Wno-parentheses # 1
      CXX_WARN_CLANG_CL_PESSIMIZING_MOVE -Wno-pessimizing-move # 1
      CXX_WARN_CLANG_CL_DEPRECATED_NON_PROTOTYPE -Wno-deprecated-non-prototype # 1
      CXX_WARN_CLANG_CL_BITFIELD_ENUM_CONVERSION -Wno-bitfield-enum-conversion # 1
      CXX_WARN_CLANG_CL_UNUSED_LAMBDA_CAPTURE -Wno-unused-lambda-capture # 1
      CXX_WARN_CLANG_CL_SHADOW_FIELD_IN_CONSTRUCTOR_MODIFIED -Wno-shadow-field-in-constructor-modified # 1
      # And some additional ones that came up when using LLVM 18.1.8 on Windows ARM64
      CXX_WARN_CLANG_CL_SWITCH_DEFAULT -Wno-switch-default
      CXX_WARN_CLANG_CL_NAN_INFINITY_DISABLED -Wno-nan-infinity-disabled
      # And another from 19.1.5
      CXX_WARN_CLANG_CL_PRE_C11_COMPAT -Wno-pre-c11-compat
    )
  endif()


  # ---------------------
  # Suppress Strict Flags

  # flags to undo strict flags

  add_check_c_compiler_flags(
    C_REMOVE_STRICT_FLAGS

    C_WARN_NO_UNUSED_PARAMETER -Wno-unused-parameter
    C_WARN_NO_UNUSED_VARIABLE -Wno-unused-variable
    C_WARN_NO_UNUSED_MACROS -Wno-unused-macros
    C_WARN_NO_MISLEADING_INDENTATION -Wno-misleading-indentation

    C_WARN_NO_MISSING_VARIABLE_DECLARATIONS -Wno-missing-variable-declarations
    C_WARN_NO_INCOMPAT_PTR_DISCARD_QUAL -Wno-incompatible-pointer-types-discards-qualifiers
    C_WARN_NO_UNUSED_FUNCTION -Wno-unused-function
    C_WARN_NO_INT_TO_VOID_POINTER_CAST -Wno-int-to-void-pointer-cast
    C_WARN_NO_MISSING_PROTOTYPES -Wno-missing-prototypes
    C_WARN_NO_DUPLICATE_ENUM -Wno-duplicate-enum
    C_WARN_NO_UNDEF -Wno-undef
    C_WARN_NO_MISSING_NORETURN -Wno-missing-noreturn
    C_WARN_NO_UNUSED_BUT_SET_VARIABLE -Wno-unused-but-set-variable
    C_WARN_NO_DEPRECATED_DECLARATIONS -Wno-deprecated-declarations
    C_WARN_NO_STRICT_PROTOTYPES -Wno-strict-prototypes
    C_WARN_NO_BITWISE_INSTEAD_OF_LOGICAL -Wno-bitwise-instead-of-logical
    C_WARN_NO_IMPLICIT_CONST_INT_FLOAT_CONVERSION -Wno-implicit-const-int-float-conversion
    C_WARN_NO_SINGLE_BIT_BITFIELD_CONSTANT_CONVERSION -Wno-single-bit-bitfield-constant-conversion
  )

  add_check_cxx_compiler_flags(
    CXX_REMOVE_STRICT_FLAGS

    CXX_WARN_NO_NONTRIVIAL_MEMCALL -Wno-nontrivial-memcall
    CXX_WARN_NO_UNUSED_PARAMETER -Wno-unused-parameter
    CXX_WARN_NO_UNUSED_PRIVATE_FIELD -Wno-unused-private-field
    CXX_WARN_NO_CXX11_NARROWING -Wno-c++11-narrowing
    CXX_WARN_NO_NON_VIRTUAL_DTOR -Wno-non-virtual-dtor
    CXX_WARN_NO_UNUSED_MACROS -Wno-unused-macros
    CXX_WARN_NO_UNUSED_VARIABLE -Wno-unused-variable
    CXX_WARN_NO_REORDER -Wno-reorder
    CXX_WARN_NO_COMMENT -Wno-comment
    CXX_WARN_NO_UNUSED_TYPEDEFS -Wno-unused-local-typedefs
    CXX_WARN_NO_UNDEFINED_VAR_TEMPLATE -Wno-undefined-var-template
    CXX_WARN_NO_INSTANTIATION_AFTER_SPECIALIZATION -Wno-instantiation-after-specialization
    CXX_WARN_NO_MISLEADING_INDENTATION -Wno-misleading-indentation
    CXX_WARN_NO_BITWISE_INSTEAD_OF_LOGICAL -Wno-bitwise-instead-of-logical
    CXX_WARN_NO_IMPLICIT_CONST_INT_FLOAT_CONVERSION -Wno-implicit-const-int-float-conversion
    CXX_WARN_NO_UNDEF -Wno-undef
    CXX_WARN_NO_UNDEF_PREFIX -Wno-undef-prefix
    CXX_WARN_NO_INCONSISTENT_MISSING_OVERRIDE -Wno-inconsistent-missing-override
  )

elseif(CMAKE_C_COMPILER_ID STREQUAL "Intel")

  add_check_c_compiler_flags(
    C_WARNINGS

    C_WARN_ALL -Wall
    C_WARN_POINTER_ARITH -Wpointer-arith
    C_WARN_NO_UNKNOWN_PRAGMAS -Wno-unknown-pragmas
  )

  add_check_cxx_compiler_flags(
    CXX_WARNINGS

    CXX_WARN_ALL -Wall
    CXX_WARN_NO_INVALID_OFFSETOF -Wno-invalid-offsetof
    CXX_WARN_NO_SIGN_COMPARE -Wno-sign-compare
  )

  # Disable numbered, false positives.
  string(APPEND C_WARNINGS " -wd188,186,144,913,556,858,597,177,1292,167,279,592,94,2722,3199")
  string(APPEND CXX_WARNINGS " -wd188,186,144,913,556,858,597,177,1292,167,279,592,94,2722,3199")
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  # Most MSVC warnings are C & C++.

  # ============================================================================
  # MSVC WARNING FLAGS - TURN WARNINGS ON
  # ============================================================================
  set(_WARNINGS_ON
    # warning level:
    "/W3"
    "/w34062"  # switch statement contains 'default' but no 'case' labels
    "/w34100"  # 'identifier' : unreferenced formal parameter
    "/w34115"  # 'type' : named type definition in parentheses
    "/w34189"  # local variable is initialized but not referenced
    # see https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/c5038?view=vs-2017
    "/w35038"  # order of initialization in c++ constructors
    # errors:
    "/we4013"  # 'function' undefined; assuming extern returning int
    "/we4133"  # incompatible pointer types
    "/we4431"  # missing type specifier - int assumed
    "/we4033"  # 'function' must return a value
  )

  # ============================================================================
  # MSVC WARNING FLAGS - TURN WARNINGS OFF
  # ============================================================================
  set(_WARNINGS_OFF
    # disable:
    "/wd4018"  # signed/unsigned mismatch
    "/wd4146"  # unary minus operator applied to unsigned type, result still unsigned
    "/wd4065"  # switch statement contains 'default' but no 'case' labels
    "/wd4127"  # conditional expression is constant
    "/wd4181"  # qualifier applied to reference type; ignored
    "/wd4200"  # zero-sized array in struct/union
    "/wd4244"  # conversion from 'type1' to 'type2', possible loss of data
    "/wd4267"  # conversion from 'size_t' to 'type', possible loss of data
    "/wd4305"  # truncation from 'type1' to 'type2'
    "/wd4800"  # forcing value to bool 'true' or 'false'
    "/wd4828"  # The file contains a character that is illegal
    "/wd4996"  # identifier was declared deprecated
    "/wd4661"  # no suitable definition provided for explicit template instantiation request
    "/wd4848"  # 'no_unique_address' is a vendor extension in C++17
  )

  # Combine both categories
  list(APPEND _WARNINGS_ON ${_WARNINGS_OFF})
  string(REPLACE ";" " " _WARNINGS "${_WARNINGS_ON}")
  set(C_WARNINGS "${_WARNINGS}")
  set(CXX_WARNINGS "${_WARNINGS}")
  unset(_WARNINGS)
  unset(_WARNINGS_ON)
  unset(_WARNINGS_OFF)
endif()

# Select C++17 as the standard for C++ projects.
set(CMAKE_CXX_STANDARD 17)
# If C++17 is not available, downgrading to an earlier standard is NOT OK.
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Do not enable compiler specific language extensions.
set(CMAKE_CXX_EXTENSIONS OFF)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Visual Studio has all standards it supports available by default
# Clang on windows copies this behavior and does not support these switches
if(CMAKE_COMPILER_IS_GNUCC OR
   (CMAKE_C_COMPILER_ID MATCHES "Clang" AND (NOT MSVC)) OR
   (CMAKE_C_COMPILER_ID STREQUAL "Intel"))

  # Use C11 + GNU extensions, works with GCC, Clang, ICC
  string(APPEND CMAKE_C_FLAGS " -std=gnu11")
endif()

# Include warnings first, so its possible to disable them with user defined flags
# eg: -Wno-uninitialized
set(CMAKE_C_FLAGS "${C_WARNINGS} ${CMAKE_C_FLAGS} ${PLATFORM_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CXX_WARNINGS} ${CMAKE_CXX_FLAGS} ${PLATFORM_CFLAGS}")

# -----------------------------------------------------------------------------
# Find and check

# Find required packages
find_package(PkgConfig REQUIRED)

# Detect YACC/LEX tools (used in multiple parts of the codebase)
motif_detect_yacc_lex()

# Find X11 and related libraries
pkg_check_modules(X REQUIRED x11 xpm xt xmu xext)

# Check for XTHREADS support
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)

# Test if XtAppLock and XtAppUnlock are available (indicates XTHREADS support)
# We need to link with Xt library to properly test these functions
set(CMAKE_REQUIRED_LIBRARIES ${X_LIBRARIES})
set(CMAKE_REQUIRED_INCLUDES ${X_INCLUDE_DIRS})
set(CMAKE_REQUIRED_FLAGS ${X_CFLAGS})

check_c_source_compiles("
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
int main() {
    XtAppContext app_context;
    XtAppLock(app_context);
    XtAppUnlock(app_context);
    return 0;
}" HAVE_XTHREADS)

set(CMAKE_REQUIRED_LIBRARIES)
set(CMAKE_REQUIRED_INCLUDES)
set(CMAKE_REQUIRED_FLAGS)

if(HAVE_XTHREADS)
    add_compile_definitions(XTHREADS=1)
    message(STATUS "XTHREADS support detected")
else()
    message(STATUS "No XTHREADS support detected")
endif()

# Always define X11 compatibility macros for all targets
add_compile_definitions(
    _Xos_processLock=XtProcessLock
    _Xos_processUnlock=XtProcessUnlock
)

# Check for Xft
if(WITH_XFT)
    pkg_check_modules(XFT xft>=2)
    if(XFT_FOUND)
        add_compile_definitions(USE_XFT)
    endif()
endif()

# Check for FontConfig support (required by Xft)
pkg_check_modules(FONTCONFIG REQUIRED fontconfig)

# Check for JPEG support
if(WITH_JPEG)
    pkg_check_modules(JPEG libjpeg)
    if(JPEG_FOUND)
        set(XM_WITH_JPEG "#define XM_WITH_JPEG 1")
        add_compile_definitions(XM_WITH_JPEG)
    else()
        set(XM_WITH_JPEG "#define XM_WITH_JPEG 0")
    endif()
else()
    set(XM_WITH_JPEG "#define XM_WITH_JPEG 0")
endif()

# Check for PNG support
if(WITH_PNG)
    pkg_check_modules(PNG libpng)
    if(PNG_FOUND)
        set(XM_WITH_PNG "#define XM_WITH_PNG 1")
        add_compile_definitions(XM_WITH_PNG)
    else()
        set(XM_WITH_PNG "#define XM_WITH_PNG 0")
    endif()
else()
    set(XM_WITH_PNG "#define XM_WITH_PNG 0")
endif()

# Check for printing support
if(WITH_PRINTING)
    pkg_check_modules(XP xp)
    if(XP_FOUND)
        set(XM_PRINTING "#define XM_PRINTING 1")
        add_compile_definitions(XM_PRINTING)
    else()
        message(FATAL_ERROR "libXp not found for printing support")
    endif()
else()
    set(XM_PRINTING "#define XM_PRINTING 0")
endif()

# Check for UTF-8 support
if(WITH_UTF8)
    set(XM_UTF8 "#define XM_UTF8 1")
    add_compile_definitions(XM_UTF8)
    find_library(ICONV_LIBRARY iconv)
    if(ICONV_LIBRARY)
        set(ICONV_LIBRARIES ${ICONV_LIBRARY})
    endif()
else()
    set(XM_UTF8 "#define XM_UTF8 0")
endif()

# Check for message catalog support
if(WITH_MESSAGE_CATALOG)
    add_compile_definitions(XM_MSGCAT=1)
else()
    # Define XM_MSGCAT as 0 when message catalog is disabled to avoid warnings
    add_compile_definitions(XM_MSGCAT=0)
endif()

# Check for UIL debug support
if(WITH_UIL_DEBUG)
    add_compile_definitions(XM_UIL_DEBUG)
endif()

# Set logging level
if(LOG_LEVEL STREQUAL "DEBUG")
    set(XM_DEFAULT_LOG_LEVEL_VAL "XM_LOG_LEVEL_DBG")
elseif(LOG_LEVEL STREQUAL "INFO")
    set(XM_DEFAULT_LOG_LEVEL_VAL "XM_LOG_LEVEL_INFO")
elseif(LOG_LEVEL STREQUAL "WARN")
    set(XM_DEFAULT_LOG_LEVEL_VAL "XM_LOG_LEVEL_WARN")
elseif(LOG_LEVEL STREQUAL "ERROR")
    set(XM_DEFAULT_LOG_LEVEL_VAL "XM_LOG_LEVEL_ERR")
elseif(LOG_LEVEL STREQUAL "CRITICAL")
    set(XM_DEFAULT_LOG_LEVEL_VAL "XM_LOG_LEVEL_CRITICAL")
else()
    set(XM_DEFAULT_LOG_LEVEL_VAL "XM_LOG_LEVEL_INFO")
endif()

add_compile_definitions(
    XM_DEFAULT_LOG_LEVEL=${XM_DEFAULT_LOG_LEVEL_VAL}
    XM_DEFAULT_LOG_OUTPUT="${LOG_OUTPUT}"
)

# Additional defines
add_compile_definitions(
    NO_OL_COMPAT=1
    HAVE_SYS_POLL_H
    HAVE_SYS_TIME_H
)

# Include directories
include_directories(include)
include_directories(${X_INCLUDE_DIRS})

# Add system includes header to all targets
add_compile_definitions(MOTIF_SYSTEM_H="${CMAKE_SOURCE_DIR}/include/motif_system.h")

# Add conditional include directories
if(XFT_FOUND)
    include_directories(${XFT_INCLUDE_DIRS})
endif()

if(JPEG_FOUND)
    include_directories(${JPEG_INCLUDE_DIRS})
endif()

if(PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIRS})
endif()

if(XP_FOUND)
    include_directories(${XP_INCLUDE_DIRS})
endif()

# Copy headers to build directory for compilation
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/Xm)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/Mrm)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/Dt)

# Copy Xm headers (excluding symlinks)
file(GLOB XM_HEADER_FILES "${CMAKE_SOURCE_DIR}/src/lib/Xm/*.h")
foreach(header ${XM_HEADER_FILES})
    get_filename_component(filename ${header} NAME)
    # Skip symlinks to avoid circular dependencies
    if(NOT IS_SYMLINK ${header})
        configure_file(${header} ${CMAKE_BINARY_DIR}/include/Xm/${filename} COPYONLY)
    endif()
endforeach()

# Copy Mrm headers
file(GLOB MRM_HEADER_FILES "${CMAKE_SOURCE_DIR}/src/lib/Mrm/*.h")
foreach(header ${MRM_HEADER_FILES})
    get_filename_component(filename ${header} NAME)
    configure_file(${header} ${CMAKE_BINARY_DIR}/include/Mrm/${filename} COPYONLY)
endforeach()

# Copy Dt headers if they exist
if(EXISTS "${CMAKE_SOURCE_DIR}/include/Dt")
    file(GLOB DT_HEADER_FILES "${CMAKE_SOURCE_DIR}/include/Dt/*.h")
    foreach(header ${DT_HEADER_FILES})
        get_filename_component(filename ${header} NAME)
        configure_file(${header} ${CMAKE_BINARY_DIR}/include/Dt/${filename} COPYONLY)
    endforeach()
endif()

# Add subdirectories
add_subdirectory(include)
add_subdirectory(src/lib)
add_subdirectory(tools)
add_subdirectory(src/bin)
add_subdirectory(data)

if(WITH_DEMOS)
    add_subdirectory(src/examples)
endif()

if(WITH_TESTS)
    add_subdirectory(src/tests)
endif()

if(WITH_DOCS)
    add_subdirectory(doc)
endif()

# Create pkg-config file
configure_file(
    ${CMAKE_SOURCE_DIR}/data/motif.pc.in
    ${CMAKE_BINARY_DIR}/motif.pc
    @ONLY
)

# Install pkg-config file
install(FILES ${CMAKE_BINARY_DIR}/motif.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# Install README
install(FILES README.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/Xm/doc
)

# Print configuration summary
# message(STATUS "")
# message(STATUS "------------------------------------------------------------------------")
# message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION}")
# message(STATUS "------------------------------------------------------------------------")
# message(STATUS "")
# message(STATUS "System configuration:")
# message(STATUS "  os.............: ${CMAKE_SYSTEM_NAME}")
# message(STATUS "  c compiler.....: ${CMAKE_C_COMPILER}")
# message(STATUS "  c standard.....: C${CMAKE_C_STANDARD}")
# message(STATUS "")
# message(STATUS "Features:")
# message(STATUS "  shared libs....: ${WITH_SHARED_LIBS}")
# message(STATUS "  static libs....: ${WITH_STATIC_LIBS}")
# message(STATUS "  demos..........: ${WITH_DEMOS}")
# message(STATUS "  tests..........: ${WITH_TESTS}")
# message(STATUS "  debug..........: ${WITH_DEBUG}")
# message(STATUS "  utf8...........: ${WITH_UTF8}")
# message(STATUS "  jpeg...........: ${WITH_JPEG}")
# message(STATUS "  png............: ${WITH_PNG}")
# message(STATUS "  xft............: ${WITH_XFT}")
# message(STATUS "  printing.......: ${WITH_PRINTING}")
# message(STATUS "  msg catalog....: ${WITH_MESSAGE_CATALOG}")
# message(STATUS "  uil debug......: ${WITH_UIL_DEBUG}")
# message(STATUS "")
# message(STATUS "Compilation......: make")
# message(STATUS "  CPPFLAGS.......: ${CMAKE_C_FLAGS}")
# message(STATUS "  CFLAGS.........: ${CMAKE_C_FLAGS}")
# message(STATUS "   X_CFLAGS......: ${X_CFLAGS}")
# if(XFT_FOUND)
#     message(STATUS "   XFT_CFLAGS....: ${XFT_CFLAGS}")
# endif()
# message(STATUS "  LDFLAGS........: ${CMAKE_EXE_LINKER_FLAGS}")
# message(STATUS "   X_LIBS........: ${X_LIBRARIES}")
# if(XFT_FOUND)
#     message(STATUS "   XFT_LIBS......: ${XFT_LIBRARIES}")
# endif()
# message(STATUS "")

if(NOT MOTIF_VENDOR_INFO)
    set(MOTIF_VENDOR_INFO "github.com/dimmus")
#    git_vendor(MOTIF_VENDOR_INFO) # TODO
endif()

if(NOT MOTIF_REVISION)
  git_describe(MOTIF_REVISION_GIT)
  set(MOTIF_REVISION "motif-${MOTIF_VERSION}.${MOTIF_VERSION_PATCH}.${MOTIF_REVISION_GIT}-${MOTIF_VERSION_CYCLE}")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_64 TRUE)
else()
  set(ARCH_64 FALSE)
endif()

# -----------------------------------------------------------------------------
# Print Final Configuration

if(FIRST_RUN)
    motif_print_summary()
endif()

if(0)
  print_all_vars()
endif()

# Should be the last step of configuration.
if(POSTCONFIGURE_SCRIPT)
  include(${POSTCONFIGURE_SCRIPT})
endif()