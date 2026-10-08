#!/bin/sh
#
# Configure, build and test Motif the way CI does.  POSIX sh, so that it
# runs unchanged on glibc and musl Linux, in emulated containers and on
# FreeBSD.  Run it from the top of the source tree.
#
# Environment:
#   CC                  C compiler (default: cc)
#   MOTIF_CI_PROFILE    debug (default), debug-asan, release, release-lto
#   BUILD_DIR           build directory (default: _build)
#   CMAKE_ARGS          extra arguments for the cmake configure step
#   MOTIF_CI_TESTS      1 (default) to run ctest after building, 0 not to
#   JOBS                parallel jobs (default: number of CPUs)
#   MOTIF_CI_NO_SKIP    1 to fail when ctest reports a skipped test
#   MOTIF_CI_TEST_TIMEOUT  per-test timeout in seconds (default: 300)
#   MOTIF_CI_READONLY_SRC  1 to make the source tree read-only while
#                       configuring, building and testing, so that a
#                       write into it fails the job (default: 0)
#
# The tests are run with ctest --no-tests=error, so a configuration that
# registers no tests fails instead of reporting success.  CTest itself
# starts each X11 suite under a server of its own: xvfb-run when that
# was found at configure time, and where there is no xvfb-run (FreeBSD)
# but an Xvfb binary, src/tests/xvfb-run.sh.  The ctest output is kept in
# $BUILD_DIR/ctest-output.log for upload as a CI artifact.

set -eu

: "${CC:=cc}"
: "${MOTIF_CI_PROFILE:=debug}"
: "${BUILD_DIR:=_build}"
: "${CMAKE_ARGS:=}"
: "${MOTIF_CI_TESTS:=1}"
if [ -z "${JOBS:-}" ]; then
  JOBS=$(getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)
fi
export CC

die() {
  echo "build.sh: $*" >&2
  exit 1
}

case "$MOTIF_CI_PROFILE" in
  debug)
    profile_args="-DCMAKE_BUILD_TYPE=Debug"
    ;;
  debug-asan)
    profile_args="-DCMAKE_BUILD_TYPE=Debug -DWITH_COMPILER_ASAN=ON -DWITH_UBSAN=ON"
    ;;
  release)
    profile_args="-DCMAKE_BUILD_TYPE=Release"
    ;;
  release-lto)
    profile_args="-DCMAKE_BUILD_TYPE=Release -DWITH_LTO=ON"
    # Clang's LTO objects need a linker that understands LLVM bitcode;
    # GNU ld only does with the LLVMgold plugin, which is often missing.
    if "$CC" --version 2>/dev/null | grep -q clang && command -v ld.lld >/dev/null 2>&1; then
      profile_args="$profile_args -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld"
      profile_args="$profile_args -DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=lld"
      profile_args="$profile_args -DCMAKE_MODULE_LINKER_FLAGS=-fuse-ld=lld"
    fi
    ;;
  *)
    die "unknown MOTIF_CI_PROFILE '$MOTIF_CI_PROFILE'"
    ;;
esac

launcher=
if command -v ccache >/dev/null 2>&1; then
  launcher="-DCMAKE_C_COMPILER_LAUNCHER=ccache"
  ccache -z >/dev/null 2>&1 || true
fi

generator="Unix Makefiles"
if command -v ninja >/dev/null 2>&1; then
  generator=Ninja
fi

