# End-to-end hostile-client test: a Motif application (victim), built
# here with whatever sanitizers the top-level build enabled, is driven
# over a deliberately malformed X client (hostile_peer) -- a drop site,
# a drag source, a clipboard owner and the owner of the root drag and
# bindings properties.  Real drags are driven with xdotool.  Needs an X
# server and xdotool; skips (77) without them.

add_executable(hostile_victim victim.c)
target_link_libraries(hostile_victim PRIVATE Xm)

add_executable(hostile_peer hostile_peer.c)
target_link_libraries(hostile_peer PRIVATE PkgConfig::MOTIF_XT)

set(MOTIF_TEST_WORKDIR ${CMAKE_CURRENT_BINARY_DIR})
motif_x_test(Hostile.client
    sh ${CMAKE_CURRENT_SOURCE_DIR}/hostile.sh
    $<TARGET_FILE:hostile_victim> $<TARGET_FILE:hostile_peer>)
set_tests_properties(Hostile.client PROPERTIES LABELS "X11;Interactive"
    RESOURCE_LOCK xvfb TIMEOUT 240)

motif_test_set_env()
