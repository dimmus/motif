# xmbench: benchmarks, not tests.  Nothing here is built by default or
# registered with CTest; build and run them with the "bench" target:
#
#   cmake --build <build> --target bench
#
# which writes <build>/src/tests/bench/xmbench.json.  XMBENCH_ARGS passes
# extra arguments (for example "-r 3 -s 0.1 micro").

add_library(xmbench_preload MODULE EXCLUDE_FROM_ALL preload.c)
target_include_directories(xmbench_preload PRIVATE ${MOTIF_XT_INCLUDE_DIRS})
target_link_libraries(xmbench_preload PRIVATE ${CMAKE_DL_LIBS})
set_target_properties(xmbench_preload PROPERTIES
    PREFIX lib OUTPUT_NAME xmbench_preload)

add_executable(xmbench EXCLUDE_FROM_ALL xmbench.c)
target_include_directories(xmbench PRIVATE
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/lib
)
target_compile_definitions(xmbench PRIVATE
    XMBENCH_PRELOAD="$<TARGET_FILE:xmbench_preload>")
target_link_libraries(xmbench PRIVATE Xm ${CMAKE_DL_LIBS})
add_dependencies(xmbench xmbench_preload)

set(XMBENCH_ARGS "" CACHE STRING "Arguments passed to xmbench by the bench target")
separate_arguments(_xmbench_args UNIX_COMMAND "${XMBENCH_ARGS}")

# The case table has X cases; run them under their own Xvfb when
# xvfb-run is available (XVFB_RUN_EXECUTABLE comes from src/tests).  The
# local input method makes XmIm create input contexts, so the spot
# location cases reach XSetICValues.
if(XVFB_RUN_EXECUTABLE)
    set(_xmbench_cmd ${XVFB_RUN_EXECUTABLE} -a
        -s "-screen 0 1920x1080x24 +extension RENDER")
else()
    set(_xmbench_cmd)
endif()
add_custom_target(bench
    COMMAND ${CMAKE_COMMAND} -E env XMODIFIERS=@im=none
            ${_xmbench_cmd} $<TARGET_FILE:xmbench>
            -j ${CMAKE_CURRENT_BINARY_DIR}/xmbench.json ${_xmbench_args}
    DEPENDS xmbench xmbench_preload
    USES_TERMINAL
    COMMENT "Running xmbench")

# mwmbench: macro-benchmarks for mwm, which it starts as the window
# manager of the bench target's display and drives through XTest.  It
# needs libXtst, and preloads libmwmbench_preload.so into mwm.  The bench
# target writes <build>/src/tests/bench/mwmbench.json; MWMBENCH_ARGS
# passes extra arguments (for example "-r 3 map title").
pkg_check_modules(MWMBENCH_XTST IMPORTED_TARGET xtst)
if(TARGET mwm AND MWMBENCH_XTST_FOUND)
    add_library(mwmbench_preload MODULE EXCLUDE_FROM_ALL mwmpreload.c)
    target_include_directories(mwmbench_preload PRIVATE ${MOTIF_XT_INCLUDE_DIRS})
    target_link_libraries(mwmbench_preload PRIVATE ${CMAKE_DL_LIBS})
    set_target_properties(mwmbench_preload PROPERTIES
        PREFIX lib OUTPUT_NAME mwmbench_preload)

    add_executable(mwmbench EXCLUDE_FROM_ALL mwmbench.c)
    target_compile_definitions(mwmbench PRIVATE
        MWMBENCH_MWM="$<TARGET_FILE:mwm>"
        MWMBENCH_PRELOAD="$<TARGET_FILE:mwmbench_preload>")
    target_link_libraries(mwmbench PRIVATE
        PkgConfig::MOTIF_XT PkgConfig::MWMBENCH_XTST)
    add_dependencies(mwmbench mwmbench_preload mwm)

    set(MWMBENCH_ARGS "" CACHE STRING
        "Arguments passed to mwmbench by the bench target")
    separate_arguments(_mwmbench_args UNIX_COMMAND "${MWMBENCH_ARGS}")
    add_custom_target(bench-mwm
        COMMAND ${_xmbench_cmd} $<TARGET_FILE:mwmbench>
                -j ${CMAKE_CURRENT_BINARY_DIR}/mwmbench.json ${_mwmbench_args}
        DEPENDS mwmbench mwmbench_preload mwm
        USES_TERMINAL
        COMMENT "Running mwmbench")
    add_dependencies(bench bench-mwm)
else()
    message(STATUS "Benchmarks: mwmbench needs mwm and libXtst; not built")
endif()
