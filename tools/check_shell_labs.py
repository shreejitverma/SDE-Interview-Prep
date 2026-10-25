#!/usr/bin/env python3
"""Gate the shell curriculum's labs: ShellCheck, shfmt, and bats over every tracked lab file.

Scope is `18-Shell-and-Unix-Engineering/labs/`, read through `git ls-files` like every vault tool,
so a new lab is checked once it is staged.
A file is a shell file when its suffix is .sh, .bash, .bats, or .zsh, or its first line is a shell shebang.

Checks, all of which must pass:
    structure   every shell file names its dialect (a shebang or a `# shellcheck shell=` directive),
                every file with a shebang is executable in git, and every lab directory has a .bats test
    shellcheck  all non-zsh files in one run, configured by labs/.shellcheckrc (ShellCheck has no zsh support)
    shfmt       `shfmt -d` on all files, configured by labs/.editorconfig
    zsh -n      a syntax check for each zsh file
    bats        every .bats file, under the default LAB_SHELL (the cross-shell matrix is labs/run-matrix.sh)

A missing linter is an error (exit 2), never a skip.
Exit 0 when everything passes or no lab files are tracked, 1 on any finding.

Usage (from the repo root):
    python3 tools/check_shell_labs.py          # run every check
    python3 tools/check_shell_labs.py --list   # show each file's dialect without running anything
"""

from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from pathlib import Path, PurePosixPath

LABS = "18-Shell-and-Unix-Engineering/labs"
# Lab directories that hold shared support files rather than a lab.
SUPPORT_DIRS = {"lib", "matrix"}
SUFFIX_DIALECT = {".sh": "sh", ".bash": "bash", ".bats": "bats", ".zsh": "zsh"}
SHEBANG_RE = re.compile(r"^#!\s*(?:/usr/bin/env\s+(?:-S\s+)?)?(?:\S*/)?(sh|bash|dash|ksh|mksh|bats|zsh)\b")
DIRECTIVE_RE = re.compile(r"^#\s*shellcheck\s+shell=(\w+)", re.M)
INSTALL_HINT = "install the lab toolchain: `brew install shellcheck shfmt bats-core` (macOS) or see 18-Shell-and-Unix-Engineering/labs/README.md"


def tracked(repo: Path) -> dict[str, str]:
    """Tracked files under LABS mapped to their git mode (e.g. 100755)."""
    out = subprocess.run(["git", "-C", str(repo), "ls-files", "-s", "--", LABS],
                         capture_output=True, text=True, check=True).stdout
    files = {}
    for line in out.splitlines():
        meta, path = line.split("\t", 1)
        files[path] = meta.split()[0]
    return files


def dialect(repo: Path, path: str) -> tuple[str | None, bool]:
    """Return (dialect, declared) for a lab file; dialect is None for non-shell files."""
    text = (repo / path).read_text(errors="replace")
    first = text.split("\n", 1)[0]
    shebang = SHEBANG_RE.match(first)
    directive = DIRECTIVE_RE.search(text)
    by_suffix = SUFFIX_DIALECT.get(PurePosixPath(path).suffix)
    found = by_suffix or (shebang and shebang[1]) or (directive and directive[1])
    return found, bool(shebang or directive)


def run(cmd: list[str], repo: Path) -> tuple[int, str]:
    proc = subprocess.run(cmd, cwd=repo, capture_output=True, text=True)
    return proc.returncode, (proc.stdout + proc.stderr).strip()


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo", type=Path, default=Path(__file__).resolve().parent.parent)
    ap.add_argument("--list", action="store_true", help="print each shell file's dialect and exit")
    args = ap.parse_args(argv)
    repo = args.repo.resolve()

    modes = tracked(repo)
    shell_files: dict[str, str] = {}
    problems: list[str] = []
    for path in sorted(modes):
        found, declared = dialect(repo, path)
        if found is None:
            continue
        shell_files[path] = found
        if not declared:
            problems.append(f"{path}: no shebang or shellcheck shell= directive, so its dialect is a guess")
        has_shebang = (repo / path).read_text(errors="replace").startswith("#!")
        if has_shebang and modes[path] != "100755":
            problems.append(f"{path}: has a shebang but is not executable in git (git update-index --chmod=+x)")

    if args.list:
        for path, d in shell_files.items():
            print(f"{d} {path}")
        print(f"{len(shell_files)} shell files under {LABS}")
        return 0

    if not shell_files:
        print(f"shell labs: 0 files tracked under {LABS}; nothing to check")
        return 0

    labs = {PurePosixPath(p).relative_to(LABS).parts[0] for p in modes if len(PurePosixPath(p).relative_to(LABS).parts) > 1}
    for lab in sorted(labs - SUPPORT_DIRS):
        if not any(p.startswith(f"{LABS}/{lab}/") and d == "bats" for p, d in shell_files.items()):
            problems.append(f"{lab}: no .bats test; every lab proves its claims with one")

    zsh_files = [p for p, d in shell_files.items() if d == "zsh"]
    lint_files = [p for p, d in shell_files.items() if d != "zsh"]
    bats_files = [p for p, d in shell_files.items() if d == "bats"]
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
        report("shellcheck", *run(["shellcheck", "--format=gcc", *lint_files], repo))
    report("shfmt", *run(["shfmt", "-d", *shell_files], repo))
    zsh_out = [(p, *run(["zsh", "-n", p], repo)) for p in zsh_files]
    if zsh_bad := [f"{p}: {out}" for p, rc, out in zsh_out if rc != 0]:
        report("zsh -n", 1, "\n".join(zsh_bad))
    if bats_files:
        report("bats", *run(["bats", *bats_files], repo))

    print(f"result: FAIL ({', '.join(failed)})" if failed else "result: PASS")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
