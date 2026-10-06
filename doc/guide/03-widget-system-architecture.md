# 3. Widget System Architecture

**Scope.** Chapter 1.1 described a widget as Xt sees it.  This chapter
describes what Motif builds on that: the three base classes
(`XmPrimitive`, `XmManager`, `XmGadget`) and the division of labour
between them; how a manager lays out its children and negotiates
geometry with its own parent; how events reach windowless gadgets; and
the keyboard model, that is, focus, traversal, tab groups and virtual
keys.  Three case studies follow in chapters 3.1 (subclassing), 3.2
(the Form constraint solver) and 3.3 (the Container's tree and grid
layouts), and chapter 4 walks through one complete widget.

---

## 3.1 Three kinds of children

Every Motif child of a container is one of:

| Kind | Base class | X window | Children | Typical classes |
|------|-----------|----------|----------|-----------------|
| Primitive widget | `XmPrimitive` (Core) | own | none | Label, PushButton, Text, List, ScrollBar |
| Gadget | `XmGadget` (RectObj) | none, drawn in the parent's | none | LabelGadget, PushButtonGadget, IconGadget |
| Manager widget | `XmManager` (Constraint) | own | widgets and gadgets | Form, RowColumn, BulletinBoard, Container |

The distinction that matters is the X window.  A window is a server
resource: creating it is a request, mapping it another, and every
resize, expose and crossing is an event the server generates and the
client must process.  A dialog of two hundred buttons as *widgets*
is two hundred windows; as *gadgets* it is one window, the parent's.
The `rc-buttons` and `rc-gadgets` cases of `xmbench` measure the
difference for a RowColumn of PushButtons versus PushButtonGadgets.
Gadgets were introduced in Motif 1.0 for exactly this reason, when
servers had a few megabytes of memory and a window cost hundreds of
bytes plus a backing-store allocation; the cost model is different
today, but the dispatch machinery of §3.4 remains.

### 3.1.1 XmPrimitive

[`Primitive.c`](../../src/lib/Xm/Primitive.c) adds to `Core` the state
that every visible Motif control shares:

- the colours and GCs: `foreground`, `top_shadow_color`,
  `bottom_shadow_color`, `highlight_color`, their pixmaps, and the
  `top_shadow_GC`, `bottom_shadow_GC`, `highlight_GC` allocated in
  `Initialize` and released in `Destroy` (chapter 2 §2.5);
- `shadow_thickness` and `highlight_thickness`, defaulted by
  `_XmSetThickness` (chapter 1.3);
- the traversal state: `traversal_on`, `navigation_type`,
  `highlight_on_enter`, `highlighted`, `highlight_drawn`, `have_traversal`;
- `unit_type` (chapter 1.3) and `layout_direction`;
- `help_callback`, `popup_handler_callback`, `convert_callback` (the
  Uniform Transfer Model), and the tool-tip string.

Its class part adds two inherited methods, `border_highlight` and
`border_unhighlight`, which draw and erase the focus rectangle:

```c
/* src/lib/Xm/Primitive.c */
static void HighlightBorder(Widget w)
{
  XmPrimitiveWidget pw = (XmPrimitiveWidget)w;
  pw->primitive.highlighted = True;
  pw->primitive.highlight_drawn = True;
  if (XtWidth(pw) == 0 || XtHeight(pw) == 0 || pw->primitive.highlight_thickness == 0)
    return;
  XmeDrawHighlight(XtDisplay(pw), XtWindow(pw), pw->primitive.highlight_GC,
                   0, 0, XtWidth(pw), XtHeight(pw), pw->primitive.highlight_thickness);
}
```

and `arm_and_activate`, the action a parent invokes when the user
presses Return on a dialog's default button.  Its action table
(`PrimitiveFocusIn`, `PrimitiveFocusOut`, `PrimitiveEnter`,
`PrimitiveLeave`, `PrimitiveTraverseLeft`, ...) and its default
translations are *augmented* into every subclass's translations at
class initialisation "IFF traversal is on", so that a subclass author
never has to bind the focus and traversal keys.

Primitive installs four traits on its class (`accessColors`,
`careParentVisual`, `specifyLayoutDirection`, `specifyUnitType`) and,
through the base class extension (chapter 1.2), the inherited
`widgetNavigable` and `focusChange` methods of §3.5.

