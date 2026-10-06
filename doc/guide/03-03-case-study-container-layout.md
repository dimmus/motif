# 3.3 Case Study: XmContainer, Tree and Grid Layouts

**Scope.** `XmContainer` ([`Container.c`](../../src/lib/Xm/Container.c),
about 9 000 lines and 173 functions, the largest widget in the
library) is the file-manager widget of Motif 2.0: it displays
`XmIconGadget` children as an outline (a tree with expand/collapse
buttons), as a detail view (outline plus columns under a header), or
spatially (free placement, a grid, or cells with snapping), with
selection, drag and drop and keyboard navigation.  This chapter looks
at two of its data structures and algorithms: the outline tree and its
O(1) append path, and the spatial cell placement.  It is also an
example of how a Motif 2.0 widget uses *traits* to talk to its
children.

---

## 3.3.1 Shape of the widget

The constants at the top of the file describe its vocabulary:

```c
#define DEFAULT_INDENTATION 40     /* pixels per outline level */
#define NO_CELL -1                 /* spatial: not placed */
#define OBNAME "OutlineButton"     /* the expand/collapse PushButtonGadgets it creates */
#define HEADERNAME "Header"        /* the detail header row */
#define INVALID_COUNT 32767        /* "recompute me" markers */
#define INVALID_DIMENSION 32767
#define MOTION_THRESHOLD 3         /* pixels before a press becomes a drag */
#define DRAG_STATE_SIZE 14
enum { ANY_FIT, EXACT_FIT, FORCE };   /* spatial placement modes, §3.3.3 */
#define _LEFT 0 ... #define _LAST 5   /* directional navigation */
#define _COLLAPSE 2 / _EXPAND 3
```

Children are `XmIconGadget`s or any object that implements the
`XmQTcontainerItem` trait; the Container itself implements
`XmQTcontainer`, through which items ask for their visual attributes
(colours, outline state) and report changes.  A child's constraint
record (`XmContainerConstraintRec`) holds `entry_parent` (the parent
in the outline tree), `position_index`, `outline_state`
(expanded/collapsed), the spatial cell index, the user-requested x/y,
and a pointer to a *node*.

## 3.3.2 The outline tree

Outline and detail layouts are tree layouts.  The tree is separate
from the Xt child list, because the Xt list is flat and ordered by
creation, while the outline order is by `XmNentryParent` and
`XmNpositionIndex`:

```c
typedef struct _XmCwidNodeRec {
  Widget widget_ptr;           /* node ----> widget */
  struct _XmCwidNodeRec *parent_ptr, *child_ptr, *prev_ptr, *next_ptr;
} XmCwidNodeRec, *CwidNode;      /* and the constraint has node_ptr: widget --> node */
```

a classic first-child/next-sibling tree with doubly linked siblings,
one node per child, and `cw->container.first_node` as the root list.
The *positions* must satisfy an invariant that a comment states
precisely: among the children with the same `XmNentryParent`, the
`XmNpositionIndex` values are 0, 1, 2, ... with no gaps and no
duplicates.  `InsertNode` maintains it when a child is created or
re-parented: find the level, find the slot by position, splice the
node in, renumber what follows.

### The append fast path

Walking a level to find the insertion point is O(k) for a level of k
siblings, so inserting n children in order into one level is O(n²),
which a file manager listing a directory of ten thousand files feels.
Children are almost always appended (created in order, with
`XmNpositionIndex` at its default `XmLAST_POSITION`).  This tree adds
a constant-time path for that case:

```c
/* FindLevelTail (Private Function)
 *	The last node of the level below parent_node (the top level when
 *	NULL), found from the most recently created child without walking
 *	the level, or NULL.
 */
static CwidNode FindLevelTail(XmContainerWidget cw, CwidNode node, CwidNode parent_node)
{
  Cardinal i, last = cw->composite.num_children;
  Widget kid = NULL;
  CwidNode n;
  /* look at a few children only: there may be many outline buttons */
  for (i = last; (kid == NULL) && (i > 0) && (last - i < 8); i--) {
    if ((cw->composite.children[i - 1] != node->widget_ptr) &&
        CtrICON(cw->composite.children[i - 1]))
      kid = cw->composite.children[i - 1];
  }
  if (kid == NULL)
    return NULL;
  /* Children are usually added in order, so the last one created, or
   * its ancestor on the level we insert into, ends that level. */
  for (n = GetContainerConstraint(kid)->node_ptr; n != NULL; n = n->parent_ptr) {
    if (n == node)
      return NULL;
    if (n->parent_ptr == parent_node)
      return (n->next_ptr == NULL) ? n : NULL;
  }
  return NULL;
}
```

