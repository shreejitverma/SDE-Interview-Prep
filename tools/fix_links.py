#!/usr/bin/env python3
"""Repair broken internal links in the vault.

Uses the same Obsidian-style resolver as tools/audit_vault.py. For every link
that does not resolve, strategies are tried in order:

1. Code in prose: C++ attributes such as [[nodiscard]], array literals such as
   [[1,2]], and code like `Item [&](n)` are wrapped in backticks so Obsidian
   stops treating them as links.
2. Curated renames from tools/link_map.tsv (old note name -> existing note).
3. Unique basename: the target's file name exists exactly once in the vault.
4. Unique normalized name within the same top-level folder (case, separators,
   and "chapter" or number prefixes ignored), for stale tables of contents.
5. With --stubs, missing wikilink targets under the given folder become seed
   notes that list the notes citing them.

Anything else is reported as unresolved for a human decision.

Usage:
    python3 tools/fix_links.py                         # dry run: summary and unresolved list
    python3 tools/fix_links.py --apply                 # rewrite links in place
    python3 tools/fix_links.py --apply --stubs 14-Low-Latency-Systems
"""

from __future__ import annotations

import argparse
import difflib
import os
import re
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass, field
from pathlib import Path, PurePosixPath
from urllib.parse import quote, unquote

import audit_vault as av

LINK_MAP = Path(__file__).with_name("link_map.tsv")
CPP_ATTRIBUTES = {"noreturn", "nodiscard", "no_unique_address", "likely", "unlikely", "maybe_unused",
                  "deprecated", "fallthrough", "carries_dependency", "assume", "attributes", "stdcall"}
LITERAL_RE = re.compile(r"""^[\d\s,."'\[\]-]+$""")
CODEISH_MD_RE = re.compile(r"^(n|auto&&\.\.\.args|[a-z_]\w{0,2})$")
FENCE_RE = av.FENCE_RE
INLINE_CODE_SPLIT = re.compile(r"(`[^`\n]*`)")
ARCHIVED_RE = re.compile(r"(^|/)(_archive|_consolidated[^/]*)/")
FILE_EXT_RE = re.compile(r"\.[A-Za-z0-9]{1,5}")


def file_ext(name: str) -> str:
    """Real file extension, or "" for note titles like "OUCH 4.2 Protocol Specification"."""
    suffix = PurePosixPath(name).suffix
    return suffix.lower() if FILE_EXT_RE.fullmatch(suffix) else ""


@dataclass
class Plan:
    rewrites: Counter = field(default_factory=Counter)
    unresolved: list[tuple[str, str, str]] = field(default_factory=list)
    stubs: dict[str, set[str]] = field(default_factory=lambda: defaultdict(set))  # title -> citing notes
    stub_dirs: dict[str, Counter] = field(default_factory=lambda: defaultdict(Counter))  # title -> candidate paths
    changed_files: set[str] = field(default_factory=set)


def load_map() -> dict[str, str]:
    out = {}
    if LINK_MAP.exists():
        for line in LINK_MAP.read_text().splitlines():
            if line.strip() and not line.startswith("#"):
                old, new = line.split("\t")
                out[old.strip().lower()] = new.strip()
    return out


def norm(stem: str) -> str:
    s = re.sub(r"(?i)chapter", "", stem.lower())
    s = re.sub(r"[^a-z0-9]", "", s)
    return s.lstrip("0123456789")


def split_wiki(inner: str) -> tuple[str, str]:
    """Split [[target#h|alias]] into target and the untouched remainder."""
    m = re.search(r"\\\||[|#^]", inner)
    return (inner[: m.start()], inner[m.start():]) if m else (inner, "")


