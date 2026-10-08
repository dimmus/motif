#!/bin/sh
# A small stand-in for xvfb-run, for the systems that have Xvfb but not
# xvfb-run (FreeBSD): run a command with DISPLAY set to a new Xvfb of
# its own, and stop the server after it.  CMake uses it for the X11
# tests when there is no xvfb-run, so that each test gets a fresh
# server as it does under xvfb-run, instead of all of them sharing one
# and seeing the selections, root properties and keyboard focus the
# tests before them left.
#
#   xvfb-run.sh [-a] [-s "server arguments"] command [arguments...]
#
# -a is accepted for compatibility: Xvfb always picks a free display
# here (-displayfd).  Exits with the status of the command, or 1 when
# the server does not start.

set -u
server_args="-screen 0 1280x1024x24"
while [ $# -gt 0 ]; do
	case $1 in
	-a | --auto-servernum) shift ;;
	-s) server_args=$2; shift 2 ;;
	--server-args=*) server_args=${1#*=}; shift ;;
	--) shift; break ;;
	-*) echo "xvfb-run.sh: unknown option $1" >&2; exit 2 ;;
	*) break ;;
	esac
done
[ $# -gt 0 ] || { echo "usage: xvfb-run.sh [-a] [-s args] command..." >&2; exit 2; }

# The default font path of FreeBSD's Xvfb does not include the bitmap
# fonts the tests use ("fixed", "8x13bold").
fp=
for d in /usr/local/share/fonts/misc /usr/share/fonts/X11/misc; do
	if [ -f "$d/fonts.dir" ]; then fp="$fp$d/,"; fi
done

dir=$(mktemp -d "${TMPDIR:-/tmp}/xvfb-run.XXXXXX") || exit 1
xvfb_pid=
cleanup() {
	if [ -n "$xvfb_pid" ]; then
		kill "$xvfb_pid" 2>/dev/null
		wait "$xvfb_pid" 2>/dev/null
	fi
	rm -rf "$dir"
}
trap cleanup EXIT
trap 'exit 143' TERM
trap 'exit 130' INT

# Xvfb writes the display number to the -displayfd descriptor once it
# accepts connections.
# shellcheck disable=SC2086 # server_args is a list
Xvfb -displayfd 3 -nolisten tcp $server_args ${fp:+-fp ${fp}built-ins} \
	3> "$dir/display" > "$dir/log" 2>&1 &
xvfb_pid=$!
i=0
until [ -s "$dir/display" ]; do
	if ! kill -0 "$xvfb_pid" 2>/dev/null || [ $i -ge 300 ]; then
		echo "xvfb-run.sh: Xvfb did not start:" >&2
		cat "$dir/log" >&2
		exit 1
	fi
	i=$((i + 1))
	sleep 0.1
done
DISPLAY=:$(head -n 1 "$dir/display")
export DISPLAY

"$@"
rc=$?
exit $rc
