"""Re-run every Developer Toolchain lab and diff its output against what the notes print.

A lab is a directory under code/ that holds a `run.sh`. The script builds and runs the lab
and prints its result on stdout. Expected output lives next to it in
`expected/<platform>.txt`, where platform is `<os>-<arch>` (for example darwin-arm64 or
linux-aarch64), because a debugger or profiler prints different things on different systems.
Output blocks in the notes are copied from these files, so `make verify` turns version drift
into a failing check instead of a silent error.

Requirements are declared in the first lines of run.sh:

    # requires: linux, pmu

- linux:   runs only on a Linux host (ptrace, perf, rr, bpftrace). On macOS such labs are
           skipped; run `make verify` inside a Linux VM or container to exercise them.
- docker:  the lab itself drives `docker` (building or running images), so it needs a working one.
- pmu:     needs hardware performance counters; set TOOLCHAIN_LABS_PMU=1 on a host that has them.
- nightly: needs a nightly Rust toolchain (Miri, -Zsanitizer).
- network: downloads something; set TOOLCHAIN_LABS_NETWORK=1 to allow it.

A lab whose requirements are not met is reported as skipped, never as passed.
An unknown requirement (a typo such as `linx`) fails the run, so a lab cannot silently stop running.

Usage (from code/):
    python3 -m _harness.verify              # check every lab on this platform
    python3 -m _harness.verify --record     # (re)write expected output for this platform
    python3 -m _harness.verify 02-Version-Control/mini_git/   # only labs whose path/ contains this text
"""

from __future__ import annotations

import argparse
import difflib
import os
import platform
import re
import shutil
import subprocess
import sys
from pathlib import Path

REQUIRES_RE = re.compile(r"^#\s*requires:\s*(.+)$", re.M)
KNOWN = ("linux", "docker", "pmu", "nightly", "network")
TIMEOUT_S = 900
PROBE_TIMEOUT_S = 10
# Run-to-run noise with no teaching value: heap and stack addresses, and temp paths.
SCRUB = (
    (re.compile(r"0x[0-9a-fA-F]{8,16}"), "0xADDR"),
    (re.compile(r"/(?:private/)?(?:var/folders|tmp)/[^\s:'\"]+"), "/TMP"),
)


def platform_key() -> str:
    machine = platform.machine().lower().replace("x86_64", "amd64")
    return f"{platform.system().lower()}-{machine}"


def requirements(run_sh: Path) -> list[str]:
    head = "\n".join(run_sh.read_text().splitlines()[:10])
    m = REQUIRES_RE.search(head)
    return [r.strip() for r in m.group(1).split(",") if r.strip()] if m else []


def probe(cmd: list[str]) -> subprocess.CompletedProcess | None:
    """Run a requirement probe; None if the tool is missing or hangs past PROBE_TIMEOUT_S."""
    if shutil.which(cmd[0]) is None:
        return None
    try:
        return subprocess.run(cmd, capture_output=True, text=True, timeout=PROBE_TIMEOUT_S)
    except subprocess.TimeoutExpired:
        return None


def unmet(reqs: list[str]) -> list[str]:
    missing = []
    for r in reqs:
        if r == "linux" and platform.system() != "Linux":
            missing.append(r)
        elif r == "docker" and ((p := probe(["docker", "info"])) is None or p.returncode != 0):
            missing.append(r)
        elif r == "pmu" and os.environ.get("TOOLCHAIN_LABS_PMU") != "1":
            missing.append(r)
        elif r == "nightly" and ((p := probe(["rustup", "toolchain", "list"])) is None or "nightly" not in p.stdout):
            missing.append(r)
        elif r == "network" and os.environ.get("TOOLCHAIN_LABS_NETWORK") != "1":
            missing.append(r)
    return missing


def scrub(text: str) -> str:
    for pattern, repl in SCRUB:
        text = pattern.sub(repl, text)
    return text


def discover(root: Path, needle: str | None) -> list[Path]:
    labs = sorted(p.parent for p in root.rglob("run.sh") if "_harness" not in p.parts)
    return [lab for lab in labs if needle is None or needle in f"{lab.relative_to(root).as_posix()}/"]


def run_lab(lab: Path) -> subprocess.CompletedProcess:
    return subprocess.run(["bash", "run.sh"], cwd=lab, capture_output=True, text=True, timeout=TIMEOUT_S)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="Verify or record Developer Toolchain lab output.")
    ap.add_argument("needle", nargs="?", help="only labs whose path, with a trailing /, contains this text")
    ap.add_argument("--record", action="store_true", help="write expected output for this platform")
    ap.add_argument("--root", type=Path, default=Path(__file__).resolve().parent.parent)
    args = ap.parse_args(argv)

    key = platform_key()
    passed = failed = skipped = unrecorded = 0
    for lab in discover(args.root, args.needle):
        name = lab.relative_to(args.root).as_posix()
        reqs = requirements(lab / "run.sh")
        unknown = [r for r in reqs if r not in KNOWN]
        if unknown:
            failed += 1
            print(f"FAIL  {name} (unknown requirement: {', '.join(unknown)}; known: {', '.join(KNOWN)})")
            continue
        missing = unmet(reqs)
        if missing:
            skipped += 1
            print(f"SKIP  {name} (requires {', '.join(missing)})")
            continue
        try:
            proc = run_lab(lab)
        except subprocess.TimeoutExpired:
            failed += 1
            print(f"FAIL  {name} (timed out after {TIMEOUT_S}s)")
            continue
        if proc.returncode != 0:
            failed += 1
            print(f"FAIL  {name} (exit {proc.returncode})\n{proc.stderr[-2000:]}")
            continue
        actual = scrub(proc.stdout)
        expected_file = lab / "expected" / f"{key}.txt"
        if args.record:
            expected_file.parent.mkdir(exist_ok=True)
            expected_file.write_text(actual)
            passed += 1
            print(f"REC   {name} -> expected/{key}.txt")
            continue
        if not expected_file.exists():
            unrecorded += 1
            print(f"MISS  {name} (no expected/{key}.txt; run make record LAB={name}/ and review the diff)")
            continue
        expected = expected_file.read_text()
        if actual == expected:
            passed += 1
            print(f"PASS  {name}")
        else:
            failed += 1
            diff = difflib.unified_diff(expected.splitlines(), actual.splitlines(),
                                        f"expected/{key}.txt", "actual", lineterm="", n=2)
            print(f"FAIL  {name} (output differs)\n" + "\n".join(list(diff)[:60]))

    verb = "recorded" if args.record else "passed"
    print(f"labs: {passed} {verb}, {failed} failed, {skipped} skipped, {unrecorded} unrecorded on {key}")
    return 1 if failed or unrecorded else 0


if __name__ == "__main__":
    sys.exit(main())
