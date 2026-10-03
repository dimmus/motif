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
