# 1.2 Base Class Extensions, Wrappers, Fast Subclassing and Traits

**Scope.** Xt's object model (chapter 1.1) has no hooks, no cheap type
test and no interfaces.  Motif needed all three, could not change Xt,
and could not change the layout of the Xt class record.  This chapter
describes the three mechanisms it built inside the one extension point
Xt left open, the `extension` pointer of each class part, and analyses
each of them: what problem it solves, how it is implemented in
[`BaseClass.c`](../../src/lib/Xm/BaseClass.c) and
[`Trait.c`](../../src/lib/Xm/Trait.c), what it costs, and what it
cannot do.

---

## 1.2.1 Class extension records

Every class part of an Xt class ends with an `XtPointer extension`.
Xt defines it as the head of a singly linked list of records that all
begin with the same four fields:

```c
/* src/lib/Xm/BaseClassP.h */
typedef struct _XmGenericClassExtRec {
  XtPointer next_extension;
  XrmQuark record_type;
  long version;
  Cardinal record_size;
} XmGenericClassExtRec, *XmGenericClassExt;
```

A reader walks the list and compares `record_type` (an interned string,
an `XrmQuark`) with the quark it owns.  Motif's quark is `XmQmotif`,
interned from the string `"OSF_MOTIF"` in `_XmInitializeExtensions`.
The lookup is `_XmGetClassExtensionPtr(listHeadPtr, owner)`, a linear
scan of a list that in practice has one or two entries.  Xt itself uses
the same list for its own extensions (`XtCompositeExtensionRec` on the
composite part, for example), which is why the type test is needed.

Motif hangs its main per-class record, `XmBaseClassExtRec`, on the
*core* part's extension list of every Motif class.  Its fields, from
[`BaseClassP.h`](../../src/lib/Xm/BaseClassP.h):

