# 3.1 Case Study: XmDataField, Extending a Widget by Subclassing

**Scope.** `XmDataField` is a `XmTextField` with an input *picture*
(a mask such as `###-####` that restricts what may be typed) and a
validation step before the keyboard focus leaves the field.  Until this
tree, it was implemented as an 8 683-line *copy* of `TextF.c` with the
picture code added; it is now a 570-line *subclass*
([`DataF.c`](../../src/lib/Xm/DataF.c)).  The rewrite is a complete,
small example of everything chapters 1.1 and 1.2 describe: which
methods chain, which are inherited, what is not inherited and must be
done by hand, and how a subclass hooks into its superclass's behaviour
through callbacks rather than by editing it.  It also records three
bugs that the copy had and the subclass does not.

---

## 3.1.1 The class record

```c
/* src/lib/Xm/DataF.c */
externaldef(xmdatafieldclassrec) XmDataFieldClassRec xmDataFieldClassRec = {
    {
        (WidgetClass)&xmTextFieldClassRec, /* superclass         */
        "XmDataField",                     /* class_name         */
        sizeof(XmDataFieldRec),            /* widget_size        */
        NULL,                              /* class_initialize   */
        ClassPartInitialize,               /* class_part_initiali*/
        FALSE,                             /* class_inited       */
        Initialize,                        /* initialize         */
        (XtArgsProc)NULL,                  /* initialize_hook    */
        XtInheritRealize,                  /* realize            */
        actions,                           /* actions            */
        XtNumber(actions),                 /* num_actions        */
        resources,                         /* resources          */
        XtNumber(resources),               /* num_resources      */
        NULLQUARK,                         /* xrm_class          */
        TRUE,                              /* compress_motion    */
        XtExposeCompressMaximal |          /* compress_exposure  */
            XtExposeNoRegion,
        TRUE,                     /* compress_enterleave*/
        FALSE,                    /* visible_interest   */
        Destroy,                  /* destroy            */
        XtInheritResize,          /* resize             */
        XtInheritExpose,          /* expose             */
        SetValues,                /* set_values         */
        (XtArgsFunc)NULL,         /* set_values_hook    */
        XtInheritSetValuesAlmost, /* set_values_almost  */
        (XtArgsProc)NULL,         /* get_values_hook    */
        (XtAcceptFocusProc)NULL,  /* accept_focus       */
        XtVersion,                /* version            */
        NULL,                     /* callback_private   */
        XtInheritTranslations,    /* tm_table           */
        XtInheritQueryGeometry,   /* query_geometry     */
        (XtStringProc)NULL,       /* display accel      */
        NULL,                     /* extension          */
    },
    { /* Xmprimitive */
        XmInheritBorderHighlight, XmInheritBorderUnhighlight, NULL, (XtActionProc)NULL,
        NULL, 0, (XtPointer)&primClassExtRec },
    { /* data class: in the place of XmTextField's text class part */
        NULL, /* extension */
    }};
```

Reading it with the rules of chapter 1.1:

- **Chained and supplied here:** `class_part_initialize`,
  `initialize`, `set_values`, `destroy`.  Xt runs TextField's versions
  first (last for `destroy`), so these four only handle the picture.
- **Inherited:** `realize`, `resize`, `expose`, `set_values_almost`,
  `query_geometry`, `tm_table`, `border_highlight`,
  `border_unhighlight`.  All drawing, scrolling, selection, clipboard,
  drag and drop and geometry are TextField's, unchanged, and a fix in
  `TextF.c` fixes DataField too.
- **Merged:** the four picture resources are appended to TextField's
  resource list; the one action, `ValidateAndMove`, is added to
  TextField's action table.
- **`extension` is `NULL`** in the core part: DataField has no base
  class extension of its own, so `ClassPartInitRootWrapper` (chapter
  1.2) allocates one and inherits every hook from TextField.
