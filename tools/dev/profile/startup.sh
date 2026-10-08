#!/bin/sh
# Startup cost of hello_motif for several Motif versions.
#
#   startup.sh [-r RUNS] PREFIX...
#
# Each PREFIX is a Motif installed by build.sh, with hello_motif in
# PREFIX/bin.  On one Xvfb, for each version, this prints:
#
#   wall-ms     wall time of a start (median of RUNS, default 30)
#   cpu-ms      CPU time of the process, from perf stat's task-clock
#               (median of RUNS)
#   faults      page faults (perf stat, median)
#   ld-Mcyc     LD_DEBUG=statistics: total startup time in the dynamic
#               loader, in millions of TSC cycles (median)
#   rel-Mcyc    the part of it spent relocating (median)
#   lookups     symbol relocations the loader resolved by a lookup, and
#   cached      ... from its cache, both counted at exit ("final"), so
#               that lazily bound PLT entries are included
#   relative    relative relocations
#   Ir          instructions executed (callgrind), which unlike the times
#               do not depend on the other load on the machine
#   Ir-ld.so    the part of them in the dynamic loader
#   requests    X requests, from an xtrace log (xtrace.awk)
#   rtrips      round trips to the X server and
#   mallocs     calls to malloc/calloc/realloc, both from
#               libxmbench_preload (PRELOAD, default: the first prefix's)
#
# perf stat counts only task-clock and page faults: the virtual machine
# this was written on has no hardware counters (perf stat -e instructions
# says <not supported>), so callgrind counts the instructions.  The timed
# runs go round robin over the versions.
set -eu

here=$(cd "$(dirname "$0")" && pwd)

runs=30
if [ "${1:-}" = -r ]; then
	runs=$2
	shift 2
fi
preload=${PRELOAD:-$1/bin/libxmbench_preload.so}
export XMODIFIERS=@im=none

tmp=$(mktemp -d)
mkfifo "$tmp/fd"
Xvfb -displayfd 3 -nolisten tcp -noreset -screen 0 1280x1024x24 +extension RENDER \
	3>"$tmp/fd" >/dev/null 2>&1 &
xvfb=$!
trap 'kill $xvfb 2>/dev/null; wait $xvfb 2>/dev/null; rm -rf "$tmp"' EXIT
read -r num <"$tmp/fd"
export DISPLAY=:$num

median() {
	sort -n | awk '{ v[NR] = $1 } END {
		if (NR % 2) print v[(NR + 1) / 2];
		else print (v[NR / 2] + v[NR / 2 + 1]) / 2 }'
}

# ldstat PATTERN: the median over the runs of the number after PATTERN
# in the loader's statistics.  The lines look like
#   "  12345:	  total startup time in dynamic loader: 1748671 cycles".
ldstat() {
	grep -h ":[[:space:]]*$2: " "$tmp/$1".ld.* |
		sed 's/.*: *\([0-9][0-9]*\).*/\1/' | median
}

# The timed runs go round robin over the versions, so that a change of
# the load on the machine during the measurement does not favour one.
for prefix in "$@"; do
	"$prefix/bin/hello_motif"
done
i=0
while [ $i -lt "$runs" ]; do
	for prefix in "$@"; do
		v=$(basename "$prefix")
		exe=$prefix/bin/hello_motif
		LD_DEBUG=statistics LD_DEBUG_OUTPUT="$tmp/$v.ld" "$exe"
		perf stat -x, -e task-clock,page-faults -o "$tmp/perf" -- "$exe"
		awk -F, '/task-clock/ { print $1 }' "$tmp/perf" >>"$tmp/$v.cpu"
		awk -F, '/page-faults/ { print $1 }' "$tmp/perf" >>"$tmp/$v.faults"
		s=$(date +%s%N)
		"$exe"
		e=$(date +%s%N)
		echo $(((e - s) / 1000)) >>"$tmp/$v.wall"
	done
	i=$((i + 1))
done

printf '%-8s %8s %7s %6s %7s %8s %7s %6s %8s %11s %10s %8s %6s %7s\n' \
	version wall-ms cpu-ms faults ld-Mcyc rel-Mcyc lookups cached \
	relative Ir Ir-ld.so requests rtrips mallocs
for prefix in "$@"; do
	v=$(basename "$prefix")
	exe=$prefix/bin/hello_motif

	valgrind --tool=callgrind --callgrind-out-file="$tmp/cg" "$exe" \
		>/dev/null 2>&1
	callgrind_annotate --threshold=100 "$tmp/cg" >"$tmp/cg.txt"
	ir=$(awk '/PROGRAM TOTALS/ { gsub(",", "", $1); print $1 }' "$tmp/cg.txt")
	irld=$(awk '/\[.*\/ld-linux[^]]*\]$/ { gsub(",", "", $1); s += $1 }
		END { print s + 0 }' "$tmp/cg.txt")

	cnt=$(XMBENCH_REPORT=1 LD_PRELOAD=$preload "$exe" 2>&1 |
		sed -n 's/^xmbench_preload: //p')

	fake=:$((num + 100))
	rm -f "$tmp/xtrace"
	xtrace -n -k -o "$tmp/xtrace" -d "$DISPLAY" -D "$fake" \
		>/dev/null 2>&1 &
	xtrace_pid=$!
	sleep 1
	DISPLAY=$fake "$exe"
	sleep 0.5
	kill $xtrace_pid
	wait $xtrace_pid 2>/dev/null || true
	awk -f "$here/xtrace.awk" "$tmp/xtrace" >"$tmp/xtrace.txt"

	printf '%-8s %8.1f %7.1f %6d %7.2f %8.2f %7d %6d %8d %11d %10d %8d %6d %7d\n' \
		"$v" "$(median <"$tmp/$v.wall" | awk '{ print $1 / 1000 }')" \
		"$(median <"$tmp/$v.cpu")" "$(median <"$tmp/$v.faults")" \
		"$(ldstat "$v" 'total startup time in dynamic loader' |
			awk '{ print $1 / 1e6 }')" \
		"$(ldstat "$v" 'time needed for relocation' |
			awk '{ print $1 / 1e6 }')" \
		"$(ldstat "$v" 'final number of relocations')" \
		"$(ldstat "$v" 'final number of relocations from cache')" \
		"$(ldstat "$v" 'number of relative relocations')" \
		"$ir" "$irld" \
		"$(awk '/^requests/ { print $2 }' "$tmp/xtrace.txt")" \
		"$(echo "$cnt" | awk '{ print $6 }')" \
		"$(echo "$cnt" | awk '{ print $2 }')"
done
