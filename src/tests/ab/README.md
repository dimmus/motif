# Layout, XmString and render table A/B harness

Tools for checking that a change to the Form, Container or List layout
code, to XmString, or to the String to RenderTable conversion, keeps
their behaviour identical, and for timing it; and for measuring round
trips over a slow connection.  They were used to validate the layout
performance work (the Form sort and sizing, the Container insert fast
path and the List selection and scrolling changes), the XmString
building and extent cache work and the cache of converted render
tables.
They are not tests: a comparison needs two builds of libXm, so nothing
here is registered with CTest or built by default.

- `xm_abtest MODE SEED [SIZE]` builds a pseudo-random configuration from
  SEED, drives it through the API and through real input (xdotool), and
  prints child geometry, selection and scroll state, callbacks and a hash
  of the window pixels after every step.  Modes: `form`, `formcyc`
  (attachment cycles), `formgrid`, `formcolumn`, `formwide`, `container`,
  `containertree` (the Container entry tree and XmNpositionIndex through
  the API only, no input; SIZE is the number of steps), `list`,
  `listscroll`, `listapi` (the List API alone, without input:
  lookups, selection and replacement by value with many duplicates, and
  the item and selection resources), `listmix` (the same with keyboard
  and button actions of the List, called with synthetic events,
  interleaved with the API, also between a button press and its release,
  and changes of the selection policy), `xmstring`, which builds
  strings from SIZE random pieces (concatenation, copies,
  XmStringGenerate, XmStringParseText) and prints their byte streams,
  text and extents with a core font, a font set and Xft, also after the
  render tables change; and `rendertable`, which builds a random
  resource database of rendition resources and a random widget tree
  whose widgets convert render tables and font lists from strings, and
  prints every table and every warning (no input there).
- `ab.sh OLD_LIBDIR NEW_LIBDIR MODE FIRST LAST [SIZE]` runs a range of
  seeds against both libraries and reports seeds whose output differs.
- `xm_layoutbench MODE N` times the phases of a layout with N children
  or items.  Modes: `form`, `formgrid`, `outline`, `spatial`, `detail`,
  `fillhead` and `fillrandom` (a Container filled by inserting each icon
  at the front or at a random place), `list`, `listops` (the List item
  and selection operations: adds at both ends, lookups, selection and
  deletion by value and by position, replacements) and `rendertable`
  (the creation of N Labels whose XmNrenderTable comes from the same
  resource: a font list, one and three renditions from the database, an
  Xft rendition).
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

## Round trips and latency

A round trip costs nothing on a local Xvfb and a lot over ssh or a
remote display.  To see it:

- `latency.sh DELAY_MS COMMAND...` starts a private Xvfb and runs
  COMMAND with `DISPLAY` going through `xproxy.py`, which holds the X
  traffic DELAY_MS in each direction (a round trip of about twice
  DELAY_MS) and appends to `latency-stats.jsonl` one line per connection
  with its requests (by opcode), replies (round trips), events and
  errors.  `DRIVER_DISPLAY` names the Xvfb itself.
- `xm_repeatbench [HOLD_MS [WORK [RUNS]]]` holds a ScrollBar arrow down
  through XTest on `DRIVER_DISPLAY` and prints the repeat rate and how
  long after the release the application saw it and the server had
  drawn everything.  WORK adds that many 800x600 copies per repeat, for
  a server that cannot keep up.  It is built when libXtst is found.

The ScrollBar autorepeat used to XSync after every repeat; to compare:

    for lib in build-old build-new; do
        LD_LIBRARY_PATH=$lib/src/lib/Xm src/tests/ab/latency.sh 25 \
            build-new/src/tests/ab/xm_repeatbench 3000 0 5
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
