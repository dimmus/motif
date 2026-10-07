# A/B harness for the Form, Container and List layout code and for the
# round trips of the ScrollBar autorepeat: not tests.
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

set(MOTIF_AB_TARGETS xm_abtest xm_layoutbench)

# The ScrollBar autorepeat timing presses the button with XTest.
pkg_check_modules(MOTIF_XTST IMPORTED_TARGET xtst)
if(MOTIF_XTST_FOUND)
    add_executable(xm_repeatbench EXCLUDE_FROM_ALL repeatbench.c)
    target_include_directories(xm_repeatbench PRIVATE ${CMAKE_BINARY_DIR}/include)
    target_link_libraries(xm_repeatbench PRIVATE Xm PkgConfig::MOTIF_XTST)
    list(APPEND MOTIF_AB_TARGETS xm_repeatbench)
endif()

add_custom_target(ab DEPENDS ${MOTIF_AB_TARGETS})
