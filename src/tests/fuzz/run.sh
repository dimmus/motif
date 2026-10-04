#!/bin/sh
# Run the Motif fuzzers for a while each.
#
#   src/tests/fuzz/run.sh <build dir> [seconds per target] [target...]
#
# The build must be configured with -DWITH_TESTS=ON -DWITH_FUZZERS=ON
# (Clang), preferably with -DWITH_COMPILER_ASAN=ON -DWITH_UBSAN=ON.  The
# corpus grows in <build dir>/src/tests/fuzz/work/<target>/corpus,
# seeded from the corpus/ directory here; crashes are written to
# work/<target>/ as crash-*, leak-*, oom-* or timeout-* files.  Copy
# reproducers worth keeping into crashes/<target>/ so that Fuzz.<target>
# replays them.  Targets that need an X server run under xvfb-run.

set -u

if [ $# -lt 1 ]; then
	echo "usage: $0 <build dir> [seconds per target] [target...]" >&2
	exit 2
fi
build=$(cd "$1" && pwd) || exit 2
seconds=${2:-60}
shift
[ $# -gt 0 ] && shift
here=$(cd "$(dirname "$0")" && pwd)
src=$(cd "$here/../../.." && pwd)
bin=$build/src/tests/fuzz
work=$bin/work

targets="$*"
if [ -z "$targets" ]; then
	targets=$(cd "$bin" && ls fuzz_* 2>/dev/null | sed 's/^fuzz_//')
fi
[ -n "$targets" ] || { echo "no fuzzers in $bin" >&2; exit 2; }

addr2line=$(command -v addr2line || true)
if [ -z "$(command -v llvm-symbolizer || true)" ] && [ -n "$addr2line" ]; then
	ASAN_SYMBOLIZER_PATH=$addr2line
	export ASAN_SYMBOLIZER_PATH
fi
LSAN_OPTIONS=suppressions=$here/lsan.supp:print_suppressions=0
export LSAN_OPTIONS

# libX11's quark lookup (_XrmInternalStringToQuark, reached through
# XrmGetStringDatabase and XrmStringToQuark) compares a new name with
# memcmp() over the new name's length against an interned name that can
# be shorter.  ASan's default strict_memcmp=1 reports that as an
# over-read of the shorter name although the bytes differ before its
# end; which names collide depends on everything interned earlier in
# the run, so the reports cannot be replayed.  Only check memcmp up to
# the first difference.
ASAN_OPTIONS_SAVED=${ASAN_OPTIONS:-strict_memcmp=0}
status=0
for t in $targets; do
	exe=$bin/fuzz_$t
	[ -x "$exe" ] || { echo "skip $t: no $exe" >&2; continue; }
	corpus=$work/$t/corpus
	mkdir -p "$corpus"
	seeds=""
	# Not crashes/: those reproducers stop the run at once.
	[ -d "$here/corpus/$t" ] && seeds="$here/corpus/$t"
	case $t in
	png|jpeg|svg) seeds="$seeds $src/src/tests/$t" ;;
	esac
	opts=""
	ASAN_OPTIONS=${ASAN_OPTIONS_SAVED-}
	case $t in
	uil|clipboard)	# leak on every input, see CMakeLists.txt
		opts=-detect_leaks=0
		ASAN_OPTIONS=detect_leaks=0 ;;
	esac
	export ASAN_OPTIONS
	echo "== $t (${seconds}s)"
	case $t in
	png|jpeg|svg|xpm|xmstring|uil)
		(cd "$work/$t" && "$exe" -max_total_time="$seconds" -timeout=25 \
			-rss_limit_mb=2048 -print_final_stats=1 $opts \
			-artifact_prefix="$work/$t/" "$corpus" $seeds) \
			> "$work/$t.log" 2>&1 ;;
	*)
		(cd "$work/$t" && xvfb-run -a -s "-screen 0 1280x1024x24" \
			"$exe" -max_total_time="$seconds" -timeout=25 \
			-rss_limit_mb=2048 -print_final_stats=1 $opts \
			-artifact_prefix="$work/$t/" "$corpus" $seeds) \
			> "$work/$t.log" 2>&1 ;;
	esac
	rc=$?
	grep -E "^#[0-9]+.*DONE|stat::number_of_executed_units|stat::peak_rss" "$work/$t.log"
	if [ $rc -ne 0 ]; then
		echo "!! $t exited with $rc, see $work/$t.log"
		ls "$work/$t" | grep -E '^(crash|leak|timeout|oom)-' | sed 's/^/   /'
		status=1
	fi
done
exit $status
