# Wsm demo library
# Workspace management utilities for Motif demos

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

# Wsm headers
set(WSM_HEADERS
    wsm_funcs.h
    wsm_proto.h
    utm_send.h
)

# Create the WsmDemo library (renamed to avoid conflict with WmWsmLib)
add_library(WsmDemo STATIC ${WSM_SRCS})

# Set include directories
target_include_directories(WsmDemo PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${X_INCLUDE_DIRS}
)

# Install the library (for development)
install(TARGETS WsmDemo
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install headers (for development)
install(FILES ${WSM_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Wsm
)
