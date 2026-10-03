cmake_minimum_required(VERSION 3.16)

project(Motif VERSION 2.4.1 LANGUAGES C)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT WITH_IN_SOURCE_BUILD)
  message(FATAL_ERROR
    "In-source builds are not supported: configure Motif in a separate build "
    "directory, e.g. 'cmake -S . -B _build'.  First remove the CMakeCache.txt "
    "and CMakeFiles that this run created in the source directory.  "
    "Set WITH_IN_SOURCE_BUILD=ON to override (not recommended).")
endif()

include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckCSourceCompiles)
include(CheckCCompilerFlag)
include(CMakeDependentOption)

include(tools/cmake/motif_helpers.cmake)

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

# Install to /usr by default (instead of CMake's /usr/local) so that consumers
# such as CDE find libXm without extra -L/-I flags or ld.so configuration.
# Must be set before GNUInstallDirs so it derives /etc and /var for this prefix.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install path prefix" FORCE)
endif()

# Set install directory variables
include(GNUInstallDirs)

enable_testing()

# -----------------------------------------------------------------------------
# Test Compiler Support

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_C_COMPILER_VERSION VERSION_LESS "11.0")
    message(FATAL_ERROR
      "The minimum supported version of GCC is 11.0, found ${CMAKE_C_COMPILER_VERSION}")
  endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_C_COMPILER_VERSION VERSION_LESS "8.0")
    message(FATAL_ERROR
      "The minimum supported version of Clang is 8.0, found ${CMAKE_C_COMPILER_VERSION}")
  endif()
endif()

# -----------------------------------------------------------------------------
# Set global definitions

# Version information
set(XM_MAJOR ${PROJECT_VERSION_MAJOR})
set(XM_MINOR ${PROJECT_VERSION_MINOR})
set(XM_PATCH ${PROJECT_VERSION_PATCH})
set(XM_VERSION "${PROJECT_VERSION}")

# ABI version for shared libraries
set(ABI_CURRENT 5)
set(ABI_REVISION 0)
set(ABI_AGE 0)
set(ABI_VERSION "${ABI_CURRENT}:${ABI_REVISION}:${ABI_AGE}")


# -----------------------------------------------------------------------------
# Declare Options

motif_option(WITH_SHARED_LIBS "Build shared libraries" ON)
motif_option(WITH_STATIC_LIBS "Build static libraries" OFF)
motif_option(WITH_DEMOS "Build examples" ON)
motif_option(WITH_TESTS "Build tests" OFF)
motif_option(WITH_DEBUG "Enable debug build" OFF)
motif_option(WITH_UTF8 "Enable UTF-8 support" ON)
motif_option(WITH_MESSAGE_CATALOG "Enable X/Open message catalog support" OFF)
motif_option(WITH_PRINTING "Enable printing support" OFF)
motif_option(WITH_UIL_DEBUG "Enable UIL compiler debugging support" OFF)
motif_option(WITH_DOCS "Enable documentation installation" ON)
motif_option(WITH_WML_TOOLS "Enable WML (Widget Meta-Language) tools" ON)
motif_option(WITH_JPEG "Enable JPEG support" ON)
motif_option(WITH_PNG "Enable PNG support" ON)
motif_option(WITH_XFT "Enable Xft support" ON)
motif_option(WITH_LTO "Enable link-time optimization (and -fno-semantic-interposition)" OFF)
motif_option(WITH_CPU_NATIVE "Optimize for the build machine's CPU (-march=native, binaries are not portable)" OFF)

# Library type of the public libraries (Xm, Mrm). Internal helper libraries
# stay static, built as PIC so they can be linked into the shared ones.
if(WITH_SHARED_LIBS)
  set(MOTIF_LIB_TYPE SHARED)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else()
  set(MOTIF_LIB_TYPE STATIC)
endif()

# These were computed but never applied to the build; see the sanitizer
# section below.
motif_option(WITH_COMPILER_CODE_COVERAGE
  "Build and link with code coverage instrumentation" OFF)
mark_as_advanced(WITH_COMPILER_CODE_COVERAGE)
motif_option(WITH_COMPILER_ASAN
  "Build and link with the address sanitizer" OFF)