# The build must not write into the source tree (generated files belong
# in the build directory).  With MOTIF_CI_READONLY_SRC=1 everything at
# the top of the tree except .git and the entries that hold the build or
# ccache directory is made read-only until this script exits.  (This
# proves nothing when run as root, which ignores the permissions.)
ro_list=
restore_source_tree() {
  if [ -n "$ro_list" ]; then
    chmod u+w .
    while IFS= read -r entry; do
      chmod -R u+w "$entry"
    done <"$ro_list"
    rm -f "$ro_list"
  fi
}
if [ "${MOTIF_CI_READONLY_SRC:-0}" = 1 ]; then
  build_abs=$(mkdir -p "$BUILD_DIR" && cd "$BUILD_DIR" && pwd)
  ccache_abs=
  if [ -n "${CCACHE_DIR:-}" ]; then
    ccache_abs=$(mkdir -p "$CCACHE_DIR" && cd "$CCACHE_DIR" && pwd)
  fi
  ro_list=$(mktemp)
  for entry in * .[!.]*; do
    [ -e "$entry" ] || continue
    [ "$entry" = .git ] && continue
    case "$build_abs/" in "$(pwd)/$entry"/*) continue ;; esac
    case "${ccache_abs:+$ccache_abs/}" in "$(pwd)/$entry"/*) continue ;; esac
    echo "$entry" >>"$ro_list"
  done
  trap restore_source_tree EXIT
  while IFS= read -r entry; do
    chmod -R a-w "$entry"
  done <"$ro_list"
  chmod a-w .
  echo "build.sh: source tree is read-only until the end of this run"
fi

echo "::group::Configure ($MOTIF_CI_PROFILE, $CC)"
# shellcheck disable=SC2086 # word splitting of the argument lists is intended
cmake -S . -B "$BUILD_DIR" -G "$generator" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DWITH_TESTS=ON \
  $launcher $profile_args $CMAKE_ARGS
echo "::endgroup::"

# A profile that silently builds without the instrumentation it promises
# would make CI report a coverage it does not have.
case "$MOTIF_CI_PROFILE" in
  debug-asan)
    grep -q -e '-fsanitize=[a-z,]*address' "$BUILD_DIR/compile_commands.json" ||
      die "WITH_COMPILER_ASAN=ON did not add -fsanitize=address to the compile commands"
    grep -q -e '-fsanitize=[a-z,]*undefined' "$BUILD_DIR/compile_commands.json" ||
      die "WITH_UBSAN=ON did not add -fsanitize=undefined to the compile commands"
    ;;
  release-lto)
    grep -q -e '-flto' "$BUILD_DIR/compile_commands.json" ||
      die "WITH_LTO=ON did not add -flto to the compile commands"
    ;;
esac

echo "::group::Build"
# The code generators run during the build (makestrs, wml, ...) are built
# with the same sanitizer flags; a leak at their exit would fail the
# build.  Leak checking is for the test run below.
ASAN_OPTIONS=${ASAN_OPTIONS:+$ASAN_OPTIONS:}detect_leaks=0 \
  cmake --build "$BUILD_DIR" --parallel "$JOBS"
echo "::endgroup::"

if command -v ccache >/dev/null 2>&1; then
  ccache -s || true
fi

if [ "$MOTIF_CI_TESTS" != 1 ]; then
  exit 0
fi

# Sanitizer settings for the test run; harmless when not instrumented.
export ASAN_OPTIONS="${ASAN_OPTIONS:-detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1:abort_on_error=1}"
export UBSAN_OPTIONS="${UBSAN_OPTIONS:-print_stacktrace=1:halt_on_error=1}"
# Leaks are reported (LeakSanitizer is on by default with ASan); known
# leaks outside Motif's control can be listed in src/tests/lsan.supp.
if [ -z "${LSAN_OPTIONS:-}" ] && [ -f src/tests/lsan.supp ]; then
  LSAN_OPTIONS="suppressions=$(pwd)/src/tests/lsan.supp:print_suppressions=0"
  export LSAN_OPTIONS
fi

log="$BUILD_DIR/ctest-output.log"
{
  rc=0
  ctest --test-dir "$BUILD_DIR" --no-tests=error --output-on-failure \
    --parallel "$JOBS" --timeout "${MOTIF_CI_TEST_TIMEOUT:-300}" || rc=$?
  echo "$rc" >"$BUILD_DIR/ctest-status"
} 2>&1 | tee "$log"
status=$(cat "$BUILD_DIR/ctest-status")

# Where an X server is available nothing may be skipped: a suite that
# skips itself (exit 77) there means the CI setup is broken.
if [ "$status" = 0 ] && [ "${MOTIF_CI_NO_SKIP:-0}" = 1 ] &&
   grep -q '(Skipped)' "$log"; then
  die "tests were skipped although MOTIF_CI_NO_SKIP=1"
fi
exit "$status"