### 3.1.2 XmManager

[`Manager.c`](../../src/lib/Xm/Manager.c) is the base of every
container.  Its instance part holds the same colours and GCs as
Primitive (so that it can draw shadows around itself and *for its
gadgets*), the focus bookkeeping (`highlighted_widget`,
`active_child`, `has_focus`, `keyboard_list`), `traversal_on`,
`navigation_type`, and `initial_focus`.  Its class record declares a
constraint record of its own (`XmManagerConstraintRec`, the
`XmNpositionIndex`-related data) and leaves `geometry_manager` and
`change_managed` `NULL` for subclasses to supply.  Two additions to
the Xt machinery are Manager's:

- **`parent_process`.**  A child that receives an event it cannot
  handle alone, such as Return in a text field inside a dialog, calls
  `_XmParentProcess`, which walks up the parents calling each
  manager's `parent_process` method with an `XmParentProcessData`
  describing the event.  This is how a dialog's *default button*
  activates on Return anywhere in the dialog and how Escape cancels
  it, without the child knowing it is in a dialog.
- **Gadget dispatch**, §3.4.

Manager also supplies the `ObjectAtPoint` extension method that
`XmObjectAtPoint` calls, and the `WidgetNavigable` method that decides
how keyboard traversal enters it.

### 3.1.3 XmGadget

[`Gadget.c`](../../src/lib/Xm/Gadget.c) descends from Xt's `RectObj`
(an object with geometry but no window) and holds what a Primitive
holds minus the window-related state, plus an `event_mask` naming the
events it wants (`XmENTER_EVENT`, `XmLEAVE_EVENT`, `XmMOTION_EVENT`,
`XmARM_EVENT`, `XmACTIVATE_EVENT`, `XmFOCUS_IN_EVENT`, ...).  A gadget
cannot have event handlers, translations or a cursor; everything it
sees is forwarded by its parent through `input_dispatch`, a class
method that replaces the translation manager.  Its drawing goes into
the parent's window with the parent's GCs or its own, which is why the
label gadgets keep their fonts and colours in the shared *cache
objects* of chapter 1.2: a gadget's instance record is about a third
the size of the widget's.

## 3.2 Geometry management

Xt's protocol between a parent and a child has five methods and a
three-valued answer:

1. A child that wants to change size calls `XtMakeGeometryRequest`
   (or `XtMakeResizeRequest`); Xt calls the parent's
   `geometry_manager`, which returns `XtGeometryYes` (done),
   `XtGeometryNo` (refused), or `XtGeometryAlmost` with a counter-offer
   that the child may accept by asking again.
2. A parent that wants to know a child's preferred size calls
   `XtQueryGeometry`, which calls the child's `query_geometry`.
3. When children are managed or unmanaged, Xt calls the parent's
   `change_managed`; the parent lays out and may ask *its* parent for
   a new size.
4. When the parent is resized by its parent, Xt calls `resize`.
5. The parent positions a child with `XtConfigureWidget`, which
   generates the child's `resize` and the X `ConfigureWindow`.

Motif wraps each step.  `XmeReplyToQueryGeometry` implements the
`query_geometry` answer protocol (compare the intended with the
preferred geometry, return Yes/No/Almost correctly); `XmeConfigureObject`
wraps `XtConfigureWidget` with a drop-site update bracket, so that
drop sites registered on moved children stay correct:

```c
/* src/lib/Xm/GadgetUtil.c */
void XmeConfigureObject(Widget wid, Position x, Position y, Dimension width, Dimension height,
                        Dimension border_width)
{
  _XmWidgetToAppContext(wid);
  XmDropSiteStartUpdate(wid);
  _XmAppLock(app);
  if (!width && !height) {
    XtWidgetGeometry desired, preferred;
    desired.request_mode = 0;
    XtQueryGeometry(wid, &desired, &preferred);
    width = preferred.width;
    height = preferred.height;
  }
  if (!width) width++;
  if (!height) height++;
  XtConfigureWidget(wid, x, y, width, height, border_width);
  XmDropSiteEndUpdate(wid);
  _XmAppUnlock(app);
}
```

