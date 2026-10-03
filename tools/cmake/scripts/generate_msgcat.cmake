# Generate a Motif message catalog from its symbolic source catalog, as the
# upstream Makefile.am rules did:
#
#   mkcatdefs <Name>MsgCatI.h <Name>.msg > localized/C/msg/<Name>.msg
#   gencat <Name>.cat localized/C/msg/<Name>.msg
#
# The source catalog uses symbolic set and message ids ("$set MS_BulletinB",
# "MSG_BulletinB_0001 ..."). mkcatdefs turns them into #defines in the header
# and writes the numeric catalog that gencat compiles.
#
# Usage:
#   cmake -DMKCATDEFS=<path> -DSOURCE=<Name.msg> -DHEADER=<NameMsgCatI.h>
#         -DCATALOG_SOURCE=<numeric Name.msg>
#         [-DGENCAT=<path> -DCATALOG=<Name.cat>]
#         -P generate_msgcat.cmake

foreach(_var MKCATDEFS SOURCE HEADER CATALOG_SOURCE)
    if(NOT ${_var})
        message(FATAL_ERROR "generate_msgcat.cmake: ${_var} is not set")
    endif()
endforeach()

get_filename_component(_header_dir "${HEADER}" DIRECTORY)
get_filename_component(_catalog_dir "${CATALOG_SOURCE}" DIRECTORY)
file(MAKE_DIRECTORY "${_header_dir}" "${_catalog_dir}")
# Never leave a stale header behind if mkcatdefs fails.
file(REMOVE "${HEADER}")

# Run from the source directory so the header records a relative path.
get_filename_component(_source_dir "${SOURCE}" DIRECTORY)
get_filename_component(_source_name "${SOURCE}" NAME)
execute_process(
    COMMAND "${MKCATDEFS}" "${HEADER}" "${_source_name}"
    WORKING_DIRECTORY "${_source_dir}"
    OUTPUT_FILE "${CATALOG_SOURCE}"
    ERROR_VARIABLE _mkcatdefs_err
    RESULT_VARIABLE _mkcatdefs_res
)
# mkcatdefs exits 0 and deletes the header when the input has no symbolic
# ids (e.g. an already numeric catalog), so check for the header as well.
if(NOT _mkcatdefs_res EQUAL 0 OR NOT EXISTS "${HEADER}")
    message(FATAL_ERROR
        "mkcatdefs failed to generate ${HEADER} from ${SOURCE}:\n"
        "${_mkcatdefs_err}")
endif()

if(GENCAT AND CATALOG)
    # gencat merges into an existing catalog, so start from scratch.
    file(REMOVE "${CATALOG}")
    execute_process(
        COMMAND "${GENCAT}" "${CATALOG}" "${CATALOG_SOURCE}"
        RESULT_VARIABLE _gencat_res
    )
    if(NOT _gencat_res EQUAL 0)
        message(FATAL_ERROR "gencat failed to generate ${CATALOG}")
    endif()
endif()
