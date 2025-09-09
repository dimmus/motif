# Demos - Motif demonstration programs
# These programs showcase various Motif widgets and features

if(ENABLE_DEMOS)
    message(STATUS "Building Motif demos")
    
    # Add demo subdirectories
    add_subdirectory(lib)
    add_subdirectory(programs)
    
    # Optional: add unsupported demos
    if(EXISTS "${CMAKE_SOURCE_DIR}/demos/unsupported/CMakeLists.txt")
        add_subdirectory(unsupported)
    endif()
    
    # Optional: add demo documentation
    if(EXISTS "${CMAKE_SOURCE_DIR}/demos/doc/CMakeLists.txt")
        add_subdirectory(doc)
    endif()
    
else()
    message(STATUS "Demos disabled - use -DENABLE_DEMOS=ON to build demos")
endif()