and in `InsertNode`:

```c
  /*
   * Appending: link the node after the last one of its level directly.
   * The loop below would also leave the indices of the others as they
   * are, since they are already 0, 1, 2...; except that a lone node
   * may have been given any XmNpositionIndex (see ConstraintSetValues).
   */
  if ((prev_node != NULL) && !CtrIsDynamic(cw, STALE_POSITIONS) &&
      ((next_node = FindLevelTail(cw, node, parent_node)) != NULL))
  {
    sc = GetContainerConstraint(next_node->widget_ptr);
    if ((c->position_index == XmLAST_POSITION) || (c->position_index > sc->position_index)) {
      if (next_node->prev_ptr == NULL)
        sc->position_index = 0;
      c->position_index = sc->position_index + 1;
      node->parent_ptr = parent_node;
      node->prev_ptr = next_node;
      node->next_ptr = NULL;
      next_node->next_ptr = node;
      ...
      return;
    }
  }
  /* otherwise the general O(k) walk */
```

The heuristic: the most recently created icon child (looked for among
the last eight Xt children, because the Container also creates its own
outline-button gadgets, which are not icons) is, or has an ancestor
that is, the tail of the level being inserted into.  If the ancestor
walk confirms that (`n->parent_ptr == parent_node` and `n->next_ptr ==
NULL`), the new node is linked after it and numbered `tail + 1`, with
no walk and no renumbering, because the invariant guarantees that the
existing indices are already 0..k−1.  If anything is unusual (the
level is empty, the last child is elsewhere, positions were marked
stale by a `set_values`, the requested position is not past the tail),
the function returns `NULL` and the general path runs.

Cost: O(min(8, children)) plus O(depth) per append, O(n) for n appends
instead of O(n²); the general path and its results are untouched, so a
non-append insert behaves exactly as before.  `xmbench container-icons`
("Container of IconGadgets: create + manage") measures it, and the
`container` mode of the A/B harness compared child geometry,
selection, callbacks and pixels with the old code.

### Layout of the tree

`ChangeManagedOutlineDetail` walks the tree depth-first, skipping the
subtrees of collapsed nodes, placing each visible item at
`x = level × XmNoutlineIndentation` and `y` after the previous one,
creating or positioning an outline button next to items that have
children, and, in detail mode, laying the detail columns of each item
under the header's tab stops (`XmNdetailTabList`, a `XmTabList` from
the render-table machinery of chapter 5).  The walk is O(visible
items); the Container stores the widths it needs per column with
`INVALID_DIMENSION` markers so that a column is remeasured only when
an item in it changes.

## 3.3.3 Spatial layouts: cells and placement

In `XmSPATIAL` layout the Container is a grid of cells
(`XmNspatialStyle`: `XmNONE` for free placement, `XmGRID`, `XmCELLS`),
each `XmNlargeCellWidth` × `XmNlargeCellHeight` (or the small-icon
sizes).  The occupancy is an array `cw->container.cells[cell_count]`
of counts, plus `next_free_cell`, plus, in `XmCELLS` style, an X
`Region` of the rectangles already occupied so that items of
different sizes can be tested for overlap.

```c
static void PlaceItemGridCells(Widget wid, Widget cwid, unsigned char fit_type)
{
  ...
  if (CtrIncludeIsAPPEND(cw))
    trial_cell = cw->container.next_free_cell;
  if (CtrIncludeIsCLOSEST(cw)) {
    closest_cell = GetCellFromCoord(wid, c->user_x, c->user_y);
    trial_cell = closest_cell;
    ...                                  /* clamp to the current grid */
  }
  start_cell = trial_cell;
  if (trial_cell < cw->container.cell_count) {
    while (!CtrItemIsPlaced(cwid)) {
      fits = False;
      if ((cw->container.cells[trial_cell] == 0) && (fit_type != FORCE)) {
        if (CtrSpatialStyleIsGRID(cw))
          fits = True;
        if (CtrSpatialStyleIsCELLS(cw)) {
          ...                            /* snap the item to the cell, then */
          if ((XRectInRegion(cw->container.cells_region, place_point.x, place_point.y,
                             cwid->core.width, cwid->core.height) == RectangleOut) &&
              (place_point.x + cwid->core.width <= cw->core.width - cw->container.margin_w) &&
              (place_point.y + cwid->core.height <= cw->core.height - cw->container.margin_h))
            fits = True;
        }
      }
      if (fits || (fit_type == FORCE)) {
        cw->container.cells[trial_cell]++;
        c->cell_idx = trial_cell;
      }
      trial_cell++;
      if (trial_cell == cw->container.cell_count)
        trial_cell = 0;
      if (start_cell == trial_cell)
        break;
    }
  }
```

