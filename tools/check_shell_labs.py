#!/usr/bin/env python3
"""Gate the shell notes' labs: ShellCheck, shfmt, and bats over every tracked lab file.

Scope is `19-Shell-and-Unix-Engineering/labs/`, read through `git ls-files` like every vault tool,
so a new lab is checked once it is staged.
A file's dialect comes from its shebang, then a `# shellcheck shell=` directive in its leading comment block
(read only for files with a shell suffix or none), then its suffix (.sh, .bash, .bats, or .zsh).
A file with none of these is not a shell file.

Checks, all of which must pass:
    structure   every shell file names its dialect (a shebang or a `# shellcheck shell=` directive),
                every file with a shebang is executable in git, and every lab directory has a .bats test
    shellcheck  all non-zsh files, configured by labs/.shellcheckrc (ShellCheck has no zsh support);
                ash and busybox files are checked as busybox, yash and posh as sh, ksh93 and mksh as ksh
    shfmt       `shfmt -d` on all files, configured by labs/.editorconfig
    zsh -n      a syntax check for each zsh file
    bats        every .bats file, under the default LAB_SHELL (the cross-shell matrix is labs/run-matrix.sh),
                with each test bounded by BATS_TEST_TIMEOUT (default 60 seconds) so a blocking lab fails instead of hanging,
                and the whole suite killed after LAB_SUITE_TIMEOUT (default 600 seconds) in case a test outlives its own

A missing linter is an error (exit 2), never a skip.
Exit 0 when everything passes or no lab files are tracked, 1 on any finding.

Usage (from the repo root):
    python3 tools/check_shell_labs.py
"""

from __future__ import annotations

import argparse
import contextlib
import itertools
import os
import re
import shutil
import signal
import subprocess
import sys
import tempfile
from pathlib import Path, PurePosixPath

LABS = "19-Shell-and-Unix-Engineering/labs"
# Lab directories that hold shared support files rather than a lab.
SUPPORT_DIRS = {"lib", "matrix"}
SUFFIX_DIALECT = {".sh": "sh", ".bash": "bash", ".bats": "bats", ".zsh": "zsh"}
SHEBANG_RE = re.compile(
    r"^#!\s*(?:/usr/bin/env\s+(?:-S\s+)?)?(?:\S*/)?(sh|ash|bash|dash|ksh|ksh93|mksh|yash|posh|busybox|bats|zsh)\b")
DIRECTIVE_RE = re.compile(r"^#\s*shellcheck\s+shell=(\w+)")
# Dialects checked under a fixed --shell rather than ShellCheck's own shebang detection.
SHELLCHECK_SHELL = {"ash": "busybox", "busybox": "busybox", "yash": "sh", "posh": "sh",
                    "ksh93": "ksh", "mksh": "ksh"}
BATS_TIMEOUT_DEFAULT = "60"
SUITE_TIMEOUT_DEFAULT = "600"
INSTALL_HINT = "install the lab toolchain: `brew install shellcheck shfmt bats-core` (macOS) or see 19-Shell-and-Unix-Engineering/labs/README.md"


def tracked(repo: Path) -> dict[str, str]:
    """Tracked files under LABS mapped to their git mode (e.g. 100755)."""
    out = subprocess.run(["git", "-C", str(repo), "ls-files", "-s", "--", LABS],
                         capture_output=True, text=True, check=True).stdout
    files = {}
    for line in out.splitlines():
        meta, path = line.split("\t", 1)
        files[path] = meta.split()[0]
    return files


def dialect(repo: Path, path: str) -> tuple[str | None, bool, bool]:
    """Return (dialect, declared, has_shebang) for a lab file; dialect is None for non-shell files."""
    lines = (repo / path).read_text(errors="replace").split("\n")
    shebang = SHEBANG_RE.match(lines[0])
    suffix = PurePosixPath(path).suffix
    directive = None
    if not suffix or suffix in SUFFIX_DIALECT:
        head = itertools.takewhile(lambda line: line.startswith("#"), lines)
        directive = next(filter(None, map(DIRECTIVE_RE.match, head)), None)
    found = (shebang and shebang[1]) or (directive and directive[1]) or SUFFIX_DIALECT.get(suffix)
    return found, bool(shebang or directive), lines[0].startswith("#!")


def run(cmd: list[str], repo: Path, env: dict[str, str] | None = None) -> tuple[int, str]:
    proc = subprocess.run(cmd, cwd=repo, capture_output=True, text=True, env=env, check=False)
    return proc.returncode, (proc.stdout + proc.stderr).strip()


