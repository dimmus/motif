# 4. A Complete Widget: XmSpinBox, Method by Method

**Scope.** Chapters 1 to 3 described the mechanisms; this chapter
reads one widget from top to bottom to show them working together.
`XmSpinBox` ([`SpinB.c`](../../src/lib/Xm/SpinB.c), about 2 300 lines)
is a manager that holds one or more text children and two arrows, and
steps a child's value through a numeric range or a list of strings.
It is small enough to read in a sitting and uses nearly everything:
constraint resources, synthetic resources, a type converter,
accelerators, actions, a timer, a trait it implements and a trait it
consumes, geometry negotiation, and the drawing primitives of chapter
2.  Every excerpt below is verbatim from the source in this tree; the
old version of this guide reproduced "typical" code for several of
these methods that did not match the file, and in particular described
the arrows as child `XmArrowButton` widgets.  They are not: the SpinBox
draws them itself.

---

## 4.1 What it is

```
┌──────────────────────────────┐
│ ┌────────────────────┐ ┌──┐  │   XmSpinBox (XmManager subclass)
│ │ 42                 │ │▲ │  │     child: XmTextField (or XmText, XmDataField, ...)
│ │                    │ │▼ │  │     up_arrow_rect, down_arrow_rect: drawn by the SpinBox
│ └────────────────────┘ └──┘  │     XmNarrowLayout: END (shown), BEGINNING, SPLIT, FLAT_*
└──────────────────────────────┘
```

The values live in the *child's constraint record*: a child is
`XmNUMERIC` (minimum, maximum, increment, decimal points) or
`XmSTRING` (an `XmStringTable` of values), and has a current
`XmNposition`.  The SpinBox itself owns the arrows, their layout, the
repeat timer and the callbacks.  Several children may be managed; the
one that has the focus (`spinBox.textw`) is the one the arrows act on.

## 4.2 Class-level setup

```c
static void ClassInitialize(void)
{
  spinAccel = XtParseAcceleratorTable(_XmSpinB_defaultAccelerators);
  /* set up base class extension quark */
  spinBoxBaseClassExtRec.record_type = XmQmotif;
}

static void ClassPartInitialize(WidgetClass classPart)
{
  XmSpinBoxWidgetClass spinC;
  spinC = (XmSpinBoxWidgetClass)classPart;
  _XmFastSubclassInit(classPart, XmSPINBOX_BIT);
  /* Install the navigator trait for all subclasses */
  XmeTraitSet((XtPointer)spinC, XmQTnavigator, (XtPointer)&spinBoxNT);
  XtSetTypeConverter(XmRString, XmRPositionValue, CvtStringToPositionValue,
                     selfConvertArgs, XtNumber(selfConvertArgs), XtCacheNone, (XtDestructor)NULL);
}
```

`ClassInitialize` runs once per process: it compiles the *accelerator*
table (not a translation table: accelerators are installed on *other*
widgets, here on the children, so that the arrow keys pressed in the
text field drive the SpinBox) and stamps the base class extension
with the Motif quark, which cannot be a static initialiser (chapter
1.2).  `ClassPartInitialize` runs for the class and for every
subclass: the fast-subclass bit, the navigator trait (so that a
ScrolledWindow or any navigator consumer can drive a SpinBox like a
scrollbar) and the converter for `XmNposition` values written as
strings in resource files (chapter 1.3).  The comment "for all
subclasses" is accurate because Xt chains this method onto subclass
records.

The accelerator string, from `Transltns.c`:

```c
_XmConst char _XmSpinB_defaultAccelerators[] = "\043override\n\
<Key>osfUp:         SpinBNext()\n\
<Key>osfDown:       SpinBPrior()\n\
<KeyUp>osfUp:       SpinBDisarm()\n\
<KeyUp>osfDown:     SpinBDisarm()\n\
<Key>osfLeft:       SpinBLeft()\n\
<Key>osfRight:      SpinBRight()\n\
<KeyUp>osfLeft:     SpinBDisarm()\n\
<KeyUp>osfRight:    SpinBDisarm()\n\
<Key>osfBeginLine:  SpinBFirst()\n\
<Key>osfEndLine:    SpinBLast()";
```

`\043override` is `#override`: these bindings take precedence over the
child's own.  The keys are virtual (chapter 3 §3.6), the actions are
the SpinBox's own (§4.6), and `<KeyUp>` bindings exist because holding
a key auto-repeats through the timer and releasing it must stop.

