# Fuzz targets

libFuzzer targets for the parsers that read untrusted input: image
files (PNG, JPEG, SVG, XPM, XBM), `.uid` files, the XmString byte
stream, compound text, the UIL compiler, `.motifbind` bindings, and the
drag-and-drop and clipboard wire data.  They build only with
`-DWITH_FUZZERS=ON` under Clang, and are most useful together with
`-DWITH_COMPILER_ASAN=ON -DWITH_UBSAN=ON`:

    CC=clang cmake -S . -B build -G Ninja -DWITH_TESTS=ON \
        -DWITH_FUZZERS=ON -DWITH_COMPILER_ASAN=ON -DWITH_UBSAN=ON
    cmake --build build

`corpus/<target>` holds a small seed corpus (the image targets also use
the fixtures in `../png`, `../jpeg`, `../svg`).  CTest registers each
target as `Fuzz.<target>`, which replays the corpus once as a
regression test; some targets need an X server and run under xvfb-run.

Run them for real with `run.sh`:

    src/tests/fuzz/run.sh build [seconds-per-target] [target...]

The growing corpus and any crash, leak, out-of-memory or timeout inputs
land in `build/src/tests/fuzz/work/<target>/`.

`crashes/<target>` holds inputs that reproduce library bugs that are not
fixed yet; see `crashes/README.md`.
