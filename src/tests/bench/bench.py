#!/usr/bin/env python3
#
# Motif
#
# Licensed under the LGPL 2.1 license.
#
"""Run xmbench reproducibly, and gate on regressions between two builds.

  bench.py run     [options] [-o OUT] BUILD... [--cases CASE|GROUP ...]
  bench.py compare [-t PCT] BASE.json HEAD.json
  bench.py gate    [options] [-o DIR] BASE_BUILD HEAD_BUILD
                   [--cases CASE|GROUP ...]

BUILD is a CMake build directory in which the xmbench target was built
(or the path of an xmbench binary).  Each build gets its own Xvfb,
started without access control, listening on a Unix socket only, pinned
with taskset to its own CPU; xmbench is pinned to another.  With
--delay MS, xmbench reaches the server through xmbench-proxy over TCP,
which delays each direction by MS milliseconds (like "tc netem delay MS"
on the loopback interface, without root) and counts the requests,
replies and round trips of the connection.

Every case runs in its own xmbench process, ROUNDS times (default 5);
the result of a case is the median over the rounds (each round is
itself the median of xmbench's REPEAT runs).  With two or more builds,
the rounds of the builds are interleaved, so that a change in the load
of the machine affects all of them alike.

"compare" and "gate" fail (exit status 1) when, for some case,
  - the median time per operation grew by more than PCT % (default 5),
    and a one-sided Mann-Whitney U test over the rounds gives a
    probability below --alpha (default 0.001) that this is noise, or
  - a round trip count grew: xmbench's _XReply count per timed run, or
    the proxy's count of round trips of the whole process.
A count is compared as the smallest value of the head build over the
rounds against the largest of the base build, so that a case whose
count depends on timing would not be reported for its jitter.  "gate"
runs both builds, compares them, and re-runs the cases whose median is
more than PCT % slower with --confirm more rounds (default 5) before it
decides: time on a shared machine is noisy, round trip counts are not.
Five rounds against five cannot reach the default alpha; ten can.
"""

import argparse
import json
import math
import os
import select
import shutil
import statistics
import subprocess
import sys
import tempfile
import time

XVFB_ARGS = ["-screen", "0", "1920x1080x24", "+extension", "RENDER",
             "-nolisten", "tcp", "-noreset"]

# Fields of an xmbench case, all medians over the rounds.
CASE_FIELDS = ("ns_per_op", "cpu_ns_per_op", "mallocs_per_op",
               "requests_per_op", "round_trips_per_op", "icvalues_per_op",
               "requests_per_run", "round_trips_per_run")
# Counts of the proxy, per xmbench process.
PROXY_FIELDS = ("requests", "replies", "errors", "events", "round_trips")
# A slowdown must be this unlikely to be noise (see p_slower).  It is
# small because about 40 cases are compared at once: five rounds against
# five cannot reach it, ten against ten can.
ALPHA = 0.001
# The counts that must not grow, and where they are.
GATED_COUNTS = (("round_trips_per_run", None), ("round_trips", "proxy"))


class BenchError(Exception):
    pass


def log(msg):
    print(msg, file=sys.stderr, flush=True)


# --------------------------------------------------------------------
# Running

def find_tool(build, name):
    """Find a program of the bench directory of a build."""
    if os.path.isfile(build) and os.access(build, os.X_OK):
        build = os.path.dirname(os.path.abspath(build))
    for sub in ("", os.path.join("src", "tests", "bench")):
        path = os.path.join(build, sub, name)
        if os.path.isfile(path) and os.access(path, os.X_OK):
            return os.path.abspath(path)
    return None


def pin(cpu):
    if cpu is None:
        return []
    return [shutil.which("taskset") or "taskset", "-c", str(cpu)]


def read_line(fd, timeout):
    """Read a line from a pipe, failing after timeout seconds."""
    data = b""
    end = time.monotonic() + timeout
    while not data.endswith(b"\n"):
        left = end - time.monotonic()
        if left <= 0 or not select.select([fd], [], [], left)[0]:
            raise BenchError("timed out waiting for a server to start")
        chunk = os.read(fd, 64)
        if not chunk:
            break
        data += chunk
    return data.decode().strip()


