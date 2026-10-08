#!/bin/sh
#
# Compare the ABI of libXm, libMrm and libUil built from the current tree
# with the ABI of the same libraries built from older git revisions, or
# from upstream Motif 2.3.8, using libabigail.  Run it from the top of a
# git checkout that contains the revisions (CI checks out with
# fetch-depth: 0).
#
# Usage: abi-check.sh REF...
#
# Each REF is exported with "git archive" (the checkout itself is not
# touched) and built with CMake, or with autotools for revisions from
# before the CMake conversion.  Both sides are installed into a staging
# directory, so that abidw and abidiff see the installed headers and
# leave out the types that are private to the library.  Three things are
# compared:
#
#   - each library: its exported functions and variables and the types
#     they reach;
#   - the types of the installed headers, also those that no exported
#     function reaches: that is where the instance and class records of
#     the *P.h headers are, whose layout every subclass compiles in.  The
#     debug information of a library only describes the types that its
#     own sources use, so every installed header is compiled into one
#     object, with all its types in the debug information, on each side;
#   - the string tables _XmStrings, _XmStrings22 and _XmStrings23, whose
#     offsets programs compile in (abidiff does not look into them).
#
# The REF "upstream-2.3.8" is the upstream release: its tarball is
# downloaded from SourceForge, checked against a pinned SHA-256 and built
# with its own configure script into $WORK_DIR/upstream-2.3.8, where later
# runs reuse it (CI caches that directory).  This tree is deliberately not
# binary compatible with it (doc/abi-policy.md), so that comparison
#   - ignores the SONAME (4 upstream, 5 here),
#   - only considers the symbols that the version scripts
#     (src/lib/*/lib*.map) export: the others are local on purpose,
#   - does not compare the widget instance records and their parts, which
#     all differ (generated suppression, see records_suppr),
#   - applies the suppressions in tools/dev/env/ci/abi/upstream-2.3.8.suppr,
#     the other documented differences,
#   - reports, but does not fail on, the re-ordered string tables.
# Anything else it reports is a new difference from upstream.
#
# The dumps and reports are left in $WORK_DIR (default: _abi) for upload
# as CI artifacts.  Each comparison prints its abidiff status: 0 when
# nothing changed, 4 for compatible changes, 12 for incompatible ones, and
# 1 or 2 for errors.  The script fails (status 1) on an incompatible
# change, a moved string or an error; a compatible change, such as a new
# member in the tail padding of a structure, is reported but does not
# fail it.  Under GitHub Actions the results are also posted as
# annotations.
#
# Environment: CC, the compiler for the header types object (default cc).

set -eu

work=${WORK_DIR:-_abi}
jobs=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 2)
libs="Xm Mrm Uil"
top=$(pwd)
here=$(cd "$(dirname "$0")" && pwd)
: "${CC:=cc}"

# What "upstream-2.3.8" stands for.  The configuration is upstream's
# default (which defines OM22_COMPATIBILITY) except for printing, which
# needs libXp, no longer shipped by most distributions, and is off by
# default in this tree too.  The K&R code of 2.3.8 needs C89 rules with
# current compilers, and its demos link only without --as-needed (the
# default of Ubuntu's linker).
upstream=upstream-2.3.8
upstream_url="https://downloads.sourceforge.net/project/motif/Motif%202.3.8%20Source%20Code/motif-2.3.8.tar.gz"
upstream_sha256=859b723666eeac7df018209d66045c9853b50b4218cecadb794e2359619ebce7
upstream_configure="--disable-printing --enable-xft --enable-jpeg --enable-png\
 LDFLAGS=-Wl,--no-as-needed"
upstream_cflags="-g -O0 -std=gnu89 -fcommon -Wno-error=implicit-function-declaration\
 -Wno-error=implicit-int -Wno-error=incompatible-pointer-types -Wno-error=int-conversion"

