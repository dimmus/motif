# The uil program: the UIL compiler objects shared with libUil (UilCompiler,
# in src/lib/Uil) and UilMain.c with main().  It does not link libUil,
# whose UilMain.c is built as the callable Uil() interface instead.
add_executable(uil UilMain.c $<TARGET_OBJECTS:UilCompiler>)
add_dependencies(uil UilCompiler)
motif_uil_compiler_setup(uil)
target_link_libraries(uil PRIVATE Mrm Xm m)

if(WITH_TESTS)
    # Pathological inputs; no display is needed
    add_test(NAME uil_robustness
        COMMAND ${CMAKE_COMMAND}
            -DUIL=$<TARGET_FILE:uil>
            -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/uil_robustness
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/uil_robustness.cmake)
endif()

# Install executables
install(TARGETS uil
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install sample UIL file
install(FILES
    XmAppl.uil
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/uil
)