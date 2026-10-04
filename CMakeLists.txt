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

# Default to an optimised build with debug information.  Use the standard
# CMAKE_C_FLAGS_<CONFIG>: Debug keeps assert() (no NDEBUG), the others define
# NDEBUG.
get_property(_motif_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT _motif_multi_config AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Build type (Debug, Release, RelWithDebInfo or MinSizeRel)" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release RelWithDebInfo MinSizeRel)
endif()

# Standard installation directories (lib or lib64, etc.) below
# CMAKE_INSTALL_PREFIX, which keeps CMake's default of /usr/local; packagers
# and system installs pass -DCMAKE_INSTALL_PREFIX=/usr.
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

# Shared library versions of libXm, libMrm and libUil.
#
# Upstream Motif 2.3 ships libXm.so.4, libMrm.so.4 and libUil.so.4 (2.3.8:
# libXm.so.4.0.4).  This tree is NOT binary compatible with them, so it must
# not reuse SONAME 4.  abidiff of an upstream 2.3.8 build (default configure,
# which defines OM22_COMPATIBILITY in the installed Xm.h) against this one
# shows:
#
#   - XmPrimitivePart and XmGadgetPart (PrimitiveP.h, GadgetP.h) no longer
#     have the OM22_COMPATIBILITY member tool_tip_string, so the instance
#     records of every widget and gadget, and the part offsets of every
#     subclass compiled against the 2.3.8 headers, are 8 bytes off;
#   - the string tables _XmStrings and _XmStrings22 were re-ordered (entries
#     removed and inserted), so 1624 of the 1692 XmC* / XmR* / XmS* macros
#     that a 2.3.8 program compiled in as offsets into them
#     ("(char*)&_XmStrings[N]") now name a different string;
#   - _XmEditResCheckMessages, XmDataFielddf_ClearSelection and
#     XmDataFielddf_SetCursorPosition are no longer exported.
#
# Programs built against 2.3.8 therefore have to be rebuilt.  libMrm and
# libUil link libXm, so they change SONAME together with it: a program
# loading an old libMrm.so.4 next to a new libXm would get two copies of
# Xm.  Bump MOTIF_SOVERSION again for any further ABI break (compare with
# abidiff against the previous release), and record it in the release
# notes.
set(MOTIF_SOVERSION 5)
set(MOTIF_LIBRARY_VERSION 5.0.0)


# -----------------------------------------------------------------------------
# Declare Options

motif_option(WITH_SHARED_LIBS "Build shared libraries (static libraries if OFF)" ON)
motif_option(WITH_DEMOS "Build examples" ON)
motif_option(WITH_TESTS "Build tests" OFF)
motif_option(WITH_UTF8 "Enable UTF-8 support" ON)
motif_option(WITH_MESSAGE_CATALOG "Enable X/Open message catalog support" OFF)
motif_option(WITH_PRINTING "Enable printing support" OFF)
motif_option(WITH_UIL_DEBUG "Enable UIL compiler debugging support" OFF)
motif_option(WITH_DOCS "Enable documentation installation" ON)
motif_option(WITH_JPEG "Enable JPEG support" ON)
motif_option(WITH_PNG "Enable PNG support" ON)
motif_option(WITH_XFT "Enable Xft support" ON)
motif_option(WITH_LTO "Enable link-time optimization (and -fno-semantic-interposition)" OFF)
motif_option(WITH_CPU_NATIVE "Optimize for the build machine's CPU (-march=native, binaries are not portable)" OFF)
motif_option(WITH_WERROR
  "Treat compiler warnings as errors in the libraries, programs, build tools and tests (not the examples); needs CMake 3.24" OFF)
set(WITH_PGO OFF CACHE STRING
  "Profile-guided optimization: OFF, GENERATE (instrumented build for the pgo-train target) or USE (build with the profiles in MOTIF_PGO_DIR)")
set_property(CACHE WITH_PGO PROPERTY STRINGS OFF GENERATE USE)
set(MOTIF_PGO_DIR "${CMAKE_BINARY_DIR}/pgo" CACHE PATH
  "Directory of the profiles that WITH_PGO=GENERATE writes and WITH_PGO=USE reads")

# Library type of the public libraries (Xm, Mrm, Uil). Internal helper
# libraries stay static, built as PIC so they can be linked into the shared
# ones.
if(WITH_SHARED_LIBS)
  set(MOTIF_LIB_TYPE SHARED)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else()
  set(MOTIF_LIB_TYPE STATIC)
endif()

motif_option(WITH_HARDENING
  "Build with hardening flags (_FORTIFY_SOURCE=3, stack protector, stack clash protection, CET, full RELRO)" ON)

# Sanitizers and coverage, see "Sanitizers and coverage" below
motif_option(WITH_COMPILER_ASAN
  "Build with AddressSanitizer (includes LeakSanitizer where supported)" OFF)
motif_option(WITH_UBSAN "Build with UndefinedBehaviorSanitizer" OFF)
motif_option(WITH_TSAN "Build with ThreadSanitizer" OFF)
motif_option(WITH_MSAN
  "Build with MemorySanitizer (Clang only; all linked libraries must be instrumented too)" OFF)
motif_option(WITH_COMPILER_CODE_COVERAGE
  "Build with code coverage instrumentation (gcov for GCC, source-based for Clang)" OFF)
mark_as_advanced(WITH_COMPILER_ASAN WITH_UBSAN WITH_TSAN WITH_MSAN
  WITH_COMPILER_CODE_COVERAGE)

# Compiler launcher; an explicit CMAKE_C_COMPILER_LAUNCHER takes precedence.
option(WITH_COMPILER_CCACHE "Use ccache to improve rebuild times" OFF)
mark_as_advanced(WITH_COMPILER_CCACHE)
if(WITH_COMPILER_CCACHE AND NOT CMAKE_C_COMPILER_LAUNCHER)
  find_program(CCACHE_PROGRAM ccache)
  mark_as_advanced(CCACHE_PROGRAM)
  if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
  else()
    message(WARNING "WITH_COMPILER_CCACHE: ccache not found")
  endif()
endif()

# Ninja job pools, to limit memory use on small machines.  Off by default:
# "ninja -j" already bounds the number of jobs.  When enabled, the pool sizes
# default to one compile job per GiB and one link job per 2 GiB of memory,
# at most one per CPU.
if(CMAKE_GENERATOR MATCHES "Ninja")
  option(WITH_NINJA_POOL_JOBS "Limit parallel compile and link jobs with Ninja job pools" OFF)
  mark_as_advanced(WITH_NINJA_POOL_JOBS)
  if(WITH_NINJA_POOL_JOBS)
    cmake_host_system_information(RESULT _ncpu QUERY NUMBER_OF_LOGICAL_CORES)
    cmake_host_system_information(RESULT _mem QUERY TOTAL_PHYSICAL_MEMORY)
    math(EXPR _compile_jobs "${_mem} / 1024")
    math(EXPR _link_jobs "${_mem} / 2048")
    foreach(_var _compile_jobs _link_jobs)
      if(${_var} GREATER _ncpu)
        set(${_var} ${_ncpu})
      endif()
      if(${_var} LESS 1)
        set(${_var} 1)
      endif()
    endforeach()
    set(MOTIF_NINJA_COMPILE_JOBS ${_compile_jobs} CACHE STRING "Size of the Ninja compile job pool")
    set(MOTIF_NINJA_LINK_JOBS ${_link_jobs} CACHE STRING "Size of the Ninja link job pool")
    mark_as_advanced(MOTIF_NINJA_COMPILE_JOBS MOTIF_NINJA_LINK_JOBS)
    set_property(GLOBAL PROPERTY JOB_POOLS
      motif_compile=${MOTIF_NINJA_COMPILE_JOBS}
      motif_link=${MOTIF_NINJA_LINK_JOBS})
    set(CMAKE_JOB_POOL_COMPILE motif_compile)
    set(CMAKE_JOB_POOL_LINK motif_link)
  endif()
endif()

# -----------------------------------------------------------------------------
# Build-time tools
#
# makestrs, mkcatdefs, wml and wmluiltok run during the build, and uil
# compiles the demos' UIL files (UID files are portable).  When
# cross-compiling, build Motif natively first and point MOTIF_HOST_TOOLS at
# the MotifHostTools.cmake that the native build writes to its build
# directory; the tools are then imported from there instead of being built.

set(MOTIF_HOST_TOOLS "" CACHE FILEPATH
  "MotifHostTools.cmake of a native Motif build, for cross-compiling")
set(MOTIF_HOST_TOOL_NAMES makestrs mkcatdefs wml wmluiltok uil)
if(MOTIF_HOST_TOOLS)
  include("${MOTIF_HOST_TOOLS}")
  foreach(_tool IN LISTS MOTIF_HOST_TOOL_NAMES)
    if(NOT TARGET MotifHost::${_tool})
      message(FATAL_ERROR "${MOTIF_HOST_TOOLS} does not provide MotifHost::${_tool}")
    endif()
  endforeach()
  set(MOTIF_HOST_TOOL_PREFIX MotifHost::)
elseif(CMAKE_CROSSCOMPILING)
  message(FATAL_ERROR
    "Cross-compiling Motif needs build tools that run on the build machine: "
    "build Motif natively and pass -DMOTIF_HOST_TOOLS=<native build>/MotifHostTools.cmake")
else()
  set(MOTIF_HOST_TOOL_PREFIX "")
endif()

# -----------------------------------------------------------------------------
# Logging options
set(LOG_LEVEL "INFO" CACHE STRING "Default logging level (DEBUG, INFO, WARN, ERROR, CRITICAL)")
set(LOG_OUTPUT "stderr" CACHE STRING "Default log output destination (stderr, stdout, file)")

# PWC_DEBUG is a project-specific debug flag (DataF.c)
option(WITH_PWC_DEBUG "Enable PWC debug output" OFF)

# -----------------------------------------------------------------------------
# Dependencies

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Parser and scanner generators for the WML tools and the UIL compiler.
# Only the UIL parser is generated when the WML tools are imported.
if(MOTIF_HOST_TOOLS)
  motif_find_parser_generators(NO_LEX)
else()
  motif_find_parser_generators()
endif()

# X11 and Xt are part of the public interface of libXm; the others are only
# used inside the libraries and programs.  The MOTIF_ prefixes keep the
# PkgConfig:: target names from clashing with those of a consuming project,
# which recreates them in MotifConfig.cmake.
pkg_check_modules(MOTIF_XT REQUIRED IMPORTED_TARGET x11 xt)
pkg_check_modules(MOTIF_XEXT REQUIRED IMPORTED_TARGET xext xmu xpm)
set(MOTIF_X11_TARGETS PkgConfig::MOTIF_XT PkgConfig::MOTIF_XEXT)

# Required by the font code even without Xft
pkg_check_modules(MOTIF_FONTCONFIG REQUIRED IMPORTED_TARGET fontconfig)

if(WITH_XFT)
  pkg_check_modules(MOTIF_XFT IMPORTED_TARGET xft>=2)
endif()
if(WITH_JPEG)
  pkg_check_modules(MOTIF_JPEG IMPORTED_TARGET libjpeg)
endif()
if(WITH_PNG)
  pkg_check_modules(MOTIF_PNG IMPORTED_TARGET libpng)
endif()
if(WITH_PRINTING)
  pkg_check_modules(MOTIF_XP IMPORTED_TARGET xp)
  if(NOT MOTIF_XP_FOUND)
    message(WARNING "libXp not found - disabling printing support")
    set(WITH_PRINTING OFF CACHE BOOL "Enable printing support" FORCE)
  endif()
endif()

if(WITH_UTF8)
  # iconv is part of the C library on glibc and musl
  find_library(ICONV_LIBRARY iconv)
  mark_as_advanced(ICONV_LIBRARY)
endif()

if(WITH_MESSAGE_CATALOG)
  # Compiles the generated numeric catalogs to .cat files (optional: the
  # libraries only need the generated *MsgCatI.h headers to build).
  find_program(GENCAT_EXECUTABLE gencat)
  if(NOT GENCAT_EXECUTABLE)
    message(STATUS "gencat not found: message catalogs (.cat) will not be compiled")
  endif()
endif()

# -----------------------------------------------------------------------------
# System checks
#
# Their results go into MOTIF_CONFIG_DEFINITIONS, which motif_build_flags
# below passes to every target built here.

set(MOTIF_CONFIG_DEFINITIONS)

set(CMAKE_REQUIRED_INCLUDES ${MOTIF_XT_INCLUDE_DIRS})
foreach(_header
    alloca.h iconv.h wctype.h fcntl.h sys/poll.h stdint.h inttypes.h)
  string(MAKE_C_IDENTIFIER "HAVE_${_header}" _var)
  string(TOUPPER "${_var}" _var)
  check_include_file("${_header}" ${_var})
  if(${_var})
    list(APPEND MOTIF_CONFIG_DEFINITIONS ${_var})
  endif()
endforeach()

foreach(_func
    iconv putenv strcasecmp strdup strlcat getpwnam getpwuid)
  string(TOUPPER "HAVE_${_func}" _var)
  check_function_exists(${_func} ${_var})
  if(${_var})
    list(APPEND MOTIF_CONFIG_DEFINITIONS ${_var})
  endif()
endforeach()

# Always defined, to 0 or 1, as the sources test it with #if
check_symbol_exists(XICProc "X11/Xlib.h" HAVE_XICPROC)
if(HAVE_XICPROC)
  list(APPEND MOTIF_CONFIG_DEFINITIONS HAVE_XICPROC=1)
else()
  list(APPEND MOTIF_CONFIG_DEFINITIONS HAVE_XICPROC=0)
endif()
unset(CMAKE_REQUIRED_INCLUDES)

# Configuration of the libraries
list(APPEND MOTIF_CONFIG_DEFINITIONS
  HAVE_LIBXMU
  NO_OL_COMPAT=1
)
if(WITH_PWC_DEBUG)
  list(APPEND MOTIF_CONFIG_DEFINITIONS PWC_DEBUG=1)
else()
  list(APPEND MOTIF_CONFIG_DEFINITIONS PWC_DEBUG=0)
endif()

# The XM_* values are also written into the generated Xm/Xm.h.
if(MOTIF_XFT_FOUND)
  list(APPEND MOTIF_CONFIG_DEFINITIONS USE_XFT)
endif()
if(MOTIF_JPEG_FOUND)
  set(XM_WITH_JPEG "#define XM_WITH_JPEG 1")
  list(APPEND MOTIF_CONFIG_DEFINITIONS XM_WITH_JPEG)
else()
  set(XM_WITH_JPEG "#define XM_WITH_JPEG 0")
endif()
if(MOTIF_PNG_FOUND)
  set(XM_WITH_PNG "#define XM_WITH_PNG 1")
  list(APPEND MOTIF_CONFIG_DEFINITIONS XM_WITH_PNG)
else()
  set(XM_WITH_PNG "#define XM_WITH_PNG 0")
endif()
if(WITH_PRINTING)
  set(XM_PRINTING "#define XM_PRINTING 1")
  list(APPEND MOTIF_CONFIG_DEFINITIONS XM_PRINTING)
else()
  set(XM_PRINTING "#define XM_PRINTING 0")
endif()
if(WITH_UTF8)
  set(XM_UTF8 "#define XM_UTF8 1")
  list(APPEND MOTIF_CONFIG_DEFINITIONS XM_UTF8)
else()
  set(XM_UTF8 "#define XM_UTF8 0")
endif()
if(WITH_MESSAGE_CATALOG)
  list(APPEND MOTIF_CONFIG_DEFINITIONS XM_MSGCAT=1)
else()
  list(APPEND MOTIF_CONFIG_DEFINITIONS XM_MSGCAT=0)
endif()

if(LOG_LEVEL STREQUAL "DEBUG")
  set(_level DBG)
elseif(LOG_LEVEL STREQUAL "WARN")
  set(_level WARN)
elseif(LOG_LEVEL STREQUAL "ERROR")
  set(_level ERR)
elseif(LOG_LEVEL STREQUAL "CRITICAL")
  set(_level CRITICAL)
else()
  set(_level INFO)
endif()
list(APPEND MOTIF_CONFIG_DEFINITIONS
  XM_DEFAULT_LOG_LEVEL=XM_LOG_LEVEL_${_level}
  XM_DEFAULT_LOG_OUTPUT="${LOG_OUTPUT}"
)

# -----------------------------------------------------------------------------
# Warnings
#
# One curated list, which the library code (Xm, Mrm, Uil, mwm, wml) and
# the tests build without warnings with GCC and Clang.  Warnings are not
# errors (WITH_WERROR=ON makes them so outside the examples, as CI does),
# apart from the few -Werror= below that always indicate a bug.

set(MOTIF_C_WARNINGS)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  motif_check_c_flags(MOTIF_C_WARNINGS
    -Wall
    -Wpointer-arith
    -Wlogical-op
    -Wundef
    -Winit-self
    -Wmissing-include-dirs
    -Wformat-signedness
    -Wrestrict
    -Wabsolute-value
    -Wshadow
    -Wimplicit-fallthrough
    -Wstrict-prototypes
    -Wold-style-definition
    -Wmissing-prototypes
    -Wsign-compare
    -Werror=implicit-function-declaration
    -Werror=return-type
    -Werror=vla
    # Legacy code patterns that are not worth the churn
    -Wno-char-subscripts
    -Wno-unknown-pragmas
    -Wno-div-by-zero
    -Wno-stringop-overread
    -Wno-misleading-indentation
    -Wno-unused-function
    -Wno-unused-variable
    -Wno-unused-but-set-variable
    -Wno-unused-value
  )
  if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    motif_check_c_flags(MOTIF_C_WARNINGS -Wno-tautological-compare)
  endif()
endif()

# -----------------------------------------------------------------------------
# Build flags shared by every target built here
#
# motif_build_flags carries the feature macros, the configuration defines,
# the include directories and the warnings.  Xm links it as
# PUBLIC $<BUILD_INTERFACE:...>, so everything in the tree that links Xm gets
# it as well; the build tools and static helper libraries that do not link
# Xm link it directly.  It is never exported or installed.

add_library(motif_build_flags INTERFACE)
target_include_directories(motif_build_flags INTERFACE
  # Generated headers (Xm/Xm.h, Xm/XmStrDefs*.h, Xm/version.h, the message
  # catalog headers, config.h) come first, so that they win over the stubs
  # of the same name in the source tree.
  ${CMAKE_BINARY_DIR}/include
  # <Xm/...> and <Mrm/...> from the sources
  ${CMAKE_SOURCE_DIR}/src/lib
  # <Dt/...>
  ${CMAKE_SOURCE_DIR}/include
)
if(MOTIF_XFT_FOUND)
  # With USE_XFT, internal and widget headers include <X11/Xft/Xft.h>.
  target_include_directories(motif_build_flags INTERFACE
    $<TARGET_PROPERTY:PkgConfig::MOTIF_XFT,INTERFACE_INCLUDE_DIRECTORIES>)
endif()
target_compile_definitions(motif_build_flags INTERFACE
  # glibc and musl expose all of POSIX, XSI, BSD and GNU with _GNU_SOURCE,
  # and the BSDs and macOS do so by default.  Requesting a _POSIX_C_SOURCE
  # or _XOPEN_SOURCE level on top would only hide interfaces again there.
  _GNU_SOURCE
  $<$<PLATFORM_ID:SunOS>:__EXTENSIONS__>
  ${MOTIF_CONFIG_DEFINITIONS}
)
target_compile_options(motif_build_flags INTERFACE ${MOTIF_C_WARNINGS})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/Xm ${CMAKE_BINARY_DIR}/include/Mrm)

