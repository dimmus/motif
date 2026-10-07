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
#include <stdint.h>

/*
 * The records of a cache class are kept in a doubly linked list headed
 * by the class's cache_head, which the installed headers expose.
 *
 * A class that registers a hash proc with _XmCacheSetHashProc also gets
 * its records indexed by that hash, so that _XmCachePart finds a part in
 * constant time however many distinct parts the class has.  The index
 * lives here, out of the installed structures: a node at the end of each
 * indexed record is chained both in a table keyed by the hash of the
 * contents and in a table keyed by the address of the record, which is
 * how _XmCacheDelete, given only the data, finds and unlinks it.  The
 * records of the other classes (and those the gadgets free themselves,
 * like CascadeButtonGadget's arrow pixmaps) are searched linearly, most
 * recently used first, as before.
 *
 * A record is only ever returned when the compare proc matches it.  A
 * part that a gadget changes in place once it is cached (as
 * ToggleButtonGadget's SetValues can) stays under the hash of its old
 * contents, so a later search for its new contents makes a record of its
 * own instead of sharing it: that costs memory, never correctness.
 *
 * Like the lists, the index is global state, used under _XmProcessLock.
 */

typedef struct _CacheClass {
  XmCacheClassPartPtr cp;
  XmCacheHashProc hash;
  struct _CacheClass *next;
} CacheClass;

typedef struct _CacheNode {
  struct _CacheNode *next_by_data; /* in data_table */
  struct _CacheNode *next_by_addr; /* in addr_table */
  XmGadgetCachePtr rec;
  CacheClass *cls;
  unsigned int hash; /* of the contents, mixed with the class */
} CacheNode;

static CacheClass *hashed_classes;
static CacheNode **data_table;
static CacheNode **addr_table;
static unsigned int table_size; /* a power of two or 0, never shrinks */
static unsigned int node_count;

/********    Static Function Declarations    ********/
static CacheClass *FindClass(XmCacheClassPartPtr cp);
static unsigned int MixHash(unsigned int h);
static unsigned int AddrHash(XmGadgetCachePtr rec);
static void GrowTables(void);
static void AddNode(CacheNode *node);
static void RemoveNode(XmGadgetCachePtr rec);
static void LinkFirst(XmGadgetCachePtr head, XmGadgetCachePtr ptr);
/********    End Static Function Declarations    ********/

static CacheClass *FindClass(XmCacheClassPartPtr cp)
{
  CacheClass *cls;
  for (cls = hashed_classes; cls; cls = cls->next)
    if (cls->cp == cp)
      return cls;
  return NULL;
}

/* The final mix of MurmurHash3, so that the low bits index the tables. */
static unsigned int MixHash(unsigned int h)
{
  h ^= h >> 16;
  h *= 0x85ebca6bu;
  h ^= h >> 13;
  h *= 0xc2b2ae35u;
  h ^= h >> 16;
  return h;
}

static unsigned int AddrHash(XmGadgetCachePtr rec)
{
  uintptr_t a = (uintptr_t)rec;
  return MixHash((unsigned int)(a >> 4) ^ (unsigned int)((a >> 16) >> 16));
}

static void GrowTables(void)
{
  unsigned int size = table_size ? table_size * 2 : 64;
  CacheNode **data = (CacheNode **)XtCalloc(size, sizeof(CacheNode *));
  CacheNode **addr = (CacheNode **)XtCalloc(size, sizeof(CacheNode *));
  CacheNode *node, *next;
  unsigned int i, b;
  for (i = 0; i < table_size; i++) {
    for (node = data_table[i]; node; node = next) {
      next = node->next_by_data;
      b = node->hash & (size - 1);
      node->next_by_data = data[b];
      data[b] = node;
    }
    for (node = addr_table[i]; node; node = next) {
      next = node->next_by_addr;
      b = AddrHash(node->rec) & (size - 1);
      node->next_by_addr = addr[b];
      addr[b] = node;
    }
  }
  XtFree((char *)data_table);
  XtFree((char *)addr_table);
  data_table = data;
  addr_table = addr;
  table_size = size;
}

static void AddNode(CacheNode *node)
{
  unsigned int b;
  if (node_count >= table_size)
    GrowTables();
  b = node->hash & (table_size - 1);
  node->next_by_data = data_table[b];
  data_table[b] = node;
  b = AddrHash(node->rec) & (table_size - 1);
  node->next_by_addr = addr_table[b];
  addr_table[b] = node;
  node_count++;
}

