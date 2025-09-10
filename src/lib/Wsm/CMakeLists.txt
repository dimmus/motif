# Wsm library sources
set(WSM_SRCS
    debug.c
    disp.c
    free.c
    pack.c
    recv.c
    send.c
    util.c
    utm_send.c
)

# Create Wsm library
add_library(Wsm STATIC ${WSM_SRCS})

# Ensure Xm string files are generated before building this target
add_dependencies(Wsm xm_strings)

target_include_directories(Wsm PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/lib
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/lib
    ${X_INCLUDE_DIRS}
)

# Install library
install(TARGETS Wsm
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install headers
install(FILES
    wsm_funcs.h
    wsm_proto.h
    utm_send.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/wsm
)