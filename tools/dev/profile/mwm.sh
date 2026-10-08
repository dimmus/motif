#!/bin/sh
# mwm managing many clients, for several Motif versions.
#
#   mwm.sh [-r RUNS] [-n WINDOWS] [-t TITLES] PREFIX...
#
# For each PREFIX (a Motif installed by build.sh), starts mwm on a new
# Xvfb, runs "xclients -n WINDOWS -t TITLES" (default 200 windows, 2000
# title changes) against it and stops mwm; RUNS times (default 5), round
# robin over the versions so that a change of the load on the machine
# does not favour one.  Prints the medians of:
#
#   map-ms     mapping the windows until mwm has reparented all of them
#   title-ms   the title changes (the client's side: until XSync)
#   wdraw-ms   withdrawing the windows one by one
#   cpu-ms     mwm's CPU time over the whole run (perf stat task-clock)
#   mallocs    mwm's calls to malloc/calloc/realloc and
#   rtrips     its round trips, from libxmbench_preload (PRELOAD, default
#              the first prefix's)
set -eu

runs=5 nwin=200 ntitles=2000
while [ $# -gt 0 ]; do
	case $1 in
	-r) runs=$2; shift 2 ;;
	-n) nwin=$2; shift 2 ;;
	-t) ntitles=$2; shift 2 ;;
	*) break ;;
	esac
done
preload=${PRELOAD:-$1/bin/libxmbench_preload.so}

tmp=$(mktemp -d)
xvfb=
cleanup() {
	[ -z "$xvfb" ] || { kill "$xvfb" 2>/dev/null; wait "$xvfb" 2>/dev/null; }
	rm -rf "$tmp"
}
trap cleanup EXIT

median() {
	sort -n | awk '{ v[NR] = $1 } END {
		if (NR % 2) print v[(NR + 1) / 2];
		else print (v[NR / 2] + v[NR / 2 + 1]) / 2 }'
}

# run PREFIX: one run, appending to $tmp/<version>.*
run() {
	v=$(basename "$1")
	mkfifo "$tmp/fd"
	Xvfb -displayfd 3 -nolisten tcp -noreset -screen 0 1280x1024x24 \
		3>"$tmp/fd" >/dev/null 2>&1 &
	xvfb=$!
	read -r num <"$tmp/fd"
	rm -f "$tmp/fd"
	export DISPLAY=:$num

	perf stat -x, -e task-clock -o "$tmp/perf" -- \
		env LD_LIBRARY_PATH="$1/lib" XMBENCH_REPORT=1 \
		LD_PRELOAD="$preload" "$1/bin/mwm" \
		-xrm 'Mwm*showFeedback: -kill' >"$tmp/mwm.log" 2>&1 &
	perf_pid=$!
	until xprop -root _MOTIF_WM_INFO 2>/dev/null | grep -q ' = '; do
		sleep 0.1
	done
	"$1/bin/xclients" -n "$nwin" -t "$ntitles" >"$tmp/clients"
	# perf runs mwm through env, which execs it: signal mwm itself.
	kill -TERM $(pgrep -x mwm)
	wait "$perf_pid" || true

	awk '/^map/ { print $(NF - 1) }' "$tmp/clients" >>"$tmp/$v.map"
	awk '/title changes/ { print $(NF - 1) }' "$tmp/clients" >>"$tmp/$v.title"
	awk '/^withdraw/ { print $(NF - 1) }' "$tmp/clients" >>"$tmp/$v.wdraw"
	awk -F, '/task-clock/ { print $1 }' "$tmp/perf" >>"$tmp/$v.cpu"
	sed -n 's/^xmbench_preload: mallocs \([0-9]*\).*/\1/p' \
		"$tmp/mwm.log" >>"$tmp/$v.mallocs"
	sed -n 's/^xmbench_preload: .* rtrips \([0-9]*\).*/\1/p' \
		"$tmp/mwm.log" >>"$tmp/$v.rtrips"

	kill "$xvfb"
	wait "$xvfb" 2>/dev/null || true
	xvfb=
}

i=0
while [ $i -lt "$runs" ]; do
	for prefix in "$@"; do
		run "$prefix"
	done
	i=$((i + 1))
done

printf '%-8s %8s %8s %8s %8s %8s %7s\n' version map-ms title-ms wdraw-ms \
	cpu-ms mallocs rtrips
for prefix in "$@"; do
	v=$(basename "$prefix")
	printf '%-8s %8.1f %8.1f %8.1f %8.1f %8.0f %7.0f\n' "$v" \
		"$(median <"$tmp/$v.map")" "$(median <"$tmp/$v.title")" \
		"$(median <"$tmp/$v.wdraw")" "$(median <"$tmp/$v.cpu")" \
		"$(median <"$tmp/$v.mallocs")" "$(median <"$tmp/$v.rtrips")"
done
