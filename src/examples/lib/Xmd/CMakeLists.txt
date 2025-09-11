# Xmd demo library
# Common utilities for Motif demos

# Xmd library sources
set(XMD_SRCS
    Help.c
    Menus.c
    Print.c
    RegEdit.c
    AdjView.c
)

# Xmd headers
set(XMD_HEADERS
    AdjView.h
    Help.h
    HelpP.h
    Menus.h
    Print.h
    PrintP.h
    RegEdit.h
    RegEditI.h
)

# Create the Xmd library
add_library(Xmd STATIC ${XMD_SRCS})

# Set include directories
target_include_directories(Xmd PUBLIC
    ${CMAKE_SOURCE_DIR}/src/lib
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${X_INCLUDE_DIRS}
)

# Link with Motif libraries
target_link_libraries(Xmd
    Xm
    ${X_LIBRARIES}
)

# Create onHelp utility program
add_executable(onHelp onHelp.c)
target_link_libraries(onHelp Xmd)

# Install the library (for development)
install(TARGETS Xmd
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install headers (for development)
install(FILES ${XMD_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Xmd
)
