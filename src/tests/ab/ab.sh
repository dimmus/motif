#!/bin/sh
# ab.sh OLD_LIBDIR NEW_LIBDIR MODE FIRST LAST [SIZE]
#
# Run xm_abtest for seeds FIRST..LAST against two builds of libXm (the
# directories holding libXm.so.*) on the current DISPLAY, and report the
# seeds whose output differs.  Differing outputs are kept in
# $AB_OUT (default ./ab-out) as MODE-SEED.old / MODE-SEED.new.
#
# XM_ABTEST overrides the harness binary (default: xm_abtest next to
# this script's build directory, or on PATH).  AB_TIMEOUT is the
# per-run timeout in seconds (default 120).  Needs xdotool.

set -u
[ $# -ge 5 ] || { echo "usage: $0 OLD_LIBDIR NEW_LIBDIR MODE FIRST LAST [SIZE]" >&2; exit 2; }
old=$1; new=$2; mode=$3; first=$4; last=$5; size=${6:-0}
abtest=${XM_ABTEST:-$(command -v xm_abtest || echo ./xm_abtest)}
out=${AB_OUT:-./ab-out}
[ -n "${DISPLAY:-}" ] || { echo "$0: DISPLAY is not set (try xvfb-run -a $0 ...)" >&2; exit 2; }
command -v xdotool > /dev/null || { echo "$0: xdotool is required" >&2; exit 2; }
[ -x "$abtest" ] || { echo "$0: no harness at $abtest (set XM_ABTEST)" >&2; exit 2; }
mkdir -p "$out"

same=0; fail=0; oldbad=0
s=$first
while [ "$s" -le "$last" ]; do
	LD_LIBRARY_PATH=$old timeout "${AB_TIMEOUT:-120}" "$abtest" "$mode" "$s" "$size" \
		> "$out/$mode-$s.old" 2>&1
	ro=$?
	LD_LIBRARY_PATH=$new timeout "${AB_TIMEOUT:-120}" "$abtest" "$mode" "$s" "$size" \
		> "$out/$mode-$s.new" 2>&1
	rn=$?
	if [ $ro -ne 0 ]; then
		oldbad=$((oldbad + 1))
		echo "seed $s: old exit $ro, new exit $rn"
	elif [ $rn -ne 0 ] || ! cmp -s "$out/$mode-$s.old" "$out/$mode-$s.new"; then
		fail=$((fail + 1))
		echo "seed $s: DIFF (new exit $rn)"
	else
		same=$((same + 1))
		rm -f "$out/$mode-$s.old" "$out/$mode-$s.new"
	fi
	s=$((s + 1))
done
echo "$mode: $same identical, $fail differ, $oldbad old-failed"
[ $fail -eq 0 ] && [ $oldbad -eq 0 ]
