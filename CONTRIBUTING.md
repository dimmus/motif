# Contributing to Motif

Bug reports, fixes, tests, documentation and translations are welcome.
Report bugs in the [issue tracker](https://github.com/dimmus/motif/issues)
and send changes as pull requests against `master`.  Report security
problems privately, as described in [SECURITY.md](SECURITY.md).

## Building and testing

Configure a debug build with the tests, in a directory of its own:

```sh
cmake -S . -B _build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON
cmake --build _build
ctest --test-dir _build --output-on-failure
```

The libraries, mwm, the build tools and the tests build without
warnings with GCC and Clang under the warning list in `CMakeLists.txt`;
keep it that way by building with `-DWITH_WERROR=ON`, as CI does.

The tests need libcheck, and `xvfb-run` for the suites that open a
display (see [src/tests/README.md](src/tests/README.md)).  Tests that
need an X server must exit with status 77 when `DISPLAY` is not set, so
that a plain `ctest` works anywhere; CTest runs them under xvfb-run when
it was found at configure time.

Before sending a change that touches parsing, memory management or
anything that handles data from other clients or from files, also run
the tests in a sanitizer build:

```sh
cmake -S . -B _build-asan -G Ninja -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON \
    -DWITH_COMPILER_ASAN=ON -DWITH_UBSAN=ON
```

CI runs the same scripts as `tools/dev/env/ci/build.sh` (for example
`MOTIF_CI_PROFILE=debug-asan CC=clang tools/dev/env/ci/build.sh`), on the
systems listed in the [README](README.md#platforms).  Its static analysis
jobs compare the number of clang-tidy, scan-build and cppcheck findings
per file with the baselines in `tools/dev/env/ci/baselines`: a change
must not add findings, and when it removes some the baseline is lowered
in the same change (`UPDATE_BASELINE=1 tools/dev/env/ci/static-analysis.sh
...`).

## Writing changes

- Keep each commit to one logical change, and keep unrelated reformatting
  out of it.
- Follow the style of the surrounding code: C17, two-space indentation in
  libXm.  The top-level `.clang-format` describes that style; use it on
  the lines you change, not on whole files (`git clang-format` does
  that).  Mrm, Uil, mwm, wml and the tests keep their own older style,
  and their `.clang-format` turns formatting off.  `.editorconfig` sets
  the basics for editors, and `.pre-commit-config.yaml` has hooks for
  [pre-commit](https://pre-commit.com) that format the changed lines and
  reject whitespace errors and build output.
- In libXm, allocate arrays with `_XmMallocArray`/`_XmReallocArray`
  (`XmI.h`) rather than multiplying sizes for `XtMalloc`, and read window
  properties with `_XmGetWindowPropertyChecked`: any client can write
  them.  The libraries cannot call `sprintf`, `vsprintf`, `strcpy` or
  `strcat` (`src/lib/XmBannedI.h` makes that a compile error); use
  `snprintf`, `memcpy` with a known length, `XtNewString` or
  `_XmConcatStrings`.
- Add a test for a bug fix where the test suite can express it: a
  libcheck case in `src/tests/Xm`, or a case in
  `src/bin/uil/tests/uil_robustness.cmake` for the UIL compiler.  Tests
  that would fail before a fix lands can be tagged `xfail` (see
  `src/tests/README.md`).
- Do not commit build output.  The build never writes into the source
  tree; if a file shows up in `git status` after a build, that is a bug.

### Commit messages

Use the form

```
<area>: <summary in the imperative>

<what was wrong and why the change fixes it>
```

where the area is the library, file or component (`Xm`, `Mrm`, `Uil`,
`mwm`, `DragICC`, `build`, `ci`, `tests`, `doc`, `localized`, ...), for
example `CutPaste: bound the format and item counts used for
allocations`.  The body should explain the bug or the reason for the
change, not repeat the diff; for a security fix, describe the input that
triggers it and its consequences.

### Public API and ABI

libXm, libMrm and libUil are shared libraries with SONAME 5.  A change
must not alter the layout of structures in the installed headers
(including the `*P.h` widget headers that subclasses compile against),
the signature of an exported function or the meaning of a resource
without a deliberate decision to break the ABI, which means a new
SONAME (see `MOTIF_SOVERSION` in `CMakeLists.txt`).  Compare with
libabigail when in doubt:

```sh
tools/dev/env/ci/abi-check.sh master
```

New exported functions need a declaration in an installed header, an
entry in `src/lib/Xm/libXm.elist` or `src/lib/Mrm/libMrm.elist`, and a
manual page.

## Releases

1. Set the version in `project(Motif VERSION ...)` in `CMakeLists.txt`
   and give `CHANGELOG.md` a `## X.Y.Z (date)` section.
2. Tag the commit with that version, `git tag -a X.Y.Z -m "Motif X.Y.Z"`,
   and push the tag.

The Release workflow (`.github/workflows/release.yml`) then makes the
tarball with `tools/dev/env/ci/dist.sh`, builds and tests Motif from it,
and publishes the GitHub release with the tarball, its checksum and the
`CHANGELOG.md` section as notes.  It refuses a tag that does not match
the version in `CMakeLists.txt`.  The notes can be edited on the release
page afterwards.

## Documentation

The manual pages are in `doc/man/man3` (one page per function, in the
format of the existing pages; functions documented on a shared page get
a one-line `.so man3/<page>.3` file).  Check new pages with
`groff -man -t -ww -z <page>`.

## Translations

The message catalogs are `src/lib/{Xm,Mrm,Uil}/*.msg` (English) and
their translations in `localized/<lang>/msg`, in UTF-8.  A translation
must list the same sets and message ids in the same order as the English
catalog and keep every printf conversion (`%s`, `%d`, ...) in the same
order; a build with `-DWITH_MESSAGE_CATALOG=ON` checks both.  Messages
that still have the English text are marked `$ TODO: translate`.  See
[doc/LOCALIZATION.md](doc/LOCALIZATION.md).

## Licence

Motif is distributed under the LGPL 2.1 or later (see [LICENSE](LICENSE)).
By contributing you agree that your contribution is distributed under
the same terms.
