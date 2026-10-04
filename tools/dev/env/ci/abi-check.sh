#!/bin/sh
#
# Compare the ABI of libXm and libMrm built from the current tree with the
# ABI of the same libraries built from one or more older git revisions,
# using libabigail.  Run it from the top of a git checkout that contains
# the revisions (CI checks out with fetch-depth: 0).
#
# Usage: abi-check.sh REF...
#
# Each REF is exported with "git archive" (the checkout itself is not
# touched) and built with CMake, or with autotools for revisions from
# before the CMake conversion.  Both sides are installed into a staging
# directory, so that abidw sees the installed headers and can drop types
# that are private to the library (--headers-dir, --drop-private-types).
# The ABI dumps and reports are left in $WORK_DIR (default: _abi) for
# upload as CI artifacts.
#
# Each comparison prints its abidiff status: 0 when nothing changed, 4
# for compatible changes, 12 for incompatible ones, and 1 or 2 for errors.
# The script fails (status 1) on an incompatible change or an error; a
# compatible change, such as a new member in the tail padding of a
# structure, is reported but does not fail it.  Under GitHub Actions the
# results are also posted as annotations.

set -eu

work=${WORK_DIR:-_abi}
jobs=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 2)
libs="Xm Mrm"
top=$(pwd)

if [ $# -eq 0 ]; then
  echo "usage: $0 REF..." >&2
  exit 2
fi

mkdir -p "$work"
work=$(cd "$work" && pwd)

# annotate LEVEL MESSAGE: a GitHub Actions annotation (error, warning).
annotate() {
  if [ -n "${GITHUB_ACTIONS:-}" ]; then
    echo "::$1 title=ABI check::$2"
  fi
}

# build_tree SRCDIR NAME: build SRCDIR and install it into $work/NAME/root.
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
    (
      cd "$srcdir"
      autoreconf -fi && ./configure --prefix=/usr CFLAGS="-g -O0" &&
        make -j"$jobs" && make install DESTDIR="$dest/root"
    ) >"$dest/build.log" 2>&1 || { tail -n 50 "$dest/build.log"; return 1; }
  else
    echo "abi-check.sh: $srcdir has neither CMakeLists.txt nor configure.ac" >&2
    return 1
  fi
}

# dump_abi NAME: write $work/NAME/lib<lib>.abi for each library.
dump_abi() {
  root=$work/$1/root
  for lib in $libs; do
    so=$(find "$root" -name "lib$lib.so.*" -type f | head -n 1)
    if [ -z "$so" ]; then
      echo "abi-check.sh: lib$lib.so not installed for $1" >&2
      return 1
    fi
    abidw --headers-dir "$root/usr/include" --drop-private-types \
      --out-file "$work/$1/lib$lib.abi" "$so"
  done
}

build_tree "$top" new
dump_abi new

status=0
for ref in "$@"; do
  name=ref-$(echo "$ref" | tr -c 'A-Za-z0-9._\n-' _)
  rm -rf "$work/src-$name"
  mkdir -p "$work/src-$name"
  git archive --format=tar -o "$work/src-$name.tar" "$ref"
  tar -x -C "$work/src-$name" -f "$work/src-$name.tar"
  rm -f "$work/src-$name.tar"
  # An old revision that no longer builds here is reported, not fatal.
  if ! build_tree "$work/src-$name" "$name" || ! dump_abi "$name"; then
    echo "=== $ref: could not be built or dumped, skipped"
    annotate error "$ref could not be built or dumped for the ABI comparison"
    status=$((status | 1))
    continue
  fi
  for lib in $libs; do
    report=$work/lib$lib-$name.txt
    rc=0
    abidiff --no-added-syms "$work/$name/lib$lib.abi" "$work/new/lib$lib.abi" \
      >"$report" 2>&1 || rc=$?
    echo "=== lib$lib: $ref -> working tree: abidiff status $rc"
    cat "$report"
    if [ "$rc" -ne 0 ]; then
      if [ $((rc & 8)) -ne 0 ]; then level=error; what="incompatible ABI changes"
      elif [ "$rc" -eq 4 ]; then level=warning; what="compatible ABI changes"
      else level=error; what="abidiff failed (status $rc)"
      fi
      annotate "$level" "lib$lib: $what compared with $ref; see the job log or the abi artifact"
    fi
    status=$((status | rc))
  done
done
# Only errors (1, 2) and incompatible changes (8) fail the check.
[ $((status & 11)) -eq 0 ]