# -----------------------------------------------------------------------------
# Sanitizers and coverage
#
# These are global compile and link options rather than part of
# motif_build_flags: every program, tests included, has to be built and
# linked with the same instrumentation as the libraries it loads.

set(MOTIF_SANITIZE_FLAGS)
if(WITH_COMPILER_ASAN)
  list(APPEND MOTIF_SANITIZE_FLAGS -fsanitize=address)
endif()
if(WITH_UBSAN)
  # Unaligned accesses are deliberately left out: the byte stream, property
  # and .uid readers do them everywhere, which would drown the other
  # reports.  Errors are reported and execution continues; for a hard stop
  # add -fno-sanitize-recover=undefined to CMAKE_C_FLAGS.
  list(APPEND MOTIF_SANITIZE_FLAGS
    -fsanitize=undefined
    -fsanitize=float-divide-by-zero
    -fsanitize=float-cast-overflow
    -fno-sanitize=alignment
  )
  # Clang 17 and later also check, as part of -fsanitize=undefined, that
  # a function is called through a pointer of its own type.  The trait
  # records, converters and callbacks take handlers defined with the
  # widget's own record type (XmRowColumnWidget, ...) through a cast to
  # the Widget-typed slot all over libXm and mwm, so this too is left
  # out until those are fixed.
  check_c_compiler_flag(-fno-sanitize=function MOTIF_HAVE_NO_SANITIZE_FUNCTION)
  if(MOTIF_HAVE_NO_SANITIZE_FUNCTION)
    list(APPEND MOTIF_SANITIZE_FLAGS -fno-sanitize=function)
  endif()
