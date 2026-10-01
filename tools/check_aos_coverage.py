#!/usr/bin/env python3
"""Check the CS 6210 AOS notes against the coverage matrix.

The matrix `01-CS-Foundations/Operating-Systems/AOS/_coverage.csv` lists every concept of every
lesson and every paper, with the note, heading, lab, and practice file that must cover it.
A row is covered when:
- its note exists, is not `status: seed`, and has a heading whose text matches the row's anchor
  (case, punctuation, and spacing are ignored) with at least MIN_CHARS of written content
  and no leftover seed callout;
- its lab folder (if any) has a README.md and a Makefile;
- its practice file (if any) exists and mentions the row id (for example `L04b-07`);
The status column in the CSV records the last `--sync-status` run; the checks above decide coverage.

Usage:
    python3 tools/check_aos_coverage.py              # strict: exit 1 if any row is not covered
    python3 tools/check_aos_coverage.py --summary    # per-lesson progress, always exit 0
    python3 tools/check_aos_coverage.py --lesson L04b --verbose
    python3 tools/check_aos_coverage.py --write-report   # refresh AOS/00-Coverage.md
    python3 tools/check_aos_coverage.py --sync-status    # set each row's status to done or todo
"""
from __future__ import annotations

import argparse
import csv
import re
import sys
from collections import OrderedDict
from pathlib import Path

AOS = Path("01-CS-Foundations/Operating-Systems/AOS")
HEADING_RE = re.compile(r"^#{1,6}\s+(.+?)\s*#*\s*$", re.MULTILINE)
COMMENT_RE = re.compile(r"<!--.*?-->", re.DOTALL)
SEED_MARK = "[!todo] Seed"
MIN_CHARS = 250


def norm(text: str) -> str:
    return re.sub(r"[^a-z0-9]+", "", text.lower())


def sections(path: Path) -> tuple[dict[str, str], bool]:
    """Map each normalized heading to the text up to the next heading; also report seed status."""
    text = path.read_text(errors="ignore")
    is_seed = re.search(r"^status:\s*seed\s*$", text.split("\n---", 1)[0], re.MULTILINE) is not None
    found = list(HEADING_RE.finditer(text))
    out: dict[str, str] = {}
    for i, m in enumerate(found):
        end = found[i + 1].start() if i + 1 < len(found) else len(text)
        out.setdefault(norm(m.group(1)), text[m.end():end])
    return out, is_seed


def check_row(row: dict, root: Path, cache: dict) -> list[str]:
    problems = []
    note = root / row["note"]
    if not note.is_file():
        problems.append(f"missing note {row['note']}")
    else:
        if note not in cache:
            cache[note] = sections(note)
        secs, is_seed = cache[note]
        body = secs.get(norm(row["anchor"]))
        if is_seed:
            problems.append(f"{row['note']} is still status: seed")
        if body is None:
            problems.append(f"no heading '{row['anchor']}' in {row['note']}")
        elif SEED_MARK in body:
            problems.append(f"'{row['anchor']}' is still a seed")
        elif len(COMMENT_RE.sub("", body).strip()) < MIN_CHARS:
            problems.append(f"'{row['anchor']}' has under {MIN_CHARS} characters")
    if row["lab"]:
        lab = root / row["lab"]
        for name in ("README.md", "Makefile"):
            if not (lab / name).is_file():
                problems.append(f"missing {row['lab']}/{name}")
    if row["practice"]:
        prac = root / row["practice"]
        if not prac.is_file():
            problems.append(f"missing practice {row['practice']}")
        elif row["id"] not in prac.read_text(errors="ignore"):
            problems.append(f"{row['practice']} does not cite {row['id']}")
    return problems


def sync_status(repo: Path, by_lesson) -> int:
    """Rewrite the status column: done when every check passes, todo otherwise."""
    path = repo / AOS / "_coverage.csv"
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        fields, rows = reader.fieldnames, list(reader)
    state = {r["id"]: ("todo" if p else "done") for items in by_lesson.values() for r, p in items}
    changed = 0
    for row in rows:
        new = state.get(row["id"], row["status"])
        changed += new != row["status"]
        row["status"] = new
    with open(path, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fields, lineterminator="\n")
        w.writeheader()
        w.writerows(rows)
    return changed


def run(repo: Path, lesson: str | None = None) -> OrderedDict[str, list[tuple[dict, list[str]]]]:
    root = repo / AOS
    with open(root / "_coverage.csv", newline="") as f:
        rows = list(csv.DictReader(f))
    by_lesson: OrderedDict[str, list] = OrderedDict()
    cache: dict = {}
    for row in rows:
        key = row["id"].rsplit("-", 1)[0] if row["kind"] == "concept" else f"papers {row['lesson']}"
        if lesson and not (row["id"].startswith(lesson) or row["lesson"] == lesson):
            continue
        by_lesson.setdefault(key, []).append((row, check_row(row, root, cache)))
    return by_lesson


def report_lines(by_lesson) -> list[str]:
    lines = ["| Group | Rows | Covered | Missing |", "| --- | ---: | ---: | ---: |"]
    total = covered = 0
    for key, items in by_lesson.items():
        ok = sum(1 for _, p in items if not p)
        total += len(items)
        covered += ok
        lines.append(f"| {key} | {len(items)} | {ok} | {len(items) - ok} |")
    lines.append(f"| **All** | **{total}** | **{covered}** | **{total - covered}** |")
    return lines


def write_report(repo: Path, by_lesson) -> None:
    head = [
        "---", "type: moc", "track: [sde, distinguished]", "level:", "status: draft", "last_reviewed:",
        "sources: []", "tags: [cs6210]", "---", "",
        "# AOS Coverage Report", "",
        "Generated by `python3 tools/check_aos_coverage.py --write-report`; do not edit by hand.",
        "A row is covered when its note is past seed and has the concept heading with written content,",
        "its lab has a README and Makefile, and its practice file cites the row id.", "",
    ]
    (repo / AOS / "00-Coverage.md").write_text("\n".join(head + report_lines(by_lesson)) + "\n")


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--repo", type=Path, default=Path("."))
    ap.add_argument("--lesson", help="only rows whose id or lesson starts with this, e.g. L04b or L07")
    ap.add_argument("--summary", action="store_true", help="print progress and exit 0")
    ap.add_argument("--verbose", action="store_true", help="list every problem")
    ap.add_argument("--write-report", action="store_true", help="write AOS/00-Coverage.md")
    ap.add_argument("--sync-status", action="store_true", help="set the status column from the checks")
    args = ap.parse_args(argv)
    by_lesson = run(args.repo, args.lesson)
    if args.sync_status:
        print(f"status changed on {sync_status(args.repo, by_lesson)} rows")
    if args.write_report:
        write_report(args.repo, by_lesson)
    print("\n".join(report_lines(by_lesson)))
    missing = [(r, p) for items in by_lesson.values() for r, p in items if p]
    if args.verbose:
        for row, problems in missing:
            print(f"{row['id']}: " + "; ".join(problems))
    if missing and not args.summary:
        print(f"{len(missing)} rows not covered; run with --verbose for details", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