The placement is a circular linear probe over the cells, starting at
`next_free_cell` (`XmNspatialIncludeModel` `XmAPPEND`), at the cell
nearest the requested coordinates (`XmCLOSEST`), or at the item's own
position (`XmFIRST_FIT` with `PlaceItemNone`).  A cell "fits" when it
is empty (grid style) or when the item, snapped to the cell, does not
intersect the region of placed items and stays inside the margins
(cells style).  The three `fit_type` modes are the policy of the
caller: `ANY_FIT` takes the first fit, `EXACT_FIT` accepts only the
requested cell (used when the user drops an item), and `FORCE` places
even over occupied cells (when the container is told to lay out
something that must appear).  If the grid is full, `RequestSpatialGrowth`
asks the Container's parent for more room and the grid grows by
rows or columns according to `XmNspatialResizeModel`.

Complexity: O(cells) per placement in the worst case, O(1) typical for
appends because `next_free_cell` is maintained; the region test is
O(rectangles in the region) per probe, which is the price of allowing
items larger than a cell.  `GetCellFromCoord`/`GetCoordFromCell` are
the O(1) index arithmetic between pixels and cells
(`cell = row × width_in_cells + column`).

## 3.3.4 Selection, navigation and drag

The rest of the file is behaviour: rubber-band and keyboard selection
under four policies (`XmNselectionPolicy`), with `XmNselectedObjects`
maintained as an array; directional navigation (`_LEFT`...`_LAST`)
that, in spatial layouts, picks the nearest item in a direction and in
outline layouts moves along the visible tree; expand/collapse through
the outline buttons or the keyboard (`_COLLAPSE`/`_EXPAND`); and the
drag source behaviour, where `MOTION_THRESHOLD` (3 pixels) decides
when a press becomes a drag so that a slightly unsteady click does not
start one, and `DRAG_STATE_SIZE` (14) sizes the table that tracks the
button and modifier state during it.  The selection code was
reworked in this tree so that a selection change no longer rescans
every item (the [CHANGELOG](../../CHANGELOG.md) states it for `XmList`;
the Container's selection arrays benefit from the same approach), and
the `TODO` notes that further Container data-structure work is
"blocked by installed struct layouts": the arrays in
`XmContainerPart` are visible to subclasses, so replacing them is an
ABI change.

## 3.3.5 Assessment

| Design | Benefit | Cost |
|--------|---------|------|
| Separate node tree with doubly linked siblings and a dense position index | Outline order independent of creation order; O(1) neighbour access; the `XmNpositionIndex` invariant makes a child's position a plain integer the application can read | Renumbering on insertion; two structures (Xt children, nodes) to keep consistent |
| Append fast path via the last-created child | O(n) directory listings | A heuristic with a correctness proof that depends on the invariant and the "stale positions" flag; eight-child window is a tuning constant |
| Cell array plus occupancy region | Constant-time grid placement; mixed-size items in cells style | Linear probing when the grid is nearly full; region operations per probe |
| Traits for items | Any object that implements `XmQTcontainerItem` can be a child; the Container never names `XmIconGadget` | Every attribute access is an indirect call through the trait record |
| One widget for outline, detail and three spatial styles | One API for file managers and icon views | 9 000 lines, 173 functions, many `CtrXxxIs...` state macros; the hardest file in the library to change |

## References

- [`src/lib/Xm/Container.c`](../../src/lib/Xm/Container.c),
  [`ContainerP.h`](../../src/lib/Xm/ContainerP.h),
  [`ContainerT.h`](../../src/lib/Xm/ContainerT.h),
  [`ContItemT.h`](../../src/lib/Xm/ContItemT.h),
  [`IconG.c`](../../src/lib/Xm/IconG.c)
- Manual pages `XmContainer(3)`, `XmIconGadget(3)`,
  `XmContainerGetItemChildren(3)`, `XmContainerReorder(3)` in
  [`doc/man/man3`](../man/man3)
- Example: `src/examples/programs/filemanager`
- `xmbench container-icons`; A/B harness mode `container`
  ([`src/tests/ab/README.md`](../../src/tests/ab/README.md))
