# Build makestrs utility
add_executable(makestrs makestrs.c)

# Build mkcatdefs utility
add_executable(mkcatdefs mkcatdefs.c)

# Build mkmsgcat utility  
add_executable(mkmsgcat mkmsgcat.c)

# Install utilities
install(TARGETS makestrs mkcatdefs mkmsgcat
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