def run_bounded(cmd: list[str], repo: Path, env: dict[str, str], limit: float) -> tuple[int, str]:
    """Run cmd in its own session and kill its process group after limit seconds.

    Output goes to a file rather than a pipe, so a stray child that keeps it open
    (a stopped process in another group, say) cannot hold the gate once cmd is gone.
    """
    with tempfile.TemporaryFile("w+") as out:
        proc = subprocess.Popen(cmd, cwd=repo, env=env, stdin=subprocess.DEVNULL, stdout=out,
                                stderr=subprocess.STDOUT, text=True, start_new_session=True)
        note = ""
        try:
            rc = proc.wait(timeout=limit)
        except subprocess.TimeoutExpired:
            with contextlib.suppress(ProcessLookupError):
                os.killpg(proc.pid, signal.SIGKILL)
            proc.wait()
            rc, note = 1, f"\nsuite killed after LAB_SUITE_TIMEOUT={limit:g} seconds"
        out.seek(0)
        return rc, out.read().strip() + note


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo", type=Path, default=Path(__file__).resolve().parent.parent)
    args = ap.parse_args(argv)
    repo = args.repo.resolve()
    try:
        suite_limit = float(os.environ.get("LAB_SUITE_TIMEOUT", SUITE_TIMEOUT_DEFAULT))
    except ValueError:
        print(f"error: LAB_SUITE_TIMEOUT must be a number of seconds, not {os.environ['LAB_SUITE_TIMEOUT']!r}",
              file=sys.stderr)
        return 2

    modes = tracked(repo)
    shell_files: dict[str, str] = {}
    problems: list[str] = []
    for path in sorted(modes):
        found, declared, has_shebang = dialect(repo, path)
        if found is None:
            continue
        shell_files[path] = found
        if not declared:
            problems.append(f"{path}: no shebang or shellcheck shell= directive, so its dialect is a guess")
        if has_shebang and modes[path] != "100755":
            problems.append(f"{path}: has a shebang but is not executable in git (git update-index --chmod=+x)")

    if not shell_files:
        print(f"shell labs: 0 files tracked under {LABS}; nothing to check")
        return 0

    zsh_files = [p for p, d in shell_files.items() if d == "zsh"]
    lint_files = [p for p, d in shell_files.items() if d != "zsh"]
    bats_files = [p for p, d in shell_files.items() if d == "bats" or p.endswith(".bats")]

    labs = {PurePosixPath(p).relative_to(LABS).parts[0] for p in modes if len(PurePosixPath(p).relative_to(LABS).parts) > 1}
    for lab in sorted(labs - SUPPORT_DIRS):
        if not any(p.startswith(f"{LABS}/{lab}/") for p in bats_files):
            problems.append(f"{lab}: no .bats test; every lab proves its claims with one")
    needed = ["shellcheck", "shfmt"] + (["bats"] if bats_files else []) + (["zsh"] if zsh_files else [])
    missing = [t for t in needed if shutil.which(t) is None]
    if missing:
        print(f"error: {', '.join(missing)} not found on PATH; {INSTALL_HINT}", file=sys.stderr)
        return 2

    print(f"shell labs: {len(shell_files)} files ({len(lint_files)} shellcheck, {len(shell_files)} shfmt, "
          f"{len(bats_files)} bats, {len(zsh_files)} zsh)")
    failed: list[str] = []

    def report(name: str, rc: int, output: str) -> None:
        if rc != 0:
            failed.append(name)
            print(f"FAIL {name}")
            if output:
                print(output)

    if problems:
        report("structure", 1, "\n".join(problems))
    if lint_files:
        groups: dict[str | None, list[str]] = {}
        for p in lint_files:
            groups.setdefault(SHELLCHECK_SHELL.get(shell_files[p]), []).append(p)
        sc = [run(["shellcheck", "--format=gcc", *([f"--shell={s}"] if s else []), *files], repo)
              for s, files in groups.items()]
        report("shellcheck", max(rc for rc, _ in sc), "\n".join(out for _, out in sc if out))
    report("shfmt", *run(["shfmt", "-d", *shell_files], repo))
    zsh_out = [(p, *run(["zsh", "-n", p], repo)) for p in zsh_files]
    if zsh_bad := [f"{p}: {out}" for p, rc, out in zsh_out if rc != 0]:
        report("zsh -n", 1, "\n".join(zsh_bad))
    if bats_files:
        bats_env = {**os.environ, "BATS_TEST_TIMEOUT": os.environ.get("BATS_TEST_TIMEOUT", BATS_TIMEOUT_DEFAULT)}
        report("bats", *run_bounded(["bats", *bats_files], repo, bats_env, suite_limit))

    print(f"result: FAIL ({', '.join(failed)})" if failed else "result: PASS")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
