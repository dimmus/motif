# Locales and message catalogs for the i18n suites, included by
# CMakeLists.txt.
#
# The I18nLocale, Xim and MsgCat suites also run in locales that are
# rarely installed: they are generated at build time with glibc's
# localedef into <build>/src/tests/locale, without root, and the tests
# run with LOCPATH pointing there and MOTIF_TEST_LOCALE naming the
# locale.  Where localedef or the glibc locale sources are missing (musl,
# the BSDs, Debian without the "locales" package) these tests are not
# registered.

# <locale source>|<charmap>|<locale name>
set(MOTIF_TEST_LOCALE_SPECS
    "ja_JP|UTF-8|ja_JP.UTF-8"
    "ja_JP|EUC-JP|ja_JP.EUC-JP"
    "de_DE|UTF-8|de_DE.UTF-8"
    "de_DE|ISO-8859-1|de_DE.ISO-8859-1"
    "he_IL|UTF-8|he_IL.UTF-8"
)

find_program(LOCALEDEF_EXECUTABLE localedef)
find_path(MOTIF_TEST_I18N_DIR NAMES locales/de_DE
    PATHS /usr/share/i18n /usr/local/share/i18n
    NO_DEFAULT_PATH)
mark_as_advanced(LOCALEDEF_EXECUTABLE MOTIF_TEST_I18N_DIR)

set(MOTIF_TEST_LOCALE_DIR ${CMAKE_CURRENT_BINARY_DIR}/locale)
set(MOTIF_TEST_LOCALES)
set(_outputs)
if(LOCALEDEF_EXECUTABLE AND MOTIF_TEST_I18N_DIR AND NOT CMAKE_CROSSCOMPILING)
    foreach(_spec IN LISTS MOTIF_TEST_LOCALE_SPECS)
        string(REPLACE "|" ";" _spec "${_spec}")
        list(GET _spec 0 _input)
        list(GET _spec 1 _charmap)
        list(GET _spec 2 _name)
        if(NOT EXISTS ${MOTIF_TEST_I18N_DIR}/locales/${_input} OR
           (NOT EXISTS ${MOTIF_TEST_I18N_DIR}/charmaps/${_charmap} AND
            NOT EXISTS ${MOTIF_TEST_I18N_DIR}/charmaps/${_charmap}.gz))
            continue()
        endif()
        add_custom_command(
            OUTPUT ${MOTIF_TEST_LOCALE_DIR}/${_name}/LC_CTYPE
            COMMAND ${CMAKE_COMMAND}
                -DLOCALEDEF=${LOCALEDEF_EXECUTABLE}
                -DINPUT=${_input}
                -DCHARMAP=${_charmap}
                -DOUTPUT=${MOTIF_TEST_LOCALE_DIR}/${_name}
                -P ${CMAKE_CURRENT_SOURCE_DIR}/localedef.cmake
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/localedef.cmake
            COMMENT "Generating the ${_name} locale for the tests"
            VERBATIM)
        list(APPEND _outputs ${MOTIF_TEST_LOCALE_DIR}/${_name}/LC_CTYPE)
        list(APPEND MOTIF_TEST_LOCALES ${_name})
    endforeach()
endif()
if(_outputs)
    add_custom_target(motif_test_locales ALL DEPENDS ${_outputs})
    add_dependencies(motif_tests motif_test_locales)
    string(REPLACE ";" " " _list "${MOTIF_TEST_LOCALES}")
    message(STATUS "Tests: generating the locales ${_list}")
else()
    message(STATUS "Tests: no localedef or glibc locale sources, the i18n "
                   "suites run in the default locale only")
endif()

# motif_locale_test(<suite> <locale> [ENV=VALUE...]): the suite in a
# generated locale, as Xm.<suite>.<locale>.
function(motif_locale_test suite locale)
    motif_x_test(Xm.${suite}.${locale} $<TARGET_FILE:motif_tests> ${suite})
    set_property(TEST Xm.${suite}.${locale} APPEND PROPERTY ENVIRONMENT
        "LOCPATH=${MOTIF_TEST_LOCALE_DIR}" "MOTIF_TEST_LOCALE=${locale}" ${ARGN})
