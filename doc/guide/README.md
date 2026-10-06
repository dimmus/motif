# The Motif Architecture Guide

This guide describes the architecture and implementation of the Motif
toolkit as it exists in this source tree (Motif 2.5).  It is written
for engineers who will read, maintain, extend or port the code: each
chapter states what a subsystem does, how the code does it, which
algorithms and data structures it uses and at what cost, and what the
design buys and gives up.  Every code excerpt is quoted from the files
in this tree and every file is linked, so the guide can be read next
to the sources.

The guide is installed with `WITH_DOCS` under `<datadir>/doc/motif/guide`
([doc/BUILD.md](../BUILD.md)).

## Contents

| Chapter | Subject |
|---------|---------|
| [0. History of Motif development](00-history-of-motif-development.md) | From X and the vendor toolkits of the 1980s, through OSF, the Style Guide, CDE, the licensing crises, ICS and the LGPL release, to the community trees of today; with a source-graded timeline and a table of which company contributed which part of the toolkit. |
| [1. Architecture overview](01-motif-architecture-overview.md) | Libraries and programs, layering on Xlib and Xt, the widget class taxonomy, source layout, generated code, cross-program conventions, the concurrency model, and what Motif 2.5 changed. |
| [1.1 The Xt object model and class records](01-01-xt-object-model-and-class-records.md) | Class and instance records, chained and inherited methods, the `XtInherit` sentinel, constraint records, and why the layouts are ABI. |
| [1.2 Base class extensions, wrappers, fast subclassing and traits](01-02-base-class-extensions-and-traits.md) | How Motif adds hooks, O(1) type tests and interfaces to Xt: the extension records, the wrapper trampolines, the flag bits, and the open-addressing trait table. |
| [1.3 Resources, string tables and converters](01-03-resources-string-tables-and-converters.md) | Resource tables and defaults, the `_XmStrings` offset scheme and its ABI consequence, type converters and the `to` protocol, synthetic resources and unit types, representation types. |
| [2. The drawing and rendering system](02-drawing-and-rendering-system.md) | Shadows, highlights, separators, arrows and indicators at the level of X requests: the segment algorithm, the corner rule, request counts, static buffers and locks. |
| [3. Widget system architecture](03-widget-system-architecture.md) | Primitive, Manager and Gadget; geometry management and the GeoMatrix engine; event dispatch to gadgets; focus, tab groups, traversal, visibility; virtual keys. |
| [3.1 Case study: DataField](03-01-case-study-datafield-subclassing.md) | A widget rewritten from an 8 683-line copy to a 570-line subclass: what chains, what is inherited, what must be re-installed, and the bugs that disappeared. |
| [3.2 Case study: Form](03-02-case-study-form-layout.md) | The attachment constraint solver: Kahn's topological sort with a heap, cached ordering, incremental relaxation with dependency tracking, and how equivalence with the old code was verified. |
| [3.3 Case study: Container](03-03-case-study-container-layout.md) | The outline tree and its O(1) append path; grid and cell placement by linear probing with an occupancy region; traits between container and items. |
| [4. A complete widget: SpinBox](04-practical-widget-implementations.md) | One widget read method by method: class setup, constraints, converters, GCs, layout with graceful degradation, drawing, hit testing, the repeat timer, callbacks and the navigator trait. |
| [5. The graphics pipeline](05-graphics-system-rendering-pipeline.md) | From an image name to a pixmap: readers and content sniffing, the two-level cache keyed by colours, scaling and depth conversion, SVG rasterization, GC caching, and Xft text with its per-display caches. |
| [5.1 Hash tables and caches](05-01-hash-tables-and-caches.md) | The four associative structures in libXm compared: chained hashing with a bucket pool, open addressing with tombstones, move-to-front lists, throwaway probing maps; their hash functions and weaknesses. |
| [6. Implementation deep dive](06-actual-implementations-deep-dive.md) | Compound strings as a tagged union with an optimized form; the locking discipline and its cost; the memory-safety helpers and banned functions; symbol versioning and what remains ABI. |
| [6.1 Text and List data structures](06-01-text-and-list-data-structures.md) | The gap buffer with geometric growth and shrink hysteresis; the List's scroll-by-copy guarded by a generation counter kept in unused ABI fields. |
| [6.2 Drag and drop and inter-client protocols](06-02-drag-and-drop-and-interclient-protocols.md) | Protocol styles and their resolution table, the shared atom and target tables, the drop-site clipping tree, the motion buffer, XDND, the clipboard, and the threat model. |

## How to read it

- **For orientation**, read chapter 1, then 1.1 and 1.2.  Almost
  every later chapter refers back to the object model and to the
  three Motif additions to it (extensions, fast subclassing, traits).
- **To write or port a widget**, read 1.1 to 1.3, 3, 3.1 and 4 in that
  order, then the drawing chapter.  Chapter 4 is a complete worked
  example; chapter 3.1 is the checklist for subclassing.
- **To work on performance**, read 3.2, 3.3, 5, 5.1 and 6.1.  Each
  names the `xmbench` case that measures the code it describes
  ([`src/tests/bench`](../../src/tests/bench)) and, for the layout
  changes, the A/B harness that proved them equivalent to the old code
  ([`src/tests/ab`](../../src/tests/ab/README.md)).
- **To work on security**, read 6, 6.2 and the image loading part of
  5, with [SECURITY.md](../../SECURITY.md) beside them.
- **For the historical and standards context**, chapter 0; its
  reference list grades every source.

## Conventions

- Paths are relative to the top of the source tree and linked.  Code
  is quoted verbatim from the named file; where an excerpt is
  shortened, `...` marks the cut.
- Complexity statements use n for the number of children, items or
  characters of the structure under discussion and name any other
  variable where it is introduced.
- "Upstream" means Motif 2.3.8 (The Open Group and ICS, December 2017);
  "this tree" means Motif 2.4 and 2.5 ([CHANGELOG](../../CHANGELOG.md)).
- The authoritative references for the layers below Motif are the Xt
  Intrinsics specification and the Xlib manual, both linked from the
  chapters that use them; for the public API, the manual pages in
  [`doc/man`](../man); for the ABI, [doc/abi-policy.md](../abi-policy.md).

## Prerequisites

C, and enough X11 to know what a window, a graphics context, a
property, an atom and a selection are.  Chapter 1 §1.2 recalls the Xt
vocabulary; no prior Motif knowledge is assumed.

## Maintaining the guide

The guide documents the code as it is.  When a mechanism described
here changes, change the chapter in the same commit, keep the
excerpts verbatim, and keep the links to the sources working
(`CONTRIBUTING.md` on documentation).  New chapters take the next
number in their section (`03-04-...`, `06-03-...`) and a row in the
table above.
