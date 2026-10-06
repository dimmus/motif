# 2. The Drawing and Rendering System

**Scope.** The Motif look is a small set of drawing primitives applied
consistently: a 3-D shadow around every control, a highlight rectangle
around the control that has keyboard focus, separators, arrows and
toggle indicators.  This chapter analyses the shared drawing module
[`Draw.c`](../../src/lib/Xm/Draw.c) and its companions
([`DrArrow.c`](../../src/lib/Xm/DrArrow.c),
[`DrHiDash.c`](../../src/lib/Xm/DrHiDash.c),
[`DrPoly.c`](../../src/lib/Xm/DrPoly.c), [`DrTog.c`](../../src/lib/Xm/DrTog.c))
at the level of the X requests they generate: what each primitive
draws, how many requests it costs, where its memory comes from, and
why it was written the way it was.  Text and image rendering are the
subject of chapter 5.

**Model of cost.** An Xlib drawing call appends a request to the
client's output buffer; the buffer is flushed when full, on
`XFlush`/`XSync`, or when the client waits for an event.  A request
that needs a reply (a *round trip*) forces a flush and a wait.  The
primitives in this chapter generate no round trips; their cost is the
number of requests, the number of bytes per request and the server's
work per pixel.  All drawing happens through a *graphics context* (GC),
a server-side object holding the pen (foreground pixel, line width,
fill style, clip mask); changing a GC is itself a request.

---

## 2.1 The 3-D shadow

### 2.1.1 Specification

A Motif shadow of thickness *t* around a rectangle (x, y, w, h) is two
L-shaped bands of width *t*: the top and left edges in the *top shadow*
colour (lighter than the background for a raised look) and the bottom
and right edges in the *bottom shadow* colour, meeting along 45-degree
diagonals at the top-right and bottom-left corners.  `XmSHADOW_IN`
swaps the two colours; `XmSHADOW_ETCHED_IN` and `XmSHADOW_ETCHED_OUT`
draw two concentric shadows of thickness t/2 with opposite colours,
giving an engraved line.

### 2.1.2 Implementation

```c
/* src/lib/Xm/Draw.c */
static void DrawSimpleShadow(Display *display, Drawable d, GC top_gc, GC bottom_gc,
                             Position x, Position y, Dimension width, Dimension height,
                             Dimension shadow_thick, Dimension cor)
/* New implementation (1.2 vs 1.1) uses XSegments instead of XRectangles. */
/* Used for the simple shadow, the etched shadow and the separators */
/* Segment has been faster than Rectangles in all my benches, either
   on Hp, Sun or Pmax. Lines has been slower, that I don't understand... */
{
  static XSegment *segms = NULL;
  static int segm_count = 0;
  int i, size2, size3;
  if (!d)
    return;
  ASSIGN_MIN(shadow_thick, (width >> 1));
  ASSIGN_MIN(shadow_thick, (height >> 1));
  if (shadow_thick <= 0)
    return;
  size2 = (shadow_thick << 1);
  size3 = size2 + shadow_thick;
  _XmProcessLock();
  if (segm_count < shadow_thick) {
    segms = (XSegment *)_XmReallocArray((char *)segms, size2 << 1, sizeof(XSegment));
    segm_count = shadow_thick;
  }
  for (i = 0; i < shadow_thick; i++) {
    /*  Top segments  */
    segms[i].x1 = x;
    segms[i].y2 = segms[i].y1 = y + i;
    segms[i].x2 = x + width - i - 1;
    /*  Left segments  */
    segms[i + shadow_thick].x2 = segms[i + shadow_thick].x1 = x + i;
    segms[i + shadow_thick].y1 = y + shadow_thick;
    segms[i + shadow_thick].y2 = y + height - i - 1;
    /*  Bottom segments  */
    segms[i + size2].x1 = x + i + ((cor) ? 0 : 1);
    segms[i + size2].y2 = segms[i + size2].y1 = y + height - i - 1;
    segms[i + size2].x2 = x + width - 1;
    /*  Right segments  */
    segms[i + size3].x2 = segms[i + size3].x1 = x + width - i - 1;
    segms[i + size3].y1 = y + i + 1 - cor;
    segms[i + size3].y2 = y + height - 1;
  }
  XDrawSegments(display, d, top_gc, &segms[0], size2);
  XDrawSegments(display, d, bottom_gc, &segms[size2], size2);
  _XmProcessUnlock();
}
```

