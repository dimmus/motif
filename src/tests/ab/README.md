# Layout A/B harness

Tools for checking that a change to the Form, Container or List layout
code keeps their behaviour identical, and for timing it.  They were used
to validate the layout performance work (the Form sort and sizing, the
Container insert fast path and the List selection and scrolling changes).
They are not tests: a comparison needs two builds of libXm, so nothing
here is registered with CTest or built by default.

- `xm_abtest MODE SEED [SIZE]` builds a pseudo-random configuration from
  SEED, drives it through the API and through real input (xdotool), and
  prints child geometry, selection and scroll state, callbacks and a hash
  of the window pixels after every step.  Modes: `form`, `formcyc`
  (attachment cycles), `formgrid`, `formcolumn`, `formwide`, `container`,
  `list`, `listscroll`.
- `ab.sh OLD_LIBDIR NEW_LIBDIR MODE FIRST LAST [SIZE]` runs a range of
  seeds against both libraries and reports seeds whose output differs.
- `xm_layoutbench form|container|list N` times the phases of a layout
  with N children or items.
- `xm_shadowbench check|time [N]` compares XmeDrawShadows (one line
  segment per pixel row and column) with XFillRectangles of the same
  rows and with polygons: `check` counts the pixels that differ for
  every shadow type, thicknesses 0 to 10 and several GCs, `time` gives
  the time and request bytes per shadow.  It needs only one library.

## Usage

Build the old and the new library (two build directories), then the
harness in one of them:

    cmake --build build-new --target ab

Run a comparison on a virtual display:

    xvfb-run -a -s "-screen 0 1280x1024x24" \
        env XM_ABTEST=build-new/src/tests/ab/xm_abtest \
        src/tests/ab/ab.sh build-old/src/lib/Xm build-new/src/lib/Xm form 1 100

Check first that the harness is deterministic by passing the same
library twice: every seed must come out identical.  Spaced clicks keep
xdotool input out of the double-click interval, so this holds on a quiet
machine; under heavy load raise `AB_TIMEOUT` rather than shortening it.

Timings:

    for lib in build-old build-new; do
        LD_LIBRARY_PATH=$lib/src/lib/Xm xvfb-run -a \
            build-new/src/tests/ab/xm_layoutbench form 1000
    done

## Notes

- The two libraries must have the same SONAME, since the harness is
  loaded against one and run against the other with `LD_LIBRARY_PATH`.
- libXm has versioned symbols (`XM_2.5`) since the symbol export work.
  A harness linked against a versioned library will not load an older
  unversioned one, but one linked against an unversioned library runs
  against both; to compare across that change, build the harness
  against the older library.
- A useful sanity check is a mutation test: break the code under test on
  purpose (for example skip the Form re-sort) and confirm that some seeds
  differ.