endif()
if(WITH_TSAN)
  if(WITH_COMPILER_ASAN OR WITH_MSAN)
    message(FATAL_ERROR "WITH_TSAN cannot be combined with WITH_COMPILER_ASAN or WITH_MSAN")
  endif()
  list(APPEND MOTIF_SANITIZE_FLAGS -fsanitize=thread)
endif()
if(WITH_MSAN)
  if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "WITH_MSAN requires Clang")
  endif()
  if(WITH_COMPILER_ASAN)
    message(FATAL_ERROR "WITH_MSAN cannot be combined with WITH_COMPILER_ASAN")
  endif()
  # Reports are false positives unless libX11, libXt and every other linked
  # library are built with MemorySanitizer as well.
  list(APPEND MOTIF_SANITIZE_FLAGS -fsanitize=memory -fsanitize-memory-track-origins)
endif()

if(MOTIF_SANITIZE_FLAGS)
  if(NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR "Sanitizers are only supported with GCC and Clang")
  endif()
  string(REPLACE ";" " " CMAKE_REQUIRED_FLAGS "${MOTIF_SANITIZE_FLAGS}")
  set(CMAKE_REQUIRED_LINK_OPTIONS ${MOTIF_SANITIZE_FLAGS})
  string(MAKE_C_IDENTIFIER "MOTIF_SANITIZERS_WORK${CMAKE_REQUIRED_FLAGS}" _var)
  check_c_source_compiles("int main(void) { return 0; }" ${_var})
  unset(CMAKE_REQUIRED_FLAGS)
  unset(CMAKE_REQUIRED_LINK_OPTIONS)
  if(NOT ${_var})
    message(FATAL_ERROR
      "The C compiler cannot build and link with ${MOTIF_SANITIZE_FLAGS}")
  endif()
  add_compile_options(${MOTIF_SANITIZE_FLAGS} -fno-omit-frame-pointer)
  add_link_options(${MOTIF_SANITIZE_FLAGS})
