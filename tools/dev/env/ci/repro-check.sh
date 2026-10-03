#!/bin/sh
#
# Check that the build is reproducible: build and install Motif twice from
# the same source tree, with SOURCE_DATE_EPOCH set, and compare the two
# installed trees with diffoscope (or cmp when diffoscope is missing).
# Run it from the top of the source tree.
#
# Usage: repro-check.sh WORK_DIR
#
# Both builds use the same build directory path, because the build still
# embeds absolute paths (see TODO.md 2.3); the second one runs with a
# different time zone, locale, umask and level of parallelism.  ccache is
# disabled, or the second build would only replay the first one.  The
# diffoscope report is left in WORK_DIR/diffoscope.{txt,html}.

set -eu

if [ $# -ne 1 ]; then
  echo "usage: $0 WORK_DIR" >&2
  exit 2
fi
mkdir -p "$1"
work=$(cd "$1" && pwd)
jobs=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 2)
: "${CMAKE_ARGS:=}"

if [ -z "${SOURCE_DATE_EPOCH:-}" ]; then
  SOURCE_DATE_EPOCH=$(git log -1 --format=%ct 2>/dev/null || echo 1700000000)
fi
CCACHE_DISABLE=1
export SOURCE_DATE_EPOCH CCACHE_DISABLE

# build_once DEST JOBS: configure, build and install into $work/DEST.
build_once() {
  rm -rf "$work/build" "$work/$1"
  # shellcheck disable=SC2086 # CMAKE_ARGS is a list
  cmake -S . -B "$work/build" -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr -DWITH_DEMOS=OFF -DWITH_TESTS=OFF \
    $CMAKE_ARGS >"$work/$1.log" 2>&1
  cmake --build "$work/build" --parallel "$2" >>"$work/$1.log" 2>&1
  DESTDIR="$work/$1" cmake --install "$work/build" >>"$work/$1.log" 2>&1
}

echo "SOURCE_DATE_EPOCH=$SOURCE_DATE_EPOCH"
build_once a "$jobs" || { tail -n 50 "$work/a.log"; exit 1; }
(
  umask 0002
  TZ=Pacific/Kiritimati LC_ALL=C.UTF-8 LANG=C.UTF-8
  export TZ LC_ALL LANG
  build_once b 1
) || { tail -n 50 "$work/b.log"; exit 1; }

echo "Installed files: $(find "$work/a" -type f | wc -l)"
if command -v diffoscope >/dev/null 2>&1; then
  if diffoscope --text "$work/diffoscope.txt" --html "$work/diffoscope.html" \
       --exclude-directory-metadata=recursive "$work/a" "$work/b"; then
    echo "repro-check: the two builds are identical"
    exit 0
  fi
  head -n 200 "$work/diffoscope.txt"
else
  echo "repro-check: diffoscope not found, comparing with cmp"
  differ=0
  (cd "$work/a" && find . -type f) | sort >"$work/a.files"
  (cd "$work/b" && find . -type f) | sort >"$work/b.files"
  if ! cmp -s "$work/a.files" "$work/b.files"; then
    diff "$work/a.files" "$work/b.files" || true
    differ=1
  fi
  while read -r f; do
    if [ -f "$work/b/$f" ] && ! cmp -s "$work/a/$f" "$work/b/$f"; then
      echo "differs: $f"
      differ=1
    fi
  done <"$work/a.files"
  if [ "$differ" = 0 ]; then
    echo "repro-check: the two builds are identical"
    exit 0
  fi
fi
echo "repro-check: the two builds differ" >&2
exit 1