/* Unlink the node of <rec> from the index, if it has one. */
static void RemoveNode(XmGadgetCachePtr rec)
{
  CacheNode **link, *node;
  if (!node_count)
    return;
  link = &addr_table[AddrHash(rec) & (table_size - 1)];
  while (*link && (*link)->rec != rec)
    link = &(*link)->next_by_addr;
  if (!(node = *link))
    return;
  *link = node->next_by_addr;
  link = &data_table[node->hash & (table_size - 1)];
  while (*link != node)
    link = &(*link)->next_by_data;
  *link = node->next_by_data;
  node_count--;
}

/* Put <ptr> at the front of the list headed by <head>. */
static void LinkFirst(XmGadgetCachePtr head, XmGadgetCachePtr ptr)
{
  ptr->next = head->next;
  if (ptr->next)
    ptr->next->prev = ptr;
  ptr->prev = head;
  head->next = ptr;
}

/************************************************************************
 *
 *  _XmCacheSetHashProc
 *	Index the records of the cache class <cp> by <hash>, which must
 *	give the same value for any two parts that the class's compare
 *	proc finds equal.  Call it once, before the class caches a part
 *	(from its ClassInitialize).
 *
 ************************************************************************/
void _XmCacheSetHashProc(XmCacheClassPartPtr cp, XmCacheHashProc hash)
{
  CacheClass *cls;
  if (FindClass(cp) || ClassCacheHead(cp).next)
    return;
  cls = (CacheClass *)XtMalloc(sizeof(CacheClass));
  cls->cp = cp;
  cls->hash = hash;
  cls->next = hashed_classes;
  hashed_classes = cls;
}

/************************************************************************
 *
 *  _XmCacheDelete
 *	Delete an existing cache record.  NOTE: <data> is a pointer to the
 *      fourth field in the cache record - It is *not* a pointer to the
 *	cache record itself!
 *
 ************************************************************************/
void _XmCacheDelete(XtPointer data)
{
  XmGadgetCachePtr ptr;
  ptr = (XmGadgetCachePtr)DataToGadgetCache(data);
  if (--ptr->ref_count <= 0) {
    (ptr->prev)->next = ptr->next;
    if (ptr->next) /* not the last record */
      (ptr->next)->prev = ptr->prev;
    RemoveNode(ptr);
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
 *	- Look for a record of the class that matches it: in the index
 *	  when the class has a hash proc, else along the class linked
 *	  list.
 *	  = If a match is found, increment the ref_count and return the
 *	    address (a record found along the list moves to its front).
 *	  = Else, allocate a new cache record, copy in temporary Cache bytes,
 *	    put it at the front of the class-cache linked list, and return
 *	    the address.
 *
 ************************************************************************/
XtPointer _XmCachePart(XmCacheClassPartPtr cp, XtPointer cpart, size_t size)
{
  XmGadgetCachePtr head = &ClassCacheHead(cp);
  XmGadgetCachePtr ptr;
  CacheClass *cls = FindClass(cp);
  CacheNode *node;
  unsigned int hash;
  size_t node_offset;

  if (cls) {
    hash = MixHash(cls->hash(cpart) ^ (unsigned int)((uintptr_t)cls >> 4));
    if (table_size) {
      for (node = data_table[hash & (table_size - 1)]; node; node = node->next_by_data) {
        if (node->hash == hash && node->cls == cls &&
            (ClassCacheCompare(cp)(cpart, CacheDataPtr(node->rec))))
        {
          node->rec->ref_count++;
          return ((XtPointer)CacheDataPtr(node->rec));
        }
      }
    }
    /* The node goes after the data, suitably aligned. */
    node_offset = XtOffsetOf(XmGadgetCacheRef, data) + size;
    node_offset = (node_offset + sizeof(void *) - 1) / sizeof(void *) * sizeof(void *);
    ptr = (XmGadgetCachePtr)XtMalloc(node_offset + sizeof(CacheNode));
    ClassCacheCopy(cp)(cpart, CacheDataPtr(ptr), size);
    ptr->ref_count = 1;
    LinkFirst(head, ptr);
    node = (CacheNode *)((char *)ptr + node_offset);
    node->rec = ptr;
    node->cls = cls;
    node->hash = hash;
    AddNode(node);
    return (CacheDataPtr(ptr));
  }

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
        LinkFirst(head, ptr);
      }
      return ((XtPointer)CacheDataPtr(ptr));
    }
  }
  /* Malloc a new rec at the front, fill it out */
  ptr = (XmGadgetCachePtr)XtMalloc(size + XtOffsetOf(XmGadgetCacheRef, data));
  ClassCacheCopy(cp)(cpart, CacheDataPtr(ptr), size);
  ptr->ref_count = 1;
  LinkFirst(head, ptr);
  return (CacheDataPtr(ptr));
}
