# Visual regression: render a fixed scene with a bitmap font and compare
# it to a committed golden PNG.  Needs libpng (for the capture), bdftopcf
# and mkfontdir (to build the font the scene uses from the BDF fonts in
# ../environment/fonts), and an X server (run under xvfb-run on a 24-bit
# screen so the pixels are reproducible).

find_program(BDFTOPCF_EXECUTABLE bdftopcf)
find_program(MKFONTDIR_EXECUTABLE mkfontdir)
mark_as_advanced(BDFTOPCF_EXECUTABLE MKFONTDIR_EXECUTABLE)

if(NOT MOTIF_PNG_FOUND)
    message(STATUS "Tests: visual regression needs libpng; skipped")
    return()
endif()
if(NOT BDFTOPCF_EXECUTABLE OR NOT MKFONTDIR_EXECUTABLE)
    message(STATUS "Tests: visual regression needs bdftopcf and mkfontdir; skipped")
    return()
endif()

# Compile the BDF fonts to PCF and build a fonts.dir, so the X server can
# serve them from ${_fontdir} without touching the system font path.
set(_bdf_dir ${CMAKE_CURRENT_SOURCE_DIR}/../environment/fonts/bdf)
set(_fontdir ${CMAKE_CURRENT_BINARY_DIR}/fonts)
file(GLOB _bdfs CONFIGURE_DEPENDS ${_bdf_dir}/*.bdf)
set(_pcfs "")
file(MAKE_DIRECTORY ${_fontdir})
foreach(_bdf IN LISTS _bdfs)
    get_filename_component(_name ${_bdf} NAME_WE)
    set(_pcf ${_fontdir}/${_name}.pcf)
    add_custom_command(OUTPUT ${_pcf}
        COMMAND ${BDFTOPCF_EXECUTABLE} -o ${_pcf} ${_bdf}
        DEPENDS ${_bdf}
        VERBATIM)
    list(APPEND _pcfs ${_pcf})
endforeach()
add_custom_command(OUTPUT ${_fontdir}/fonts.dir
    COMMAND ${MKFONTDIR_EXECUTABLE} ${_fontdir}
    DEPENDS ${_pcfs}
    VERBATIM)
add_custom_target(motif_test_fonts ALL DEPENDS ${_fontdir}/fonts.dir)

add_executable(vizshot vizshot.c)
target_include_directories(vizshot PRIVATE
    ${CMAKE_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}/src/lib)
target_link_libraries(vizshot PRIVATE Xm PkgConfig::MOTIF_PNG)
add_dependencies(vizshot motif_test_fonts)

set(MOTIF_TEST_WORKDIR ${CMAKE_CURRENT_BINARY_DIR})
set(_golden ${CMAKE_CURRENT_SOURCE_DIR}/golden/scene.png)

# Rendering must be identical across two runs.
motif_x_test(Visual.deterministic
    $<TARGET_FILE:vizshot> --fontpath ${_fontdir} --selftest)
# ... and match the committed golden, within a small tolerance for
# cross-build PCF/rounding differences.
motif_x_test(Visual.golden
    $<TARGET_FILE:vizshot> --fontpath ${_fontdir} --check ${_golden}
    --tolerance 80)
set_tests_properties(Visual.deterministic Visual.golden PROPERTIES
    LABELS "X11;Visual")

# Regenerate the golden from the current code:
#   cmake --build <build> --target update-golden
add_custom_target(update-golden
    COMMAND ${XVFB_RUN_EXECUTABLE} ${XVFB_RUN_ARGS}
            $<TARGET_FILE:vizshot> --fontpath ${_fontdir} --out ${_golden}
    DEPENDS vizshot
    COMMENT "Regenerating ${_golden}")

motif_test_set_env()