## 4.3 Resources and constraints

The widget resources (`arrow_layout`, `arrow_orientation`,
`arrow_size`, margins, `spacing`, `initial_delay` 250 ms,
`repeat_delay` 200 ms, `default_arrow_sensitivity`, the two
callbacks, `detail_shadow_thickness` defaulted by `_XmSetThickness`)
are plain `XtResource` entries addressed with
`XtOffsetOf(XmSpinBoxRec, spinBox.field)`.  The constraint resources
are declared the same way against `XmSpinBoxConstraintRec`:

```c
#define ConstraintOffset(field) XtOffsetOf(XmSpinBoxConstraintRec, spinBox.field)
static XtResource constraints[] = {
  { XmNspinBoxChildType, XmCSpinBoxChildType, XmRSpinBoxChildType,
    sizeof(unsigned char), ConstraintOffset(sb_child_type), XmRImmediate, (XtPointer)XmSTRING },
  { XmNpositionType, XmCPositionType, XmRPositionType,
    sizeof(unsigned char), ConstraintOffset(position_type), XmRImmediate, (XtPointer)XmPOSITION_VALUE },
  { XmNnumValues, ..., ConstraintOffset(num_values), XmRImmediate, (XtPointer)0 },
  { XmNvalues, XmCValues, XmRXmStringTable, sizeof(XmStringTable), ConstraintOffset(values), XmRStringTable, NULL },
  { XmNminimumValue, ... (XtPointer)0 }, { XmNmaximumValue, ... (XtPointer)10 },
  { XmNincrementValue, ... (XtPointer)1 }, { XmNdecimalPoints, XmCDecimalPoints, XmRShort, sizeof(short), ... },
  { XmNarrowSensitivity, ..., XmRArrowSensitivity, ... (XtPointer)XmARROWS_DEFAULT_SENSITIVITY },
  { XmNwrap, XmCWrap, XmRBoolean, sizeof(Boolean), ConstraintOffset(wrap), XmRImmediate, (XtPointer)True },
  { XmNposition, XmCPosition, XmRPositionValue, sizeof(int), ConstraintOffset(position), XmRImmediate, (XtPointer)0 }
};
```

`XmRSpinBoxChildType`, `XmRPositionType` and `XmRArrowSensitivity` are
representation types with generated string converters;
`XmRPositionValue` is the class's own converter, because converting
`"5"` for a child whose `XmNpositionType` is `XmPOSITION_INDEX` needs
the child's minimum and increment.  The same conversion is applied to
values set through `XtSetValues` by the *synthetic* constraint
resource (`syn_constraints`) with `GetPositionValue`/`SetPositionValue`
as export and import procedures (chapter 1.3 §1.3.4): the record always
holds a *value*, and the application sees a value or an index as it
chose.

## 4.4 Instance creation

```c
static void Initialize(Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)new_w;
  XGCValues GCvalues;
  XtGCMask GCmask, unusedMask;
  spinW->spinBox.textw = 0;
  spinW->spinBox.dim_mask = 0;
  ...                                   /* zero the private state and the arrow rectangles */
  if (!spinW->core.accelerators)
    spinW->core.accelerators = spinAccel;
  if (spinW->spinBox.initial_delay < 1)
    spinW->spinBox.initial_delay = spinW->spinBox.repeat_delay;
  /* Get arrow GC */
  GCmask = GCForeground | GCBackground | GCGraphicsExposures;
  GCvalues.foreground = spinW->core.background_pixel;
  GCvalues.background = spinW->manager.foreground;
  GCvalues.graphics_exposures = False;
  /* Share gc with scrollbar */
  spinW->spinBox.arrow_gc = XtAllocateGC(new_w, 0, GCmask, &GCvalues, 0, GCFont);
  GCmask |= GCFillStyle | GCStipple;
  unusedMask = GCClipXOrigin | GCClipYOrigin | GCFont;
  GCvalues.background = spinW->core.background_pixel;
  GCvalues.foreground = spinW->manager.foreground;
  GCvalues.fill_style = FillOpaqueStippled;
  GCvalues.stipple = _XmGetInsensitiveStippleBitmap(new_w);
  /* share GC with ArrowButton */
  spinW->spinBox.insensitive_gc = XtAllocateGC(new_w, 0, GCmask, &GCvalues, GCClipMask, unusedMask);
}
```