class Server:
    """An Xvfb, and a proxy in front of it when delay is not None."""

    def __init__(self, xvfb, proxy, delay, cpu, workdir, name):
        self.procs = []
        self.stats = None
        try:
            self.start(xvfb, proxy, delay, cpu, workdir, name)
        except BaseException:
            self.stop()
            raise

    def start(self, xvfb, proxy, delay, cpu, workdir, name):
        rd, wr = os.pipe()
        try:
            log_file = open(os.path.join(workdir, name + "-xvfb.log"), "w")
            proc = subprocess.Popen(
                pin(cpu) + [xvfb, "-displayfd", str(wr)] + XVFB_ARGS,
                pass_fds=(wr,), stdin=subprocess.DEVNULL,
                stdout=log_file, stderr=subprocess.STDOUT)
            log_file.close()
            self.procs.append(proc)
            os.close(wr)
            wr = None
            number = read_line(rd, 30)
        finally:
            if wr is not None:
                os.close(wr)
            os.close(rd)
        if not number.isdigit():
            raise BenchError("Xvfb did not start, see %s-xvfb.log" % name)
        self.display = ":" + number
        if delay is None:
            return
        self.stats = os.path.join(workdir, name + "-proxy.jsonl")
        open(self.stats, "w").close()
        upstream = "/tmp/.X11-unix/X" + number
        # Look for a free port 6000 + N above the X displays.
        for port in range(200 + int(number), 1200):
            proc = subprocess.Popen(
                pin(cpu) + [proxy, "-d", str(delay), "-s", self.stats,
                            str(port), upstream],
                stdin=subprocess.DEVNULL, stdout=subprocess.PIPE)
            try:
                ready = read_line(proc.stdout.fileno(), 10) == "ready"
            except BenchError:
                ready = False
            if ready:
                self.procs.append(proc)
                self.display = "127.0.0.1:%d" % port
                return
            proc.kill()
            proc.wait()
            proc.stdout.close()
        raise BenchError("no free port for xmbench-proxy")

    def proxy_lines(self):
        if not self.stats:
            return []
        with open(self.stats) as f:
            return [json.loads(line) for line in f if line.strip()]

    def stop(self):
        for proc in reversed(self.procs):
            if proc.poll() is None:
                proc.terminate()
                try:
                    proc.wait(10)
                except subprocess.TimeoutExpired:
                    proc.kill()
                    proc.wait()
            if proc.stdout:
                proc.stdout.close()
        self.procs = []


class Build:
    def __init__(self, name, path):
        self.name = name
        self.xmbench = find_tool(path, "xmbench")
        if not self.xmbench:
            raise BenchError("%s: no xmbench (build the xmbench target)"
                             % path)
        self.server = None
        self.cases = {}     # name -> list of per-round results


def list_cases(xmbench, selection):
    """The case names of an xmbench, in its order, that match selection."""
    out = subprocess.run([xmbench, "-l"], check=True, capture_output=True,
                         text=True, env=dict(os.environ,
                                             XMBENCH_NO_PRELOAD="1")).stdout
    cases = []
    for line in out.splitlines():
        fields = line.split()
        if len(fields) >= 2 and (not selection or "all" in selection or
                                 fields[0] in selection or
                                 fields[1] in selection):
            cases.append(fields[0])
    return cases


def xmbench(build, args, xmargs):
    """Run xmbench with the display of its build."""
    env = dict(os.environ, DISPLAY=build.server.display,
               XMODIFIERS="@im=none", XAUTHORITY=os.devnull)
    proc = subprocess.run(pin(args.client_cpu) + [build.xmbench] + xmargs,
                          env=env, stdout=subprocess.DEVNULL,
                          stderr=subprocess.PIPE, text=True,
                          timeout=args.timeout)
    if proc.returncode != 0:
        raise BenchError("%s: xmbench %s: exit status %d\n%s" % (
            build.name, " ".join(xmargs), proc.returncode, proc.stderr))