mark_as_advanced(WITH_COMPILER_ASAN)

option(WITH_COMPILER_CCACHE "Use ccache to improve rebuild times" OFF)
mark_as_advanced(WITH_COMPILER_CCACHE)

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
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)

check_function_exists(getcwd HAVE_GETCWD)
check_function_exists(getwd HAVE_GETWD)
check_function_exists(iconv HAVE_ICONV)
check_function_exists(nanosleep HAVE_NANOSLEEP)
check_function_exists(putenv HAVE_PUTENV)
check_function_exists(regcomp HAVE_REGCOMP)
check_function_exists(regexec HAVE_REGEX)
if(NOT HAVE_REGEX)
    set(HAVE_REGEX 0)
endif()
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(strlcat HAVE_STRLCAT)
check_function_exists(getpwnam HAVE_GETPWNAM)
check_function_exists(getpwuid HAVE_GETPWUID)
check_function_exists(re_comp HAVE_RE_COMP)
check_function_exists(re_exec HAVE_RE_EXEC)

# Check for X11-specific functions
include(CheckSymbolExists)
find_package(X11 REQUIRED)
check_symbol_exists(XICProc "X11/Xlib.h" HAVE_XICPROC)
if(NOT HAVE_XICPROC)
    set(HAVE_XICPROC 0)
endif()
# Always define HAVE_XICPROC to avoid -Wundef warnings
if(NOT DEFINED HAVE_XICPROC)
    set(HAVE_XICPROC 0)
endif()

