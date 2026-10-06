# 6. Implementation Deep Dive: Compound Strings, Locking, Memory Safety and the ABI

**Scope.** The remaining chapters go below the widget level into the
library's shared implementation: the representation of compound
strings (`XmString`), the threading discipline and its cost, the
memory-safety rules introduced by the 2026 review and the helpers that
enforce them, and the mechanics of the ABI promise (symbol
versioning, export lists, the measurements behind them).  Chapter 6.1
covers the two data structures with the most interesting algorithms,
the Text gap buffer and the List, and chapter 6.2 the drag and drop
and other inter-client protocols.

---

## 6.1 `XmString`: the compound string

A Motif label is not a `char *`.  It is an `XmString`, a *compound
string*: a sequence of segments, each with text in some encoding, a
*tag* that selects a rendition (font, colour) from the widget's render
table, a direction (left-to-right or right-to-left), optional tabs
before it, and line breaks between them.  The abstraction exists so
that the same string can be drawn with any render table, so that
mixed-script text (Latin and Hebrew, Latin and Japanese) can carry
direction changes and font changes inline, and so that strings can
travel between clients as a byte stream (the clipboard target
`COMPOUND_TEXT`/`_MOTIF_COMPOUND_STRING`).

### 6.1.1 Representation

The internal layout, from [`XmStringI.h`](../../src/lib/Xm/XmStringI.h),
is designed around one observation: almost every compound string in a
real program is one short segment of text in the default font.

```c
/*  XmStringOpt is an optimized string containing text of less than
  256 bytes with an associated string direction and up to three
  implicit tabs.  The text is stored immediately after the header
  within the string. */
typedef struct __XmStringOptHeader {
  unsigned int type : 2;                     /* XmSTRING_OPTIMIZED */
  unsigned int text_type : 2;                /* MB, WC, locale or charset text.*/
  unsigned int tag_index : TAG_INDEX_BITS;   /* index into charset cache */
  unsigned int rend_begin : 1;
  unsigned char byte_count;                  /* size of text in this seg.*/
  unsigned int rend_end : 1;
  unsigned int rend_index : REND_INDEX_BITS; /* index in tag cache */
  unsigned int str_dir : 2;
  unsigned int flipped : 1;
  unsigned int tabs : 2;                     /* number of tabs preceding the text */
  unsigned int refcount : 6;
} _XmStringOptHeader;

typedef struct __XmStringOpt {
  _XmStringOptHeader header;
  char text[TEXT_BYTES_IN_STRUCT];
} _XmStringOptRec, *_XmStringOpt;
```

An *optimized* string is one allocation: a bit-packed header of a few
bytes followed by the text inline.  The tag is not stored as a string
but as an index into a process-wide cache of tags (`tag_index`), so
that comparing tags is an integer compare and the header stays small.
A `refcount` of six bits lets `XmStringCopy` share the allocation up
to 63 times before it must really copy.

Everything that does not fit (more than 255 bytes, more than three
tabs, several segments, several lines, rendition changes) is a
*multiple-entry* string:

```c
typedef struct __XmStringMultiHeader {
  unsigned int type : 2;          /* XmSTRING_MULTIPLE_ENTRY */
  unsigned int implicit_line : 1; /* 1 => linefeed at end */
  unsigned int entry_count : 21;
  unsigned char refcount;
} _XmStringMultiHeader;

typedef struct __XmStringMulti {
  _XmStringMultiHeader header;
  _XmStringEntry *entry; /* pointer to array of pointers to entries */
} _XmStringMultiRec, *_XmStringMulti;
```

whose entries are *segments* of three kinds: optimized segments (the
same compact form, as a member of a larger string), *array* segments
(a line: an array of segments), and *unoptimized* segments, the fully
general form with a direction push/pop, counts of rendition begins
and ends, and text held by pointer:

```c
typedef struct __XmStringUnoptSegHdrRec {
  unsigned int type : 2;            /* XmSTRING_ENTRY_UNOPTIMIZED */
  unsigned int soft_line_break : 1;
  unsigned int permanent : 1;       /* 0 => Pointer data can be freed */
  unsigned int pop_after : 1;       /* whether a pop follows the text */
  unsigned int str_dir : 2;
  unsigned int flipped : 1;
  XmDirection push_before;          /* if NULL => no push */
  unsigned char tabs_before;
  XmTextType text_type;
} _XmStringUnoptSegHdrRec;
```