Three things to notice.  The accelerators are stored in the widget's
own `core.accelerators` so that `XtInstallAccelerators(child, spinbox)`
in `InsertChild` can copy them onto each child.  The two GCs are
obtained with `XtAllocateGC`, whose last two arguments declare which
fields the widget *may modify* (`GCClipMask` for the insensitive GC,
since `DrawSpinArrow` resets its clip) and which it *does not care
about* (`GCFont`, the clip origins), so that Xt can share the GC with a
ScrollBar or ArrowButton that asked for the same colours; the comments
say so.  The insensitive GC uses `FillOpaqueStippled` with the 50 %
stipple that every Motif widget uses to grey out, obtained from the
per-screen cache in `Screen.c`.  `Destroy` releases both with
`XtReleaseGC` and nothing else: the SpinBox allocates no memory of its
own.

## 4.5 Children, size and layout

```c
static void InsertChild(Widget newChild)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)XtParent(newChild);
  XtWidgetProc insert_child;
  /* call manager's InsertChild method */
  _XmProcessLock();
  insert_child = ((XmManagerWidgetClass)xmManagerWidgetClass)->composite_class.insert_child;
  _XmProcessUnlock();
  (*insert_child)(newChild);
  if (XmeTraitGet((XtPointer)XtClass(newChild), XmQTaccessTextual) != NULL) {
    spinW->spinBox.textw = newChild;
    XtInsertEventHandler(newChild, FocusChangeMask, False, SpinChildFocusChange, (XtPointer)spinW, XtListHead);
    XtInsertEventHandler(newChild, ButtonPressMask, False, SpinChildFocusChange, (XtPointer)spinW, XtListHead);
  }
  XtInstallAccelerators(newChild, (Widget)spinW);
}
```

`insert_child` is an *inherited* method, so calling the superclass's
version means reading it out of the superclass record by hand (chapter
1.1 §1.1.7).  A child that implements `accessTextual` becomes the
active text child and gets two event handlers at the head of its
list, so that the SpinBox learns about focus changes and clicks before
the child's own handlers run.  `ChangeManaged` then recomputes the
natural size, asks the parent (`_XmMakeGeometryRequest`, which does
the `XtGeometryAlmost` dance), lays out, and sets the text of every
managed child from its position:

```c
static void GetSpinSize(Widget w, Dimension *wide, Dimension *high)
{
  ...
  if (*wide == 0) {
    *wide = arrowsWide * arrowSize;
    *wide += (arrowsWide - 1) * spacing;
    *wide += 2 * spinW->spinBox.margin_width;
    *wide += 2 * SB_ShadowPixels(spinW);
    if (SB_WithChild(spinW))
      for (i = 0; (Cardinal)i < SB_ChildCount(spinW); i++) {
        childW = spinW->composite.children[i];
        if (XtIsManaged(childW))
          *wide += XtWidth(childW) + spinW->spinBox.spacing;
      }
    /* Remember our best width */
    spinW->spinBox.ideal_width = *wide;
  }
  if (!*high) {
    *high = arrowsHigh * arrowSize;
    *high += (arrowsHigh - 1) * spacing;
    *high += 2 * spinW->spinBox.margin_height;
    if (SB_WithChild(spinW))
      for (...)  *high = MAX(*high, childHeight);
    *high += 2 * SB_ShadowPixels(spinW);
    spinW->spinBox.ideal_height = *high;
  }
  if (*wide == 0) *wide = 1;
  if (*high == 0) *high = 1;
  ...
}
```

The natural size is arrows plus margins plus shadows plus the sum of
the children's widths (height: the maximum).  `SB_NumArrowsWide` is 1
when the arrows are stacked (`XmARROWS_END`/`BEGINNING`) and 2 when
side by side; `SB_ShadowPixels` is shadow thickness plus 2 or 0.  The
"ideal" size is cached so that `LayoutSpinBox` can tell whether it has
enough room and degrade gracefully: give up the margins first, then
shrink the spacing, in both axes, centring the arrows vertically when
there is room to spare.  Children are placed with `XmeConfigureObject`
(chapter 3) and the arrow rectangles are computed into
`up_arrow_rect`/`down_arrow_rect` for `DrawSpinArrow` and `ArrowWasHit`
to share.  `QueryGeometry` answers the parent with the same function
through `XmeReplyToQueryGeometry`; `SetValues` recomputes the size and
layout only when a geometry resource changed and the widget is
realized, and otherwise returns the redisplay flag for sensitivity and
colour changes (chapter 1.1 §1.1.4).