The `width++` lines encode an X rule that every layout in Motif must
respect: a window of zero width or height is a protocol error
(`BadValue`), so a widget is never configured smaller than 1 × 1.

### 3.2.1 The GeoMatrix engine

The dialogs (`XmBulletinBoard` and its subclasses `XmMessageBox`,
`XmSelectionBox`, `XmFileSelectionBox`, `XmCommand`) share a layout
engine in [`GeoUtils.c`](../../src/lib/Xm/GeoUtils.c), the *GeoMatrix*:
a matrix of "boxes" (`XmKidGeometry`, one per child) arranged in rows,
with per-row fill and stretch policies.  A dialog class supplies a
*create matrix* procedure that places its children in rows (message
row, separator row, button row), and two generic entry points do the
rest:

- `_XmHandleQueryGeometry` builds the matrix, computes the preferred
  size with `_XmGeoMatrixGet` and `_XmGeoArrangeBoxes`, applies the
  resize policy (`XmRESIZE_NONE`: keep the current size;
  `XmRESIZE_GROW`: never shrink; `XmRESIZE_ANY`), and replies through
  `XmeReplyToQueryGeometry`.
- `_XmHandleGeometryManager` handles a child's request by building the
  matrix with the child at its requested size, computing the result,
  and asking the parent in turn.  It keeps a *cache* of the last
  computed matrix per widget (`XmGeoMatrix *cachePtr`) so that the
  `XtGeometryAlmost` dance, in which the child asks, is offered a
  compromise, and asks again with the compromise, does not recompute
  the layout: "This is a successive geometry request which matches the
  cached geometry record" is answered from the cache.

The arrangement algorithms are O(n) per row in the number of boxes,
with one `qsort` by box width (`boxWidthCompare`, O(k log k)) in the
averaging fit, and the separator and menu-bar rows get fix-ups
(`_XmSeparatorFix`, `_XmMenuBarFix`) that stretch them to the dialog
width.  Form (chapter 3.2) and RowColumn (`RCLayout.c`) have their own
engines; the GeoMatrix is for layouts whose row structure is fixed by
the class.

## 3.3 Event flow

An X event arrives at Xt, which finds the widget by window
(`XtWindowToWidget`, a hash lookup per display) and dispatches through
the widget's *translation table* to an *action* (for example
`<Btn1Down>: Arm()` in a PushButton) or to an *event handler*.  Motif
keeps three conventions on top of that:

- **Actions are the unit of behaviour.**  Every user-visible thing a
  widget does is an action with a name (`Activate()`,
  `ListBeginSelect()`, `SpinBNext()`), so that users can rebind it in a
  resource file and so that the virtual key layer (§3.6) can describe
  behaviour in terms of keys, not keysyms.
- **Callbacks are the application interface.**  An action does its
  work and then calls a callback list (`XmNactivateCallback`,
  `XmNvalueChangedCallback`) with a reason code and the event, through
  `XtCallCallbackList`; the `XmAnyCallbackStruct` prefix `{reason,
  event}` is common to all of them.  A *verify* callback
  (`XmNmodifyVerifyCallback`, `XmNvalidateCallback`) is called *before*
  the change with a `doit`/`accept` flag the application may clear,
  which is how DataField's picture check works (chapter 3.1).
- **Compression.**  Motif classes set `compress_motion`,
  `compress_enterleave` and `XtExposeCompressMaximal` so that Xt
  delivers one event for a burst; the drag-and-drop motion buffer
  (chapter 6.2) and the List's `XtExposeGraphicsExpose` requirement
  (chapter 6.1) are two places where a class needs *more* events than
  the default and says so in its class record.

## 3.4 Dispatching events to gadgets

Because gadgets have no window, the X server knows nothing about them:
every event in a gadget's area is delivered to the manager's window.
The manager's own actions and event handlers therefore hit-test and
forward.  The motion case from [`Manager.c`](../../src/lib/Xm/Manager.c):