The whole thing is a *tagged union*: the first two bits of any
`XmString` or entry say which record follows (`type`), and every
function in `XmString.c` switches on them.  The union
`_XmStringRec` of the three headers is what the public opaque
`XmString` points to.  A `_XmStringNREntry` is the same type as
`_XmStringEntry` (made so in this tree, to remove a strict-aliasing
miscompile that crashed the UIL compiler).

### 6.1.2 Operations and their cost

| Operation | Optimized string | Multi-entry string |
|-----------|------------------|--------------------|
| `XmStringCreateLocalized` / `XmStringCreate` | one allocation, O(n) copy | — |
| `XmStringCopy` | refcount++ (O(1)) until 63 | refcount++ |
| `XmStringConcat` | may stay optimized if the result fits | O(entries) new array |
| `XmStringCompare` | memcmp | segment walk |
| `XmStringExtent`, `XmStringDraw` | one font lookup, one text extents call | per segment: tag → rendition lookup (`_XmRenderTableFindRendition`, memoised per string in a *rendering cache*), direction handling, tab stops |
| `XmStringToXmStringTable`, `XmStringGetNextTriple` | — | iteration by *context* |
| byte-stream in/out (`XmCvtXmStringToByteStream`) | header + text | recursive encoding with length prefixes |

The *scanning* and *rendering caches* (`_XmSCANNING_CACHE`,
`_XmRENDERING_CACHE` in the header) attach per-string memoised state
(the rendition resolved for each segment against a given render
table) so that redrawing a label does not re-resolve its tags.  The
[TODO](../../TODO.md) lists an extent cache and a builder API as
future work; `xmbench xmstring-extent-multi` ("3 lines with a tab") is
the case that would show it.

### 6.1.3 Layout and direction

Drawing walks the segments once per line, placing each at the current
x, advancing by its extent, honouring tabs against the rendition's
`XmTabList`, and reversing the order of runs inside a right-to-left
push.  Direction changes are a stack: an unoptimized segment may
`push_before` a direction and `pop_after` it.  The byte-stream form of
a string is untrusted input when it arrives by paste or drop, and the
"Unreleased" entry of the [CHANGELOG](../../CHANGELOG.md) records what
happens when the stack is unbalanced: "a string with layout direction
pushes that are not popped (or popped on a later line) could make
`XmStringExtent` and `XmStringDraw` loop forever or read past the
string's segments".  The fix makes the layout code tolerate pops
without pushes and lay out "unbalanced layout pushes without a cycle
of segments", that is, it bounds the recursion by the data rather than
trusting the data to terminate it.  The lesson generalises to every
parser in the library (§6.3).

## 6.2 Locking

Motif 2.1 made the library thread-safe in the Xt sense: an application
that calls `XtToolkitThreadInitialize()` may use several threads, and
the toolkit serialises them with Xt's two locks, the *application
context* lock (`XtAppLock`, one per `XtAppContext`, recursive) and the
*process* lock (`XtProcessLock`, one per process, recursive).  The
rules, visible in every file of this guide:

1. Every public entry point takes the application lock of the widget's
   or display's context for its whole duration (`_XmAppLock(app)`; the
   `_XmWidgetToAppContext(w)` macro declares and fetches it).  Xt's
   dispatcher already holds it while calling actions, callbacks and
   class methods, and the locks are recursive, so this costs nothing
   on the common path and protects the direct-call path.
