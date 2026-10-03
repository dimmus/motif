# The Xm headers are installed from the explicit lists in src/lib/Xm.

# The Dt headers are only here so that uil, wml and mwm can be built with
# CDE support (see Dt/README); CDE provides the real ones.  Do not install
# them.

# Create include directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)

# Generate config.h
configure_file(
    ${CMAKE_SOURCE_DIR}/include/config.h.in
    ${CMAKE_BINARY_DIR}/include/config.h
    @ONLY
)

# Xm/Xm.h is generated in src/lib/Xm.
