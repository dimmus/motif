# 5.1 Hash Tables and Caches

**Scope.** libXm has four different associative structures, written
at different times for different trade-offs: the generic chained hash
table of [`Hash.c`](../../src/lib/Xm/Hash.c) (1993) used by the image,
pixmap, GC, drop-site and Xft caches; the open-addressing trait table
of [`Trait.c`](../../src/lib/Xm/Trait.c) (this tree); the
move-to-front lists of the gadget part cache in
[`Cache.c`](../../src/lib/Xm/Cache.c); and the small per-layout index
maps of `Form.c` (chapter 3.2).  This chapter compares them: data
layout, hash functions, growth policy, deletion, complexity, and the
weaknesses a reader should know before relying on them.

---

## 5.1.1 `XmHashTable`: separate chaining with a bucket pool

```c
/* src/lib/Xm/Hash.c */
typedef struct _XmHashBucketRec {
  XmHashValue hashed_key;
  XmHashKey hash_key;
  XtPointer value;
  struct _XmHashBucketRec *next;
} XmHashBucketRec, *XmHashBucket;

typedef struct _XmHashTableRec {
  Cardinal size;
  Cardinal count;
  XmHashCompareProc compare;
  XmHashFunction hasher;
  XmHashBucket *buckets;
} XmHashTableRec;

/* Available table sizes,  should be prime numbers */
static XmConst int size_table[] = {17, 31, 67, 131, 257, 521, 1031, 2053, 4099, 8209, 0};
```

**Interface.**  `_XmAllocHashTable(size_hint, compare, hasher)` picks
the first prime at or above the hint (so a hint of 100 gives 131
buckets); `NULL` procedures mean "compare pointers" and "the pointer
value is the hash", which the Xft draw table uses with `Window` keys.
`_XmAddHashEntry` inserts at the head of the chain; `_XmGetHashEntry`
(a macro over `_XmGetHashEntryIterate`) finds the first match;
`_XmGetHashEntryIterate` continues from a previous match, so a table
may hold several values under one key; `_XmRemoveHashEntry` unlinks
the first match; `_XmMapHashTable` visits every entry and may free the
current one; `_XmResizeHashTable` rehashes into a larger prime.

**Bucket pool.**  Buckets are carved from 256-entry slabs and recycled
through a free list:

```c
static XmHashBucket NewBucket(void)
{
  if (FreeBucketList == NULL) {
    /* Allocate alot of buckets at once to cut down on fragmentation */
    buckets = (XmHashBucket)XtMalloc(NUMBUCKETS * sizeof(XmHashBucketRec));
    for (i = 0; i < NUMBUCKETS; i++) buckets[i].next = &buckets[i + 1];
    buckets[NUMBUCKETS - 1].next = (XmHashBucket)NULL;
    FreeBucketList = buckets;
  }
  rbucket = FreeBucketList;
  FreeBucketList = FreeBucketList->next;
  return (rbucket);
}
static void FreeBucket(XmHashBucket b) { b->next = FreeBucketList; FreeBucketList = b; }
```

Slabs are never returned to the system.  The pool is process-wide and
shared by every table, protected by whatever lock the caller holds
(every caller in libXm holds `_XmProcessLock`).

**Complexity.**  Insert O(1); lookup and delete O(1 + α) where α =
count/size is the load factor; no automatic resizing, so a table that
is never resized degrades linearly.  `_XmResizeHashTable` is O(n + m):
it walks every chain and, for each bucket whose new index differs,
unlinks it and *appends it to the end of its new chain* "to maintain
ordering", which makes the rehash O(n × average chain length) in the
worst case and is, as the comment says, "a slow method, but always
correct".  The Xft tables call it whenever count exceeds size, so they
stay at α ≤ 1 with amortised O(1) growth; the image and pixmap tables
never resize and rely on their 131-bucket hint being enough for the
icons of one program.

**Hash functions supplied by callers.**  The string hash in
`ImageCache.c` deserves a note:

```c
/* src/lib/Xm/ImageCache.c */
static XmHashValue HashString(XmHashKey key)
{
  char *data = (char *)key;
  unsigned int len = strlen(data);
  return (((len << 8) | data[0]) << 8) | data[len];
}
```

`data[len]` is the terminating NUL, so the last term is always zero
and the hash depends only on the *length and the first character* of
the image name.  Icon names of the same length starting with the same
letter (`icon_open.xpm`, `icon_save.xpm`) collide and share a chain.
It is not incorrect, and with a few dozen images per program it is
not measurable, but it is a hash function to replace before anyone
puts thousands of names in the table.  The Xft font hash
(`h = h·31 + c` over four strings and five integers) and the trait
hash below are what a modern reader expects.

**A fragile idiom.**  `_XmRemoveHashEntry` frees the bucket and then
returns `entry->hash_key` from the freed bucket.  Because `FreeBucket`
only pushes the bucket onto the free list and never gives memory back,
the read is safe in practice, but it is a use-after-free in principle
and would break if the pool were ever changed to use `free()`.  The
callers in libXm ignore the return value.

## 5.1.2 The trait table: open addressing with tombstones

Chapter 1.2 reproduced the code; the comparison with `Hash.c` is the
point here.

