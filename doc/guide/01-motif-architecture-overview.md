# 1. Motif Architecture Overview

**Scope.** This chapter describes the structure of the Motif toolkit as it
exists in this source tree (Motif 2.5): the libraries and programs that
make it up, the layers of the X Window System it is built on, the widget
class taxonomy, the organisation of the sources and the build, and the
engineering decisions that distinguish this tree from upstream Motif
2.3.8.  It is the map that the remaining chapters zoom into.

**Audience.** Readers who know C and have written at least one X11 or Xt
program.  Section 1.2 recalls the Xlib and Xt vocabulary that the rest of
the guide assumes.

**Conventions.** Paths are relative to the root of the source tree.
Function and type names are those of the code; where a line of code is
quoted, it is quoted verbatim from the file named next to it.  Links to
`.c` and `.h` files point into the tree, so they work in a checkout and
on GitHub.

---

## 1.1 What Motif is

Motif is a *widget toolkit* for the X Window System: a library of
reusable user-interface components ("widgets": buttons, text fields,
lists, menus, dialogs, layout containers) together with a look and feel
specification (the Motif Style Guide), a user-interface description
language (UIL), a window manager (mwm), and the conventions needed for
several Motif programs on one display to interoperate (keyboard
bindings, clipboard, drag and drop).

In this tree Motif is delivered as three libraries and four programs:

| Artifact | Source | Role |
|----------|--------|------|
| `libXm.so.5` | [`src/lib/Xm`](../../src/lib/Xm) | The widget library: every `Xm*` class, compound strings (`XmString`), render tables, the image cache, drag and drop, the clipboard, keyboard traversal, input methods, the `XmLog` facility. 207 C files, about 263 000 lines. |
| `libMrm.so.5` | [`src/lib/Mrm`](../../src/lib/Mrm) | The Motif Resource Manager: loads *UID* files (the compiled form of UIL) and creates widget hierarchies from them (`MrmOpenHierarchy`, `MrmFetchWidget`). |
| `libUil.so.5` | [`src/lib/Uil`](../../src/lib/Uil) | The callable UIL compiler (`Uil()`): a yacc grammar with 116 token declarations, a lexer, a semantic checker and a UID writer, about 13 800 lines. |
| `uil` | [`src/bin/uil`](../../src/bin/uil) | Command-line front end of `libUil`. |
| `mwm` | [`src/bin/mwm`](../../src/bin/mwm) | The Motif Window Manager: ICCCM-compliant, decorates windows in the Motif 3-D style, reads `_MOTIF_WM_HINTS` and `_MOTIF_WM_MENU`. |
| `xmbind` | [`src/bin/xmbind`](../../src/bin/xmbind) | Installs virtual key bindings on the root window (`_MOTIF_BINDINGS`), see §1.6. |
| `wml`, `wmluiltok` | [`src/bin/wml`](../../src/bin/wml) | Build-time generators: the *Widget Meta-Language* describes every widget class, resource and reason once (`motif.wml`), and `wml` generates the UIL compiler's keyword tables and grammar tokens from it ([README](../../src/bin/wml/README)). |
| `makestrs`, `mkcatdefs`, `mkmsgcat` | [`src/bin/utils`](../../src/bin/utils) | Build-time generators for the resource-name string tables and the message catalogs (§1.5, chapter 1.3). |

The relationships between these pieces are:

```
            ┌────────────┐   compiles   ┌──────────┐   loads    ┌──────────────┐
  hello.uil │   uil /    │ ───────────▶ │ hello.uid│ ─────────▶ │   libMrm     │
            │   libUil   │              └──────────┘            │ MrmFetchWidget
            └────────────┘                                      └──────┬───────┘
                   ▲  keyword tables                                   │ XtCreateWidget
                   │  generated from motif.wml                         ▼
            ┌────────────┐                                      ┌──────────────┐
            │    wml     │                                      │    libXm     │◀── application C code
            └────────────┘                                      └──────┬───────┘
                                                                       │
                                            ┌────────────┐             ▼
                                            │    mwm     │◀─ ICCCM ─▶ libXt ─▶ libX11 ─▶ X server
                                            └────────────┘   _MOTIF_WM_*  │
                                                                          ├─▶ libXft ─▶ fontconfig, FreeType
                                                                          ├─▶ libXpm (bundled), libpng, libjpeg, nanosvg (bundled)
                                                                          └─▶ libXp (optional printing)
```