def warm_up(build, args):
    """Run every case once with one operation.  The first Motif client
    of a display leaves state on the server (the drag window, which it
    creates through a second connection, and the atoms) that later
    clients reuse, so the first measured process would differ."""
    xmbench(build, args, ["-r", "1", "-s", "1e-9"])


def run_one(build, case, args, workdir):
    """Run one case once in a new xmbench process."""
    server = build.server
    before = len(server.proxy_lines())
    out = os.path.join(workdir, "%s-%s.json" % (build.name, case))
    xmbench(build, args, ["-r", str(args.repeat), "-s", str(args.scale),
                          "-j", out, case])
    with open(out) as f:
        data = json.load(f)
    os.unlink(out)
    if len(data.get("cases", [])) != 1:
        raise BenchError("%s %s: no result" % (build.name, case))
    result = data["cases"][0]
    # The proxy writes a line once the connection has closed.  An older
    # xmbench does not say whether it opened the display.
    opened = data.get("display")
    if server.stats and opened is not False:
        end = time.monotonic() + (10 if opened else 2)
        while len(server.proxy_lines()) <= before:
            if time.monotonic() > end:
                if opened:
                    raise BenchError("%s %s: no proxy statistics"
                                     % (build.name, case))
                return result
            time.sleep(0.01)
        # The connections of a process close together, when it exits.
        lines = server.proxy_lines()[before:]
        while True:
            time.sleep(0.1)
            more = server.proxy_lines()[before:]
            if len(more) == len(lines):
                break
            lines = more
        result["proxy"] = {k: sum(line[k] for line in lines)
                           for k in PROXY_FIELDS}
    return result


def summarize(runs):
    """Fold the results of the rounds of a case into one."""
    res = {"name": runs[0]["name"], "group": runs[0]["group"],
           "n": runs[0]["n"], "rounds": len(runs)}
    for key in CASE_FIELDS:
        values = [r[key] for r in runs if key in r]
        if values:
            res[key] = statistics.median(values)
            if key.endswith("_per_run"):
                res[key + "_range"] = [min(values), max(values)]
    res["ns_per_op_runs"] = [r["ns_per_op"] for r in runs]
    res["cpu_ns_per_op_runs"] = [r["cpu_ns_per_op"] for r in runs]
    proxied = [r["proxy"] for r in runs if "proxy" in r]
    if proxied:
        res["proxy"] = {k: statistics.median(p[k] for p in proxied)
                        for k in PROXY_FIELDS}
        res["proxy"]["round_trips_range"] = [
            min(p["round_trips"] for p in proxied),
            max(p["round_trips"] for p in proxied)]
    return res


def report_json(build, args):
    return {
        "bench": "xmbench",
        "runner": "bench.py",
        "version": 1,
        "build": build.name,
        "xmbench": build.xmbench,
        "rounds": args.rounds,
        "repeat": args.repeat,
        "scale": args.scale,
        "delay_ms": args.delay,
        "client_cpu": args.client_cpu,
        "server_cpu": args.server_cpu,
        "xvfb_args": XVFB_ARGS,
        "cases": [summarize(runs) for runs in build.cases.values()],
    }


def default_cpus(args):
    """Pin the client and the server to the last two allowed CPUs."""
    if args.no_pin or not shutil.which("taskset"):
        args.client_cpu = args.server_cpu = None
        return
    cpus = sorted(os.sched_getaffinity(0))
    if args.client_cpu is None:
        args.client_cpu = cpus[-1]
    if args.server_cpu is None:
        others = [c for c in cpus if c != args.client_cpu]
        args.server_cpu = others[-1] if others else args.client_cpu


def run_builds(builds, cases, args, rounds, workdir):
    """Run the rounds, interleaving the builds; results go in builds."""
    for r in range(rounds):
        # Alternate the order, so that neither build always runs first.
        order = builds if r % 2 == 0 else builds[::-1]
        for case in cases:
            for build in order:
                result = run_one(build, case, args, workdir)
                build.cases.setdefault(case, []).append(result)
                proxy = result.get("proxy", {})
                log("round %d/%d %-24s %-5s %12.1f ns/op %6g rtrips%s" % (
                    r + 1, rounds, case, build.name, result["ns_per_op"],
                    result.get("round_trips_per_run",
                               result["round_trips_per_op"]),
                    " %d proxy rtrips" % proxy["round_trips"]
                    if proxy else ""))


