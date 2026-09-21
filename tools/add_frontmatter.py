#!/usr/bin/env python3
"""Give every knowledge note the vault frontmatter schema (tools/vault_schema.py).

- Notes without frontmatter get the full schema, with `type` inferred from the
  note's location and name and `track` from its top-level folder.
- Notes with frontmatter only gain the schema keys they lack; existing keys and
  values are never changed.
- `level` and `last_reviewed` stay blank: they record human judgement.
- `status` defaults to draft (unreviewed against the note standard).

Skipped: archived drafts, private paths, Templater templates, generated files,
and repo metadata (root-level files, tools/, .github/).

Usage:
    python3 tools/add_frontmatter.py            # dry run
    python3 tools/add_frontmatter.py --apply
"""

from __future__ import annotations

import argparse
import re
import sys
from collections import Counter
from pathlib import Path, PurePosixPath

import audit_vault as av
from vault_schema import tracks_for

YAML_LINE_RE = re.compile(r"^(\s*$|[A-Za-z_][\w-]*\s*:|\s+|- |#)")
SCHEMA_KEYS = ("type", "track", "level", "status", "last_reviewed", "sources")
TYPE_RULES: list[tuple[re.Pattern, str]] = [
    (re.compile(r"(^|/)(readme|_readme|index|00 home|00-dashboard)\.md$|/moc - [^/]*$", re.I), "moc"),
    (re.compile(r"^15-Technical-Whitepapers/|/Sources/"), "paper"),
    (re.compile(r"/War Story - |/02-Case-Studies/|case-stud", re.I), "case-study"),
    (re.compile(r"/(Lab|Drill) - |^06-Interview-Prep/|^16-Interview-Command-Center/|^00-Start-Here/", re.I), "playbook"),
    (re.compile(r"design[- ]?patterns|Gold-Standard-Cpp-Patterns", re.I), "pattern"),
    (re.compile(r"/LeetCode/|/Practice-Platforms/", re.I), "problem"),
]


def infer_type(path: str) -> str:
    return next((t for rx, t in TYPE_RULES if rx.search(path)), "concept")


def schema_lines(path: str) -> dict[str, str]:
    return {
        "type": f"type: {infer_type(path)}",
        "track": f"track: [{', '.join(tracks_for(path))}]",
        "level": "level:",
        "status": "status: draft",
        "last_reviewed": "last_reviewed:",
        "sources": "sources: []",
    }


def split_frontmatter(text: str) -> tuple[list[str], str] | None:
    """Return (frontmatter lines, body) when the note opens with a closed --- block."""
    if not text.startswith("---\n"):
        return None
    end = text.find("\n---", 4)
    while end != -1 and text[end + 4:end + 5] not in ("\n", ""):
        end = text.find("\n---", end + 4)
    if end == -1:
        return None
    return text[4:end].split("\n") if end > 4 else [], text[end + 4:]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo", type=Path, default=Path(__file__).resolve().parent.parent)
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()
    repo = args.repo.resolve()

    stats: Counter = Counter()
    types: Counter = Counter()
    for f in av.tracked_files(repo):
        if av.is_private(f) or not av.is_knowledge_note(f):
            continue
        path = repo / f
        text = path.read_text(errors="ignore")
        wanted = schema_lines(f)
        parsed = split_frontmatter(text)
        if parsed is None:
            new = "---\n" + "\n".join(wanted.values()) + "\n---\n\n" + text.lstrip("\n")
            stats["added"] += 1
            types[infer_type(f)] += 1
        else:
            fm, body = parsed
            if not all(YAML_LINE_RE.match(line) for line in fm):  # a leading --- rule, not frontmatter
                stats["skipped_not_yaml"] += 1
                print(f"skip (leading block is not YAML): {f}")
                continue
            present = {m.group(1) for line in fm if (m := re.match(r"^([A-Za-z_][\w-]*)\s*:", line))}
            missing = [wanted[k] for k in SCHEMA_KEYS if k not in present]
            if not missing:
                stats["complete"] += 1
                continue
            new = "---\n" + "\n".join([*fm, *missing]) + "\n---" + body
            stats["extended"] += 1
        if args.apply:
            path.write_text(new)

    for k, v in sorted(stats.items()):
        print(f"{k}: {v}")
    print("types_added: " + ", ".join(f"{t}={n}" for t, n in types.most_common()))
    if not args.apply:
        print("dry run; pass --apply to write")
    return 0


if __name__ == "__main__":
    sys.exit(main())
