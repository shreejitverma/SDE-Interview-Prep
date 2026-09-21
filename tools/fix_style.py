#!/usr/bin/env python3
"""Remove emojis and em dashes from the vault's Markdown, keeping their meaning.

Emojis that carry information become words or typographic symbols:
- LeetCode's premium marker (lock, U+1F512) -> Premium
- question-bank frequency (three, two, or one fire U+1F525) -> High / Med / Low,
  and a trailing star (U+2B50) -> (must-know)
- check marks (U+2705, U+2714 U+FE0F) -> U+2713, and crosses (U+274C,
  U+2716 U+FE0F) -> U+2717, so pros and cons keep their sign
Every other emoji is decoration and is removed together with one adjacent space.

Em dashes become " - " in prose. At the start of a line they become an escaped
"\\-" so an attribution does not turn into a list item, and inside code blocks
they become a plain "-" so alignment is kept.

The typographic symbols U+2610 (ballot box), U+2713/2714 (check marks) and
U+2717/2718 (crosses) are not emojis and are left alone.

Usage:
    python3 tools/fix_style.py            # dry run
    python3 tools/fix_style.py --apply
"""

from __future__ import annotations

import argparse
import re
import sys
from collections import Counter
from pathlib import Path

import audit_vault as av

EMOJI = av.EMOJI_CHAR
RUN = f"(?:{EMOJI}[\\uFE0F\\u200D]*)+"
RUN_RE = re.compile(RUN)
EM = "\u2014"

CELL_MAP = [  # (cell content regex, replacement); applied to whole table cells
    (re.compile(r"^\U0001F525{3}\s*\u2B50\uFE0F?$"), "High (must-know)"),
    (re.compile(r"^\U0001F525{2}\s*\u2B50\uFE0F?$"), "Med (must-know)"),
    (re.compile(r"^\U0001F525\s*\u2B50\uFE0F?$"), "Low (must-know)"),
    (re.compile(r"^\U0001F525{3}$"), "High"),
    (re.compile(r"^\U0001F525{2}$"), "Med"),
    (re.compile(r"^\U0001F525$"), "Low"),
    (re.compile(r"^\U0001F512$"), "Premium"),
]
INLINE_MAP = [
    ('"\U0001F512" means', '"Premium" means'),
    ("\u2705", "\u2713"), ("\u2714\uFE0F", "\u2713"),
    ("\u274C", "\u2717"), ("\u2716\uFE0F", "\u2717"),
]


LEADING_LOCK_RE = re.compile(r"^\U0001F512(?=\s*,)")  # "lock, same as [...]" keeps its premium meaning


def fix_cells(line: str, stats: Counter) -> str:
    """Map whole table cells; rows may omit the leading pipe (e.g. LeetCode indexes)."""
    if line.count("|") < 2:
        return line
    cells = line.split("|")
    for i, cell in enumerate(cells):
        content = cell.strip()
        if LEADING_LOCK_RE.match(content):
            cells[i] = cell.replace(content, LEADING_LOCK_RE.sub("Premium", content, count=1))
            stats["emoji-to-word"] += 1
            continue
        for rx, word in CELL_MAP:
            if rx.match(content):
                cells[i] = cell.replace(content, word)
                stats["emoji-to-word"] += 1
                break
    return "|".join(cells)


def drop_emojis(line: str, stats: Counter) -> str:
    def repl(m: re.Match) -> str:
        stats["emoji-removed"] += 1
        return ""
    # remove each run with one adjacent space: prefer the space after it, else the one before
    line = re.sub(RUN + r" ", repl, line)
    line = re.sub(r" " + RUN, repl, line)
    return RUN_RE.sub(repl, line)


def fix_dashes(line: str, in_code: bool, stats: Counter) -> str:
    n = line.count(EM)
    if not n:
        return line
    stats["em-dash"] += n
    if in_code:
        return line.replace(EM, "-")
    m = re.match(rf"^(\s*(?:>\s*)*){EM}\s*", line)
    if m:  # leading dash: escape it so Markdown does not start a list
        line = m.group(1) + "\\- " + line[m.end():]
    return re.sub(rf"[ \t]*{EM}+[ \t]*", " - ", line)


def fix_text(text: str, stats: Counter) -> str:
    out, fence = [], None
    for line in text.split("\n"):
        m = av.FENCE_RE.match(line)
        in_code = fence is not None
        if fence and m and m.group(2)[0] == fence[0] and len(m.group(2)) >= len(fence):
            fence = None
        elif not fence and m:
            fence = m.group(2)
        for a, b in INLINE_MAP:
            if a in line:
                stats["emoji-to-symbol"] += line.count(a)
                line = line.replace(a, b)
        if not in_code:
            line = fix_cells(line, stats)
        line = drop_emojis(line, stats)
        line = fix_dashes(line, in_code, stats)
        out.append(line)
    return "\n".join(out)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo", type=Path, default=Path(__file__).resolve().parent.parent)
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()
    repo = args.repo.resolve()

    stats: Counter = Counter()
    changed = 0
    for f in av.tracked_files(repo):
        if not f.endswith(".md") or av.is_private(f):
            continue
        path = repo / f
        text = path.read_text(errors="ignore")
        new = fix_text(text, stats)
        if new != text:
            changed += 1
            if args.apply:
                path.write_text(new)
    for k, v in sorted(stats.items()):
        print(f"{k}: {v}")
    print(f"files_changed: {changed}")
    if not args.apply:
        print("dry run; pass --apply to write")
    return 0


if __name__ == "__main__":
    sys.exit(main())
