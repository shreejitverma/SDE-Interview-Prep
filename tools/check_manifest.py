#!/usr/bin/env python3
"""Check a book folder's manifest.tsv against the notes it orders.

A book folder (such as 18-Developer-Toolchain) keeps its reading order in `manifest.tsv`,
which a later build reads to compile the notes into a book. The manifest is valid when:

- the header is exactly: order, pillar_label, pillar_title, note_id, kind, path;
- `order` runs 1, 2, ..., N with no gaps or repeats;
- every `path` (relative to the book folder) is a tracked note, listed once;
- every tracked knowledge note in the folder (outside `_archive/`) is listed;
- `note_id` is the note's path without `.md` (unique by construction, stable for the book build);
- `kind` equals the note's frontmatter `type`.

Like every vault tool it reads git-tracked files, so `git add` new notes before checking.

Usage (from the repo root):
    python3 tools/check_manifest.py                          # default book: 18-Developer-Toolchain
    python3 tools/check_manifest.py --book 18-Developer-Toolchain --repo .
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path, PurePosixPath

COLUMNS = ("order", "pillar_label", "pillar_title", "note_id", "kind", "path")
DEFAULT_BOOK = "18-Developer-Toolchain"
TYPE_RE = re.compile(r"\A---\n(?:.*\n)*?type:[ \t]*(\S+)[ \t]*\n(?:.*\n)*?---", re.M)


def tracked_notes(repo: Path, book: str) -> list[str]:
    out = subprocess.run(["git", "ls-files", "-z", "--", f"{book}/"], cwd=repo, check=True, capture_output=True).stdout
    paths = [p for p in out.decode().split("\0") if p.endswith(".md")]
    return sorted(PurePosixPath(p).relative_to(book).as_posix() for p in paths if "/_archive/" not in f"/{p}")


def note_type(text: str) -> str | None:
    m = TYPE_RE.match(text)
    return m.group(1) if m else None


def check(repo: Path, book: str) -> list[str]:
    manifest = repo / book / "manifest.tsv"
    if not manifest.is_file():
        return [f"{book}/manifest.tsv is missing"]
    rows = [line.split("\t") for line in manifest.read_text().splitlines() if line.strip()]
    if not rows or tuple(rows[0]) != COLUMNS:
        return [f"header must be: {chr(9).join(COLUMNS)}"]
    notes = tracked_notes(repo, book)
    known = set(notes)
    problems: list[str] = []
    seen_paths: set[str] = set()
    seen_ids: set[str] = set()
    for n, row in enumerate(rows[1:], start=1):
        where = f"manifest row {n + 1}"
        if len(row) != len(COLUMNS):
            problems.append(f"{where}: expected {len(COLUMNS)} columns, found {len(row)}")
            continue
        order, _label, _title, note_id, kind, path = row
        if order != str(n):
            problems.append(f"{where}: order is {order!r}, expected {n}")
        if path in seen_paths:
            problems.append(f"{where}: {path} is listed more than once")
        seen_paths.add(path)
        if path not in known:
            problems.append(f"{where}: {path} is not a tracked note in {book}/")
            continue
        if note_id != path[:-3]:
            problems.append(f"{where}: note_id {note_id!r} must be the path without .md, {path[:-3]!r}")
        if note_id in seen_ids:
            problems.append(f"{where}: note_id {note_id!r} is not unique")
        seen_ids.add(note_id)
        actual = note_type((repo / book / path).read_text())
        if kind != actual:
            problems.append(f"{where}: kind {kind!r} does not match frontmatter type {actual!r} in {path}")
    for path in notes:
        if path not in seen_paths:
            problems.append(f"{book}/{path} is not in manifest.tsv")
    return problems


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--repo", type=Path, default=Path(__file__).resolve().parent.parent)
    ap.add_argument("--book", default=DEFAULT_BOOK)
    args = ap.parse_args(argv)
    if not (args.repo / args.book).is_dir():
        print(f"manifest: {args.book}/ does not exist; nothing to check")
        return 0
    problems = check(args.repo, args.book)
    for p in problems:
        print(f"FAIL {p}")
    rows = max(0, len((args.repo / args.book / "manifest.tsv").read_text().splitlines()) - 1) if (args.repo / args.book / "manifest.tsv").is_file() else 0
    print(f"manifest: {args.book}: {rows} rows, {len(tracked_notes(args.repo, args.book))} tracked notes")
    print(f"result: {'FAIL' if problems else 'PASS'}")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
