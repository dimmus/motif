# Fuzzer reproducers for open library bugs

Each file here is an input that makes its fuzzer fail because of a
library bug that is **not fixed yet**.  `Fuzz.<target>.crashes` replays
the whole directory and passes *while the inputs still fail*, so that a
fix is noticed (then move or delete the reproducer).  The regular
`Fuzz.<target>` test replays only the clean corpus in `../corpus`, so it
stays green.

These are Motif bugs, found by the fuzzers added with the tests; none is
fixed in this change (fixing them is another workstream).

## clipboard/corrupt-record-exit
`XmClipboardInquirePendingItems` on a malformed `_MOTIF_CLIP_*` record
set reaches `ClipboardError` (`CutPaste.c`), which prints a message and
calls `exit(1)`.  Any client that writes the root-window clipboard
properties can make every Motif application that touches the clipboard
exit.  The reproducer is replayed with `FUZZ_CLIPBOARD_EXIT=1`, which
lets the exit happen instead of catching it.

## motifbind/parsekeysym-stack-overflow
A `.motifbind` line (or `_MOTIF_BINDINGS` property) with a key name
longer than 100 bytes overflows `keySymName[100]` in `ParseKeySym`
(`MapEvents.c:395`), a stack buffer, through `strncpy` of the whole
name.  Reached from `_XmVirtKeysInitialize` as every Motif application
starts, so a hostile root-window property is enough.

## xpm/huge-chars-per-pixel-oom
An XPM whose header asks for a huge width, height or colour count makes
`_XmxpmParseColors` / the pixel loops allocate gigabytes
(`Xpmparse.c`), an unbounded allocation from attacker-controlled
counts.  libFuzzer reports it as an out-of-memory.
