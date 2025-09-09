# Documentation installation
# Install man pages and other documentation

# Install man1 pages (executables)
file(GLOB MAN1_PAGES "${CMAKE_SOURCE_DIR}/doc/man/man1/*.1")
if(MAN1_PAGES)
    install(FILES ${MAN1_PAGES}
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
    )
endif()

# Install man3 pages (library functions)
file(GLOB MAN3_PAGES "${CMAKE_SOURCE_DIR}/doc/man/man3/*.3")
if(MAN3_PAGES)
    install(FILES ${MAN3_PAGES}
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man3
    )
endif()

# Install man4 pages (if any)
file(GLOB MAN4_PAGES "${CMAKE_SOURCE_DIR}/doc/man/man4/*.4")
if(MAN4_PAGES)
    install(FILES ${MAN4_PAGES}
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man4
    )
endif()

# Install man5 pages (if any)
file(GLOB MAN5_PAGES "${CMAKE_SOURCE_DIR}/doc/man/man5/*.5")
if(MAN5_PAGES)
    install(FILES ${MAN5_PAGES}
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man5
    )
endif()

# Install other documentation files
file(GLOB DOC_FILES 
    "${CMAKE_SOURCE_DIR}/doc/*.md"
    "${CMAKE_SOURCE_DIR}/doc/*.txt"
)
if(DOC_FILES)
    install(FILES ${DOC_FILES}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/motif
    )
endif()

# Install guide directory if it exists
if(EXISTS "${CMAKE_SOURCE_DIR}/doc/guide")
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/doc/guide"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/motif
        FILES_MATCHING PATTERN "*.html" PATTERN "*.txt" PATTERN "*.pdf"
    )
endif()
