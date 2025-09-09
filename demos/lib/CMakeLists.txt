# Demo libraries
# Libraries used by demo programs

# Add demo library subdirectories
add_subdirectory(Xmd)
add_subdirectory(Wsm)
add_subdirectory(Exm)

# Optional: add other demo libraries if they exist
if(EXISTS "${CMAKE_SOURCE_DIR}/demos/lib/ExmCxx/CMakeLists.txt")
    add_subdirectory(ExmCxx)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/demos/lib/XmCxx/CMakeLists.txt")
    add_subdirectory(XmCxx)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/demos/lib/uil/CMakeLists.txt")
    add_subdirectory(uil)
endif()
