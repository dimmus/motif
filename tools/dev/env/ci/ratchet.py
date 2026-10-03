#!/usr/bin/env python3
"""Ratchet static-analysis findings against a committed baseline.

Usage:
  ratchet.py collect clang-tidy SRCDIR BUILDDIR LOG...  > counts.txt
  ratchet.py collect scan-build SRCDIR BUILDDIR REPORTDIR > counts.txt
  ratchet.py collect cppcheck   SRCDIR BUILDDIR XML     > counts.txt
  ratchet.py compare BASELINE CURRENT

"collect" turns the output of one tool into lines "<count>\t<file>\t<check>",
with paths relative to the source tree ("@build/..." for generated files)
and line numbers dropped, so that unrelated edits that move code around do
not change the result.  Findings are de-duplicated per file, line and check
first, because a header is analysed once per translation unit.

"compare" fails when any (file, check) pair has more findings than in the
baseline, which includes pairs the baseline does not have.  Pairs that went
down are reported so that the baseline can be lowered in the same change.
A missing baseline file is bootstrap mode: the counts are printed and the
comparison passes.
"""

import collections
import html
import os
import re
import sys
import xml.etree.ElementTree as ET


def relpath(path, srcdir, builddir):
    path = os.path.normpath(os.path.join(builddir, path))
    for root, prefix in ((builddir, "@build/"), (srcdir, "")):
        root = os.path.normpath(root) + os.sep
        if path.startswith(root):
            return prefix + path[len(root):]
    return None  # outside the project: system or X11 headers


def emit(findings, srcdir, builddir):
    counts = collections.Counter()
    for path, line, check in set(findings):
        rel = relpath(path, srcdir, builddir)
        if rel is not None:
            counts[(rel, check)] += 1
    for (rel, check), n in sorted(counts.items()):
        print(f"{n}\t{rel}\t{check}")


CLANG_TIDY_RE = re.compile(
    r"^(?P<file>/[^:]+):(?P<line>\d+):\d+: (?:warning|error): .* \[(?P<check>[^\]]+)\]$")


def collect_clang_tidy(srcdir, builddir, logs):
    findings = []
    for log in logs:
        with open(log, errors="replace") as f:
            for line in f:
                m = CLANG_TIDY_RE.match(line.rstrip("\n"))
                if m:
                    for check in m.group("check").split(","):
                        findings.append((m.group("file"), m.group("line"), check))
    return findings


SCAN_BUILD_RE = re.compile(r"<!-- (BUGTYPE|BUGFILE|BUGLINE|BUGCATEGORY) (.*?) -->")


def collect_scan_build(srcdir, builddir, reportdir):
    findings = []
    for dirpath, _, files in os.walk(reportdir):
        for name in files:
            if not (name.startswith("report-") and name.endswith(".html")):
                continue
            with open(os.path.join(dirpath, name), errors="replace") as f:
                meta = dict(SCAN_BUILD_RE.findall(f.read()))
            if "BUGFILE" in meta:
                check = html.unescape(meta.get("BUGCATEGORY", "") + ": "
                                      + meta.get("BUGTYPE", ""))
                findings.append((meta["BUGFILE"], meta.get("BUGLINE", "0"), check))
    return findings


def collect_cppcheck(srcdir, builddir, xmlfile):
    findings = []
    for err in ET.parse(xmlfile).getroot().iter("error"):
        loc = err.find("location")
        if loc is None:
            continue
        path = loc.get("file")
        if not os.path.isabs(path):
            path = os.path.join(srcdir, path)
        findings.append((path, loc.get("line"), err.get("id")))
    return findings


def read_counts(path):
    counts = {}
    with open(path) as f:
        for line in f:
            line = line.rstrip("\n")
            if not line or line.startswith("#"):
                continue
            n, rel, check = line.split("\t", 2)
            counts[(rel, check)] = int(n)
    return counts


def compare(baseline_path, current_path):
    current = read_counts(current_path)
    total = sum(current.values())
    if not os.path.exists(baseline_path):
        print(f"ratchet: no baseline {baseline_path}; bootstrap mode, "
              f"{total} findings in {len(current)} file/check pairs. "
              f"Commit the generated counts as the baseline.")
        return 0
    baseline = read_counts(baseline_path)
    worse = sorted(k for k in current if current[k] > baseline.get(k, 0))
    better = sorted(k for k in baseline if current.get(k, 0) < baseline[k])
    for rel, check in worse:
        print(f"NEW: {rel}: {check}: {baseline.get((rel, check), 0)} -> "
              f"{current[(rel, check)]}")
    for rel, check in better:
        print(f"fixed: {rel}: {check}: {baseline[(rel, check)]} -> "
              f"{current.get((rel, check), 0)}")
    print(f"ratchet: {total} findings (baseline {sum(baseline.values())}), "
          f"{len(worse)} pairs got worse, {len(better)} got better")
    if worse:
        print("ratchet: fix the new findings (or, for a false positive, "
              "suppress it in the code with a NOLINT comment and a reason)")
        return 1
    if better:
        print(f"ratchet: please lower the baseline: copy the new counts "
              f"over {baseline_path}")
    return 0


def main(argv):
    if len(argv) >= 6 and argv[1] == "collect":
        tool, srcdir, builddir = argv[2], os.path.abspath(argv[3]), os.path.abspath(argv[4])
        collectors = {"clang-tidy": lambda: collect_clang_tidy(srcdir, builddir, argv[5:]),
                      "scan-build": lambda: collect_scan_build(srcdir, builddir, argv[5]),
                      "cppcheck": lambda: collect_cppcheck(srcdir, builddir, argv[5])}
        if tool in collectors:
            emit(collectors[tool](), srcdir, builddir)
            return 0
    elif len(argv) == 4 and argv[1] == "compare":
        return compare(argv[2], argv[3])
    sys.stderr.write(__doc__)
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv))
