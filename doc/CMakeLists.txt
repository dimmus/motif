# Documentation installation
# Install man pages and other documentation

# Install man1 pages (executables)
file(GLOB MAN1_PAGES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/doc/man/man1/*.1")
if(MAN1_PAGES)
    install(FILES ${MAN1_PAGES}
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
    )
endif()

# Install man3 pages (library functions)
file(GLOB MAN3_PAGES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/doc/man/man3/*.3")
if(MAN3_PAGES)
    install(FILES ${MAN3_PAGES}
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man3
    )
endif()

# Install man4 pages (if any)
file(GLOB MAN4_PAGES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/doc/man/man4/*.4")
if(MAN4_PAGES)
    install(FILES ${MAN4_PAGES}
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man4
    )
endif()

# Install man5 pages (if any)
file(GLOB MAN5_PAGES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/doc/man/man5/*.5")
if(MAN5_PAGES)
    install(FILES ${MAN5_PAGES}
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man5
    )
endif()

# Install the other documentation, and the project files that users and
# packagers look for next to it (the README is installed by the top-level
# CMakeLists.txt).
file(GLOB DOC_FILES "${CMAKE_SOURCE_DIR}/doc/*.md")
install(FILES
    ${DOC_FILES}
    ${CMAKE_SOURCE_DIR}/AUTHORS
    ${CMAKE_SOURCE_DIR}/CHANGELOG.md
    ${CMAKE_SOURCE_DIR}/SECURITY.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/motif
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/doc/guide"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/motif
    FILES_MATCHING PATTERN "*.md"
)

# doc.manpages.<library>: every function that a shared library exports and
# its installed headers declare must have a page in man/man3 (see
# tools/cmake/scripts/check_manpages.cmake).  The libraries and their
# headers are those of motif_exports_test().
if(WITH_TESTS AND CMAKE_NM)
    get_property(_libraries GLOBAL PROPERTY MOTIF_PUBLIC_LIBRARIES)
    foreach(_lib IN LISTS _libraries)
        get_target_property(_type ${_lib} TYPE)
        if(NOT _type STREQUAL "SHARED_LIBRARY")
            continue()
        endif()
        get_property(_headers GLOBAL PROPERTY MOTIF_PUBLIC_HEADERS_${_lib})
        set(_args ${CMAKE_CURRENT_BINARY_DIR}/${_lib}_manpages_test.cmake)
        file(GENERATE OUTPUT ${_args} CONTENT
"set(NM \"${CMAKE_NM}\")
set(LIBRARY \"$<TARGET_FILE:${_lib}>\")
set(HEADERS \"${_headers}\")
set(MANDIR \"${CMAKE_CURRENT_SOURCE_DIR}/man\")
")
        add_test(NAME doc.manpages.${_lib}
            COMMAND ${CMAKE_COMMAND} -DARGS=${_args}
                -P ${CMAKE_SOURCE_DIR}/tools/cmake/scripts/check_manpages.cmake)
        set_tests_properties(doc.manpages.${_lib} PROPERTIES LABELS doc)
    endforeach()
endif()
