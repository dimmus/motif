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

Suites labelled `X11` (`FontList`, `FontListEntry`, `Layout`) need an X
server.  When `xvfb-run` is found at configure time, CTest starts each of
them under its own Xvfb; configure with `-DXVFB_RUN_EXECUTABLE=OFF` to use
`$DISPLAY` instead.  Without `DISPLAY` these suites exit with status 77 and
CTest reports them as skipped.

Test cases tagged `xfail` document known library bugs.  They are left out
of normal runs and checked by `Xm.<suite>.xfail` (`motif_tests --xfail
<suite>`), which passes only while all of them still fail.  When a fix
makes one pass, remove its tag.

`XmString/` holds the old interactive XmString programs and data, which
are not built; `environment/fonts` holds BDF fonts for future rendering
tests.

`bench/` holds `xmbench`, a set of micro- and macro-benchmarks for libXm.
It is not part of CTest and is not built by default; `cmake --build
<build> --target bench` builds it and runs every case (the X ones under
`xvfb-run` when it was found) and writes `<build>/src/tests/bench/xmbench.json`.
`xmbench -l` lists the cases; each reports ns, mallocs, X requests, round
trips and `XSetICValues` calls per operation (the counters come from a
small `LD_PRELOAD` library that `xmbench` loads itself).
