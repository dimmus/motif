# 3.2 Case Study: XmForm, a Constraint Layout Solver

**Scope.** `XmForm` positions its children by *attachments*: each of a
child's four edges may be attached to the form, to a sibling, to the
opposite edge of a sibling, or to a fractional position, with an
offset.  Resolving the attachments is a small constraint-satisfaction
problem that upstream Motif solved by repeated recomputation in O(n²)
and that this tree solves with a topological sort and an incremental
fixed-point iteration ([`Form.c`](../../src/lib/Xm/Form.c)).  This
chapter describes the problem, both algorithms and the data structures
that make the new one work, and how its equivalence to the old
behaviour was verified.

---

## 3.2.1 The problem

For each managed child *c* and each side *s* in {left, right, top,
bottom}, the constraint record holds an attachment
`c->att[s]` with a type, a target widget, a position and an offset:

| `XmNtopAttachment` | Meaning of the top edge |
|--------------------|-------------------------|
| `XmATTACH_NONE` | free (follows the opposite edge and the child's size) |
| `XmATTACH_FORM` | at the form's top edge plus offset |
| `XmATTACH_OPPOSITE_FORM` | at the form's *bottom* edge plus offset |
| `XmATTACH_WIDGET` | below the bottom edge of the target sibling plus offset |
| `XmATTACH_OPPOSITE_WIDGET` | at the *top* edge of the target sibling plus offset |
| `XmATTACH_POSITION` | at `position / XmNfractionBase` of the form's height |
| `XmATTACH_SELF` | at the child's current position, converted to a position attachment |

Two things make this harder than a one-pass layout:

1. **Dependencies between children.**  A child attached to a sibling
   cannot be placed before the sibling.  The dependencies form a
   directed graph; the Style Guide forbids cycles, but applications
   create them (a Form with a resizable child chain often has one by
   accident), and the toolkit must still produce *some* layout.
2. **The form's size depends on the children and vice versa.**  A
   child attached to the right edge of the form reads the form's width;
   the form's preferred width is the extent of its children.  The
   original code resolves this by iterating to a fixed point.

The layout runs on every `change_managed`, on every geometry request
from a child, on every `resize` of the form and on every `set_values`
of an attachment, so its cost is paid often: a dialog of 50 children
is laid out a few times at startup and again at every resize.

## 3.2.2 Ordering the children: a topological sort

Upstream sorted the children with a nested loop that, for each child,
scanned the list for one whose attachment targets were all placed,
O(n²) comparisons, and did so on every layout.  This tree's
`SortChildren` is Kahn's algorithm with a min-heap, and it is cached:

```c
/* src/lib/Xm/Form.c */
/*  SortChildren
 *	Link the RectObj children into the list that the layout walks:
 *	the managed children in an order where each one comes after the
 *	siblings it is attached to, then the managed children caught in
 *	an attachment cycle, then the unmanaged children.  Among the
 *	children that are ready, the one that comes first in the
 *	children array is taken first.
 *
 *	A child's sorted field is cleared when it is created and when its
 *	attachments change, and records whether it was managed when it
 *	was sorted.  The list is rebuilt only when one of them is out of
 *	date or the list does not match the children any more.
 */
static void SortChildren(XmFormWidget fw)
{
  ...
  if (!dirty && SortedListValid(fw, num_rect))
    return;
  MapInit(&map, (int)num);                       /* widget -> index hash */
  ...
  /* indeg[i]: attachments of managed child i to unsorted siblings,
   * edges[edge_start[k]..edge_start[k + 1]): the children attached
   * to child k. */
  indeg = (int *)XtCalloc((Cardinal)(3 * num + 2), sizeof(int));
  edge_start = indeg + num;
  order = edge_start + num + 1;
  for (i = 0; i < (int)num; i++) {                /* count edges */
    ...
      if (((c->att[j].type == XmATTACH_WIDGET) || (c->att[j].type == XmATTACH_OPPOSITE_WIDGET)) &&
          SIBLINGS(c->att[j].w, child) && XtIsRectObj(c->att[j].w) &&
          ((k = MapFind(&map, c->att[j].w)) >= 0) && !GetFormConstraint(children[k])->sorted)
      {
        indeg[i]++;
        edge_start[k + 1]++;
      }
  }
  for (i = 0; i < (int)num; i++)
    edge_start[i + 1] += edge_start[i];          /* prefix sums: CSR layout */
  edges = (int *)_XmMallocArray(edge_start[num] + 1, sizeof(int));
  ...                                            /* second pass fills edges */
  /* Take the ready children lowest index first. */
  heap = (int *)_XmMallocArray(num + 1, sizeof(int));
  for (i = 0; i < (int)num; i++)
    if (XtIsRectObj(child) && XtIsManaged(child) && (indeg[i] == 0))
      HeapPush(&heap, &heap_len, &heap_size, i);
  while (heap_len > 0) {
    i = HeapPop(heap, &heap_len);
    order[num_order++] = i;
    GetFormConstraint(children[i])->sorted = True;
    for (j = edge_start[i]; j < edge_start[i + 1]; j++)
      if (--indeg[edges[j]] == 0)
        HeapPush(&heap, &heap_len, &heap_size, edges[j]);
  }
  /* Add other children that haven't been sorted */        /* the cycle members */
  ...
  /* Then the unmanaged ones, last first. */
  ...
}
```

The structure, step by step:

1. **Validation.**  Each constraint record carries a `sorted` field
   with three values: "not sorted since its attachments changed",
   "sorted while managed", "sorted while unmanaged".  If every child's
   field matches its current managed state and `SortedListValid`
   confirms that the cached linked list (`first_child`/`next_sibling`)
   still contains exactly the RectObj children in the expected shape,
   the sort is skipped.  This is an O(n) check that replaces an
   O(n log n) or, upstream, O(n²) sort on the common path (a resize
   with no change of attachments).
2. **Widget-to-index map.**  Attachment targets are widget pointers;
   the algorithm needs indices.  `FormIndexMap` is a small
   open-addressing hash table (power-of-two size, mask, linear
   probing) built once per sort, O(n), giving O(1) `MapFind` instead of
   a linear scan per target.
3. **Graph in CSR form.**  Two passes over the attachments count the
   edges per source and then fill one `edges` array indexed through
   `edge_start` prefix sums.  This is the *compressed sparse row*
   layout: O(n + e) memory, contiguous, no per-edge allocation.  Only
   `XmATTACH_WIDGET` and `XmATTACH_OPPOSITE_WIDGET` to *managed
   siblings* count as edges; attachments to unmanaged children are
   treated as already resolved ("THIS IS PROBABLY WRONG AND SHOULD BE
   FIXED SOMEDAY", says a comment that has been there since the 1990s,
   and the new code preserves the behaviour deliberately).
4. **Kahn's algorithm with a min-heap.**  Children with in-degree 0 are
   ready; the heap yields the one with the lowest index first, which
   is the tie-break rule upstream had ("among the children that are
   ready, the one that comes first in the children array"), so the
   resulting order is *identical* to the old one for acyclic graphs.
   Each pop decrements the in-degree of its dependents.
5. **Cycles.**  Children never popped are part of, or depend on, a
   cycle.  They are appended in index order, as upstream did, so that
   a cyclic Form lays out the same way as before: the layout step
   below will then relax them iteratively.
6. **Unmanaged children last, newest first**, again matching the old
   list shape, which `SortedListValid` relies on.

Complexity: O(n + e) for the graph, O((n + e) log n) for the heap
operations, O(n) for the map; e ≤ 4n, so O(n log n) overall, against
O(n²) before.  The `form-chain` case of `xmbench` creates and manages
a Form of chained children and the A/B harness of §3.2.5 checked the
geometry against the old code.

## 3.2.3 Computing the layout: incremental relaxation

Once ordered, each child's edges are computed from its attachments
(`ComputeAttachment`-style arithmetic in `CalcEdgeValue` and friends,
not shown).  The subtlety is the form's own size.  The original
algorithm, kept in spirit, is:

```c
/*  RelaxChildren
 *	Compute the temporary edge values of the managed children and
 *	grow *form_width and *form_height to hold them.  Returns False
 *	if the constraints do not settle.
 *
 *	The children are taken one at a time, in sorted order, and after
 *	each one all the children taken so far are recomputed until the
 *	form size settles.  Only the children whose inputs changed are
 *	actually recomputed (see LayoutSync); this gives the same result
 *	as recomputing them all, without the quadratic cost.
 */
static Boolean RelaxChildren(XmFormWidget fw, Dimension *form_width, Dimension *form_height,
                             Widget instigator, XtWidgetGeometry *geometry)
{
  FormLayout layout;
  Boolean finished = True;
  int i;
  LayoutInit(fw, &layout);
  for (i = 0; i < layout.num_managed; i++) {
    layout.scope = i;
    /* child i is new to the prefix, so it is dirty */
    LayoutCall(fw, &layout, i, instigator, geometry, form_width, form_height);
    if (!LayoutSync(fw, &layout, form_width, form_height, instigator, geometry)) {
      finished = False;
      break;
    }
  }
  LayoutFree(&layout);
  return (finished);
}
```

The outer loop grows a *prefix* of the sorted list one child at a
time; after adding a child, the prefix is brought back to a fixed
point.  Upstream did that by recomputing every child in the prefix
until nothing changed, which is O(n) per round and O(n) rounds in the
worst case per child, O(n²) to O(n³) overall.  The new `LayoutSync`
recomputes only what can have changed, using the scratch state that
`LayoutInit` builds:

```c
typedef struct {
  Widget *kids;        /* RectObj children in the order of the sorted list */
  int num_kids;
  int num_managed;     /* the leading managed children of kids */
  FormIndexMap map;    /* kids[i] -> i */
  int *dep_start;      /* deps[dep_start[i]..dep_start[i + 1]) are the */
  int *deps;           /* managed children attached to kids[i] */
  unsigned char *flags; /* FL_* for each managed child */
  /* Children that read the form width (axis 0) or height (axis 1),
   * ascending.  Such a child is also dirty when the size along that
   * axis changed after it was last computed, that is when its seen
   * value is not the current epoch. */
  int *readers[2];
  int num_readers[2];
  unsigned int epoch[2];
  unsigned int *seen[2];
  int cursor[2];       /* LayoutSync: next reader to look at */
  int *deferred;       /* scratch for LayoutSync */
  int *heap;           /* min-heap of the flagged dirty children; may
                        * also hold children that are clean again */
  ...
} FormLayout;
```

The idea is *change propagation*:

- A child is **dirty** when one of its inputs changed: it was just
  added to the prefix, a sibling it is attached to moved, or the form
  size along an axis it reads changed.  `dep_start`/`deps` is the
  reverse dependency graph (again CSR), so when child *k* moves, its
  dependents are flagged in O(out-degree).
- Children whose edges depend on the form's width or height are listed
  per axis in `readers[]`.  The form size carries an **epoch** per
  axis, incremented whenever the size grows; each reader remembers the
  epoch it last saw.  After a size change, the readers whose `seen`
  is stale are dirty, found by advancing a `cursor` through the sorted
  reader list rather than scanning all children.
- A **min-heap** orders the dirty children by sorted index, so that
  a child is recomputed after the siblings it depends on and, in the
  common acyclic case, exactly once per round.  The heap may hold a
  child that became clean again; it is skipped on pop (the
  `FL_DIRTY` flag is the truth).
- `FL_UNSAFE` marks a child whose edge value exceeded
  `MAX_EDGE_VALUE` (65 535, the range of an X `Dimension`); the code
  stops growing the form at that point instead of wrapping, which was
  one of the integer-overflow classes fixed in the security review.

The work is proportional to the number of (child, dependency) pairs
that actually change, which for an acyclic Form is O(n + e) per prefix
step in the worst case and far less typically; a cycle still iterates,
bounded by the same termination test as before ("Returns False if the
constraints do not settle", after which the Form warns and uses what
it has).

## 3.2.4 Data-structure choices

| Structure | Why this one |
|-----------|--------------|
| Linked list through constraint records (`first_child`, `next_sibling`) | Upstream's representation; kept so that the rest of `Form.c` (and anything that inspects constraint records) is unchanged; validated rather than rebuilt |
| `FormIndexMap` open-addressing hash | Pointer keys, built per layout, needs only insert and find; a power-of-two table with linear probing is the smallest correct thing |
| CSR adjacency (`edge_start`, `edges`; `dep_start`, `deps`) | Two allocations, cache-friendly iteration, no per-node lists to free; the graph is static for the duration of one sort or layout |
| Binary min-heap on indices | Gives the "lowest index first" order that preserves upstream's results; O(log n) push/pop; a small array |
| Epoch counters per axis | Avoids walking every reader on every size change; an integer compare per reader instead of a dirty flag that must be set in bulk |

All scratch memory is allocated with `_XmMallocArray`/`XtCalloc` and
freed in `LayoutFree`; nothing persists between layouts except the
sorted list and the `sorted` flags.

## 3.2.5 Verifying equivalence

A layout algorithm that is faster but places one child one pixel off
is a regression, and the Form has thirty years of applications relying
on its exact behaviour, cycles and all.  The change was validated with
the A/B harness in [`src/tests/ab`](../../src/tests/ab/README.md):
`xm_abtest form SEED` builds a pseudo-random Form from a seed
(including the modes `formcyc` for attachment cycles, `formgrid`,
`formcolumn`, `formwide`), drives it through the API and through real
input, and prints every child's geometry, the callbacks and a hash of
the window pixels after every step; `ab.sh` runs a range of seeds
against the old and the new libXm (loaded with `LD_LIBRARY_PATH`) and
reports differing seeds.  The README recommends a *mutation test*:
break the new code on purpose (skip the re-sort) and confirm that
seeds start to differ, to prove the harness can see a difference.
The `Layout` suite of the libcheck tests and `xm_layoutbench form N`
(timing by phase) complete the picture.

## 3.2.6 Assessment

| | Upstream | This tree |
|---|---|---|
| Sort | O(n²) nested scan on every layout | Kahn with heap, O(n log n), cached across layouts with an O(n) validity check |
| Relaxation | recompute whole prefix until stable, O(n²)+ | recompute only dirty children via reverse dependencies and epochs |
| Results | — | identical geometry, order and callbacks on all tested seeds, including cycles |
| Memory | none extra | O(n + e) scratch per layout, freed after |
| Code | 1 loop | ~400 lines of scratch-state management, with the invariants documented in the struct |
| Known deliberate oddities kept | unmanaged children as attachment targets count as placed; cyclic children ordered by index; `XtGeometryAlmost` handling | same |

The design question the case study answers is how to make a 1989
algorithm fast without changing a single observable result: keep the
representation, keep the tie-breaks, add indices and dependency
tracking around the existing arithmetic, and measure equivalence
rather than assume it.

## References

- [`src/lib/Xm/Form.c`](../../src/lib/Xm/Form.c),
  [`FormP.h`](../../src/lib/Xm/FormP.h)
- Kahn, A. B., "Topological sorting of large networks", *CACM* 5(11),
  1962; summary: <https://en.wikipedia.org/wiki/Topological_sorting#Kahn's_algorithm>
- Compressed sparse row graphs: <https://en.wikipedia.org/wiki/Sparse_matrix#Compressed_sparse_row_(CSR,_CRS_or_Yale_format)>
- Manual page `XmForm(3)` in [`doc/man/man3`](../man/man3)
- [`src/tests/ab/README.md`](../../src/tests/ab/README.md),
  `xmbench form-chain form-chain-destroy`
