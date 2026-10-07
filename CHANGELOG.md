# Changelog

This file summarises the changes in this tree since Motif 2.3.8, the last
upstream release (December 2017).  The git history has the details.

## Unreleased

### Compatibility

- The `xmbind.alias` lookup is gone.  libXm, mwm and xmbind no longer
  read `~/xmbind.alias` or `$XMBINDDIR/xmbind.alias` to pick a bindings
  file from the X server vendor string; they read `~/.motifbind` and
  otherwise use the built-in bindings, which still include the vendor
  tables libXm has always carried.  The vendor files that went with the
  alias (dec, doubleclick, hal, hp, ibm, sgi, sony, sun, sun_at) and
  `xmbind.alias` are no longer installed, and the installed `XmosP.h`
  no longer defines `XMBINDDIR`, `XMBINDDIR_FALLBACK` or `XMBINDFILE`.
  The `CDE` and `pc` files are still installed in `share/X11/bindings`
  as sample `.motifbind` files.  No current X server reported a vendor
  string the alias file matched.

### Build

- The warning list no longer turns any warning off: the `-Wno-*`
  flags for unused variables and functions, misleading indentation,
  char subscripts, unknown pragmas, division by zero,
  `-Wstringop-overread`, Clang's tautological comparisons and the
  flex fall-throughs in wml are gone, and so are the ones that hid
  missing or stale profiles under `WITH_PGO=USE`.  The libraries,
  programs, tools, tests and examples build without warnings with GCC
  and Clang, also with LTO, the sanitizers and PGO.  The dead code this
  exposed was removed, including `src/lib/Wsm/debug.c`, and mwm's
  what(1) version string now takes the project version.
- `WITH_PGO` instruments and optimizes only the code the training runs:
  libXm, libMrm, the UIL compiler and mwm.  The examples, tests and
  build tools had no profile, and with Clang their many `main()`
  functions shared one profile record.

### Code

- `XmTextField` and `XmText` crashed when their render table had no
  loaded font, as with a rendition that names a font but leaves
  `XmNfontType` at `XmAS_IS`.  They fell back to the default text render
  table, which inside a BulletinBoard is its `XmNtextRenderTable` and so
  often the same fontless table, and kept a NULL font.  They now fall
  back further to the system default render table (`XmDEFAULT_FONT`),
  still warning that the table has no font.  `XmText` no longer leaks
  the font context on that path, and setting its render table to NULL
  no longer stores the parent's table without a copy and frees it later.

### Documentation

- `doc/guide` rewritten as a technical architecture guide of sixteen
  chapters, with every excerpt taken from the sources in this tree and
  linked to them: the Xt object model, base class extensions and
  traits, resources and string tables, drawing, the widget system with
  case studies of DataField, Form and Container, a SpinBox walk-through,
  the graphics pipeline and its caches, compound strings, locking and
  memory safety, the Text and List data structures, and drag and drop.
  A new chapter on the history of Motif development, from the 1980s
  vendor toolkits and OSF to the LGPL release and the current trees,
  consolidates publicly available sources with their evidence graded.

### Security

- XPM: scanning an `XImage` whose `bitmap_unit` is zero, not a multiple
  of 8 or wider than a pixel, or whose depth is above 32, divided by
  zero or overflowed a stack variable; such images are now rejected.
- XmString layout: a string with layout direction pushes that are not
  popped (or popped on a later line) could make `XmStringExtent` and
  `XmStringDraw` loop forever or read past the string's segments, so a
  pasted or dropped compound string could hang a client.  Pops without
  a push no longer leave segments unmeasured.

## 2.5.0 (2026-10-04)

Changes since 2.4.1.  The release is 2.5.0 rather than 2.4.2 because
the libraries no longer export the internal symbols that 2.4.1 did
(see below), although they keep SONAME 5.

### Compatibility

- The libraries keep SONAME 5 (`libXm.so.5`, `libMrm.so.5`, and now
  `libUil.so.5`, which used to be static only), with file version 5.0.0.
  abidiff against an upstream 2.3.8 build shows that the ABI is not
  compatible: `XmPrimitivePart` and `XmGadgetPart` lost the
  `OM22_COMPATIBILITY` member `tool_tip_string`, so every widget record
  and subclass part offset moved; the `_XmStrings` tables were re-ordered,
  so 1624 of the 1692 `XmC*`/`XmR*`/`XmS*` macros compiled into 2.3.8
  programs name other strings; and `_XmEditResCheckMessages`,
  `XmDataFielddf_ClearSelection` and `XmDataFielddf_SetCursorPosition` are
  no longer exported.  Programs and widgets built against 2.3.8 must be
  rebuilt.
