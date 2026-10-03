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
)

# Suites that open a display, and suites that do not.
set(MOTIF_TEST_X_SUITES FontListEntry FontList)
set(MOTIF_TEST_SUITES Svg Log LogConfig)
set(MOTIF_TEST_DEFS "")

# Suites with test cases tagged "xfail" (known library bugs); these get an
# extra Xm.<suite>.xfail test that passes only while those cases still fail.
set(MOTIF_TEST_XFAIL_SUITES "")

if(WITH_JPEG AND JPEG_FOUND)
    list(APPEND MOTIF_TEST_SRCS Xm/testJpeg.c)
    list(APPEND MOTIF_TEST_SUITES Jpeg)
    list(APPEND MOTIF_TEST_XFAIL_SUITES Jpeg)
    list(APPEND MOTIF_TEST_DEFS XM_WITH_JPEG)
endif()

if(WITH_PNG AND PNG_FOUND)
    list(APPEND MOTIF_TEST_SRCS Xm/testPng.c)
    list(APPEND MOTIF_TEST_SUITES Png)
    list(APPEND MOTIF_TEST_DEFS XM_WITH_PNG)
endif()

add_executable(motif_tests ${MOTIF_TEST_SRCS})
target_compile_definitions(motif_tests PRIVATE ${MOTIF_TEST_DEFS})
target_include_directories(motif_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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
    if(XVFB_RUN_EXECUTABLE)
        add_test(NAME Xm.${suite}
            COMMAND ${XVFB_RUN_EXECUTABLE} ${XVFB_RUN_ARGS}
                    $<TARGET_FILE:motif_tests> ${suite}
            WORKING_DIRECTORY ${MOTIF_TEST_WORKDIR})
        # xvfb-run -a picks a free display number racily; serialise them.
        set_tests_properties(Xm.${suite} PROPERTIES RESOURCE_LOCK xvfb)
    else()
        add_test(NAME Xm.${suite}
            COMMAND motif_tests ${suite}
            WORKING_DIRECTORY ${MOTIF_TEST_WORKDIR})
    endif()
    set_tests_properties(Xm.${suite} PROPERTIES LABELS X11)
    list(APPEND MOTIF_TESTS Xm.${suite})
endforeach()

set_tests_properties(${MOTIF_TESTS} PROPERTIES SKIP_RETURN_CODE 77)

if(XVFB_RUN_EXECUTABLE)
    message(STATUS "Tests: X11 suites run under ${XVFB_RUN_EXECUTABLE}")
else()
    message(STATUS "Tests: not using xvfb-run, X11 suites use $DISPLAY or are skipped")
endif()
