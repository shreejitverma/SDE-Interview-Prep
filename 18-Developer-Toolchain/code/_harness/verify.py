"""Re-run every Developer Toolchain lab and diff its output against what the notes print.

A lab is a directory under code/ that holds a `run.sh`. The script builds and runs the lab
and prints its result on stdout. Expected output lives next to it in
`expected/<platform>.txt`, where platform is `<os>-<arch>` (for example darwin-arm64 or
linux-aarch64), because a debugger or profiler prints different things on different systems.
Output blocks in the notes are copied from these files, so `make verify` turns version drift
into a failing check instead of a silent error.

Requirements are declared in the first lines of run.sh:

    # requires: linux, pmu

- linux:   needs a Linux kernel (ptrace, perf, rr, bpftrace). On any other host the lab runs in a
           container of the pinned image built from linux-labs.Dockerfile, when docker is reachable.
- macos:   runs only on a macOS host (Instruments, xctrace, codesign). On Linux such labs are skipped.
- docker:  the lab itself drives `docker` (building or running images), so it needs a working one.
- pmu:     needs hardware performance counters; set TOOLCHAIN_LABS_PMU=1 on a host that has them.
- nightly: needs a nightly Rust toolchain (Miri, -Zsanitizer).
- network: downloads something; set TOOLCHAIN_LABS_NETWORK=1 to allow it.

A lab whose requirements are not met is reported as skipped, never as passed.
An unknown requirement (a typo such as `linx`) fails the run, so a lab cannot silently stop running.

Container dispatch: a `linux` lab on a non-Linux host reads and writes expected/linux-<arch>.txt with
the container's arch, and works on a private copy of its directory, so it cannot dirty the tree.
No reachable docker, or TOOLCHAIN_LABS_CONTAINER=0, skips the lab and names the reason.
A docker error, image build failure, or timeout fails the lab; none of them is ever a skip.
The other requirements are judged for the container, not the host: see CONTAINER_LACKS.

Usage (from code/):
    python3 -m _harness.verify              # check every lab on this platform
    python3 -m _harness.verify --record     # (re)write expected output for this platform
    python3 -m _harness.verify 02-Version-Control/mini_git/   # only labs whose path/ contains this text
"""

from __future__ import annotations

import argparse
import difflib
import functools
import hashlib
import itertools
import os
import platform
import re
import shutil
import subprocess
import sys
from pathlib import Path

REQUIRES_RE = re.compile(r"^#\s*requires:\s*(.+)$", re.MULTILINE)
KNOWN = ("linux", "macos", "docker", "pmu", "nightly", "network")
TIMEOUT_S = 900
PROBE_TIMEOUT_S = 10
BUILD_TIMEOUT_S = 900
CONTAINER_SWITCH = "TOOLCHAIN_LABS_CONTAINER"
DOCKERFILE = Path(__file__).resolve().parent / "linux-labs.Dockerfile"
# Requirements a dispatched lab cannot meet, whatever the host offers.
CONTAINER_LACKS = {
    "pmu": "the container VM exposes no hardware counters",
    "nightly": "the lab image has no Rust toolchain",
    "docker": "the lab container gets no docker socket, which would hand it root on the VM",
}
# Least privilege that reproduces the recordings: no capabilities, no privilege gain, and the image's
# unprivileged user. Core dumps need only RLIMIT_CORE, since core_pattern belongs to the VM kernel and a
# container cannot change it. gdb can still trace its own children: the default seccomp profile allows ptrace.
CONTAINER_FLAGS = ("--init", "--cap-drop", "ALL", "--security-opt", "no-new-privileges", "--ulimit", "core=-1")
CONTAINER_SCRIPT = "cp -R /mnt/lab /tmp/lab && cd /tmp/lab && exec bash run.sh"
# Run-to-run noise with no teaching value: heap and stack addresses, and temp paths.
SCRUB = (
    (re.compile(r"0x[0-9a-fA-F]{8,16}"), "0xADDR"),
    (re.compile(r"/(?:private/)?(?:var/folders|tmp)/[^\s:'\"]+"), "/TMP"),
)


def make_key(system: str, machine: str) -> str:
    return f"{system.lower()}-{machine.lower().replace('x86_64', 'amd64')}"


def platform_key() -> str:
    return make_key(platform.system(), platform.machine())


def requirements(run_sh: Path) -> list[str]:
    head = "\n".join(run_sh.read_text().splitlines()[:10])
    m = REQUIRES_RE.search(head)
    return [r.strip() for r in m.group(1).split(",") if r.strip()] if m else []


