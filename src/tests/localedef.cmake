# Compile a locale for the tests into a directory, without root:
#
#   cmake -DLOCALEDEF=<path> -DINPUT=<locale source, e.g. ja_JP>
#         -DCHARMAP=<charmap, e.g. EUC-JP> -DOUTPUT=<directory>
#         -P localedef.cmake
#
# glibc's localedef exits with 1 when it only warns (some sources are not
# strictly ISO C conforming), so success is judged by the output: -c
# writes it in spite of warnings, and LC_CTYPE must be there.

foreach(_var LOCALEDEF INPUT CHARMAP OUTPUT)
    if(NOT ${_var})
        message(FATAL_ERROR "localedef.cmake: ${_var} is not set")
    endif()
endforeach()

file(REMOVE_RECURSE "${OUTPUT}")
get_filename_component(_parent "${OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_parent}")
# localedef must not pick up a LOCPATH of the environment.
unset(ENV{LOCPATH})
execute_process(
    COMMAND "${LOCALEDEF}" -c -i "${INPUT}" -f "${CHARMAP}" "${OUTPUT}"
    RESULT_VARIABLE _res
    OUTPUT_VARIABLE _out
    ERROR_VARIABLE _out)
if(NOT EXISTS "${OUTPUT}/LC_CTYPE")
    file(REMOVE_RECURSE "${OUTPUT}")
    message(FATAL_ERROR
        "localedef -i ${INPUT} -f ${CHARMAP} failed (${_res}):\n${_out}")
endif()