endif()

# The build runs some of the programs it builds (makestrs, mkcatdefs, wml,
# wmluiltok, uil).  These short-lived generators do not free all their
# memory, so keep LeakSanitizer from failing the build because of them;
# memory errors are still reported.  Prefix such commands with
# ${MOTIF_TOOL_ENV}.
set(MOTIF_TOOL_ENV)
if(WITH_COMPILER_ASAN)
  set(MOTIF_TOOL_ENV ${CMAKE_COMMAND} -E env ASAN_OPTIONS=detect_leaks=0)
endif()

if(WITH_COMPILER_CODE_COVERAGE)
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(_coverage_flags --coverage)
  elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # Write the raw profiles with LLVM_PROFILE_FILE, then merge them with
    # llvm-profdata and report with llvm-cov.
    set(_coverage_flags -fprofile-instr-generate -fcoverage-mapping)
  else()
    message(FATAL_ERROR
      "WITH_COMPILER_CODE_COVERAGE is not supported with ${CMAKE_C_COMPILER_ID}")
  endif()
  add_compile_options(${_coverage_flags})
  add_link_options(${_coverage_flags})
endif()

# -----------------------------------------------------------------------------
# Hardening
#
# Global for the same reason as the sanitizers: the programs as well as the
# libraries should get them.

