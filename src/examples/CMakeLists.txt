# Demos - Motif demonstration programs
# These programs showcase various Motif widgets and features

if(WITH_DEMOS)
    message(STATUS "Building Motif examples")
    
    # Add example subdirectories
    add_subdirectory(lib)
    add_subdirectory(programs)
    
    # Optional: add unsupported examples
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/examples/unsupported/CMakeLists.txt")
        add_subdirectory(unsupported)
    endif()
    
    # Optional: add example documentation
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/examples/doc/CMakeLists.txt")
        add_subdirectory(doc)
    endif()
    
else()
    message(STATUS "Examples disabled - use -DWITH_DEMOS=ON to build examples")
endif()
