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
/*
 * Internal data structures
 *
 * Traits are installed on widget classes and, for tool tips, on widget
 * instances, and are looked up very often (XmeTraitGet has well over a
 * hundred callers, many of them on every child or every event).  They
 * live in an open addressing hash table keyed by (object, trait name),
 * with linear probing in a power of two sized array that grows as it
 * fills, so that a lookup, hit or miss, is a few probes into one array.
 *
 * A slot is in use when its data is not NULL (XmeTraitSet with NULL data
 * removes the trait).  A removed slot keeps the tombstone marker as its
 * object so that probing goes on past it.
 */
typedef struct _XmTraitSlot {
  XtPointer obj;
  XrmQuark name;
  XtPointer data;
} XmTraitSlotRec, *XmTraitSlot;

#define TRAIT_INITIAL_SIZE 512 /* a power of two */

static XmTraitSlot TraitSlots;
static Cardinal TraitMask;   /* size - 1 */
static Cardinal TraitInUse;  /* slots holding a trait */
static Cardinal TraitFilled; /* slots in use or holding a tombstone */
static char TraitTombstone;
#define TOMBSTONE ((XtPointer)&TraitTombstone)

/*
 * Static functions
 */
static Cardinal TraitHash(XtPointer obj, XrmQuark name);
static XmTraitSlot TraitFind(XtPointer obj, XrmQuark name, Boolean for_insert);
static void TraitResize(Cardinal size);
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
  /* Create Hash Table */
  if (TraitSlots == NULL)
    TraitResize(TRAIT_INITIAL_SIZE);
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
  XtPointer trait = NULL;
  XmTraitSlot slot;
  _XmProcessLock();
  if (TraitSlots && (slot = TraitFind(obj, name, False)) != NULL)
    trait = slot->data;
  _XmProcessUnlock();
  return (trait);
}

Boolean XmeTraitSet(XtPointer object, XrmQuark name, XtPointer data)
{
  XmTraitSlot slot;
  _XmProcessLock();
  if (data != NULL) {
    /* Keep the load under 3/4, counting tombstones. */
    if ((TraitFilled + 1) * 4 > (TraitMask + 1) * 3 || !TraitSlots) {
      Cardinal size = TraitSlots ? TraitMask + 1 : TRAIT_INITIAL_SIZE;
      /* Grow if mostly full of traits, else just drop the tombstones. */
      if ((TraitInUse + 1) * 2 > size)
        size *= 2;
      TraitResize(size);
    }
    slot = TraitFind(object, name, True);
    if (slot->data == NULL) {
      if (slot->obj != TOMBSTONE)
        TraitFilled++;
      TraitInUse++;
      slot->obj = object;
      slot->name = name;
    }
    slot->data = data;
  }
  else if (TraitSlots && (slot = TraitFind(object, name, False)) != NULL) {
    /* if data == NULL then remove the trait */
    slot->obj = TOMBSTONE;
    slot->name = NULLQUARK;
    slot->data = NULL;
    TraitInUse--;
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
 * Return the slot that holds (obj, name), or NULL if there is none; with
 * for_insert, return the slot to store it in instead of NULL (the first
 * tombstone met, else the empty slot that ended the probe).  The table
 * always has an empty slot, so the probe terminates.
 */
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
      if (!tomb)
        tomb = slot;
    }
    else
      return for_insert ? (tomb ? tomb : slot) : NULL;
  }
}

/* Move the traits into a new array of size slots (a power of two). */
static void TraitResize(Cardinal size)
{
  XmTraitSlot old = TraitSlots;
  Cardinal i, old_size = old ? TraitMask + 1 : 0;
  TraitSlots = (XmTraitSlot)XtCalloc(size, sizeof(XmTraitSlotRec));
  TraitMask = size - 1;
  TraitInUse = TraitFilled = 0;
  for (i = 0; i < old_size; i++)
    if (old[i].data != NULL) {
      XmTraitSlot slot = TraitFind(old[i].obj, old[i].name, True);
      *slot = old[i];
      TraitInUse++;
      TraitFilled++;
    }
  XtFree((char *)old);
}
