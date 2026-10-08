# Motif unit tests

The unit tests use [Check](https://libcheck.github.io/check/) (`pkg-config check`)
and are built when Motif is configured with `-DWITH_TESTS=ON`:

    cmake -S . -B build -DWITH_TESTS=ON
    cmake --build build
    ctest --test-dir build --no-tests=error --output-on-failure

`runner.c` builds a single `motif_tests` program from the suites in `Xm/`.
Each suite is registered with CTest as `Xm.<suite>`; `motif_tests` with no
arguments runs all of them, and `motif_tests <suite> ...` runs only the
named ones.  The suites open the fixtures in `png/`, `jpeg/` and `svg/` by
relative path, so CTest runs them from a copy of those directories in the
build tree (`<build>/src/tests/fixtures`).

Suites labelled `X11` (`FontList`, `FontListEntry`, `XmStringCT`,
`XmStringExtent`, `Widgets`, `Text`, `Clipboard`, `Layout`, `XErrors`,
`List`, `I18nLocale`, `MsgCat`, `Rtl`, `Xim`, `RoundTrips`, `ConstApi`,
`RenderTable`, `Gadgets`) need an X server, as do the `Uil.load*`
tests, those in `interactive/` and `visual/` and some in `fuzz/`.  When
`xvfb-run` is found at configure time, CTest starts each of them under
its own Xvfb (with `-noreset`, see `XVFB_RUN_ARGS`); configure with
`-DXVFB_RUN_EXECUTABLE=OFF` to use `$DISPLAY` instead.  Without `DISPLAY`
these tests exit with status 77 and CTest reports them as skipped.

Test cases tagged `xfail` document known library bugs.  They are left out
of normal runs and checked by `Xm.<suite>.xfail` (`motif_tests --xfail
<suite>`), which passes only while all of them still fail.  When a fix
makes one pass, remove its tag.

The i18n suites:

- `I18nLocale` runs XmString, XmTextField, XmText, XmList and XmLabel
  with text of the locale's language in its codeset.  Besides
  `Xm.I18nLocale` (the default locale), CTest runs it as
  `Xm.I18nLocale.<locale>` in `ja_JP.UTF-8`, `ja_JP.EUC-JP`,
  `de_DE.UTF-8`, `de_DE.ISO-8859-1` and `he_IL.UTF-8`.  These locales are
  compiled at build time with glibc's `localedef` into
  `<build>/src/tests/locale` (no root needed) and the tests run with
  `LOCPATH` pointing there and `MOTIF_TEST_LOCALE` naming the locale
  (see `i18n.cmake`).  Without `localedef` and the glibc locale sources
  (`/usr/share/i18n`, the `locales` package on Debian and Ubuntu,
  `glibc-locale-source` on Fedora) only the default-locale tests are
  registered.
- `Rtl` builds widgets under a left-to-right and a right-to-left shell
  and checks that the second layout is the mirror of the first (Label,
  PushButton, Form, RowColumn, ScrolledWindow, scrolled Text and List).
- `Xim` types through `stubxim` (`xim/stubxim.c`), a small input method
  server that speaks the XIM protocol to Xlib, so that XmIm's
  on-the-spot preedit callbacks, over-the-spot spot location,
  off-the-spot areas, commits and XIC resets run end to end.  It runs in
  a UTF-8 locale and, as `Xm.Xim.<locale>`, in the generated `ja_JP`
  ones.
- `MsgCat` checks that a Motif warning comes from the message catalog
  `NLSPATH` finds: `Xm.MsgCat.C` with a test catalog, and
  `Xm.MsgCat.de_DE.UTF-8` with the German one of `localized/`.  They need
  `-DWITH_MESSAGE_CATALOG=ON` and `gencat`; `Xm.MsgCat` checks the
  built-in message.

The other directories:

- `uil/` compiles every `.uil` file in the tree with `uil` and loads the
  `.uid` with Mrm (`Uil.compile.*`, `Uil.load.*`, `Uil.loadbuffer.*`).
- `interactive/` drives a Text/TextField program and mwm (in a nested
  Xephyr) with real input through `xdotool` (`Text.xdotool`,
  `Mwm.xdotool`); they are skipped without `xdotool` or `Xephyr`.
  `Mwm.manage` (`mwm_tests`, libcheck) runs mwm as the window manager
  of its display and checks the focus timestamps, the properties it
  reads when it manages a window and later, `_NET_CLIENT_LIST`, and the
  title shown after many title changes in a row.
- `visual/` renders a fixed scene with the BDF fonts of
  `environment/fonts` and compares it with `golden/scene.png`
  (`Visual.*`; `--target update-golden` regenerates it).
- `threads/` runs Motif from several threads: `Threads.mtapps` (two
  application contexts and displays, one per thread; the
  ThreadSanitizer test of a `-DWITH_TSAN=ON` build) and
  `Threads.sharedapp` (two threads sharing one application context).
  See `doc/thread-safety.md`.
- `fuzz/` holds the libFuzzer targets (`-DWITH_FUZZERS=ON`, Clang); see
  `fuzz/README.md`.
- `XmString/` holds the old interactive XmString programs and data, which
  are not built.

With `-DWITH_COMPILER_CODE_COVERAGE=ON` the `coverage` target runs the
tests and reports libXm's line coverage (see `coverage.cmake`).

`bench/` holds `xmbench`, a set of micro- and macro-benchmarks for libXm.
It is not part of CTest and is not built by default; `cmake --build
<build> --target bench` builds it and runs every case (the X ones under
`xvfb-run` when it was found) and writes `<build>/src/tests/bench/xmbench.json`.
`xmbench -l` lists the cases; each reports ns, mallocs, X requests, round
trips and `XSetICValues` calls per operation (the counters come from a
small `LD_PRELOAD` library that `xmbench` loads itself; with
`XMBENCH_REPORT=1` it prints them when any program it is preloaded into
exits, see `doc/profiling.md`), and in the JSON the exact requests and
round trips per timed run.

`bench/` also holds `mwmbench`, macro-benchmarks for mwm (built when
libXtst is found): it starts mwm as the window manager of its display
and maps and destroys 500 clients, retitles a client 10,000 times (back
to back, and waiting for mwm each time) and drags a window by its title
bar (opaque and outline) and by its resize handle with XTest.  The
`bench` target runs it too and writes `mwmbench.json`, in the format of
`xmbench.json`, with mwm's and the X server's CPU time, mwm's mallocs,
requests and round trips per operation, and for the map case the time
until all clients are mapped and mwm's memory.  `-m` runs another mwm,
for example one built from another commit; the counters come from
`libmwmbench_preload.so`, which `mwmbench` preloads into mwm.  mwm,
the X server and `mwmbench` wake each other up for every operation;
on a virtual machine the latency of those wake-ups across CPUs can
dwarf the work, so for A/B comparisons run everything on one CPU
(`taskset -c 0 xvfb-run -a mwmbench ...`).

`ab/` holds the A/B harness for the Form, Container and List layout code
(`xm_abtest`, `xm_layoutbench` and `ab.sh`).  It is not part of CTest
either, since a comparison needs two builds of libXm; `cmake --build
<build> --target ab` builds it.  See `ab/README.md`.

## Benchmark runs and the regression gate

`bench/bench.py` runs `xmbench` the same way every time, for one build or
to compare two:

    bench.py run -o out.json <build> [--cases CASE|GROUP ...]
    bench.py run --delay 2 -o out.json <build>
    bench.py compare old.json new.json
    bench.py gate <base-build> <head-build>

- Each build gets its own Xvfb (`-screen 0 1920x1080x24 +extension
  RENDER`, no TCP, no access control), warmed up by one run of every
  case, since the first Motif client of a display leaves state on the
  server (the drag window) that later ones reuse.
- `taskset` pins `xmbench` to the last allowed CPU and the server to the
  one before (`--client-cpu`, `--server-cpu`, `--no-pin`).
- Every case runs in its own process, `--rounds` times (default 5); a
  case's result is the median over the rounds, each being the median of
  `--repeat` runs inside the process (`xmbench -r`, default 3).  With two
  builds the rounds alternate between them.
- `--delay MS` puts `xmbench-proxy` between `xmbench` and the server, over
  TCP: it delays each direction by MS milliseconds, as `tc qdisc add dev
  lo root netem delay MS` would but without root, so that a round trip
  costs 2 × MS more (plus the host's timer wake-up latency).  It also
  counts the requests, replies, errors, events and round trips (replies
  or errors the client receives after having sent something since the
  previous one) of each connection, and the JSON gets them per process,
  under `proxy`.  `bench.proxy` (CTest) checks those counts and the delay.

`compare` and `gate` fail when, for some case, a round trip count grew
(`xmbench`'s count of `_XReply` calls per timed run, or the proxy's
count for the whole process, initialization included), or the median
time per operation is more than `--threshold` % (default 5) slower and
that is unlikely to be noise: a one-sided Mann-Whitney U test over the
rounds must give p < `--alpha` (default 0.001, small because some 40
cases are compared at once).  Five rounds against five cannot reach
that, so `gate` re-runs the cases whose median is above the threshold
with `--confirm` more rounds (default 5) before it decides.  The counts
are deterministic once the server is warm; still, a count is compared
as head's lowest value over the rounds against base's highest, so that
a count that depended on timing would not fail for its jitter.
`bench.compare` (CTest) checks this logic on canned results.

On a quiet machine the rounds of a case spread by a few percent; on a
loaded one (the 2-CPU development box this was written on, shared with
other builds) by 30 % or more, and the test then only reports what it
can tell from noise: a library made 32 % slower in `XmeTraitGet` was
caught there (p = 0.001 over 10 + 10 rounds), one made 3 % slower was
not, and master against an unchanged head gave no time report (lowest
p 0.08).  Round trips are caught whatever the load: one more `XSync`
per `XmGetVisibility` (`visibility`: 1000 -> 2000 per run) or a single
one at `XmDisplay` creation (every X case: +1 proxy round trip).

The `Bench` workflow (`.github/workflows/bench.yml`) runs `bench/gate.sh`
on every pull request: it builds the bench of the pull request's head and
of its merge base in the same job and gates in two passes, `time`
(Unix socket, scale 0.5) and `latency` (`--delay 2`, scale 0.05, where
round trips dominate).  The step summary has the table of both, and the
JSON results are uploaded.  To run it locally:

    git worktree add ../motif-base master
    src/tests/bench/gate.sh ../motif-base . bench-results
