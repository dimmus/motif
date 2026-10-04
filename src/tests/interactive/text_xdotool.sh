#!/bin/sh
# Drive XmTextField and XmText with real input through xdotool (XTest):
# typing, a word selection with the mouse, a drag selection, middle
# button paste of PRIMARY into another application, and the clipboard
# between two applications with the keyboard.
#
#   text_xdotool.sh <textapp>
#
# Needs an X server without a window manager (CTest runs it under
# xvfb-run); exits 77 when there is no display or no xdotool.

set -u
app=$1

[ -n "${DISPLAY:-}" ] || { echo "SKIP: DISPLAY is not set"; exit 77; }
command -v xdotool > /dev/null || { echo "SKIP: no xdotool"; exit 77; }

work=$(mktemp -d "${TMPDIR:-/tmp}/text_xdotool.XXXXXX") || exit 1
pids=""
cleanup() {
	for p in $pids; do kill "$p" 2>/dev/null; done
	sleep 0.5
	for p in $pids; do kill -9 "$p" 2>/dev/null; done
	rm -rf "$work"
}
trap cleanup EXIT

fail() {
	echo "FAIL: $*"
	exit 1
}

# start <name> <x> <y>: start textapp, wait until it is mapped
start() {
	"$app" -title "$1" -geometry "+$2+$3" > "$work/$1.out" 2> "$work/$1.err" &
	eval "pid_$1=$!"
	pids="$pids $!"
	i=0
	until grep -q '^ready' "$work/$1.out" 2>/dev/null; do
		i=$((i + 1))
		[ $i -lt 100 ] || fail "$1 did not start: $(cat "$work/$1.err")"
		sleep 0.1
	done
}

# finish <name>: stop textapp and get the contents of its widgets
finish() {
	eval "p=\$pid_$1"
	kill -TERM "$p"
	wait "$p" 2>/dev/null
	tf=$(sed -n 's/^tf=//p' "$work/$1.out")
	text=$(sed -n 's/^text=//p' "$work/$1.out")
	# Calls through function pointers of another type (callback procs
	# cast all over Motif) are reported by UBSan but not counted here.
	if grep -q "ERROR: .*Sanitizer" "$work/$1.err" ||
	   grep "runtime error" "$work/$1.err" |
	   grep -qv "through pointer to incorrect function type"; then
		cat "$work/$1.err"
		fail "$1: sanitizer report"
	fi
}

settle() {
	xdotool sleep 0.3
}

# Without a window manager the windows sit where -geometry put them:
# A at (0,0), B at (0,200).  The TextField is the top 30 pixels of each
# window, the Text below it.
start A 0 0
start B 0 200

# Typing into A's TextField and Text
xdotool mousemove 40 12 click 1
settle
xdotool type --delay 20 "hello world"
xdotool mousemove 40 60 click 1
settle
xdotool type --delay 20 "first line"
xdotool key Return
xdotool type --delay 20 "second"
settle

# Double click on "hello" selects the word (PRIMARY); paste it with
# the middle button into B's TextField.
xdotool mousemove 12 12 click --repeat 2 --delay 80 1
settle
xdotool mousemove 40 212 click 2
settle

# A drag selection over the start of A's Text, pasted into B's Text
xdotool mousemove 6 45 mousedown 1 mousemove 60 45 mousemove 200 45 mouseup 1
settle
xdotool mousemove 40 260 click 2
settle

# The clipboard: select all of A's TextField from the keyboard, copy
# it, paste it at the end of B's TextField.  Motif binds copy and paste
# to Ctrl+Insert and Shift+Insert (osfCopy, osfPaste); Ctrl+C and Ctrl+V
# are not bound by default.
xdotool mousemove 40 12 click 1
settle
xdotool key Home shift+End ctrl+Insert
settle
xdotool mousemove 40 212 click 1
settle
xdotool key End shift+Insert
settle

finish A
a_tf=$tf
a_text=$text
finish B
b_tf=$tf
b_text=$text

echo "A: tf='$a_tf' text='$a_text'"
echo "B: tf='$b_tf' text='$b_text'"

[ "$a_tf" = "hello world" ] || fail "typing into A's TextField"
[ "$a_text" = 'first line\nsecond' ] || fail "typing into A's Text"
case $b_tf in
hello*) ;;
*) fail "middle button paste of the word selection into B's TextField" ;;
esac
[ "$b_tf" = "hellohello world" ] || fail "clipboard copy from A, paste into B"
case $b_text in
first*) ;;
*) fail "drag selection in A pasted into B's Text" ;;
esac
echo PASS
