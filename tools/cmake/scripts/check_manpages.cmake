# Check that every function a shared library exports and its installed
# headers declare has a manual page.
#
#   cmake -DARGS=<file> -P check_manpages.cmake
#
# where <file> sets NM, LIBRARY (the shared library), HEADERS (its
# installed headers) and MANDIR (doc/man); see doc/CMakeLists.txt.
#
# A function is checked when it is exported (a text symbol in the dynamic
# symbol table) and named in one of the headers, comments aside: this
# includes the obsolete _Xm names that XmP.h maps onto Xme functions with
# #define and the internal functions that installed macros call.  Its page
# is MANDIR/man3/<function>.3, which must name the function; a page that
# is only a ".so man3/<other>.3" line stands for <other>.3, which must
# exist and name the function.

if(DEFINED ARGS)
  include(${ARGS})
endif()
foreach(_var NM LIBRARY HEADERS MANDIR)
  if(NOT DEFINED ${_var})
    message(FATAL_ERROR "check_manpages.cmake: ${_var} is not set")
  endif()
endforeach()

# Exported functions, without their version
execute_process(COMMAND ${NM} -D --defined-only ${LIBRARY}
  OUTPUT_VARIABLE _out RESULT_VARIABLE _rc)
if(NOT _rc EQUAL 0)
  message(FATAL_ERROR "${NM} -D ${LIBRARY} failed")
endif()
string(REGEX MATCHALL " [TW] [A-Za-z_][A-Za-z0-9_]*" _lines "${_out}")
set(_functions)
foreach(_l IN LISTS _lines)
  string(REGEX REPLACE "^ . " "" _l "${_l}")
  list(APPEND _functions ${_l})
endforeach()
list(REMOVE_DUPLICATES _functions)

# Identifiers of the headers, comments removed
set(_ids)
foreach(_h IN LISTS HEADERS)
  file(READ ${_h} _text)
  string(REGEX REPLACE "/\\*([^*]|\\*+[^*/])*\\*+/" " " _text "${_text}")
  string(REGEX REPLACE "//[^\n]*" " " _text "${_text}")
  string(REGEX MATCHALL "[A-Za-z_][A-Za-z0-9_]*" _words "${_text}")
  list(APPEND _ids ${_words})
endforeach()
list(REMOVE_DUPLICATES _ids)

# functions & ids, as A - (A - ids)
set(_undeclared ${_functions})
list(REMOVE_ITEM _undeclared ${_ids})
set(_declared ${_functions})
if(_undeclared)
  list(REMOVE_ITEM _declared ${_undeclared})
endif()

set(_missing)
set(_bad)
foreach(_f IN LISTS _declared)
  set(_page ${MANDIR}/man3/${_f}.3)
  if(NOT EXISTS ${_page})
    list(APPEND _missing ${_f})
    continue()
  endif()
  file(READ ${_page} _text)
  if(_text MATCHES "^\\.so man3/([A-Za-z0-9_]+\\.3)[ \t]*\n?$")
    set(_page ${MANDIR}/man3/${CMAKE_MATCH_1})
    if(NOT EXISTS ${_page})
      list(APPEND _bad "${_f}.3: ${CMAKE_MATCH_1} does not exist")
      continue()
    endif()
    file(READ ${_page} _text)
  endif()
  # The name as a word, or after a font escape such as \fB, with the
  # escapes that only stop hyphenation or protect a dot (\% and \&) removed
  string(REPLACE "\\%" "" _text "${_text}")
  string(REPLACE "\\&" "" _text "${_text}")
  if(NOT _text MATCHES "(^|[^A-Za-z0-9_]|\\\\f[BIPR])${_f}([^A-Za-z0-9_]|$)")
    get_filename_component(_name ${_page} NAME)
    list(APPEND _bad "${_f}.3: ${_name} does not name ${_f}")
  endif()
endforeach()

list(LENGTH _declared _n)
if(_missing OR _bad)
  set(_msg "${LIBRARY}:")
  if(_missing)
    list(LENGTH _missing _nmissing)
    string(REPLACE ";" "\n  " _list "${_missing}")
    string(APPEND _msg "\n${_nmissing} of the ${_n} functions declared in the installed "
      "headers have no page in ${MANDIR}/man3:\n  ${_list}")
  endif()
  if(_bad)
    string(REPLACE ";" "\n  " _list "${_bad}")
    string(APPEND _msg "\nthese pages are wrong:\n  ${_list}")
  endif()
  message(FATAL_ERROR "${_msg}")
endif()
message(STATUS "${LIBRARY}: all ${_n} functions declared in the installed "
  "headers have a page")
