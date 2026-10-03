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
#ifdef REV_INFO
#  ifndef lint
static char rcsid[] = "$XConsortium: Cache.c /main/12 1995/07/14 10:12:26 drk $"
#  endif
#endif
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "CacheI.h"
#include <Xm/GadgetP.h>
    /********    Static Function Declarations    ********/
    /********    End Static Function Declarations    ********/
    /************************************************************************
     *
     *  _XmCacheDelete
     *	Delete an existing cache record.  NOTE: <data> is a pointer to the
     *      fourth field in the cache record - It is *not* a pointer to the
     *	cache record itself!
     *
     ************************************************************************/
    void
    _XmCacheDelete(XtPointer data)
{
  XmGadgetCachePtr ptr;
  ptr = (XmGadgetCachePtr)DataToGadgetCache(data);
  if (--ptr->ref_count <= 0) {
    (ptr->prev)->next = ptr->next;
    if (ptr->next) /* not the last record */
      (ptr->next)->prev = ptr->prev;
    XtFree((char *)ptr);
  }
}

/************************************************************************
 *
 *  _XmCacheCopy
 *	Copy <size> bytes from <src> to <dest>.
 *
 ************************************************************************/
void _XmCacheCopy(XtPointer src, XtPointer dest, size_t size)
{
  memcpy(dest, src, size);
}

/************************************************************************
 *
 *  _XmCachePart
 *	Pass in a pointer, <cpart>, to <size> bytes of a temporary Cache
 *	record.
 *	- Run through the class linked list.
 *	  = If a match is found, increment the ref_count, move the record
 *	    to the front of the list and return the address.
 *	  = Else, allocate a new cache record, copy in temporary Cache bytes,
 *	    put it at the front of the class-cache linked list, and return
 *	    the address.
 *
 ************************************************************************/
XtPointer _XmCachePart(XmCacheClassPartPtr cp, XtPointer cpart, size_t size)
{
  XmGadgetCachePtr head = &ClassCacheHead(cp);
  XmGadgetCachePtr ptr;
  /*
   * The records are kept most recently used first: gadgets tend to be
   * created and changed in runs that share their cache part, so the
   * search for one usually stops at the first record.
   */
  for (ptr = head->next; ptr; ptr = ptr->next) {
    if ((ClassCacheCompare(cp)(cpart, CacheDataPtr(ptr)))) {
      ptr->ref_count++;
      if (ptr != head->next) {
        /* Unlink, then put it back at the front. */
        ptr->prev->next = ptr->next;
        if (ptr->next)
          ptr->next->prev = ptr->prev;
        ptr->next = head->next;
        ptr->next->prev = ptr;
        ptr->prev = head;
        head->next = ptr;
      }
      return ((XtPointer)CacheDataPtr(ptr));
    }
  }
  /* Malloc a new rec at the front, fill it out */
  ptr = (XmGadgetCachePtr)XtMalloc(size + XtOffsetOf(XmGadgetCacheRef, data));
  ClassCacheCopy(cp)(cpart, CacheDataPtr(ptr), size);
  ptr->ref_count = 1;
  ptr->next = head->next;
  if (ptr->next)
    ptr->next->prev = ptr;
  ptr->prev = head;
  head->next = ptr;
  return (CacheDataPtr(ptr));
}