## 1.2 Layering on the X Window System

Motif sits on two lower layers, and it is impossible to understand the
toolkit without knowing what each of them already provides.

**Xlib** ([libX11](https://www.x.org/releases/current/doc/libX11/libX11/libX11.html))
is the C binding of the X protocol.  It gives the toolkit windows,
graphics contexts (GCs), pixmaps, colormaps, fonts, properties, atoms,
selections and events.  Every drawing call in Motif ends in an Xlib
request, and the main performance question in Motif is *how many
requests, and how many of them are round trips* (requests that wait for
a reply: `XGetWindowAttributes`, `XQueryTree`, `XAllocColor`, `XSync`).
The benchmarks in `src/tests/bench` count exactly those two quantities
(chapter 6).

**Xt**, the X Toolkit Intrinsics
([libXt specification](https://www.x.org/releases/current/doc/libXt/intrinsics.html)),
is an object system written in C.  It defines:

- the *widget class* (a statically initialised structure of method
  pointers and resource tables, `WidgetClassRec`) and the *widget
  instance* (`WidgetRec`), with single inheritance by structure
  prefixing (chapter 1.1);
- the *resource* mechanism: named, typed, defaultable attributes that are
  set from the command line, resource files, `XtSetValues` and converted
  from strings by registered *type converters* (chapter 1.3);
- the *geometry management* protocol between a parent (composite) and its
  children: `query_geometry`, `geometry_manager`, `change_managed`,
  `resize`, and the `XtGeometryAlmost` negotiation;
- the *translation* mechanism: event sequences such as `<Key>osfUp` map
  to named *actions*, which are C functions in a class's action table;
- callbacks, timeouts, work procedures, the event loop
  (`XtAppMainLoop`), and since X11R6 optional thread locks
  (`XtAppLock`, `XtProcessLock`).

Motif adds, on top of Xt, three things that Xt does not have:

1. **A look and feel**: the 3-D shadows, highlight rectangles, the
   keyboard traversal model and the virtual key bindings (chapters 2
   and 3).
2. **Richer base classes**: `XmPrimitive` and `XmManager` carry the
   colours, shadows, traversal state and the Motif *class extension*
   records that every Motif widget inherits (chapters 1.1 and 1.2), and
   `XmGadget` provides windowless children (chapter 3).
3. **Shared infrastructure**: compound strings and render tables (fonts,
   colours, tabs, direction) that replace plain `char *` labels; an image
   and pixmap cache; the drag and drop and clipboard protocols; input
   method handling; the Mrm/UIL description language.

### 1.2.1 Why a toolkit *on top of* Xt rather than a replacement

Xt was the X Consortium's standard object layer, and by 1989 both of the
toolkits Motif was assembled from (HP's widgets and DEC's XUI) were Xt
toolkits.  Building on Xt gave Motif, for free, resource files, the
translation manager, geometry negotiation and compatibility with every
Xt program.  The cost is visible throughout this guide: Xt's class
record is fixed, so Motif had to invent *class extensions* and a
*wrapper* mechanism to add pre- and post-hooks to `initialize` and
`set_values` (chapter 1.2); Xt's `XtIsSubclass` walks the superclass
chain, so Motif added *fast subclassing* bit flags; Xt has no notion of
interfaces, so Motif 2.0 added *traits*.  Each of these is a
workaround for a limitation of a 1988 object model expressed in C, and
each is explained in its own section with its trade-offs.

## 1.3 The widget class taxonomy

Every Motif class descends from one of four roots.  The table lists the
classes in this tree with their direct superclass, as declared in the
first field of each class record (for example `(WidgetClass)&xmLabelClassRec`
in [`PushB.c`](../../src/lib/Xm/PushB.c)).

**Primitive widgets** (`Core` → `XmPrimitive`): leaf widgets with their
own X window.

| Class | Superclass | Source |
|-------|------------|--------|
| XmPrimitive | Core | [Primitive.c](../../src/lib/Xm/Primitive.c) |
| XmLabel | XmPrimitive | [Label.c](../../src/lib/Xm/Label.c) |
| XmPushButton, XmToggleButton, XmDrawnButton, XmCascadeButton | XmLabel | PushB.c, ToggleB.c, DrawnB.c, CascadeB.c |
| XmTearOffButton | XmPushButton | TearOffB.c |
| XmArrowButton, XmSeparator, XmScrollBar, XmSash, XmList, XmText, XmTextField, XmIconButton, XmI18List, XmTabBox | XmPrimitive | ArrowB.c, Separator.c, ScrollBar.c, Sash.c, List.c, Text.c, TextF.c, IconButton.c, I18List.c, TabBox.c |
| XmDataField | XmTextField | [DataF.c](../../src/lib/Xm/DataF.c) (chapter 3.1) |

**Gadgets** (`Object` → `RectObj` → `XmGadget`): windowless children that
a manager draws and dispatches events to (chapter 3, §3.4).

| Class | Superclass |
|-------|------------|
| XmGadget | RectObj |
| XmLabelGadget, XmArrowButtonGadget, XmSeparatorGadget, XmIconGadget | XmGadget |
| XmPushButtonGadget, XmToggleButtonGadget, XmCascadeButtonGadget | XmLabelGadget |

Each gadget class is accompanied by a *cache object* class
(`XmLabelGCacheObj`, ... , subclasses of `XmExtObject`) that holds the
part of the gadget's state that many gadgets share, so that a thousand
identical label gadgets store their fonts, colours and margins once
(chapter 6).

**Managers** (`Core` → `Composite` → `Constraint` → `XmManager`):
containers with their own window and *constraint* resources on their
children.

| Class | Superclass | Layout model |
|-------|------------|--------------|
| XmManager | Constraint | — |
| XmBulletinBoard | XmManager | absolute positions; base of the dialogs |
| XmForm, XmColumn, XmTabStack | XmBulletinBoard | attachments (chapter 3.2); labelled column; tabbed pages |
| XmMessageBox, XmSelectionBox | XmBulletinBoard | fixed dialog layouts via the GeoMatrix engine (chapter 3, §3.2.1) |
| XmCommand, XmFileSelectionBox | XmSelectionBox | command history; file browsing |
| XmRowColumn | XmManager | rows/columns; also every menu type |
| XmFrame, XmDrawingArea, XmScale, XmPanedWindow, XmPaned, XmScrolledWindow, XmNotebook, XmComboBox, XmSpinBox, XmContainer, XmButtonBox, XmIconBox, XmDropDown, XmColorSelector, XmMultiList, XmHierarchy | XmManager | one specific layout each; SpinBox is chapter 4, Container chapter 3.3 |
| XmMainWindow | XmScrolledWindow | menu bar, command and message areas |
| XmSimpleSpinBox | XmSpinBox | a SpinBox that creates its own TextField |
| XmOutline, XmTree | XmHierarchy | outline and tree views |
| XmFontSelector | XmPaned | font chooser |

**Shells** (`Shell` → `WMShell` → `VendorShell` ...): top-level windows.
Motif does not subclass `VendorShell`; it *replaces* its class record
(`VendorS.c`) so that every Xt shell in a Motif program gets Motif's
window-manager protocol handling, input method support and the
`XmNdefaultFontList`-style resources.  `XmDialogShell` (transient
dialogs), `XmMenuShell` (an `OverrideShell` for menus) and `XmGrabShell`
add to it.  The *shell extension objects* (`VendorSE.c`, `DialogSE.c`)
are `XmExtObject` subclasses that hold Motif's per-shell state outside
the Xt shell record, again because the Xt record cannot be extended.

Two further `Object` subclasses are not widgets at all: `XmDisplay` and
`XmScreen` are per-display and per-screen singletons that hold the drag
and drop protocol settings, the pixmap and GC caches of chapter 5 and
the default cursors; and `XmDragContext`, `XmDragIcon`, `XmDropSite`
objects model a drag in progress (chapter 6.2).

## 1.4 Source layout

```
src/lib/Xm          libXm: one .c per class, plus shared modules:
                      BaseClass.c  class extensions, wrappers, fast subclassing
                      Trait.c      the trait table
                      Draw.c, DrArrow.c, DrPoly.c, DrTog.c, DrHiDash.c  drawing primitives
                      GeoUtils.c, GMUtils.c  geometry negotiation and the GeoMatrix engine
                      XmString.c, XmStringFunc.c, XmRenderT.c, XmFontList.c  compound strings
                      ImageCache.c, PixConv.c, ReadImage.c, Png.c, Jpeg.c, Svg.c, Xpm*.c  images
                      Hash.c       the hash table used by the caches
                      Traversal.c, TravAct.c, FocusAct.c, VirtKeys.c  keyboard traversal and bindings
                      DragC.c, DragBS.c, DragICC.c, DropSMgr.c, DropTrans.c  drag and drop
                      CutPaste.c, Transfer.c, Dest.c  clipboard and the uniform transfer model
                      XmIm.c       input methods
                      Xm.c, XmI.h  shared helpers, locks, checked allocation
src/lib/Xm/*P.h     the "private" headers: instance and class records, installed for subclassing
src/lib/Xm/*I.h     internal headers, not installed
src/lib/Xm/*T.h     trait headers (NavigatorT.h, AccTextT.h, ...)
src/lib/Mrm         libMrm
src/lib/Uil         libUil (Uil.y is the grammar)
src/bin/{uil,mwm,xmbind,wml,utils}
src/examples        example programs and their UIL files
src/tests           libcheck suites, fuzzers, benchmarks, the layout A/B harness
include             config.h.in and stub CDE headers
data                virtual key bindings, bitmaps, pkg-config templates
localized           German, Spanish, French, Italian and Japanese message catalogs
doc/man             3 man1, 831 man3, 1 man4 and 3 man5 pages
tools               CMake helpers, presets, CI scripts
```

The `*P.h`/`*I.h` split is the toolkit's visibility model.  A `*P.h`
header is part of the API ([doc/abi-policy.md](../abi-policy.md)): it
declares the instance record that a subclass compiles against, so its
layout is frozen for the life of a SONAME.  An `*I.h` header is private
to the library and may change at will.  The [`XmI.h`](../../src/lib/Xm/XmI.h)
header is the most important internal one: it defines the lock macros
(§1.7), the checked allocators and the `ASSIGN_MIN`/`ASSIGN_MAX` helpers
used throughout.

## 1.5 Generated code

Three generators run at build time, and knowing what they produce avoids
confusion when reading the sources:

1. **makestrs** reads [`xmstring.list.in`](../../src/lib/Xm/xmstring.list.in)
   (1 705 names) and writes `XmStrDefs.h`, in which every resource
   name, class and representation type (`XmNlabelString`,
   `XmCLabelString`, `XmRXmString`, ...) is a macro expanding to an
   offset into one exported character array, `_XmStrings`.  The scheme
   and its ABI consequences are the subject of chapter 1.3.
2. **wml** reads [`motif.wml`](../../src/bin/wml/motif.wml) and
   generates the UIL compiler's keyword tables, class/argument/reason
   relationships and the token definitions of `Uil.y`.
3. **mkcatdefs** turns the symbolic message catalogs
   (`src/lib/Xm/Xm.msg`, ...) into numeric catalogs and the `XmMsgCatI.h`
   header; the build also checks every translation against the English
   catalog ([doc/LOCALIZATION.md](../LOCALIZATION.md)).

Because the build runs these programs, cross-compiling needs a native
build first (`MOTIF_HOST_TOOLS`, [doc/BUILD.md](../BUILD.md)).

## 1.6 Cross-program conventions

A Motif program is rarely alone on a display, and several of the
toolkit's mechanisms exist to make independently written Motif programs
agree with each other:

- **Virtual keysyms.** Translations are written against *virtual* keys
  (`osfUp`, `osfActivate`, `osfCancel`, 47 of them in
  [`VirtKeys.c`](../../src/lib/Xm/VirtKeys.c)) rather than physical
  keysyms.  The binding from physical to virtual keys is read from the
  `_MOTIF_BINDINGS` property of the root window, which `xmbind` installs
  from `~/.motifbind` or the vendor-specific files in
  `data/bindings`, with a compiled-in fallback.  This is how `osfBackSpace`
  can be `Delete` on one keyboard and `BackSpace` on another without
  changing any application.
- **Drag and drop** negotiates a protocol style per drag between the
  initiator and the receiver, and shares its atom and target tables
  through properties on a hidden window so that clients do not have to
  intern the same atoms repeatedly (chapter 6.2).  XDND is supported next
  to the Motif protocol.
- **The clipboard** (`CutPaste.c`) keeps its records as `_MOTIF_CLIP_*`
  properties on the root window, so that a copy survives the exit of the
  program that made it.
- **mwm** reads `_MOTIF_WM_HINTS` (decorations and functions per window)
  and `_MOTIF_WM_MENU` (client-supplied menu entries) from client windows.

Every one of these reads data that *another client wrote*, which is why
the 2026 security review ([SECURITY.md](../../SECURITY.md)) treats all of
them as untrusted input (§1.8).

## 1.7 Concurrency model

Xt is single-threaded by design; since X11R6 it provides optional locks
that an application enables with `XtToolkitThreadInitialize()`.  Motif
follows the same model.  Every public entry point brackets its work with
`_XmAppLock(app)` (the application-context lock) and the modules that
keep process-wide static state (the segment buffer of `Draw.c`, the
caches, the trait table) use `_XmProcessLock()`.  In this tree these
macros are

```c
/* src/lib/Xm/XmI.h */
#  define _XmIsThreadInitialized() (_XtProcessLock)
#  define _XmAppLock(app) \
      do { \
        if (_XmIsThreadInitialized()) \
          XtAppLock(app); \
      } while (0)
```

that is, a load of libXt's `_XtProcessLock` function pointer and a
branch.  `XtAppLock` itself is a no-op until threads are initialised,
but calling it still costs a call through the PLT; testing the pointer
first keeps the thousands of lock sites on the hot paths at the cost of
one predictable branch each.  The trade-off is a dependency on a libXt
internal (`_XtProcessLock`), noted in the header as something to remove
when Xt offers an API for it.

A consequence that recurs in later chapters: *correctness* of the
static caches under threads depends on every access being inside a
process lock, and several of the 2026 fixes were lock imbalances in the
image cache.

## 1.8 What is different in Motif 2.5

This tree continues upstream Motif 2.3.8 (December 2017).  The
[CHANGELOG](../../CHANGELOG.md) is the authoritative list; the
architectural consequences are summarised here because they explain
code the reader will meet.

**ABI.** The libraries carry SONAME 5 and export only their API through
GNU ld version scripts ([`libXm.map`](../../src/lib/Xm/libXm.map));
libXm went from 3 224 to 1 742 exported symbols
([doc/abi-policy.md](../abi-policy.md)).  Two layout changes forced the
new SONAME: the `OM22_COMPATIBILITY` member `tool_tip_string` left
`XmPrimitivePart` and `XmGadgetPart`, moving every subclass part by
eight bytes, and the `_XmStrings` table was re-ordered.  Chapter 1.3
explains why re-ordering a string table is an ABI break.

**Security.** A review of every parser of data from other clients or
from files (drag and drop, clipboard, selections, XmString byte streams,
render tables, XPM/PNG/JPEG/SVG, UID files, UIL, mwm properties) fixed
out-of-bounds accesses, use-after-free, integer overflows in size
computations and fixed-buffer overflows.  Two helpers introduced for it
appear throughout the code: `_XmMallocArray(num, size)` and
`_XmReallocArray`, which check the multiplication before calling
`XtMalloc` (which takes a 32-bit `Cardinal`), and the
[`XmBannedI.h`](../../src/lib/XmBannedI.h) header, force-included into
every library source with `-include`, which declares `sprintf`,
`strcpy` and `strcat` with the `deprecated` attribute while the build
passes `-Werror=deprecated-declarations`: an unbounded copy is a
compile error.

**Performance.** `XmForm` sorts and sizes its children in O(n log n)
instead of O(n²) (chapter 3.2); `XmContainer` appends children in O(1)
(chapter 3.3); `XmList` scrolls by copying the rows that stay visible
(chapter 6.1); Xft fonts, colours and draws are cached per display
(chapter 5); the trait table is an open-addressing hash table (chapter
5.1); the Text gap buffer grows geometrically (chapter 6.1).  Every one
of these is measured by a case of `xmbench`, and the layout changes were
validated against the old code with the A/B harness in
[`src/tests/ab`](../../src/tests/ab/README.md), which drives both
libraries through the same pseudo-random configurations and compares
geometry, callbacks and window pixels.

**Code base.** C17 with prototypes throughout; HP-UX, AIX, VMS and SVR4
code removed; `XmDataField` is a 570-line subclass of `XmTextField`
instead of an 8 683-line copy (chapter 3.1).

**Build and tests.** CMake only; a libcheck suite, fuzzers, a visual
regression test and ABI export tests under CTest; CI on glibc and musl,
32-bit, big-endian s390x and FreeBSD ([README](../../README.md#platforms)).

## 1.9 How to read the rest of this guide

| Chapter | Question it answers |
|---------|---------------------|
| [1.1 The Xt object model and class records](01-01-xt-object-model-and-class-records.md) | How is a widget class declared, how do methods chain and inherit, what does a widget record look like in memory? |
| [1.2 Base class extensions, wrappers and traits](01-02-base-class-extensions-and-traits.md) | How does Motif add hooks, fast type tests and interfaces to an object model that has none? |
| [1.3 Resources, string tables and converters](01-03-resources-string-tables-and-converters.md) | How are resources declared, defaulted, converted and why are resource names offsets into a table? |
| [2 Drawing and rendering](02-drawing-and-rendering-system.md) | How are shadows, highlights and separators drawn, in how many requests, and why segments? |
| [3 Widget system architecture](03-widget-system-architecture.md) | Primitive vs Manager vs Gadget, geometry management, event dispatch, keyboard traversal. |
| [3.1 DataField](03-01-case-study-datafield-subclassing.md), [3.2 Form](03-02-case-study-form-layout.md), [3.3 Container](03-03-case-study-container-layout.md) | Three case studies: subclassing, a constraint solver, a tree and grid layout. |
| [4 SpinBox walk-through](04-practical-widget-implementations.md) | One complete widget, method by method, from the real source. |
| [5 Graphics pipeline](05-graphics-system-rendering-pipeline.md), [5.1 Hash tables and caches](05-01-hash-tables-and-caches.md) | Images, pixmaps, scaling, SVG, Xft, GCs, and the data structures behind the caches. |
| [6 Implementation deep dive](06-actual-implementations-deep-dive.md), [6.1 Text and List](06-01-text-and-list-data-structures.md), [6.2 Drag and drop](06-02-drag-and-drop-and-interclient-protocols.md) | Compound strings, locking, memory safety, the gap buffer, list scrolling, the drag protocols. |
| [0 History](00-history-of-motif-development.md) | Where all of this came from, 1984 to today. |

## References

- X Toolkit Intrinsics, C Language Interface (X11R7.7):
  <https://www.x.org/releases/current/doc/libXt/intrinsics.html>
- Xlib, C Language X Interface:
  <https://www.x.org/releases/current/doc/libX11/libX11/libX11.html>
- Inter-Client Communication Conventions Manual (ICCCM):
  <https://www.x.org/releases/current/doc/xorg-docs/icccm/icccm.html>
- The Motif manual pages in [`doc/man`](../man) (`man XmPrimitive`,
  `man XmManager`, `man VirtualBindings`, `man mwm`).
- [README](../../README.md), [CHANGELOG](../../CHANGELOG.md),
  [SECURITY.md](../../SECURITY.md), [doc/abi-policy.md](../abi-policy.md),
  [doc/BUILD.md](../BUILD.md).