if [ $# -eq 0 ]; then
  echo "usage: $0 REF..." >&2
  exit 2
fi

mkdir -p "$work"
work=$(cd "$work" && pwd)

# annotate LEVEL MESSAGE: a GitHub Actions annotation (error, warning,
# notice).
annotate() {
  if [ -n "${GITHUB_ACTIONS:-}" ]; then
    echo "::$1 title=ABI check::$2"
  fi
}

# build_tree SRCDIR NAME [CFLAGS [CONFIGURE_ARGS]]: build SRCDIR and
# install it into $work/NAME/root.  CFLAGS and CONFIGURE_ARGS apply to
# autotools trees only.
build_tree() {
  srcdir=$1
  dest=$work/$2
  rm -rf "$dest"
  mkdir -p "$dest"
  if [ -f "$srcdir/CMakeLists.txt" ]; then
    cmake -S "$srcdir" -B "$dest/build" -G Ninja \
      -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr \
      -DWITH_DEMOS=OFF -DWITH_DOCS=OFF -DWITH_TESTS=OFF \
      >"$dest/configure.log" 2>&1 || { cat "$dest/configure.log"; return 1; }
    cmake --build "$dest/build" --parallel "$jobs" >"$dest/build.log" 2>&1 ||
      { tail -n 50 "$dest/build.log"; return 1; }
    DESTDIR="$dest/root" cmake --install "$dest/build" >"$dest/install.log" 2>&1 ||
      { tail -n 50 "$dest/install.log"; return 1; }
  elif [ -f "$srcdir/configure.ac" ]; then
    # A release tarball has a configure script, a git revision does not.
    (
      cd "$srcdir"
      { [ -x configure ] || autoreconf -fi; } &&
        # shellcheck disable=SC2086 # the options are separate words
        ./configure --prefix=/usr CFLAGS="${3:--g -O0}" ${4:-} &&
        make -j"$jobs" && make install DESTDIR="$dest/root"
    ) >"$dest/build.log" 2>&1 || { tail -n 50 "$dest/build.log"; return 1; }
  else
    echo "abi-check.sh: $srcdir has neither CMakeLists.txt nor configure.ac" >&2
    return 1
  fi
}

# build_ref REF NAME: export the git revision REF and build it.
build_ref() {
  rm -rf "$work/src-$2"
  mkdir -p "$work/src-$2"
  git archive --format=tar -o "$work/src-$2.tar" "$1"
  tar -x -C "$work/src-$2" -f "$work/src-$2.tar"
  rm -f "$work/src-$2.tar"
  build_tree "$work/src-$2" "$2"
}

# build_upstream NAME: download, check and build the upstream release,
# unless $work/NAME already holds a build of the same tarball with the
# same options.
build_upstream() {
  stamp="$upstream_sha256 $upstream_configure $upstream_cflags"
  if [ -d "$work/$1/root" ] && [ "$(cat "$work/$1/stamp" 2>/dev/null)" = "$stamp" ]; then
    echo "=== $1: reusing the build in $work/$1"
    return 0
  fi
  tarball=$work/motif-2.3.8.tar.gz
  if ! echo "$upstream_sha256  $tarball" | sha256sum -c - >/dev/null 2>&1; then
    curl -fsSL --retry 3 -o "$tarball" "$upstream_url"
    echo "$upstream_sha256  $tarball" | sha256sum -c - || return 1
  fi
  rm -rf "$work/src-$1"
  mkdir -p "$work/src-$1"
  tar -x -z -C "$work/src-$1" --strip-components=1 -f "$tarball"
  build_tree "$work/src-$1" "$1" "$upstream_cflags" "$upstream_configure" ||
    return 1
  rm -rf "$work/src-$1"
  echo "$stamp" >"$work/$1/stamp"
}

# dump_abi NAME: write $work/NAME/lib<lib>.abi for each library that the
# tree installs as a shared library (older revisions had a static
# libUil), and $work/NAME/types.abi, the types of its installed headers.
dump_abi() {
  root=$work/$1/root
  for lib in $libs; do
    rm -f "$work/$1/lib$lib.abi"
    so=$(find "$root" -name "lib$lib.so.*" -type f | head -n 1)
    if [ -z "$so" ]; then
      if [ "$1" = new ]; then
        echo "abi-check.sh: lib$lib.so not installed" >&2
        return 1
      fi
      continue
    fi
    abidw --headers-dir "$root/usr/include" --drop-private-types \
      --out-file "$work/$1/lib$lib.abi" "$so"
  done
  # Every installed header in one object; -fno-eliminate-unused-debug-types
  # puts the types that nothing uses into its debug information too.  A
  # header that does not compile here on its own (Print.h needs libXp's
  # headers) is left out.
  cflags="-I$root/usr/include $(pkg-config --cflags xft xt x11 2>/dev/null)"
  : >"$work/$1/types.c"
  for h in $(cd "$root/usr/include" && ls Xm/*.h Mrm/*.h uil/*.h); do
    # shellcheck disable=SC2086 # $cflags is a list of options
    if echo "#include <$h>" | $CC -fsyntax-only -w $cflags -x c - 2>/dev/null; then
      echo "#include <$h>" >>"$work/$1/types.c"
    else
      echo "=== $1: $h does not compile on its own here, its types are not compared"
    fi
  done
  # shellcheck disable=SC2086 # $cflags is a list of options
  $CC -g -O0 -w -fno-eliminate-unused-debug-types -fPIC -shared $cflags \
    -o "$work/$1/types.so" "$work/$1/types.c" ||
    { echo "abi-check.sh: the installed headers of $1 do not compile together" >&2; return 1; }
  abidw --load-all-types --headers-dir "$root/usr/include" --drop-private-types \
    --out-file "$work/$1/types.abi" "$work/$1/types.so"
}

# whitelist LIB: write the symbols that src/lib/LIB/libLIB.map exports to
# $work/libLIB.whitelist, in the format of abidiff --kmi-whitelist.
whitelist() {
  {
    echo "[abi_whitelist]"
    sed -e 's|/\*.*\*/||g' -e '/\/\*/,/\*\//d' "$top/src/lib/$1/lib$1.map" |
      sed -n 's/^[[:space:]]*\([A-Za-z_][A-Za-z0-9_]*\);[[:space:]]*$/  \1/p'
  } >"$work/lib$1.whitelist"
}

# records_suppr NAME: write $work/records.suppr, a suppression of the
# widget instance records, and of the parts they consist of, that the
# installed *P.h headers of NAME and of the working tree define: the
# records whose first member is a CorePart, ObjectPart or RectObjPart.
# libabigail drops an "L" after a digit from a type name (_XmI18ListRec
# becomes _XmI18istRec), so both spellings are listed.
records_suppr() {
  names=$(cat "$work/$1/root/usr/include/Xm/"*P.h "$work/new/root/usr/include/Xm/"*P.h |
    sed -e 's|/\*.*\*/||g' |
    awk '
      /^[ \t]*typedef[ \t]+struct[ \t]+_?Xm[A-Za-z0-9]*Rec[ \t{]*$/ {
        tag = $3; sub(/[{].*/, "", tag)
        inrec = 1; first = 1; n = 0; next
      }
      !inrec { next }
      /^[ \t]*[{]?[ \t]*$/ || /^[ \t]*#/ { next }
      /^[ \t]*}/ {
        if (inst) { print tag; for (i = 1; i <= n; i++) print parts[i] }
        name = $0; sub(/^[ \t]*}[ \t]*/, "", name); sub(/[ \t,;].*/, "", name)
        if (inst && name != "") print name
        inrec = 0; inst = 0; next
      }
      {
        type = $1
        if (first) {
          inst = (type == "CorePart" || type == "ObjectPart" || type == "RectObjPart")
          first = 0
        }
        if (type ~ /^_?Xm[A-Za-z0-9]*Part$/) parts[++n] = type
      }' |
    sed -e 's/^_//' -e 'p' -e 's/\([0-9]\)L/\1/' | sort -u | tr '\n' '|' | sed 's/|$//')
  [ -n "$names" ] || { echo "abi-check.sh: no widget records found" >&2; return 1; }
  cat >"$work/records.suppr" <<EOF
# Generated by abi-check.sh from the *P.h headers.
[suppress_type]
  label = widget instance records and parts
  type_kind = struct
  name_regexp = ^_?($names)\$
EOF
}

# strings_of SO SYMBOL: print the strings of the string table SYMBOL of
# the shared library SO, one per line, each after its offset into the
# table.  A program compiles an XmN*, XmC*, XmR* or XmS* name in as such
# an offset ("(char *)&_XmStrings[N]").
strings_of() {
  set -- "$1" "$2" "$(readelf -sW "$1" |
    awk -v s="$2" '$8 == s || index($8, s "@") == 1 { print $2, $3, $7; exit }')"
  # shellcheck disable=SC2086 # value, size and section index
  set -- "$1" "$2" $3
  [ $# -eq 5 ] || return 1
  sect=$(readelf -SW "$1" | sed -n 's/^ *\[ *\([0-9]*\)\] */\1 /p' |
    awk -v n="$5" '$1 == n { print $4, $5; exit }')
  # shellcheck disable=SC2086 # address and file offset of the section
  set -- "$@" $sect
  dd if="$1" bs=1 skip=$((0x$3 - 0x$6 + 0x$7)) count="$4" 2>/dev/null |
    tr '\0' '\n' | LC_ALL=C awk '{ print o, $0; o += length($0) + 1 }' o=0
}

# string_tables NAME REF: compare the string tables of NAME's libXm with
# the working tree's.  Every string must still be at its offset; new
# strings may only be appended.
string_tables() {
  old=$(find "$work/$1/root" -name "libXm.so.*" -type f | head -n 1)
  new=$(find "$work/new/root" -name "libXm.so.*" -type f | head -n 1)
  for table in _XmStrings _XmStrings22 _XmStrings23; do
    strings_of "$old" $table >"$work/$1/$table.txt" || continue
    strings_of "$new" $table >"$work/new/$table.txt" ||
      { echo "abi-check.sh: no $table in the working tree's libXm" >&2; return 1; }
    LC_ALL=C sort "$work/$1/$table.txt" >"$work/$1/$table.sorted"
    LC_ALL=C sort "$work/new/$table.txt" |
      LC_ALL=C comm -23 "$work/$1/$table.sorted" - >"$work/$table-$1.txt"
    moved=$(wc -l <"$work/$table-$1.txt")
    echo "=== libXm: $table: $moved of $(wc -l <"$work/$1/$table.txt") strings of $2 moved"
    if [ "$moved" -ne 0 ]; then
      head -n 20 "$work/$table-$1.txt"
      if [ "$1" = "$upstream" ]; then
        echo "(documented: the tables were re-ordered, doc/abi-policy.md)"
      else
        annotate error "libXm: $moved strings of $table moved compared with $2"
        return 1
      fi
    fi
  done
}

# compare WHAT OLD NEW REF [ABIDIFF_OPTION...]: run abidiff, show and
# annotate the result, and add its status to $status.
compare() {
  what=$1 old=$2 new=$3 ref=$4
  shift 4
  report=$work/$what-$name.txt
  rc=0
  abidiff --no-added-syms "$@" "$old" "$new" >"$report" 2>&1 || rc=$?
  echo "=== $what: $ref -> working tree: abidiff status $rc"
  cat "$report"
  if [ "$rc" -ne 0 ]; then
    if [ $((rc & 8)) -ne 0 ]; then level=error; msg="incompatible ABI changes"
    elif [ "$rc" -eq 4 ]; then level=warning; msg="compatible ABI changes"
    else level=error; msg="abidiff failed (status $rc)"
    fi
    annotate "$level" "$what: $msg compared with $ref; see the job log or the abi artifact"
  fi
  status=$((status | rc))
}

build_tree "$top" new
dump_abi new

status=0
for ref in "$@"; do
  if [ "$ref" = "$upstream" ]; then
    name=$ref
    build=build_upstream
  else
    name=ref-$(echo "$ref" | tr -c 'A-Za-z0-9._\n-' _)
    build="build_ref $ref"
  fi
  # An old revision that no longer builds here is reported, not fatal.
  if ! $build "$name" || ! dump_abi "$name"; then
    echo "=== $ref: could not be built or dumped, skipped"
    annotate error "$ref could not be built or dumped for the ABI comparison"
    status=$((status | 1))
    continue
  fi
  hd="--hd1 $work/$name/root/usr/include --hd2 $work/new/root/usr/include"
  types_suppr="--suppressions $here/abi/headers.suppr"
  if [ "$name" = "$upstream" ]; then
    records_suppr "$name"
    types_suppr="$types_suppr --suppressions $here/abi/$upstream.suppr"
    types_suppr="$types_suppr --suppressions $work/records.suppr"
  fi
  for lib in $libs; do
    if [ ! -f "$work/$name/lib$lib.abi" ]; then
      echo "=== lib$lib: not a shared library in $ref, not compared"
      continue
    fi
    if [ "$name" = "$upstream" ]; then
      whitelist "$lib"
      # shellcheck disable=SC2086 # $hd is a list of options
      compare "lib$lib" "$work/$name/lib$lib.abi" "$work/new/lib$lib.abi" "$ref" \
        $hd --ignore-soname --kmi-whitelist "$work/lib$lib.whitelist" \
        --suppressions "$here/abi/$upstream.suppr"
    else
      # shellcheck disable=SC2086 # $hd is a list of options
      compare "lib$lib" "$work/$name/lib$lib.abi" "$work/new/lib$lib.abi" "$ref" $hd
    fi
  done
  # shellcheck disable=SC2086 # lists of options
  compare headers "$work/$name/types.abi" "$work/new/types.abi" "$ref" \
    --non-reachable-types $hd $types_suppr
  string_tables "$name" "$ref" || status=$((status | 8))
done
# Only errors (1, 2) and incompatible changes (8) fail the check.
[ $((status & 11)) -eq 0 ]
