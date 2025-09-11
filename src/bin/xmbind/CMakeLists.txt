add_executable(xmbind xmbind.c)

target_include_directories(xmbind PRIVATE
    ${CMAKE_SOURCE_DIR}/src/lib
    ${CMAKE_BINARY_DIR}/src/lib
    ${X_INCLUDE_DIRS}
)

target_link_libraries(xmbind Xm ${X_LIBRARIES})

install(TARGETS xmbind
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
