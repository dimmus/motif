# Feed the UIL compiler inputs that used to overflow its fixed size
# buffers or hang it, and check that it reports them as ordinary
# diagnostics.  Run with:
#
#   cmake -DUIL=<path to uil> -DWORK_DIR=<scratch directory> -P uil_robustness.cmake
#
# The checks are most useful in a build with -fsanitize=address,undefined,
# where any sanitizer report makes the test fail.

if(NOT UIL OR NOT WORK_DIR)
  message(FATAL_ERROR "UIL and WORK_DIR must be set")
endif()

file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${WORK_DIR}")

set(failures 0)

# uil_case(<name> <expected rc: 0, 1 or ANY> <regex the output must match>
#          [FORBID <regex the output must not match>]
#          [ENV var=value...] ARGS <uil arguments>...)
function(uil_case name expect_rc expect_regex)
  cmake_parse_arguments(C "" "FORBID" "ENV;ARGS" ${ARGN})
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env ASAN_OPTIONS=detect_leaks=0 ${C_ENV}
            ${UIL} ${C_ARGS}
    WORKING_DIRECTORY "${WORK_DIR}"
    RESULT_VARIABLE rc
    OUTPUT_VARIABLE out
    ERROR_VARIABLE out
    TIMEOUT 60)
  set(ok TRUE)
  if(NOT rc MATCHES "^[0-9]+$")
    set(ok FALSE)                       # timeout or signal
  elseif(NOT expect_rc STREQUAL "ANY" AND NOT rc EQUAL expect_rc)
    set(ok FALSE)
  endif()
  if(out MATCHES "Sanitizer|runtime error:")
    set(ok FALSE)
  endif()
  if(NOT expect_regex STREQUAL "" AND NOT out MATCHES "${expect_regex}")
    set(ok FALSE)
  endif()
  if(C_FORBID AND out MATCHES "${C_FORBID}")
    set(ok FALSE)
  endif()
  if(ok)
    message(STATUS "PASS ${name}")
  else()
    message(STATUS "FAIL ${name} (rc=${rc})\n${out}")
    math(EXPR f "${failures} + 1")
    set(failures ${f} PARENT_SCOPE)
  endif()
endfunction()

# A UIL string literal of the given text, continued over several lines
# (source lines are limited to 131 characters).
function(uil_string out text)
  string(LENGTH "${text}" len)
  set(result "'")
  set(pos 0)
  while(pos LESS len)
    string(SUBSTRING "${text}" ${pos} 100 part)
    if(pos GREATER 0)
      string(APPEND result "\\\n")
    endif()
    string(APPEND result "${part}")
    math(EXPR pos "${pos} + 100")
  endwhile()
  string(APPEND result "'")
  set(${out} "${result}" PARENT_SCOPE)
endfunction()

string(REPEAT "A" 300 a300)
string(REPEAT "./" 140 dots)
string(REPEAT "d" 200 d200)
string(REPEAT "q" 200 q200)
string(REPEAT "v" 300 v300)
string(REPEAT "1" 125 ones125)
string(REPEAT "a" 128 a128)
string(REPEAT "0" 127 zeros127)
string(REPEAT "@" 120 at120)
string(REPEAT "X" 400 x400)

file(WRITE "${WORK_DIR}/sub.uih" "value v : 1;\n")
file(WRITE "${WORK_DIR}/inc_ok.uil" "module m\ninclude file 'sub.uih';\nend module;\n")
file(WRITE "${WORK_DIR}/syntax.uil" "module m\nvalue x : ;\nend module;\n")

# Include file names longer than the buffers that hold them.
uil_string(s "${a300}.uih")
file(WRITE "${WORK_DIR}/inc_long.uil" "module m\ninclude file\n${s};\nend module;\n")
uil_case(include_name 1 "invalid include file name" ARGS -o a.uid inc_long.uil)

uil_string(s "B${d200}.uih")
file(WRITE "${WORK_DIR}/inc_dir.uil" "module m\ninclude file\n${s};\nend module;\n")
uil_case(include_dir 1 "error opening source file"
         ARGS -o a.uid -I${WORK_DIR}/${d200} inc_dir.uil)

# Long main file, output file and diagnostic location paths.
uil_case(main_file_name 1 "error opening source file"
         ARGS -o a.uid ${dots}syntax.uil)
string(SUBSTRING "${dots}" 0 230 dots230)
uil_case(diagnostic_location 1 "file: \\./\\./"
         ARGS -o a.uid ${dots230}syntax.uil)
uil_case(output_file_name 0 "" ARGS -o ${dots}out.uid inc_ok.uil)

# Listings: long module version, long message, long user argument name.
uil_string(s "${v300}")
file(WRITE "${WORK_DIR}/version.uil" "module m version =\n${s}\nend module;\n")
uil_case(listing_title ANY "" ARGS -o a.uid -v a.lis version.uil)

