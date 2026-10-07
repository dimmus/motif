/* $TOG: Trait.c /main/9 1997/07/07 11:36:10 cshi $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * HISTORY
 */
/* #define DEBUG */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "TraitI.h"
#include "XmI.h"
#include <X11/IntrinsicP.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xm/TraitP.h>
#include <Xm/VendorSP.h>
#include <Xm/XmP.h>
#include <stdatomic.h>
/*
 * Internal data structures
 *
 * Traits are installed on widget classes and, for tool tips, on widget
 * instances, and are looked up very often (XmeTraitGet has well over a
 * hundred callers, many of them on every child or every event).  They
 * live in an open addressing hash table keyed by (object, trait name),
 * with linear probing in a power of two sized array that grows as it
 * fills, so that a lookup, hit or miss, is a few probes into one array.
 * A slot is in use when its data is not NULL; removing a trait shifts
 * the slots after it back (there are no tombstones).
 *
 * XmeTraitSet changes the table under the process lock, but XmeTraitGet
 * does not take it: traits are mostly set once, when a class is
 * initialized, and read all the time, from every thread of a threaded
 * program.  A reader validates its lookup with a sequence count instead
 * (a seqlock): a writer makes TraitSeq odd, changes slots in place and
 * makes it even again, and a reader that saw it odd, or changed by the
 * time its probe ended, repeats the lookup under the lock.  The slots
 * are atomics, so a reader racing with a writer reads whole words, and
 * the writer's release stores order its odd TraitSeq before any slot
 * change a reader can see.
 *
 * Growing the table copies the traits into a new array before it is
 * published, but a reader may still be probing the old one, which is
 * therefore never freed: it is kept on the new table's retired list
 * (which also keeps leak checkers from reporting it).  Since the table only ever doubles, the retired arrays together are
 * smaller than the current one.
 */
typedef struct _XmTraitSlot {
  _Atomic(XtPointer) obj;
  _Atomic(XrmQuark) name;
  _Atomic(XtPointer) data;
} XmTraitSlotRec, *XmTraitSlot;

typedef struct _XmTraitTable {
  struct _XmTraitTable *retired; /* the previous, smaller table */
  Cardinal mask;                 /* size - 1 */
  XmTraitSlotRec slots[];
} XmTraitTableRec, *XmTraitTable;

#define TRAIT_INITIAL_SIZE 512 /* a power of two */

static _Atomic(XmTraitTable) TraitTable;
static atomic_ulong TraitSeq; /* odd while XmeTraitSet changes slots */
static Cardinal TraitInUse;   /* slots holding a trait, under the lock */

/*
 * Static functions
 */
static Cardinal TraitHash(XtPointer obj, XrmQuark name);
static XtPointer TraitLookup(XmTraitTable table, XtPointer obj, XrmQuark name);
static Cardinal TraitFind(XmTraitTable table, XtPointer obj, XrmQuark name);
static void TraitStore(XmTraitSlot slot, XtPointer obj, XrmQuark name, XtPointer data);
static XmTraitTable TraitGrow(XmTraitTable old);
static void TraitDelete(XmTraitTable table, Cardinal i);
/*
 * List all quarks here
 */
externaldef(traits) XrmQuark XmQTmotifTrait = NULLQUARK;
externaldef(traits) XrmQuark XmQTmenuSystem = NULLQUARK;
externaldef(traits) XrmQuark XmQTtransfer = NULLQUARK;
externaldef(traits) XrmQuark XmQTaccessTextual = NULLQUARK;
externaldef(traits) XrmQuark XmQTmenuSavvy = NULLQUARK;
externaldef(traits) XrmQuark XmQTnavigator = NULLQUARK;
externaldef(traits) XrmQuark XmQTscrollFrame = NULLQUARK;
externaldef(traits) XrmQuark XmQTactivatable = NULLQUARK;
externaldef(traits) XrmQuark XmQTdialogShellSavvy = NULLQUARK;
externaldef(traits) XrmQuark XmQTjoinSide = NULLQUARK;
externaldef(traits) XrmQuark XmQTcareParentVisual = NULLQUARK;
externaldef(traits) XrmQuark XmQTspecifyRenderTable = NULLQUARK;
externaldef(traits) XrmQuark XmQTtakesDefault = NULLQUARK;
externaldef(traits) XrmQuark XmQTcontainerItem = NULLQUARK;
externaldef(traits) XrmQuark XmQTcontainer = NULLQUARK;
externaldef(traits) XrmQuark XmQTspecifyLayoutDirection = NULLQUARK;
externaldef(traits) XrmQuark XmQTaccessColors = NULLQUARK;
externaldef(traits) XrmQuark XmQTspecifyUnitType = NULLQUARK;
externaldef(traits) XrmQuark XmQTtraversalControl = NULLQUARK;
externaldef(traits) XrmQuark XmQTspecifyUnhighlight = NULLQUARK;
externaldef(traits) XrmQuark XmQTpointIn = NULLQUARK;
externaldef(traits) XrmQuark _XmQTclipWindow = NULLQUARK;
externaldef(traits) XrmQuark XmQTtoolTipConfig = NULLQUARK;
externaldef(traits) XrmQuark XmQTtoolTip = NULLQUARK;

