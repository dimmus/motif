# A/B harness for the Form, Container and List layout code: not tests.
# Nothing here is built by default or registered with CTest, because a
# comparison needs two builds of libXm.  Build the programs with
#
#   cmake --build <build> --target ab
#
# and compare two libraries with ab.sh; see README.md.

add_executable(xm_abtest EXCLUDE_FROM_ALL abtest.c)
target_include_directories(xm_abtest PRIVATE ${CMAKE_BINARY_DIR}/include)
target_link_libraries(xm_abtest PRIVATE Xm)

add_executable(xm_layoutbench EXCLUDE_FROM_ALL layoutbench.c)
target_include_directories(xm_layoutbench PRIVATE ${CMAKE_BINARY_DIR}/include)
target_link_libraries(xm_layoutbench PRIVATE Xm ${CMAKE_DL_LIBS})

add_executable(xm_shadowbench EXCLUDE_FROM_ALL shadowbench.c)
target_include_directories(xm_shadowbench PRIVATE ${CMAKE_BINARY_DIR}/include)
target_link_libraries(xm_shadowbench PRIVATE Xm)

add_custom_target(ab DEPENDS xm_abtest xm_layoutbench xm_shadowbench)
