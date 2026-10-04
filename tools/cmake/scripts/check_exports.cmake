# Check that a shared library exports every symbol its installed headers
# declare, i.e. that nothing was left out of its version script.
#
#   cmake -DARGS=<file> -P check_exports.cmake
#
# where <file> sets NM, LIBRARY (the shared library), OBJECTS (its object
# files), HEADERS (its installed headers) and MAP (its version script); see
# motif_exports_test() in the top-level CMakeLists.txt.
#
# A symbol is reported when it is defined as global in one of the objects
# of the library, is named in one of the headers (comments aside), and is
# not exported.  Fix it by adding the symbol to the version script.

if(DEFINED ARGS)
  include(${ARGS})
endif()
foreach(_var NM LIBRARY OBJECTS HEADERS MAP)
  if(NOT DEFINED ${_var})
    message(FATAL_ERROR "check_exports.cmake: ${_var} is not set")
  endif()
endforeach()

# Exported symbols, without their version
execute_process(COMMAND ${NM} -D --defined-only ${LIBRARY}
  OUTPUT_VARIABLE _out RESULT_VARIABLE _rc)
if(NOT _rc EQUAL 0)
  message(FATAL_ERROR "${NM} -D ${LIBRARY} failed")
endif()
string(REGEX MATCHALL "[A-Za-z_][A-Za-z0-9_]*(@@?[A-Za-z0-9_.]+)?\n" _lines "${_out}")
set(_exported)
foreach(_l IN LISTS _lines)
  string(REGEX REPLACE "(@.*)?\n$" "" _l "${_l}")
  list(APPEND _exported ${_l})
endforeach()

# Global symbols defined by the objects
execute_process(COMMAND ${NM} -g --defined-only ${OBJECTS}
  OUTPUT_VARIABLE _out RESULT_VARIABLE _rc)
if(NOT _rc EQUAL 0)
  message(FATAL_ERROR "${NM} -g of the objects of ${LIBRARY} failed")
endif()
string(REGEX MATCHALL " [A-TV-Z] [A-Za-z_][A-Za-z0-9_]*\n" _lines "${_out}")
set(_defined)
foreach(_l IN LISTS _lines)
  string(REGEX REPLACE "^ . (.*)\n$" "\\1" _l "${_l}")
  list(APPEND _defined ${_l})
endforeach()
list(REMOVE_DUPLICATES _defined)

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

# (defined - exported) & ids, as A - (A - ids)
set(_missing ${_defined})
list(REMOVE_ITEM _missing ${_exported})
set(_undeclared ${_missing})
list(REMOVE_ITEM _undeclared ${_ids})
set(_declared_missing ${_missing})
if(_undeclared)
  list(REMOVE_ITEM _declared_missing ${_undeclared})
endif()

list(LENGTH _exported _nexp)
if(_declared_missing)
  list(SORT _declared_missing)
  string(REPLACE ";" "\n  " _list "${_declared_missing}")
  message(FATAL_ERROR
    "${LIBRARY}: these symbols are declared in installed headers but not "
    "exported; add them to ${MAP} (see doc/abi-policy.md):\n  ${_list}")
endif()
message(STATUS "${LIBRARY}: ${_nexp} exported symbols, none missing")