/*
 * Initialize traits system
 *
 * This routine sets up all quarks used by the traits in
 * Motif
 */
void _XmInitializeTraits(void)
{
  static Boolean initialized = False;
  /* avoid initializing more than once */
  if (initialized)
    return;
  initialized = True;
  XmQTmotifTrait = XrmPermStringToQuark("XmQTmotifTrait");
  /* Menu system manipulation and status */
  XmQTmenuSystem = XrmPermStringToQuark("XmTmenuSystem");
  XmQTmenuSavvy = XrmPermStringToQuark("XmTmenuSavvy");
  /* Transfer Trait */
  XmQTtransfer = XrmPermStringToQuark("XmTtransfer");
  /* String get/set */
  XmQTaccessTextual = XrmPermStringToQuark("XmTaccessTextual");
  /* Navigator/Scrolling trait */
  XmQTnavigator = XrmPermStringToQuark("XmTnavigator");
  XmQTscrollFrame = XrmPermStringToQuark("XmTscrollFrame");
  _XmQTclipWindow = XrmPermStringToQuark("_XmTclipWindow");
  /* Activatable trait */
  XmQTactivatable = XrmPermStringToQuark("XmTactivatable");
  /* JoinSide trait */
  XmQTjoinSide = XrmPermStringToQuark("XmTjoinSide");
  /* DialogShellSavvy trait */
  XmQTdialogShellSavvy = XrmPermStringToQuark("XmTdialogShellSavvy");
  /* Care about Parent Visual trait */
  XmQTcareParentVisual = XrmPermStringToQuark("XmTcareParentVisual");
  /* SpecifyRenderTable trait */
  XmQTspecifyRenderTable = XrmPermStringToQuark("XmTspecifyRenderTable");
  /* TakesDefault trait */
  XmQTtakesDefault = XrmPermStringToQuark("XmTtakesDefault");
  /* Container/Item trait */
  XmQTcontainerItem = XrmPermStringToQuark("XmTcontainerItem");
  XmQTcontainer = XrmPermStringToQuark("XmTcontainer");
  /* LayoutDirection trait */
  XmQTspecifyLayoutDirection = XrmPermStringToQuark("XmTspecifyLayoutDirection");
  /* get colors */
  XmQTaccessColors = XrmPermStringToQuark("XmTaccessColors");
  /* Unit type */
  XmQTspecifyUnitType = XrmPermStringToQuark("XmTspecifyUnitType");
  /* Traversal control. */
  XmQTtraversalControl = XrmPermStringToQuark("XmTtraversalControl");
  /* Specify UnHighlight GC trait */
  XmQTspecifyUnhighlight = XrmPermStringToQuark("XmTspecifyUnhighlight");
  /* PointIn trait */
  XmQTpointIn = XrmPermStringToQuark("XmTpointIn");
  /* ToolTip traits */
  XmQTtoolTipConfig = XrmPermStringToQuark("XmTtoolTipConfig");
  XmQTtoolTip = XrmPermStringToQuark("XmTtoolTip");
}

XtPointer XmeTraitGet(XtPointer obj, XrmQuark name)
{
  XtPointer trait;
  unsigned long seq = atomic_load_explicit(&TraitSeq, memory_order_acquire);
  if (!(seq & 1)) {
    trait = TraitLookup(atomic_load_explicit(&TraitTable, memory_order_acquire), obj, name);
    /* The slot loads are acquires, so this load is not done before them. */
    if (atomic_load_explicit(&TraitSeq, memory_order_relaxed) == seq)
      return trait;
  }
  /* XmeTraitSet is changing the table: wait for it. */
  _XmProcessLock();
  trait = TraitLookup(atomic_load_explicit(&TraitTable, memory_order_relaxed), obj, name);
  _XmProcessUnlock();
  return (trait);
}

Boolean XmeTraitSet(XtPointer object, XrmQuark name, XtPointer data)
{
  XmTraitTable table;
  Cardinal i;
  unsigned long seq;
  _XmProcessLock();
  table = atomic_load_explicit(&TraitTable, memory_order_relaxed);
  /* Keep the load under 3/4. */
  if (data != NULL && (!table || (TraitInUse + 1) * 4 > (table->mask + 1) * 3))
    table = TraitGrow(table);
  if (table != NULL) {
    i = TraitFind(table, object, name);
    if (data != NULL || atomic_load_explicit(&table->slots[i].data, memory_order_relaxed)) {
      seq = atomic_load_explicit(&TraitSeq, memory_order_relaxed);
      atomic_store_explicit(&TraitSeq, seq + 1, memory_order_relaxed);
      if (data == NULL) {
        /* if data == NULL then remove the trait */
        TraitDelete(table, i);
        TraitInUse--;
      }
      else {
        if (atomic_load_explicit(&table->slots[i].data, memory_order_relaxed) == NULL)
          TraitInUse++;
        TraitStore(&table->slots[i], object, name, data);
      }
      atomic_store_explicit(&TraitSeq, seq + 2, memory_order_release);
    }
  }
  _XmProcessUnlock();
  return True;
}

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