**The geometry.** Row *i* of the top band is a horizontal segment from
x to x + w − i − 1: each successive row is one pixel shorter on the
right, which produces the diagonal mitre.  Column *i* of the left band
starts at y + t (below the top band, which already covers the top-left
square) and ends at y + h − i − 1, one pixel shorter per column, the
mitre at the bottom-left.  The bottom and right bands mirror them.  The
total is 4t segments; the first 2t (top and left) are drawn with
`top_gc` in one request, the last 2t with `bottom_gc` in a second.

**The `cor` ("corner") parameter** decides who owns the diagonal
pixels.  With `cor == 0` (plain shadows) the bottom band starts at
x + i + 1 and the right band at y + i + 1, so the pixels on the two
diagonals belong to the top band, drawn first; with `cor == 1` (used
for both rings of an etched shadow) the bottom and right bands start
one pixel earlier and, being drawn second, overwrite the diagonal in
the bottom colour.  Two concentric rings with opposite colours then
meet cleanly.

**Thickness clamping.** `ASSIGN_MIN(shadow_thick, width >> 1)` and the
same for the height keep the two bands from crossing in the middle of
a small widget; `XmeDrawShadows` on a 3 × 3 pixel area with thickness
10 draws a thickness-1 shadow.

**Memory.** The segment array is a `static` grown on demand with
`_XmReallocArray` (chapter 1 §1.8) and never freed: the largest shadow
ever drawn in the process determines its size (4t `XSegment`s of 8
bytes, so a few hundred bytes).  The array is shared by every thread,
hence `_XmProcessLock` around its use.  The alternative, a stack array
sized for the common case with a heap fallback, is what the
`XmStackAlloc` macros in `XmI.h` provide elsewhere in the library;
here the designers chose the simpler static buffer because the
function is called thousands of times per second during a resize and
the lock is free in a single-threaded program (chapter 1 §1.7).

### 2.1.3 The public entry point

```c
void XmeDrawShadows(Display *display, Drawable d, GC top_gc, GC bottom_gc,
                    Position x, Position y, Dimension width, Dimension height,
                    Dimension shad_thick, unsigned int shad_type)
{
  ...
  if ((shad_type == XmSHADOW_IN) || (shad_type == XmSHADOW_ETCHED_IN)) {
    tmp_gc = top_gc; top_gc = bottom_gc; bottom_gc = tmp_gc;   /* switch top and bottom shadows */
  }
  if ((shad_type == XmSHADOW_ETCHED_IN || shad_type == XmSHADOW_ETCHED_OUT) && (shad_thick != 1)) {
    DrawSimpleShadow(display, d, top_gc, bottom_gc, x, y, width, height, shad_thick / 2, 1);
    DrawSimpleShadow(display, d, bottom_gc, top_gc,
                     x + shad_thick / 2, y + shad_thick / 2,
                     width - (shad_thick / 2) * 2, height - (shad_thick / 2) * 2,
                     shad_thick / 2, 1);
  }
  else
    DrawSimpleShadow(display, d, top_gc, bottom_gc, x, y, width, height, shad_thick, 0);
  _XmAppUnlock(app);
}
```

`XmSHADOW_IN` is implemented by swapping the GCs rather than by a
second code path, a pattern repeated in `XmeDrawSeparator` and
`XmeDrawArrow`.  An etched shadow of odd thickness loses one pixel
(thickness 3 becomes two rings of 1), and an etched shadow of thickness
1 degrades to a plain one.

### 2.1.4 Cost

| Primitive | X requests | Request size | Server work |
|-----------|-----------:|--------------|-------------|
| Plain shadow, thickness t | 2 `PolySegment` | 2t segments × 8 bytes each | 4t line draws of ≤ max(w, h) pixels |
| Etched shadow, thickness t | 4 `PolySegment` | 4 × (t/2) segments | 8 (t/2) line draws |

Compare the alternatives the original author benchmarked in 1992:
four `XFillRectangles` calls (two per GC) would need only 4 rectangles
per band *but cannot produce the diagonal mitre* without 4t rectangles
of decreasing size, that is the same count as segments with 8 bytes
more per element; `XDrawLines` would need one request per band but
draws a connected polyline with joins at the corners, which the author
measured as slower.  The modern reason to keep segments is different:
with any current server the time is dominated by the request round
through the kernel socket, and two requests per shadow is the minimum
for two colours (a GC change would be a third).  The `shadow-2` and
`shadow-8` cases of `xmbench` ([`src/tests/bench`](../../src/tests/bench))
measure exactly this function at the two thicknesses, reporting
nanoseconds and request count per call.

## 2.2 The focus highlight

`XmeDrawHighlight` draws the solid rectangle that marks keyboard focus:

```c
void XmeDrawHighlight(Display *display, Drawable d, GC gc, Position x, Position y,
                      Dimension width, Dimension height, Dimension highlight_thickness)
{
  XRectangle rect[4];
  ...
  rect[0].x = rect[1].x = rect[2].x = x;
  rect[3].x = x + width - highlight_thickness;
  rect[0].y = rect[2].y = rect[3].y = y;
  rect[1].y = y + height - highlight_thickness;
  rect[0].width = rect[1].width = width;
  rect[2].width = rect[3].width = highlight_thickness;
  rect[0].height = rect[1].height = highlight_thickness;
  rect[2].height = rect[3].height = height;
  XFillRectangles(display, d, gc, rect, 4);
  _XmAppUnlock(app);
}
```

One request, four rectangles (top, bottom, left, right), with the four
corner squares painted twice.  Double painting is harmless for a solid
fill and for a stipple, because X stipples are aligned to the drawable's
origin, not to the rectangle; it would matter for a GC with `GXxor`,
which Motif never uses here.  The alternative of four non-overlapping
rectangles saves 4t² pixels of fill at the cost of more arithmetic in
the client, which is not where the time goes.

[`DrHiDash.c`](../../src/lib/Xm/DrHiDash.c) provides the *dashed*
variant used by `XmList` for the location cursor, drawn as four
segments with `XSetLineAttributes(..., highlight_thickness, line_style,
CapButt, JoinMiter)`.  Because the line width is a GC attribute, the
function reads the GC's current line attributes, changes them, draws,
and restores them: three extra requests and a modification of a GC the
caller may consider read-only, which the source itself calls "a hack".
The correct alternative is a dedicated GC per widget, which is what
Primitive does for the solid highlight (`highlight_GC`).

## 2.3 Separators

`XmeDrawSeparator` handles eight styles in three groups:

| Style | Requests | Method |
|-------|----------|--------|
| `XmSINGLE_LINE`, `XmSINGLE_DASHED_LINE` | 1 `PolySegment` | one centred segment in `separator_gc` (the dash pattern is in the GC) |
| `XmDOUBLE_LINE`, `XmDOUBLE_DASHED_LINE` | 1 `PolySegment` | two segments at centre ± 1 |
| `XmSHADOW_ETCHED_IN/OUT` | 2 `PolyLine` (t < 4) or 2 `PolySegment` | one "dash" the length of the separator |
| `XmSHADOW_ETCHED_IN_DASH/OUT_DASH` | 2 per dash | dashes of length 3 × 2 × (t/2), gaps of the same length |

