# Security policy

## Supported versions

Security fixes are made on the `master` branch and in the next release.
Only the latest release of this tree is supported.  Upstream Motif 2.3.8
and older releases from The Open Group or ICS are not maintained here,
but most of the issues listed below exist in them as well.

## Reporting a vulnerability

Please do not report security problems in public issues.

- Preferably, use GitHub's private vulnerability reporting: on
  <https://github.com/dimmus/motif/security>, choose *Report a
  vulnerability*.
- Otherwise, write to the maintainer listed in [AUTHORS](AUTHORS).

Include the affected version or commit, a description of the problem and
its impact, and if possible a reproducer: a program, a file (XPM, PNG,
JPEG, SVG, UID, UIL, mwmrc) or the property or client message contents
that trigger it.  Output of an AddressSanitizer build
(`-DWITH_COMPILER_ASAN=ON`) helps a lot.

We aim to acknowledge a report within a week, to agree a disclosure date
with the reporter, and to publish a fix within 90 days of the report.
Fixed vulnerabilities are described in [CHANGELOG.md](CHANGELOG.md) and,
for issues with a CVE identifier, in a GitHub security advisory.  Unless
the reporter prefers otherwise, they are credited.

## Threat model

Motif applications run with the privileges of their user, so the
interesting attacks are those that let someone else control them.  These
inputs are untrusted:

- **Other X clients on the same display.**  Any client can set
  properties on the root window and on other clients' windows, own
  selections and send client messages.  Motif reads such data in drag
  and drop (`_MOTIF_DRAG_*` properties and messages, XDND), the
  clipboard (`_MOTIF_CLIP_*` records on the root window), selection
  transfers (`TARGETS`, `INSERT_SELECTION`, compound text), render
  table and XmString properties, virtual key bindings
  (`_MOTIF_BINDINGS`) and colour sets from other clients.  A bug in any
  of these lets a client crash, and possibly take over, every Motif
  application on the display.
- **Clients, from the point of view of mwm.**  The window manager reads
  menus (`_MOTIF_WM_MENU`), hints and session data from every client it
  manages.
- **Files the user did not write**: images named in resources or
  loaded by the application (XPM, PNG, JPEG, SVG), UID files loaded by
  Mrm, and UIL sources and `.wmd` files given to the UIL compiler.

The X server, the user's own resource files, environment and mwmrc, and
the application's own code are trusted.

## Issue classes fixed so far

A review of the code in 2026 fixed, among others, the following classes
of bugs.  Each fix is a separate commit with a description of the
problem.

- **Out-of-bounds reads and writes on wire data.**  Drag and drop
  receiver info, drop site trees, region boxes, the shared atoms and
  targets tables, byte-swapped protocol fields, clipboard records and
  format counts, compound text and XmString byte streams, render table
  properties, `WM_NORMAL_HINTS` and `_MOTIF_WM_HINTS` were read using
  lengths and counts taken from the data itself.
- **Use-after-free and double free.**  Drop site stream parsing
  continued after freeing the stream; the image cache read and freed
  images after destroying them; render table copies shared slots
  without a reference.
- **Integer overflow in size calculations.**  Image dimensions, XmString
  tag counters and segment counts, clipboard item counts and UID record
  sizes could wrap and lead to undersized allocations.
- **Fixed-size buffer overflows.**  `strcpy`, `strcat` and `sprintf`
  into fixed buffers with input from properties, files or the
  environment: virtual key bindings, font names, path names, colour
  names, input method modifiers, warning texts, UIL file names and
  listing lines, Mrm error messages.
- **Unbounded recursion and loops.**  Cyclic or deeply nested drop site
  trees, UID widget trees and B-tree indexes; XPM comments and pixel
  data running past the end of the input (CVE-2022-46285,
  CVE-2022-44617, CVE-2023-43788, CVE-2023-43789).
- **Running external programs.**  The bundled XPM reader piped `.Z` and
  `.gz` files through `uncompress` and `gunzip` found in `$PATH`
  (CVE-2022-4883); mwm ran its configuration file through `cpp` with a
  shell.
- **Trusting client-supplied commands.**  mwm executed any function,
  including `f.exec`, from a client's `_MOTIF_WM_MENU`, and loaded
  `@file` bitmap labels named by clients.
- **Format string mismatches.**  Translated message catalogs used as
  printf formats had conversions that did not match the arguments; the
  build now checks every translation against the C catalog.
- **Uninitialized memory.**  Mrm and the UIL compiler wrote
  uninitialized bytes into UID files.

Known open issues are tracked in the issue tracker once they have been
disclosed.

## Hardening

Builds use `-fstack-protector-strong`, `-fstack-clash-protection`,
`-fcf-protection`, full RELRO and, in optimized builds,
`_FORTIFY_SOURCE=3` by default (`WITH_HARDENING`).  CI runs the tests
under AddressSanitizer and UndefinedBehaviorSanitizer and runs
scan-build, clang-tidy, cppcheck and CodeQL.
