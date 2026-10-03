# Compile one .uil file with the uil compiler built by this tree and check
# that it succeeds without sanitizer reports.  Run with:
#
#   cmake -DUIL=<uil> -DSOURCE=<file.uil> -DOUTPUT=<file.uid>
#         [-DINCLUDES=<dir>,<dir>...] [-DFRAGMENT=ON] -P uil_compile.cmake
#
# A fragment is a file meant to be included by a module (value and
# procedure declarations, with no "module" header of its own); it is
# compiled through a small module that includes it.

if(NOT UIL OR NOT SOURCE OR NOT OUTPUT)
  message(FATAL_ERROR "UIL, SOURCE and OUTPUT must be set")
endif()

get_filename_component(_dir "${OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_dir}")
file(REMOVE "${OUTPUT}")

set(_input "${SOURCE}")
if(FRAGMENT)
  set(_input "${OUTPUT}.wrapper.uil")
  file(WRITE "${_input}"
    "module uil_test_fragment\n"
    "    names = case_sensitive\n"
    "include file '${SOURCE}';\n"
    "end module;\n")
endif()

set(_args -o "${OUTPUT}")
string(REPLACE "," ";" INCLUDES "${INCLUDES}")
foreach(_inc IN LISTS INCLUDES)
  list(APPEND _args "-I${_inc}")
endforeach()

# The compiler does not free everything before it exits; leaks there are
# not what this test is about.  Memory errors are still reported.
execute_process(
  COMMAND ${CMAKE_COMMAND} -E env ASAN_OPTIONS=detect_leaks=0
          ${UIL} ${_args} "${_input}"
  RESULT_VARIABLE _rc
  OUTPUT_VARIABLE _out
  ERROR_VARIABLE _out
  TIMEOUT 120)

set(_ok TRUE)
if(NOT _rc EQUAL 0)
  set(_ok FALSE)
endif()
if(_out MATCHES "Sanitizer|runtime error:")
  set(_ok FALSE)
endif()
if(NOT EXISTS "${OUTPUT}")
  set(_ok FALSE)
endif()

if(NOT _ok)
  message(FATAL_ERROR "uil failed on ${SOURCE} (rc=${_rc}):\n${_out}")
endif()
message(STATUS "compiled ${SOURCE}")
