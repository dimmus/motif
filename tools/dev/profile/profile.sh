#!/bin/sh
# Profile the scenarios of doc/profiling.md.
#
#   profile.sh PREFIX OUT [SCENARIO ...]
#
# PREFIX is a Motif installed by build.sh (this tree's, since the
# xmbench scenarios need xmbench); OUT receives the raw profiles and a
# summary per scenario.  Scenarios (default: all of them):
#
#   startup   hello_motif, one start and exit
#   xmbench   every xmbench case, one perf profile each
#   text      xmbench text-append: 10 MB appended to an XmText, 1 KB at a time
#   list      xmbench list-add: 100k items appended to an XmList
#   mwm       mwm managing 200 clients (xclients -n 200 -t 2000)
#
# For each scenario the tools are, where they apply:
#   perf record -g        CPU samples (task-clock) with call graphs
#   valgrind callgrind    exact instruction counts per function
#   heaptrack             allocations, peak heap
#   xtrace                the X protocol: requests, replies, events
#   libxmbench_preload    round trips, with backtraces (rtrips.py), and mallocs
#
# The scenarios run on an Xvfb of their own.  PERF_CALLGRAPH selects the
# perf unwinder (default "dwarf"; "fp" works too, the libraries are
# built with frame pointers).
set -eu

prefix=$1
out=$2
shift 2
scenarios=${*:-startup xmbench text list mwm}

bin=$prefix/bin
here=$(cd "$(dirname "$0")" && pwd)
preload=${PRELOAD:-$bin/libxmbench_preload.so}
callgraph=${PERF_CALLGRAPH:-dwarf}
mkdir -p "$out"
export LD_LIBRARY_PATH="$prefix/lib"
export XMODIFIERS=@im=none
# xmbench re-executes itself with its counting library; the profilers
# would then follow the child, so it is preloaded once from here.
export XMBENCH_NO_PRELOAD=1

xvfb_pid=
xtrace_pid=
start_xvfb() {
	fifo=$out/.displayfd
	rm -f "$fifo"
	mkfifo "$fifo"
	Xvfb -displayfd 3 -nolisten tcp -noreset -screen 0 1280x1024x24 \
		+extension RENDER 3>"$fifo" >/dev/null 2>&1 &
	xvfb_pid=$!
	read -r num <"$fifo"
	rm -f "$fifo"
	export DISPLAY=:$num
}

stop_pid() {
	[ -n "$1" ] || return 0
	kill "$1" 2>/dev/null || true
	wait "$1" 2>/dev/null || true
}

cleanup() {
	stop_pid "$xtrace_pid"
	stop_pid "$xvfb_pid"
}
trap cleanup EXIT INT TERM

# perf report: the top functions by self time.
perf_top() {
	perf report -i "$1" --no-children --stdio --percent-limit 1.5 \
		--sort symbol,dso -g none 2>/dev/null | grep -v '^#' |
		grep -v '^$' | head -n "${2:-15}"
}

# callgrind_annotate: the total and the top functions by self and by
# inclusive instruction count.
cg_top() {
	callgrind_annotate "$1" 2>/dev/null | sed -n '1,/^--.*file:function/p' |
		grep -E 'PROGRAM TOTALS|Ir' | head -n 3
	callgrind_annotate "$1" 2>/dev/null |
		sed -n '/file:function/,$p' | sed -n "2,$((${2:-15} + 1))p"
	echo "-- inclusive"
	callgrind_annotate --inclusive=yes "$1" 2>/dev/null |
		sed -n '/file:function/,$p' | sed -n "2,$((${2:-15} + 1))p"
}

# heaptrack: the totals, and the top allocation sites (stacks.py).
heap_summary() {
	f=$(ls "$1".* | grep -v '\.top$' | head -n 1)
	heaptrack_print -f "$f" -p 0 -a 0 -T 0 -l 0 2>/dev/null |
		sed -n '/^total runtime/,$p'
	heaptrack_print -f "$f" -p 0 -a 0 -T 0 -l 0 \
		--flamegraph-cost-type allocations -F "$1.stacks" >/dev/null 2>&1
	python3 "$here/stacks.py" -n 15 "$1.stacks"
	rm -f "$1.stacks"
}

