# The uil program: the UIL compiler objects shared with libUil (UilCompiler,
# in src/lib/Uil) and UilMain.c with main().  It does not link libUil,
# whose UilMain.c is built as the callable Uil() interface instead.
add_executable(uil UilMain.c $<TARGET_OBJECTS:UilCompiler>)
add_dependencies(uil UilCompiler)
motif_uil_compiler_setup(uil)
target_link_libraries(uil PRIVATE Mrm Xm m)

# Install executables
install(TARGETS uil
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install sample UIL file
install(FILES
    XmAppl.uil
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/uil
)