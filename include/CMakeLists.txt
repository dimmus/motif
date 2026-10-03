# The Xm headers are installed from the explicit lists in src/lib/Xm.

# Install Dt headers
install(DIRECTORY Dt/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Dt
    FILES_MATCHING PATTERN "*.h"
)

# Create include directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)

# Generate config.h
configure_file(
    ${CMAKE_SOURCE_DIR}/include/config.h.in
    ${CMAKE_BINARY_DIR}/include/config.h
    @ONLY
)

# Xm/Xm.h is generated in src/lib/Xm.