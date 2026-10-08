#!/usr/bin/env python3
#
# Motif
#
# Licensed under the LGPL 2.1 license.
#
"""Tests of the result folding and the regression gate of bench.py."""

import io
import json
import os
import sys
import tempfile
import unittest
from contextlib import redirect_stdout

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import bench  # noqa: E402


def run(name, ns, rtrips=0, proxy=None, per_run=True):
    """One round of a case, as run_one returns it."""
    r = {"name": name, "group": "micro", "n": 1000, "ns_per_op": ns,
         "cpu_ns_per_op": ns / 2, "mallocs_per_op": 1.0,
         "requests_per_op": 2.0, "round_trips_per_op": rtrips / 1000,
         "icvalues_per_op": 0.0}
    if per_run:
        r["requests_per_run"] = 2000
        r["round_trips_per_run"] = rtrips
    if proxy is not None:
        r["proxy"] = {"requests": 3000, "replies": proxy, "errors": 0,
                      "events": 5, "round_trips": proxy}
    return r


def rounds(name, ns, *args, **kw):
    """Eleven rounds of a case around ns, 0.5 % apart."""
    return [run(name, ns * (1 + d / 200.0), *args, **kw)
            for d in range(-5, 6)]


def report(*cases):
    return {"cases": [bench.summarize(runs) for runs in cases]}


class Summarize(unittest.TestCase):
    def test_median_and_ranges(self):
        s = bench.summarize([run("a", 100, 2, 50), run("a", 300, 2, 50),
                             run("a", 200, 3, 52)])
        self.assertEqual(s["rounds"], 3)
        self.assertEqual(s["ns_per_op"], 200)
        self.assertEqual(s["round_trips_per_run"], 2)
        self.assertEqual(s["round_trips_per_run_range"], [2, 3])
        self.assertEqual(s["proxy"]["round_trips"], 50)
        self.assertEqual(s["proxy"]["round_trips_range"], [50, 52])
        self.assertEqual(s["ns_per_op_runs"], [100, 300, 200])

    def test_without_proxy(self):
        s = bench.summarize([run("a", 1), run("a", 3)])
        self.assertNotIn("proxy", s)
        self.assertEqual(s["ns_per_op"], 2)