## 4.6 Drawing

```c
static void Redisplay(Widget w, XEvent *event, Region region)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)w;
  if (XtIsRealized(w)) {
    ClearArrows(w);
    if (spinW->manager.shadow_thickness > 0) {
      int width, height;
      width = (spinW->spinBox.ideal_width < XtWidth(spinW)) ? spinW->spinBox.ideal_width : XtWidth(spinW);
      height = (spinW->spinBox.ideal_height < XtHeight(spinW)) ? spinW->spinBox.ideal_height : XtHeight(spinW);
      XmeDrawShadows(XtDisplay(w), XtWindow(w), spinW->manager.top_shadow_GC, spinW->manager.bottom_shadow_GC,
                     0, 0, width, height, spinW->manager.shadow_thickness, XmSHADOW_OUT);
    }
    _XmSetFocusFlag(w, XmFOCUS_IGNORE, False);
    DrawSpinArrow(w, XmARROW_UP);
    DrawSpinArrow(w, XmARROW_DOWN);
  }
}
```

The shadow is drawn around the *ideal* size, not the allocated one, so
that a SpinBox stretched by a Form does not grow a frame around empty
space.  Each arrow is drawn with chapter 2's `XmeDrawArrow`, choosing
the GC by state:

```c
static void DrawSpinArrow(Widget arrowWidget, int arrowFlag)
{
  ...
    if (arrowFlag == XmARROW_UP) {
      arrowX = spinW->spinBox.up_arrow_rect.x; ...
      if (UpArrowSensitive(spinW)) {
        arrowGC = spinW->spinBox.arrow_gc;
        arrowPressed = spinW->spinBox.up_arrow_pressed;
      }
      else {
        arrowGC = spinW->spinBox.insensitive_gc;
        XSetClipMask(XtDisplay(arrowWidget), arrowGC, None);
      }
    }
    ...
    arrowDirection = GetArrowDirection(arrowWidget, arrowFlag);
    XmeDrawArrow(XtDisplay(arrowWidget), XtWindow(arrowWidget),
                 arrowPressed ? spinW->manager.bottom_shadow_GC : spinW->manager.top_shadow_GC,
                 arrowPressed ? spinW->manager.top_shadow_GC : spinW->manager.bottom_shadow_GC,
                 arrowGC, arrowX, arrowY, arrowWidth, arrowHeight,
                 spinW->spinBox.detail_shadow_thickness, arrowDirection);
}
```

A pressed arrow is drawn by swapping the two shadow GCs, the same
trick as `XmSHADOW_IN` in chapter 2.  `GetArrowDirection` maps "up" to
`XmARROW_UP`, `XmARROW_RIGHT` or, in a right-to-left layout, the
mirrored direction.  Sensitivity is a small decision procedure:

```c
static Boolean UpArrowSensitive(XmSpinBoxWidget spinW)
{
  ...
  if (XtIsSensitive((Widget)spinW) != True)
    upState = (unsigned char)XmARROWS_INSENSITIVE;
  else if (SB_ChildCount(spinW) && SB_WithChild(spinW)) {
    spinC = SB_GetConstraintRec(spinW->spinBox.textw);
    upState = spinC->arrow_sensitivity;
  }
  else
    upState = (unsigned char)XmARROWS_DEFAULT_SENSITIVITY;
  if (upState == (unsigned char)XmARROWS_DEFAULT_SENSITIVITY)
    upState = spinW->spinBox.default_arrow_sensitivity;
  return (upState & (unsigned char)XmARROWS_INCREMENT_SENSITIVE);
}
```

the child's constraint overrides the widget's default, and the
`XmARROWS_*` values are bit masks so that one byte says which arrows
are live.

## 4.7 Input: actions, hit test, timer

The action table binds the ten action names to functions:

```c
static XtActionsRec actionsTable[] = {
  {"SpinBArm", SpinBArm}, {"SpinBDisarm", SpinBDisarm}, {"SpinBPrior", SpinBPrior},
  {"SpinBNext", SpinBNext}, {"SpinBLeft", SpinBLeft}, {"SpinBRight", SpinBRight},
  {"SpinBFirst", SpinBFirst}, {"SpinBLast", SpinBLast}, {"SpinBEnter", SpinBEnter},
  {"SpinBLeave", SpinBLeave},
};
```

Mouse input arrives through the default translations on the SpinBox's
own window (`<Btn1Down>: SpinBArm()`, `<Btn1Up>: SpinBDisarm()`), and
because the arrows are not widgets, `SpinBArm` hit-tests the event
against the rectangles:

```c
static void SpinBArm(Widget armWidget, XEvent *armEvent, String *armParams, Cardinal *armCount)
{
  if (armEvent->type == ButtonPress) {
    if (ArrowWasHit(armWidget, XmARROW_UP, armEvent))
      SpinBAction(armWidget, XmARROW_UP);
    else if (ArrowWasHit(armWidget, XmARROW_DOWN, armEvent))
      SpinBAction(armWidget, XmARROW_DOWN);
  }
}
```

`SpinBAction` records which arrow was hit (`last_hit`), moves the
keyboard focus to the text child (`XmProcessTraversal(textw,
XmTRAVERSE_CURRENT)`), redraws the arrow pressed, and starts the
repeat timer:

```c
static void SpinTimeOut(Widget w, int spinDelay)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)w;
  if (spinW->spinBox.initial_delay > 0 && spinW->spinBox.repeat_delay > 0)
    spinW->spinBox.spin_timer = XtAppAddTimeOut(XtWidgetToApplicationContext(w), spinDelay, SpinBArrow, (XtPointer)w);
}

static void SpinBArrow(XtPointer spinData, XtIntervalId *spinInterval)
{
  XmSpinBoxWidget spinW = (XmSpinBoxWidget)spinData;
  spinW->spinBox.make_change = False;
  if (spinW->spinBox.up_arrow_pressed) {
    if (UpArrowSensitive(spinW)) {
      SpinTimeOut((Widget)spinData, spinW->spinBox.repeat_delay);
      DrawSpinArrow((Widget)spinData, XmARROW_UP);
      ArrowSpinUp((Widget)spinData, (XEvent *)NULL);
    }
    else { spinW->spinBox.up_arrow_pressed = False; DrawSpinArrow((Widget)spinData, XmARROW_UP); }
  }
  else if (spinW->spinBox.down_arrow_pressed) { ... }
}
```

This is the standard Xt auto-repeat idiom: a one-shot timeout
(`initial_delay`) whose handler re-arms itself with `repeat_delay`
while the button stays pressed, steps the value, and stops when the
arrow becomes insensitive (the range end without wrap).  The timer
handler is not an action and has no event, hence `(XEvent *)NULL` in
the callbacks.  `SpinBDisarm` removes a pending timer, redraws both
arrows unpressed, and performs the step *if the timer never fired*
(`make_change`): a short click steps once on release, a long press
steps on the timer and not again on release.  The same two functions
serve the keyboard through the accelerators: `SpinBNext` on `osfUp`
and `SpinBDisarm` on its `<KeyUp>`.

## 4.8 Stepping the value

```c
static void ArrowSpinUp(Widget w, XEvent *callEvent)
{
  ...
  if (SB_ChildCount(spinW) && SB_WithChild(spinW)) {
    spinC = SB_GetConstraintRec(spinW->spinBox.textw);
    inPosition = spinC->position;
    spinW->spinBox.boundary = False;
    spinC->position += (SB_ChildIsNumeric(spinC) ? spinC->increment_value : 1);
    if (spinC->position > SB_ChildMaximumPositionValue(spinC)) {
      if (spinC->wrap) {
        spinW->spinBox.boundary = True;
        spinC->position = SB_ChildMinimumPositionValue(spinC);
      }
      else {
        spinC->position = inPosition;
        XBell(XtDisplay(spinW), 0);
      }
    }
    /* Update the Text Widget */
    if (inPosition != spinC->position) {
      if (ArrowVerify((Widget)spinW, callEvent, XmCR_SPIN_NEXT)) {
        UpdateChildText(spinW->spinBox.textw);
        ArrowCallback((Widget)spinW, callEvent, XmCR_SPIN_NEXT);
      }
      else
        spinC->position = inPosition;
    }
  }
  else
    ArrowCallback((Widget)spinW, callEvent, XmCR_SPIN_NEXT);
}
```