- **The third class part** is `XmDataFieldClassPart`, a single
  `extension` pointer, which occupies exactly the slot of
  `XmTextFieldClassPart` (also a single `extension` pointer) in the
  superclass record.  The class record is therefore layout-compatible
  with TextField's, which matters because TextField's own methods cast
  `XtClass(w)` to `XmTextFieldWidgetClass` and read `text_class`.

The instance record appends one part:

```c
/* src/lib/Xm/DataFP.h */
typedef struct _XmDataFieldRec {
  CorePart core;
  XmPrimitivePart primitive;
  XmTextFieldPart text;
  XmDataFieldPart data;
} XmDataFieldRec;
```

so that a DataField *is* a TextField in memory: `XmIsTextField()` is
true for it, every `XmTextField*` function accepts it, and SpinBox,
ComboBox, BulletinBoard's `XmNtextTranslations` and RowColumn's text
alignment handle it without knowing it exists
([CHANGELOG](../../CHANGELOG.md)).  The `XmDataField*` public functions
remain as one-line wrappers for source compatibility:

```c
char *XmDataFieldGetString(Widget w) { return XmTextFieldGetString(w); }
```

## 3.1.2 What is not inherited, and why

Two things have to be set up by hand in `ClassPartInitialize`, and
both are consequences of design choices documented in chapter 1.2.

```c
static void ClassPartInitialize(WidgetClass w_class)
{
  char *event_bindings;
  size_t size;
  _XmFastSubclassInit(w_class, XmDATAFIELD_BIT);
  /* XmTextField sets its traits on its own class only. */
  XmeTraitSet((XtPointer)w_class, XmQTtransfer,
              XmeTraitGet((XtPointer)xmTextFieldWidgetClass, XmQTtransfer));
  XmeTraitSet((XtPointer)w_class, XmQTaccessTextual,
              XmeTraitGet((XtPointer)xmTextFieldWidgetClass, XmQTaccessTextual));
  /* XmTextField's translations, with Tab bound to ValidateAndMove.  The
   * table this replaces is XmTextField's own (XtInheritTranslations). */
  size = strlen(_XmDataF_EventBindings4) + strlen("\n") + strlen(_XmTextF_EventBindings1) +
         strlen(_XmTextF_EventBindings2) + strlen(_XmTextF_EventBindings3) + 1;
  event_bindings = XtMalloc(size);
  snprintf(event_bindings, size, "%s\n%s%s%s", _XmDataF_EventBindings4,
           _XmTextF_EventBindings1, _XmTextF_EventBindings2, _XmTextF_EventBindings3);
  _XmProcessLock();
  w_class->core_class.tm_table = (String)XtParseTranslationTable(event_bindings);
  _XmProcessUnlock();
  XtFree(event_bindings);
}
```

**Traits.** `XmeTraitGet` looks up the exact class, and TextField
installs its `transfer` (drag and drop, clipboard) and `accessTextual`
(get/set the text, used by SpinBox and ComboBox) traits on its own
class only.  Without the two `XmeTraitSet` lines, a DataField inside a
SpinBox would be ignored by `UpdateChildText` (chapter 4) and could not
be a drop site.  The records are shared, not copied: the pointer that
TextField registered is registered again under the DataField class.
Note the `_XmFastSubclassInit` line too: `XmIsDataField()` needs its
own bit, while `XmIsTextField()` comes for free from the chained call
of TextField's `ClassPartInitialize`.

**Translations.** Xt's `XtInheritTranslations` would give DataField
exactly TextField's table.  DataField needs the same table with Tab and
Shift-Tab bound to `ValidateAndMove()` instead of the traversal
actions, so it builds the string by prepending its own bindings
(`_XmDataF_EventBindings4`) to TextField's three parts and parses it
into its *own* `tm_table`.  Xt's translation manager resolves
duplicate event bindings in favour of the earlier entry, so the prefix
wins.  The table is built with `snprintf` into a sized buffer, in line
with the banned-function rule of chapter 1 §1.8; the original copy used
`strcpy`/`strcat`.

## 3.1.3 Hooking the superclass through its own callback

