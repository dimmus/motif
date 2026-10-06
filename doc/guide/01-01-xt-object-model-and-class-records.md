# 1.1 The Xt Object Model and Class Records

**Scope.** Motif widgets are Xt widgets, and almost every design
decision in libXm is a response to how Xt expresses objects in C.  This
chapter describes that object model precisely: the class record, the
instance record, how methods are inherited and chained, what the
`XtInherit*` constants really are, and what all of this costs at run
time and in binary compatibility.  The examples are taken from the
class records of `XmSpinBox`, `XmDataField`, `XmPrimitive` and
`XmManager` in this tree.

**Prerequisites.** [Chapter 1](01-motif-architecture-overview.md).
The normative reference is the Xt specification, chapter 1 ("Intrinsics
and Widgets") and chapter 2 ("Widget Instantiation"):
<https://www.x.org/releases/current/doc/libXt/intrinsics.html>.

---

## 1.1.1 Objects in C, 1988 style

Xt predates C++ on Unix workstations and was designed to be usable from
any language with a C binding.  It implements classes and single
inheritance with three ideas:

1. **A class is a statically initialised structure of function pointers
   and tables** (the *class record*).  There is exactly one per class,
   it lives in the library's data segment, and its address is the class
   identity (`xmPushButtonWidgetClass` is `&xmPushButtonClassRec`).
2. **An instance is a structure whose first member is its superclass's
   instance structure** (the *instance record*).  A pointer to a
   `XmPushButtonRec` is therefore also a valid pointer to a
   `XmLabelRec`, a `XmPrimitiveRec` and a `CoreRec`; the generic
   `Widget` type is a pointer to the common prefix.
3. **Method dispatch is an indirect call through the class record**;
   there is no virtual-table walk because *inheritance is resolved once,
   when the class is initialised*, by copying pointers from the
   superclass record into the subclass record (§1.1.4).

The same three ideas organise the *class* records: `XmSpinBoxClassRec`
begins with `CoreClassPart`, then `CompositeClassPart`,
`ConstraintClassPart`, `XmManagerClassPart` and finally
`XmSpinBoxClassPart`, one *part* per ancestor.

## 1.1.2 Anatomy of a class record

The complete class record of `XmSpinBox`, from
[`SpinB.c`](../../src/lib/Xm/SpinB.c), with the meaning of each field.
The comments in the right column are the ones in the source.

```c
externaldef(xmspinboxclassrec) XmSpinBoxClassRec xmSpinBoxClassRec = {
    {
        (WidgetClass)&xmManagerClassRec, /* superclass */
        "XmSpinBox",                     /* class_name */
        sizeof(XmSpinBoxRec),            /* widget_size */
        ClassInitialize,                 /* class_initialize */
        ClassPartInitialize,             /* class_part_initialize */
        FALSE,                           /* class_inited */
        Initialize,                      /* initialize */
        NULL,                            /* initialize_hook */
        XtInheritRealize,                /* realize */
        actionsTable,                    /* actions */
        XtNumber(actionsTable),          /* num_actions */
        resources,                       /* resources */
        XtNumber(resources),             /* num_resources */
        NULLQUARK,                       /* xrm_class */
        TRUE,                            /* compress_motion */
        XtExposeCompressMaximal |        /* compress_exposure */
            XtExposeNoRegion,
        TRUE,                               /* compress_enterleave */
        FALSE,                              /* visible_interest */
        Destroy,                            /* destroy */
        Resize,                             /* resize */
        Redisplay,                          /* expose */
        SetValues,                          /* set_values */
        NULL,                               /* set_values_hook */
        XtInheritSetValuesAlmost,           /* set_values_almost */
        NULL,                               /* get_values_hook */
        XtInheritAcceptFocus,               /* accept_focus */
        XtVersion,                          /* version */
        NULL,                               /* callback private */
        defaultTranslations,                /* tm_table */
        QueryGeometry,                      /* query_geometry */
        NULL,                               /* display_accelerator */
        (XtPointer)&spinBoxBaseClassExtRec, /* extension */
    },
    { /* composite_class fields */
        GeometryManager, ChangeManaged, InsertChild, XtInheritDeleteChild, NULL },
    { /* constraint_class fields */
        constraints, XtNumber(constraints), sizeof(XmSpinBoxConstraintRec),
        ConstraintInitialize, ConstraintDestroy, ConstraintSetValues, NULL },
    { /* manager_class fields */
        NULL, syn_resources, XtNumber(syn_resources),
        syn_constraints, XtNumber(syn_constraints), NULL, NULL },
    { /* spinbox_class fields */
        NULL, /* get_callback_widget */
        NULL  /* extension */
    }};
externaldef(xmspinboxwidgetclass)
    WidgetClass xmSpinBoxWidgetClass = (WidgetClass)&xmSpinBoxClassRec;
```

The `CoreClassPart` fields fall into four groups:

| Group | Fields | Notes |
|-------|--------|-------|
| Identity | `superclass`, `class_name`, `widget_size`, `version`, `class_inited` | `widget_size` is the size of the *instance* record; `class_inited` is set by Xt after the one-time initialisation. |
| Resource and event tables | `resources`, `num_resources`, `actions`, `num_actions`, `tm_table`, `xrm_class`, `display_accelerator` | Merged by Xt with the superclass's tables (chapter 1.3). `tm_table` is the default translation table, as a string before class initialisation and compiled afterwards. |
| Event-compression policy | `compress_motion`, `compress_exposure`, `compress_enterleave`, `visible_interest` | Flags that tell Xt which events to coalesce before calling the widget. `XtExposeCompressMaximal` lets Xt deliver one expose for a burst; `XtExposeGraphicsExpose` must be present for the List scrolling optimisation of chapter 6.1 to be safe. |
| Methods | `class_initialize`, `class_part_initialize`, `initialize`, `initialize_hook`, `realize`, `destroy`, `resize`, `expose`, `set_values`, `set_values_hook`, `set_values_almost`, `get_values_hook`, `accept_focus`, `query_geometry` | See §1.1.4 for which ones chain and which are inherited. |
| Extension | `extension` | The head of a linked list of *class extension records*, which is how Motif attaches its own per-class data to an Xt class (chapter 1.2). |

The `externaldef(name)` macro expands to nothing more than a storage
class on most platforms (it was a VMS global-symbol attribute); it marks
the symbols that the version script exports.

## 1.1.3 Anatomy of an instance record

The instance record of `XmDataField`, from
[`DataFP.h`](../../src/lib/Xm/DataFP.h):

```c
typedef struct _XmDataFieldRec {
  CorePart core;
  XmPrimitivePart primitive;
  XmTextFieldPart text;
  XmDataFieldPart data;
} XmDataFieldRec;
```

Each ancestor contributes one *part*, and a part is a plain struct of
the state that the class manages: `CorePart` has the window, geometry,
colormap, event table and `being_destroyed` flag; `XmPrimitivePart` has
the foreground, the shadow and highlight colours and GCs, the traversal
and navigation state; `XmTextFieldPart` has the text source, cursor,
selection and the twenty-odd GCs and fonts of a text field.  The last
part belongs to the class itself.

Two consequences follow from the prefix layout:

- **Up-casting is a no-op** and down-casting is a plain cast.  Every
  function in libXm that takes a `Widget` and needs the primitive part
  writes `((XmPrimitiveWidget)w)->primitive.foreground`.  The pointer
  value does not change; only the static type does.  This is legal C:
  a pointer to a structure, suitably converted, points to its first
  member, and `core` is the first member at every level.
- **The layout of every part is ABI.**  A subclass compiled against
  `TextFP.h` embeds `XmTextFieldPart` by value and addresses its own
  `data` part at `sizeof(CorePart) + sizeof(XmPrimitivePart) +
  sizeof(XmTextFieldPart)`.  Adding one byte to `XmPrimitivePart`
  moves every subclass part of every widget, which is exactly what
  removing `tool_tip_string` from it did, and why this tree is
  `libXm.so.5` ([doc/abi-policy.md](../abi-policy.md)).  Where a field
  had to be added, it went into existing padding: `XmTextFieldPart`
  gained `alignment` "in what was tail padding, so the size of the
  record and the offsets of the other members did not change"
  ([CHANGELOG](../../CHANGELOG.md)).

Access to parts is wrapped in macros in the `*P.h` headers, for example
`XmDataField_picture(w)` for `((XmDataFieldWidget)(w))->data.picture`.
The macros exist so that a part can be moved (as `XmDataFieldPart` was,
when DataField became a subclass) without rewriting the uses, and so
that callers do not have to know whether a value lives in the widget or
in a shared cache object (gadgets, chapter 1.2).

## 1.1.4 Inheritance and chaining

Xt distinguishes three kinds of method, and the kind is fixed per
field, not per class:

**Chained methods** are called for every class in the chain, in a fixed
order, on the same widget.  The subclass does not call its superclass;
Xt does.

| Method | Order | Return value |
|--------|-------|--------------|
| `class_part_initialize` | superclass first | — |
| `initialize`, `initialize_hook` | superclass first | — |
| `set_values`, `set_values_hook` | superclass first | the `Boolean` "redisplay needed" results are OR-ed |
| `get_values_hook` | superclass first | — |
| `destroy` | subclass first | — |
| `constraint_class.initialize`, `.set_values`, `.destroy` | as above, on the child's constraint record | |

**Inherited methods** are called once, on the most specific class only.
A subclass either supplies its own or writes an `XtInherit*` sentinel
in the field:

| Method | Sentinel | Inherited by |
|--------|----------|--------------|
| `realize` | `XtInheritRealize` | most Motif classes (Primitive and Manager supply the real one) |
| `resize`, `expose` | `XtInheritResize`, `XtInheritExpose` | DataField inherits TextField's drawing |
| `set_values_almost` | `XtInheritSetValuesAlmost` | nearly everyone |
| `accept_focus` | `XtInheritAcceptFocus` | |
| `query_geometry` | `XtInheritQueryGeometry` | |
| `tm_table` | `XtInheritTranslations` | |
| `composite_class.geometry_manager`, `.change_managed`, `.insert_child`, `.delete_child` | `XtInheritGeometryManager`, ... | SpinBox inherits `delete_child` from Manager |
| `XmPrimitiveClassPart.border_highlight`, `.border_unhighlight` | `XmInheritBorderHighlight`, ... | Motif-defined inherited methods |

**Merged tables**: `resources`, `actions` and the constraint resources
are concatenated by Xt (subclass entries override superclass entries
with the same name).

### 1.1.4.1 What `XtInheritRealize` is

Every `XtInherit*` constant is the same value: the address of a private
Xt function, `_XtInherit`.

```c
/* src/lib/Xm/XmP.h, the Motif-defined ones */
#  define XmInheritBorderHighlight ((XtWidgetProc)_XtInherit)
#  define XmInheritFocusChange ((XmFocusChangeProc)_XtInherit)
```

During `XtInitializeWidgetClass`, each class's `class_part_initialize`
is run (chained, superclass first) on the *new* class record, and the
one belonging to the class that *defines* a field replaces the sentinel
with the superclass's pointer:

```c
if (wc->core_class.realize == XtInheritRealize)
    wc->core_class.realize = superclass->core_class.realize;
```

(this is Xt's own code for the core fields; Motif's
`ClassPartInitialize` in `Primitive.c` does the same for
`border_highlight`).  Inheritance is therefore a *copy at class
initialisation*, which has three properties worth stating:

- A call of an inherited method costs one indirect call, the same as a
  call of an overridden one.  There is no run-time walk.
- Resolution happens once per class, lazily, the first time a widget of
  that class is created (`XtCreateWidget` calls
  `XtInitializeWidgetClass` if `class_inited` is false).  Reading a
  class record before that is unreliable, which is why `XmeTraitGet`
  and the fast-subclass tests (chapter 1.2) are only valid after class
  initialisation.
- The sentinel is a *function pointer with a magic value*.  C has no
  "unset" value for a function pointer other than `NULL`, and `NULL`
  already means "no method", so Xt had to invent one.

### 1.1.4.2 Chaining in practice

The chained `initialize` of a `XmPushButton` runs, in order:
`Core`, `XmPrimitive`, `XmLabel`, `XmPushButton`.  Each one sees the
same `req`/`new_w` pair and may only rely on the parts of its own
ancestors being initialised.  This forces a discipline that is visible
everywhere in libXm: a class never touches a subclass part, and a class
that needs to run *after* its subclasses (to see the final values of
their resources) cannot do so through Xt alone.  That gap is what
Motif's *post-hooks* fill (chapter 1.2).

For `set_values`, the convention is that each level compares the fields
it owns in `old` and `new_w`, repairs invalid combinations, and returns
whether a redraw is needed:

```c
/* src/lib/Xm/SpinB.c */
  if ((newW->core.sensitive != oldW->core.sensitive) ||
      (newW->core.ancestor_sensitive != oldW->core.ancestor_sensitive))
    displayFlag = True;
```

Xt ORs the results of every level and, if any is true, clears the
window and generates an expose.

## 1.1.5 Three instance records in one call

Several methods receive more than one copy of the widget:

- `initialize(Widget req, Widget new_w, ...)`: `req` is the record as
  Xt built it from resource defaults, resource files and the argument
  list; `new_w` is the one that will become the widget.  A class may
  change `new_w` (clamp a value, substitute a default) and compare
  against `req` to see what the application *asked* for.
- `set_values(Widget old, Widget req, Widget new_w, ...)`: `old` is the
  widget before the call, `req` what the caller asked for, `new_w` the
  merged result.  `XmSpinBox` compares `reqW` against `oldW` for
  geometry resources and `newW` against `oldW` for sensitivity: the
  former detects "the application set this", the latter "this changed
  for any reason".

The copies are plain `memcpy`s of `widget_size` bytes (plus the
constraint record for children of constraint widgets).  Any pointer
member is therefore shallow-copied, which is why `set_values` methods
must never free a pointer that `old` and `new_w` share unless they
replace it in `new_w`; the DataField `SetValues` of chapter 3.1 shows
the pattern.

## 1.1.6 Constraint records

A manager that descends from `Constraint` attaches a second record to
each child: the *constraint record*, allocated by the parent's
`constraint_class.constraint_size` and stored in `child->core.constraints`.
`XmSpinBox` keeps the per-child numeric range, the string table and the
current position there (`XmSpinBoxConstraintRec`); `XmForm` keeps the
four attachments.  Constraint records have their own chained
`initialize`/`set_values`/`destroy` and their own resource list, which
is why `XmNminimumValue` is set on the *child* of a SpinBox and
`XmNtopAttachment` on the *child* of a Form.  Chapters 3.2, 3.3 and 4
rely on this.

## 1.1.7 Costs and trade-offs

| Property | Benefit | Cost |
|----------|---------|------|
| Static class records | No class construction at run time; a class is a few kilobytes of initialised data; `dlopen`-able widget sets. | Positional aggregate initialisers: a field added to `CoreClassPart` by Xt would break every class record (Xt therefore never did). A `-Wextra` build of the 2.4.1 baseline reported 673 `-Wmissing-field-initializers` warnings for records that stop early ([TODO.md](../../TODO.md)). |
| Prefix layout | Zero-cost up-casts; every generic routine works on `Widget`. | The whole layout is ABI; a one-byte change anywhere in a part forces a new SONAME. Instance records are large (a `XmTextFieldRec` is several hundred bytes) because every ancestor's state is inline. |
| Copy-on-init inheritance | O(1) dispatch, no vtable walk. | No way to call "the superclass version" at run time except by reading the superclass record by hand (`xmTextFieldClassRec.core_class.expose`), which DataField's wrappers do. |
| Chained methods | Each level owns its part; subclasses are short. | No post-hook; subclasses cannot see the final state in `initialize`; Motif added wrappers (chapter 1.2). |
| `XtIsSubclass` | Correct for any class. | O(depth) pointer chase per test; Motif added fast-subclass bits (chapter 1.2). |
| Single inheritance | Simple, predictable layouts. | No interfaces; a Text and a TextField cannot share a type a ComboBox could ask for; Motif added traits (chapter 1.2). |

## 1.1.8 Example: reading a class record quickly

A practical procedure when opening an unfamiliar widget source:

1. Find `ClassRec = {` and read the `superclass` field: it tells you
   which `*P.h` headers to open for the inherited parts.
2. Read the method column of the `CoreClassPart`: every `NULL` or
   `XtInherit*` entry is a method you do not need to look for in this
   file.
3. Read the `extension` field.  If it points at a
   `XmBaseClassExtRec` (every Motif class) look at its `flags`-setting
   `ClassPartInitialize` to learn the class's fast-subclass bit and
   which traits it installs; chapter 1.2 explains both.
4. Read the resource table; the `XtOffsetOf` arguments name the part
   fields that the class actually owns.

## References

- Xt specification, §1.4 "Class Records" and §1.6 "Inheritance of
  Superclass Operations": <https://www.x.org/releases/current/doc/libXt/intrinsics.html>
- [`src/lib/Xm/SpinB.c`](../../src/lib/Xm/SpinB.c),
  [`DataF.c`](../../src/lib/Xm/DataF.c), [`DataFP.h`](../../src/lib/Xm/DataFP.h),
  [`Primitive.c`](../../src/lib/Xm/Primitive.c), [`Manager.c`](../../src/lib/Xm/Manager.c),
  [`XmP.h`](../../src/lib/Xm/XmP.h).
- [doc/abi-policy.md](../abi-policy.md), "What is the API".
