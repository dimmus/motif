# Fuzzer reproducers for open library bugs

Each file here is an input that makes its fuzzer fail because of a
library bug that is **not fixed yet**.  `Fuzz.<target>.crashes` replays
the whole directory and passes *while the inputs still fail*, so that a
fix is noticed (then move or delete the reproducer).  The regular
`Fuzz.<target>` test replays only the clean corpus in `../corpus`, so it
stays green.

These are Motif bugs, found by the fuzzers added with the tests; none is
fixed in this change (fixing them is another workstream).

## uid/corrupt-xmstring-segv
A corrupt `.uid` makes `MrmFetchWidget` create an `XmPushButton` whose
`XmNlabelString` is not a valid compound string; `Label.c` Initialize
copies it with `XmStringCopy`, and `Clone` (`XmString.c:5630`) reads
through a wild entry pointer.  Mrm does not validate the compound
string literals it hands to widgets.  Needs an X server.

## xpm/huge-chars-per-pixel-oom
An XPM whose header asks for a huge width, height or colour count makes
`_XmxpmParseColors` / the pixel loops allocate gigabytes
(`Xpmparse.c`), an unbounded allocation from attacker-controlled
counts.  libFuzzer reports it as an out-of-memory.
