# Unit tests (libcheck), run through CTest.
#
# Every suite is registered as its own CTest test, Xm.<suite>, running the
# same motif_tests binary.  Suites that need an X server are run under
# xvfb-run when it is found at configure time; otherwise they use the
# caller's DISPLAY and exit with status 77 (reported as skipped) when
# there is none.

find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK IMPORTED_TARGET check)
if(NOT CHECK_FOUND)
    message(FATAL_ERROR "WITH_TESTS requires libcheck (pkg-config module 'check'). "
                        "Install it, or configure with -DWITH_TESTS=OFF.")
endif()

find_program(XVFB_RUN_EXECUTABLE xvfb-run
    DOC "xvfb-run used for the tests that need an X server (OFF: use $DISPLAY)")
set(XVFB_RUN_ARGS -a -s "-screen 0 1280x1024x24 +extension RENDER"
    CACHE STRING "Arguments passed to xvfb-run")
mark_as_advanced(XVFB_RUN_EXECUTABLE XVFB_RUN_ARGS)

set(MOTIF_TEST_SRCS
    runner.c
    Xm/testXmFontListEntry.c
    Xm/testXmFontList.c
    Xm/testSvg.c
    Xm/testLog.c
    Xm/testLogConfig.c
    Xm/testXmString.c
    Xm/testWidgets.c
)

# Suites that open a display, and suites that do not.
set(MOTIF_TEST_X_SUITES FontListEntry FontList XmStringCT Widgets)
set(MOTIF_TEST_SUITES Svg Log LogConfig XmString)
set(MOTIF_TEST_DEFS "")

# Suites with test cases tagged "xfail" (known library bugs); these get an
# extra Xm.<suite>.xfail test that passes only while those cases still fail.
# The second list holds the ones that need an X server.
set(MOTIF_TEST_XFAIL_SUITES XmString)
set(MOTIF_TEST_X_XFAIL_SUITES Widgets)

if(MOTIF_JPEG_FOUND)
    list(APPEND MOTIF_TEST_SRCS Xm/testJpeg.c)
    list(APPEND MOTIF_TEST_SUITES Jpeg)
    list(APPEND MOTIF_TEST_DEFS XM_WITH_JPEG)
endif()

if(MOTIF_PNG_FOUND)
    list(APPEND MOTIF_TEST_SRCS Xm/testPng.c)
    list(APPEND MOTIF_TEST_SUITES Png)
    list(APPEND MOTIF_TEST_DEFS XM_WITH_PNG)
endif()

