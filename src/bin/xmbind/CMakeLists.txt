add_executable(xmbind xmbind.c)

target_link_libraries(xmbind Xm)

install(TARGETS xmbind
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
