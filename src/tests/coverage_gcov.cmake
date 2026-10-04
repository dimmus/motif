# Summarize libXm's line coverage from the .gcda/.gcno files using plain
# gcov, when neither gcovr nor lcov is installed.  Driven by
# coverage.cmake; prints "libXm lines: <p>%" and writes the per-file
# numbers to OUTPUT_DIR/summary.txt.  Inputs: BUILD_DIR, SOURCE_DIR,
# OUTPUT_DIR, GCOV.
#
# gcov prints one "File '<path>'\nLines executed:<p>% of <n>" block per
# source file an object pulls in, so the same .c (and its headers)
# appears under many objects; the numbers are collected per file path so
# each is counted once.

set(_xm_obj_dir "${BUILD_DIR}/src/lib/Xm/CMakeFiles/Xm.dir")
file(GLOB_RECURSE _gcda "${_xm_obj_dir}/*.gcda")

set(_files "")          # list of source paths seen
set(_pcts "")           # parallel list: executed % for each path
set(_lines_list "")     # parallel list: line count for each path

foreach(_d IN LISTS _gcda)
  get_filename_component(_obj_dir "${_d}" DIRECTORY)
  execute_process(
    COMMAND "${GCOV}" -n "${_d}"
    WORKING_DIRECTORY "${_obj_dir}"
    OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
  # Split into "File '...'" / "Lines executed:...% of N" pairs.
  string(REGEX MATCHALL "File '[^']*'\nLines executed:[0-9.]+% of [0-9]+"
         _entries "${_out}")
  foreach(_e IN LISTS _entries)
    string(REGEX MATCH "File '([^']*)'" _ "${_e}")
    set(_file "${CMAKE_MATCH_1}")
    # Only libXm's own C sources, by absolute or relative path.
    if(NOT _file MATCHES "src/lib/Xm/[^/]+\\.c$")
      continue()
    endif()
    string(REGEX MATCH "Lines executed:([0-9.]+)% of ([0-9]+)" _ "${_e}")
    set(_pct "${CMAKE_MATCH_1}")
    set(_n "${CMAKE_MATCH_2}")
    list(FIND _files "${_file}" _idx)
    if(_idx EQUAL -1)
      list(APPEND _files "${_file}")
      list(APPEND _pcts "${_pct}")
      list(APPEND _lines_list "${_n}")
    endif()
  endforeach()
endforeach()

set(_total 0)
set(_covered 0)
set(_report "")
list(LENGTH _files _count)
if(_count GREATER 0)
  math(EXPR _last "${_count} - 1")
  foreach(_i RANGE ${_last})
    list(GET _files ${_i} _file)
    list(GET _pcts ${_i} _pct)
    list(GET _lines_list ${_i} _n)
    # covered = round(pct/100 * n), with pct scaled by 100 (integer math)
    string(REGEX REPLACE "\\." "" _ps "${_pct}")   # "54.97" -> "5497"
    string(REGEX MATCH "\\.([0-9]+)" _ "${_pct}")
    string(LENGTH "${CMAKE_MATCH_1}" _decimals)
    if(_decimals EQUAL 1)
      math(EXPR _ps "${_ps} * 10")                 # one decimal -> two
    elseif(_decimals EQUAL 0)
      math(EXPR _ps "${_ps} * 100")
    endif()
    math(EXPR _c "(${_n} * ${_ps} + 5000) / 10000")
    math(EXPR _total "${_total} + ${_n}")
    math(EXPR _covered "${_covered} + ${_c}")
    get_filename_component(_name "${_file}" NAME)
    string(APPEND _report "${_pct}%\t${_n}\t${_name}\n")
  endforeach()
endif()

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
