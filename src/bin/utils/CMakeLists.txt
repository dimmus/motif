# Build makestrs utility
add_executable(makestrs makestrs.c)

# Build mkcatdefs utility
add_executable(mkcatdefs mkcatdefs.c)

# Build mkmsgcat utility  
add_executable(mkmsgcat mkmsgcat.c)

foreach(_tool makestrs mkcatdefs mkmsgcat)
    target_link_libraries(${_tool} motif_build_flags)
endforeach()

# These are build-time generators and are not installed.
