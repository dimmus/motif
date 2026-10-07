# Tests that drive real input (XTest, through xdotool) into Motif
# programs on an X server.  Each needs xdotool; without it, or without a
# display, they are reported as skipped.

add_executable(textapp textapp.c)
target_link_libraries(textapp PRIVATE Xm)

set(MOTIF_TEST_WORKDIR ${CMAKE_CURRENT_BINARY_DIR})
motif_x_test(Text.xdotool
    sh ${CMAKE_CURRENT_SOURCE_DIR}/text_xdotool.sh $<TARGET_FILE:textapp>)
set_tests_properties(Text.xdotool PROPERTIES LABELS "X11;Interactive")

# mwm end-to-end: parse test.mwmrc and a malformed .mwmrc, manage a
# client in a nested Xephyr, drive f.* through key bindings, and survive
# a hostile client.  Needs Xephyr and xdotool as well.
add_executable(wmclient wmclient.c)
target_link_libraries(wmclient PRIVATE PkgConfig::MOTIF_XT)
if(TARGET mwm)
    motif_x_test(Mwm.xdotool
        sh ${CMAKE_CURRENT_SOURCE_DIR}/mwm_xdotool.sh
        $<TARGET_FILE:mwm> $<TARGET_FILE:wmclient>)
    set_tests_properties(Mwm.xdotool PROPERTIES LABELS "X11;Interactive"
        RESOURCE_LOCK xvfb TIMEOUT 180)

    # mwm as the window manager of the test display: focus timestamps,
    # the properties it reads when it manages a window and later,
    # _NET_CLIENT_LIST, and the title after many changes in a row.
    add_executable(mwm_tests testMwm.c)
    target_link_libraries(mwm_tests PRIVATE PkgConfig::CHECK
        PkgConfig::MOTIF_XT)
    motif_x_test(Mwm.manage $<TARGET_FILE:mwm_tests> $<TARGET_FILE:mwm>)
    set_tests_properties(Mwm.manage PROPERTIES TIMEOUT 180)
endif()

motif_test_set_env()
