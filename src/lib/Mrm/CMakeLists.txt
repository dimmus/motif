# Mrm library sources
set(MRM_SRCS
    MrmIbuffer.c
    MrmIentry.c
    MrmIfile.c
    MrmIheader.c
    MrmIindex.c
    MrmIindexw.c
    MrmIrid.c
    MrmIswap.c
    MrmItop.c
    MrmItopw.c
    Mrmappl.c
    Mrmcontext.c
    Mrmerror.c
    Mrmhier.c
    Mrmicon.c
    Mrminit.c
    Mrmlread.c
    Mrmlwrite.c
    Mrmmodule.c
    Mrmos.c
    Mrmptrlist.c
    Mrmtable.c
    Mrmtime.c
    Mrmvm.c
    Mrmwci.c
    Mrmwcrw.c
    Mrmwcrwr.c
    Mrmwread.c
    Mrmwrefs.c
    Mrmwvalues.c
    Mrmwwrite.c
    MrmMessages.c
)

# Mrm headers
set(MRM_HEADERS
    MrmDecls.h
    MrmPublic.h
    MrmAppl.h
    MrmosI.h
)

# Internal headers
set(MRM_INTERNAL_HEADERS
    IDB.h
    Mrm.h
    MrmMsgI.h
    MrmWidget.h
)

# Generate message catalog header file
if(WITH_MESSAGE_CATALOG)
    motif_add_message_catalog(mrm_msgcat_header Mrm
        ${CMAKE_CURRENT_SOURCE_DIR}/Mrm.msg
        ${CMAKE_BINARY_DIR}/include/Mrm/MrmMsgCatI.h
    )
else()
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/include/Mrm/MrmMsgCatI.h
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/Mrm
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/src/lib/Mrm/MrmMsgCatI.h
            ${CMAKE_BINARY_DIR}/include/Mrm/MrmMsgCatI.h
        COMMENT "Copying Mrm message catalog stub"
    )
    add_custom_target(mrm_msgcat_header ALL DEPENDS ${CMAKE_BINARY_DIR}/include/Mrm/MrmMsgCatI.h)
endif()

# Create the library
add_library(Mrm ${MOTIF_LIB_TYPE} ${MRM_SRCS})

# Ensure message catalog header is generated before building this target
add_dependencies(Mrm mrm_msgcat_header)

# Set library properties
set_target_properties(Mrm PROPERTIES
    VERSION ${ABI_CURRENT}.${ABI_REVISION}.${ABI_AGE}
    SOVERSION ${ABI_CURRENT}
    OUTPUT_NAME "Mrm"
)

# Include directories
target_include_directories(Mrm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/lib/Mrm>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/lib/Mrm
        ${CMAKE_SOURCE_DIR}/src/lib/Xm
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_BINARY_DIR}/include/Mrm
        ${X_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(Mrm
    PUBLIC
        Xm
        ${X_LIBRARIES}
)

# Add conditional libraries
if(XFT_FOUND)
    target_link_libraries(Mrm PRIVATE ${XFT_LIBRARIES})
    target_include_directories(Mrm PRIVATE ${XFT_INCLUDE_DIRS})
endif()

# Install the library
install(TARGETS Mrm
    EXPORT MrmTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES ${MRM_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Mrm
)

# Install internal headers (for development)
install(FILES ${MRM_INTERNAL_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Mrm
)

# Export targets
install(EXPORT MrmTargets
    FILE MrmTargets.cmake
    NAMESPACE Mrm::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Mrm
)