# Check if compiler supports ANSI C (__STDC__)
# Modern compilers should define __STDC__ by default
check_c_source_compiles("
#ifdef __STDC__
int main() { return 0; }
#else
#error __STDC__ not defined
#endif
" HAVE_STDC)

if(HAVE_STDC)
    set(__STDC__ 1)
    message(STATUS "Compiler supports ANSI C (__STDC__ defined)")
else()
    set(__STDC__ 0)
    message(STATUS "Compiler does not support ANSI C (__STDC__ not defined)")
endif()

# Check for Xmu library
find_library(XMU_LIBRARY Xmu)
if(XMU_LIBRARY)
    set(HAVE_LIBXMU TRUE)
else()
    set(HAVE_LIBXMU FALSE)
endif()

# PWC_DEBUG is a project-specific debug flag
option(WITH_PWC_DEBUG "Enable PWC debug output" OFF)
if(WITH_PWC_DEBUG)
    set(HAVE_PWC_DEBUG TRUE)
else()
    set(HAVE_PWC_DEBUG FALSE)
endif()

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

# Add definitions for new checks
if(HAVE_STDINT_H)
    add_compile_definitions(HAVE_STDINT_H)
endif()
if(HAVE_INTTYPES_H)
    add_compile_definitions(HAVE_INTTYPES_H)
endif()
if(HAVE_XICPROC)
    add_compile_definitions(HAVE_XICPROC=1)
else()
    add_compile_definitions(HAVE_XICPROC=0)
endif()
if(HAVE_LIBXMU)
    add_compile_definitions(HAVE_LIBXMU)
endif()
if(HAVE_PWC_DEBUG)
    add_compile_definitions(PWC_DEBUG=1)
else()
    add_compile_definitions(PWC_DEBUG=0)
endif()

# Compiler flags
if(WITH_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -ggdb -DDEBUG")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -DNDEBUG")
endif()

# Optimization options. These are applied here rather than in the presets in
# tools/cmake/config, because presets run before the compiler is detected.
if(WITH_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT _ipo_supported OUTPUT _ipo_output LANGUAGES C)
  if(_ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  else()
    message(WARNING "WITH_LTO: link-time optimization not supported: ${_ipo_output}")
  endif()
  unset(_ipo_supported)
  unset(_ipo_output)

  # Exported functions in a -fPIC library are otherwise assumed to be
  # interposable, which blocks inlining calls between them inside libXm.
  if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fno-semantic-interposition)
  endif()
endif()

if(WITH_CPU_NATIVE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-march=native)
endif()


# -----------------------------------------------------------------------------
# Warnings

set(C_WARNINGS)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  motif_check_c_flags(C_WARNINGS
    -Wall
    -Werror=implicit-function-declaration
    -Werror=return-type
    -Werror=vla
    -Wpointer-arith
    -Wwrite-strings
    -Wlogical-op
    -Wundef
    -Winit-self
    -Wmissing-include-dirs
    -Wformat-signedness
    -Wrestrict
    -Wnonnull
    -Wabsolute-value
    -Wuninitialized
    -Wshadow
    -Wno-char-subscripts
    -Wno-unknown-pragmas
    -Wno-div-by-zero
    -Wno-stringop-overread
    -Wno-stringop-overflow
    -Wno-unused-function
    -Wno-unused-variable
    -Wno-unused-but-set-variable
    -Wno-unused-value
    -Wno-unused-parameter
    -Wno-discarded-qualifiers
    -Wno-missing-prototypes
    -Wno-redundant-decls
    -Wno-strict-prototypes
    -Wno-implicit-fallthrough
    -Wno-misleading-indentation
    -Wno-type-limits
    -Wno-tautological-compare
  )
endif()
string(REPLACE ";" " " C_WARNINGS "${C_WARNINGS}")

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use C11 + GNU extensions
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  string(APPEND CMAKE_C_FLAGS " -std=gnu11")
endif()

# Include warnings first, so its possible to disable them with user defined flags
# eg: -Wno-uninitialized
set(CMAKE_C_FLAGS "${C_WARNINGS} ${CMAKE_C_FLAGS}")


# -----------------------------------------------------------------------------
# Find and check

# Find required packages
find_package(PkgConfig REQUIRED)

# Detect YACC/LEX tools (used in multiple parts of the codebase)
motif_find_parser_generators()

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
        message(STATUS "Printing support enabled (libXp found)")
    else()
        message(WARNING "libXp not found - disabling printing support")
        set(WITH_PRINTING OFF CACHE BOOL "Enable printing support" FORCE)
        set(XM_PRINTING "#define XM_PRINTING 0")
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
    # Compiles the generated numeric catalogs to .cat files (optional: the
    # libraries only need the generated *MsgCatI.h headers to build).
    find_program(GENCAT_EXECUTABLE gencat)
    if(NOT GENCAT_EXECUTABLE)
        message(STATUS "gencat not found: message catalogs (.cat) will not be compiled")
    endif()
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

# Copy Xm headers (excluding symlinks). XmMsgCatI.h is left out: the
# xm_msgcat_header target writes it (generated, or the stub without message
# catalogs), and copying the stub here on every reconfigure would overwrite
# the generated header.
file(GLOB XM_HEADER_FILES "${CMAKE_SOURCE_DIR}/src/lib/Xm/*.h")
list(REMOVE_ITEM XM_HEADER_FILES "${CMAKE_SOURCE_DIR}/src/lib/Xm/XmMsgCatI.h")
foreach(header ${XM_HEADER_FILES})
    get_filename_component(filename ${header} NAME)
    # Skip symlinks to avoid circular dependencies
    if(NOT IS_SYMLINK ${header})
        configure_file(${header} ${CMAKE_BINARY_DIR}/include/Xm/${filename} COPYONLY)
    endif()
endforeach()

# Copy generated version.h to include directory (as a custom command)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/include/Xm/version.h
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/Xm
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/src/lib/Xm/version.h
        ${CMAKE_BINARY_DIR}/include/Xm/version.h
    DEPENDS ${CMAKE_BINARY_DIR}/src/lib/Xm/version.h
    COMMENT "Copying version.h to include directory"
)

# Create a target that depends on the version header copy
add_custom_target(copy_version_header ALL
    DEPENDS ${CMAKE_BINARY_DIR}/include/Xm/version.h
)

# Copy Mrm headers (MrmMsgCatI.h is written by mrm_msgcat_header, as for Xm)
file(GLOB MRM_HEADER_FILES "${CMAKE_SOURCE_DIR}/src/lib/Mrm/*.h")
list(REMOVE_ITEM MRM_HEADER_FILES "${CMAKE_SOURCE_DIR}/src/lib/Mrm/MrmMsgCatI.h")
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
# message(STATUS "  examples.......: ${WITH_DEMOS}")
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

# -----------------------------------------------------------------------------
# Print Final Configuration

motif_print_summary()
