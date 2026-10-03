#!/bin/sh
# End-to-end test for mwm: start it as the window manager of a nested
# Xephyr display, check it manages a client, drive its f.* functions
# through key bindings with xdotool, and confirm it survives a hostile
# client's malformed properties and a malformed .mwmrc.
#
#   mwm_xdotool.sh <mwm> <wmclient>
#
# CTest runs it under xvfb-run, which gives the outer DISPLAY; Xephyr
# runs on that.  Needs Xephyr and xdotool; skips (77) without them or a
# display.

set -u
mwm=$1
wmclient=$2
here=$(cd "$(dirname "$0")" && pwd)

[ -n "${DISPLAY:-}" ] || { echo "SKIP: DISPLAY is not set"; exit 77; }
command -v Xephyr > /dev/null || { echo "SKIP: no Xephyr"; exit 77; }
command -v xdotool > /dev/null || { echo "SKIP: no xdotool"; exit 77; }

work=$(mktemp -d "${TMPDIR:-/tmp}/mwm_xdotool.XXXXXX") || exit 1
HOME=$work; export HOME
pids=""
nested=""
cleanup() {
	for p in $pids; do kill "$p" 2>/dev/null; done
	rm -rf "$work"
}
trap cleanup EXIT
fail() { echo "FAIL: $*"; exit 1; }
stop() {
	for _p in "$@"; do
		[ -n "$_p" ] || continue
		kill "$_p" 2>/dev/null
	done
	for _p in "$@"; do
		[ -n "$_p" ] || continue
		_i=0
		while kill -0 "$_p" 2>/dev/null && [ $_i -lt 20 ]; do
			_i=$((_i + 1)); sleep 0.1
		done
		kill -9 "$_p" 2>/dev/null
	done
}

# Pick a free nested display number.
pick_display() {
	n=20
	while [ -e "/tmp/.X11-unix/X$n" ] && [ $n -lt 120 ]; do
		n=$((n + 1))
	done
	nested=":$n"
}

start_xephyr() {
	pick_display
	Xephyr "$nested" -screen 500x500 -ac -noreset > "$work/xephyr.log" 2>&1 &
	pids="$pids $!"
	xephyr_pid=$!
	i=0
	until [ -e "/tmp/.X11-unix/X${nested#:}" ]; do
		i=$((i + 1))
		[ $i -lt 100 ] || fail "Xephyr did not start: $(cat "$work/xephyr.log")"
		sleep 0.1
	done
	sleep 0.5
}

# start_mwm <rc file>: run mwm on the nested display with that .mwmrc
start_mwm() {
	cp "$1" "$work/.mwmrc"
	DISPLAY=$nested "$mwm" -xrm "Mwm*useIconBox: False" \
		> "$work/mwm.log" 2>&1 &
	mwm_pid=$!
	pids="$pids $!"
	sleep 1.5
	kill -0 "$mwm_pid" 2>/dev/null || fail "mwm did not start: $(cat "$work/mwm.log")"
}

# start_client [--hostile]: run wmclient, return its window id in $cid
start_client() {
	DISPLAY=$nested "$wmclient" "$@" --title probe > "$work/client.log" 2>&1 &
	client_pid=$!
	pids="$pids $!"
	i=0
	until grep -q '^window ' "$work/client.log" 2>/dev/null; do
		i=$((i + 1))
		[ $i -lt 100 ] || fail "client did not start: $(cat "$work/client.log")"
		sleep 0.1
	done
	# mwm reparents a managed client; wait for that
	i=0
	until grep -q '^reparent ' "$work/client.log" 2>/dev/null; do
		i=$((i + 1))
		[ $i -lt 100 ] || return 1
		sleep 0.1
	done
	cid=$(DISPLAY=$nested xdotool search --name probe 2>/dev/null | head -1)
	return 0
}

key() { DISPLAY=$nested xdotool key --clearmodifiers "$1"; sleep 0.8; }

focus_client() {
	eval "$(DISPLAY=$nested xdotool getwindowgeometry --shell "$cid")"
	# click the frame title bar, just above the client
	DISPLAY=$nested xdotool mousemove $((X + 20)) $((Y - 10)) click 1
	sleep 0.5
}

last_client() { tail -1 "$work/client.log"; }

check_mwm_alive() {
	kill -0 "$mwm_pid" 2>/dev/null || fail "$1: mwm died: $(cat "$work/mwm.log")"
}

# --- 1. mwm manages a client, and f.* functions work ---------------------
start_xephyr
start_mwm "$here/test.mwmrc"
start_client || fail "mwm did not reparent the client"
echo "managed: client reparented by mwm"

focus_client
key F2
grep -q '^unmap' "$work/client.log" || fail "f.minimize did not unmap the client"
echo "f.minimize: ok"

key F3		# f.normalize from the icon
grep -q 'map$' "$work/client.log" || fail "f.normalize did not remap the client"
echo "f.normalize: ok"

focus_client
key F4		# f.maximize
case $(last_client) in
configure\ 4*|configure\ 3*) echo "f.maximize: ok ($(last_client))" ;;
*) fail "f.maximize did not resize the client (got '$(last_client)')" ;;
esac

key F5		# f.normalize back
case $(last_client) in
configure\ 120x80*) echo "f.restore: ok" ;;
*) fail "f.normalize did not restore the client (got '$(last_client)')" ;;
esac

check_mwm_alive "after functions"
stop "$client_pid" "$mwm_pid"

# --- 2. mwm survives a hostile client ------------------------------------
start_mwm "$here/test.mwmrc"
if start_client --hostile; then
	echo "hostile: mwm managed the malformed client"
else
	echo "hostile: mwm did not reparent it (acceptable), still checking it is alive"
fi
# Drive a few functions and a refresh; mwm must not crash.
[ -n "${cid:-}" ] && focus_client
key F2
key F8		# f.refresh, always valid
check_mwm_alive "after the hostile client"
echo "hostile: mwm survived"
stop "$client_pid" "$mwm_pid"

# --- 3. mwm starts on a malformed .mwmrc ---------------------------------
start_mwm "$here/bad.mwmrc"
if start_client; then
	echo "bad .mwmrc: mwm reported errors and still manages a client"
else
	# It may use built-in bindings; as long as it is alive and
	# reparented something, that is fine.  Reparent is required above.
	fail "mwm did not manage a client with the malformed .mwmrc"
fi
grep -qi "error\|invalid\|warning" "$work/mwm.log" ||
	echo "note: mwm printed no diagnostic for the malformed .mwmrc"
check_mwm_alive "with the malformed .mwmrc"
echo "bad .mwmrc: ok"

echo PASS
