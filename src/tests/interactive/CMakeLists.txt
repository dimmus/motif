# Tests that drive real input (XTest, through xdotool) into Motif
# programs on an X server.  Each needs xdotool; without it, or without a
# display, they are reported as skipped.

add_executable(textapp textapp.c)
target_link_libraries(textapp PRIVATE Xm)

set(MOTIF_TEST_WORKDIR ${CMAKE_CURRENT_BINARY_DIR})
motif_x_test(Text.xdotool
    sh ${CMAKE_CURRENT_SOURCE_DIR}/text_xdotool.sh $<TARGET_FILE:textapp>)
set_tests_properties(Text.xdotool PROPERTIES LABELS "X11;Interactive")

motif_test_set_env()
