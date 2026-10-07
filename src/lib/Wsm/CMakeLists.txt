# Wsm library sources
set(WSM_SRCS
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

target_link_libraries(Wsm PRIVATE motif_build_flags PkgConfig::MOTIF_XT)

# libWsm is linked into mwm and is not installed.
