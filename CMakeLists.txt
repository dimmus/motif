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

# Shared library versions of libXm, libMrm and libUil.
#
# Upstream Motif 2.3 ships libXm.so.4, libMrm.so.4 and libUil.so.4 (2.3.8:
# libXm.so.4.0.4), and every existing Motif program (CDE, nedit, xpdf, ...)
# is linked against SONAME 4.  This tree adds interfaces but keeps the
# existing ones, so it stays at SONAME 4 and continues the upstream version
# numbering.  The earlier libtool-style 5:0:0 ("libXm.so.5.0.0") changed
# nothing in the ABI apart from the SONAME: abidiff of the two builds reports
# only the SONAME change.  Bump MOTIF_SOVERSION only for a real ABI break,
# and record it in the release notes.
set(MOTIF_SOVERSION 4)
set(MOTIF_LIBRARY_VERSION 4.0.5)


# -----------------------------------------------------------------------------
# Declare Options

motif_option(WITH_SHARED_LIBS "Build shared libraries" ON)
motif_option(WITH_STATIC_LIBS "Build static libraries" OFF)
motif_option(WITH_DEMOS "Build examples" ON)
motif_option(WITH_TESTS "Build tests" OFF)
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

# PWC_DEBUG is a project-specific debug flag (DataF.c)
option(WITH_PWC_DEBUG "Enable PWC debug output" OFF)

# -----------------------------------------------------------------------------
# Dependencies

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Parser and scanner generators for the WML tools and the UIL compiler
motif_find_parser_generators()

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
    alloca.h iconv.h wctype.h fcntl.h sys/poll.h sys/time.h stdint.h
    inttypes.h X11/Xos_r.h X11/Xpoll.h)
  string(MAKE_C_IDENTIFIER "HAVE_${_header}" _var)
  string(TOUPPER "${_var}" _var)
  check_include_file("${_header}" ${_var})
  if(${_var})
    list(APPEND MOTIF_CONFIG_DEFINITIONS ${_var})
  endif()
endforeach()

foreach(_func
    getcwd getwd iconv nanosleep putenv regcomp strcasecmp strdup strlcat
    getpwnam getpwuid re_comp re_exec)
  string(TOUPPER "HAVE_${_func}" _var)
  check_function_exists(${_func} ${_var})
  if(${_var})
    list(APPEND MOTIF_CONFIG_DEFINITIONS ${_var})
  endif()
endforeach()
check_function_exists(regexec HAVE_REGEX)
if(HAVE_REGEX)
  list(APPEND MOTIF_CONFIG_DEFINITIONS HAVE_REGEX)
endif()

# Always defined, to 0 or 1, as the sources test it with #if
check_symbol_exists(XICProc "X11/Xlib.h" HAVE_XICPROC)
if(HAVE_XICPROC)
  list(APPEND MOTIF_CONFIG_DEFINITIONS HAVE_XICPROC=1)
else()
  list(APPEND MOTIF_CONFIG_DEFINITIONS HAVE_XICPROC=0)
endif()
unset(CMAKE_REQUIRED_INCLUDES)

# XtAppLock() and XtAppUnlock() indicate a thread-aware libXt
set(CMAKE_REQUIRED_INCLUDES ${MOTIF_XT_INCLUDE_DIRS})
set(CMAKE_REQUIRED_LIBRARIES ${MOTIF_XT_LINK_LIBRARIES})
check_c_source_compiles("
#include <X11/Intrinsic.h>
int main(void) {
    XtAppContext app_context = 0;
    XtAppLock(app_context);
    XtAppUnlock(app_context);
    return 0;
}" HAVE_XTHREADS)
unset(CMAKE_REQUIRED_INCLUDES)
unset(CMAKE_REQUIRED_LIBRARIES)
if(HAVE_XTHREADS)
  list(APPEND MOTIF_CONFIG_DEFINITIONS XTHREADS=1)
endif()

# Configuration of the libraries
list(APPEND MOTIF_CONFIG_DEFINITIONS
  HAVE_LIBXMU
  NO_OL_COMPAT=1
  _Xos_processLock=XtProcessLock
  _Xos_processUnlock=XtProcessUnlock
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
# One curated list.  These are not enabled yet because the code still
# triggers them; fix and enable them in this order:
#   -Wimplicit-fallthrough, -Wstrict-prototypes, -Wmissing-prototypes,
#   -Wdiscarded-qualifiers, -Wstringop-overflow, -Wsign-compare.
# The compiler enables -Wdiscarded-qualifiers and -Wstringop-overflow by
# default, so they are switched off explicitly until then.  Warnings are not
# errors (set CMAKE_COMPILE_WARNING_AS_ERROR to make them so), apart from the
# few -Werror= below that always indicate a bug.

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
    -Werror=implicit-function-declaration
    -Werror=return-type
    -Werror=vla
    # To be fixed, see above
    -Wno-discarded-qualifiers
    -Wno-incompatible-pointer-types-discards-qualifiers
    -Wno-stringop-overflow
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

# C17 with GNU extensions (-std=gnu17).  C23 needs the remaining
# unprototyped () function declarations fixed first.
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

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
if(WITH_PRINTING)
  list(APPEND _pc_requires xp)
endif()
string(REPLACE ";" " " MOTIF_PC_XM_REQUIRES_PRIVATE "${_pc_requires}")

set(_pc_libs)
if(ICONV_LIBRARY)
  list(APPEND _pc_libs -liconv)
endif()
list(APPEND _pc_libs -lm ${CMAKE_THREAD_LIBS_INIT})
string(REPLACE ";" " " MOTIF_PC_XM_LIBS_PRIVATE "${_pc_libs}")

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

# Install README
install(FILES README.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/Xm/doc
)

# -----------------------------------------------------------------------------
# Print Final Configuration

motif_print_summary()
