# 6.2 Drag and Drop and the Inter-Client Protocols

**Scope.** Drag and drop in Motif is a protocol between two X clients
that may know nothing about each other, negotiated through window
properties and client messages, with the X server as the only shared
medium.  This chapter describes the Motif protocol's design
(initiator, receiver, protocol styles, shared tables), its data
structures (the drag context, the drop-site database), the XDND
support that sits beside it, the clipboard, and the threat model that
all of them share.

---

## 6.2.1 Actors and objects

| Object | Class | Role |
|--------|-------|------|
| `XmDragContext` | `XmDragContext` (`DragC.c`) | One per drag in progress, created by `XmDragStart` in the *initiator*; owns the drag icon, the targets, the operations, the protocol state machine and the motion buffer |
| `XmDragIcon` | `XmDragIcon` (`DragIcon.c`) | The pixmap and mask that follow the pointer, composed from source, operation and state icons in a `XmDragOverShell` (`DragOverS.c`), an override-redirect shell that moves with the pointer |
| `XmDropSiteManager` | `XmDropSiteManager` (`DropSMgr.c`) | One per display, in the `XmDisplay` object; the *receiver* side's database of registered drop sites |
| `XmDropTransfer` | `XmDropTransfer` (`DropTrans.c`) | The transfer of the dropped data, through X selections |
| `XmDisplay` | `XmDisplay` (`Display.c`) | Per-display defaults: `XmNdragInitiatorProtocolStyle`, `XmNdragReceiverProtocolStyle`, `XmNenableDragIcon`, `XmNenableBtn1Transfer` |

The *Uniform Transfer Model* of Motif 2.0 (`Transfer.c`, the
`XmQTtransfer` trait) sits above all of this: a widget implements
`convert` and `destination` callbacks once, and the same code serves
the primary selection, the clipboard and drag and drop.

## 6.2.2 Protocol styles

The initiator and the receiver each declare a *style*, and the
Motif protocol picks one of three ways to run the drag from the pair.
The table in [`DragC.c`](../../src/lib/Xm/DragC.c):

```c
     * Initiator  ------------------------------------
     *      NO    | NO | NO | NO | NO | NO | NO | X  |
     *      DO    | NO | DO | DO | DO | DO | DO | X  |
     *      PP    | NO | DO | P  | P  | P  | D  | X  |
     *      P     | NO | DO | P  | P  | P  | DO | X  |
     *      PD    | NO | DO | D  | P  | D  | D  | X  |
     *      D     | NO | DO | D  | DO | D  | D  | X  |
     *      PR    | NO | DO | P  | P  | D  | D  | X  |
     *      X     | NO | X  | X  | X  | X  | X  | X  |
```

(rows: initiator, columns: receiver; NO none, DO drop-only, P
preregister, D dynamic, PP/PD "prefer", PR "prefer receiver", X XDND).

- **Drop only** (`XmDRAG_DROP_ONLY`): no feedback during the drag; the
  receiver learns of the drop when it happens.
- **Preregister** (`XmDRAG_PREREGISTER`): the receiver writes its drop
  sites (rectangles, operations, targets, animation style) into a
  property on each of its top-level windows (`_MOTIF_DRAG_RECEIVER_INFO`)
  in advance; the initiator reads them and provides all feedback
  itself, with *no messages to the receiver during the drag*.  This is
  the fast path: pointer motion costs the initiator a rectangle lookup
  and no inter-client traffic.
- **Dynamic** (`XmDRAG_DYNAMIC`): the initiator sends `_MOTIF_DRAG_AND_DROP_MESSAGE`
  client messages (`TOP_LEVEL_ENTER`, `DRAG_MOTION`, `OPERATION_CHANGED`,
  `DROP_SITE_ENTER/LEAVE`, `TOP_LEVEL_LEAVE`, `DROP_START`) and the
  receiver answers each; the receiver computes the feedback and can
  change its mind per position.  Flexible and slow: a round of
  messages per motion event.
- **Prefer** variants express what a client can do with what it would
  rather do; the table resolves each pair deterministically, which is
  essential because both sides compute it independently and must agree.

The messages are byte-packed structures (`DragICC.c`, `DragICCI.h`)
with a byte-order mark, so that a big-endian and a little-endian client
on the same display can swap correctly; "byte-swapped correctly" is
one of the fixes in the [CHANGELOG](../../CHANGELOG.md).

## 6.2.3 Shared tables on a hidden window

Preregistration needs atoms (one per target type, one per operation)
and target lists, and interning atoms is a round trip each.  The
protocol therefore shares them, once per display, through a hidden
window, as [`DragBS.c`](../../src/lib/Xm/DragBS.c) explains:

> The data is stored on window properties of motifWindow, a persistent,
> override-redirect, InputOnly child window of the display's default
> root window.  A client looks for the motifWindow id on the
> "_MOTIF_DRAG_WINDOW" property of the display's default root window.
> If it finds the id, the client saves it ... Otherwise, the client
> creates the motifWindow and stores its id on that root property.

Two properties on that window:

- `_MOTIF_DRAG_ATOMS`: a table of (atom, timestamp) pairs.  A client
  allocates an atom with `_XmAllocMotifAtom(timestamp)` (reuse one
  whose timestamp is 0, else append `_MOTIF_ATOM_n`), and frees it by
  zeroing the timestamp.  The atoms name *selections* for the transfer,
  which must be unique per drag.
- `_MOTIF_DRAG_TARGETS`: a table of *target lists*, each sorted, each
  with an index.  `_XmTargetsToIndex` sorts a client's target list and
  looks for an identical one (O(lists × targets) comparison, cached
  client-side), appending it if new; a drop site then advertises an
  *index* instead of a list of atoms, and `_XmIndexToTargets` maps it
  back.  Sorting "to avoid permutations" means that
  `{TEXT, STRING}` and `{STRING, TEXT}` are one entry.

Every client on the display reads and *writes* these properties, and a
malicious or buggy client can write anything: a targets table with a
count larger than the property, an atoms table with an odd length, a
receiver-info property whose drop-site tree points past its end.  The
2026 review bounds-checked all of them ("the Motif drag protocol
messages, drop site and receiver-info properties, the shared atoms and
targets tables and XDND data are bounds-checked", and "a remote drop
site stream can no longer cause a use-after-free").  The
`_XmGetWindowPropertyChecked` helper of chapter 6 is the common entry.

## 6.2.4 The drop-site database

On the receiver, `XmDropSiteRegister(widget, args)` records a drop
site; the manager keeps them in a tree that mirrors the widget tree,
as the comment at the top of [`DropSMgr.c`](../../src/lib/Xm/DropSMgr.c)
describes:

> Drag and Drop maintains a two way mapping between information records
> and the dropsites widgets they represent.  There are two kinds of
> records ... The first kind of records are associated with a real
> dropsite ... The second kind are clipping records which represent
> widgets which in some way obscure one or more dropsites.  ... When a
> new record is created, it is associated via a hashtable kept in
> dsm -> dstable to the widget it represents.

and, on any change:

> RemoveAllClippers ... The dropsites become a flat list held in the
> child list of the topmost node.  SyncDropSiteGeometry ... see what
> updates are needed to the internal geometry information.
> DetectAndInsertAllClippers ... We now rebuild the clipper hierarchy.

The *clippers* are the reason for the tree: a drop site inside a
ScrolledWindow is only droppable where the clip window shows it, so
every ancestor that clips is a node whose rectangle bounds its
subtree.  Finding the drop site under the pointer is a descent of the
tree, O(depth × children per level), and the `XmDropSiteStartUpdate`
/`XmDropSiteEndUpdate` bracket around `XmeConfigureObject` (chapter 3)
batches the rebuild when many children move at once.  The records
"are kept internally in a compacted form" with accessor macros in
`DropSMgrI.h`; the `XmDSInfo` records are keyed by widget in a
`XmHashTable` (chapter 5.1).  In preregister style the whole tree is
serialised into the `_MOTIF_DRAG_RECEIVER_INFO` property of the shell,
which is the "drop site stream" the initiator parses.

## 6.2.5 The initiator's event loop

`XmDragStart` creates the drag context, grabs the pointer, and
installs the drag-over shell.  Motion events are buffered:

```c
/* src/lib/Xm/DragC.c */
typedef struct _MotionEntryRec {
  int type; Time time; Window window; Window subwindow; Position x, y; unsigned int state;
} MotionEntryRec, *MotionEntry;
#define STACKMOTIONBUFFERSIZE 120
typedef struct _MotionBufferRec {
  XmDragReceiverInfo currReceiverInfo;
  Cardinal count;
  MotionEntryRec entries[STACKMOTIONBUFFERSIZE];
} MotionBufferRec, *MotionBuffer;
#define MOTIONFILTER 16
```

The drag context reads all pending motion into the buffer and processes
only the last event of a run, or every sixteenth (`MOTIONFILTER`), so
that a fast mouse on a slow receiver (dynamic style) does not queue a
message per pixel; a stack buffer of 120 entries avoids allocation on
the hot path.  When a drop starts, the initiator arms a timer of ten
times the Xt selection timeout (`XtAppGetSelectionTimeout`) "in case
the drop site dies", so that a receiver that never completes the
transfer cannot leave the drag context waiting forever.  The drop itself is an X selection
transfer: the initiator owns a selection named by a shared atom, the
receiver's `XmDropTransfer` requests the targets it wants with
`XtGetSelectionValues`, and the initiator's convert callback (the
transfer trait) supplies them, so the data never passes through a
property that another client could read in transit.