if(WITH_HARDENING AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  set(MOTIF_HARDENING_FLAGS)
  motif_check_c_flags(MOTIF_HARDENING_FLAGS
    -fstack-protector-strong
    -fstack-clash-protection
    -fcf-protection
  )
  add_compile_options(${MOTIF_HARDENING_FLAGS})

  # _FORTIFY_SOURCE needs optimisation (glibc warns without it), and the
  # sanitizers do these checks themselves.  -U first, as some toolchains
  # predefine a different level.
  if(NOT MOTIF_SANITIZE_FLAGS)
    set(_optimized "$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>")
    add_compile_options(
      "$<${_optimized}:-U_FORTIFY_SOURCE>"
      "$<${_optimized}:-D_FORTIFY_SOURCE=3>"
    )
  endif()

  if(NOT APPLE)
    set(CMAKE_REQUIRED_LINK_OPTIONS -Wl,-z,relro -Wl,-z,now)
    check_c_source_compiles("int main(void) { return 0; }" MOTIF_HAVE_LDFLAG_RELRO_NOW)
    unset(CMAKE_REQUIRED_LINK_OPTIONS)
    if(MOTIF_HAVE_LDFLAG_RELRO_NOW)
      add_link_options(-Wl,-z,relro -Wl,-z,now)
    endif()
  endif()
endif()

# Strict aliasing
#
# The sources still hand typed pointers to X functions that take a generic
# one by address, (XPointer *)&ptr to XFindContext, (unsigned char **)&ptr
# to XGetWindowProperty and so on, about 90 places in libXm alone, and the
# value is then written through one type and read through another.  Until
# they are gone, do not let the optimizer assume that this never happens:
# it did in 97547c53, and the result was heap corruption.

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-fno-strict-aliasing)
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

