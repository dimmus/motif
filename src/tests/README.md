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
`Widgets`, `Text`, `Layout`) need an X server, as do the `Uil.load*`
tests, those in `interactive/` and `visual/` and some in `fuzz/`.  When
`xvfb-run` is found at configure time, CTest starts each of them under
its own Xvfb (with `-noreset`, see `XVFB_RUN_ARGS`); configure with
`-DXVFB_RUN_EXECUTABLE=OFF` to use `$DISPLAY` instead.  Without `DISPLAY`
these tests exit with status 77 and CTest reports them as skipped.

Test cases tagged `xfail` document known library bugs.  They are left out
of normal runs and checked by `Xm.<suite>.xfail` (`motif_tests --xfail
<suite>`), which passes only while all of them still fail.  When a fix
makes one pass, remove its tag.

The other directories:

- `uil/` compiles every `.uil` file in the tree with `uil` and loads the
  `.uid` with Mrm (`Uil.compile.*`, `Uil.load.*`, `Uil.loadbuffer.*`).
- `interactive/` drives a Text/TextField program and mwm (in a nested
  Xephyr) with real input through `xdotool` (`Text.xdotool`,
  `Mwm.xdotool`); they are skipped without `xdotool` or `Xephyr`.
- `visual/` renders a fixed scene with the BDF fonts of
  `environment/fonts` and compares it with `golden/scene.png`
  (`Visual.*`; `--target update-golden` regenerates it).
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
small `LD_PRELOAD` library that `xmbench` loads itself), and in the JSON
the exact requests and round trips per timed run.

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
