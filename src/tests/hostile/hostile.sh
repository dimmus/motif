#!/bin/sh
# End-to-end hostile-client test.  A Motif application (victim), built
# with the sanitizers the top-level build enabled, is driven against a
# deliberately malformed X client (hostile_peer):
#
#   1. hostile root properties, then the victim starts and runs a drag;
#   2. the victim drags (real xdotool pointer motion) across a row of
#      drop sites whose _MOTIF_DRAG_RECEIVER_INFO is malformed;
#   3. a hostile drag source injects malformed drag messages and answers
#      the victim's conversions with garbage;
#   4. a hostile clipboard owner: the victim inquires, retrieves and
#      pastes, meeting malformed replies and _MOTIF_CLIP_* records.
#
# The victim must not crash or report a sanitizer error through any of
# it.  CTest runs this under xvfb-run, which supplies DISPLAY.  Needs
# xdotool; skips (77) without a display or xdotool.
#
#   hostile.sh <victim> <hostile_peer>

set -u
victim=$1
peer=$2

[ -n "${DISPLAY:-}" ] || { echo "SKIP: DISPLAY is not set"; exit 77; }
command -v xdotool > /dev/null || { echo "SKIP: no xdotool"; exit 77; }

work=$(mktemp -d "${TMPDIR:-/tmp}/hostile.XXXXXX") || exit 1
pids=""

cleanup() {
	for p in $pids; do kill "$p" 2>/dev/null; done
	sleep 0.3
	for p in $pids; do kill -9 "$p" 2>/dev/null; done
	rm -rf "$work"
}
trap cleanup EXIT