# Optimized builds (Release, RelWithDebInfo, MinSizeRel):
#   -fno-semantic-interposition lets the compiler inline and call directly
#     the exported functions of a library from inside it, as LD_PRELOAD
#     interposition of calls internal to libXm is not supported anyway;
#   -fno-plt calls the functions of other libraries through the GOT rather
#     than a PLT stub.  Only with immediate binding (-z now, WITH_HARDENING):
#     with lazy binding it would resolve every imported function at load
#     time instead of on first call.
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  set(_motif_optimized "$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>")
  set(_motif_opt_flags)
  motif_check_c_flags(_motif_opt_flags -fno-semantic-interposition)
  if(MOTIF_HAVE_LDFLAG_RELRO_NOW AND WITH_HARDENING)
    motif_check_c_flags(_motif_opt_flags -fno-plt)
  endif()
  foreach(_flag IN LISTS _motif_opt_flags)
    add_compile_options("$<${_motif_optimized}:${_flag}>")
  endforeach()
  unset(_motif_opt_flags)
endif()

# Profile-guided optimization.  Build with WITH_PGO=GENERATE, run the
# training workload (the pgo-train target), then reconfigure the same
# build directory with WITH_PGO=USE and rebuild.  With Clang, merge the raw
# profiles first: llvm-profdata merge -o ${MOTIF_PGO_DIR}/default.profdata
# ${MOTIF_PGO_DIR}/*.profraw
if(WITH_PGO)
  if(NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR "WITH_PGO is only supported with GCC and Clang")
  endif()
  if(WITH_PGO STREQUAL "GENERATE")
    file(MAKE_DIRECTORY ${MOTIF_PGO_DIR})
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
      # The libraries are used by several threads in some programs
      set(_motif_pgo_flags -fprofile-generate=${MOTIF_PGO_DIR} -fprofile-update=prefer-atomic)
    else()
      set(_motif_pgo_flags -fprofile-generate=${MOTIF_PGO_DIR})
    endif()
    add_compile_options(${_motif_pgo_flags})
    add_link_options(${_motif_pgo_flags})
    unset(_motif_pgo_flags)
  elseif(WITH_PGO STREQUAL "USE")
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
      # Code the training did not run is optimized as usual rather than for
      # size, and sources without a profile are not an error.
      add_compile_options(-fprofile-use=${MOTIF_PGO_DIR} -fprofile-partial-training
        -Wno-missing-profile)
    else()
      if(NOT EXISTS ${MOTIF_PGO_DIR}/default.profdata)
        message(FATAL_ERROR "WITH_PGO=USE: ${MOTIF_PGO_DIR}/default.profdata not found; "
          "merge the raw profiles with llvm-profdata first")
      endif()
      add_compile_options(-fprofile-use=${MOTIF_PGO_DIR}/default.profdata
        -Wno-profile-instr-unprofiled -Wno-profile-instr-out-of-date)
    endif()
  else()
    message(FATAL_ERROR "WITH_PGO must be OFF, GENERATE or USE, not '${WITH_PGO}'")
  endif()
  message(STATUS "Profile-guided optimization: ${WITH_PGO} (${MOTIF_PGO_DIR})")
endif()

# Exported symbols.  The shared libraries export only what their version
# scripts (src/lib/*/lib*.map) list, in a version node; everything else is
# bound inside the library at link time, which needs neither a PLT entry
# nor a symbolic relocation.  See doc/abi-policy.md.
set(MOTIF_HAVE_VERSION_SCRIPT OFF)
if(WITH_SHARED_LIBS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND NOT APPLE AND NOT WIN32)
  set(_map ${CMAKE_BINARY_DIR}/CMakeFiles/motif_version_test.map)
  file(WRITE ${_map} "MOTIF_TEST { global: main; local: *; };\n")
  set(CMAKE_REQUIRED_LINK_OPTIONS -Wl,--version-script=${_map})
  check_c_source_compiles("int main(void) { return 0; }" MOTIF_HAVE_LDFLAG_VERSION_SCRIPT)
  # The scripts also list the symbols of optional features (printing, JPEG,
  # PNG), which lld rejects unless told otherwise when they are left out.
  set(CMAKE_REQUIRED_LINK_OPTIONS -Wl,--version-script=${_map} -Wl,--undefined-version)
  check_c_source_compiles("int main(void) { return 0; }" MOTIF_HAVE_LDFLAG_UNDEFINED_VERSION)
  unset(CMAKE_REQUIRED_LINK_OPTIONS)
  unset(_map)
  if(MOTIF_HAVE_LDFLAG_VERSION_SCRIPT)
    set(MOTIF_HAVE_VERSION_SCRIPT ON)
  else()
    message(WARNING "The linker does not support --version-script: the "
      "libraries will export all their symbols")
  endif()
