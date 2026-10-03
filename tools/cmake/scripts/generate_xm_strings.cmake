# Generate the Xm string tables with makestrs, as the upstream Makefile.am
# rules did:
#
#   makestrs -f xmstring.list > XmStrDefs.c
#
# makestrs reads the templates named in xmstring.list (XmStrDefs.ct and the
# *.ht header templates) from its working directory and writes the headers
# there, so the templates are first copied into WORK_DIR.  The headers are then
# copied to HEADER_DIR, where the rest of the build includes them from.
#
# Usage:
#   cmake -DMAKESTRS=<path> -DSOURCE_DIR=<src/lib/Xm> -DLIST=<xmstring.list>
#         -DWORK_DIR=<dir> -DHEADER_DIR=<include/Xm>
#         -P generate_xm_strings.cmake
#
# SPDX-License-Identifier: LGPL-2.1-or-later

foreach(_var MAKESTRS SOURCE_DIR LIST WORK_DIR HEADER_DIR)
  if(NOT ${_var})
    message(FATAL_ERROR "generate_xm_strings.cmake: ${_var} is not set")
  endif()
endforeach()

set(_templates XmStrDefs.ct XmStrDefs.ht XmStrDefs22.ht XmStrDefs23.ht XmStrDefsI.ht)
set(_headers XmStrDefs.h XmStrDefs22.h XmStrDefs23.h XmStrDefsI.h)

file(MAKE_DIRECTORY "${WORK_DIR}" "${HEADER_DIR}")
foreach(_t IN LISTS _templates)
  file(COPY "${SOURCE_DIR}/${_t}" DESTINATION "${WORK_DIR}")
endforeach()
# Never leave stale output behind if makestrs fails.
foreach(_h IN LISTS _headers)
  file(REMOVE "${WORK_DIR}/${_h}" "${HEADER_DIR}/${_h}")
endforeach()
file(REMOVE "${WORK_DIR}/XmStrDefs.c")

execute_process(
  COMMAND "${MAKESTRS}" -f "${LIST}"
  WORKING_DIRECTORY "${WORK_DIR}"
  OUTPUT_FILE "${WORK_DIR}/XmStrDefs.c.tmp"
  ERROR_VARIABLE _err
  RESULT_VARIABLE _res
)
if(NOT _res EQUAL 0)
  file(REMOVE "${WORK_DIR}/XmStrDefs.c.tmp")
  message(FATAL_ERROR "makestrs failed (${_res}):\n${_err}")
endif()
if(_err)
  message(WARNING "makestrs: ${_err}")
endif()

foreach(_h IN LISTS _headers)
  if(NOT EXISTS "${WORK_DIR}/${_h}")
    message(FATAL_ERROR "makestrs did not generate ${_h}")
  endif()
  file(COPY "${WORK_DIR}/${_h}" DESTINATION "${HEADER_DIR}")
endforeach()
file(RENAME "${WORK_DIR}/XmStrDefs.c.tmp" "${WORK_DIR}/XmStrDefs.c")
