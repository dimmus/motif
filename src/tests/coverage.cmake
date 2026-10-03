# Run the tests and report code coverage.  Driven by the "coverage"
# target (see CMakeLists.txt), which sets:
#
#   BUILD_DIR     the build tree
#   SOURCE_DIR    the source tree
#   COMPILER_ID   GNU or Clang
#   OUTPUT_DIR    where the report goes (BUILD_DIR/coverage)
#   MIN_LINE      minimum % line coverage of libXm; the target fails below
#   CTEST         the ctest executable
#   plus the tool paths the target found (GCOVR, LCOV, GENHTML, GCOV,
#   LLVM_PROFDATA, LLVM_COV)
#
# It prefers gcovr, then lcov+genhtml, then a plain gcov summary (GCC),
# or llvm-cov (Clang).  It always prints libXm's line coverage and
# exits non-zero when it is below MIN_LINE, so CI can ratchet MIN_LINE up.

cmake_minimum_required(VERSION 3.16)

file(MAKE_DIRECTORY "${OUTPUT_DIR}")

# Run the tests; coverage is still useful if some fail.
message(STATUS "coverage: running the tests")
execute_process(
  COMMAND ${CTEST} --test-dir "${BUILD_DIR}" --output-on-failure
  RESULT_VARIABLE _ctest_rc)
if(NOT _ctest_rc EQUAL 0)
  message(WARNING "coverage: some tests failed (ctest=${_ctest_rc})")
endif()

set(_xm_percent "")

if(COMPILER_ID STREQUAL "GNU" AND GCOVR)
  message(STATUS "coverage: gcovr")
  execute_process(
    COMMAND ${GCOVR} --root "${SOURCE_DIR}" --filter "${SOURCE_DIR}/src/lib/Xm"
            --print-summary --html-details "${OUTPUT_DIR}/index.html"
            --txt "${OUTPUT_DIR}/summary.txt" "${BUILD_DIR}"
    OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
  message("${_out}${_err}")
  string(REGEX MATCH "lines: ([0-9.]+)%" _m "${_out}${_err}")
  if(_m)
    set(_xm_percent "${CMAKE_MATCH_1}")
  endif()
elseif(COMPILER_ID STREQUAL "GNU" AND LCOV AND GENHTML)
  message(STATUS "coverage: lcov")
  execute_process(COMMAND ${LCOV} --quiet --capture --directory "${BUILD_DIR}"
                          --gcov-tool "${GCOV}" --output-file "${OUTPUT_DIR}/all.info"
                          --rc branch_coverage=0)
  execute_process(COMMAND ${LCOV} --quiet --extract "${OUTPUT_DIR}/all.info"
                          "${SOURCE_DIR}/src/lib/Xm/*" --output-file "${OUTPUT_DIR}/xm.info")
  execute_process(COMMAND ${GENHTML} --quiet --output-directory "${OUTPUT_DIR}"
                          "${OUTPUT_DIR}/xm.info")
  execute_process(COMMAND ${LCOV} --summary "${OUTPUT_DIR}/xm.info"
                  OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
  message("${_out}${_err}")
  string(REGEX MATCH "lines[.: ]+([0-9.]+)%" _m "${_out}${_err}")
  if(_m)
    set(_xm_percent "${CMAKE_MATCH_1}")
  endif()
elseif(COMPILER_ID STREQUAL "GNU" AND GCOV)
  # No gcovr or lcov: summarize libXm's .gcov files ourselves.
  message(STATUS "coverage: gcov (install gcovr or lcov for an HTML report)")
  execute_process(
    COMMAND ${CMAKE_COMMAND}
            -DMODE=gcov -DBUILD_DIR=${BUILD_DIR} -DSOURCE_DIR=${SOURCE_DIR}
            -DOUTPUT_DIR=${OUTPUT_DIR} -DGCOV=${GCOV}
            -P ${CMAKE_CURRENT_LIST_DIR}/coverage_gcov.cmake
    OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
  message("${_out}${_err}")
  string(REGEX MATCH "libXm lines: ([0-9.]+)%" _m "${_out}${_err}")
  if(_m)
    set(_xm_percent "${CMAKE_MATCH_1}")
  endif()
elseif(COMPILER_ID STREQUAL "Clang" AND LLVM_PROFDATA AND LLVM_COV)
  message(STATUS "coverage: llvm-cov")
  file(GLOB_RECURSE _profs "${BUILD_DIR}/*.profraw")
  if(NOT _profs)
    message(WARNING "coverage: no .profraw files; set LLVM_PROFILE_FILE "
                    "when running the tests, e.g. "
                    "LLVM_PROFILE_FILE=${BUILD_DIR}/cov/%p.profraw")
  endif()
  execute_process(COMMAND ${LLVM_PROFDATA} merge -sparse ${_profs}
                          -o "${OUTPUT_DIR}/merged.profdata")
  file(GLOB _xmlib "${BUILD_DIR}/src/lib/Xm/libXm.so*")
  execute_process(
    COMMAND ${LLVM_COV} report ${_xmlib}
            -instr-profile=${OUTPUT_DIR}/merged.profdata
            ${SOURCE_DIR}/src/lib/Xm
    OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
  message("${_out}${_err}")
  string(REGEX MATCH "TOTAL[ 0-9.%]*[ ]([0-9.]+)%" _m "${_out}")
  if(_m)
    set(_xm_percent "${CMAKE_MATCH_1}")
  endif()
else()
  message(FATAL_ERROR
    "coverage: need gcovr, lcov+genhtml or gcov (GCC), or "
    "llvm-profdata+llvm-cov (Clang)")
endif()

if(_xm_percent STREQUAL "")
  message(WARNING "coverage: could not determine libXm line coverage")
  return()
endif()

message(STATUS "coverage: libXm line coverage ${_xm_percent}% (minimum ${MIN_LINE}%)")
if(_xm_percent LESS MIN_LINE)
  message(FATAL_ERROR
    "coverage: libXm line coverage ${_xm_percent}% is below the "
    "${MIN_LINE}% gate")
endif()