class Fixer:
    def __init__(self, repo: Path, stub_root: str | None):
        self.repo = repo
        self.files = av.tracked_files(repo)
        self.stub_root = stub_root
        self.link_map = load_map()
        self.reindex()

    def reindex(self) -> None:
        self.resolver = av.Resolver(self.files)
        self.by_norm: dict[tuple[str, str], list[str]] = defaultdict(list)
        self.by_any_stem: dict[str, list[str]] = defaultdict(list)
        for f in self.files:
            self.by_any_stem[PurePosixPath(f).stem.lower()].append(f)
            pp = PurePosixPath(f)
            self.by_norm[(av.top(f), norm(pp.stem) + pp.suffix.lower())].append(f)

    # -- choosing a replacement -------------------------------------------------

    def wiki_target(self, src: str, target: str) -> str | None:
        stem = PurePosixPath(target).name
        stem_md = stem[:-3] if stem.lower().endswith(".md") else stem
        mapped = self.link_map.get(stem_md.lower())
        if mapped:
            return mapped
        live = lambda hs: hs if ARCHIVED_RE.search(target) else [h for h in hs if not ARCHIVED_RE.search(h)]
        hits = live(self.resolver.by_stem.get(stem_md.lower(), []))
        if len(hits) == 1:
            return self.shortest(hits[0])
        hits = live(self.resolver.by_name.get(stem.lower(), []) or self.by_any_stem.get(stem.lower(), []))
        if len(hits) == 1:  # non-note file (code, pdf): Obsidian needs the extension
            return self.shortest(hits[0], keep_ext=True)
        return None

    def shortest(self, path: str, keep_ext: bool = False) -> str:
        pp = PurePosixPath(path)
        stem = pp.stem if pp.suffix.lower() == ".md" and not keep_ext else pp.name
        if pp.suffix.lower() == ".md" and len(self.resolver.by_stem.get(pp.stem.lower(), [])) == 1:
            return stem
        return str(pp.with_suffix("")) if pp.suffix.lower() == ".md" else path

    def md_target(self, src: str, target: str) -> str | None:
        raw = unquote(target.strip("<>")).split("#", 1)[0]
        name = PurePosixPath(raw).name
        if not name:
            return None
        hits = self.resolver.by_name.get(name.lower(), [])
        if len(hits) != 1:
            ext = PurePosixPath(name).suffix.lower() or ".md"
            base = name[: -len(ext)] if name.lower().endswith(ext) else name
            hits = self.by_norm.get((av.top(src), norm(base) + ext), [])
        if not ARCHIVED_RE.search(raw):  # superseded copies are never a repair target
            hits = [h for h in hits if not ARCHIVED_RE.search(h)]
        if len(hits) != 1 or not self.same_place(raw, hits[0]):
            return None
        rel = os.path.relpath(hits[0], PurePosixPath(src).parent)
        if raw.startswith("./") and not rel.startswith("."):
            rel = "./" + rel
        if " " in rel:
            rel = quote(rel, safe="/.-_~")
        anchor = target.strip("<>").split("#", 1)
        return rel + ("#" + anchor[1] if len(anchor) == 2 else "")

    @staticmethod
    def same_place(raw: str, hit: str) -> bool:
        """Guard against same-named files that mean something else.

        mcqs/04-x.md must not be re-pointed at notes/04-x.md. Accept a match only
        when the link named no folder, or when some folder above the match is
        spelled almost like the folder the link named.
        """
        parent = PurePosixPath(raw).parent.name
        if parent in ("", ".", ".."):
            return True
        return any(difflib.SequenceMatcher(None, parent.lower(), a.lower()).ratio() >= 0.8
                   for a in PurePosixPath(hit).parent.parts)

    # -- rewriting --------------------------------------------------------------

    def fix_segment(self, src: str, seg: str, plan: Plan) -> str:
        def wiki(m: re.Match) -> str:
            whole, inner = m.group(0), m.group(1)
            if self.resolver.wiki(src, inner) is not None:
                return whole
            target, rest = split_wiki(inner)
            t = target.strip()
            if t.lower() in CPP_ATTRIBUTES or t.lower().startswith("assume(") or LITERAL_RE.match(t) or t.startswith('"'):
                plan.rewrites["code-in-prose"] += 1
                return f"`{whole}`"
            new = self.wiki_target(src, t)
            if new is None and t.startswith("../"):
                new = self.wiki_target(src, PurePosixPath(t).name)
            if new is None:
                if self.stub_root and src.startswith(self.stub_root + "/"):
                    title = PurePosixPath(t).name
                    plan.stubs[title.lower()].add(src)
                    plan.stub_dirs[title.lower()][self.stub_path(src, t)] += 1
                    plan.rewrites["stub"] += 1
                    return f"{whole[:len(whole) - len(inner) - 2]}{PurePosixPath(t).name}{rest}]]"
                plan.unresolved.append((src, "wiki", inner))
                return whole
            if file_ext(new) not in ("", ".md") and not whole.startswith("!"):
                label = rest.split("|", 1)[1] if "|" in rest else PurePosixPath(new).name
                rel = os.path.relpath(self.resolver.wiki(src, new) or new, PurePosixPath(src).parent)
                plan.rewrites["code-file-link"] += 1
                return f"[{label}]({quote(rel, safe='/.-_~')})"
            plan.rewrites["wiki-renamed"] += 1
            return f"{whole[:len(whole) - len(inner) - 2]}{new}{rest}]]"

        def md(m: re.Match) -> str:
            whole, target = m.group(0), m.group(1)
            if self.resolver.md(src, target) is not None:
                return whole
            if CODEISH_MD_RE.match(target):
                plan.rewrites["code-in-prose"] += 1
                return f"`{whole}`"
            if re.match(r"^[a-z0-9-]+\.(com|org|io|net|dev)/", target):
                plan.rewrites["md-missing-scheme"] += 1
                return whole.replace(f"({target}", f"(https://{target}", 1)
            new = self.md_target(src, target)
            if new is None:
                plan.unresolved.append((src, "md", target))
                return whole
            plan.rewrites["md-repathed"] += 1
            return whole.replace(f"({target}", f"({new}", 1)

        seg = av.WIKILINK_RE.sub(wiki, seg)
        return av.MDLINK_RE.sub(md, seg)

    def fix_text(self, src: str, text: str, plan: Plan) -> str:
        out, fence = [], None
        for line in text.split("\n"):
            m = FENCE_RE.match(line)
            if fence:
                if m and m.group(2)[0] == fence[0] and len(m.group(2)) >= len(fence):
                    fence = None
                out.append(line)
                continue
            if m:
                fence = m.group(2)
                out.append(line)
                continue
            parts = INLINE_CODE_SPLIT.split(line)
            out.append("".join(p if p.startswith("`") else self.fix_segment(src, p, plan) for p in parts))
        return "\n".join(out)

    # -- stubs --------------------------------------------------------------------

    def stub_path(self, src: str, target: str) -> str:
        t = PurePosixPath(target)
        root = PurePosixPath(self.stub_root)
        if t.parts[0] == "Sources":
            return str(root / "Sources" / f"{t.name}.md")
        if len(t.parts) > 1 and (root / t.parent).as_posix() in self.resolver.dirs:
            return str(root / t.parent / f"{t.name}.md")
        if t.parts[0] == "Drills":
            drills = [f for f in self.files if f.startswith(str(root)) and PurePosixPath(f).name.startswith("Drill - ")]
            if drills:
                return str(PurePosixPath(drills[0]).parent / f"{t.name}.md")
        return str(PurePosixPath(src).parent / f"{t.name}.md")

    def write_stubs(self, plan: Plan) -> None:
        for key, citers in sorted(plan.stubs.items()):
            path = plan.stub_dirs[key].most_common(1)[0][0]
            p = self.repo / path
            if p.exists():
                continue
            is_source = "/Sources/" in path
            title = PurePosixPath(path).stem
            lines = [
                "---",
                f"type: {'paper' if is_source else 'concept'}",
                "track: [low-latency]",
                "level:",
                "status: seed",
                "last_reviewed:",
                "sources: []",
                "---",
                "",
                f"# {title}",
                "",
                "> [!todo] Seed note",
                "> Other notes link here, but this note has not been written yet.",
                "> " + ("Summarize the source: what it specifies or argues, the key numbers, and why it matters for low-latency systems."
                        if is_source else "Write it to the vault note standard: TL;DR, core concepts, worked example, pitfalls, interview questions, further reading."),
                "",
                "## Cited by",
                "",
                *[f"- [[{PurePosixPath(c).stem}]]" for c in sorted(citers)],
                "",
            ]
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text("\n".join(lines))
            self.files.append(path)
        self.reindex()


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo", type=Path, default=Path(__file__).resolve().parent.parent)
    ap.add_argument("--apply", action="store_true", help="write changes (default is a dry run)")
    ap.add_argument("--stubs", metavar="FOLDER", help="create seed notes for missing wikilink targets cited from FOLDER")
    args = ap.parse_args()

    fixer = Fixer(args.repo.resolve(), args.stubs)
    notes = [f for f in fixer.files if f.lower().endswith(".md") and not av.is_private(f)]

    if args.stubs:  # first pass only discovers stubs so the rewrite pass can link to them by name
        discover = Plan()
        for f in notes:
            fixer.fix_text(f, (fixer.repo / f).read_text(errors="ignore"), discover)
        if args.apply:
            fixer.write_stubs(discover)
        print(f"stubs: {len(discover.stubs)}")

    plan = Plan()
    for f in notes:
        path = fixer.repo / f
        text = path.read_text(errors="ignore")
        new = fixer.fix_text(f, text, plan)
        if new != text:
            plan.changed_files.add(f)
            if args.apply:
                path.write_text(new)

    for k, v in sorted(plan.rewrites.items()):
        print(f"{k}: {v}")
    print(f"files_changed: {len(plan.changed_files)}")
    print(f"unresolved: {len(plan.unresolved)}")
    for src, kind, target in plan.unresolved:
        print(f"  {src} ({kind}) -> {target}")
    if not args.apply:
        print("dry run; pass --apply to write")
    return 0


if __name__ == "__main__":
    sys.exit(main())
