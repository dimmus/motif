# libFuzzer targets (WITH_FUZZERS, Clang only).
#
# Each fuzz_<name> program fuzzes one parser of untrusted input.  The
# Motif libraries are instrumented for coverage (-fsanitize=fuzzer-no-link)
# so that libFuzzer can steer; combine with WITH_COMPILER_ASAN and
# WITH_UBSAN to catch memory errors.  The targets marked X need an X
# server and are run under xvfb-run.
#
# Fuzz.<name> runs every input of the seed corpus (corpus/<name>, plus
# the image fixtures for the image loaders) once, as a regression test;
# Fuzz.<name>.crashes replays the reproducers of known, unfixed library
# bugs in crashes/<name> and passes while they still fail.  run.sh runs
# the fuzzers for real, for a given time.

if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "WITH_FUZZERS requires Clang (libFuzzer)")
endif()

include(CheckCSourceCompiles)
set(CMAKE_REQUIRED_FLAGS "-fsanitize=fuzzer")
set(CMAKE_REQUIRED_LINK_OPTIONS -fsanitize=fuzzer)
check_c_source_compiles("
#include <stddef.h>
#include <stdint.h>
int LLVMFuzzerTestOneInput(const uint8_t *d, size_t n) { (void)d; (void)n; return 0; }"
    MOTIF_HAVE_LIBFUZZER)
unset(CMAKE_REQUIRED_FLAGS)
unset(CMAKE_REQUIRED_LINK_OPTIONS)
if(NOT MOTIF_HAVE_LIBFUZZER)
    message(FATAL_ERROR "WITH_FUZZERS: ${CMAKE_C_COMPILER} cannot link with -fsanitize=fuzzer")
endif()

# Coverage feedback from the libraries under test
foreach(_lib Xm Mrm Uil UilCompiler)
    if(TARGET ${_lib})
        target_compile_options(${_lib} PRIVATE -fsanitize=fuzzer-no-link)
    endif()
endforeach()

set(MOTIF_FUZZ_CORPUS ${CMAKE_CURRENT_SOURCE_DIR}/corpus)
set(MOTIF_FUZZ_CRASHES ${CMAKE_CURRENT_SOURCE_DIR}/crashes)

# Known leaks the fuzzers must not stop at; see lsan.supp there.
set(MOTIF_FUZZ_ENV "")
if(WITH_COMPILER_ASAN)
    list(APPEND MOTIF_FUZZ_ENV
        "LSAN_OPTIONS=suppressions=${CMAKE_CURRENT_SOURCE_DIR}/lsan.supp:print_suppressions=0")
    if(NOT LLVM_SYMBOLIZER_EXECUTABLE AND ADDR2LINE_EXECUTABLE)
        list(APPEND MOTIF_FUZZ_ENV "ASAN_SYMBOLIZER_PATH=${ADDR2LINE_EXECUTABLE}")
    endif()
endif()