class Compare(unittest.TestCase):
    def check(self, base, head, threshold=5.0):
        rows, bad = bench.compare(report(*base), report(*head), threshold)
        return {r["name"]: r["problems"] for r in rows}, bad

    def test_equal(self):
        rows, bad = self.check([[run("a", 100, 1, 10)]],
                               [[run("a", 100, 1, 10)]])
        self.assertEqual(bad, [])
        self.assertEqual(rows, {"a": []})

    def test_time_threshold(self):
        rows, bad = self.check([rounds("a", 100), rounds("b", 100)],
                               [rounds("a", 105), rounds("b", 105.1)])
        self.assertEqual(rows["a"], [])
        self.assertEqual(rows["b"], ["+5.1% time"])
        self.assertEqual([r["name"] for r in bad], ["b"])

    def test_min_ns(self):
        # 5.8 -> 6.4 ns is +10 %, but 0.6 ns: a shift in where the code
        # of a few-nanosecond case landed, not a regression.
        rows, bad = self.check([rounds("a", 5.8)], [rounds("a", 6.4)])
        self.assertEqual(bad, [])
        rows, bad = bench.compare(report(rounds("a", 5.8)),
                                  report(rounds("a", 6.4)), 5.0,
                                  min_ns=0.5)
        self.assertEqual(rows[0]["problems"], ["+10.3% time"])
        # The same 10 % of a 100 ns case is 10 ns.
        rows, bad = self.check([rounds("a", 100)], [rounds("a", 110)])
        self.assertEqual(rows["a"], ["+10.0% time"])

    def test_threshold_option(self):
        rows, bad = self.check([rounds("a", 100)], [rounds("a", 109)], 10)
        self.assertEqual(bad, [])

    def test_noise_is_not_a_regression(self):
        # The median is 10 % higher, but the rounds overlap too much for
        # that to be told from noise.
        base = [run("a", ns) for ns in (80, 95, 100, 120, 150)]
        head = [run("a", ns) for ns in (85, 90, 110, 115, 160)]
        rows, bad = self.check([base], [head])
        self.assertEqual(bad, [])
        # Five rounds against five, all slower: p is about 0.006, not
        # enough with about 40 cases compared at once.
        head = [run("a", ns) for ns in (151, 160, 170, 180, 190)]
        rows, bad = self.check([base], [head])
        self.assertEqual(bad, [])
        # Ten against ten, all slower: p is about 1e-4.
        base += [run("a", ns) for ns in (85, 96, 101, 119, 149)]
        head += [run("a", ns) for ns in (152, 161, 171, 181, 191)]
        rows, bad = self.check([base], [head])
        self.assertEqual(len(rows["a"]), 1)
        self.assertTrue(rows["a"][0].endswith("% time"))

    def test_p_slower(self):
        self.assertAlmostEqual(bench.p_slower([1, 2, 3], [1, 2, 3]), 0.5,
                               delta=0.15)
        self.assertLess(bench.p_slower([1, 2, 3, 4, 5], [6, 7, 8, 9, 10]),
                        0.01)
        self.assertGreater(bench.p_slower([6, 7, 8, 9, 10],
                                          [1, 2, 3, 4, 5]), 0.99)
        self.assertEqual(bench.p_slower([], [1]), 1.0)
        self.assertEqual(bench.p_slower([1, 1], [1, 1]), 1.0)

    def test_counts_only(self):
        rows, bad = self.check([[run("a", 100, 1)], [run("b", 100, 1)]],
                               [[run("a", 900, 1)], [run("b", 100, 2)]],
                               None)
        self.assertEqual(rows["a"], [])
        self.assertEqual(rows["b"], ["round_trips_per_run 1 -> 2"])
        out = io.StringIO()
        with redirect_stdout(out):
            bench.report(*bench.compare(report([run("a", 1)]),
                                        report([run("a", 9)]), None),
                         None)
        self.assertIn("time is not gated", out.getvalue())

    def test_faster_is_fine(self):
        rows, bad = self.check([[run("a", 100, 5, 20)]],
                               [[run("a", 50, 4, 19)]])
        self.assertEqual(bad, [])

    def test_median_over_rounds(self):
        # One slow outlier round does not make a regression.
        base = [run("a", 100), run("a", 100), run("a", 100)]
        head = [run("a", 100), run("a", 400), run("a", 101)]
        rows, bad = self.check([base], [head])
        self.assertEqual(bad, [])

    def test_xreply_round_trip(self):
        rows, bad = self.check([[run("a", 100, 10)]],
                               [[run("a", 90, 11)]])
        self.assertEqual(rows["a"], ["round_trips_per_run 10 -> 11"])

    def test_proxy_round_trip(self):
        rows, bad = self.check([[run("a", 100, 10, 77)]],
                               [[run("a", 100, 10, 78)]])
        self.assertEqual(rows["a"], ["proxy round_trips 77 -> 78"])

    def test_both(self):
        rows, bad = self.check([rounds("a", 100, 10, 77)],
                               [rounds("a", 200, 11, 78)])
        self.assertEqual(len(rows["a"]), 3)

    def test_jitter_is_not_an_increase(self):
        # A count that varies over the rounds (autorepeat) is compared
        # as head's lowest against base's highest.
        base = [run("a", 100, 10, 50), run("a", 100, 12, 52)]
        head = [run("a", 100, 11, 51), run("a", 100, 12, 52)]
        rows, bad = self.check([base], [head])
        self.assertEqual(bad, [])
        head = [run("a", 100, 13, 53), run("a", 100, 14, 54)]
        rows, bad = self.check([base], [head])
        self.assertEqual(len(rows["a"]), 2)

    def test_older_base(self):
        # A base whose xmbench has no exact count per run (nor a proxy)
        # is compared on the count per operation.
        rows, bad = self.check([[run("a", 100, 10, per_run=False)]],
                               [[run("a", 100, 10, 40)]])
        self.assertEqual(bad, [])
        rows, bad = self.check([[run("a", 100, 10, per_run=False)]],
                               [[run("a", 100, 20, 40)]])
        self.assertEqual(rows["a"], ["round_trips_per_op 0.01 -> 0.02"])

    def test_only_common_cases(self):
        rows, bad = self.check([[run("a", 100)], [run("gone", 100)]],
                               [[run("a", 100)], [run("new", 1e9, 99)]])
        self.assertEqual(list(rows), ["a"])

    def test_compare_command(self):
        base = report(rounds("a", 100, 1, 10), rounds("b", 100, 1, 10))
        head = report(rounds("a", 100, 1, 10), rounds("b", 120, 1, 10))
        with tempfile.TemporaryDirectory() as d:
            paths = []
            for name, data in (("base", base), ("head", head)):
                paths.append(os.path.join(d, name + ".json"))
                bench.write_json(paths[-1], data)
            summary = os.path.join(d, "summary.md")
            os.environ["GITHUB_STEP_SUMMARY"] = summary
            try:
                out = io.StringIO()
                with redirect_stdout(out):
                    rc = bench.main(["compare"] + paths)
                self.assertEqual(rc, 1)
                self.assertIn("b: +20.0% time", out.getvalue())
                with redirect_stdout(io.StringIO()):
                    rc = bench.main(["compare", "-t", "25"] + paths)
                self.assertEqual(rc, 0)
            finally:
                del os.environ["GITHUB_STEP_SUMMARY"]
            with open(summary) as f:
                md = f.read()
            self.assertIn("| b | 100.0 | 120.0 | +20.0% | 0.000 |", md)
            self.assertIn("No regression.", md)

    def test_report_is_json(self):
        # What gate writes must load back into compare.
        with tempfile.TemporaryDirectory() as d:
            path = os.path.join(d, "r.json")
            bench.write_json(path, report([run("a", 1, 2, 3)]))
            with open(path) as f:
                data = json.load(f)
        self.assertEqual(data["cases"][0]["proxy"]["round_trips"], 3)


if __name__ == "__main__":
    unittest.main()