/*
 * Return the trait stored for (obj, name) in table, or NULL.  The probe
 * ends at an empty slot, of which there always is one, and at the
 * latest after visiting every slot, in case a writer is moving them.
 */
static XtPointer TraitLookup(XmTraitTable table, XtPointer obj, XrmQuark name)
{
  Cardinal i, n;
  if (table == NULL)
    return NULL;
  i = TraitHash(obj, name) & table->mask;
  for (n = 0; n <= table->mask; n++, i = (i + 1) & table->mask) {
    XmTraitSlot slot = &table->slots[i];
    XtPointer data = atomic_load_explicit(&slot->data, memory_order_acquire);
    if (data == NULL)
      break;
    if (atomic_load_explicit(&slot->obj, memory_order_acquire) == obj &&
        atomic_load_explicit(&slot->name, memory_order_acquire) == name)
      return data;
  }
  return NULL;
}

/*
 * Return the index of the slot that holds (obj, name), or of the empty
 * slot that ended the probe if there is none.  Called under the lock.
 */
static Cardinal TraitFind(XmTraitTable table, XtPointer obj, XrmQuark name)
{
  Cardinal i = TraitHash(obj, name) & table->mask;
  for (;; i = (i + 1) & table->mask) {
    XmTraitSlot slot = &table->slots[i];
    if (atomic_load_explicit(&slot->data, memory_order_relaxed) == NULL ||
        (atomic_load_explicit(&slot->obj, memory_order_relaxed) == obj &&
         atomic_load_explicit(&slot->name, memory_order_relaxed) == name))
      return i;
  }
}

/*
 * Write a slot of the published table.  The release stores keep the
 * writer's odd TraitSeq, stored before, ahead of them for any reader.
 */
static void TraitStore(XmTraitSlot slot, XtPointer obj, XrmQuark name, XtPointer data)
{
  atomic_store_explicit(&slot->obj, obj, memory_order_release);
  atomic_store_explicit(&slot->name, name, memory_order_release);
  atomic_store_explicit(&slot->data, data, memory_order_release);
}

/*
 * Empty slot i and move back the traits after it that could no longer
 * be reached past the gap (backward shift deletion for linear probing).
 */
static void TraitDelete(XmTraitTable table, Cardinal i)
{
  Cardinal j, home, mask = table->mask;
  for (j = (i + 1) & mask;; j = (j + 1) & mask) {
    XmTraitSlot slot = &table->slots[j];
    XtPointer data = atomic_load_explicit(&slot->data, memory_order_relaxed);
    XtPointer obj;
    XrmQuark name;
    if (data == NULL)
      break;
    obj = atomic_load_explicit(&slot->obj, memory_order_relaxed);
    name = atomic_load_explicit(&slot->name, memory_order_relaxed);
    home = TraitHash(obj, name) & mask;
    /* Leave it if its home is cyclically in (i, j]. */
    if (((j - home) & mask) < ((j - i) & mask))
      continue;
    TraitStore(&table->slots[i], obj, name, data);
    i = j;
  }
  TraitStore(&table->slots[i], NULL, NULLQUARK, NULL);
}

/*
 * Publish a table twice the size of old (or the initial one) holding
 * its traits, and return it.  Called under the lock; old stays valid.
 */
static XmTraitTable TraitGrow(XmTraitTable old)
{
  Cardinal size = old ? (old->mask + 1) * 2 : TRAIT_INITIAL_SIZE;
  Cardinal i;
  XmTraitTable table = (XmTraitTable)XtCalloc(1, sizeof(XmTraitTableRec) + size * sizeof(XmTraitSlotRec));
  table->retired = old;
  table->mask = size - 1;
  for (i = 0; old && i <= old->mask; i++) {
    XmTraitSlot from = &old->slots[i];
    XtPointer data = atomic_load_explicit(&from->data, memory_order_relaxed);
    if (data != NULL) {
      XtPointer obj = atomic_load_explicit(&from->obj, memory_order_relaxed);
      XrmQuark name = atomic_load_explicit(&from->name, memory_order_relaxed);
      XmTraitSlot to = &table->slots[TraitFind(table, obj, name)];
      atomic_store_explicit(&to->obj, obj, memory_order_relaxed);
      atomic_store_explicit(&to->name, name, memory_order_relaxed);
      atomic_store_explicit(&to->data, data, memory_order_relaxed);
    }
  }
  /* Readers that load the new table see it filled in. */
  atomic_store_explicit(&TraitTable, table, memory_order_release);
  return table;
}