# motif_fuzz_test(<test> <target> <needs X> <environment> <args>...)
function(motif_fuzz_test test target x env)
    set(MOTIF_TEST_WORKDIR ${CMAKE_CURRENT_BINARY_DIR})
    if(x)
        motif_x_test(${test} $<TARGET_FILE:fuzz_${target}> ${ARGN})
        set_tests_properties(${test} PROPERTIES LABELS "X11;Fuzz")
    else()
        add_test(NAME ${test} COMMAND fuzz_${target} ${ARGN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        set_tests_properties(${test} PROPERTIES LABELS Fuzz)
    endif()
    if(env)
        set_tests_properties(${test} PROPERTIES ENVIRONMENT "${env}")
    endif()
endfunction()

# motif_fuzzer(<name> [X] [NO_LEAKS] SOURCES <src>... [DEFINES <def>...]
#              [LIBS <lib>...] [SEEDS <dir>...] [CRASH_ENV <var=value>...])
# NO_LEAKS turns leak detection off, for code that leaks on every input;
# CRASH_ENV is extra environment for replaying crashes/<name>.
function(motif_fuzzer name)
    cmake_parse_arguments(F "X;NO_LEAKS" "" "SOURCES;DEFINES;LIBS;SEEDS;CRASH_ENV" ${ARGN})
    add_executable(fuzz_${name} ${F_SOURCES})
    target_compile_definitions(fuzz_${name} PRIVATE ${F_DEFINES})
    target_compile_options(fuzz_${name} PRIVATE -fsanitize=fuzzer)
    target_link_options(fuzz_${name} PRIVATE -fsanitize=fuzzer)
    target_include_directories(fuzz_${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}/src/lib
        ${CMAKE_SOURCE_DIR}/src/lib/Xm
        ${CMAKE_SOURCE_DIR}/src/lib/Uil
        ${CMAKE_BINARY_DIR}/include
    )
    target_link_libraries(fuzz_${name} PRIVATE ${F_LIBS} Xm)

    # libFuzzer's default error exit code, 77, is CTest's "skipped"
    set(_opts -runs=0 -error_exitcode=1)
    set(_env ${MOTIF_FUZZ_ENV})
    if(F_NO_LEAKS)
        list(APPEND _opts -detect_leaks=0)
        list(APPEND _env ASAN_OPTIONS=detect_leaks=0)
    endif()

    # Fuzz.<name>: the seeds must all run cleanly.
    set(_dirs ${F_SEEDS})
    if(EXISTS ${MOTIF_FUZZ_CORPUS}/${name})
        list(APPEND _dirs ${MOTIF_FUZZ_CORPUS}/${name})
    endif()
    motif_fuzz_test(Fuzz.${name} ${name} "${F_X}" "${_env}" ${_opts} ${_dirs})

    # Fuzz.<name>.crashes: reproducers of library bugs that are not fixed
    # yet (see crashes/README); the test passes while they still fail.
    if(EXISTS ${MOTIF_FUZZ_CRASHES}/${name})
        # No leak suppressions here: some reproducers are leaks.
        set(_crash_env ${F_CRASH_ENV})
        if(F_NO_LEAKS)
            list(APPEND _crash_env ASAN_OPTIONS=detect_leaks=0)
        endif()
        if(NOT LLVM_SYMBOLIZER_EXECUTABLE AND ADDR2LINE_EXECUTABLE)
            list(APPEND _crash_env "ASAN_SYMBOLIZER_PATH=${ADDR2LINE_EXECUTABLE}")
        endif()
        motif_fuzz_test(Fuzz.${name}.crashes ${name} "${F_X}"
            "${_crash_env}" ${_opts} ${MOTIF_FUZZ_CRASHES}/${name})
        set_tests_properties(Fuzz.${name}.crashes PROPERTIES WILL_FAIL TRUE)
    endif()
endfunction()

if(MOTIF_PNG_FOUND)
    motif_fuzzer(png SOURCES fuzz_image.c DEFINES FUZZ_PNG
        SEEDS ${CMAKE_SOURCE_DIR}/src/tests/png)
endif()
if(MOTIF_JPEG_FOUND)
    motif_fuzzer(jpeg SOURCES fuzz_image.c DEFINES FUZZ_JPEG
        SEEDS ${CMAKE_SOURCE_DIR}/src/tests/jpeg)
endif()
motif_fuzzer(svg SOURCES fuzz_image.c DEFINES FUZZ_SVG
    SEEDS ${CMAKE_SOURCE_DIR}/src/tests/svg)
motif_fuzzer(xpm SOURCES fuzz_xpm.c)
motif_fuzzer(xmstring SOURCES fuzz_xmstring.c)
# Every Uil() call leaks its lexer buffer, comments and some values
# (UilLexAna.c lex_initialize_analyzer, UilSarObj.c sar_assoc_comment,
# UilP2Out.c out_emit_value, ...), about 17 kB per compilation.
motif_fuzzer(uil NO_LEAKS SOURCES fuzz_uil.c LIBS Uil)
motif_fuzzer(uid X SOURCES fuzz_uid.c LIBS Mrm)
motif_fuzzer(ct X SOURCES fuzz_ct.c)
motif_fuzzer(xbm X SOURCES fuzz_xbm.c)
motif_fuzzer(motifbind X SOURCES fuzz_motifbind.c)
motif_fuzzer(dnd X SOURCES fuzz_dnd.c)
# Corrupt records are reported as a warning now, not a fatal error, so
# the clipboard code no longer exits on them (the former reproducer,
# crashes/clipboard/corrupt-record-exit, is now a clean corpus seed).
# FUZZ_CLIPBOARD_EXIT still forces the old fatal behaviour if anything
# regresses to XtErrorMsg.
motif_fuzzer(clipboard X NO_LEAKS SOURCES fuzz_clipboard.c)
