#!/usr/bin/env python3
"""Summarise where the round trips of a program come from.

    XMBENCH_REPLY_BACKTRACE=1 LD_PRELOAD=.../libxmbench_preload.so \\
        PROGRAM 2>bt.txt
    rtrips.py [-d DEPTH] bt.txt

libxmbench_preload prints a backtrace for every round trip (every wait
for a reply in libxcb, which Xlib's _XReply makes).  This groups them
by the first DEPTH frames (default 4) below libxcb and _XReply, with the
function
names and source lines resolved by addr2line from the debug information
(backtrace_symbols only knows exported functions), and prints the
groups, most frequent first.
"""
import argparse
import collections
import re
import subprocess
import sys

FRAME = re.compile(r'^(?P<obj>[^(\s]+)\((?P<sym>[^)+]*)\+(?P<off>0x[0-9a-f]+)\)')

_cache = {}


def resolve(obj, sym, off):
    """Return 'function (file:line)' for an offset into OBJ."""
    key = (obj, sym, off)
    if key in _cache:
        return _cache[key]
    name = sym or '?'
    where = ''
    if not sym:
        # An offset from the start of the object: addr2line takes it as
        # an address for a shared library (they are linked at 0).
        try:
            out = subprocess.run(['addr2line', '-f', '-C', '-i', '-e', obj,
                                  off], capture_output=True, text=True,
                                 check=False).stdout.split('\n')
            if out and out[0] and out[0] != '??':
                name = out[0]
                where = out[1].rsplit('/', 1)[-1] if len(out) > 1 else ''
        except OSError:
            pass
    lib = obj.rsplit('/', 1)[-1]
    text = '%s [%s%s]' % (name, lib, (' ' + where) if where else '')
    _cache[key] = text
    return text


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('-d', '--depth', type=int, default=4)
    ap.add_argument('file')
    args = ap.parse_args()

    groups = collections.Counter()
    frames = []
    total = 0
    with open(args.file, errors='replace') as f:
        for line in f:
            line = line.rstrip('\n')
            if line == '--':
                # Skip the preload library's own frames, libxcb's and
                # _XReply.
                stack = [fr for fr in frames
                         if 'xmbench_preload' not in fr[0] and
                         'libxcb' not in fr[0] and fr[1] != '_XReply']
                key = ' <- '.join(resolve(*fr) for fr in stack[:args.depth])
                groups[key] += 1
                total += 1
                frames = []
                continue
            m = FRAME.match(line)
            if m:
                frames.append((m.group('obj'), m.group('sym'),
                               m.group('off')))
    print('%d round trips' % total)
    for key, n in groups.most_common():
        print('%5d  %s' % (n, key))
    return 0


if __name__ == '__main__':
    sys.exit(main())
