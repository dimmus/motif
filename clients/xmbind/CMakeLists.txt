add_executable(xmbind xmbind.c)

target_include_directories(xmbind PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_BINARY_DIR}/lib
    ${X_INCLUDE_DIRS}
)

target_link_libraries(xmbind Xm ${X_LIBRARIES})

install(TARGETS xmbind
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
