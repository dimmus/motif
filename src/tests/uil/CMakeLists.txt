# UIL -> UID -> Mrm round trip.
#
# Every .uil file in the tree is compiled with the uil built here
# (Uil.compile.<file>, no display needed); the .uid it produces is then
# loaded with Mrm, from the file and from a memory buffer, every widget
# and literal in it fetched and the widgets realized (Uil.load.<file>
# and Uil.loadbuffer.<file>, which need an X server).

add_executable(uidload uidload.c)
target_include_directories(uidload PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/src/lib
    ${CMAKE_BINARY_DIR}/include
)
target_link_libraries(uidload PRIVATE Mrm Xm)

# Files that are not compiled, and why.
set(MOTIF_UIL_SKIP
    # Includes periodic_l.uil, which is a module of its own; the examples
    # build leaves it out as well.
    examples/programs/periodic/periodic.uil
    # Includes menus.uih, which is not in the tree.
    examples/unsupported/cstext/texteditor/textedit.uil
    # Uses the Exm demo widgets, which need Exm.wmd (built with the demos).
    examples/programs/Exm/app_in_uil/exm_in_uil.uil
    # Declares an argument of type "callback", which uil does not accept
    # ("unexpected NAME token"); nothing in the tree includes it.
    examples/programs/Exm/Exm.uil
)

# Files that are compiled but not loaded, and why.
set(MOTIF_UIL_NO_LOAD
    # Its widgets are instances of the demo's own Dog and Square classes.
    examples.unsupported.dogs.dogs
)

# Other .uid files a file is loaded with, for literals it imports.
set(MOTIF_UIL_LOAD_WITH_examples.programs.animate.animate
    examples.programs.animate.dog)
set(MOTIF_UIL_LOAD_WITH_examples.programs.animate.xmanimate
    examples.programs.animate.plane)

# Resource checks for uidload, by file.
set(MOTIF_UIL_CHECKS_examples.programs.hellomotif.hellomotif
    helloworld_button:x=15
    helloworld_button:y=60
    "helloworld_button:labelString=\"Hello\nWorld!\""
)

file(GLOB_RECURSE MOTIF_UIL_FILES CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/*.uil)
list(SORT MOTIF_UIL_FILES)

set(_work ${CMAKE_CURRENT_BINARY_DIR}/uid)
file(MAKE_DIRECTORY ${_work})
foreach(_rel IN LISTS MOTIF_UIL_FILES)
    if(_rel IN_LIST MOTIF_UIL_SKIP OR _rel MATCHES "^tests/")
        continue()
    endif()
    set(_src ${CMAKE_SOURCE_DIR}/src/${_rel})
    get_filename_component(_dir ${_src} DIRECTORY)
    string(REGEX REPLACE "\\.uil$" "" _id "${_rel}")
    string(REPLACE "/" "." _id "${_id}")
    set(_uid ${_work}/${_id}.uid)

    # Files without a module header are fragments for inclusion.
    file(STRINGS ${_src} _module REGEX "^[ \t]*[Mm][Oo][Dd][Uu][Ll][Ee][ \t]")
    if(_module)
        set(_fragment OFF)
    else()
        set(_fragment ON)
    endif()

    add_test(NAME Uil.compile.${_id}
        COMMAND ${CMAKE_COMMAND}
            -DUIL=$<TARGET_FILE:${MOTIF_HOST_TOOL_PREFIX}uil>
            -DSOURCE=${_src}
            -DOUTPUT=${_uid}
            "-DINCLUDES=${_dir},${CMAKE_SOURCE_DIR}/src/bin/uil"
            -DFRAGMENT=${_fragment}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/uil_compile.cmake)
    set_tests_properties(Uil.compile.${_id} PROPERTIES
        FIXTURES_SETUP uid.${_id} LABELS Uil)

    if(_fragment OR _id IN_LIST MOTIF_UIL_NO_LOAD)
        continue()
    endif()
    set(MOTIF_TEST_WORKDIR ${_work})
    set(_with "")
    set(_fixtures uid.${_id})
    foreach(_other IN LISTS MOTIF_UIL_LOAD_WITH_${_id})
        list(APPEND _with ${_work}/${_other}.uid)
        list(APPEND _fixtures uid.${_other})
    endforeach()
    motif_x_test(Uil.load.${_id}
        $<TARGET_FILE:uidload> --procs ${_src} ${_uid} ${_with}
        ${MOTIF_UIL_CHECKS_${_id}})
    set_tests_properties(Uil.load.${_id} PROPERTIES
        FIXTURES_REQUIRED "${_fixtures}" LABELS "X11;Uil")
    # MrmOpenHierarchyFromBuffer takes a single file
    if(NOT _with)
        motif_x_test(Uil.loadbuffer.${_id}
            $<TARGET_FILE:uidload> --buffer --procs ${_src} ${_uid}
            ${MOTIF_UIL_CHECKS_${_id}})
        set_tests_properties(Uil.loadbuffer.${_id} PROPERTIES
            FIXTURES_REQUIRED "${_fixtures}" LABELS "X11;Uil")
    endif()
endforeach()

motif_test_set_env()
