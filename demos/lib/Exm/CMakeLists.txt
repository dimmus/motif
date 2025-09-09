# Exm demo library
# Extended Motif widgets for demos

# Exm library sources
set(EXM_SRCS
    CommandB.c
    Grid.c
    Panner.c
    MenuB.c
    Simple.c
    ExmString.c
    StringTrans.c
    TabB.c
)

# ExmMrm library sources
set(EXMMRM_SRCS
    ExmMrm.c
)

# Exm headers
set(EXM_HEADERS
    CommandB.h
    CommandBP.h
    ExmMrm.h
    Grid.h
    GridP.h
    MenuB.h
    MenuBP.h
    Panner.h
    PannerP.h
    Simple.h
    SimpleP.h
    ExmString.h
    ExmStringP.h
    StringTrans.h
    StringTransP.h
    TabB.h
    TabBP.h
)

# Create the Exm library
add_library(Exm STATIC ${EXM_SRCS})

# Create the ExmMrm library
add_library(ExmMrm STATIC ${EXMMRM_SRCS})

# Set include directories for both libraries
target_include_directories(Exm PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/demos/lib
    ${X_INCLUDE_DIRS}
)

target_include_directories(ExmMrm PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/demos/lib
    ${X_INCLUDE_DIRS}
)

# Link ExmMrm with Mrm library
target_link_libraries(ExmMrm
    Mrm
    ${X_LIBRARIES}
)

# Add wml subdirectory if it exists
if(EXISTS "${CMAKE_SOURCE_DIR}/demos/lib/Exm/wml/CMakeLists.txt")
    add_subdirectory(wml)
endif()

# Install the libraries (for development)
install(TARGETS Exm ExmMrm
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install headers (for development)
install(FILES ${EXM_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Exm
)