def probe(cmd: list[str]) -> subprocess.CompletedProcess | None:
    """Run a requirement probe; None if the tool is missing or hangs past PROBE_TIMEOUT_S."""
    if shutil.which(cmd[0]) is None:
        return None
    try:
        return subprocess.run(cmd, capture_output=True, text=True, check=False, timeout=PROBE_TIMEOUT_S)
    except subprocess.TimeoutExpired:
        return None


def unmet(reqs: list[str]) -> list[str]:
    missing = []
    for r in reqs:
        if r == "linux" and platform.system() != "Linux":
            missing.append(r)
        elif r == "macos" and platform.system() != "Darwin":
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


def dispatches(reqs: list[str]) -> bool:
    return "linux" in reqs and "macos" not in reqs and platform.system() != "Linux"


def unmet_in_container(reqs: list[str]) -> list[str]:
    missing = [f"{r} ({CONTAINER_LACKS[r]})" for r in reqs if r in CONTAINER_LACKS]
    if "network" in reqs and os.environ.get("TOOLCHAIN_LABS_NETWORK") != "1":
        missing.append("network")
    return missing


class ContainerError(Exception):
    """Docker could not run the lab: always a failure, never a skip."""

    def __init__(self, summary: str, stderr: str = "") -> None:
        super().__init__(summary)
        self.summary = summary
        self.stderr = stderr[-2000:]


class LabContainer:
    """The pinned Linux image; one per harness run, so the probe and the build happen once."""

    def __init__(self) -> None:
        self.image = ""
        self._key = ""
        self._prepare_error: ContainerError | None = None
        self._names = itertools.count()

    @functools.cached_property
    def unavailable(self) -> str | None:
        """Why the container cannot be used at all, or None when docker is reachable."""
        host = platform.system().lower()
        if os.environ.get(CONTAINER_SWITCH) == "0":
            return f"host is {host} and {CONTAINER_SWITCH}=0 turns container dispatch off"
        if shutil.which("docker") is None:
            return f"host is {host} and there is no docker executable on PATH"
        p = probe(["docker", "info"])
        if p is None:
            return f"host is {host} and `docker info` did not answer within {PROBE_TIMEOUT_S}s"
        if p.returncode != 0:
            detail = p.stderr.strip().splitlines()[-1:] or [f"exit {p.returncode}"]
            return f"host is {host} and `docker info` failed: {detail[0]}"
        return None

    def prepare(self) -> str:
        """Build the image if needed and return the platform key of the container."""
        if self._prepare_error is not None:
            raise self._prepare_error
        if not self._key:
            try:
                self._key = self._build_and_ask_arch()
            except OSError as e:
                self._prepare_error = ContainerError(f"could not read {DOCKERFILE}: {e}")
                raise self._prepare_error from e
            except ContainerError as e:
                self._prepare_error = e
                raise
        return self._key

    def _build_and_ask_arch(self) -> str:
        self.image = "toolchain-labs:" + hashlib.sha256(DOCKERFILE.read_bytes()).hexdigest()[:12]
        if self._docker(["image", "inspect", self.image], PROBE_TIMEOUT_S).returncode != 0:
            print(f"BUILD {self.image} from {DOCKERFILE.name}", flush=True)
            cmd = ["build", "-f", str(DOCKERFILE), "-t", self.image, str(DOCKERFILE.parent)]
            built = self._docker(cmd, BUILD_TIMEOUT_S)
            if built.returncode != 0:
                raise ContainerError(f"docker build failed, exit {built.returncode}", built.stderr)
        asked = self._docker(["run", "--rm", "--network", "none", self.image, "uname", "-m"], PROBE_TIMEOUT_S * 6)
        machine = asked.stdout.strip()
        if asked.returncode != 0 or not machine:
            raise ContainerError(f"could not read the container arch, exit {asked.returncode}", asked.stderr)
        return make_key("linux", machine)

    @staticmethod
    def _docker(args: list[str], timeout: float) -> subprocess.CompletedProcess:
        try:
            return subprocess.run(["docker", *args], capture_output=True, text=True, check=False, timeout=timeout)
        except subprocess.TimeoutExpired as e:
            raise ContainerError(f"docker {args[0]} timed out after {timeout}s", stderr_of(e)) from e
        except OSError as e:
            raise ContainerError(f"could not run docker: {e}") from e

    def run(self, lab: Path, reqs: list[str]) -> subprocess.CompletedProcess:
        name = f"toolchain-lab-{os.getpid()}-{next(self._names)}"
        network = [] if "network" in reqs else ["--network", "none"]
        mount = f"type=bind,source={lab.resolve()},target=/mnt/lab,readonly"
        cmd = ["docker", "run", "--rm", "--name", name, *CONTAINER_FLAGS, *network, "--mount", mount,
               self.image, "sh", "-c", CONTAINER_SCRIPT]
        try:
            return subprocess.run(cmd, capture_output=True, text=True, check=False, timeout=TIMEOUT_S)
        except subprocess.TimeoutExpired as e:
            # Killing the docker client leaves the container running, so remove it by name.
            removed = probe(["docker", "rm", "-f", name])
            left = "" if removed is not None and removed.returncode == 0 else f"; container {name} may still be running"
            raise ContainerError(f"timed out after {TIMEOUT_S}s{left}", stderr_of(e)) from e
        except OSError as e:
            raise ContainerError(f"could not run docker: {e}") from e


