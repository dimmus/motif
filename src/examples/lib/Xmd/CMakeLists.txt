# Xmd example library
# Common utilities for Motif examples

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
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link with Motif libraries
target_link_libraries(Xmd PUBLIC Xm)

# Ensure string generation happens before building Xmd
add_dependencies(Xmd xm_strings)

# Create onHelp utility program
add_executable(onHelp onHelp.c)
target_link_libraries(onHelp PRIVATE Xmd)

# Only the example programs use this library; it is not installed.