The order is the Motif callback contract: compute the candidate,
offer it to `XmNmodifyVerifyCallback` with `doit = True`
(`ArrowVerify` builds an `XmSpinBoxCallbackStruct` with the new
`position`, the `value` string and `crossed_boundary`), and only if
the application did not clear `doit` commit it, update the display
and call `XmNvalueChangedCallback`.  With no text child the SpinBox
still fires the callbacks, which makes it usable as a bare pair of
arrows.

`UpdateChildText` is where the trait pays off (chapter 1.2): it formats
the position (`NumToString`, which handles `XmNdecimalPoints` by
inserting the locale's decimal point) or picks the `XmNvalues` entry,
and hands it to the child through `XmQTaccessTextual`, in
`XmFORMAT_MBYTE` or `XmFORMAT_XmSTRING`, without knowing the child's
class.

## 4.9 Being a navigator

The three functions in `spinBoxNT` implement the navigator trait so
that the SpinBox can be driven externally: `SpinNGetValue` reports the
current position, minimum, maximum and increment of the active child
in an `XmNavigatorDataRec`; `SpinNSetValue` sets the position (through
the same verify/commit path) and redraws; `SpinNChangeMoveCB`
registers or removes a move callback.  Nothing in the SpinBox calls
these; a scroll frame does (`XmScrolledWindow` implements the
`XmQTscrollFrame` trait, whose `addNavigator` method connects any
navigator child), or an application.

## 4.10 What the walk-through shows

| Mechanism | Where it appears in `SpinB.c` |
|-----------|------------------------------|
| Chained methods (chapter 1.1) | `Initialize`, `SetValues`, `Destroy`, `ConstraintInitialize/SetValues/Destroy` handle only the SpinBox part |
| Inherited methods | `XtInheritRealize`, `XtInheritDeleteChild`; `InsertChild` calls Manager's by hand |
| Base class extension (chapter 1.2) | `spinBoxBaseClassExtRec`, stamped in `ClassInitialize` |
| Fast subclassing | `XmSPINBOX_BIT`; `XmIsTextField`/`XmIsText` tests in `ChangeManaged` |
| Traits | implements `navigator`; consumes `accessTextual` |
| Resources and converters (chapter 1.3) | widget and constraint tables; `CvtStringToPositionValue` with `XtBaseOffset` argument; synthetic `XmNposition` |
| Drawing (chapter 2) | `XmeDrawShadows`, `XmeDrawArrow`, GC swapping for the pressed look, shared GCs via `XtAllocateGC` |
| Geometry (chapter 3) | `GetSpinSize`, `LayoutSpinBox` with graceful degradation, `XmeReplyToQueryGeometry`, `_XmMakeGeometryRequest`, `XmeConfigureObject` |
| Input (chapter 3) | actions on the manager's window, hit test against rectangles, accelerators installed on children, virtual keys, `XmProcessTraversal` |
| Timers | `XtAppAddTimeOut` auto-repeat with initial and repeat delays |
| Callbacks | verify-then-commit with a `doit` flag; reason codes `XmCR_SPIN_NEXT`, `XmCR_SPIN_PRIOR`, `XmCR_SPIN_FIRST`, `XmCR_SPIN_LAST` and `XmCR_OK` |

What it does *not* show: memory management (the SpinBox owns nothing
but two GCs), and threads (every entry point relies on Xt's
dispatcher holding the application lock).  For a widget that owns
data structures, read chapter 6.1.

## References

- [`src/lib/Xm/SpinB.c`](../../src/lib/Xm/SpinB.c),
  [`SpinBP.h`](../../src/lib/Xm/SpinBP.h),
  [`Transltns.c`](../../src/lib/Xm/Transltns.c) (`_XmSpinB_defaultAccelerators`),
  [`SSpinB.c`](../../src/lib/Xm/SSpinB.c) (`XmSimpleSpinBox`, a SpinBox that creates its own TextField)
- Manual pages `XmSpinBox(3)`, `XmSimpleSpinBox(3)`,
  `XmSpinBoxValidatePosition(3)` in [`doc/man/man3`](../man/man3)
- The `Widgets` suite in `src/tests/Xm` creates every widget class,
  SpinBox included