## 6.2.6 XDND

The freedesktop *XDND* protocol is what GTK, Qt and Java use.  Motif
supports it as a seventh protocol style (`XmDRAG_XDND`, the last row
and column of the table): the atoms `XdndAware`, `XdndEnter`,
`XdndPosition`, `XdndStatus`, `XdndLeave`, `XdndDrop`, `XdndFinished`
and the action atoms are interned in `DragICC.c`, the receiver answers
`XdndPosition` with `XdndStatus`, and the transfer uses the `XdndSelection`.
A Motif initiator dragging over a GTK window, or a GTK initiator over
a Motif drop site, therefore works; a window that advertises
`XdndAware` is recognised before the Motif tables are consulted.  The
XDND support is less complete than the Motif protocol's (no
preregistration, one target list), and the Hentenaar tree describes
its own XDND work as "transparent" (chapter 0); this tree's XDND data
is bounds-checked like the rest.

## 6.2.7 The clipboard

[`CutPaste.c`](../../src/lib/Xm/CutPaste.c) implements the Motif
clipboard API (`XmClipboardStartCopy`, `XmClipboardCopy`,
`XmClipboardRetrieve`, ...) on top of the `CLIPBOARD` selection with
one addition: the clipboard's records live in `_MOTIF_CLIP_*`
properties *on the root window*, so that the data survives the exit
of the program that copied it, and so that a program can ask what
formats are available without a selection round trip.  The records
hold item counts, format registrations and the data itself, written
by any client; the review validated "clipboard records, format
registrations and item counts read from the root window" and bounded
the wait for a foreign clipboard owner.  One open issue remains in the
TODO: a corrupt record still makes the library call `exit(1)` through
`ClipboardError`, which is a denial of service by any client on the
display and is tracked with a reproducer.

## 6.2.8 Assessment

| Design | For | Against |
|--------|-----|---------|
| Three styles with a resolution table | Fast feedback (preregister) when both sides can; full flexibility (dynamic) when needed; always agrees | Two complete code paths on each side; the table is a 7 × 7 convention nobody else implements |
| Shared atom and target tables | No per-drag interning; target lists compared by index | Shared mutable state on the root window written by every client; must be validated on every read |
| Clipping tree of drop sites | Correct drop detection inside scrolling containers | Rebuilt on geometry changes; compacted records with macro access |
| Motion buffer and filter | Bounded message rate | Feedback lags on a dynamic receiver by up to 16 events |
| Selection-based transfer | Data stays between the two clients; arbitrary targets | The full ICCCM selection machinery for a drop |
| XDND beside Motif DnD | Interoperates with every modern toolkit | Two protocols in one widget; the Motif one is unused by anything but Motif and CDE |

The protocols were designed when "another client on the display" meant
another user's xterm, not a hostile process; they are the part of
Motif where the 2026 security review found the most, and the part
where a reader should assume every length and count comes from an
adversary.

## References

- [`src/lib/Xm/DragC.c`](../../src/lib/Xm/DragC.c),
  [`DragBS.c`](../../src/lib/Xm/DragBS.c),
  [`DragICC.c`](../../src/lib/Xm/DragICC.c),
  [`DragICCI.h`](../../src/lib/Xm/DragICCI.h),
  [`DragIcon.c`](../../src/lib/Xm/DragIcon.c),
  [`DragOverS.c`](../../src/lib/Xm/DragOverS.c),
  [`DropSMgr.c`](../../src/lib/Xm/DropSMgr.c),
  [`DropSMgrI.h`](../../src/lib/Xm/DropSMgrI.h),
  [`DropTrans.c`](../../src/lib/Xm/DropTrans.c),
  [`Transfer.c`](../../src/lib/Xm/Transfer.c),
  [`CutPaste.c`](../../src/lib/Xm/CutPaste.c),
  [`Display.c`](../../src/lib/Xm/Display.c)
- ICCCM, chapter 2 "Peer-to-Peer Communication by Means of Selections":
  <https://www.x.org/releases/current/doc/xorg-docs/icccm/icccm.html>
- XDND specification: <https://www.freedesktop.org/wiki/Specifications/XDND/>
- Manual pages `XmDragStart(3)`, `XmDragContext(3)`, `XmDropSite(3)`,
  `XmDropSiteRegister(3)`, `XmDropTransfer(3)`, `XmTransferValue(3)`,
  `XmClipboardCopy(3)`, `XmDisplay(3)` in [`doc/man/man3`](../man/man3)
- Example: `src/examples/programs/drag_and_drop`
- [SECURITY.md](../../SECURITY.md), "Threat model";
  fuzzers for the drag messages in [`src/tests/fuzz`](../../src/tests/fuzz)
