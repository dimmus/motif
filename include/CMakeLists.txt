# Install Xm headers from src/lib/Xm directory
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/lib/Xm/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Xm
    FILES_MATCHING PATTERN "*.h"
)

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

# Create Xm subdirectory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/Xm)

# Generate Xm.h
configure_file(
    ${CMAKE_SOURCE_DIR}/src/lib/Xm/Xm.h.in
    ${CMAKE_BINARY_DIR}/include/Xm/Xm.h
    @ONLY
)