```c
static void ManagerMotion(...)
{
  ...
  if (event->xmotion.subwindow != 0 || !mw->manager.has_focus)
    return;
  gadget = _XmInputForGadget((Widget)mw, event->xmotion.x, event->xmotion.y);
  oldGadget = (XmGadget)mw->manager.highlighted_widget;
  /*  Dispatch motion events to the child  */
  if (gadget != NULL) {
    if (gadget->gadget.event_mask & XmMOTION_EVENT)
      _XmDispatchGadgetInput((Widget)gadget, event, XmMOTION_EVENT);
  }
  /*  Check for and process a leave window condition  */
  if (oldGadget != NULL && gadget != oldGadget) {
    if (oldGadget->gadget.event_mask & XmLEAVE_EVENT)
      _XmDispatchGadgetInput((Widget)oldGadget, event, XmLEAVE_EVENT);
    mw->manager.highlighted_widget = NULL;
  }
  /*  Check for and process an enter window condition  */
  if (gadget != NULL && gadget != oldGadget) {
    if (gadget->gadget.event_mask & XmENTER_EVENT) {
      _XmDispatchGadgetInput((Widget)gadget, event, XmENTER_EVENT);
      mw->manager.highlighted_widget = (Widget)gadget;
    }
    else
      mw->manager.highlighted_widget = NULL;
  }
}
```

The server generates `EnterNotify`/`LeaveNotify` for windows; for
gadgets the manager *synthesises* them from motion by remembering
which gadget the pointer was in last (`highlighted_widget`).  The hit
test is a reverse linear scan of the children:

```c
static Widget ObjectAtPoint(Widget wid, Position x, Position y)
{
  CompositeWidget cw = (CompositeWidget)wid;
  int i;
  Widget widget;
  /* For the case of overlapping gadgets, the last one in the
   * composite list will be the visible gadget (see order of
   * redisplay in XmeRedisplayGadgets).  So, search the child
   * list from the tail to the head to get this visible gadget
   * as the one to get the input.
   */
  i = cw->composite.num_children;
  while (i--) {
    widget = cw->composite.children[i];
    if (XmIsGadget(widget) && XtIsManaged(widget)) {
      if (x >= widget->core.x && y >= widget->core.y && x < widget->core.x + widget->core.width &&
          y < widget->core.y + widget->core.height)
        return (widget);
    }
  }
  return (NULL);
}
```

O(n) per motion event in the number of children, with `XmIsGadget`
being the O(1) fast-subclass test of chapter 1.2.  For the RowColumns
and menus that gadgets are used in, n is tens; a manager with
thousands of gadgets would want a spatial index, and the Container
(chapter 3.3), which can hold thousands of IconGadgets, supplies its
own `object_at_point` through the manager class extension, which is
why the method is an extension slot rather than hard-coded.  Exposure
works the other way round: `XmeRedisplayGadgets`, called from a
manager's `expose`, walks the children and redraws every gadget
intersecting the exposed region, in child order, so that later
children paint over earlier ones, consistent with the hit test.

## 3.5 Keyboard focus and traversal

Motif's keyboard model, specified by the Style Guide after
Presentation Manager, has three layers.

**Focus policy.**  `XmNkeyboardFocusPolicy` on the shell is
`XmEXPLICIT` (focus moves by Tab and arrow keys, the default) or
`XmPOINTER` (focus follows the mouse).  Under `XmEXPLICIT`, Motif
manages focus itself inside each shell: the shell's window has the X
focus, and `Traversal.c` decides which descendant receives key events
(`XmGetFocusWidget`), redirecting them with the `focusChange` and
`XmNfocusCallback` machinery.  Gadgets can have "focus" only this
way, since they have no window to give the X focus to.

**Tab groups.**  Every widget has `XmNnavigationType`:

```c
/* src/lib/Xm/Xm.h.in */
enum { XmNONE, XmTAB_GROUP, XmSTICKY_TAB_GROUP, XmEXCLUSIVE_TAB_GROUP };
```

Tab and Shift-Tab (`osfNextField`/`osfPrevField` in the translations,
`XmTRAVERSE_NEXT_TAB_GROUP`/`PREV_TAB_GROUP` in the code) move between
tab groups; the arrow keys (`XmTRAVERSE_UP/DOWN/LEFT/RIGHT`) move
within one; `osfBeginLine`-style keys give `XmTRAVERSE_HOME`.
Managers decide how traversal enters them with the inherited
`widgetNavigable` method:

```c
/* src/lib/Xm/Manager.c */
static XmNavigability WidgetNavigable(Widget wid)
{
  if (XtIsSensitive(wid) && ((XmManagerWidget)wid)->manager.traversal_on) {
    XmNavigationType nav_type = ((XmManagerWidget)wid)->manager.navigation_type;
    if ((nav_type == XmSTICKY_TAB_GROUP) || (nav_type == XmEXCLUSIVE_TAB_GROUP) ||
        ((nav_type == XmTAB_GROUP) && !_XmShellIsExclusive(wid)))
      return XmDESCENDANTS_TAB_NAVIGABLE;
    return XmDESCENDANTS_NAVIGABLE;
  }
  return XmNOT_NAVIGABLE;
}
```

`XmProcessTraversal(w, direction)` is the public entry: it builds the
list of traversable widgets in the shell (sensitive, managed, mapped,
`traversal_on`, navigable per the method above, and not obscured, see
§3.5.1), finds the current focus in it, and moves.  Directional
traversal (`XmTRAVERSE_LEFT` and the like) picks the geometrically
nearest candidate in that direction; the comparisons are O(n) in the
number of traversable widgets of the shell per key press.  DataField's
`ValidateAndMove` (chapter 3.1) and SpinBox's `SpinBAction` (chapter 4)
both end in a call to it.

### 3.5.1 Visibility

Traversal must not move the focus to a widget the user cannot see, and
`XmGetVisibility` answers whether a widget is unobscured, partially or
fully obscured by its ancestors' clipping or by siblings stacked above
it:

```c
/* src/lib/Xm/Traversal.c */
XmVisibility XmGetVisibility(Widget wid)
{
  ...
  if (!wid || !_XmComputeVisibilityRect(wid, &rect, FALSE, TRUE))
    return (XmVISIBILITY_FULLY_OBSCURED);          /* clipped away by an ancestor */
  if ((rect.width != XtWidth(wid)) || (rect.height != XtHeight(wid)))
    return (XmVISIBILITY_PARTIALLY_OBSCURED);
  /* Obscurity by siblings */
  if (!(parent_window = XtWindow(XtParent(wid))) ||
      XQueryTree(XtDisplay(wid), parent_window, &rootwindow, &p_window, &children, &numchildren) == 0)
    return (XmVISIBILITY_UNOBSCURED);
  /* walk through those which are under the window of interest */
  for (i = 0; (unsigned int)i < numchildren; i++)
    if (children[i] == XtWindow(wid)) break;
  i++;
  /* process windows above the window of interest */
  if ((unsigned int)i < numchildren) {
    Region region = XCreateRegion(), tmp_region = XCreateRegion(), left_region = XCreateRegion();
    XUnionRectWithRegion(&rect, region, region);
    _XmSetRect(&parent_rect, XtParent(wid));
    while ((unsigned int)i < numchildren) {
      if (SiblingGeometry(XtParent(wid), children[i], &srcRectB)) {
        srcRectB.x += parent_rect.x;
        srcRectB.y += parent_rect.y;
        if (_XmIntersectionOf(&rect, &srcRectB, &intersect_rect))
          XUnionRectWithRegion(&intersect_rect, tmp_region, tmp_region);
      }
      i++;
    }
    XSubtractRegion(region, tmp_region, left_region);
    value = XEqualRegion(region, left_region) ? XmVISIBILITY_UNOBSCURED
          : XEmptyRegion(left_region) ? XmVISIBILITY_FULLY_OBSCURED
          : XmVISIBILITY_PARTIALLY_OBSCURED;
    ...
```

