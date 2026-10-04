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
# A translated catalog (localized/<lang>/msg/<Name>.msg) is generated the
# same way, and then checked against the C catalog:
#
#   - mkcatdefs numbers sets and messages by their position, so the
#     translation must list the same symbolic ids in the same order.  Its
#     header must therefore define exactly what REFERENCE_HEADER defines.
#   - The libraries use the messages as printf formats for the arguments
#     of the C message, so each translated message must have the same
#     conversions, in the same order, as in REFERENCE_CATALOG.
#
# Usage:
#   cmake -DMKCATDEFS=<path> -DSOURCE=<Name.msg> -DHEADER=<NameMsgCatI.h>
#         -DCATALOG_SOURCE=<numeric Name.msg>
#         [-DGENCAT=<path> -DCATALOG=<Name.cat>]
#         [-DLOCALE=<locale for mkcatdefs and gencat, e.g. C.UTF-8>]
#         [-DREFERENCE_HEADER=<C NameMsgCatI.h>
#          -DREFERENCE_CATALOG=<C numeric Name.msg>]
#         -P generate_msgcat.cmake

foreach(_var MKCATDEFS SOURCE HEADER CATALOG_SOURCE)
    if(NOT ${_var})
        message(FATAL_ERROR "generate_msgcat.cmake: ${_var} is not set")
    endif()
endforeach()

# mkcatdefs and gencat decode the text in the current locale, so a UTF-8
# catalog has to be processed in a UTF-8 locale.
if(LOCALE)
    set(ENV{LC_ALL} "${LOCALE}")
endif()

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

if(REFERENCE_HEADER)
    file(STRINGS "${HEADER}" _defines REGEX "^#define ")
    file(STRINGS "${REFERENCE_HEADER}" _reference_defines REGEX "^#define ")
    if(NOT _defines STREQUAL _reference_defines)
        # Report the first difference.
        list(APPEND _defines "(end of catalog)")
        list(APPEND _reference_defines "(end of catalog)")
        set(_index 0)
        foreach(_define IN LISTS _reference_defines)
            list(GET _defines ${_index} _found)
            if(NOT _found STREQUAL _define)
                set(_expected "${_define}")
                break()
            endif()
            math(EXPR _index "${_index} + 1")
        endforeach()
        message(FATAL_ERROR
            "${SOURCE} does not list the sets and messages of the C catalog "
            "in the same order: expected \"${_expected}\", found "
            "\"${_found}\" (compare ${HEADER} with ${REFERENCE_HEADER}).  "
            "Add a missing message with the English text where the C "
            "catalog has it.")
    endif()
endif()

# _motif_msgcat_formats(<numeric catalog> <prefix>)
#
# Set <prefix>_keys to the "<set>.<message>" ids of the catalog and
# <prefix>_<set>.<message> to the printf conversions of each message, as
# "<number of * arguments><length modifier><conversion>" items with %i
# taken as %d.
function(_motif_msgcat_formats catalog prefix)
    file(READ "${catalog}" _text)
    # Join continued lines, and drop the characters that CMake would take
    # as list syntax; neither matters for the conversions.
    string(REPLACE "\\\n" "" _text "${_text}")
    string(REGEX REPLACE "[][;]" " " _text "${_text}")
    string(REPLACE "\n" ";" _lines "${_text}")
    set(_set 0)
    set(_keys)
    foreach(_line IN LISTS _lines)
        if(_line MATCHES "^\\$set[ \t]+([0-9]+)")
            set(_set ${CMAKE_MATCH_1})
        elseif(_line MATCHES "^([0-9]+)(.*)$")
            set(_key ${_set}.${CMAKE_MATCH_1})
            string(REGEX MATCHALL
                "%[-+ #0-9.*$]*(hh|h|ll|l|L|q|j|z|t)?[diouxXeEfFgGaAcCsSpn%]"
                _specs "${CMAKE_MATCH_2}")
            set(_formats)
            foreach(_spec IN LISTS _specs)
                if(_spec STREQUAL "%%")
                    continue()
                endif()
                string(REGEX MATCHALL "[*]" _stars "${_spec}")
                list(LENGTH _stars _stars)
                string(REGEX MATCH "[hlLqjzt]*.$" _conversion "${_spec}")
                string(REGEX REPLACE "i$" "d" _conversion "${_conversion}")
                list(APPEND _formats "${_stars}${_conversion}")
            endforeach()
            list(APPEND _keys ${_key})
            set(${prefix}_${_key} "${_formats}" PARENT_SCOPE)
        endif()
    endforeach()
    set(${prefix}_keys "${_keys}" PARENT_SCOPE)
endfunction()

if(REFERENCE_CATALOG)
    _motif_msgcat_formats("${REFERENCE_CATALOG}" _reference)
    _motif_msgcat_formats("${CATALOG_SOURCE}" _translation)
    set(_errors)
    foreach(_key IN LISTS _translation_keys)
        if(NOT "${_translation_${_key}}" STREQUAL "${_reference_${_key}}")
            string(APPEND _errors
                "\n  message ${_key} (set.message): "
                "\"${_translation_${_key}}\" instead of \"${_reference_${_key}}\"")
        endif()
    endforeach()
    if(_errors)
        message(FATAL_ERROR
            "${SOURCE}: these messages do not have the printf conversions "
            "of the C catalog, which the libraries pass their arguments "
            "for:${_errors}")
    endif()
endif()

if(GENCAT AND CATALOG)
    # A "$" alone on a line is a comment for mkcatdefs, but glibc gencat
    # reports it as an unknown directive and fails.  Make it "$ ".
    file(READ "${CATALOG_SOURCE}" _text)
    string(REGEX REPLACE "^\\$\n" "$ \n" _text "${_text}")
    string(REPLACE "\n$\n" "\n$ \n" _text "${_text}")
    string(REPLACE "\n$\n" "\n$ \n" _text "${_text}")
    file(WRITE "${CATALOG_SOURCE}" "${_text}")

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
