#!/bin/sh
# latency.sh DELAY_MS COMMAND [ARGS...]
#
# Run COMMAND against a private Xvfb through xproxy.py, which holds the
# X traffic DELAY_MS in each direction (a round trip of about twice
# DELAY_MS) and counts it.  COMMAND gets DISPLAY (through the proxy) and
# DRIVER_DISPLAY (the Xvfb itself, for input that should not wait on
# the slow link).  The proxy appends one JSON line per X connection to
# $LATENCY_STATS (default ./latency-stats.jsonl).  LD_LIBRARY_PATH is
# passed on, to pick the libXm to measure.  XNUM sets the Xvfb display
# number (default 71; the proxy listens on display XNUM+1).

set -u
[ $# -ge 2 ] || { echo "usage: $0 DELAY_MS COMMAND [ARGS...]" >&2; exit 2; }
delay=$1; shift
n=${XNUM:-71}
here=$(cd "$(dirname "$0")" && pwd)
stats=${LATENCY_STATS:-./latency-stats.jsonl}

Xvfb :$n -screen 0 1280x1024x24 -nolisten tcp > /dev/null 2>&1 &
xvfb=$!
i=0
while [ ! -S /tmp/.X11-unix/X$n ] && [ $i -lt 100 ]; do
	sleep 0.05
	i=$((i + 1))
done
python3 "$here/xproxy.py" $((6000 + n + 1)) /tmp/.X11-unix/X$n "$delay" "$stats" &
proxy=$!
sleep 0.5
DRIVER_DISPLAY=:$n DISPLAY=127.0.0.1:$((n + 1)) "$@"
status=$?
sleep 0.3
kill $proxy $xvfb
wait $proxy $xvfb 2> /dev/null
exit $status