def start(builds, args, workdir):
    xvfb = shutil.which("Xvfb")
    if not xvfb:
        raise BenchError("Xvfb not found")
    proxy = None
    if args.delay is not None:
        proxy = args.proxy or find_tool(args.builds[-1], "xmbench-proxy")
        if not proxy:
            raise BenchError("no xmbench-proxy (build it, or give --proxy)")
    for build in builds:
        build.server = Server(xvfb, proxy, args.delay, args.server_cpu,
                              workdir, build.name)
        warm_up(build, args)


def stop(builds):
    for build in builds:
        if build.server:
            build.server.stop()
            build.server = None


def named_builds(paths, names):
    if names and len(names) != len(paths):
        raise BenchError("give as many --name as builds")
    names = names or [os.path.basename(os.path.normpath(p)) or "build"
                      for p in paths]
    if len(set(names)) != len(names):
        names = ["build%d" % i for i in range(len(paths))]
    return [Build(n, p) for n, p in zip(names, paths)]


def common_cases(builds, selection):
    lists = [list_cases(b.xmbench, selection) for b in builds]
    cases = [c for c in lists[-1] if all(c in l for l in lists)]
    for build, l in zip(builds, lists):
        for c in l:
            if c not in cases:
                log("note: %s only in %s, not run" % (c, build.name))
    if not cases:
        raise BenchError("no case to run")
    return cases


def write_json(path, data):
    with open(path, "w") as f:
        json.dump(data, f, indent=1)
        f.write("\n")


# --------------------------------------------------------------------
# Comparing

def count_of(case, key, where):
    """(low, high) of a gated count over the rounds, or None."""
    src = case.get(where) if where else case
    if not src or key not in src:
        return None
    rng = src.get(key + "_range")
    return tuple(rng) if rng else (src[key], src[key])


def counts(base, head, key, where):
    """A gated count of both cases, and its name.  An older xmbench has
    no exact count per run; then compare the rounded count per
    operation."""
    if key == "round_trips_per_run" and not (key in base and key in head):
        key = "round_trips_per_op"
    return count_of(base, key, where), count_of(head, key, where), key


def p_slower(base, head):
    """One-sided Mann-Whitney U test: the probability that head's times
    would be at least this much above base's by chance (normal
    approximation, with the correction for ties)."""
    n1, n2 = len(base), len(head)
    if not n1 or not n2:
        return 1.0
    u = sum((h > b) + 0.5 * (h == b) for h in head for b in base)
    values = sorted(base + head)
    n = n1 + n2
    ties = sum(t ** 3 - t for t in (values.count(v) for v in set(values)))
    var = n1 * n2 / 12.0 * ((n + 1) - ties / float(n * (n - 1)))
    if var <= 0:
        return 1.0
    z = (u - n1 * n2 / 2.0 - 0.5) / math.sqrt(var)
    return 0.5 * math.erfc(z / math.sqrt(2))


def compare(base, head, threshold, alpha=ALPHA):
    """Compare two reports; returns (rows, regressions).  A threshold of
    None compares the round trip counts only."""
    base_cases = {c["name"]: c for c in base["cases"]}
    rows, bad = [], []
    for h in head["cases"]:
        b = base_cases.get(h["name"])
        if not b:
            continue
        change = (h["ns_per_op"] / b["ns_per_op"] - 1) * 100 \
            if b["ns_per_op"] > 0 else 0.0
        p = p_slower(b.get("ns_per_op_runs", []),
                     h.get("ns_per_op_runs", []))
        problems = []
        if threshold is not None and change > threshold + 1e-9 and \
                p < alpha:
            problems.append("%+.1f%% time" % change)
        row = {"name": h["name"], "base_ns": b["ns_per_op"],
               "head_ns": h["ns_per_op"], "change": change, "p": p}
        for key, where in GATED_COUNTS:
            bc, hc, key = counts(b, h, key, where)
            if bc and hc and hc[0] > bc[1]:
                problems.append("%s%s %g -> %g" % (
                    "proxy " if where else "", key, bc[1], hc[0]))
            col = "proxy" if where else "rtrips"
            row["base_" + col], row["head_" + col] = bc, hc
        row["problems"] = problems
        rows.append(row)
        if problems:
            bad.append(row)
    return rows, bad