| Field | Purpose |
|-------|---------|
| `next_extension`, `record_type`, `version`, `record_size` | The generic header. `record_type` is set to `XmQmotif` at class initialisation (`spinBoxBaseClassExtRec.record_type = XmQmotif;` in SpinBox's `ClassInitialize`), because a quark is not a compile-time constant and cannot appear in a static initialiser. |
| `initializePrehook`, `initializePosthook`, `setValuesPrehook`, `setValuesPosthook`, `getValuesPrehook`, `getValuesPosthook`, `classPartInitPrehook`, `classPartInitPosthook` | The hooks of §1.2.2. |
| `secondaryObjectClass`, `secondaryObjectCreate`, `getSecResData`, `ext_resources`, `compiled_ext_resources`, `num_ext_resources`, `use_sub_resources` | The *secondary object* machinery: a class may keep some of its resources in a separate object (the gadget cache objects, the shell extension objects), and these fields let `XtGetResourceList`-style queries and `XtSetValues` see them. |
| `flags[32]` | 256 bits of fast-subclass flags, §1.2.3. |
| `widgetNavigable`, `focusChange` | Two Motif-defined *inherited* methods for keyboard traversal (chapter 3). They live here rather than in `XmPrimitiveClassPart` because shells and gadgets need them too. |
| `wrapperData` | Per-class bookkeeping of the wrapper mechanism, §1.2.2. |

Hooks are inherited the same way Xt methods are: a class writes
`XmInheritInitializePrehook` (another `_XtInherit` sentinel) and
`ClassPartInitRootWrapper` copies the superclass's value at class
initialisation:

```c
/* src/lib/Xm/BaseClass.c */
      if ((*wcePtr)->initializePrehook == XmInheritInitializePrehook)
        (*wcePtr)->initializePrehook = (*scePtr)->initializePrehook;
```

The same function also *creates* a base class extension record for a
class that has none, provided its superclass has one, so that an
application-defined subclass of `XmPushButton` that knows nothing about
Motif extensions still inherits every hook and flag.  That is why a
plain Xt-style subclass of a Motif widget keeps working.

## 1.2.2 Pre- and post-hooks: the wrapper mechanism

### The problem

Xt chains `initialize` from the root class down to the leaf and
provides no way to run code *before the first* or *after the last* link
of the chain.  Two Motif features need exactly that:

- **Gadget cache objects.** A `XmLabelGadget` keeps its fonts, colours,
  margins and alignment in a *cache part* that identical gadgets share
  (a flyweight).  When `XtSetValues` is called on a gadget, the cache
  resources named in the argument list must be applied to a *private
  scratch copy*, and only after every class in the chain has run can the
  copy be interned in the cache and the gadget pointed at the shared
  instance.  `LabelG.c`'s `SetValuesPrehook` allocates the scratch
  secondary object, runs `XtSetSubvalues` on it with the cache class's
  resource list and points the gadget at it; `InitializePosthook` calls
  `_XmCachePart` to intern it and frees the temporaries.
- **Shell extension objects.** `VendorShell` is an Xt class whose
  record Motif cannot extend, so Motif's per-shell state lives in a
  separate `XmVendorShellExtObject`.  `VendorS.c`'s `InitializePrehook`
  creates it before any shell class runs, and `InitializePosthook`
  finishes its setup afterwards.

### The solution

`_XmInitializeExtensions` (called once, from the `ClassInitialize` of the Motif base classes: Primitive, Manager, Gadget, MenuShell and VendorShell)
patches the *root* class of the whole Xt hierarchy:

```c
/* src/lib/Xm/BaseClass.c */
void _XmInitializeExtensions(void)
{
  static Boolean firstTime = True;
  if (firstTime) {
    XmQmotif = XrmPermStringToQuark("OSF_MOTIF");
    objectClassWrapper.initialize = objectClass->core_class.initialize;
    objectClassWrapper.setValues = objectClass->core_class.set_values;
    objectClassWrapper.getValues = objectClass->core_class.get_values_hook;
    objectClassWrapper.classPartInit = objectClass->core_class.class_part_initialize;
    objectClass->core_class.class_part_initialize = ClassPartInitRootWrapper;
    objectClass->core_class.initialize = InitializeRootWrapper;
    objectClass->core_class.set_values = SetValuesRootWrapper;
    objectClass->core_class.get_values_hook = GetValuesRootWrapper;
    firstTime = False;
  }
  ...
}
```

Because `objectClass` is the first link of every chain, `InitializeRootWrapper`
runs before any class's `initialize`.  It looks up the *leaf* class's
base extension (`XtClass(new_w)`), calls its `initializePrehook`, and
then arranges for the post-hook:

```c
static void InitializeRootWrapper(Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{
  WidgetClass wc = XtClass(new_w);
  XmBaseClassExt *wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);
  if (wcePtr && *wcePtr) {
    if ((*wcePtr)->initializePrehook)
      (*((*wcePtr)->initializePrehook))(req, new_w, args, num_args);
    if ((*wcePtr)->initializePosthook) {
      XmWrapperData wrapperData;
      _XmProcessLock();
      ...
        wrapperData = GetWrapperData(wc);
        if (wrapperData->initializeLeafCount == 0) {
          wrapperData->initializeLeaf = wc->core_class.initialize;
          wc->core_class.initialize = InitializeLeafWrappers[GetDepth(wc)];
        }
        (wrapperData->initializeLeafCount)++;
      ...
      _XmProcessUnlock();
    }
    if (objectClassWrapper.initialize)
      (*objectClassWrapper.initialize)(req, new_w, args, num_args);
  }
}
```

The post-hook trick is the interesting part.  Xt will call the leaf
class's `core_class.initialize` *last*, so the root wrapper
**temporarily replaces that pointer** with a *leaf wrapper*, which
calls the real leaf `initialize`, then the post-hook, and restores the
pointer when the last concurrent creation of that class finishes
(`initializeLeafCount`, a reference count that handles a widget
creating widgets of its own class inside its `initialize`).

### Why there are ten `InitializeLeafWrapperN` functions

A C function pointer carries no closure, and the leaf wrapper must know
*which* class in the chain it was installed on, because Xt calls the
chain with the same arguments at every level and the wrapper may be
reached from a subclass of the class it was installed on (the subclass
inherits the patched pointer if it was created while the patch was
in place).  The solution is a small array of trampolines, one per
*depth* in the class hierarchy:

```c
static void InitializeLeafWrapper0(Widget req, Widget new_w, ArgList args, Cardinal *num_args)
{ InitializeLeafWrapper(req, new_w, args, num_args, 0); }
static void InitializeLeafWrapper1(...) { InitializeLeafWrapper(..., 1); }
...
```

and `InitializeLeafWrappers[GetDepth(wc)]` selects the one whose
constant equals the depth of the class being wrapped.  Inside,
`InitializeLeafWrapper` compares the constant with the depth of the
*actual* class and, if they differ, walks `depthDiff` superclass links
to find the class that holds the saved pointer.  There are 10 such
trampolines for `initialize` and `set_values`, 11 for `realize`, 14 for
`resize` and 13 for `geometry_manager` (the `realize`, `resize` and
`geometry_manager` wrappers serve different purposes: the realize
wrapper calls the VendorShell extension object's realize callback, the
resize wrapper re-evaluates the keyboard focus once per shell resize
through `_XmNavigResize`, and the geometry wrapper brackets a child's
geometry change with `XmDropSiteStartUpdate`/`XmDropSiteEndUpdate` so
that the drop-site database of chapter 6.2 follows it).  A Motif class hierarchy deeper than the array would
not work; the deepest in this tree (`XmTree`:
Core→Composite→Constraint→XmManager→XmHierarchy→XmTree) is well inside.

### Analysis

*Correctness.* The patched pointer is global mutable state in a class
record, protected by `_XmProcessLock`.  With threads initialised, two
application contexts creating widgets of the same class at the same
time serialise on that lock; the count makes nested creation safe.
Without threads, it is a plain counter.

*Cost.* Per widget creation: one extension lookup (a one- or two-entry
list walk), one `GetWrapperData` (a lookup keyed by class), two extra
indirect calls, and the lock test.  Measured as part of the
`rc-buttons` and `rc-gadgets` cases of `xmbench`, this is small
compared with the X requests of realisation.

*Fragility.* The mechanism depends on `objectClass` being the root of
every chain and on Xt's chaining order; both are specified.  It also
depends on `GetDepth` agreeing with the trampoline table, and on nobody
else patching the same pointers.  It is the kind of code that works
for thirty years and that nobody wants to touch, and the source says
so ("Might want to break up into per-class work that gets explicitly
chained. For right now, each class has to replicate all superclass
logic in hook routine.", `LabelG.c`).

*Alternative.* A toolkit designed today would put `pre`/`post` slots in
the class record or pass a continuation.  Within Xt, the only other
option is to require every Motif class to call `_XmPreInitialize()` and
`_XmPostInitialize()` by hand, which third-party subclasses would
forget.  The wrapper makes the hooks *inherited* and invisible, at the
price of the machinery above.

## 1.2.3 Fast subclassing

### The problem

`XtIsSubclass(w, class)` walks `core_class.superclass` from `XtClass(w)`
to the root, O(depth) pointer chases per test.  libXm tests the class
of a widget constantly: `XmIsGadget` in every manager's event dispatch,
`XmIsPrimitive`/`XmIsManager` in traversal, `XmIsTextField` in the
`XmText*` convenience functions, `XmIsPrintShell` in the image cache.

### The solution

The 32-byte `flags` array of the base class extension holds one bit
per Motif class.  The bit numbers are an enumeration in
[`XmP.h`](../../src/lib/Xm/XmP.h), starting at `XmCASCADE_BUTTON_BIT = 1`
and currently reaching `XmCOMBINATION_BOX_2_BIT`; applications may use
bits 192 to 255 (`XmFIRST_APPLICATION_SUBCLASS_BIT`), the rest are
reserved, because *bit numbers are ABI*.

```c
/* src/lib/Xm/BaseClass.c */
inline void _XmFastSubclassInit(WidgetClass wc, unsigned int bit)
{
  XmBaseClassExt *basePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);
  if (basePtr && (*basePtr))
    _XmSetFlagsBit(((*basePtr)->flags), bit);
}

inline Boolean _XmIsFastSubclass(WidgetClass wc, unsigned int bit)
{
  XmBaseClassExt *basePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);
  if (!basePtr || !(*basePtr))
    return False;
  return _XmGetFlagsBit(((*basePtr)->flags), bit) ? True : False;
}
/* src/lib/Xm/XmP.h */
#  define XmIsContainer(w) (_XmIsFastSubclass(XtClass(w), XmCONTAINER_BIT))
```

Each class's `ClassPartInitialize` sets *its own* bit only:

```c
/* src/lib/Xm/Primitive.c */   _XmFastSubclassInit(w, XmPRIMITIVE_BIT);
/* src/lib/Xm/PushB.c */       _XmFastSubclassInit(wc, XmPUSH_BUTTON_BIT);
```

The accumulation that makes `XmIsPrimitive(pushButton)` true is a
consequence of Xt's chaining rule for `class_part_initialize`: when
`xmPushButtonClassRec` is initialised, Xt calls `Primitive`'s
`ClassPartInitialize` *on the PushButton class record*, then `Label`'s,
then `PushButton`'s, and each sets its bit on the same `flags` array.
The flags of a class are therefore the union of the bits of all its
Motif ancestors, computed once, at class initialisation, with no
explicit copying (`ClassPartInitRootWrapper` zeroes the array first so
that a static initialiser cannot leave garbage).

### Analysis

| | `XtIsSubclass` | `_XmIsFastSubclass` |
|---|---|---|
| Time | O(depth) | O(1): one extension-list walk (one or two nodes), one bit test |
| Space | none | 32 bytes per class |
| Works for | any class | classes whose `ClassPartInitialize` sets a bit, and their subclasses |
| Before class init | correct | returns False (flags not yet set) |
| ABI | none | bit numbers are frozen; adding a class takes the next free bit |

A third-party widget that subclasses, say, `XmLabel` without calling
`_XmFastSubclassInit` still answers `XmIsLabel` correctly, because
`XmLabel`'s `ClassPartInitialize` chains onto it.  What it cannot do is
answer `XmIsMyWidget`, unless it takes an application bit.

## 1.2.4 Traits

### The problem

Single inheritance cannot express "any widget that can give me its
text", which `XmComboBox`, `XmSpinBox` and `XmSelectionBox` need for a
child that may be a `XmTextField`, a `XmText` or a `XmDataField`; nor
"any widget that can be scrolled by a ScrolledWindow", nor "any widget
that can be activated by a default button".  Motif 1.x solved these
with chains of `XmIsXxx` tests; Motif 2.0 introduced **traits**: named
interfaces, implemented by a static record of function pointers,
installed per class, looked up by (class, name).

### Names and records

A trait is identified by a quark.  [`Trait.c`](../../src/lib/Xm/Trait.c)
defines 25 of them, interned in `_XmInitializeTraits`; the public ones
have a header `Xm/<Name>T.h` that declares the record.  Two of them, as
used by the SpinBox in chapter 4:

```c
/* src/lib/Xm/AccTextT.h: "give me / set your value as text" */
typedef struct _XmAccessTextualTraitRec {
  int version; /* 0 */
  XmAccessTextualGetValuesProc getValue;
  XmAccessTextualSetValuesProc setValue;
  XmAccessTextualPreferredProc preferredFormat;
} XmAccessTextualTraitRec, *XmAccessTextualTrait;

/* src/lib/Xm/NavigatorT.h: "I can be driven like a scrollbar" */
typedef struct _XmNavigatorTraitRec {
  int version; /* 0 */
  XmNavigatorMoveCBProc changeMoveCB;
  XmNavigatorSetValueProc setValue;
  XmNavigatorGetValueProc getValue;
} XmNavigatorTraitRec, *XmNavigatorTrait;
```

The `version` field is the extension point: a later version may append
members, and a consumer checks the version before using them.
Every record in the tree is version 0 except the scroll-frame trait
(`ScrollFrameT.h`), which is version 1.

The full set, by purpose: `XmQTaccessTextual`, `XmQTaccessColors`,
`XmQTspecifyRenderTable`, `XmQTspecifyUnitType`,
`XmQTspecifyLayoutDirection`, `XmQTspecifyUnhighlight` (queries of a
widget's properties); `XmQTactivatable`, `XmQTtakesDefault`,
`XmQTtraversalControl`, `XmQTpointIn` (behaviour a parent needs from a
child); `XmQTnavigator`, `XmQTscrollFrame`, `_XmQTclipWindow`
(scrolling); `XmQTcontainer`, `XmQTcontainerItem`, `XmQTjoinSide`
(container layouts); `XmQTmenuSystem`, `XmQTmenuSavvy`,
`XmQTdialogShellSavvy`, `XmQTcareParentVisual` (menus, dialogs, visual
changes); `XmQTtransfer` (the uniform transfer model of drag and drop,
clipboard and selections); `XmQTtoolTip`, `XmQTtoolTipConfig`;
`XmQTmotifTrait` (a marker).

### Installing and using a trait

A class installs its traits in `ClassPartInitialize`, with its own class
as the key:

```c
/* src/lib/Xm/SpinB.c */
static XmConst XmNavigatorTraitRec spinBoxNT = { 0, SpinNChangeMoveCB, SpinNSetValue, SpinNGetValue };
...
  XmeTraitSet((XtPointer)spinC, XmQTnavigator, (XtPointer)&spinBoxNT);
```

A consumer asks the *class* of a widget and calls through the record:

```c
/* src/lib/Xm/SpinB.c, UpdateChildText */
  textT = (XmAccessTextualTrait)XmeTraitGet((XtPointer)XtClass(textW), XmQTaccessTextual);
  if (textT == NULL)
    return;
  ...
  textT->setValue(textW, (XtPointer)buffer, XmFORMAT_MBYTE);
```

Note what the SpinBox does *not* do: it never tests `XmIsTextField`.
Any child whose class installs `XmQTaccessTextual` works, which is how a
`XmText` or an application widget can be a SpinBox child.

**Traits are not inherited.** `XmeTraitGet` looks up the exact class
pointer it is given.  A subclass that wants its superclass's trait must
install it again, as `XmDataField` does
(chapter 3.1):

```c
/* src/lib/Xm/DataF.c */
  /* XmTextField sets its traits on its own class only. */
  XmeTraitSet((XtPointer)w_class, XmQTaccessTextual,
              XmeTraitGet((XtPointer)xmTextFieldWidgetClass, XmQTaccessTextual));
```

This is a deliberate design choice with a real cost (every subclass
author must know it) and a real benefit (a subclass can *refuse* a
trait, and a lookup is one hash probe instead of a walk).  The upstream
DataField, which was a copy of TextField rather than a subclass, had a
related bug fixed in this tree: it installed "a pointer to a local
variable as its transfer trait" ([CHANGELOG](../../CHANGELOG.md)).

### The trait table

All traits of all classes live in one process-wide table, keyed by the
pair (object pointer, quark).  In this tree it is an open-addressing
hash table with linear probing and tombstones:

```c
/* src/lib/Xm/Trait.c */
typedef struct _XmTraitSlot { XtPointer obj; XrmQuark name; XtPointer data; } XmTraitSlotRec, *XmTraitSlot;
#define TRAIT_INITIAL_SIZE 512 /* a power of two */
static XmTraitSlot TraitSlots;
static Cardinal TraitMask;   /* size - 1 */
static Cardinal TraitInUse;  /* slots holding a trait */
static Cardinal TraitFilled; /* slots in use or holding a tombstone */

static Cardinal TraitHash(XtPointer obj, XrmQuark name)
{
  /* Objects come from malloc or are static classes: mix in the high
   * bits, the low ones are mostly alignment. */
  unsigned long h = (unsigned long)obj;
  h ^= h >> 15;
  h += (unsigned long)name * 0x9e3779b1UL;
  h ^= h >> 13;
  h *= 0x85ebca6bUL;
  h ^= h >> 16;
  return (Cardinal)h;
}

static XmTraitSlot TraitFind(XtPointer obj, XrmQuark name, Boolean for_insert)
{
  Cardinal i = TraitHash(obj, name) & TraitMask;
  XmTraitSlot tomb = NULL;
  for (;; i = (i + 1) & TraitMask) {
    XmTraitSlot slot = &TraitSlots[i];
    if (slot->data != NULL) {
      if (slot->obj == obj && slot->name == name)
        return slot;
    }
    else if (slot->obj == TOMBSTONE) {
      if (!tomb) tomb = slot;
    }
    else
      return for_insert ? (tomb ? tomb : slot) : NULL;
  }
}
```

Design points:

- **Hash function.** Class pointers are 8- or 16-byte aligned addresses
  in the data segment, so their low bits carry no information; the
  function folds the high bits down and mixes with two odd constants
  (the golden-ratio constant `0x9e3779b1` and a MurmurHash3 finaliser
  constant), a standard recipe for pointer keys.
- **Load factor.** `XmeTraitSet` keeps `(TraitFilled + 1) * 4 <= (TraitMask + 1) * 3`,
  that is, live slots plus tombstones under 75 %.  When the bound is
  reached it doubles the table if more than half of it holds live
  traits, otherwise it rebuilds at the same size to drop tombstones.
  Expected probe length for a successful lookup with linear probing at
  load α is about ½(1 + 1/(1 − α)), which is 2.5 at α = 0.75; the
  actual load in a Motif program is far lower (a few hundred traits in
  512 or 1 024 slots).
- **Deletion** (`XmeTraitSet` with `data == NULL`, or the
  `XmeTraitRemove` macro) writes a tombstone, so that probe chains
  through the removed slot stay intact.
- **Locking.** Both operations take `_XmProcessLock`.  `XmeTraitGet` is
  on hot paths (every `XmIsXxx`-style capability test, every default
  button update), so the lock test of chapter 1 §1.7 matters here.

The `trait-get` case of `xmbench` measures "XmeTraitGet, hits and
misses"; the [CHANGELOG](../../CHANGELOG.md) records that traits "use
cheaper lookups" than before.

### Analysis

| Property | Benefit | Cost or limitation |
|----------|---------|--------------------|
| Interface by name | Any class, in or out of libXm, can implement a trait; consumers need no class test. | A name is a string; a typo compiles. Trait records are untyped `XtPointer`s cast on use. |
| Per-class installation | Exact, explicit; a subclass can drop a trait. | Not inherited: every subclass re-installs (DataField). |
| Central table | One lookup path; traits on non-widget objects (`XmScreen`, `XmDisplay`) work the same way. | Global mutable state; needs the process lock; cannot be freed per display. |
| Version field | Records can grow compatibly. | Only the scroll-frame trait has ever used it (version 1); every other record is version 0. |

## 1.2.5 Putting the three together: creating a `XmLabelGadget`

1. `XtCreateWidget` sees `class_inited == False` for `xmLabelGadgetClass`
   and runs class initialisation: Xt chains `class_part_initialize`
   from `Object` down.  `ClassPartInitRootWrapper` (installed on
   `objectClass` by `_XmInitializeExtensions`) creates or completes the
   base extension record, resolves the `XmInherit*` hooks from
   `XmGadget`'s record, and zeroes `flags`.  `XmGadget`'s and
   `XmLabelGadget`'s own `ClassPartInitialize` set `XmGADGET_BIT` and
   `XmLABEL_GADGET_BIT`, install the `accessColors`, `careParentVisual`
   and `specifyRenderTable` traits, and compile the cache class.
2. Xt allocates the instance record and fills the resources.  It then
   calls the `initialize` chain, whose first link is
   `InitializeRootWrapper`: it runs `XmLabelGadget`'s `initializePrehook`
   (allocate a scratch cache object, apply the cache resources to it),
   patches the leaf `initialize`, and calls `Object`'s real `initialize`.
3. The chain runs `RectObj`, `XmGadget`, `XmLabelGadget`'s `initialize`
   (compute the label's size from the font in the scratch cache part).
   The last call lands in `InitializeLeafWrapperN`, which calls the real
   leaf method, then `InitializePosthook`: intern the scratch part in
   the shared cache (`_XmCachePart`), point the gadget at the shared
   copy, free the scratch objects, restore the class pointer.
4. From now on, `XmIsGadget(w)` is one bit test and the parent's event
   dispatch (chapter 3) can find the gadget in O(1) per test; and
   `XmeTraitGet(XtClass(w), XmQTaccessColors)` is one hash probe.

## References

- [`src/lib/Xm/BaseClass.c`](../../src/lib/Xm/BaseClass.c),
  [`BaseClassP.h`](../../src/lib/Xm/BaseClassP.h),
  [`BaseClassI.h`](../../src/lib/Xm/BaseClassI.h)
- [`src/lib/Xm/Trait.c`](../../src/lib/Xm/Trait.c),
  [`TraitP.h`](../../src/lib/Xm/TraitP.h),
  [`AccTextT.h`](../../src/lib/Xm/AccTextT.h),
  [`NavigatorT.h`](../../src/lib/Xm/NavigatorT.h)
- [`src/lib/Xm/LabelG.c`](../../src/lib/Xm/LabelG.c) (cache object hooks),
  [`VendorS.c`](../../src/lib/Xm/VendorS.c) (shell extension hooks),
  [`Cache.c`](../../src/lib/Xm/Cache.c) (`_XmCachePart`)
- Linear probing analysis: D. E. Knuth, *The Art of Computer
  Programming*, vol. 3, §6.4; summary at
  <https://en.wikipedia.org/wiki/Linear_probing>
- Xt specification, §1.6.12 "Class Extension Records":
  <https://www.x.org/releases/current/doc/libXt/intrinsics.html>