file(WRITE "${WORK_DIR}/message.uil" "module m\n${at120}\nend module;\n")
uil_case(listing_message 1 "unknown sequence" ARGS -o a.uid -v a.lis message.uil)

uil_string(s "${q200}")
file(WRITE "${WORK_DIR}/userarg.uil"
  "module m\nvalue userarg : argument(\n${s}, integer);\n"
  "object w : XmLabel { arguments { userarg = 1; }; };\nend module;\n")
uil_case(machine_listing 0 "" ARGS -o a.uid -m -v a.lis userarg.uil)

# Tokens that do not fit in one lex buffer.
file(WRITE "${WORK_DIR}/real.uil" "module m\nvalue r :\n${ones125}.e+x;\nend module;\n")
uil_case(real_backup 1 "unexpected" ARGS -o a.uid real.uil)

file(WRITE "${WORK_DIR}/name.uil" "module m\nvalue\n${a128}bc\n : 1;\nend module;\n")
uil_case(long_name 1 "name exceeds 31 characters" ARGS -o a.uid name.uil)

file(WRITE "${WORK_DIR}/integer.uil" "module m\nvalue v : exported\n1${zeros127}5;\nend module;\n")
uil_case(long_integer 1 "out of range" ARGS -o a.uid integer.uil)

file(WRITE "${WORK_DIR}/int11.uil" "module m\nvalue v : exported\n21474836471;\nend module;\n")
uil_case(integer_digits 1 "out of range" ARGS -o a.uid int11.uil)

# Localized strings used to loop forever.
file(WRITE "${WORK_DIR}/lstr.uil" "module m\nvalue s : #\"abc\nend module;\n")
uil_case(localized_string 1 "not terminated" ARGS -o a.uid -s lstr.uil)

# An empty source file divided by its size of zero.
file(WRITE "${WORK_DIR}/empty.uil" "")
uil_case(empty_file 1 "invalid module structure" ARGS -o a.uid empty.uil)

# A binary operator whose operand is an undeclared name: the second
# operand used to be dereferenced (NULL), and a missing first operand
# made later expressions report bogus circular definitions.
file(WRITE "${WORK_DIR}/undeclared.uil"
  "module m\nobject w : XmLabel { arguments {\n"
  "XmNlabelString = compound_string('a') & nosuch1;\n"
  "XmNx = nosuch2 + 1; XmNy = 2 * 3; }; };\nend module;\n")
uil_case(undeclared_operand 1 "value nosuch2 was never defined"
         FORBID "circularly defined" ARGS -o a.uid undeclared.uil)

# A widget name as an operand, declared before or after its use: the
# widget entry used to be evaluated as a value entry (heap over-read).
file(WRITE "${WORK_DIR}/widget_operand.uil"
  "module m\nobject w1 : XmLabel { arguments { XmNx = 1; }; };\n"
  "object w2 : XmLabel { arguments {\n"
  "XmNx = 1 + w1; XmNlabelString = compound_string('a') & w3; }; };\n"
  "object w3 : XmLabel { arguments { XmNx = 1; }; };\nend module;\n")
uil_case(widget_operand 1 "context requires a value - widget was specified"
         ARGS -o a.uid widget_operand.uil)

# "^" used to compile as "|".
file(WRITE "${WORK_DIR}/xor.uil" "module m\nvalue a : exported 6 ^ 3;\nend module;\n")
uil_case(xor_operator 0 "" ARGS -o a.uid -m -v xor.lis xor.uil)
file(READ "${WORK_DIR}/xor.lis" lis)
# The machine listing shows a 4-byte integer as "value: 5" and an 8-byte
# one (64-bit long) on a line of its own.
if(NOT lis MATCHES "(\n +|value: )5 *\n" OR lis MATCHES "(\n +|value: )7 *\n")
  message(STATUS "FAIL xor_operator: 6 ^ 3 is not 5 in the listing\n${lis}")
  math(EXPR failures "${failures} + 1")
endif()

# Environment and database names.
uil_case(lang_codeset 1 "unknown character set"
         ENV LANG=en_US.${x400} ARGS -o a.uid inc_ok.uil)
uil_case(wmdpath 1 "error opening database file"
         ENV WMDPATH=${WORK_DIR}/none/%N%S ARGS -o a.uid -wmd foo inc_ok.uil)
uil_case(wmd_absolute 1 "error opening database file"
         ARGS -o a.uid -wmd ${WORK_DIR}/none.wmd inc_ok.uil)
uil_case(wmd_short_name 1 "error opening database file"
         ARGS -o a.uid -wmd ab inc_ok.uil)

if(failures GREATER 0)
  message(FATAL_ERROR "${failures} UIL robustness case(s) failed")
endif()
