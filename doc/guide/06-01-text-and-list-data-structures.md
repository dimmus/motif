# 6.1 Text and List: Two Data Structures

**Scope.** `XmText` is the multi-line editor and `XmList` the
selectable list; both are 1989 designs that have been tuned rather
than replaced, because their instance records are ABI.  This chapter
examines the two places where an algorithm, not a widget method,
determines the cost: the Text *source* (the buffer behind one or
several Text widgets) and the List's redraw on scrolling and
selection.

---

## 6.1.1 The Text source: a gap buffer

`XmText` separates the *widget* (display, cursor, input) from the
*source* (the characters), so that several widgets can edit one
buffer (`XmNsource`).  The default source,
[`TextStrSo.c`](../../src/lib/Xm/TextStrSo.c), is a *gap buffer*:

```
 ptr                                    gap_start        gap_end        ptr + maxlength*char_size
  │  text before the cursor ...          │  (unused)      │  ... text after the cursor   │
  └──────────────────────────────────────┴────────────────┴──────────────────────────────┘
                 length characters in total; characters are 1, 2 or 4 bytes (char_size)
```

The buffer holds the text in two runs with a *gap* between them.  An
insertion at the gap writes into it; a deletion at the gap widens it;
moving the insertion point moves the gap by copying the characters
between the old and new position across it.  Position arithmetic is
one branch:

```c
/* _XmStringSourceGetChar, simplified: a position before the gap is at
 * ptr + pos; after it, skip the gap */
    gap_size = (data->gap_end - data->gap_start) / char_size;
    if (data->ptr + char_pos < data->gap_start)
      return &data->ptr[char_pos];
    return &data->ptr[(position + gap_size) * char_size];
```

**Why a gap buffer.**  Text editing is local: a user types runs of
characters at one place.  A plain array costs O(n) per insertion (shift
the tail); a rope or piece table costs O(log n) but needs pointer
chasing for every character access, which the Text widget's line
layout (`_XmTextUpdateLineTable`, which rescans from the edit to the
end of the paragraph) does constantly.  A gap buffer gives amortised
O(1) insertion at the cursor, O(1) random access, and contiguous
memory for scanning, at the cost of O(distance) when the cursor jumps.
Emacs made the same choice for the same reasons; the Motif source
supports three character sizes (`char`, `BITS16` for two-byte
charsets, `wchar_t`) with the same code through `char_size`.

**Growth policy** (this tree):

```c
#define TEXT_INCREMENT 1024
#define TEXT_INITIAL_INCREM 64
/*
 * Return a buffer length, in characters, of at least needed + 1 (one
 * slot is reserved), growing from len: small buffers double, larger ones
 * grow by half, so that a run of inserts costs amortised O(1) per
 * character.  Return 0 if no buffer of char_size characters that large
 * can be allocated.
 */
static int BufferLength(int len, long needed, int char_size)
{
  long limit = INT_MAX / char_size;
  long l = len < TEXT_INITIAL_INCREM ? TEXT_INITIAL_INCREM : len;
  if (needed < 0 || needed >= limit)
    return 0;
  while (l <= needed) {
    if (l < TEXT_INCREMENT)
      l *= 2;
    else if (l > limit - l / 2)
      l = limit;
    else
      l += l / 2;
  }
  return (int)l;
}
```

