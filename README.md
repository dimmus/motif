# Motif

[![Build](https://github.com/dimmus/motif/actions/workflows/build.yml/badge.svg)](https://github.com/dimmus/motif/actions/workflows/build.yml)
[![CodeQL](https://github.com/dimmus/motif/actions/workflows/codeql.yml/badge.svg)](https://github.com/dimmus/motif/actions/workflows/codeql.yml)
[![Latest tag](https://img.shields.io/github/v/tag/dimmus/motif)](https://github.com/dimmus/motif/tags)
[![License: LGPL-2.1-or-later](https://img.shields.io/badge/license-LGPL--2.1--or--later-blue.svg)](LICENSE)

Motif is the X11 user interface toolkit (libXm), with the Motif Resource
Manager (libMrm), the User Interface Language compiler (uil, libUil) and
the Motif Window Manager (mwm).  Motif was developed by the Open Software
Foundation from 1988 and became the toolkit of the Common Desktop
Environment; The Open Group released it under the LGPL in 2012.

This tree continues Motif 2.3.8, the last upstream release, as version
**2.5.0**.  It keeps the Motif 2.x API but is **not binary compatible**
with 2.3.8: the libraries have SONAME 5 (`libXm.so.5`, `libMrm.so.5`,
`libUil.so.5`) and programs built against 2.3.8 have to be recompiled.
See [CHANGELOG.md](CHANGELOG.md) for what changed and
[SECURITY.md](SECURITY.md) for the security fixes and how to report
vulnerabilities.

## Features

- The Motif 2.x widget set: `XmLabel`, `XmPushButton`, `XmToggleButton`,
  `XmText`, `XmTextField`, `XmList`, `XmScale`, `XmScrollBar`, `XmForm`,
  `XmRowColumn`, `XmPanedWindow`, `XmMainWindow`, `XmNotebook`,
  `XmContainer`, `XmComboBox`, `XmSpinBox`, the dialogs, and the widgets
  merged from the ICS extensions: `XmButtonBox`, `XmColorSelector`,
  `XmColumn`, `XmDataField`, `XmDropDown`, `XmFontSelector`,
  `XmIconBox`, `XmIconButton`, `XmMultiList`, `XmOutline`, `XmPaned`,
  `XmTabStack` and `XmTree`.
- Keyboard traversal, virtual key bindings and drag and drop (Motif and
  XDND).
- Internationalized text input and output, with UTF-8 support
  (`WITH_UTF8`) and anti-aliased fonts through Xft (`WITH_XFT`).
- XPM, PNG, JPEG and SVG images for pixmap resources.
- X/Open message catalogs for the library messages, with German,
  Spanish, French, Italian and Japanese translations
  (`WITH_MESSAGE_CATALOG`).
- Optional printing through libXp (`WITH_PRINTING`).

There is no assistive technology (AT-SPI/ATK) support: screen readers
cannot read Motif applications.

## Platforms

CI builds and tests every change on:

| System | Compilers | Notes |
|--------|-----------|-------|
| Ubuntu 24.04 (glibc, x86_64) | GCC, Clang | Debug with ASan and UBSan; Release with LTO |
| Alpine Linux 3.22 (musl, x86_64 and x86) | GCC, Clang | Debug; Release with LTO on x86_64 |
| Debian stable on s390x (big-endian, under qemu) | GCC | Debug |
| FreeBSD | Clang | Debug |
| Debian stable, Fedora | GCC | Distribution packaging and install-and-consume test |
| Debian trixie, Fedora | GCC | `dpkg-buildpackage` and `rpmbuild` of `tools/packaging`, lintian, rpmlint, install and consume |

CI also runs warnings-as-errors builds (informational), scan-build,
clang-tidy and cppcheck against committed baselines, an ABI comparison
(informational), a reproducible-build check and CodeQL.  The workflows
are in `.github/workflows`; `.gitverse/workflows` mirrors the main Linux
jobs.  The scripts they run are in `tools/dev/env/ci` and can be run
locally.

Other Unix-like systems with X11 may work but are not tested.

## Requirements

- CMake 3.16 or later, and Ninja or make
- A C17 compiler: GCC 11 or later, or Clang 8 or later (CMake rejects
  older versions); the code is built with `-std=gnu17`
- pkg-config, flex (or lex) and bison (or yacc)
- X11 libraries: `x11`, `xt`, `xext`, `xmu`, `xpm` and `fontconfig`
  (required); `xft` 2 or later, `libpng` and `libjpeg` (optional,
  used when found); `xp` (only with `WITH_PRINTING`)
- `check` (libcheck) for the tests; `xvfb-run` (Xvfb and xauth) and the
  core X fonts to run the tests that need an X server; `xdotool` and
  `Xephyr` for the tests that drive Text and mwm with real input
- `abidiff` (libabigail) for the ABI comparison in
  `tools/dev/env/ci/abi-check.sh` (optional)
- `gencat` to compile message catalogs (part of glibc; optional)

On Debian or Ubuntu:

```sh
sudo apt-get install build-essential cmake ninja-build pkg-config flex bison \
    libx11-dev libxt-dev libxext-dev libxmu-dev libxpm-dev libxft-dev \
    libfontconfig-dev libpng-dev libjpeg-dev x11proto-dev xbitmaps \
    check xvfb xauth xfonts-base xdotool xserver-xephyr abigail-tools
```

On Fedora:

```sh
sudo dnf install gcc cmake ninja-build pkgconf-pkg-config flex bison \
    'pkgconfig(x11)' 'pkgconfig(xt)' 'pkgconfig(xext)' 'pkgconfig(xmu)' \
    'pkgconfig(xpm)' 'pkgconfig(xft)' 'pkgconfig(fontconfig)' \
    'pkgconfig(libpng)' 'pkgconfig(libjpeg)' 'pkgconfig(xbitmaps)' \
    'pkgconfig(check)' xorg-x11-server-Xvfb xorg-x11-xauth \
    xorg-x11-fonts-misc xdotool xorg-x11-server-Xephyr libabigail
```

`tools/dev/env/ci/deps.sh` installs everything CI needs on Debian,
Ubuntu, Fedora, Alpine, Arch Linux and FreeBSD.
`tools/dev/scripts/deps_check.sh` reports which build and test
dependencies are missing on your system and offers to install them; see
[tools/dev/README.md](tools/dev/README.md).

## Building

```sh
cmake -S . -B _build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build _build
sudo cmake --install _build
```

The default install prefix is `/usr/local`; packagers pass
`-DCMAKE_INSTALL_PREFIX=/usr` and the usual `CMAKE_INSTALL_*DIR`
variables (GNUInstallDirs), and `DESTDIR` for staging.  In-source builds
are refused.

The `GNUmakefile` is a thin wrapper for people who prefer `make`: `make`
configures and builds in `../build_<os>`, `make install` and `make test`
install and test that build, and the targets `debug`, `release`, `full`,
`lite` and `developer` seed a new build directory from the presets in
`tools/cmake/config` (for example `make release ninja`).  Run `make help`
for the list.

### Options

| Option | Default | Effect |
|--------|---------|--------|
| `WITH_SHARED_LIBS` | ON | Shared libraries (static if OFF) |
| `WITH_UTF8` | ON | UTF-8 text support |
| `WITH_XFT` | ON | Xft fonts, if Xft 2 is found |
| `WITH_PNG`, `WITH_JPEG` | ON | PNG and JPEG images, if the libraries are found |
| `WITH_PRINTING` | OFF | Printing through libXp |
| `WITH_MESSAGE_CATALOG` | OFF | X/Open message catalogs (see below) |
| `WITH_DEMOS` | ON | Build the example programs in `src/examples` |
| `WITH_TESTS` | OFF | Build the tests (needs libcheck) |
| `WITH_DOCS` | ON | Install the manual pages and `doc/*.md` |
| `WITH_UIL_DEBUG` | OFF | Debugging output in the UIL compiler |
| `WITH_HARDENING` | ON | `-fstack-protector-strong`, `-fstack-clash-protection`, `-fcf-protection`, full RELRO, and `_FORTIFY_SOURCE=3` in optimized builds |
| `WITH_LTO` | OFF | Link-time optimization (optimized builds always use `-fno-semantic-interposition`) |
| `WITH_PGO` | OFF | Profile-guided optimization: `GENERATE`, then `USE` (see [doc/abi-policy.md](doc/abi-policy.md)) |
| `WITH_CPU_NATIVE` | OFF | `-march=native` (binaries are not portable) |
| `WITH_WERROR` | OFF | Warnings are errors everywhere but in the examples (CMake 3.24 or later) |
| `WITH_COMPILER_ASAN`, `WITH_UBSAN`, `WITH_TSAN`, `WITH_MSAN` | OFF | Sanitizers (see below) |
| `WITH_COMPILER_CODE_COVERAGE` | OFF | Coverage instrumentation and the `coverage` target |
| `WITH_FUZZERS` | OFF | libFuzzer targets in `src/tests/fuzz` (Clang, with `WITH_TESTS`) |
| `WITH_COMPILER_CCACHE` | OFF | Compile through ccache |
| `WITH_NINJA_POOL_JOBS` | OFF | Limit parallel compile and link jobs by available memory (Ninja) |
| `LOG_LEVEL` | `INFO` | Default level of the `XmLog` functions (`DEBUG`, `INFO`, `WARN`, `ERROR`, `CRITICAL`) |
| `LOG_OUTPUT` | `stderr` | Default destination of the `XmLog` functions (`stderr`, `stdout`, `file`) |
| `MOTIF_HOST_TOOLS` | | `MotifHostTools.cmake` of a native build, for cross-compiling |

The configuration summary at the end of the cmake run lists the options
in effect.

### Cross-compiling

The build runs some of the programs it builds (makestrs, mkcatdefs, wml,
wmluiltok and uil).  To cross-compile, build Motif natively first, then
point the cross build at the `MotifHostTools.cmake` that the native
build wrote:

```sh
cmake -S . -B _cross -DCMAKE_TOOLCHAIN_FILE=... \
    -DMOTIF_HOST_TOOLS=/path/to/native/_build/MotifHostTools.cmake
```

### Message catalogs

With `-DWITH_MESSAGE_CATALOG=ON` the library messages are looked up with
`catgets()`.  The C catalogs are generated from `src/lib/{Xm,Mrm,Uil}/*.msg`
and the translations in `localized/<lang>/msg` are checked against them
(same message ids, same printf conversions); when `gencat` is found
they are compiled and installed as `<localedir>/<lang>/LC_MESSAGES/Xm`,
`Mrm` and `Uil`.  glibc finds them through its default `NLSPATH`; on
musl set `NLSPATH=<localedir>/%l/LC_MESSAGES/%N`.  The translations are
UTF-8.  See [doc/LOCALIZATION.md](doc/LOCALIZATION.md).

## Testing

```sh
cmake -S . -B _build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON
cmake --build _build
ctest --test-dir _build --output-on-failure
```

The tests are a libcheck suite (`src/tests`, one CTest test per suite,
`Xm.<suite>`), a robustness test of the UIL compiler against
pathological input (`uil_robustness`), the compilation and loading of
every `.uil` file in the tree (`Uil.*`), Text and mwm driven with real
input through xdotool (`*.xdotool`), a visual regression test against a
golden image (`Visual.*`) and checks that the libraries export what
their headers declare (`abi.exports.*`).  Tests that need an X server
are labelled `X11`; when `xvfb-run` is found at configure time CTest
runs each of them under its own Xvfb, otherwise they use `$DISPLAY` and
are reported as skipped (exit status 77) when there is none.  With
`-DWITH_FUZZERS=ON` (Clang) CTest also runs each fuzzer over its seed
corpus (`Fuzz.<name>`), and over the reproducers of known bugs that are
not fixed yet, which are expected to fail (`Fuzz.<name>.crashes`).  See
[src/tests/README.md](src/tests/README.md).

Two targets build tools that CTest does not run.  `bench` builds and
runs `xmbench`, the micro- and macro-benchmarks in `src/tests/bench`,
and writes `xmbench.json`.  `ab` builds the A/B harness in
`src/tests/ab`, which runs the same pseudo-random Form, Container and
List configurations against two builds of libXm and reports any
difference in geometry, selection, callbacks or pixels; use it to check
that a change to the layout code keeps their behaviour identical.

### Sanitizers and coverage

`-DWITH_COMPILER_ASAN=ON` (AddressSanitizer with LeakSanitizer),
`-DWITH_UBSAN=ON`, `-DWITH_TSAN=ON` and `-DWITH_MSAN=ON` (Clang only;
every linked library, libX11 and libXt included, must be instrumented
too) build the libraries, programs and tests with that sanitizer.
ASan and UBSan can be combined; TSan and MSan cannot be combined with
ASan.  The build's own code generators run with leak detection off.
UBSan leaves out the alignment check and Clang's function type check
(`-fsanitize=function`), which the casts of handlers to the Xt and trait
function types would set off throughout.

With `-DWITH_COMPILER_CODE_COVERAGE=ON`, GCC builds with `--coverage`
(the `.gcda` files are written next to the objects, for gcov, lcov or
gcovr) and Clang with source-based coverage (set `LLVM_PROFILE_FILE`,
merge with `llvm-profdata merge` and report with `llvm-cov`).

## Using Motif

The install provides pkg-config files for the three libraries:

```sh
cc -o myapp myapp.c $(pkg-config --cflags --libs motif)   # libXm
cc -o myuil myuil.c $(pkg-config --cflags --libs mrm)     # libMrm and libXm
```

and a CMake package.  `CONFIG` is needed, because CMake's own
`FindMotif` module would be used otherwise:

```cmake
find_package(Motif 2.4 CONFIG REQUIRED)          # COMPONENTS Xm Mrm Uil
target_link_libraries(myapp PRIVATE Motif::Xm)   # or Motif::Mrm, Motif::Uil
```

Set `CMAKE_PREFIX_PATH` to the Motif prefix if it is not a default one.

A minimal program:

```c
#include <Xm/Xm.h>
#include <Xm/PushB.h>

static void
activate(Widget w, XtPointer client_data, XtPointer call_data)
{
  XtAppSetExitFlag(XtWidgetToApplicationContext(w));
}

int
main(int argc, char *argv[])
{
  XtAppContext app;
  Widget toplevel, button;

  toplevel = XtVaOpenApplication(&app, "Hello", NULL, 0, &argc, argv, NULL,
                                 sessionShellWidgetClass, NULL);
  button = XmCreatePushButton(toplevel, "hello", NULL, 0);
  XtAddCallback(button, XmNactivateCallback, activate, NULL);
  XtManageChild(button);
  XtRealizeWidget(toplevel);
  XtAppMainLoop(app);
  return 0;
}
```

The same interface in UIL, compiled with `uil -o hello.uid hello.uil`
and loaded with `MrmOpenHierarchy` and `MrmFetchWidget`:

```uil
module hello
    names = case_sensitive

object hello : XmPushButton {
    arguments {
        XmNlabelString = "Hello, World!";
    };
};

end module;
```

The manual pages (`man XmPushButton`, `man uil`, `man mwm`, ...) are
installed with `WITH_DOCS`; `doc/guide` describes the architecture of
the toolkit.  The example programs in `src/examples` are built with
`WITH_DEMOS` and installed to `<datadir>/Xm`.

## Source layout

```
src/lib/Xm       libXm, the widget library
src/lib/Mrm      libMrm, the Motif Resource Manager (reads UID files)
src/lib/Uil      libUil, the callable UIL compiler
src/bin/uil      the uil program
src/bin/mwm      the Motif Window Manager
src/bin/xmbind   xmbind, which installs virtual key bindings
src/bin/wml      the WML tools that generate the UIL compiler tables
src/bin/utils    build-time tools (makestrs, mkcatdefs, mkmsgcat)
src/examples     example programs
src/tests        the tests, fuzzers, benchmarks and layout A/B harness
include          config.h.in and stub CDE headers for the build
data             sample key bindings, bitmaps and the pkg-config templates
localized        translated message catalogs
doc              manual pages and documentation
tools            CMake helpers and presets, CI and development scripts
```

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).  Report bugs in the
[issue tracker](https://github.com/dimmus/motif/issues), and security
problems privately as described in [SECURITY.md](SECURITY.md).

## License

Motif is free software under the GNU Lesser General Public License,
version 2.1 or (at your option) any later version; see
[LICENSE](LICENSE).  Some files carry additional permissive notices
(for example the XPM code in `src/lib/Xm/Xpm*`).

## Acknowledgments

Motif is the work of the Open Software Foundation, The Open Group,
Integrated Computer Solutions (who maintained Motif 2.3) and many
contributors; see [AUTHORS](AUTHORS).  Thanks also to
[Tim Hentenaar](https://github.com/thentenaar),
[Olivier Fourdan](https://github.com/ofourdan) and
[Alexander Pampuchin](https://github.com/alx210), whose work on Motif
this tree draws on.