The algorithm is a region subtraction: the union of the intersections
of the widget's rectangle with every sibling window stacked above it,
subtracted from the widget's rectangle.  `XQueryTree` is one round
trip; the original code then made one `XGetWindowAttributes` round
trip *per sibling above* to get its geometry and map state, O(k) round
trips for k siblings.  This tree's `SiblingGeometry` answers from Xt's
own records when the sibling window belongs to a realized child of the
same parent ("Xt knows the geometry, and a realized widget that is
mapped when managed is mapped exactly when it is managed"), and only
falls back to the server for foreign windows.  The `visibility` case
of `xmbench` ("XmGetVisibility with 50 siblings above") measures the
change, and the [CHANGELOG](../../CHANGELOG.md) records it under
"fewer X requests".  Region operations are O(number of rectangles) per
union, so the client-side cost is O(k) with a small constant; the
round trips were the cost that mattered.

## 3.6 Virtual keys

The translations of every Motif class are written against *virtual
keysyms*: `<Key>osfUp`, `<Key>osfActivate`, `<Key>osfCancel`,
`<Key>osfBackSpace`.  None of these exists in the X server's keyboard
map.  [`VirtKeys.c`](../../src/lib/Xm/VirtKeys.c) installs Motif's own
key translator on each display:

```c
/* src/lib/Xm/VirtKeys.c */
  XtSetKeyTranslator(dpy, (XtKeyProc)XmTranslateKey);
```

`XmTranslateKey` is called by Xt for every key event to turn a keycode
and modifier state into a keysym.  It first consults a per-display
table of *virtual bindings* (`osfBackSpace: <Key>BackSpace`,
`osfCancel: <Key>Escape`, ...) and returns the virtual keysym when one
matches, otherwise the physical one.  The bindings come, in order of
priority, from the `_MOTIF_BINDINGS` property on the root window
(installed by `xmbind` from the user's `~/.motifbind` or the vendor
files in `data/bindings`), from the `XmNdefaultVirtualBindings`
resource, and from a compiled-in fallback string.  The 47 virtual keys
are listed in `VirtKeys.c` (`XmVosfActivate` ... `XmVosfUp`, a few of
them marked defunct).

The design separates *what a key means* from *which key it is*, which
is what made the CUA key semantics portable across the keyboards of
seven vendors in 1989 and still lets a user swap Delete and BackSpace
for every Motif program at once.  Its cost is a table lookup per key
event (the table is small and sorted) and the surprise, for newcomers,
that a translation written with a physical keysym may be shadowed by a
virtual one.  The manual page `VirtualBindings(3)` documents the
format.

## 3.7 Summary of trade-offs

| Decision | Benefit | Cost |
|----------|---------|------|
| Gadgets | One window per container instead of one per control; cheaper creation and exposure | The manager must hit-test, synthesise enter/leave and draw for them; no per-gadget cursors, colormaps or event handlers; cache objects for shared state |
| `parent_process` | Default button and cancel work for any child in any dialog | Another chained-by-hand protocol outside Xt |
| GeoMatrix with a cache | One layout engine for every dialog; the Almost protocol does not recompute | Rows are fixed per class; a new dialog type writes a matrix builder |
| Explicit focus policy in the toolkit | Focus for gadgets; consistent traversal; `XmNinitialFocus` | The toolkit, not the server, owns focus inside a shell; subtle interactions with input methods and grabs |
| Virtual keys | Keyboard-independent behaviour, user-rebindable | A translator on every key event; two namespaces of keysyms |

## References

- Xt specification, chapters 6 "Geometry Management", 7 "Event
  Management" and 10 "Translation Management":
  <https://www.x.org/releases/current/doc/libXt/intrinsics.html>
- [`src/lib/Xm/Primitive.c`](../../src/lib/Xm/Primitive.c),
  [`Manager.c`](../../src/lib/Xm/Manager.c), [`Gadget.c`](../../src/lib/Xm/Gadget.c),
  [`GadgetUtil.c`](../../src/lib/Xm/GadgetUtil.c), [`GeoUtils.c`](../../src/lib/Xm/GeoUtils.c),
  [`Traversal.c`](../../src/lib/Xm/Traversal.c), [`TravAct.c`](../../src/lib/Xm/TravAct.c),
  [`VirtKeys.c`](../../src/lib/Xm/VirtKeys.c), [`Xm.h.in`](../../src/lib/Xm/Xm.h.in)
- Manual pages `XmPrimitive(3)`, `XmManager(3)`, `XmGadget(3)`,
  `XmProcessTraversal(3)`, `XmGetVisibility(3)`, `VirtualBindings(3)`,
  `xmbind(1)` in [`doc/man`](../man)
- [`data/bindings`](../../data/bindings), the vendor virtual binding files
