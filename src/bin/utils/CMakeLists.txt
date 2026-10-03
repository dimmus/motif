# Build-time generators.  A cross build imports them from a native build
# instead (MOTIF_HOST_TOOLS).
if(MOTIF_HOST_TOOLS)
    return()
endif()

# Build makestrs utility
add_executable(makestrs makestrs.c)

# Build mkcatdefs utility
add_executable(mkcatdefs mkcatdefs.c)

# Build mkmsgcat utility
add_executable(mkmsgcat mkmsgcat.c)

foreach(_tool makestrs mkcatdefs mkmsgcat)
    target_link_libraries(${_tool} PRIVATE motif_build_flags)
endforeach()

# These are build-time generators and are not installed.
