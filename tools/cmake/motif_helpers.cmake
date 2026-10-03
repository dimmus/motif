# Helper functions for the Motif build.
#
# SPDX-License-Identifier: LGPL-2.1-or-later

include(CheckCCompilerFlag)

# motif_check_c_flags(<var> <flag>...)
#
# Append to the list <var> every <flag> that the C compiler accepts.  GCC
# silently accepts any -Wno-<warning>, so the positive form of such a flag is
# what gets tested.
function(motif_check_c_flags var)
  set(_flags ${${var}})
  foreach(_flag IN LISTS ARGN)
    string(REGEX REPLACE "^-Wno-(error=)?" "-W" _test "${_flag}")
    string(MAKE_C_IDENTIFIER "MOTIF_HAVE_CFLAG${_test}" _cache)
    check_c_compiler_flag("${_test}" ${_cache})
    if(${_cache})
      list(APPEND _flags "${_flag}")
    endif()
  endforeach()
  set(${var} "${_flags}" PARENT_SCOPE)
endfunction()

# motif_find_parser_generators()
#
# Find a yacc-compatible parser generator that understands -d -o (bison or
# byacc), a lex-compatible scanner generator (flex or lex) and its library,
# which provides main() for wmluiltok.  Sets the cache variables
# MOTIF_YACC_EXECUTABLE, MOTIF_LEX_EXECUTABLE and MOTIF_LEX_LIBRARY.
function(motif_find_parser_generators)
  find_program(MOTIF_YACC_EXECUTABLE NAMES bison byacc
    DOC "yacc-compatible parser generator that accepts -d -o (bison or byacc)")
  if(NOT MOTIF_YACC_EXECUTABLE)
    message(FATAL_ERROR "Neither bison nor byacc was found; install one of them.")
  endif()
  find_program(MOTIF_LEX_EXECUTABLE NAMES flex lex
    DOC "lex-compatible scanner generator (flex or lex)")
  if(NOT MOTIF_LEX_EXECUTABLE)
    message(FATAL_ERROR "Neither flex nor lex was found; install one of them.")
  endif()
  find_library(MOTIF_LEX_LIBRARY NAMES fl l
    DOC "lex library (libfl or libl), which provides main() and yywrap()")
  if(NOT MOTIF_LEX_LIBRARY)
    message(FATAL_ERROR "The lex library (libfl or libl) was not found.")
  endif()
endfunction()

# motif_add_message_catalog(<target> <name> <source> <header>)
#
# Generate the message catalog header <header> (symbolic set and message ids,
# e.g. XmMsgCatI.h) from the symbolic source catalog <source> with mkcatdefs.
# This also writes the numeric catalog to localized/C/msg/<name>.msg in the
# build tree and, when gencat was found, compiles it to <name>.cat next to it.
# Adds the target <target> that builds them all.
#
# The .cat is installed as ${CMAKE_INSTALL_LOCALEDIR}/C/LC_MESSAGES/<name>,
# without the suffix: catopen("<name>", NL_CAT_LOCALE) expands %N in NLSPATH
# to the bare name, and glibc's built-in search path includes
# /usr/share/locale/%L/LC_MESSAGES/%N. musl has no built-in path, so set
# NLSPATH=<localedir>/%L/LC_MESSAGES/%N there (and for other prefixes).
function(motif_add_message_catalog target name source header)
  set(_catalog_source ${CMAKE_BINARY_DIR}/localized/C/msg/${name}.msg)
  set(_outputs ${header} ${_catalog_source})
  set(_gencat_args)
  if(GENCAT_EXECUTABLE)
    set(_catalog ${CMAKE_BINARY_DIR}/localized/C/msg/${name}.cat)
    list(APPEND _outputs ${_catalog})
    set(_gencat_args -DGENCAT=${GENCAT_EXECUTABLE} -DCATALOG=${_catalog})
  endif()

  add_custom_command(
    OUTPUT ${_outputs}
    COMMAND ${CMAKE_COMMAND}
      -DMKCATDEFS=$<TARGET_FILE:${MOTIF_HOST_TOOL_PREFIX}mkcatdefs>
      -DSOURCE=${source}
      -DHEADER=${header}
      -DCATALOG_SOURCE=${_catalog_source}
      ${_gencat_args}
      -P ${CMAKE_SOURCE_DIR}/tools/cmake/scripts/generate_msgcat.cmake
    DEPENDS ${MOTIF_HOST_TOOL_PREFIX}mkcatdefs ${source} ${CMAKE_SOURCE_DIR}/tools/cmake/scripts/generate_msgcat.cmake
    COMMENT "Generating ${name} message catalog"
    VERBATIM
  )
  add_custom_target(${target} ALL DEPENDS ${_outputs})

  if(GENCAT_EXECUTABLE)
    install(FILES ${_catalog}
      DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/C/LC_MESSAGES
      RENAME ${name}
    )
  endif()
endfunction()

# motif_print_summary()
#
# Print the configuration chosen for this build.
function(motif_print_summary)
  get_property(_options GLOBAL PROPERTY MOTIF_OPTIONS)
  list(SORT _options)
  set(_width 0)
  foreach(_opt IN LISTS _options)
    string(LENGTH "${_opt}" _len)
    if(_len GREATER _width)
      set(_width ${_len})
    endif()
  endforeach()

  message(STATUS "")
  message(STATUS "Motif ${PROJECT_VERSION} configuration:")
  message(STATUS "  System:         ${CMAKE_SYSTEM}")
  message(STATUS "  C compiler:     ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
  message(STATUS "  Build type:     ${CMAKE_BUILD_TYPE}")
  message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
  message(STATUS "  Options:")
  foreach(_opt IN LISTS _options)
    string(LENGTH "${_opt}" _len)
    math(EXPR _pad "${_width} - ${_len} + 1")
    string(REPEAT " " ${_pad} _spaces)
    if(${_opt})
      set(_value ON)
    else()
      set(_value OFF)
    endif()
    message(STATUS "    ${_opt}${_spaces}${_value}")
  endforeach()
  message(STATUS "")
endfunction()

# motif_option(<name> <description> <default>)
#
# option() that is also listed by motif_print_summary().
macro(motif_option name description default)
  option(${name} "${description}" ${default})
  set_property(GLOBAL APPEND PROPERTY MOTIF_OPTIONS ${name})
endmacro()