| | `XmHashTable` | Trait table |
|---|---|---|
| Layout | array of chain heads, buckets from a pool | one flat array of `{obj, name, data}` slots |
| Key | opaque pointer plus caller's compare | (pointer, quark) pair compared inline |
| Hash | caller's | pointer mixing with two multiplicative constants |
| Collision | chain | linear probing, step 1 |
| Deletion | unlink | tombstone; dropped at the next rebuild |
| Growth | manual `_XmResizeHashTable` | automatic at 75 % filled, doubling when more than half the slots hold live traits |
| Memory per entry | 32 bytes (bucket) + 8 (head slot share) | 24 bytes, no pointers |
| Cache behaviour | two or three dependent loads per lookup | one load for the common hit in the first probe |
| Iteration | `_XmMapHashTable` | none needed |

The trait table is read on hot paths (`XmeTraitGet` from traversal,
default-button handling, every container item access) and written
only at class initialisation, which is exactly the profile where open
addressing wins: dense, read-mostly, small keys.  A chained table is
the better general tool when values are large structs that the
caller allocates anyway (the pixmap and GC records), when the same key
may map to several values (the pixmap tables), or when entries must
be freed through a visitor (every cache on display close).

## 5.1.3 The gadget part cache: a move-to-front list

The *cache parts* of chapter 1.2 (one `XmLabelGCacheObjPart` shared by
all label gadgets with the same font, colours, margins and alignment)
are interned by `_XmCachePart`:

```c
/* src/lib/Xm/Cache.c */
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
      if (ptr != head->next) {           /* Unlink, then put it back at the front. */
        ...
      }
      return ((XtPointer)CacheDataPtr(ptr));
    }
  }
  /* Malloc a new rec at the front, fill it out */
  ptr = (XmGadgetCachePtr)XtMalloc(size + XtOffsetOf(XmGadgetCacheRef, data));
  ClassCacheCopy(cp)(cpart, CacheDataPtr(ptr), size);
  ptr->ref_count = 1;
  ...
  return (CacheDataPtr(ptr));
}
```

This is a linear search with a *compare procedure supplied by the
class* (the cache class's `compare` method, which compares the part
field by field) over a doubly linked list kept in most-recently-used
order.  O(k) per intern for k distinct parts, O(1) in the common case
of a run of identical gadgets, as the comment argues.  The
`gadget-cache` case of `xmbench` ("create LabelGadgets, 200 distinct
cache parts") measures the worst case.  A hash table would need a
hash over a struct with font and pixmap pointers, which the
class-supplied compare already handles; the list was kept, and the
move-to-front added, because the distribution of parts in a real
program is a few values used thousands of times.  Reference counts
free a part when its last gadget goes away (`_XmCacheDelete`).

The companion idiom is the *extension data stack*: `XtSetValues` on a
gadget pushes an `XmWidgetExtData` (the scratch cache object) for the
duration of the call and pops it in the post-hook.  `LabelG.c` keeps
up to four such records in a small free list (`MAX_FREE_EXT_DATA`)
"rather than a calloc/free pair each time", the same pooling instinct
as `Hash.c`'s buckets, at a scale where it is measurable in
`gadget-set` ("XtSetValues of a cached LabelGadget resource").

## 5.1.4 The per-layout index maps

`Form.c`'s `FormIndexMap` (chapter 3.2) is the fourth design: a
power-of-two array of `(Widget, index)` pairs with linear probing,
built and freed per layout, with no deletion.  It exists because the
alternative, a linear scan of the children for every attachment
target, made the sort quadratic, and because a table that lives for
one function call needs neither tombstones nor growth.  It is the
smallest correct hash table in the library: an allocation, a mask, a
loop.

## 5.1.5 Guidelines distilled from the four

- **Chained table** (`Hash.c`) when values are caller-allocated
  records, keys may repeat, entries are freed by a visitor, or the
  table lives as long as the process.  Pass a real hash function;
  resize when count exceeds size (the Xft code's `AddHashEntry` is the
  template).
- **Open addressing** (`Trait.c`) when keys are small and the table is
  read far more than written.  Keep the load under 75 % counting
  tombstones; rebuild to drop them.
- **Move-to-front list** (`Cache.c`) when the number of distinct
  entries is small and accesses come in runs.
- **Throwaway probing map** (`Form.c`) for a per-call index over a
  known set of pointers.

All four are guarded by `_XmProcessLock` where they are shared, and
all four are internal (`*I.h` headers, not exported since the version
scripts), so any of them can be replaced without an ABI change, which
is more than can be said for the arrays inside `XmListPart` and
`XmContainerPart` (chapter 6.1).

## References

- [`src/lib/Xm/Hash.c`](../../src/lib/Xm/Hash.c),
  [`HashI.h`](../../src/lib/Xm/HashI.h),
  [`Trait.c`](../../src/lib/Xm/Trait.c),
  [`Cache.c`](../../src/lib/Xm/Cache.c), [`CacheI.h`](../../src/lib/Xm/CacheI.h),
  [`ImageCache.c`](../../src/lib/Xm/ImageCache.c),
  [`XmRenderT.c`](../../src/lib/Xm/XmRenderT.c),
  [`Form.c`](../../src/lib/Xm/Form.c), [`LabelG.c`](../../src/lib/Xm/LabelG.c)
- Knuth, *The Art of Computer Programming* vol. 3, §6.4 "Hashing";
  <https://en.wikipedia.org/wiki/Hash_table>
- `xmbench trait-get gadget-cache gadget-set gadget-get-shells`
