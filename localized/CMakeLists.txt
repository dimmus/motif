# Translated message catalogs
#
# localized/<lang>/msg/<Name>.msg are UTF-8 translations of the symbolic
# catalogs src/lib/{Xm,Mrm,Uil}/<Name>.msg.  Each one is generated like the
# C catalog (see motif_add_message_catalog), checked against it (same
# message ids in the same order, same printf conversions), compiled with
# gencat and installed as ${CMAKE_INSTALL_LOCALEDIR}/<lang>/LC_MESSAGES/<Name>.
#
# glibc's default NLSPATH searches <localedir>/%l/LC_MESSAGES/%N, %l being
# the language part of LC_MESSAGES, so every <lang>_* locale finds them.
# The catalogs are UTF-8, so they display correctly only in UTF-8 locales.

set(MOTIF_CATALOG_LANGUAGES de es fr it ja)

# <Name>|<target that generates the C catalog>|<its header>
set(_catalogs
  "Xm|xm_msgcat_header|${CMAKE_BINARY_DIR}/include/Xm/XmMsgCatI.h"
  "Mrm|mrm_msgcat_header|${CMAKE_BINARY_DIR}/include/Mrm/MrmMsgCatI.h"
  "Uil|uil_msgcat_header|${CMAKE_BINARY_DIR}/include/uil/UilMsgCatI.h"
)

set(_outputs)
foreach(_lang IN LISTS MOTIF_CATALOG_LANGUAGES)
  foreach(_catalog IN LISTS _catalogs)
    string(REPLACE "|" ";" _catalog "${_catalog}")
    list(GET _catalog 0 _name)
    list(GET _catalog 1 _reference_target)
    list(GET _catalog 2 _reference_header)
    set(_reference_catalog ${CMAKE_BINARY_DIR}/localized/C/msg/${_name}.msg)

    set(_source ${CMAKE_CURRENT_SOURCE_DIR}/${_lang}/msg/${_name}.msg)
    set(_dir ${CMAKE_CURRENT_BINARY_DIR}/${_lang}/msg)
    set(_catalog_source ${_dir}/${_name}.msg)
    # Only needed to check the message ids against the C catalog.
    set(_header ${_dir}/${_name}MsgCatI.h)
    set(_target_outputs ${_catalog_source} ${_header})
    set(_gencat_args)
    if(GENCAT_EXECUTABLE)
      set(_cat ${_dir}/${_name}.cat)
      list(APPEND _target_outputs ${_cat})
      set(_gencat_args -DGENCAT=${GENCAT_EXECUTABLE} -DCATALOG=${_cat})
      install(FILES ${_cat}
        DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/${_lang}/LC_MESSAGES
        RENAME ${_name}
      )
    endif()

    add_custom_command(
      OUTPUT ${_target_outputs}
      COMMAND ${MOTIF_TOOL_ENV} ${CMAKE_COMMAND}
        -DMKCATDEFS=$<TARGET_FILE:${MOTIF_HOST_TOOL_PREFIX}mkcatdefs>
        -DSOURCE=${_source}
        -DHEADER=${_header}
        -DCATALOG_SOURCE=${_catalog_source}
        -DLOCALE=C.UTF-8
        -DREFERENCE_HEADER=${_reference_header}
        -DREFERENCE_CATALOG=${_reference_catalog}
        ${_gencat_args}
        -P ${CMAKE_SOURCE_DIR}/tools/cmake/scripts/generate_msgcat.cmake
      DEPENDS
        ${MOTIF_HOST_TOOL_PREFIX}mkcatdefs ${_reference_target}
        ${_source} ${_reference_header} ${_reference_catalog}
        ${CMAKE_SOURCE_DIR}/tools/cmake/scripts/generate_msgcat.cmake
      COMMENT "Generating ${_lang} ${_name} message catalog"
      VERBATIM
    )
    list(APPEND _outputs ${_target_outputs})
  endforeach()
endforeach()

add_custom_target(localized_msgcat ALL DEPENDS ${_outputs})
