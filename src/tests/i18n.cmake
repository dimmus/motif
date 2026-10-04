# Locales for the i18n suites, included by CMakeLists.txt.
#
# The I18nLocale suite also runs in locales that are
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

# motif_locale_test(<suite> <locale>): the suite in a generated locale,
# as Xm.<suite>.<locale>.
function(motif_locale_test suite locale)
    motif_x_test(Xm.${suite}.${locale} $<TARGET_FILE:motif_tests> ${suite})
    set_property(TEST Xm.${suite}.${locale} APPEND PROPERTY ENVIRONMENT
        "LOCPATH=${MOTIF_TEST_LOCALE_DIR}" "MOTIF_TEST_LOCALE=${locale}")
endfunction()

foreach(_locale IN LISTS MOTIF_TEST_LOCALES)
    motif_locale_test(I18nLocale ${_locale})
endforeach()