endif()

# motif_version_script(<target> <script>)
#
# Link the shared library <target> with the version script <script>.
function(motif_version_script target script)
  if(MOTIF_HAVE_VERSION_SCRIPT)
    target_link_options(${target} PRIVATE -Wl,--version-script=${script})
    if(MOTIF_HAVE_LDFLAG_UNDEFINED_VERSION)
      target_link_options(${target} PRIVATE -Wl,--undefined-version)
    endif()
    set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS ${script})
  endif()
endfunction()

# motif_ban_unsafe_string_functions(<target>...)
#
# Compile the sources of the library <target>s with src/lib/XmBannedI.h
# force-included, so that a call of sprintf, vsprintf, strcpy or strcat
# fails to compile (GCC and Clang).
function(motif_ban_unsafe_string_functions)
  if(NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    return()
  endif()
  foreach(_target IN LISTS ARGN)
    target_compile_options(${_target} PRIVATE
      "SHELL:-include ${CMAKE_SOURCE_DIR}/src/lib/XmBannedI.h"
      -Werror=deprecated-declarations)
  endforeach()
endfunction()

# motif_exports_test(<target> <script> HEADERS <header>...
#                    [OBJECTS_OF <object library>...])
#
# With WITH_TESTS, add the test abi.exports.<target>: the shared library
# <target> must export every symbol that the installed <header>s declare,
# that is <script> must not leave one out.  The objects of <target> and of
# the OBJECTS_OF libraries are the symbols looked for.
function(motif_exports_test target script)
  cmake_parse_arguments(E "" "" "HEADERS;OBJECTS_OF" ${ARGN})
  if(NOT WITH_TESTS OR NOT MOTIF_HAVE_VERSION_SCRIPT OR NOT CMAKE_NM)
    return()
  endif()
  set(_headers)
  foreach(_h IN LISTS E_HEADERS)
    get_filename_component(_h ${_h} ABSOLUTE)
    list(APPEND _headers ${_h})
  endforeach()
  set(_objects "$<TARGET_OBJECTS:${target}>")
  foreach(_lib IN LISTS E_OBJECTS_OF)
    string(APPEND _objects ";$<TARGET_OBJECTS:${_lib}>")
  endforeach()
  set(_args ${CMAKE_CURRENT_BINARY_DIR}/${target}_exports_test.cmake)
  file(GENERATE OUTPUT ${_args} CONTENT
"set(NM \"${CMAKE_NM}\")
set(LIBRARY \"$<TARGET_FILE:${target}>\")
set(MAP \"${script}\")
set(OBJECTS \"${_objects}\")
set(HEADERS \"${_headers}\")
")
  add_test(NAME abi.exports.${target}
    COMMAND ${CMAKE_COMMAND} -DARGS=${_args}
      -P ${CMAKE_SOURCE_DIR}/tools/cmake/scripts/check_exports.cmake)
  set_tests_properties(abi.exports.${target} PROPERTIES LABELS ABI)
endfunction()

# C17 with GNU extensions (-std=gnu17).  C23 needs the remaining
# unprototyped () function declarations fixed first.
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# WITH_WERROR: CMAKE_COMPILE_WARNING_AS_ERROR initializes the
# COMPILE_WARNING_AS_ERROR property of the targets created after it is set,
# so it is on for everything below except the examples, which are older code
# that is not kept free of the curated warnings.
if(WITH_WERROR)
  if(CMAKE_VERSION VERSION_LESS 3.24)
    message(FATAL_ERROR "WITH_WERROR needs CMake 3.24 or later")
  endif()
  set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
endif()

# Add subdirectories
add_subdirectory(include)
add_subdirectory(src/lib)
add_subdirectory(src/bin)
add_subdirectory(data)

if(WITH_MESSAGE_CATALOG)
    add_subdirectory(localized)
endif()

if(WITH_DEMOS)
    if(WITH_WERROR)
        set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
    endif()
    add_subdirectory(src/examples)
    if(WITH_WERROR)
        set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
    endif()
endif()

if(WITH_TESTS)
    add_subdirectory(src/tests)
endif()

if(WITH_DOCS)
    add_subdirectory(doc)
endif()

# The training workload of WITH_PGO=GENERATE: the test suite (WITH_TESTS),
# whose X tests run under xvfb-run, and MOTIF_PGO_TRAINING_COMMAND, e.g. a
# benchmark run under xvfb-run.
if(WITH_PGO STREQUAL "GENERATE")
  set(MOTIF_PGO_TRAINING_COMMAND "" CACHE STRING
    "Additional PGO training command (a ;-list), run by the pgo-train target")
  set(_motif_pgo_commands)
  if(MOTIF_PGO_TRAINING_COMMAND)
    list(APPEND _motif_pgo_commands COMMAND ${MOTIF_PGO_TRAINING_COMMAND})
  endif()
  if(WITH_TESTS)
    list(APPEND _motif_pgo_commands
      COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>)
  endif()
  if(_motif_pgo_commands)
    add_custom_target(pgo-train ${_motif_pgo_commands}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Running the PGO training workload (profiles in ${MOTIF_PGO_DIR})"
      USES_TERMINAL)
  else()
    message(WARNING "WITH_PGO=GENERATE: no training workload; enable WITH_TESTS, "
      "set MOTIF_PGO_TRAINING_COMMAND or run programs of this build to collect "
      "the profiles")
  endif()
  unset(_motif_pgo_commands)
endif()

# Let a cross build import the build-time tools of this one.
if(NOT MOTIF_HOST_TOOLS)
  export(TARGETS ${MOTIF_HOST_TOOL_NAMES}
    NAMESPACE MotifHost::
    FILE ${CMAKE_BINARY_DIR}/MotifHostTools.cmake
  )
endif()

# -----------------------------------------------------------------------------
# pkg-config files: motif.pc (libXm), mrm.pc and uil.pc

set(_pc_requires xext xmu xpm fontconfig)
if(MOTIF_XFT_FOUND)
  list(APPEND _pc_requires xft)
endif()
if(MOTIF_JPEG_FOUND)
  list(APPEND _pc_requires libjpeg)
endif()
if(MOTIF_PNG_FOUND)
  list(APPEND _pc_requires libpng)
endif()
string(REPLACE ";" " " MOTIF_PC_XM_REQUIRES_PRIVATE "${_pc_requires}")

# With printing, <Xm/Xm.h> includes <X11/extensions/Print.h>, so libXp is
# part of the public interface as X11 and Xt are.
set(MOTIF_PC_XM_REQUIRES "xt x11")
if(WITH_PRINTING)
  string(APPEND MOTIF_PC_XM_REQUIRES " xp")
endif()

set(_pc_libs)
if(ICONV_LIBRARY)
  list(APPEND _pc_libs -liconv)
endif()
list(APPEND _pc_libs -lm ${CMAKE_THREAD_LIBS_INIT})
string(REPLACE ";" " " MOTIF_PC_XM_LIBS_PRIVATE "${_pc_libs}")

# Relative to ${prefix} where possible, so that the files can be relocated
# (pkgconf --define-prefix); absolute CMAKE_INSTALL_*DIR values are kept.
foreach(_dir LIBDIR:exec_prefix INCLUDEDIR:prefix)
  string(REPLACE ":" ";" _dir "${_dir}")
  list(GET _dir 0 _name)
  list(GET _dir 1 _base)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${_name}}")
    set(MOTIF_PC_${_name} "${CMAKE_INSTALL_FULL_${_name}}")
  else()
    set(MOTIF_PC_${_name} "\${${_base}}/${CMAKE_INSTALL_${_name}}")
  endif()
endforeach()

foreach(_pc motif mrm uil)
  configure_file(
    ${CMAKE_SOURCE_DIR}/data/${_pc}.pc.in
    ${CMAKE_BINARY_DIR}/${_pc}.pc
    @ONLY
  )
  install(FILES ${CMAKE_BINARY_DIR}/${_pc}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  )
endforeach()

# -----------------------------------------------------------------------------
# CMake package: find_package(Motif) provides Motif::Xm, Motif::Mrm and
# Motif::Uil.

include(CMakePackageConfigHelpers)
set(MOTIF_CMAKE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/Motif)

# pkg-config modules that MotifConfig.cmake looks up as PkgConfig::<prefix>,
# as "<prefix>|<modules>".  Static libraries also need their private
# dependencies.
set(MOTIF_CONFIG_PKG_DEPS "MOTIF_XT|x11 xt")
if(WITH_PRINTING)
  list(APPEND MOTIF_CONFIG_PKG_DEPS "MOTIF_XP|xp")
endif()
if(NOT WITH_SHARED_LIBS)
  list(APPEND MOTIF_CONFIG_PKG_DEPS
    "MOTIF_XEXT|xext xmu xpm"
    "MOTIF_FONTCONFIG|fontconfig"
  )
  if(MOTIF_XFT_FOUND)
    list(APPEND MOTIF_CONFIG_PKG_DEPS "MOTIF_XFT|xft>=2")
  endif()
  if(MOTIF_JPEG_FOUND)
    list(APPEND MOTIF_CONFIG_PKG_DEPS "MOTIF_JPEG|libjpeg")
  endif()
  if(MOTIF_PNG_FOUND)
    list(APPEND MOTIF_CONFIG_PKG_DEPS "MOTIF_PNG|libpng")
  endif()
endif()

configure_package_config_file(
  ${CMAKE_SOURCE_DIR}/tools/cmake/MotifConfig.cmake.in
  ${CMAKE_BINARY_DIR}/MotifConfig.cmake
  INSTALL_DESTINATION ${MOTIF_CMAKE_INSTALL_DIR}
)
write_basic_package_version_file(
  ${CMAKE_BINARY_DIR}/MotifConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(EXPORT MotifTargets
  NAMESPACE Motif::
  DESTINATION ${MOTIF_CMAKE_INSTALL_DIR}
)
install(FILES
  ${CMAKE_BINARY_DIR}/MotifConfig.cmake
  ${CMAKE_BINARY_DIR}/MotifConfigVersion.cmake
  DESTINATION ${MOTIF_CMAKE_INSTALL_DIR}
)

# Install README
install(FILES README.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/Xm/doc
)

# -----------------------------------------------------------------------------
# Print Final Configuration

motif_print_summary()