Upstream grew the buffer by a fixed 1 024 characters, so filling a
10 MB buffer by appending reallocated it ten thousand times and copied
O(n²/1024) bytes in total; `xmbench text-append` ("XmTextInsert 1 KB at
the end, 10 MB total") is that scenario.  Doubling below 1 024 and
growing by 1.5× above gives the textbook amortised O(1) per character
with at most 50 % slack, and the `limit` arithmetic keeps the byte
count below `INT_MAX` for the widest character size.

**Shrinking with hysteresis.**  After a deletion:

```c
  /*
   * Give memory back only once most of the buffer is unused, so that
   * edits around a size boundary do not reallocate (and move the gap
   * to the end) every time.  The new buffer still has room to grow.
   */
  if (data->maxlength > TEXT_INCREMENT && data->length < data->maxlength / 4) {
    /* Move the gap to the last position. */
    _XmStringSourceSetGappedBuffer(data, data->length);
    data->maxlength = BufferLength(TEXT_INITIAL_INCREM, data->length, char_size);
    data->ptr = _XmReallocArray(data->ptr, data->maxlength, char_size);
    ...
  }
```

Shrinking at one quarter occupancy while growing at full occupancy
leaves a factor of two between the thresholds, so a buffer hovering
around a boundary (`xmbench text-insdel`, "insert/delete 2 chars at a
buffer boundary") does not thrash.

**What the source also tracks.**  The selection (`left`, `right`,
`hasselection`), the list of widgets viewing the buffer
(`widgets[numwidgets]`, each told to invalidate and update its line
table after an edit), and the *gap position policy*: the gap is moved
to the edit position lazily, so a sequence of edits at one place costs
one move.  Undo is not part of the source; the TODO lists it as
untested.

## 6.1.2 The List: scrolling by copying

`XmList` keeps its items as an array of `XmString`s plus a parallel
array of `ElementPtr` records (text, extent, `selected`,
`LastTimeDrawn`), and draws the visible window of `visibleItemCount`
rows starting at `top_position`.  Upstream redrew every visible row on
every scroll; this tree's `ScrollList` copies the rows that remain
visible with one `XCopyArea` and draws only the rows that enter the
window:

```c
/* src/lib/Xm/List.c */
static void ScrollList(XmListWidget lw, int old_top)
{
  ...
  /* The gaps between the bands show the window background, which is
   * only the same everywhere when it is not a pixmap; and the copy needs
   * Redisplay to see GraphicsExpose, which a subclass may not ask for. */
  if (!XtIsRealized((Widget)lw) || !lw->list.items || !lw->list.itemCount ||
      (lw->core.background_pixmap != XtUnspecifiedPixmap) ||
      !(XtClass((Widget)lw)->core_class.compress_exposure & XtExposeGraphicsExpose) ||
      !XtIsSensitive((Widget)lw) || (lw->list.spacing < 1) || (DrawnTop(lw) != old_top) ||
      (DrawnGen(lw) != ListGen(lw)) || (DrawnXOrigin(lw) != lw->list.XOrigin) ||
      (DrawnVizCount(lw) != lw->list.visibleItemCount) || (DrawnItemHeight(lw) != height))
  {
    DrawList(lw, NULL, TRUE);
    return;
  }
  num = MIN(top + lw->list.visibleItemCount, lw->list.itemCount);
  old_num = MIN(old_top + lw->list.visibleItemCount, lw->list.itemCount);
  first = MAX(top, old_top);
  last = MIN(num, old_num);
  ...
  /* Copy the kept rows, keeping source and destination in the clip. */
  shift = LINEHEIGHTS(lw, top - old_top);
  d0 = ...; d1 = ...;
  if (d0 < d1) {
    values.graphics_exposures = True;
    gc = XtGetGC((Widget)lw, GCGraphicsExposures, &values);
    XCopyArea(dpy, win, win, gc, clip_x, d0 + shift, clip_w, d1 - d0, clip_x, d0);
    XtReleaseGC((Widget)lw, gc);
  }
  /* Draw the other rows, fill the top lines of the copied ones. */
  for (pos = top; pos < num; pos++) {
    ...
    if ((pos < first) || (pos >= last) || (item->selected != item->LastTimeDrawn) ||
        ((y0 < y1) && ((y0 < d0) || (y1 > d1))))
    {
      DrawItems(lw, pos, pos + 1, TRUE);
      continue;
    }
    ...
  }
```

Three ideas make a window-to-window copy safe, and each corresponds to
a condition in the guard:

1. **The window must show what the List thinks it shows.**  A copy is
   only valid if the pixels on screen are those of the last full
   `DrawList` with the same items, extents, horizontal origin, visible
   count and row height.  The List records those parameters when it
   draws and compares them before copying.  The record lives in
   instance fields the List does not otherwise use, because adding
   fields to `XmListPart` would change the ABI:

   ```c
   /*
    * What the window shows, for ScrollList.  These live in fields of the
    * instance record that the List does not use otherwise.  ListGen counts
    * the changes that can alter how rows are drawn; the Drawn values are
    * those of the last full DrawList, valid while DrawnGen equals ListGen
    * (and DrawnTop is not -1).
    */
   #define ListGen(lw) ((lw)->list.vmin)
   #define DrawnGen(lw) ((lw)->list.vExtent)
   #define DrawnTop(lw) ((lw)->list.vOrigin)
   #define DrawnXOrigin(lw) ((lw)->list.vmax)
   #define DrawnVizCount(lw) ((lw)->list.FontHeight)
   #define DrawnItemHeight(lw) ((lw)->list.CharWidth)
   #define RowsChanged(lw) (ListGen(lw) = (int)((unsigned int)ListGen(lw) + 1)) /* may wrap */
   ```

   `ListGen` is a *generation counter* incremented by every operation
   that changes rows (add, delete, replace, font change); `DrawnGen` is
   the generation at the last full draw.  A generation compare is O(1)
   and replaces the alternative of tracking which rows changed.
2. **Selection state is per row.**  `LastTimeDrawn` records whether a
   row was drawn selected; a row whose `selected` differs is redrawn
   rather than copied, and `DrawItem` returns early when the two
   agree.  This is also what makes selection O(changed rows): selecting
   an item in a 100 000-item list (`xmbench list-select`) redraws one
   row, and `XmListDeleteItem` no longer rescans every item to rebuild
   the selected-positions array.
3. **The source of the copy may be obscured.**  If another window
   covers part of the List, the server cannot copy those pixels; with
   `graphics_exposures` set it sends `GraphicsExpose` events for the
   missing areas, and the List's `Redisplay` repaints them.  That
   requires the class to receive `GraphicsExpose` at all
   (`XtExposeGraphicsExpose` in `compress_exposure`), which a subclass
   may have turned off, hence the check; and it requires the background
   to be a solid colour, since a pixmap background would be shifted by
   the copy.

Cost per scroll: one `CopyArea` request plus O(new rows) draws instead
of O(visible rows) draws; `xmbench list-pagedown` ("ListNextPage in a
100k list") measures it, and the A/B harness modes `list` and
`listscroll` compared selection state, callbacks and window pixels with
the old code.

## 6.1.3 What was not changed, and why

Both widgets still keep arrays where a different structure would be
asymptotically better: the List's `items`/`InternalList` arrays make
`XmListAddItem` at the front O(n), and the Text line table is rebuilt
from the edit point.  The instance records (`XmListPart`, `XmTextPart`
and the source record) are declared in installed headers and compiled
into applications' subclasses, so replacing an array by, say, a
balanced tree is an ABI break and waits for a SONAME change; the
TODO's performance section lists "List and Container data structures
(blocked by installed struct layouts)".  The changes that *were* made
all fit inside the existing layout: a growth policy, a generation
counter in unused fields, a copy instead of a redraw.

## References

- [`src/lib/Xm/TextStrSo.c`](../../src/lib/Xm/TextStrSo.c),
  [`TextStrSoP.h`](../../src/lib/Xm/TextStrSoP.h), [`Text.c`](../../src/lib/Xm/Text.c),
  [`TextOut.c`](../../src/lib/Xm/TextOut.c), [`TextIn.c`](../../src/lib/Xm/TextIn.c)
- [`src/lib/Xm/List.c`](../../src/lib/Xm/List.c), [`ListP.h`](../../src/lib/Xm/ListP.h)
- Gap buffers: <https://en.wikipedia.org/wiki/Gap_buffer>;
  amortised analysis of geometric growth: <https://en.wikipedia.org/wiki/Dynamic_array#Geometric_expansion_and_amortized_cost>
- Xlib, `XCopyArea` and `GraphicsExpose`: <https://www.x.org/releases/current/doc/libX11/libX11/libX11.html#Copying_Areas>
- Manual pages `XmText(3)`, `XmTextSource(3)`, `XmList(3)` in [`doc/man/man3`](../man/man3)
- `xmbench text-append text-type text-insdel text-cursor list-add list-select list-delete list-pagedown`;
  the `Text` and `Layout` libcheck suites; the `Text.xdotool` test that
  drives a Text widget with real key events
