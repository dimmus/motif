# Summarize libXm's line coverage from the .gcda/.gcno files, using
# plain gcov, when neither gcovr nor lcov is installed.  Driven by
# coverage.cmake; prints "libXm lines: <p>%" and writes the per-file
# numbers to OUTPUT_DIR/summary.txt.  Inputs: BUILD_DIR, SOURCE_DIR,
# OUTPUT_DIR, GCOV.

set(_xm_obj_dir "${BUILD_DIR}/src/lib/Xm/CMakeFiles/Xm.dir")
file(GLOB_RECURSE _gcda "${_xm_obj_dir}/*.gcda")

set(_total 0)
set(_covered 0)
set(_report "")

foreach(_d IN LISTS _gcda)
  # Run gcov in the object directory so it finds the .gcno next to it.
  get_filename_component(_obj_dir "${_d}" DIRECTORY)
  execute_process(
    COMMAND "${GCOV}" -n -b "${_d}"
    WORKING_DIRECTORY "${_obj_dir}"
    OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
  # gcov prints, per source file:
  #   File 'src/lib/Xm/Foo.c'
  #   Lines executed:NN.NN% of MMMM
  set(_text "${_out}")
  string(REGEX MATCHALL "File '[^']*'\nLines executed:[0-9.]+% of [0-9]+"
         _entries "${_text}")
  foreach(_e IN LISTS _entries)
    string(REGEX MATCH "File '([^']*)'" _fm "${_e}")
    set(_file "${CMAKE_MATCH_1}")
    if(NOT _file MATCHES "/src/lib/Xm/" AND NOT _file MATCHES "^src/lib/Xm/"
       AND NOT _file MATCHES "^[^/]+\\.c$")
      continue()
    endif()
    string(REGEX MATCH "Lines executed:([0-9.]+)% of ([0-9]+)" _lm "${_e}")
    set(_pct "${CMAKE_MATCH_1}")
    set(_lines "${CMAKE_MATCH_2}")
    # covered = round(pct/100 * lines)
    math(EXPR _c "(${_lines} * 0)")       # placeholder, refined below
    # CMake math is integer only; scale the percentage by 100.
    string(REPLACE "." "" _pct_scaled "${_pct}00")   # e.g. 12.34 -> 1234
    string(REGEX MATCH "^[0-9]+" _pct_scaled "${_pct_scaled}")
    math(EXPR _c "(${_lines} * ${_pct_scaled} + 5000) / 10000")
    math(EXPR _total "${_total} + ${_lines}")
    math(EXPR _covered "${_covered} + ${_c}")
    get_filename_component(_name "${_file}" NAME)
    string(APPEND _report "${_pct}%\t${_lines}\t${_name}\n")
  endforeach()
endforeach()

file(WRITE "${OUTPUT_DIR}/summary.txt" "${_report}")

if(_total EQUAL 0)
  message("libXm lines: 0% (no .gcda files; build with "
          "-DWITH_COMPILER_CODE_COVERAGE=ON and run the tests first)")
  return()
endif()

math(EXPR _whole "(${_covered} * 100) / ${_total}")
math(EXPR _frac "((${_covered} * 10000) / ${_total}) % 100")
if(_frac LESS 10)
  set(_frac "0${_frac}")
endif()
message("libXm lines: ${_whole}.${_frac}% (${_covered}/${_total})")
