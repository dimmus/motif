# WmWsmLib sources
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
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_BINARY_DIR}/lib
    ${CMAKE_BINARY_DIR}/include
    ${X_INCLUDE_DIRS}
)
