#!/usr/bin/env python3
"""Top allocation sites from a heaptrack flame-graph stack file.

    heaptrack_print -f heaptrack.gz --flamegraph-cost-type allocations \\
        -F stacks.txt
    stacks.py [-n N] [-d DEPTH] stacks.txt

Each line of the file is a call stack, outermost frame first, and a
cost.  This attributes the cost of each stack to its innermost DEPTH
frames (default 2) after leaving out the allocators themselves (malloc,
XtMalloc, ...), so that XtMalloc called from XmStringCreate counts for
XmStringCreate rather than for XtMalloc, and prints the N (default 15)
costliest sites.
"""
import argparse
import collections
import sys

ALLOCATORS = {
    'malloc', 'calloc', 'realloc', 'free', 'posix_memalign',
    'aligned_alloc', 'memalign', 'strdup', 'strndup', '__strdup',
    '__libc_malloc', '__libc_calloc', '__libc_realloc',
    'XtMalloc', 'XtCalloc', 'XtRealloc', 'XtNewString', '__XtMalloc',
    '__XtCalloc', '_XtHeapAlloc', 'XtAsprintf',
    '_XmReallocArray', 'Xpermalloc', '_XlcCopyFromArg',
}


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('-n', type=int, default=15)
    ap.add_argument('-d', '--depth', type=int, default=2)
    ap.add_argument('file')
    args = ap.parse_args()

    sites = collections.Counter()
    total = 0
    with open(args.file, errors='replace') as f:
        for line in f:
            stack, _, cost = line.rstrip('\n').rpartition(' ')
            if not stack or not cost.isdigit():
                continue
            # Frames look like "XtMalloc (Alloc.c)" or "0x7f12345678".
            frames = [fr.strip() for fr in stack.split(';')]
            frames = [fr for fr in frames
                      if fr and fr.split(' (')[0] not in ALLOCATORS]
            key = ' <- '.join(reversed(frames[-args.depth:])) or '?'
            sites[key] += int(cost)
            total += int(cost)
    print('%d allocations' % total)
    for key, n in sites.most_common(args.n):
        print('%8d %5.1f%%  %s' % (n, 100.0 * n / max(total, 1), key))
    return 0


if __name__ == '__main__':
    sys.exit(main())