# The Widgets suite's table of widget classes, generated from the public
# Xm headers so that new widgets are tested without editing the suite.
file(GLOB MOTIF_XM_HEADERS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/lib/Xm/*.h)
set(MOTIF_WIDGET_CLASSES_EXCLUDE "")
if(NOT WITH_PRINTING)
    list(APPEND MOTIF_WIDGET_CLASSES_EXCLUDE Print.h)
endif()
string(REPLACE ";" "," _exclude "${MOTIF_WIDGET_CLASSES_EXCLUDE}")
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/widget_classes.h
    COMMAND ${CMAKE_COMMAND}
        -DMOTIF_XM_SOURCE_DIR=${CMAKE_SOURCE_DIR}/src/lib/Xm
        -DMOTIF_WIDGET_CLASSES_OUT=${CMAKE_CURRENT_BINARY_DIR}/widget_classes.h
        -DMOTIF_WIDGET_CLASSES_EXCLUDE=${_exclude}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/widget_classes.cmake
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/widget_classes.cmake ${MOTIF_XM_HEADERS}
    COMMENT "Generating the widget class table for the tests"
    VERBATIM)
list(APPEND MOTIF_TEST_SRCS ${CMAKE_CURRENT_BINARY_DIR}/widget_classes.h)

add_executable(motif_tests ${MOTIF_TEST_SRCS})
target_compile_definitions(motif_tests PRIVATE ${MOTIF_TEST_DEFS})
target_include_directories(motif_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src/lib/Xm
    ${CMAKE_BINARY_DIR}/include
)
target_link_libraries(motif_tests PRIVATE Xm PkgConfig::CHECK)

# The suites open their fixtures by relative path and some write log files
# into the current directory, so run them from a build-tree copy of the
# fixtures rather than from the source tree.
set(MOTIF_TEST_WORKDIR ${CMAKE_CURRENT_BINARY_DIR}/fixtures)
file(GLOB MOTIF_TEST_FIXTURES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/*
    ${CMAKE_CURRENT_SOURCE_DIR}/png/*
    ${CMAKE_CURRENT_SOURCE_DIR}/svg/*
)
foreach(fixture IN LISTS MOTIF_TEST_FIXTURES)
    configure_file(${fixture} ${MOTIF_TEST_WORKDIR}/${fixture} COPYONLY)
endforeach()

# Sanitizer settings for the tests: known library leaks are suppressed
# (see lsan.supp), which needs symbolized stacks; use addr2line when
# there is no llvm-symbolizer.
set(MOTIF_TEST_ENV "")
if(WITH_COMPILER_ASAN)
    list(APPEND MOTIF_TEST_ENV
        "LSAN_OPTIONS=suppressions=${CMAKE_CURRENT_SOURCE_DIR}/lsan.supp:print_suppressions=0")
    find_program(LLVM_SYMBOLIZER_EXECUTABLE llvm-symbolizer)
    find_program(ADDR2LINE_EXECUTABLE addr2line)
    mark_as_advanced(LLVM_SYMBOLIZER_EXECUTABLE ADDR2LINE_EXECUTABLE)
    if(NOT LLVM_SYMBOLIZER_EXECUTABLE AND ADDR2LINE_EXECUTABLE)
        list(APPEND MOTIF_TEST_ENV "ASAN_SYMBOLIZER_PATH=${ADDR2LINE_EXECUTABLE}")
    endif()
endif()

# Apply MOTIF_TEST_ENV to every test of the current directory.
function(motif_test_set_env)
    get_property(_tests DIRECTORY PROPERTY TESTS)
    if(_tests AND MOTIF_TEST_ENV)
        set_tests_properties(${_tests} PROPERTIES ENVIRONMENT "${MOTIF_TEST_ENV}")
    endif()
endfunction()

# motif_x_test(<name> <command> [args...]): a test that needs an X server.
# It runs under its own Xvfb when xvfb-run was found, and otherwise uses
# $DISPLAY; the programs exit with 77 (skipped) when there is no display.
function(motif_x_test name)
    if(XVFB_RUN_EXECUTABLE)
        add_test(NAME ${name}
            COMMAND ${XVFB_RUN_EXECUTABLE} ${XVFB_RUN_ARGS} ${ARGN}
            WORKING_DIRECTORY ${MOTIF_TEST_WORKDIR})
        # xvfb-run -a picks a free display number racily; serialise them.
        set_tests_properties(${name} PROPERTIES RESOURCE_LOCK xvfb)
    else()
        add_test(NAME ${name} COMMAND ${ARGN}
            WORKING_DIRECTORY ${MOTIF_TEST_WORKDIR})
    endif()
    set_tests_properties(${name} PROPERTIES LABELS X11 SKIP_RETURN_CODE 77)
endfunction()

foreach(suite IN LISTS MOTIF_TEST_SUITES)
    add_test(NAME Xm.${suite}
        COMMAND motif_tests ${suite}
        WORKING_DIRECTORY ${MOTIF_TEST_WORKDIR})
    list(APPEND MOTIF_TESTS Xm.${suite})
endforeach()

foreach(suite IN LISTS MOTIF_TEST_XFAIL_SUITES)
    add_test(NAME Xm.${suite}.xfail
        COMMAND motif_tests --xfail ${suite}
        WORKING_DIRECTORY ${MOTIF_TEST_WORKDIR})
    list(APPEND MOTIF_TESTS Xm.${suite}.xfail)
endforeach()

foreach(suite IN LISTS MOTIF_TEST_X_SUITES)
    motif_x_test(Xm.${suite} $<TARGET_FILE:motif_tests> ${suite})
endforeach()

foreach(suite IN LISTS MOTIF_TEST_X_XFAIL_SUITES)
    motif_x_test(Xm.${suite}.xfail $<TARGET_FILE:motif_tests> --xfail ${suite})
endforeach()

set_tests_properties(${MOTIF_TESTS} PROPERTIES SKIP_RETURN_CODE 77)
motif_test_set_env()

if(XVFB_RUN_EXECUTABLE)
    message(STATUS "Tests: X11 suites run under ${XVFB_RUN_EXECUTABLE}")
else()
    message(STATUS "Tests: not using xvfb-run, X11 suites use $DISPLAY or are skipped")
endif()