The picture check must run on every edit, before the text changes.
TextField already has a hook for that, its `XmNmodifyVerifyCallback`,
so DataField *registers a callback on itself* instead of overriding any
drawing or input method:

```c
static void Initialize(Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
  XmDataFieldWidget df = (XmDataFieldWidget)new_w;
  XmDataField_picture(df) = NULL;
  if (XmDataField_picture_source(df)) {
    XmDataField_picture_source(df) = XtNewString(XmDataField_picture_source(df));
    XmDataField_picture(df) = XmParsePicture(XmDataField_picture_source(df));
    if (XmDataField_picture(df))
      XtAddCallback(new_w, XmNmodifyVerifyCallback, PictureVerifyCallback, NULL);
  }
}
```

The `XtNewString` copy is the standard rule for string resources: the
pointer in the record after resource processing belongs to the caller
(an argument list or a converter cache), so the widget takes its own
copy and frees it in `Destroy`.

`SetValues` handles a change of picture, and contains the fix for one
of the copy's bugs:

```c
static Boolean SetValues(Widget old, Widget request, Widget new_w, ArgList args, Cardinal *num_args)
{
  if (XmDataField_picture_source(old) != XmDataField_picture_source(new_w)) {
    XtFree((char *)XmDataField_picture_source(old));
    XmDataField_picture_source(new_w) = XtNewString(XmDataField_picture_source(new_w));
    if (XmDataField_picture(new_w)) {
      XmPictureDelete(XmDataField_picture(new_w));
      XmDataField_picture(new_w) = NULL;
    }
    if (XmDataField_picture_source(new_w))
      XmDataField_picture(new_w) = XmParsePicture(XmDataField_picture_source(new_w));
    /* Remove first, so that the callback is never registered twice. */
    XtRemoveCallback(new_w, XmNmodifyVerifyCallback, PictureVerifyCallback, NULL);
    if (XmDataField_picture(new_w))
      XtAddCallback(new_w, XmNmodifyVerifyCallback, PictureVerifyCallback, NULL);
  }
  return False;
}
```

The pointer comparison `old != new_w` on the string is the idiom for
"was this resource in the argument list": Xt copies the record, so the
pointers are equal unless `XtSetValues` stored a new one.  The copy
freed `old`'s string and installed the new one correctly, but added the
callback again without removing the previous registration, "so a
rejected character called `XmNpictureErrorCallback` twice"
([CHANGELOG](../../CHANGELOG.md)).  `return False` is correct because
the picture does not change what is drawn; Xt ORs it with TextField's
result.

## 3.1.4 The picture check

```c
static void PictureVerifyCallback(Widget w, XtPointer client_d, XtPointer call_d)
{
  XmTextVerifyCallbackStruct *cbs = (XmTextVerifyCallbackStruct *)call_d;
  char *curr, *newptr, *changed = NULL;
  int src, dst, i;
  XmPictureState ps;
  Boolean done = False;
  /* If we're just backspacing, allow the change irregarless */
  if (cbs->startPos < cbs->currInsert || cbs->text->length == 0)
    return;
  /* Get the current string, and splice in the intended changes */
  curr = XmTextFieldGetString(w);
  newptr = _XmMallocArray(cbs->text->length + strlen(curr) + 2, sizeof(char));
  ...                                   /* prefix, inserted text, suffix */
  XtFree(curr);
  /* Run it through the picture, and bail if it isn't accepted */
  ps = XmGetNewPictureState(XmDataField_picture(w));
  for (i = 0; (size_t)i < strlen(newptr); i++) {
    changed = XmPictureProcessCharacter(ps, newptr[i], &done);
    if (changed == NULL || done)
      break;
  }
  XtFree(newptr);
  if (changed == NULL) {
    cbs->doit = False;
    XmPictureDeleteState(ps);
    XtCallCallbackList(w, XmDataField_picture_error_cb(w), NULL);
    return;
  }
  /* And now try autofilling */
  if (XmDataField_auto_fill(w))
    changed = XmPictureDoAutoFill(ps);
  else
    changed = XmPictureGetCurrentString(ps);
  cbs->startPos = 0;
  XtFree(cbs->text->ptr);
  cbs->text->ptr = XtNewString(changed);
  cbs->text->length = strlen(changed);
  XmPictureDeleteState(ps);
}
```

