# Building Motif

The [README](../README.md) covers the requirements, the common options
and testing.  This file adds the details for packagers and developers.

## CMake

Motif builds with CMake 3.16 or later, out of the source tree:

```sh
cmake -S . -B _build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build _build
ctest --test-dir _build            # with -DWITH_TESTS=ON
DESTDIR=/tmp/stage cmake --install _build
```

- Without `CMAKE_BUILD_TYPE` a single-configuration generator builds
  `RelWithDebInfo`.  Debug keeps `assert()`; the other types define
  `NDEBUG`.
- The code is compiled as C17 with GNU extensions (`-std=gnu17`) and
  `_GNU_SOURCE`.  GCC older than 11 and Clang older than 8 are rejected.
- Warnings come from one curated list in `CMakeLists.txt`; a few always
  indicate bugs and are errors (`-Werror=implicit-function-declaration`,
  `-Werror=return-type`, `-Werror=vla`).  Set
  `CMAKE_COMPILE_WARNING_AS_ERROR=ON` to make all warnings errors.
- The build never writes into the source tree, and configuring in the
  source directory is refused (`WITH_IN_SOURCE_BUILD=ON` overrides).
- `cmake -LH _build` lists every option with its help text; the summary
  printed at the end of the configure run shows the ones in effect.

## Presets

`tools/cmake/config` holds initial caches for common configurations,
used with `cmake -C`:

| Preset | Build type | Notes |
|--------|------------|-------|
| `motif_debug.cmake` | Debug | Tests, examples, coverage, all features, UIL debugging |
| `motif_developer.cmake` | Debug | Tests, examples, coverage, ASan, ccache, Ninja job pools |
| `motif_release.cmake` | Release | `-O3`, LTO, all features, examples, no tests |
| `motif_full.cmake` | Release | `-O3`, LTO, `-march=native`, all features, tests |
| `motif_lite.cmake` | Release | `-Os`, stripped, no optional features, examples or docs |

```sh
cmake -C tools/cmake/config/motif_release.cmake -S . -B _build-release -G Ninja
```

A preset only seeds a new cache; options given with `-D` on the same
command line win.

## The GNUmakefile

`make` in the source directory configures and builds in
`../build_<os>` (override with `BUILD_DIR=`).  The goals `debug`,
`release`, `full`, `lite` and `developer` pick the preset of the same
name and a build directory of their own (`../build_<os>_release`, ...),
and `ninja` and `ccache` add the Ninja generator and ccache; goals can be
combined, as in `make release ninja`.  `make install`, `make test`,
`make clean` (removes the CMake cache), `make clean_all` (removes the
build directory) and `make help` do what their names say;
`BUILD_CMAKE_ARGS` passes extra arguments to cmake.

## What is installed

With the default `CMAKE_INSTALL_PREFIX` of `/usr/local` and
GNUInstallDirs:

| Path | Contents |
|------|----------|
| `lib/libXm.so.5`, `libMrm.so.5`, `libUil.so.5` | The libraries (static with `WITH_SHARED_LIBS=OFF`) |
| `include/Xm`, `include/Mrm`, `include/uil` | Public and widget-writer headers |
| `lib/pkgconfig/motif.pc`, `mrm.pc`, `uil.pc` | pkg-config files, relative to `${prefix}` |
| `lib/cmake/Motif` | The CMake package for `find_package(Motif CONFIG)` |
| `bin/uil`, `bin/mwm`, `bin/xmbind` | Programs |
| `etc/X11/system.mwmrc` | mwm's default configuration (`CMAKE_INSTALL_SYSCONFDIR`) |
| `share/X11/bindings` | Sample `.motifbind` files (`CDE`, `pc`) |
| `include/X11/bitmaps` | Bitmaps used by Motif applications |
| `share/man/man1`, `man3`, `man4`, `man5` | Manual pages (`WITH_DOCS`) |
| `share/doc/motif` | `doc/*.md`, `doc/guide`, `AUTHORS`, `CHANGELOG.md`, `SECURITY.md` (`WITH_DOCS`) |
| `share/locale/<lang>/LC_MESSAGES/{Xm,Mrm,Uil}` | Message catalogs (`WITH_MESSAGE_CATALOG`, needs gencat) |
| `share/Xm/<example>` | Example programs (`WITH_DEMOS`) |

Run-time search paths compiled into the libraries and mwm (for example
where mwm looks for `system.mwmrc`) are absolute paths below the install
prefix.  Packagers normally configure with
`-DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_SYSCONFDIR=/etc` and stage
with `DESTDIR`.  `tools/dev/env/ci/package-smoke.sh` is the packaging
check that CI runs on Debian and Fedora: it builds with the
distribution's flags, checks the staged install (symlinks, RPATH,
`ldd`), installs it and builds a client through pkg-config and
through `find_package(Motif)`.

## Cross-compiling

The build runs `makestrs`, `mkcatdefs`, `wml`, `wmluiltok` and `uil`,
which must run on the build machine.  Build Motif natively first; that
build writes `MotifHostTools.cmake` into its build directory.  Then
configure the cross build with your toolchain file and
`-DMOTIF_HOST_TOOLS=<native build>/MotifHostTools.cmake`, and the tools
are imported from the native build instead of being built.

## Static analysis, ABI and reproducibility

The CI scripts in `tools/dev/env/ci` can be run locally from the top of
the source tree:

- `build.sh` configures, builds and tests one profile
  (`MOTIF_CI_PROFILE=debug`, `debug-asan`, `release` or `release-lto`);
- `static-analysis.sh clang-tidy|scan-build|cppcheck BUILD_DIR OUT_DIR`
  runs an analyser and compares its findings with the baselines in
  `tools/dev/env/ci/baselines`;
- `abi-check.sh REF...` compares the ABI of libXm and libMrm with older
  revisions using libabigail;
- `repro-check.sh DIR` builds twice and compares the results.