2. Every access to *process-wide* static state takes the process lock:
   the caches (chapter 5), the trait table, the static drawing buffers
   (chapter 2), the string tag cache, the wrapper bookkeeping in class
   records (chapter 1.2).  The process lock may be taken while holding
   an application lock, never the reverse (Xt's lock ordering).
3. A lock must be released on every return path.  The `GetXftColor`
   function in chapter 5 is written as "lock, look up, unlock, query
   the server, lock, insert, unlock" precisely so that no round trip
   happens under the lock.

In this tree the macros compile to a pointer test (chapter 1 §1.7):

```c
#  define _XmProcessLock() \
      do { \
        if (_XmIsThreadInitialized()) \
          XtProcessLock(); \
      } while (0)
```

**Cost.**  In a single-threaded program each lock site is a load of
`_XtProcessLock` and a well-predicted branch; the `xmbench -t` flag
initialises threads first so that the real locks are measured, and the
difference is the cost of Xt's recursive mutex per entry point.  The
[TODO](../../TODO.md) records that TSan has never been run and that
"a full audit of the mutable statics with a two-context TSan test"
remains to be done; the `XmLog` facility
([doc/LOGGING_CONFIGURATION.md](../LOGGING_CONFIGURATION.md)) does no
locking at all and says so.

**Assessment.**  Coarse recursive locks make a 1989 code base safe to
call from threads without redesign, at the price of no parallelism
inside the toolkit: two threads drawing into two windows serialise on
the application lock.  That is the Xt model, and Motif cannot do
better than Xt.  What Motif can do, and this tree did, is keep the
uncontended cost near zero and fix the lock imbalances that make
"thread-safe" true rather than nominal.

## 6.3 Memory safety

The 2026 review ([SECURITY.md](../../SECURITY.md)) started from the
observation that a Motif program parses data written by *other X
clients* (properties, selections, client messages) and by *files* the
user did not write, and that a bug in any such parser is a remote
crash of every Motif program on the display.  Four mechanisms now
apply library-wide; each shows up in code quoted earlier.

**Checked array allocation.**

```c
/* src/lib/Xm/Xm.c */
char *_XmMallocArray(size_t num, size_t size)
{
  if (size != 0 && num > MAX_ALLOC_SIZE / size) {
    ArrayAllocError("malloc");
    return NULL;
  }
  return XtMalloc((Cardinal)(num * size));
}
```

`XtMalloc` takes a `Cardinal` (32-bit), so `XtMalloc(n * sizeof(T))`
with a large or negative `n` wraps or truncates and returns a buffer
far smaller than the caller then writes.  `_XmMallocArray` and
`_XmReallocArray` check the product first (and a negative `int` count
converts to a huge `size_t`, so it is caught too).  Every array
allocation in chapters 2 to 5 uses them;
[CONTRIBUTING.md](../../CONTRIBUTING.md) makes it a rule.

**Banned functions.**

```c
/* src/lib/XmBannedI.h, force-included into every library source */
#    define _XM_BANNED(instead) __attribute__((__deprecated__("unbounded, use " instead)))
extern int(sprintf)(char *, const char *, ...) _XM_BANNED("snprintf");
extern int(vsprintf)(char *, const char *, va_list) _XM_BANNED("vsnprintf");
extern char *(strcpy)(char *, const char *) _XM_BANNED("memcpy, snprintf or XtNewString");
extern char *(strcat)(char *, const char *) _XM_BANNED("memcpy, snprintf or _XmConcatStrings");
```

with `-Werror=deprecated-declarations` in the compiler flags: a call
to any of the four is a compile error, with the replacement named in
the message.  The parenthesised names defeat glibc's function-like
`_FORTIFY_SOURCE` macros.  Even `XtNewString`, which Xt defines as a
macro around `strcpy`, is replaced by a bounded inline
(`_XmBannedNewString`) that also refuses a string whose length does not
fit a `Cardinal`.  `_XmConcatStrings(list, count)` is the bounded
replacement for `strcat` chains; the DataField translation table of
chapter 3.1 uses `snprintf` into a computed size.

**Checked property reads.**  `_XmGetWindowPropertyChecked` wraps
`XGetWindowProperty` and validates the type, format and length against
what the caller expects, so that a drag-and-drop or clipboard record
of the wrong size is rejected rather than indexed (chapter 6.2).

**Bounded recursion and loops.**  Drop-site trees, UID widget trees,
B-tree indexes, XPM pixel streams and XmString direction stacks are
walked with explicit bounds derived from the buffer, never from counts
inside the data.  The fuzzers in `src/tests/fuzz` (libFuzzer targets
for XPM, PNG, JPEG, SVG, UID, UIL, XmString byte streams and the
drag protocol messages) are the regression test for this class; CTest
runs each over its seed corpus and over the reproducers of known open
bugs, which are expected to fail until fixed.

## 6.4 The ABI, mechanically

[doc/abi-policy.md](../abi-policy.md) is the normative document; the
mechanics that a reader of the sources meets:

- **Version scripts.**  Each library is linked with a GNU ld version
  script ([`libXm.map`](../../src/lib/Xm/libXm.map)) that lists the
  exported symbols under one version node (`XM_2.5`) and makes
  everything else local.  libXm exports 1 742 symbols instead of 3 224;
  the 1 417 `_Xm*` symbols that the old `.elist` already called
  internal, the vendored nanosvg, and 65 newer internals are gone.
  A new public function goes into a *new* node (`XM_2.6 { ... } XM_2.5;`),
  never into an old one, so that a program linked against 2.6 fails to
  load on 2.5 with a clear message instead of an undefined symbol at
  call time.
- **Tests.**  `abi.exports.Xm`, `.Mrm`, `.Uil` (`ctest -L ABI`) parse
  the installed headers and fail when a declared function is missing
  from the map, and `tools/dev/env/ci/abi-check.sh` runs `abidiff`
  (libabigail) against a previous build to catch a changed signature
  or structure.
- **Why exports matter for performance.**  With fewer exported
  symbols, calls inside the library bind at link time; with
  `-fno-semantic-interposition` the compiler may inline them; with
  `-fno-plt` and `-z now` imported calls go through the GOT without a
  PLT stub.  The measurements in `abi-policy.md` show the dynamic
  symbol table shrinking from 191 KB to 114 KB and the loader's symbol
  lookups at startup from 4 688 to 4 380, while wall time stays
  dominated by X round trips.
- **What is still ABI.**  The instance and class records in every
  `*P.h` (chapter 1.1), the order of `_XmStrings` (chapter 1.3), the
  fast-subclass bit numbers (chapter 1.2), the trait record layouts,
  the enumeration values in `Xm.h`, and the `_Xm*` functions that
  CDE's sources declare by hand.  The TODO's note that List and
  Container data-structure work is "blocked by installed struct
  layouts" is this constraint in practice.

## 6.5 Reading the rest

- [6.1 Text and List data structures](06-01-text-and-list-data-structures.md):
  the gap buffer with geometric growth and hysteresis; the List's
  scroll-by-copy and its drawn-state generation counter.
- [6.2 Drag and drop and inter-client protocols](06-02-drag-and-drop-and-interclient-protocols.md):
  the protocol-style negotiation table, the shared atom and target
  tables, the drop-site database, XDND, the clipboard, and why all of
  it is untrusted input.

## References

- [`src/lib/Xm/XmString.c`](../../src/lib/Xm/XmString.c),
  [`XmStringI.h`](../../src/lib/Xm/XmStringI.h),
  [`XmStringFunc.c`](../../src/lib/Xm/XmStringFunc.c),
  [`XmRenderT.c`](../../src/lib/Xm/XmRenderT.c)
- [`src/lib/Xm/XmI.h`](../../src/lib/Xm/XmI.h), [`Xm.c`](../../src/lib/Xm/Xm.c),
  [`src/lib/XmBannedI.h`](../../src/lib/XmBannedI.h)
- [`src/lib/Xm/libXm.map`](../../src/lib/Xm/libXm.map),
  [`src/tests/fuzz`](../../src/tests/fuzz)
- Xt specification, chapter 7.1 "Multi-threaded Programming":
  <https://www.x.org/releases/current/doc/libXt/intrinsics.html>
- GNU ld version scripts: <https://sourceware.org/binutils/docs/ld/VERSION.html>;
  libabigail: <https://sourceware.org/libabigail/>;
  GCC `-fno-semantic-interposition`: <https://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html>
- libFuzzer: <https://llvm.org/docs/LibFuzzer.html>
- Manual pages `XmString(3)`, `XmStringCreate(3)`, `XmStringDraw(3)`,
  `XmStringExtent(3)`, `XmRenderTable(3)` in [`doc/man/man3`](../man/man3)
- [SECURITY.md](../../SECURITY.md), [doc/abi-policy.md](../abi-policy.md),
  [CONTRIBUTING.md](../../CONTRIBUTING.md), [TODO.md](../../TODO.md)
