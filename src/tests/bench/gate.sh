#!/bin/sh
#
# Motif
#
# Licensed under the LGPL 2.1 license.
#
# The benchmark regression gate: build xmbench from two source trees and
# compare them with bench.py, on the same machine, in the same run.
#
#   gate.sh BASE_SRC HEAD_SRC [OUT_DIR]
#
# Two passes, both failing on any increase of a round trip count:
#   time      xmbench on a Unix socket, the CPU cost (GATE_TIME_ARGS); it
#             also fails on a median time per operation more than
#             GATE_THRESHOLD % (default 5) and 1 ns slower, if the rounds
#             show that it is not noise (see bench.py);
#   latency   through xmbench-proxy with 2 ms each way, where every round
#             trip costs 4 ms, at a smaller scale (GATE_LATENCY_ARGS).
#             Its times are reported but not gated: at that scale, the
#             cases that make no round trip run too briefly to be timed
#             to 5 %, and the others are already gated by their counts.
# The results (base.json and head.json of each pass) go to OUT_DIR
# (default bench-results).  BUILD_DIR (default _build-bench) holds the
# two builds; CMAKE_ARGS adds configure arguments.  Exits with 0 when
# the base has no xmbench yet: there is nothing to compare.

set -eu

[ $# -ge 2 ] || { echo "usage: gate.sh BASE_SRC HEAD_SRC [OUT_DIR]" >&2; exit 2; }
base_src=$1
head_src=$2
out=${3:-bench-results}
build_dir=${BUILD_DIR:-_build-bench}
threshold=${GATE_THRESHOLD:-5}
time_args=${GATE_TIME_ARGS:---rounds 5 --repeat 3 --scale 0.5}
latency_args=${GATE_LATENCY_ARGS:---rounds 5 --repeat 1 --scale 0.05}

launcher=
if command -v ccache >/dev/null 2>&1; then
  launcher=-DCMAKE_C_COMPILER_LAUNCHER=ccache
fi
generator="Unix Makefiles"
if command -v ninja >/dev/null 2>&1; then
  generator=Ninja
fi
jobs=$(nproc 2>/dev/null || echo 2)

# build NAME SRC TARGETS...: configure and build the bench of SRC.
build() {
  name=$1 src=$2
  shift 2
  echo "::group::Build the $name bench"
  # shellcheck disable=SC2086 # word splitting of the arguments is intended
  cmake -S "$src" -B "$build_dir/$name" -G "$generator" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo -DWITH_TESTS=ON \
    $launcher ${CMAKE_ARGS:-} >"$build_dir/$name.log" 2>&1 &&
    cmake --build "$build_dir/$name" --parallel "$jobs" --target "$@" \
      >>"$build_dir/$name.log" 2>&1
  status=$?
  echo "::endgroup::"
  return $status
}

mkdir -p "$build_dir" "$out"
if ! build head "$head_src" xmbench xmbench_preload xmbench-proxy; then
  cat "$build_dir/head.log" >&2
  exit 2
fi
if ! build base "$base_src" xmbench xmbench_preload; then
  if ! grep -q 'add_executable(xmbench' "$base_src/src/tests/bench/CMakeLists.txt" 2>/dev/null; then
    echo "gate.sh: the base has no xmbench, nothing to compare"
    exit 0
  fi
  cat "$build_dir/base.log" >&2
  exit 2
fi

bench="python3 $head_src/src/tests/bench/bench.py"
status=0
for pass in time latency; do
  if [ $pass = time ]; then
    args="--threshold $threshold $time_args"
  else
    args="--counts-only --delay 2 $latency_args"
  fi
  echo "::group::xmbench $pass pass"
  # shellcheck disable=SC2086
  $bench gate --label "$pass" $args -o "$out/$pass" \
    "$build_dir/base" "$build_dir/head" || status=1
  echo "::endgroup::"
done
exit $status