fail() {
	echo "FAIL: $*"
	for f in "$work"/*.err "$work"/*.out; do
		[ -f "$f" ] && { echo "--- $f"; cat "$f"; }
	done
	exit 1
}

# A clean sanitizer exit aborts; UBSan prints "runtime error".  Calls
# through a function pointer of another type are reported by UBSan but
# are endemic in Motif (callback procs are cast all over), so they do
# not count, exactly as the text_xdotool test does.
check_sane() {
	_who=$1
	_err="$work/$_who.err"
	[ -f "$_err" ] || return 0
	if grep -q "ERROR: .*Sanitizer\|SUMMARY: .*Sanitizer" "$_err" ||
	   grep "runtime error" "$_err" 2>/dev/null |
	   grep -qv "through pointer to incorrect function type"; then
		echo "--- $_who.err"
		cat "$_err"
		fail "$_who: sanitizer report"
	fi
}

# wait_line <file> <pattern> <what>
wait_line() {
	_i=0
	until grep -q "$2" "$1" 2>/dev/null; do
		_i=$((_i + 1))
		[ $_i -lt 150 ] || return 1
		sleep 0.1
	done
	return 0
}

start_peer() {
	_mode=$1
	shift
	"$peer" "$_mode" "$@" > "$work/peer.out" 2> "$work/peer.err" &
	peer_pid=$!
	pids="$pids $peer_pid"
	wait_line "$work/peer.out" '^ready' "peer $_mode" ||
		fail "hostile_peer $_mode did not become ready: $(cat "$work/peer.err")"
}

stop_peer() {
	[ -n "${peer_pid:-}" ] || return 0
	kill "$peer_pid" 2>/dev/null
	wait "$peer_pid" 2>/dev/null
	peer_pid=""
}

start_victim() {
	# This test is about memory corruption and undefined behaviour, not
	# leaks (those are covered, and suppressed, elsewhere): a real bug
	# makes ASan/UBSan print to stderr, which check_sane looks for.
	ASAN_OPTIONS="${ASAN_OPTIONS:-}:detect_leaks=0" \
	UBSAN_OPTIONS="${UBSAN_OPTIONS:-}:print_stacktrace=1" \
	"$victim" -geometry 400x300+0+300 -title victim \
		> "$work/victim.out" 2> "$work/victim.err" &
	victim_pid=$!
	pids="$pids $victim_pid"
	wait_line "$work/victim.out" '^ready' "victim" ||
		fail "victim did not become ready: $(cat "$work/victim.err")"
}

victim_alive() {
	kill -0 "$victim_pid" 2>/dev/null ||
		fail "$1: victim died: $(tail -20 "$work/victim.err")"
}

stop_victim() {
	[ -n "${victim_pid:-}" ] || return 0
	kill -TERM "$victim_pid" 2>/dev/null
	_i=0
	while kill -0 "$victim_pid" 2>/dev/null && [ $_i -lt 30 ]; do
		_i=$((_i + 1)); sleep 0.1
	done
	kill -9 "$victim_pid" 2>/dev/null
	victim_pid=""
}

settle() { xdotool sleep 0.4; }

# --- 1. hostile root properties before the victim starts -----------------
echo "== phase 1: hostile root and drop sites"
start_peer --dropsite
# the drop sites are a row starting at x=0,y=0; the victim window is below
# them (geometry +0+300) so a drag can sweep up across the whole row
dropwins=$(sed -n 's/^window //p' "$work/peer.out")
[ -n "$dropwins" ] || fail "peer --dropsite printed no window ids"
echo "drop sites: $dropwins"

start_victim
victim_alive "after start with hostile root"

# --- 2. a real drag across the malformed drop sites ----------------------
echo "== phase 2: drag across malformed receiver-info drop sites"
# Press Btn1 inside the victim's drag area (top ~120px of the window at
# y=300), then move the pointer up across the row of drop sites at y~60,
# stopping on each so the toolkit reads each window's receiver info, then
# release.
xdotool mousemove 40 340
settle
xdotool mousedown 1
settle
wait_line "$work/victim.out" '^drag-start' "drag-start" ||
	fail "the Btn1 drag did not start (no drag-start marker)"
# sweep across each drop site
x=20
while [ $x -lt 620 ]; do
	xdotool mousemove $x 60
	xdotool sleep 0.15
	x=$((x + 40))
done
# back down over the victim and release
xdotool mousemove 200 380
settle
xdotool mouseup 1
settle
victim_alive "after dragging over hostile drop sites"
check_sane victim
echo "phase 2: survived the drag"
stop_victim
stop_peer

# --- 3. a hostile drag source toward the victim --------------------------
echo "== phase 3: hostile drag source"
start_victim
victim_alive "restart for source phase"
vwin=$(xdotool search --name victim 2>/dev/null | head -1)
[ -n "$vwin" ] || fail "could not find the victim window"
start_peer --source --target "$vwin"
settle
settle
victim_alive "after hostile source messages"
check_sane victim
echo "phase 3: survived the hostile source"
stop_peer
stop_victim

# --- 4. a hostile clipboard owner ----------------------------------------
echo "== phase 4: hostile clipboard owner"
start_peer --clipboard
start_victim
victim_alive "restart for clipboard phase"
# Put the pointer over the Text widget (the lower part of the window at
# +0+300) so keys reach it under Xvfb's pointer-root focus, then trigger
# the clipboard, paste and drag actions.
xdotool mousemove 100 480
settle
xdotool click 1
settle
xdotool key c    # inquire + retrieve from the hostile owner
settle
xdotool key p    # paste CLIPBOARD into the Text
settle
xdotool key d    # re-read the hostile root drag tables
settle
victim_alive "after hostile clipboard"
check_sane victim
wait_line "$work/victim.out" '^clip-done' "clip-done" ||
	fail "the clipboard inquire/retrieve action did not run (no clip-done)"
wait_line "$work/victim.out" '^paste-done' "paste-done" ||
	fail "the paste action did not run (no paste-done)"
wait_line "$work/victim.out" '^drag-done' "drag-done" ||
	fail "the root-table drag action did not run (no drag-done)"
echo "phase 4: survived the hostile clipboard"
stop_victim
stop_peer

echo PASS
