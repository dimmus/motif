# Profiling

This document says how Motif was profiled, what the profiles show for
five workloads, and how much a small program costs to start compared
with earlier releases.  The scripts are in `tools/dev/profile`; the
benchmarks they drive are those of `src/tests/bench` (`xmbench`).

The numbers below were taken in October 2026 on a shared virtual
machine (2 CPUs, an Intel Pentium Gold G6405), in an Ubuntu 24.04
container with GCC 13, against Xvfb.  Other jobs ran on the machine at
the same time, so the times are noisy: the instruction counts
(callgrind), allocation counts (heaptrack, libxmbench_preload) and round
trip counts are exact and repeatable, the times are medians of
interleaved runs.  The virtual machine has no hardware performance
counters (`perf stat -e instructions` says `<not supported>`), so `perf`
samples the CPU clock (`task-clock`) and the instruction counts come
from callgrind.

## Contents

- [How to reproduce](#how-to-reproduce)
- [Counting round trips](#counting-round-trips)
- [Startup: hello_motif](#startup-hello_motif)
- [xmbench](#xmbench)
- [XmText: 10 MB appended](#xmtext-10-mb-appended)
- [XmList: 100k items](#xmlist-100k-items)
- [mwm managing 200 clients](#mwm-managing-200-clients)
- [What is left](#what-is-left)

## How to reproduce

`perf`, valgrind and heaptrack run in a container, so that the host
needs only podman (or docker):

| File (`tools/dev/profile/`) | What it is |
|---|---|
| `Containerfile` | Ubuntu 24.04 with perf, valgrind, heaptrack, xtrace, Xvfb, the build dependencies of this tree and of the autotools releases, and the debug symbols of the X libraries |
| `mkimage.sh` | builds that image where `podman build` cannot (rootless podman with vfs storage has no overlay mount for the build context) |
| `build.sh SOURCE PREFIX` | builds a tree or a release tarball (2.3.x, 2.4.x with autotools, this tree with CMake) with `-O2 -g -fno-omit-frame-pointer`, installs it in PREFIX with `hello_motif` and `xclients` |
| `hello_motif.c` | the startup probe: an application shell with a RowColumn, a Label, a PushButton and a Text, realized and drawn, then exit |
| `xclients.c` | an Xlib client that maps N windows, waits until the window manager has reparented them, changes their titles, and withdraws them |
| `profile.sh PREFIX OUT [SCENARIO...]` | the scenarios below, under perf, callgrind, heaptrack, xtrace and libxmbench_preload |
| `startup.sh PREFIX...` | the startup table |
| `mwm.sh PREFIX...` | mwm and xclients, timed, for several versions |
| `rtrips.py`, `stacks.py`, `xtrace.awk` | summaries: where the round trips come from, the top allocation sites of a heaptrack profile, an xtrace log |

```sh
# The image (or: podman build -t motif-profile -f tools/dev/profile/Containerfile .)
tools/dev/profile/mkimage.sh motif-profile

# A container with this tree read-only in /src and a scratch directory
# for the tarballs and the results in /prof.  perf needs perf_event_open,
# which the default seccomp profile refuses; nothing needs the network.
mkdir -p ../prof/src
git archive --prefix=motif-master/ -o ../prof/src/motif-master.tar.gz master
git archive --prefix=motif-2.4.1/ -o ../prof/src/motif-2.4.1.tar.gz 2.4.1
curl -L -o ../prof/src/motif-2.3.8.tar.gz \
  https://sourceforge.net/projects/motif/files/Motif%202.3.8%20Source%20Code/motif-2.3.8.tar.gz/download
podman run -d --name motif-prof --network=none --security-opt seccomp=unconfined \
    -v "$PWD:/src:ro" -v "$PWD/../prof:/prof" motif-profile sleep infinity

# Build the versions to compare (in the container's /build and /opt)
podman exec motif-prof sh /src/tools/dev/profile/build.sh /src /opt/branch
podman exec motif-prof sh /src/tools/dev/profile/build.sh /prof/src/motif-master.tar.gz /opt/master
podman exec motif-prof sh /src/tools/dev/profile/build.sh /prof/src/motif-2.4.1.tar.gz /opt/2.4.1
podman exec motif-prof sh /src/tools/dev/profile/build.sh /prof/src/motif-2.3.8.tar.gz /opt/2.3.8

# Profiles, the startup table, mwm.  The scripts count round trips and
# allocations with the first prefix's libxmbench_preload.so; PRELOAD
# names another one (builds before this document have no exit report).
export PRELOAD=/opt/branch/bin/libxmbench_preload.so
podman exec -e PRELOAD motif-prof sh /src/tools/dev/profile/profile.sh /opt/branch /prof/out/branch
podman exec -e PRELOAD motif-prof sh /src/tools/dev/profile/startup.sh -r 40 /opt/2.3.8 /opt/2.4.1 /opt/master /opt/branch
podman exec -e PRELOAD motif-prof sh /src/tools/dev/profile/mwm.sh -r 7 /opt/master /opt/branch

podman rm -f motif-prof
```

`profile.sh` writes, per scenario, `perf.top` (functions by self time),
`callgrind.top` (by self and by inclusive instruction count),
`heaptrack.top` (totals and allocation sites), `xtrace.log.summary`
(requests, replies, events, replies by request) and, for startup and
mwm, `preload.txt` (round trips and mallocs) and `rtrips.txt` (where the
round trips come from).  The raw `perf.data`, `callgrind.out` and
heaptrack files stay next to them for `perf report`,
`callgrind_annotate` or kcachegrind and `heaptrack_gui`.  The scenarios:

| Scenario | Workload |
|---|---|
| `startup` | `hello_motif`, one start |
| `xmbench` | every `xmbench` case, one perf profile each |
| `text` | `xmbench text-append`: 10 MB appended to an XmText, 1 KB at a time |
| `list` | `xmbench list-add`: 100,000 items appended to an XmList |
| `mwm` | mwm managing 200 clients: `xclients -n 200 -t 2000` maps 200 windows, changes their titles 2000 times, withdraws them |

## Counting round trips

A round trip is a wait for the server: Xlib sends its requests and
blocks for the reply, which costs at least the latency of the
connection, whatever the size of the request.  They are counted in two
ways:

- `libxmbench_preload.so` (`src/tests/bench/preload.c`), which
  `xmbench` preloads into itself, and the scripts into `hello_motif` and
  mwm with `XMBENCH_REPORT=1` (the counts are printed at exit).  It
  counts the waits for a reply in libxcb (`xcb_wait_for_reply64`, which
  Xlib's `_XReply` calls) that follow new requests (`xcb_writev`): the
  further waits that `_XReply` makes for the replies of earlier requests
  with asynchronous handlers (`XInternAtoms` asks for all its atoms at
  once) are part of the same round trip.  This gives the number of
  `_XReply` calls, but works where libX11 is linked with
  `-Bsymbolic-functions` (Debian, Ubuntu), which binds libX11's own
  calls to `_XReply` inside it, out of reach of `LD_PRELOAD`.  With
  `XMBENCH_REPLY_BACKTRACE=1` every round trip prints a backtrace, and
  `rtrips.py` groups them by caller.
- xtrace, a proxy between the client and the server that logs the
  protocol: `xtrace.awk` counts requests, replies, events and errors.
  It shows what the round trips ask for, but replies are not round
  trips (one `XInternAtoms` of 60 atoms is 60 replies in one wait).

## Startup: hello_motif

`hello_motif` started 40 times for each version, round robin, on one
Xvfb that keeps running (`-noreset`), so that the Motif drag window and
its tables exist, as in a running session; medians:

| | 2.3.8 | 2.4.1 | master | this branch |
|---|---:|---:|---:|---:|
| Wall time (ms) | 32.9 | 33.1 | 36.4 | 26.1 |
| CPU time (ms, perf stat task-clock) | 14.5 | 15.5 | 15.2 | 11.9 |
| Page faults | 415 | 407 | 415 | 416 |
| Dynamic loader, total (M cycles, `LD_DEBUG=statistics`) | 1.42 | 1.27 | 1.54 | 1.46 |
| of which relocation (M cycles) | 0.37 | 0.33 | 0.42 | 0.45 |
| Symbol lookups (final number of relocations) | 2358 | 2343 | 2836 | 2836 |
| Lookups from the cache | 7673 | 7671 | 7542 | 7542 |
| Relative relocations | 8000 | 7862 | 7966 | 7966 |
| Instructions (callgrind) | 15.10 M | 15.04 M | 15.45 M | 15.32 M |
| of which in ld.so | 2.11 M | 2.05 M | 2.42 M | 2.42 M |
| X requests (xtrace) | 253 | 256 | 257 | 220 |
| Round trips | 80 | 81 | 80 | 43 |
| Calls to malloc/calloc/realloc | 6767 | 6775 | 6726 | 6581 |

2.3.8 is the upstream tarball, 2.4.1 the tag of this tree before the
CMake build, master the tree this branch starts from; all are built
with the same compiler and `-O2 -g -fno-omit-frame-pointer`, master and
the branch with their CMake RelWithDebInfo defaults on top
(`-fno-semantic-interposition`, `-fno-plt`, `-z now`, version scripts).
On the host (Arch Linux, whose libX11 lets `_XReply` be counted
directly), the same probe makes 81 round trips with master and 44 with
this branch, and 91 and 53 on a server where it is the first Motif
client (it then creates the drag window and writes its tables).

What the numbers say:

- **Round trips** are what the branch changes: 80 to 43.  34 came from
  `CvtStringToVirtualBinding` (`VirtKeys.c`), which fetched the keyboard
  mapping of each of the 34 default virtual bindings with
  `XGetKeyboardMapping`, although it had Xt's copy of the whole mapping
  at hand; 3 from the `XSync` that ended each protected read of the
  drag window's properties (`DragBS.c`).  The wall time follows, by
  about 0.27 ms per round trip on this machine (much more over a
  network); the other columns did not change beyond the noise.
- **The dynamic loader** does more work since the CMake build than in
  2.3.8 and 2.4.1: 2836 symbol lookups instead of about 2350, 2.42 M
  instructions instead of 2.1 M (16% of the startup).  The version
  scripts reduced libXm's exports from 3224 to 1742 symbols
  (`doc/abi-policy.md`), but the release flags link with `-z now`
  (hardening) and `-fno-plt`, so every imported function is bound at
  startup instead of when it is first called.  The difference is about
  0.3 M instructions, 0.1 ms.
- **Most of the startup is in libX11's internationalization**, not in
  Motif (callgrind, master, inclusive): the local input method that the
  editable XmText opens (`XOpenIM` from `XmImRegister`, in
  `_XmTextInputCreate`) parses the Compose file, 4.78 M instructions
  (31%); opening the locale (`_XOpenLC`, from `XtOpenApplication`)
  3.78 M (24%); the dynamic loader 2.42 M (16%).  Motif's own share is
  small: the XmDisplay 0.92 M (6%, of which the virtual bindings
  0.50 M), the class initializations 0.6 M, the converters 0.27 M.
- `LD_DEBUG=statistics` cycle counts move by ±10% between runs here;
  the instruction counts are the reliable comparison.

Where the 43 round trips of the branch come from (`rtrips.txt`):

| Round trips | From |
|---:|---|
| 11 | Xlib and Xt opening the display: the connection, XKB and its map, the extensions that libXext and the font code query, Xt's atoms and screen resources |
| 11 | the XmDisplay: the drag window, its proxy, atoms and targets tables (5), the virtual bindings properties (3), `_MOTIF_WM_INFO`, its atoms (2) |
| 6 | the default colors (`XmeGetDefaultPixel`: 5 `XAllocColor`, 1 `XQueryColor`) |
| 3 | the ColorObj (`XGetSelectionOwner`, an atom) and the XmScreen (`XQueryBestCursor`) |
| 3 | the shell's WM properties |
| 2 | Xt's keyboard and modifier mappings |
| 2 | the default font (`XLoadQueryFont`) |
| 2 | the XmText: its atoms (`XInternAtoms`) and the `XSync` of `_XmImRealize` |
| 1 | the ISO10646 atoms of the first XmString drawn |
| 2 | the two `XSync` of `hello_motif` itself |

On a TrueColor visual the server's answer to `XAllocColor` is a function
of the visual's masks, so the color round trips could be computed
locally, but only if the client rounds the RGB values as the server
does, which the protocol leaves to the server.

## xmbench

The top functions of every case, from `perf record` of one run per
case (`xmbench/perf.top`; short cases have few samples, so only the
large shares mean much).  The cases are those of `xmbench -l`.

| Case | Top functions (self time) | Note |
|---|---|---|
| trait-get | `TraitFind` 51%, `XmeTraitGet` 37% | the lookup itself; Trait.c |
| gadget-get, toggle-get, gadget-get-shells, spot-same, text-cursor | `_XrmInternalStringToQuark` 10-16%, `XtIsSubclass`, `GetValues`, locks | Xt converting the resource names of every `XtGetValues`/`XtSetValues` to quarks |
| gadget-set, toggle-set | `XCheckIfEvent` 62-75%, `CheckExposureEvent` 13-16% | Xt's exposure compression scanning the event queue: the case queues 10,000 exposures before it handles them, so this is the benchmark's pattern more than a cost of real programs |
| gadget-cache | `_XmLabelCacheCompare` 12% | the gadget cache (Cache.c) |
| container-icons | `IconGCacheCompare` 70%, `_XmCachePart` 6% | the IconGadget cache comparison, quadratic in the number of distinct icons |
| rc-buttons, xft-labels, rendertable-cvt | `XSaveContext` 8-10% | libX11's XContext table, grown and rehashed as widgets register |
| rc-gadgets, rc-gadgets-destroy, form-chain | `GetResources`, `XtIsSubclass`, `XtReleaseGC` | Xt widget creation |
| list-add | spread: `XmStringParseText`, `SetValues` | see [XmList](#xmlist-100k-items) |
| list-select | `UpdateSelection` 53%, `ItemNumber` 19% | List.c: a linear scan per selection |
| list-delete | `APIDeletePositions` 35%, `ItemNumber` 30% | List.c: linear scans |
| list-pagedown | `XmStringParseText`, gconv | the bench; 103 requests and 1 round trip per page |
| text-append | `_XmStringSourceGetChar` 24%, `Replace` 21% | linear in the inserted text since the line table is bisected |
| text-type, text-insdel | `_XmTextUpdateLineTable` 84-88% | shifting the start of every following line, see [XmText](#xmtext-10-mb-appended) |
| scrollbar-repeat, menu-post | `recvmsg`, `poll`, xcb | waiting for the server (timers, grabs) |
| shadow-2, shadow-8 | `DrawSimpleShadow` 17-25% | Draw.c, two requests per call |
| visibility | `XtIsSubclass`, `XtWindowToWidget` | one round trip per call (`XQueryTree`) |
| xmstring-create, xmstring-concat | `XmStringParseText` 24%, gconv, `mbrtowc`; `realloc` 29% | XmString.c: locale conversion per character; exact-size reallocations |
| xmstring-extent* , xmstring-draw* | `XTextExtents` 12-38%, `XftCharIndex`, `XftGlyphRender` | the font libraries |

`xmbench -r 5`, master and this branch on the same Xvfb, CPU time per
operation (the cases not listed did not change beyond the noise):

| Case | master | branch |
|---|---:|---:|
| text-append (1 KB at the end, 10 MB) | 14.7 µs | 7.3 µs |
| text-type (1 character at the middle of 1 MB) | 10.7 µs | 6.7 µs |
| text-insdel (2 characters in and out at the middle) | 10.7 µs | 6.6 µs |

## XmText: 10 MB appended

`xmbench text-append`: 10,000 insertions of 1 KB lines at the end of an
XmText.

| | master | branch |
|---|---:|---:|
| Instructions (callgrind, whole run) | 1744 M | 549 M |
| `_XmTextGetTableIndex` | 1201 M (69%) | 5 M (1%) |
| `Scan` (TextStrSo.c) | 230 M | 230 M |
| `_XmStringSourceGetChar` | 123 M | 123 M |
| `Replace` (TextStrSo.c) | 104 M | 104 M |
| Allocations (heaptrack) | 36,539 | 36,419 |
| Round trips | 0 per insertion | 0 per insertion |

`_XmTextGetTableIndex` found the line of a position by walking the line
table from `table_index`, which stays near the top of the text, so that
every insertion at the end walked the whole table: quadratic.  It now
bisects the table (`Text.c`).  What remains is linear in the inserted
text: `Scan` and `_XmStringSourceGetChar` look for the line breaks of
the new text, `Replace` copies it into the gap buffer.  Each insertion
makes three allocations (82% of the run's): two in `RefigureLines` and
the temporary table of `_XmTextUpdateLineTable`; the rest are Xt and
Xlib at startup.

Typing in the middle of a long text (text-type) still costs
`_XmTextUpdateLineTable` 88% of the time: after an edit it adds the
change in length to the start of every following line.  The table
holds absolute positions in `XmTextLineTableRec` (31-bit fields), which
the installed `TextP.h` defines, so keeping positions relative to a
moving point would change what subclasses read; it is a tight loop
over 4-byte records, about 5 µs per edit with 8,000 lines after it.

## XmList: 100k items

`xmbench list-add`: 100,000 `XmListAddItemUnselected` at the end of a
scrolled list; the benchmark creates each item with
`XmStringCreateLocalized`.

| | master | branch |
|---|---:|---:|
| Instructions (callgrind, whole run) | 1616 M | 1616 M |
| `SetVerticalScrollbar` (inclusive) | 792 M (49%) | same |
| of which `XtSetValues` on the ScrollBar | 689 M (43%) | same |
| `XmStringCreateLocalized` (the benchmark's own strings) | 530 M (33%) | same |
| `AddInternalElements` | 143 M (9%) | same |
| `XmStringExtent` of the new items | 103 M (6%) | same |
| Allocations (heaptrack) | 506,908 | 506,786 |
| X requests | 5 per item | 5 per item |
| Round trips | 0 per item | 0 per item |

Half the cost of adding an item is updating the vertical ScrollBar:
every addition calls `XtSetValues` on it (new maximum and slider size),
which redraws it (`XmeDrawShadows`: the 5 requests per item).  Batching
that update (once per burst of additions, or only when the slider
changes) would halve the cost of filling a list; that is List.c, which
another stream changes.  Each item also costs two allocations in
`AddInternalElements` and one in `APIAddItems`, besides the two of the
benchmark's string.

## mwm managing 200 clients

`xclients -n 200 -t 2000`: 200 windows mapped at once, 2000 title
changes round robin, the windows withdrawn one by one, each time
waiting for mwm.  `mwm.sh -r 7`, medians, interleaved:

| | master | branch |
|---|---:|---:|
| Map until all are reparented (ms) | 1326 | 1321 |
| 2000 title changes, client side (ms) | 91 | 85 |
| Withdraw one by one (ms) | 836 | 868 |
| mwm CPU time (ms) | 605 | 581 |
| mwm calls to malloc/calloc/realloc | 112,061 | 71,932 |
| mwm round trips | 5,756 | 5,722 |
| mwm instructions (callgrind) | 185.0 M | 179.8 M |

The profile (branch; master where it differs):

- **perf** (samples of mwm, two runs): `XCheckWindowEvent` 14-16%,
  `UpdateScreenClientList` 2-4% (master: 5.5%), then malloc, poll and
  the locks.  `XCheckWindowEvent` is `GetTimestamp` (`WmEvent.c`), which
  `ManageWindow` calls for each new client: it appends to a property of
  its own window, `XSync`s, and looks for the PropertyNotify in the
  event queue, which while 200 windows map at once holds hundreds of
  MapRequests and ConfigureRequests, so the search is quadratic in the
  burst.
- **callgrind** (inclusive): the title changes are 56% of mwm's
  instructions: `ProcessWmWindowTitle` converts the WM_NAME property
  (`XmCvtTextPropertyToXmStringTable`, 16%), redraws the title bar
  (`DrawWindowTitle`, 22%, mostly `XmStringDrawImage` and the extents)
  and redraws the icon title (`RedisplayIconTitle`, 17%) of windows
  that are not iconified, whose icon is not mapped.  `ManageWindow` is
  20%.  `UpdateScreenClientList` went from 7.1 M to 1.6 M instructions,
  `WithdrawWindow` from 6.4 M to 2.7 M.
- **heaptrack**: master made 112,458 allocations, 40,400 of them in
  `UpdateScreenClientList`, which rebuilt `_NET_CLIENT_LIST` with one
  `realloc` per client on every manage and withdraw (quadratic); the
  branch counts the clients first: 72,757 allocations, of which xcb's
  reading of replies and events is 35%.
- **Round trips by request** (xtrace replies): 4,417 GetProperty (2,000
  of them `XGetWMName` for the title changes, about 12 per managed
  window for its ICCCM and Motif properties), 606 GetInputFocus (the
  `XSync`s: three per managed window, one in `ManageWindow` and one in
  `GetTimestamp` among them), 406 GetWindowAttributes with GetGeometry,
  200 QueryExtents.

## What is left

The hotspots that this work did not change, because another stream
owns the code or because they are outside Motif, by expected gain:

1. **Opening the input method when an XmText is created**
   (`XmImRegister` → `XOpenIM`, XmIm.c): 31% of the instructions of a
   small program's startup, for parsing the Compose file of the locale;
   opening it on the first focus-in instead would take it out of the
   startup of every program with an editable text.
2. **The ScrollBar update on every XmList addition**
   (`SetVerticalScrollbar`, List.c): 49% of filling a list and its 5
   requests per item.
3. **XmList selection and deletion by value** (`UpdateSelection`,
   `ItemNumber`, `APIDeletePositions`, List.c): linear scans, about 2 ms
   per operation in a 100k list.
4. **IconGadget cache** (`IconGCacheCompare`, IconG.c / Cache.c): 70% of
   creating 10,000 icons.
5. **mwm** (not owned by another stream, but changes of behaviour
   rather than small fixes): `GetTimestamp` searching the event queue
   for its PropertyNotify and the `XSync` before it (one of them could
   go: `ManageWindow` already `XSync`s just before); redrawing the icon
   title of a window whose icon is not shown (17% of a title change; the
   icon is redrawn from its Expose when it is mapped, but the icon box
   and the workspace states need care).
6. **XmString creation** (`XmStringParseText`, the locale conversion
   per character, XmString.c), which dominates `xmstring-create` and a
   third of `list-add`.
7. **The text line table shift** after an edit in the middle of a long
   XmText (`_XmTextUpdateLineTable`): bound by the installed
   `XmTextLineTableRec`.
8. **Startup outside Motif**: the locale (`_XOpenLC`, 24%), and the
   dynamic loader's binding of every import at startup under `-z now`
   (16%); `-z lazy` would trade the hardening for about 0.3 M
   instructions.