def fmt_count(c):
    if c is None:
        return "-"
    return "%g" % c[0] if c[0] == c[1] else "%g-%g" % c


def table(rows, threshold, alpha, markdown=False):
    head = ("case", "base ns/op", "head ns/op", "change", "p", "rtrips",
            "proxy rtrips", "verdict")
    lines = []
    for r in rows:
        verdict = "; ".join(r["problems"]) or "ok"
        lines.append((r["name"], "%.1f" % r["base_ns"],
                      "%.1f" % r["head_ns"], "%+.1f%%" % r["change"],
                      "%.3f" % r["p"],
                      "%s -> %s" % (fmt_count(r["base_rtrips"]),
                                    fmt_count(r["head_rtrips"])),
                      "%s -> %s" % (fmt_count(r["base_proxy"]),
                                    fmt_count(r["head_proxy"])),
                      verdict))
    if markdown:
        out = ["| " + " | ".join(head) + " |",
               "|" + "---|" * len(head)]
        out += ["| " + " | ".join(l) + " |" for l in lines]
    else:
        widths = [max(len(x[i]) for x in [head] + lines)
                  for i in range(len(head))]
        out = ["  ".join(x[i].ljust(widths[i]) for i in range(len(head)))
               .rstrip() for x in [head] + lines]
    out.append("")
    if threshold is None:
        out.append("Gated: any increase of a round trip count (time is "
                   "not gated).")
    else:
        out.append("Gated: a median time per operation more than %g %% "
                   "slower, with p < %g that it is noise (one-sided "
                   "Mann-Whitney U over the rounds); any increase of a "
                   "round trip count." % (threshold, alpha))
    return "\n".join(out)


def report(rows, bad, threshold, label=None, alpha=ALPHA):
    print(table(rows, threshold, alpha))
    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with open(summary, "a") as f:
            f.write("### xmbench%s: head against base\n\n"
                    % (" (%s)" % label if label else ""))
            f.write(table(rows, threshold, alpha, markdown=True) + "\n\n")
            f.write("**%d regression(s)**\n" % len(bad) if bad
                    else "No regression.\n")
    if bad:
        print("\n%d regression(s):" % len(bad))
        for r in bad:
            print("  %s: %s" % (r["name"], "; ".join(r["problems"])))
    return 1 if bad else 0


# --------------------------------------------------------------------
# Commands

def cmd_run(args):
    builds = named_builds(args.builds, args.name)
    cases = common_cases(builds, args.cases)
    default_cpus(args)
    workdir = tempfile.mkdtemp(prefix="xmbench-", dir=args.workdir)
    try:
        start(builds, args, workdir)
        run_builds(builds, cases, args, args.rounds, workdir)
    finally:
        stop(builds)
        shutil.rmtree(workdir, ignore_errors=True)
    for build in builds:
        data = report_json(build, args)
        if len(builds) == 1 and args.output:
            path = args.output
        else:
            path = os.path.join(args.output or ".", build.name + ".json")
        write_json(path, data)
        log("wrote %s" % path)
    return 0


def cmd_compare(args):
    with open(args.base) as f:
        base = json.load(f)
    with open(args.head) as f:
        head = json.load(f)
    rows, bad = compare(base, head, args.threshold, args.alpha)
    return report(rows, bad, args.threshold, args.label, args.alpha)


