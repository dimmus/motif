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
`Widgets`, `Text`, `Layout`, `I18nLocale`, `Rtl`, `Xim`) need
an X server, as do the `Uil.load*`
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
small `LD_PRELOAD` library that `xmbench` loads itself).

`ab/` holds the A/B harness for the Form, Container and List layout code
(`xm_abtest`, `xm_layoutbench` and `ab.sh`).  It is not part of CTest
either, since a comparison needs two builds of libXm; `cmake --build
<build> --target ab` builds it.  See `ab/README.md`.
