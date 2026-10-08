# Two threads, each with its own XtAppContext and Display, building and
# destroying Motif widget trees at the same time.  In a WITH_TSAN build
# ThreadSanitizer fails the test on any data race it sees in Motif (see
# tsan.supp for what is suppressed in libX11 and libXt, and why).

find_package(Threads REQUIRED)
add_executable(mtapps mtapps.c)
target_link_libraries(mtapps PRIVATE Mrm Xm Threads::Threads)

# Each thread also loads hellomotif.uid with Mrm; the uil tests compile it.
set(_uid_id examples.programs.hellomotif.hellomotif)
set(MOTIF_TEST_WORKDIR ${CMAKE_CURRENT_BINARY_DIR})
motif_x_test(Threads.mtapps $<TARGET_FILE:mtapps> 3 2
    ${CMAKE_BINARY_DIR}/src/tests/uil/uid/${_uid_id}.uid)
set_tests_properties(Threads.mtapps PROPERTIES TIMEOUT 600
    FIXTURES_REQUIRED uid.${_uid_id})
if(WITH_TSAN)
    set_tests_properties(Threads.mtapps PROPERTIES ENVIRONMENT
        "TSAN_OPTIONS=suppressions=${CMAKE_CURRENT_SOURCE_DIR}/tsan.supp:halt_on_error=0:second_deadlock_stack=1")
endif()

# Two threads sharing one application context and display: the lock
# order between Xt's application and process locks.
add_executable(sharedapp sharedapp.c)
target_link_libraries(sharedapp PRIVATE Xm Threads::Threads)
motif_x_test(Threads.sharedapp $<TARGET_FILE:sharedapp> 3000)
set_tests_properties(Threads.sharedapp PROPERTIES TIMEOUT 600)
if(WITH_TSAN)
    set_tests_properties(Threads.sharedapp PROPERTIES ENVIRONMENT
        "TSAN_OPTIONS=suppressions=${CMAKE_CURRENT_SOURCE_DIR}/tsan.supp:halt_on_error=0:second_deadlock_stack=1")
endif()

motif_test_set_env()