endfunction()

foreach(_locale IN LISTS MOTIF_TEST_LOCALES)
    motif_locale_test(I18nLocale ${_locale})
    if(_locale MATCHES "^ja_JP")
        motif_locale_test(Xim ${_locale})
    endif()
endforeach()

# Message catalogs: Xm.MsgCat checks that a warning comes from the
# catalog that NLSPATH finds.  Xm.MsgCat.C uses a copy of the C catalog
# with one message changed, Xm.MsgCat.de_DE.UTF-8 the German catalog.
# Xm.MsgCat itself (no catalog) checks the built-in message.
if(WITH_MESSAGE_CATALOG AND GENCAT_EXECUTABLE)
    set(_dir ${CMAKE_CURRENT_BINARY_DIR}/msgcat)
    file(READ ${CMAKE_SOURCE_DIR}/src/lib/Xm/Xm.msg _msg)
    set(_from "MSG_Form_0000 Fraction base cannot be zero.")
    string(FIND "${_msg}" "${_from}" _at)
    if(_at LESS 0)
        message(FATAL_ERROR "${_from} is not in src/lib/Xm/Xm.msg")
    endif()
    string(REPLACE "${_from}" "MSG_Form_0000 Test catalog: fraction base is zero."
        _msg "${_msg}")
    file(WRITE ${_dir}/Xm.msg "${_msg}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/src/lib/Xm/Xm.msg)
    add_custom_command(
        OUTPUT ${_dir}/C/Xm.cat
        COMMAND ${MOTIF_TOOL_ENV} ${CMAKE_COMMAND}
            -DMKCATDEFS=$<TARGET_FILE:${MOTIF_HOST_TOOL_PREFIX}mkcatdefs>
            -DSOURCE=${_dir}/Xm.msg
            -DHEADER=${_dir}/XmMsgCatI.h
            -DCATALOG_SOURCE=${_dir}/C/Xm.msg
            -DREFERENCE_HEADER=${CMAKE_BINARY_DIR}/include/Xm/XmMsgCatI.h
            -DGENCAT=${GENCAT_EXECUTABLE}
            -DCATALOG=${_dir}/C/Xm.cat
            -P ${CMAKE_SOURCE_DIR}/tools/cmake/scripts/generate_msgcat.cmake
        DEPENDS ${MOTIF_HOST_TOOL_PREFIX}mkcatdefs xm_msgcat_header ${_dir}/Xm.msg
            ${CMAKE_SOURCE_DIR}/tools/cmake/scripts/generate_msgcat.cmake
        COMMENT "Generating the test message catalog"
        VERBATIM)
    add_custom_target(motif_test_msgcat ALL DEPENDS ${_dir}/C/Xm.cat)
    add_dependencies(motif_tests motif_test_msgcat)

    motif_x_test(Xm.MsgCat.C $<TARGET_FILE:motif_tests> MsgCat)
    set_property(TEST Xm.MsgCat.C APPEND PROPERTY ENVIRONMENT
        "MOTIF_TEST_LOCALE=C" "NLSPATH=${_dir}/%l/%N.cat"
        "MOTIF_TEST_MSGCAT_SOURCE=${_dir}/Xm.msg")
    if("de_DE.UTF-8" IN_LIST MOTIF_TEST_LOCALES AND TARGET localized_msgcat)
        add_dependencies(motif_tests localized_msgcat)
        motif_locale_test(MsgCat de_DE.UTF-8
            "NLSPATH=${CMAKE_BINARY_DIR}/localized/%l/msg/%N.cat"
            "MOTIF_TEST_MSGCAT_SOURCE=${CMAKE_SOURCE_DIR}/localized/de/msg/Xm.msg")
    endif()
endif()