- The shared libraries export only their API, through version scripts
  with the version nodes `XM_2.5`, `MRM_2.5` and `UIL_2.5`: libXm went
  from 3224 to 1742 exported symbols, libMrm from 336 to 218 and libUil
  from 433 to 50.  The symbols that are gone are internal (`_Xm*` that
  the old export lists already marked internal, the vendored nanosvg,
  the Idb/Urm internals, the UIL parser's `yyparse` and `yylex`).  The
  internal headers (`*I.h`, `Mrm/IDB.h`, `Mrm/Mrm.h`, ...) are no longer
  installed.  See [doc/abi-policy.md](doc/abi-policy.md).
- `XmStringCreate`, `XmStringCreateLocalized`, `XmStringLtoRCreate`,
  `XmStringCreateSimple` and `XmStringCreateLtoR` take `const char *`.
- Every installed header can be included on its own and from C++
  (C linkage for `<Xm/obsolete.h>` and `<uil/UilSymGl.h>`, no `register`
  in prototypes, a fixed `extern "C"` block in `PanedP.h`).
- New function `MrmOpenHierarchyFromBufferWithSize`, which takes the size
  of the UID buffer so that it can be validated.
- `<Xm/DataF.h>` now declares the seventeen exported `XmDataField`
  functions it lacked, and `<Xm/TabList.h>` declares
  `XmTabAttributesFree` (it declared the misspelt `XmTabAttibutesFree`,
  which is kept as a macro).
- `XmDataField` is now a subclass of `XmTextField`, as its manual page
  always said, instead of a copy of it.  `XmIsTextField()` is true for a
  DataField, so the `XmTextField*` functions, `XmTextGetString` and the
  other `XmText*` functions that accept a TextField, SpinBox,
  BulletinBoard's `XmNtextTranslations` and RowColumn's text alignment
  all handle it.  It gets TextField's selection, clipboard and drag and
  drop code, translations and resource defaults.  The `XmDataField*`
  functions and the exported `_XmDataField*` ones remain, as wrappers.
- `XmTextField` has the `XmNalignment` resource that only DataField had:
  `XmALIGNMENT_END` keeps the end of the text at the right margin.  The
  new `alignment` member of `XmTextFieldPart` is in what was tail
  padding, so the size of the record and the offsets of the other
  members did not change (abidiff: compatible change).  The `alignment`
  member of `XmDataFieldPart` is unused; `XmDataField_alignment()` reads
  the TextField one.

### Security

A review of the code that parses data from other X clients and from
files fixed a large number of memory-safety bugs.  See
[SECURITY.md](SECURITY.md) for the classes of issues.  In summary:

- Drag and drop: the Motif drag protocol messages, drop site and
  receiver-info properties, the shared atoms and targets tables and
  XDND data are bounds-checked and byte-swapped correctly; a remote drop
  site stream can no longer cause a use-after-free.
- Clipboard and selections: clipboard records, format registrations and
  item counts read from the root window are validated; selection
  replies (`TARGETS`, `INSERT_SELECTION`, compound text) are checked
  for type and length; waits for a foreign clipboard owner are bounded.
- XmString and render tables: byte streams are validated before use,
  tag counters and segment counts can no longer overflow, render table
  properties from other clients are parsed safely, and render table
  reference counting is fixed.
- Strings and buffers: fixed-size `strcpy`/`sprintf`/`strcat` buffers
  across Xm (virtual key bindings, font names, path names, warning
  texts, colour names, input method modifiers) were replaced or bounded.
- Images: the bundled XPM code is fixed for CVE-2022-44617,
  CVE-2022-46285, CVE-2023-43788, CVE-2023-43789 and a buffer writer
  overflow, and never runs external decompressors (CVE-2022-4883); the PNG, JPEG
  and SVG loaders check sizes and overflow, and the image cache has no
  use-after-free or lock imbalance left.
- Mrm: UID files are validated (record and data entry reads, B-tree
  indexes, compression tables, widget records, argument lists,
  literals, icons, compound strings) and cyclic widget trees are
  rejected.
- UIL compiler: buffer overflows in the lexer, the listing, file names,
  the `.wmd` database reader and literal handling were fixed; a
  robustness test feeds it pathological input.
- mwm: client-supplied menus (`_MOTIF_WM_MENU`), window manager
  properties, `WM_NORMAL_HINTS`, session client data and `@file`
  bitmap labels are no longer trusted; the shell-based `cpp` config
  file path is gone.
- Translations: five translated messages had printf conversions that
  did not match their arguments; the build now rejects such
  translations.
- Earlier, fixes for issues reported by Coverity in the UIL compiler,
  Mrm, DataField, Text, TabStack and the geometry code.

### Build

- CMake is the only build system (3.16 or later).  It honours
  `CMAKE_BUILD_TYPE`, builds as C17 with GNU extensions, never writes
  into the source tree, refuses in-source builds and supports
  cross-compiling (`MOTIF_HOST_TOOLS`).  The `GNUmakefile` is a small
  wrapper around it.
- Hardening flags are on by default (`WITH_HARDENING`); new options
  `WITH_UBSAN`, `WITH_TSAN`, `WITH_MSAN`, `WITH_LTO`, `WITH_CPU_NATIVE`
  and `WITH_NINJA_POOL_JOBS`; the sanitizer and coverage options work.
- The install provides `motif.pc`, `mrm.pc` and `uil.pc` and a CMake
  package (`find_package(Motif CONFIG)` with `Motif::Xm`, `Motif::Mrm`
  and `Motif::Uil`), installs only what users need, and installs
  `system.mwmrc` where mwm looks for it.
- Optimized builds use `-fno-semantic-interposition` and, with
  immediate binding, `-fno-plt`; `WITH_PGO=GENERATE|USE` builds with
  profile-guided optimization.
- `WITH_MESSAGE_CATALOG=ON` generates the C message catalogs from the
  symbolic sources and builds, checks and installs the German, Spanish,
  French, Italian and Japanese translations.  These are now UTF-8 and
  contain every message id.

### Code

- HP-UX and AIX code removed; `demos` renamed to `src/examples`.
- The `workspace` demo and its `WsmDemo` library are gone: the demo needs
  the workspace manager protocol of Mwm 2.0, which this mwm does not
  implement, so it could never connect.
- Performance: `XmForm` sorts and sizes its children in O(n log n) and
  `XmContainer` appends children without walking their level; `XmList`
  scrolls by copying the rows that stay visible and no longer rescans
  every item on selection or deletion; the Xft fonts, colours and draws
  are cached per display; traits, gadget caches and extension records
  use cheaper lookups; menus, ScrollBar autorepeat, `XmGetVisibility`
  and the input method spot location make fewer X requests; the
  Text gap buffer grows geometrically.  `xmbench` (`--target bench`)
  measures these.
- `DataF.c` shrank from 8683 lines, plus 748 in the removed `DataFSel.c`,
  to about 570 now that DataField subclasses TextField.  Fixed on the
  way: DataField installed a pointer to a local variable as its transfer
  trait; setting `XmNpicture` registered the picture check a second time,
  so a rejected character called `XmNpictureErrorCallback` twice; and
  the picture check leaked a copy of the value for every character it
  checked.  `XmTextFieldReplaceWcs` still freed a string literal when its
  argument did not convert to the locale's encoding; that had been fixed
  only in DataField's copy.
- UIL compiler: on 64-bit big-endian machines (s390x, ppc64) every
  binary expression on integers or booleans, such as `2 + 3` or
  `6 ^ 3`, compiled to 0.  The operands were read as an `int` overlaying
  the high half of a `long`.

### Tests and CI

- Legacy test trees that no longer built were removed; the old
  interactive XmString programs remain, unbuilt, in `src/tests/XmString`.
- The libcheck suite builds with `WITH_TESTS=ON` and runs under CTest,
  with the X11 suites under xvfb-run.  It covers XmString, every widget
  class, Text and TextField, i18n conversions and Form/List/Container
  layout; every `.uil` file in the tree is compiled and loaded with Mrm;
  Text and mwm are driven with real input through xdotool (mwm in a
  nested Xephyr); a visual test compares a rendered scene with a golden
  image; `abi.exports.*` check the version scripts against the headers.
- libFuzzer targets for the parsers of untrusted input
  (`WITH_FUZZERS=ON`, Clang) and a `coverage` target with a ratchet.
- CI covers glibc and musl Linux, 32-bit x86, big-endian s390x and
  FreeBSD with GCC and Clang, ASan/UBSan builds, packaging on Debian and
  Fedora, static analysis with ratcheted baselines, ABI comparison and
  reproducible builds.

### Documentation

- New manual pages for the `XmLog`, `XmeXpm`, TabBox, TabStack,
  `XmTabbedStackList`, DataField, DropDown, `XmI18List` and creation
  functions that had none.
- `CHANGELOG.md`, `CONTRIBUTING.md`, `SECURITY.md`, `AUTHORS` and the
  API and ABI policy (`doc/abi-policy.md`); the README describes the
  build as it is.

## 2.4.1 (2025-09-03)

- A logging facility, the `XmLog` functions in `<Xm/Log.h>`, always
  built in, with `LOG_LEVEL` and `LOG_OUTPUT` build options.
- K&R function definitions converted to prototypes throughout; legacy
  SVR4/SYSV, VMS, HP-UX and AIX code removed; `NeedFunctionPrototypes`
  conditionals removed.
- Builds with current GCC and Clang and on Alpine Linux (musl); many
  compiler warnings fixed.
- Documentation of the toolkit architecture in `doc/guide`.

## 2.4.0 (2025-08-23)

The first release of this tree, continuing from the upstream
repository after 2.3.8.

- Fixes for upstream bugs 1624 to 1708 that were committed upstream
  after 2.3.8 but never released.
- The bundled XPM code was updated to libXpm 3.5.12 (upstream, 2023).
- Fixes from other maintained Motif trees, including MrmOpenHierarchyPerDisplay
  crashing (bug 1161), `XmList` ringing the bell with
  `XmQUICK_NAVIGATE` (bug 1210) and `XmTextSetInsertionPosition` in
  modify/verify callbacks (bug 1366).
- Motif 2.2 compatibility (`OM22_COMPATIBILITY`) and imake support
  removed; the autotools build modernised; GCC 15 build errors fixed;
  unsafe `sprintf`, `tempnam` and `fprintf` uses replaced.
- The libtool library version became 5:0:0 (`libXm.so.5`).
- LGPL-2.1 licence text added; a clang-format style; GitHub and
  GitVerse CI.