The algorithm is: build the string as it *would* be after the edit;
feed it character by character to the picture's state machine
([`Picture.c`](../../src/lib/Xm/Picture.c), a parser of the picture
grammar with literals, classes such as `#` and `@`, repetition and
alternatives); reject the edit (`cbs->doit = False`) if the machine
rejects any character; otherwise optionally auto-fill the literal
characters that follow and *replace the whole text* by rewriting the
callback structure (`startPos = 0`, new `text->ptr`).  TextField frees
whatever the callbacks leave in `cbs->text`, so the string replaced
here is freed here; the copy "leaked a copy of the value for every
character it checked".

Complexity: O(n) picture steps per keystroke for a field of length n,
plus O(n) for the splice; a picture state machine step is O(1) in the
number of alternatives at that position.  For the fields a picture is
used on (phone numbers, dates, account numbers) n is tens of
characters.

## 3.1.5 The validation action

```c
static void ValidateAndMove(Widget w, XEvent *ev, String *args, Cardinal *nargs)
{
  XmDataFieldCallbackStruct cbs;
  cbs.w = w;
  cbs.text = XmTextFieldGetString(w);
  cbs.accept = True;
  XtCallCallbackList(w, XmDataField_validate_cb(w), (XtPointer)&cbs);
  XtFree(cbs.text);
  if (cbs.accept == False) {
    XBell(XtDisplay(w), 0);
    return;
  }
  if (*nargs > 0 && strncasecmp(args[0], "prev", 4) == 0)
    (void)XmProcessTraversal(w, XmTRAVERSE_PREV_TAB_GROUP);
  else
    (void)XmProcessTraversal(w, XmTRAVERSE_NEXT_TAB_GROUP);
}
```

This is the one place the subclass changes behaviour visible to the
user: Tab asks the application's `XmNvalidateCallback` whether the
value is acceptable, and only then calls the toolkit's traversal
(chapter 3).  The action takes its direction from the translation
parameter (`ValidateAndMove(prev)`), the standard way to parametrise
an action from a translation table.

## 3.1.6 Assessment

| | The copy (upstream 2.3.8) | The subclass (this tree) |
|---|---|---|
| Size | 8 683 lines + 748 in `DataFSel.c` | about 570 lines |
| Behaviour shared with TextField | diverged over time (`XmTextFieldReplaceWcs` freed a string literal in TextField; the copy had fixed it only for itself) | identical by construction |
| `XmIsTextField()` | false; the `XmText*` functions rejected it | true |
| Traits | registered a pointer to a local variable as the transfer trait | TextField's records re-registered |
| Bugs listed above | present | fixed |
| What a reader must know | the whole of `TextF.c` | the rules of chapters 1.1 and 1.2 |
| ABI | separate record layout | `XmDataFieldPart` moved after `XmTextFieldPart`; the `alignment` member is unused and `XmDataField_alignment()` reads TextField's |

The general lesson, which applies to every widget in the taxonomy of
chapter 1: in Xt, subclassing costs four small chained methods and a
checklist (fast-subclass bit, traits, translations); copying costs
everything the superclass will ever fix.

## References

- [`src/lib/Xm/DataF.c`](../../src/lib/Xm/DataF.c),
  [`DataFP.h`](../../src/lib/Xm/DataFP.h),
  [`Picture.c`](../../src/lib/Xm/Picture.c),
  [`TextF.c`](../../src/lib/Xm/TextF.c), [`TextFP.h`](../../src/lib/Xm/TextFP.h)
- Manual pages `XmDataField(3)`, `XmTextField(3)`, `XmParsePicture(3)`
  in [`doc/man/man3`](../man/man3)
- [CHANGELOG](../../CHANGELOG.md), "Compatibility" and "Code" in 2.5.0
- Tests: the DataField cases in `src/tests/Xm` (chapter 1 §1.8)