def cmd_gate(args):
    builds = named_builds(args.builds, ["base", "head"])
    cases = common_cases(builds, args.cases)
    default_cpus(args)
    out = args.output or "."
    os.makedirs(out, exist_ok=True)
    workdir = tempfile.mkdtemp(prefix="xmbench-", dir=args.workdir)
    try:
        start(builds, args, workdir)
        run_builds(builds, cases, args, args.rounds, workdir)
        reports = [report_json(b, args) for b in builds]
        rows, bad = compare(reports[0], reports[1], args.threshold,
                            args.alpha)
        # Time is noisy: confirm with more rounds of the suspects.
        # Not only the significant ones: five rounds can hide a shift.
        slow = [r["name"] for r in rows if args.threshold is not None and
                r["change"] > args.threshold]
        if slow and args.confirm:
            log("confirming %s with %d more rounds" % (" ".join(slow),
                                                       args.confirm))
            run_builds(builds, slow, args, args.confirm, workdir)
    finally:
        stop(builds)
        shutil.rmtree(workdir, ignore_errors=True)
    reports = [report_json(b, args) for b in builds]
    for build, data in zip(builds, reports):
        write_json(os.path.join(out, build.name + ".json"), data)
    rows, bad = compare(reports[0], reports[1], args.threshold, args.alpha)
    return report(rows, bad, args.threshold, args.label, args.alpha)


def main(argv=None):
    parser = argparse.ArgumentParser(
        description=__doc__.split("\n")[0],
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="\n".join(__doc__.split("\n")[1:]))
    sub = parser.add_subparsers(dest="command", required=True)

    def run_options(p):
        p.add_argument("-r", "--rounds", type=int, default=5,
                       help="xmbench processes per case (default 5)")
        p.add_argument("--repeat", type=int, default=3,
                       help="xmbench -r: runs per process (default 3)")
        p.add_argument("-s", "--scale", type=float, default=1.0,
                       help="xmbench -s: operation count scale")
        p.add_argument("-d", "--delay", type=float, default=None,
                       metavar="MS",
                       help="run through xmbench-proxy with this "
                       "latency each way (0: proxy without delay)")
        p.add_argument("--proxy", help="xmbench-proxy to use (default: "
                       "the one of the last build)")
        p.add_argument("--client-cpu", type=int, default=None)
        p.add_argument("--server-cpu", type=int, default=None)
        p.add_argument("--no-pin", action="store_true",
                       help="do not pin with taskset")
        p.add_argument("--timeout", type=float, default=1800,
                       help="seconds allowed per xmbench process")
        p.add_argument("--workdir", default=None,
                       help="directory for the temporary files")

    p = sub.add_parser("run", help="run builds, write one JSON each")
    run_options(p)
    p.add_argument("-o", "--output",
                   help="JSON file (one build) or directory")
    p.add_argument("--name", action="append",
                   help="name of a build, once per build")
    p.add_argument("builds", nargs="+", metavar="BUILD")
    p.add_argument("--cases", nargs="*", default=[],
                   help="cases or groups (default: all)")
    p.set_defaults(func=cmd_run)

    def compare_options(p):
        p.add_argument("-t", "--threshold", type=float, default=5.0,
                       help="allowed slowdown in %% (default 5)")
        p.add_argument("--alpha", type=float, default=ALPHA,
                       help="largest p that a slowdown is noise "
                       "(default %g)" % ALPHA)
        p.add_argument("--counts-only", dest="threshold",
                       action="store_const", const=None,
                       help="gate on the round trip counts only")
        p.add_argument("--label",
                       help="name of the comparison in the step summary")

    p = sub.add_parser("compare", help="compare two JSON reports")
    compare_options(p)
    p.add_argument("base")
    p.add_argument("head")
    p.set_defaults(func=cmd_compare)

    p = sub.add_parser("gate", help="run and compare base and head")
    run_options(p)
    compare_options(p)
    p.add_argument("--confirm", type=int, default=5,
                   help="extra rounds for the cases that look slower "
                   "(default 5, 0: none)")
    p.add_argument("-o", "--output",
                   help="directory for base.json and head.json")
    p.add_argument("builds", nargs=2, metavar="BUILD",
                   help="the base build, then the head build")
    p.add_argument("--cases", nargs="*", default=[],
                   help="cases or groups (default: all)")
    p.set_defaults(func=cmd_gate)

    args = parser.parse_args(argv)
    try:
        return args.func(args)
    except (BenchError, subprocess.SubprocessError, OSError) as e:
        log("bench.py: %s" % e)
        return 2


if __name__ == "__main__":
    sys.exit(main())