xtrace_summary() {
	awk -f "$here/xtrace.awk" "$1"
}

# xtrace_run LOG COMMAND...: run COMMAND through xtrace, summarise the
# protocol (xtrace.awk: requests, replies, round trips, events).
xtrace_run() {
	log=$1
	shift
	real=$DISPLAY
	num=$(( ${real#:} + 100 ))
	rm -f "$log"
	xtrace -n -k -o "$log" -d "$real" -D ":$num" >/dev/null 2>&1 &
	xtrace_pid=$!
	sleep 1
	DISPLAY=:$num "$@" || true
	sleep 1
	stop_pid "$xtrace_pid"
	xtrace_pid=
	xtrace_summary "$log" >"$log.summary"
}

counted() {
	XMBENCH_REPORT=1 LD_PRELOAD=$preload "$@" 2>&1 >/dev/null |
		grep '^xmbench_preload:' | tail -n 1
}

# --- startup ------------------------------------------------------------

scenario_startup() {
	d=$out/startup
	mkdir -p "$d"
	"$bin/hello_motif"
	perf record -q -F 20000 --call-graph "$callgraph" -o "$d/perf.data" -- \
		"$bin/hello_motif" >/dev/null 2>&1
	perf_top "$d/perf.data" 25 >"$d/perf.top"
	valgrind --tool=callgrind --callgrind-out-file="$d/callgrind.out" \
		"$bin/hello_motif" >/dev/null 2>&1
	cg_top "$d/callgrind.out" 25 >"$d/callgrind.top"
	heaptrack -o "$d/heaptrack" "$bin/hello_motif" >/dev/null 2>&1
	heap_summary "$d/heaptrack" >"$d/heaptrack.top"
	xtrace_run "$d/xtrace.log" "$bin/hello_motif" >/dev/null
	counted "$bin/hello_motif" >"$d/preload.txt"
	XMBENCH_REPLY_BACKTRACE=1 LD_PRELOAD=$preload "$bin/hello_motif" \
		2>"$d/rtrips.bt"
	python3 "$here/rtrips.py" -d 3 "$d/rtrips.bt" >"$d/rtrips.txt"
	rm -f "$d/rtrips.bt"
}

# --- xmbench ------------------------------------------------------------

scenario_xmbench() {
	d=$out/xmbench
	mkdir -p "$d"
	LD_PRELOAD=$preload XMBENCH_NO_PRELOAD= "$bin/xmbench" -r 5 \
		-j "$d/xmbench.json" >"$d/xmbench.txt" 2>&1
	for c in $("$bin/xmbench" -l | awk '{print $1}'); do
		perf record -q -F 5000 --call-graph "$callgraph" \
			-o "$d/$c.perf.data" -- "$bin/xmbench" -r 1 "$c" \
			>/dev/null 2>&1 || continue
		{
			echo "== $c"
			perf_top "$d/$c.perf.data" 8
		} >"$d/$c.perf.top"
		# The DWARF call graphs are large; keep only the summary.
		rm -f "$d/$c.perf.data"
	done
	cat "$d"/*.perf.top >"$d/perf.top"
}

# --- text and list (xmbench macro cases, profiled in depth) -------------

scenario_case() {
	name=$1
	c=$2
	d=$out/$name
	mkdir -p "$d"
	perf record -q -F 5000 --call-graph "$callgraph" -o "$d/perf.data" -- \
		"$bin/xmbench" -r 1 "$c" >/dev/null 2>&1
	perf_top "$d/perf.data" 20 >"$d/perf.top"
	valgrind --tool=callgrind --callgrind-out-file="$d/callgrind.out" \
		"$bin/xmbench" -r 1 "$c" >/dev/null 2>&1
	cg_top "$d/callgrind.out" 20 >"$d/callgrind.top"
	heaptrack -o "$d/heaptrack" "$bin/xmbench" -r 1 "$c" >/dev/null 2>&1
	heap_summary "$d/heaptrack" >"$d/heaptrack.top"
	xtrace_run "$d/xtrace.log" "$bin/xmbench" -r 1 "$c" >/dev/null
	rm -f "$d/xtrace.log"
	LD_PRELOAD=$preload XMBENCH_NO_PRELOAD= "$bin/xmbench" -r 5 "$c" \
		>"$d/xmbench.txt" 2>&1
}

# --- mwm ----------------------------------------------------------------

wait_mwm() {
	i=0
	until xprop -root _MOTIF_WM_INFO 2>/dev/null | grep -q ' = '; do
		i=$((i + 1))
		[ $i -lt 600 ] || { echo "mwm did not start" >&2; return 1; }
		sleep 0.1
	done
}

# mwm_run LOG WRAPPER...: start mwm under WRAPPER, run the clients, stop
# mwm with SIGTERM (which, with the kill feedback off, makes it exit
# normally, so that callgrind and heaptrack write their data).
mwm_run() {
	log=$1
	shift
	"$@" "$bin/mwm" -xrm 'Mwm*showFeedback: -kill' >"$log" 2>&1 &
	mwm_pid=$!
	wait_mwm
	"$bin/xclients" -n 200 -t 2000 >>"$log.clients" 2>&1
	sleep 1
	# Under perf or heaptrack, $! is the wrapper: signal mwm itself.
	# valgrind runs mwm in its own process, which has another name.
	pids=$(pgrep -x mwm || echo "$mwm_pid")
	kill -TERM $pids 2>/dev/null || true
	wait "$mwm_pid" 2>/dev/null || true
}

scenario_mwm() {
	d=$out/mwm
	mkdir -p "$d"
	rm -f "$d"/*.clients
	mwm_run "$d/plain.log" env
	mwm_run "$d/perf.log" perf record -q -F 5000 --call-graph "$callgraph" \
		-o "$d/perf.data" --
	perf_top "$d/perf.data" 25 >"$d/perf.top"
	mwm_run "$d/callgrind.log" valgrind --tool=callgrind \
		--callgrind-out-file="$d/callgrind.out"
	cg_top "$d/callgrind.out" 25 >"$d/callgrind.top"
	mwm_run "$d/heaptrack.log" heaptrack -o "$d/heaptrack"
	heap_summary "$d/heaptrack" >"$d/heaptrack.top"
	mwm_run "$d/preload.log" env XMBENCH_REPORT=1 LD_PRELOAD="$preload"
	grep '^xmbench_preload:' "$d/preload.log" >"$d/preload.txt" || true
	# xtrace between mwm and the server.
	real=$DISPLAY
	num=$(( ${real#:} + 100 ))
	rm -f "$d/xtrace.log"
	xtrace -n -k -o "$d/xtrace.log" -d "$real" -D ":$num" >/dev/null 2>&1 &
	xtrace_pid=$!
	sleep 1
	DISPLAY=:$num "$bin/mwm" -xrm 'Mwm*showFeedback: -kill' \
		>"$d/xtrace.mwm.log" 2>&1 &
	mwm_pid=$!
	wait_mwm
	"$bin/xclients" -n 200 -t 2000 >>"$d/xtrace.clients" 2>&1
	sleep 1
	kill -TERM "$mwm_pid"
	wait "$mwm_pid" 2>/dev/null || true
	sleep 1
	stop_pid "$xtrace_pid"
	xtrace_pid=
	xtrace_summary "$d/xtrace.log" >"$d/xtrace.log.summary"
	rm -f "$d/xtrace.log"
}

start_xvfb
for s in $scenarios; do
	echo "== $s"
	case $s in
	startup) scenario_startup ;;
	xmbench) scenario_xmbench ;;
	text) scenario_case text text-append ;;
	list) scenario_case list list-add ;;
	mwm) scenario_mwm ;;
	*) echo "unknown scenario $s" >&2; exit 2 ;;
	esac
done