def stderr_of(e: subprocess.TimeoutExpired) -> str:
    err = e.stderr or b""
    return err.decode(errors="replace") if isinstance(err, bytes) else err


def scrub(text: str) -> str:
    for pattern, repl in SCRUB:
        text = pattern.sub(repl, text)
    return text


def discover(root: Path, needle: str | None) -> list[Path]:
    labs = sorted(p.parent for p in root.rglob("run.sh") if "_harness" not in p.parts)
    return [lab for lab in labs if needle is None or needle in f"{lab.relative_to(root).as_posix()}/"]


def run_lab(lab: Path) -> subprocess.CompletedProcess:
    return subprocess.run(["bash", "run.sh"], cwd=lab, capture_output=True, text=True, check=False, timeout=TIMEOUT_S)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="Verify or record Developer Toolchain lab output.")
    ap.add_argument("needle", nargs="?", help="only labs whose path, with a trailing /, contains this text")
    ap.add_argument("--record", action="store_true", help="write expected output for this platform")
    ap.add_argument("--root", type=Path, default=Path(__file__).resolve().parent.parent)
    args = ap.parse_args(argv)

    key = platform_key()
    container = LabContainer()
    passed = failed = skipped = unrecorded = 0
    for lab in discover(args.root, args.needle):
        name = lab.relative_to(args.root).as_posix()
        reqs = requirements(lab / "run.sh")
        unknown = [r for r in reqs if r not in KNOWN]
        if unknown:
            failed += 1
            print(f"FAIL  {name} (unknown requirement: {', '.join(unknown)}; known: {', '.join(KNOWN)})")
            continue
        in_container = dispatches(reqs)
        missing = unmet_in_container(reqs) if in_container else unmet(reqs)
        if missing:
            skipped += 1
            print(f"SKIP  {name} (requires {', '.join(missing)})")
            continue
        if in_container and (why := container.unavailable):
            skipped += 1
            print(f"SKIP  {name} (requires linux: {why})")
            continue
        lab_key, where = key, ""
        try:
            if in_container:
                lab_key = container.prepare()
                where = f" (in container {container.image}, {lab_key})"
                proc = container.run(lab, reqs)
            else:
                proc = run_lab(lab)
        except subprocess.TimeoutExpired:
            failed += 1
            print(f"FAIL  {name} (timed out after {TIMEOUT_S}s)")
            continue
        except ContainerError as e:
            failed += 1
            print(f"FAIL  {name} ({e.summary})\n{e.stderr}")
            continue
        if proc.returncode != 0:
            failed += 1
            print(f"FAIL  {name} (exit {proc.returncode}){where}\n{proc.stderr[-2000:]}")
            continue
        actual = scrub(proc.stdout)
        expected_file = lab / "expected" / f"{lab_key}.txt"
        if args.record:
            expected_file.parent.mkdir(exist_ok=True)
            expected_file.write_text(actual)
            passed += 1
            print(f"REC   {name} -> expected/{lab_key}.txt{where}")
            continue
        if not expected_file.exists():
            unrecorded += 1
            print(f"MISS  {name} (no expected/{lab_key}.txt; run make record LAB={name}/ and review the diff)")
            continue
        expected = expected_file.read_text()
        if actual == expected:
            passed += 1
            print(f"PASS  {name}{where}")
        else:
            failed += 1
            diff = difflib.unified_diff(expected.splitlines(), actual.splitlines(),
                                        f"expected/{lab_key}.txt", "actual", lineterm="", n=2)
            print(f"FAIL  {name} (output differs){where}\n" + "\n".join(list(diff)[:60]))

    verb = "recorded" if args.record else "passed"
    print(f"labs: {passed} {verb}, {failed} failed, {skipped} skipped, {unrecorded} unrecorded on {key}")
    return 1 if failed or unrecorded else 0


if __name__ == "__main__":
    sys.exit(main())