The shadowed styles do not call `DrawSimpleShadow` for the common
thicknesses 1 to 3; the source explains why with a picture: a two-row
etched line must be top-colour row over bottom-colour row, and
`DrawSimpleShadow` with its mitres would render the two rows with a
one-pixel stagger ("it looks non symetrical the way it is without
special code").  So `t < 4` draws one or two `XDrawLine`s per dash, and
only `t ≥ 4` uses the general shadow routine on a (dash × t) box.  The
dash loop is O(width / dash length) requests, with a final partial dash
drawn separately so that the separator ends at the margin exactly.

`XmeClearBorder` is the eraser for all of the above: four
`XClearArea` requests (there is no multi-rectangle clear in the X
protocol), one per band, with `exposures = False` so that clearing
does not generate expose events for the widget to redraw itself in a
loop.

## 2.4 Arrows and indicators

`XmeDrawArrow` ([`DrArrow.c`](../../src/lib/Xm/DrArrow.c)) draws the
triangular arrows of `XmArrowButton`, `XmScrollBar` and `XmSpinBox`
with a top-shadow edge, a bottom-shadow edge and a centre fill.  The
algorithm:

1. Compute the largest square that fits (`size = min(w, h) − 2`) and
   centre it.
2. Generate the arrow *pointing up* as three lists of 1- or 2-pixel-high
   rectangles, row by row from the base up (`wwidth` shrinks by 2 per
   row): the left edge rows go to the `top` list, the right edge rows to
   the `bot` list, the interior to `cent`.  The lists are static arrays
   grown on demand (`size / 2 + 6` entries each), like the shadow
   segments.
3. Rotate for the requested direction by swapping x/y and width/height
   (`XmARROW_LEFT`/`RIGHT`) and mirroring (`XmARROW_DOWN`/`RIGHT`
   also swap the `top` and `bot` lists, since the lit edge changes
   side).
4. Emit three `XFillRectangles` requests, one per GC.  Thickness 1 is
   produced by drawing the thickness-2 shadow and then recursively
   drawing a flat (thickness 0) arrow one pixel smaller in the centre
   colour over it.

The number of rectangles is O(size), the request count is a constant
3 (or 5 for thickness 1, since the recursive call issues three of its
own), and the pixel work is O(size²), which is the
minimum for a filled triangle.

[`DrTog.c`](../../src/lib/Xm/DrTog.c) draws the toggle indicators
(`XmeDrawDiamond`, `XmeDrawIndicator`, `XmeDrawCircle`, the check mark
and the cross) with the same approach: rectangle lists per row, three
GCs, a handful of requests.  [`DrPoly.c`](../../src/lib/Xm/DrPoly.c)
generalises the shadow to any polygon (`XmeDrawPolygonShadow`) by
building an X `Region` from the points and handing it to the region
shadow code in `Region.c`, which walks the region's horizontal spans
and classifies each edge as lit or shaded by its orientation; it is
used by the Notebook tabs.

## 2.5 Where the GCs come from

Every `Xme*` drawing function takes its GCs as parameters; it never
creates one.  The owners are the widgets: `XmPrimitive` allocates
`top_shadow_GC`, `bottom_shadow_GC` and `highlight_GC` in its
`initialize` from the widget's colours and pixmaps, through
`XtGetGC`/`XtAllocateGC`, which share one server-side GC among all
widgets on the screen that ask for the same values.  A Motif
application with a thousand buttons of the same colours therefore
holds three or four GCs for all of them, and the per-widget state is a
few pointers.  The SpinBox of chapter 4 shows the allocation idiom,
including the "unused mask" that tells Xt which GC fields the widget
promises not to care about, so that the GC can be shared with widgets
that set those fields differently.

The cost of this sharing is that a widget must never modify a shared
GC (the dashed-highlight hack above is the one place that does, and
restores it), and that colour changes mean releasing and re-acquiring
GCs in `set_values`, which the `XmNbackground` change handling in
Primitive does.

## 2.6 Threading and re-entrancy

Each public function takes the application lock (`_XmAppLock`) for the
duration of its requests, and the ones with static buffers take the
process lock inside.  Both are the no-op-unless-threaded macros of
chapter 1 §1.7.  The functions are not re-entrant with respect to the
static buffers even in a single thread: `XmeDrawArrow` calls itself for
thickness 1 *after* its own `XFillRectangles` calls, which is safe only
because the recursive call is made with `shadow_thick == 0` and the
rectangle lists are regenerated before use.  This is the kind of
invariant that a comment, not the type system, protects, and it is
worth knowing before changing the order of operations in that file.

## 2.7 Design assessment

| Decision | For | Against |
|----------|-----|---------|
| Segments, two requests per shadow | Minimum requests for two colours; exact mitres; constant client cost per segment | 4t elements per shadow versus 4 for an unmitred rectangle approach |
| Static grown buffers | No per-call allocation; tiny | Process-wide state; needs a lock; never shrinks |
| Thickness clamping | No overdraw on small widgets | Odd etched thicknesses lose a pixel |
| GC swapping for IN/OUT | One code path | A reader must remember the swap when reasoning about colours |
| Special cases for t < 4 separators | Pixel-exact default look | Two algorithms to maintain |
| Caller-owned GCs | Server-side sharing; no GC churn in draw calls | Any GC change is the caller's problem; the dashed highlight breaks the rule |

What the module does *not* do is as telling as what it does: no
double buffering, no anti-aliasing, no transparency.  Those belong to
the era of Xft and the RENDER extension, which Motif uses only for
text (chapter 5); the 3-D look is drawn with the core protocol so that
it works on every X server, including one from 1988.

## References

- X protocol requests used: `PolySegment`, `PolyLine`,
  `PolyFillRectangle`, `ClearArea`, `ChangeGC`; Xlib manual chapter 8
  "Graphics Functions": <https://www.x.org/releases/current/doc/libX11/libX11/libX11.html#Graphics_Functions>
- [`src/lib/Xm/Draw.c`](../../src/lib/Xm/Draw.c),
  [`DrArrow.c`](../../src/lib/Xm/DrArrow.c),
  [`DrHiDash.c`](../../src/lib/Xm/DrHiDash.c),
  [`DrPoly.c`](../../src/lib/Xm/DrPoly.c),
  [`DrTog.c`](../../src/lib/Xm/DrTog.c),
  [`Region.c`](../../src/lib/Xm/Region.c),
  [`DrawP.h`](../../src/lib/Xm/DrawP.h) (the public `Xme*` prototypes)
- Manual pages `XmeDrawShadows(3)`, `XmeDrawHighlight(3)`,
  `XmeDrawSeparator(3)`, `XmeDrawArrow(3)`, `XmeClearBorder(3)` in
  [`doc/man/man3`](../man/man3)
- Benchmarks: `xmbench shadow-2 shadow-8`
  ([`src/tests/bench/xmbench.c`](../../src/tests/bench/xmbench.c))
