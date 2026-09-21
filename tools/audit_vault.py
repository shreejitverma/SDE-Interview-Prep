#!/usr/bin/env python3
"""Structural audit of the vault: counts, duplicates, links, orphans, frontmatter,
READMEs, style violations, and stale or vendored content.

Operates on git-tracked files (what the public repo and a fresh clone contain).
Link resolution follows Obsidian rules: wikilinks resolve by vault path or by
unique basename anywhere; Markdown links resolve relative to the note, then to
the vault root. Links inside fenced or inline code are ignored.

Usage:
    python3 tools/audit_vault.py                   # writes AUDIT.md at the repo root
    python3 tools/audit_vault.py --out - --json    # machine-readable summary to stdout
    python3 tools/audit_vault.py --check links     # exit 1 if any broken link (for CI)
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass
from datetime import date
from pathlib import Path, PurePosixPath
from urllib.parse import unquote

from private_paths import is_private, private_link_prefixes

PRIVATE_LINK_PREFIXES = tuple(private_link_prefixes())

CODE_EXTS = {".py", ".cpp", ".cc", ".c", ".h", ".hpp", ".java", ".go", ".rs", ".js", ".ts", ".sql", ".sh", ".kt", ".scala"}
DOC_EXTS = {".md"}
PAPER_EXTS = {".pdf", ".tex"}
README_NAMES = {"readme.md", "_readme.md", "index.md", "_index.md"}
ENTRY_PREFIXES = ("00 home", "00-dashboard", "moc - ")
ARCHIVED_RE = re.compile(r"(^|/)(_archive|_consolidated[^/]*)/")
NON_KNOWLEDGE_RE = re.compile(r"^(tools|\.github)/|/_Templates/|(^|/)SUMMARY\.md$|^[^/]+$")


def is_knowledge_note(path: str) -> bool:
    """Notes that carry the vault frontmatter schema: not archived, templates, generated, or repo metadata."""
    return path.endswith(".md") and not ARCHIVED_RE.search(path) and not NON_KNOWLEDGE_RE.search(path)

FENCE_RE = re.compile(r"^(\s*)(`{3,}|~{3,})")
INLINE_CODE_RE = re.compile(r"`[^`\n]*`")
WIKILINK_RE = re.compile(r"!?\[\[([^\[\]\n]+?)\]\]")
MDLINK_RE = re.compile(r"!?\[(?:[^\[\]\n]|\[[^\[\]\n]*\])*\]\(\s*(<[^>\n]+>|[^()\s]+(?:\([^()\s]*\)[^()\s]*)*)(?:\s+[\"'(][^\n]*?[\"')])?\s*\)")
SCHEME_RE = re.compile(r"^[a-zA-Z][a-zA-Z0-9+.-]*:")
# Emoji characters. The typographic symbols U+2610-2612 (ballot boxes), U+2713/2714 (check marks)
# and U+2717/2718 (crosses) are not emojis and are deliberately excluded. Arrows and other
# symbols count only when followed by the emoji variation selector U+FE0F.
EMOJI_CHAR = (
    "(?:[\U0001F000-\U0001FAFF\u2600-\u260F\u2613-\u2712\u2715\u2716\u2719-\u27BF"
    "\u2B50\u2B55\u2B1B\u2B1C]|[\u2190-\u21FF\u2300-\u23FF\u2B00-\u2BFF](?=\uFE0F))"
)
EMOJI_RE = re.compile(EMOJI_CHAR)
EM_DASH = "\u2014"
TABLE_ALIAS_RE = re.compile(r"\[\[[^\]\n]*[^\\\]]\|[^\]\n]*\]\]")  # [[a|b]] without the escaped \| a table needs

VENDOR_MARKERS = {"license", "license.md", "license.txt", "code_of_conduct.md", "contributing.md",
                  "package.json", "setup.py", "pom.xml", "cargo.toml", "go.mod"}
JUNK_RE = re.compile(r"(^|/)(\.DS_Store|cmake-build-[^/]+/|CMakeFiles/|__pycache__/|\.ipynb_checkpoints/)|\.(o|obj|exe|class|pyc|out)$")
GENERIC_DIR_KEYS = {"note", "code", "src", "intro", "idea", "pattern", "asset", "image", "doc", "test", "example", "archive", "_archive"}
LARGE_FILE_BYTES = 5_000_000
LANG_TOKENS = {"java", "python", "py", "cpp", "c", "go", "golang", "rust", "js", "javascript", "ts", "in", "and", "the", "of"}


@dataclass
class Link:
    src: str
    target: str
    kind: str  # wiki | md


def git(repo: Path, *args: str) -> str:
    return subprocess.run(["git", *args], cwd=repo, check=True, capture_output=True, text=True).stdout


def tracked_files(repo: Path) -> list[str]:
    out = subprocess.run(["git", "ls-files", "-z"], cwd=repo, check=True, capture_output=True).stdout
    return [p for p in out.decode().split("\0") if p and (repo / p).is_file()]


def top(path: str) -> str:
    return path.split("/", 1)[0] if "/" in path else "(root)"


def entry_note(folder: str, names: set[str]) -> str | None:
    """The note that introduces a folder, by vault convention, or None.

    names holds the lower-cased file names directly inside folder.
    """
    for n in ("readme.md", "_readme.md", "index.md", "_index.md", f"{PurePosixPath(folder).name.lower()}.md"):
        if n in names:
            return n
    return next((n for n in sorted(names) if n.endswith(".md") and n.startswith(ENTRY_PREFIXES)), None)


def strip_code(text: str) -> str:
    """Blank out fenced code blocks and inline code so links inside them are ignored."""
    out, fence = [], None
    for line in text.splitlines():
        m = FENCE_RE.match(line)
        if fence:
            if m and m.group(2)[0] == fence[0] and len(m.group(2)) >= len(fence):
                fence = None
            out.append("")
            continue
        if m:
            fence = m.group(2)
            out.append("")
            continue
        out.append(INLINE_CODE_RE.sub("", line))
    return "\n".join(out)


def has_frontmatter(text: str) -> bool:
    if not text.startswith("---\n"):
        return False
    return "\n---\n" in text[4:] or text[4:].startswith("---\n") or text.rstrip().endswith("\n---")


class Resolver:
    def __init__(self, files: list[str]):
        self.files = set(files)
        self.files_lower = {f.lower(): f for f in files}
        self.dirs = {str(PurePosixPath(f).parent) for f in files}
        for f in files:  # every ancestor directory exists too
            p = PurePosixPath(f).parent
            while str(p) not in (".", ""):
                self.dirs.add(str(p))
                p = p.parent
        self.by_name: dict[str, list[str]] = defaultdict(list)
        self.by_stem: dict[str, list[str]] = defaultdict(list)
        for f in files:
            pp = PurePosixPath(f)
            self.by_name[pp.name.lower()].append(f)
            if pp.suffix.lower() == ".md":
                self.by_stem[pp.stem.lower()].append(f)

    def exists(self, path: str) -> str | None:
        norm = str(PurePosixPath(path))
        parts: list[str] = []
        for part in norm.split("/"):  # collapse ".." without touching the filesystem
            if part == "..":
                if not parts:
                    return None
                parts.pop()
            elif part not in (".", ""):
                parts.append(part)
        norm = "/".join(parts)
        if norm in self.files or norm in self.dirs:
            return norm
        return self.files_lower.get(norm.lower())

    def wiki(self, src: str, raw: str) -> str | None | bool:
        """Return the resolved path, None if broken, True if it is a same-note anchor."""
        target = raw.replace("\\|", "|").split("|", 1)[0].split("#", 1)[0].split("^", 1)[0].strip()
        if not target:
            return True
        has_ext = PurePosixPath(target).suffix.lower() in {".md", ".png", ".jpg", ".jpeg", ".gif", ".svg", ".pdf",
                                                            ".webp", ".canvas", ".excalidraw", ".base", ".mp4"}
        if target.lstrip("./").startswith(PRIVATE_LINK_PREFIXES):
            return True  # resolves locally through the private symlinks, never in the public repo
        candidates = [target] if has_ext else [target + ".md", target]
        for c in candidates:
            if "/" in c:
                if hit := self.exists(c):
                    return hit
                if hit := self.exists(str(PurePosixPath(src).parent / c)):
                    return hit
                suffix = "/" + c.lower()
                for f in self.by_name.get(PurePosixPath(c).name.lower(), []):
                    if f.lower().endswith(suffix):
                        return f
            else:
                if c.lower().endswith(".md") and (hits := self.by_stem.get(c.lower()[:-3])):
                    return hits[0]
                if hits := self.by_name.get(c.lower()):
                    return hits[0]
        return None

    def md(self, src: str, raw: str) -> str | None | bool:
        target = raw.strip()
        if target.startswith("<") and target.endswith(">"):
            target = target[1:-1]
        if SCHEME_RE.match(target) or target.startswith("//"):
            return True  # external URL or app link
        target = unquote(target.split("#", 1)[0].split("?", 1)[0])
        if not target:
            return True  # same-note anchor
        if target.lstrip("./").startswith(PRIVATE_LINK_PREFIXES):
            return True
        base = PurePosixPath(src).parent
        roots = [target.lstrip("/")] if target.startswith("/") else [str(base / target), target]
        for r in roots:
            for c in (r, r + ".md"):
                if hit := self.exists(c):
                    return hit
        return None


def extract_links(src: str, text: str) -> list[Link]:
    body = strip_code(text)
    links = [Link(src, m.group(1), "wiki") for m in WIKILINK_RE.finditer(body)]
    links += [Link(src, m.group(1), "md") for m in MDLINK_RE.finditer(body)]
    return links


def norm_dir_key(name: str) -> str:
    tokens = re.findall(r"[a-z0-9]+", name.lower().replace("c++", "cpp"))
    tokens = [t.rstrip("s") for t in tokens if not t.isdigit() and t not in LANG_TOKENS]
    return " ".join(sorted(tokens))


def last_touched(repo: Path) -> dict[str, str]:
    """Most recent commit date per tracked path, in one pass over history."""
    seen: dict[str, str] = {}
    current = ""
    for line in git(repo, "-c", "core.quotePath=false", "log", "--format=@%cs", "--name-only", "--no-renames").splitlines():
        if line.startswith("@"):
            current = line[1:]
        elif line and line not in seen:
            seen[line] = current
    return seen


def fmt_table(headers: list[str], rows: list[list[object]]) -> list[str]:
    out = ["| " + " | ".join(headers) + " |", "| " + " | ".join(":---" if i == 0 else "---:" for i in range(len(headers))) + " |"]
    out += ["| " + " | ".join(str(c) for c in r) + " |" for r in rows]
    return out


def details(summary: str, lines: list[str]) -> list[str]:
    return ["<details>", f"<summary>{summary}</summary>", "", *lines, "", "</details>", ""]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo", type=Path, default=Path(__file__).resolve().parent.parent)
    ap.add_argument("--out", default="AUDIT.md", help="output path relative to repo, or - for none")
    ap.add_argument("--json", action="store_true", help="print a JSON summary to stdout")
    ap.add_argument("--check", choices=["links", "style"], help="exit 1 if this category has any violation")
    args = ap.parse_args()
    repo = args.repo.resolve()

    all_files = tracked_files(repo)
    resolver = Resolver(all_files)  # private notes still resolve links, but are never reported
    files = [f for f in all_files if not is_private(f)]
    private_count = len(all_files) - len(files)
    md_files = [f for f in files if f.lower().endswith(".md")]
    texts = {f: (repo / f).read_text(errors="ignore") for f in md_files}

    # 1. Counts by top-level folder.
    counts: dict[str, Counter] = defaultdict(Counter)
    for f in files:
        ext = PurePosixPath(f).suffix.lower()
        kind = "md" if ext in DOC_EXTS else "code" if ext in CODE_EXTS else "papers" if ext in PAPER_EXTS else "other"
        c = counts[top(f)]
        c["files"] += 1
        c[kind] += 1
        c["bytes"] += (repo / f).stat().st_size

    names_in_dir: dict[str, set[str]] = defaultdict(set)
    for f in files:
        pp = PurePosixPath(f)
        names_in_dir[str(pp.parent)].add(pp.name.lower())
    live_notes_in: dict[str, list[str]] = defaultdict(list)
    for f in md_files:
        if not ARCHIVED_RE.search(f):
            live_notes_in[str(PurePosixPath(f).parent)].append(f)
    dirs_with_notes = {str(p) for f in md_files for p in PurePosixPath(f).parents if str(p) != "."}

    def folder_entry(d: str) -> str | None:
        """Entry note of a folder: a conventional name, else its only note when it has no note sub-folders."""
        e = entry_note(d, names_in_dir[d])
        if e:
            return next((f for f in files if str(PurePosixPath(f).parent) == d and PurePosixPath(f).name.lower() == e), None)
        subdirs = {x for x in dirs_with_notes if str(PurePosixPath(x).parent) == d}
        if len(live_notes_in[d]) == 1 and not subdirs:
            return live_notes_in[d][0]
        return None

    # 2. Links, broken links, inbound counts.
    broken: list[tuple[str, str, str]] = []
    inbound: Counter = Counter()
    total_links = 0
    for f, text in texts.items():
        for link in extract_links(f, text):
            res = resolver.wiki(f, link.target) if link.kind == "wiki" else resolver.md(f, link.target)
            if res is True:
                continue
            total_links += 1
            if res is None:
                broken.append((f, link.kind, link.target))
            elif res != f:
                inbound[folder_entry(res) or res if res in resolver.dirs else res] += 1
    archived = [f for f in md_files if ARCHIVED_RE.search(f)]
    orphans = [f for f in md_files if inbound[f] == 0 and is_knowledge_note(f)]
    fixable = []  # broken wikilinks whose last path segment names exactly one existing note
    for f, kind, t in broken:
        if kind == "wiki":
            name = t.replace("\\|", "|").split("|", 1)[0].split("#", 1)[0].rsplit("/", 1)[-1].strip().lower()
            if len(resolver.by_stem.get(name.removesuffix(".md"), [])) == 1:
                fixable.append((f, t))

    # 3. Frontmatter.
    no_fm = [f for f in md_files if is_knowledge_note(f) and not has_frontmatter(texts[f])]

    # 4. Folders without a README-like entry note (folders that hold notes, depth <= 3).
    note_dirs: set[str] = set()
    for f in md_files:
        parts = PurePosixPath(f).parent.parts
        for d in range(1, min(len(parts), 3) + 1):
            note_dirs.add("/".join(parts[:d]))
    no_readme = sorted(d for d in note_dirs if not ARCHIVED_RE.search(d + "/") and folder_entry(d) is None)

    # 4b. Wikilink aliases that split a table cell: inside a table row the pipe must be escaped.
    table_breaks = []
    for f, text in texts.items():
        for i, line in enumerate(strip_code(text).split("\n"), 1):
            if line.lstrip().startswith("|") and TABLE_ALIAS_RE.search(line):
                table_breaks.append(f"{f}:{i}")

    # 5. Style: emojis and em dashes in notes.
    emoji_files = {f: len(EMOJI_RE.findall(t)) for f, t in texts.items() if EMOJI_RE.search(t)}
    emdash_files = {f: t.count(EM_DASH) for f, t in texts.items() if EM_DASH in t}

    # 6. Duplicates: identical content across folders, and folders whose names normalize alike.
    by_hash: dict[str, list[str]] = defaultdict(list)
    for f in files:
        p = repo / f
        name = p.name.lower()
        if p.stat().st_size < 256 or name in VENDOR_MARKERS or name == "__init__.py":
            continue
        by_hash[hashlib.sha1(p.read_bytes()).hexdigest()].append(f)
    dup_groups = [g for g in by_hash.values() if len(g) > 1]
    dup_top = sorted(((repo / g[0]).stat().st_size * (len(g) - 1), g) for g in dup_groups)[::-1][:15]
    large = sorted(((repo / f).stat().st_size, f) for f in files if (repo / f).stat().st_size >= LARGE_FILE_BYTES)[::-1]
    dup_bytes = sum((repo / g[0]).stat().st_size * (len(g) - 1) for g in dup_groups)
    pair_counts: Counter = Counter()
    for g in dup_groups:
        dirs = sorted({"/".join(PurePosixPath(f).parts[:2]) for f in g})
        for i in range(len(dirs)):
            for j in range(i + 1, len(dirs)):
                pair_counts[(dirs[i], dirs[j])] += 1
    dir_keys: dict[str, list[str]] = defaultdict(list)
    all_dirs = sorted(d for d in resolver.dirs if d.count("/") <= 2 and not d.startswith("."))
    for d in all_dirs:
        key = norm_dir_key(PurePosixPath(d).name)
        if key and key not in GENERIC_DIR_KEYS and not PurePosixPath(d).name.startswith("."):
            dir_keys[key].append(d)
    name_dups = {k: v for k, v in dir_keys.items() if len(v) > 1}

    # Basename overlap between second-level folders catches topical overlap without identical bytes.
    stems: dict[str, set[str]] = defaultdict(set)
    for f in files:
        pp = PurePosixPath(f)
        if len(pp.parts) >= 3 and pp.suffix.lower() in DOC_EXTS | CODE_EXTS:
            stems["/".join(pp.parts[:2])].add(re.sub(r"[^a-z0-9]", "", pp.stem.lower()))
    overlap = []
    keys = [k for k, v in stems.items() if len(v) >= 10]
    for i in range(len(keys)):
        for j in range(i + 1, len(keys)):
            a, b = stems[keys[i]], stems[keys[j]]
            shared = len(a & b)
            if shared >= 10:
                overlap.append((shared / min(len(a), len(b)), shared, keys[i], keys[j]))
    overlap.sort(reverse=True)

    # 7. Stale and vendored content.
    touched = last_touched(repo)
    vendored: dict[str, int] = {}
    for f in files:
        pp = PurePosixPath(f)
        if pp.name.lower() in VENDOR_MARKERS and len(pp.parts) > 1:
            d = str(pp.parent)
            if not any(d.startswith(v + "/") for v in vendored):
                vendored = {v: n for v, n in vendored.items() if not v.startswith(d + "/")}
                vendored[d] = 0
    for f in files:
        for v in vendored:
            if f.startswith(v + "/"):
                vendored[v] += 1
                break
    vendored_last = {v: max((touched.get(f, "") for f in files if f.startswith(v + "/")), default="") for v in vendored}
    top_last = defaultdict(str)
    for f in files:
        top_last[top(f)] = max(top_last[top(f)], touched.get(f, ""))
    junk = [f for f in files if JUNK_RE.search(f)]

    summary = {
        "generated": date.today().isoformat(), "private_files_excluded": private_count,
        "files": len(files), "notes": len(md_files), "links_checked": total_links,
        "archived_notes": len(archived), "broken_links": len(broken), "table_breaking_links": len(table_breaks), "broken_fixable_by_basename": len(fixable), "orphans": len(orphans), "no_frontmatter": len(no_fm),
        "folders_without_readme": len(no_readme), "emoji_files": len(emoji_files),
        "emoji_count": sum(emoji_files.values()), "emdash_files": len(emdash_files),
        "emdash_count": sum(emdash_files.values()), "duplicate_groups": len(dup_groups),
        "duplicate_bytes": dup_bytes, "vendored_dirs": len(vendored), "tracked_junk": len(junk), "large_files": len(large), "large_files_mb": round(sum(b for b, _ in large) / 1e6, 1),
    }

    if args.out != "-":
        write_report(repo / args.out, summary, counts, top_last, broken, orphans, no_fm, no_readme,
                     emoji_files, emdash_files, pair_counts, name_dups, overlap, vendored, vendored_last, junk,
                     fixable, dup_top, large)
    if args.json:
        print(json.dumps(summary, indent=2))
    else:
        for k, v in summary.items():
            print(f"{k}: {v}")

    if args.check == "links":
        for loc in table_breaks:
            print(f"table-breaking wikilink: {loc}")
        return 1 if broken or table_breaks else 0
    if args.check == "style":
        return 1 if emoji_files or emdash_files else 0
    return 0


def write_report(path, s, counts, top_last, broken, orphans, no_fm, no_readme, emoji_files, emdash_files,
                 pair_counts, name_dups, overlap, vendored, vendored_last, junk, fixable, dup_top, large) -> None:
    mb = lambda b: f"{b / 1e6:.1f}"
    L = [
        "# Vault Audit",
        "",
        f"Generated {s['generated']} by `python3 tools/audit_vault.py` over git-tracked files.",
        "Re-run the script after every structural change; this file is its output and should not be hand-edited.",
        f"{s['private_files_excluded']} files in private locations (`tools/private_paths.py`) are excluded; `tools/audit_pii.py` inventories them into a private path.",
        "",
        "## Summary",
        "",
        *fmt_table(["Check", "Result"], [
            ["Tracked files", s["files"]], ["Markdown notes", s["notes"]],
            ["Internal links checked", s["links_checked"]], ["Broken links (links into private locations are not counted)", s["broken_links"]],
            ["Wikilink aliases that split a table cell", s["table_breaking_links"]],
            ["Broken wikilinks fixable by unique basename", s["broken_fixable_by_basename"]],
            ["Orphan knowledge notes (no inbound links)", s["orphans"]],
            ["Archived drafts (`_archive/`, `_consolidated*/`)", s["archived_notes"]], ["Knowledge notes without frontmatter", s["no_frontmatter"]],
            ["Note folders without README (depth <= 3)", s["folders_without_readme"]],
            ["Notes with emojis / total emojis", f"{s['emoji_files']} / {s['emoji_count']}"],
            ["Notes with em dashes / total em dashes", f"{s['emdash_files']} / {s['emdash_count']}"],
            ["Identical-content groups / redundant MB", f"{s['duplicate_groups']} / {mb(s['duplicate_bytes'])}"],
            ["Vendored or imported repos", s["vendored_dirs"]], ["Tracked build junk", s["tracked_junk"]],
            ["Files >= 5 MB / total MB", f"{s['large_files']} / {s['large_files_mb']}"],
        ]),
        "",
        "## 1. File counts by top-level folder",
        "",
        *fmt_table(["Folder", "Files", "Notes", "Code", "Papers", "Other", "MB", "Last touched"],
                   [[f"`{k}`", c["files"], c["md"], c["code"], c["papers"], c["other"], mb(c["bytes"]), top_last[k]]
                    for k, c in sorted(counts.items())]),
        "",
        "## 2. Duplicate and overlapping sections",
        "",
        "### Folders whose names normalize to the same topic",
        "",
    ]
    for k, v in sorted(name_dups.items(), key=lambda kv: -len(kv[1])):
        L.append(f"- **{k}**: " + ", ".join(f"`{d}`" for d in v))
    L += ["", "### Folder pairs sharing the most identical files", ""]
    L += fmt_table(["Folder A", "Folder B", "Identical files"],
                   [[f"`{a}`", f"`{b}`", n] for (a, b), n in pair_counts.most_common(25)])
    L += ["", "### Largest identical-content groups (redundant bytes)", ""]
    L += fmt_table(["First copy", "Copies", "Redundant MB"],
                   [[f"`{g[0]}`", len(g), mb(b)] for b, g in dup_top])
    L += ["", "### Folder pairs with overlapping note or code names (topical overlap)", ""]
    L += fmt_table(["Folder A", "Folder B", "Shared names", "Overlap of smaller"],
                   [[f"`{a}`", f"`{b}`", n, f"{r:.0%}"] for r, n, a, b in overlap[:25]])

    by_top = Counter(top(f) for f, _, _ in broken)
    L += ["", "## 3. Broken links", "", *fmt_table(["Folder", "Broken"], [[f"`{k}`", v] for k, v in by_top.most_common()]), ""]
    L += [f"{len(fixable)} broken wikilinks point at a path that no longer exists but name a note that exists exactly once elsewhere.",
          "These come from folder reorganizations that did not rewrite links and can be fixed mechanically.", ""]
    L += details(f"All {len(broken)} broken links", [f"- `{f}` ({kind}) -> `{t}`" for f, kind, t in sorted(broken)])

    ot = Counter(top(f) for f in orphans)
    L += ["## 4. Orphan notes", "", "Notes that no other note links to. Most become reachable once each folder has a Map of Content.", "",
          *fmt_table(["Folder", "Orphans"], [[f"`{k}`", v] for k, v in ot.most_common()]), ""]
    L += details(f"All {len(orphans)} orphans", [f"- `{f}`" for f in sorted(orphans)])

    ft = Counter(top(f) for f in no_fm)
    L += ["## 5. Notes without frontmatter", "", *fmt_table(["Folder", "Notes"], [[f"`{k}`", v] for k, v in ft.most_common()]), ""]
    L += details(f"All {len(no_fm)} notes", [f"- `{f}`" for f in sorted(no_fm)])

    L += ["## 6. Note folders without a README", "",
          "A folder counts as covered by `README.md`, `_README.md`, `index.md`, a folder note named after it, `00 Home`, `00-Dashboard`, or a `MOC - ` note.", ""]
    L += details(f"All {len(no_readme)} folders", [f"- `{d}`" for d in no_readme])

    L += ["## 7. Emoji and em-dash violations", "",
          *fmt_table(["Note", "Emojis"], [[f"`{f}`", n] for f, n in sorted(emoji_files.items(), key=lambda kv: -kv[1])[:20]]), ""]
    L += details(f"All {len(emoji_files)} notes with emojis", [f"- `{f}`: {n}" for f, n in sorted(emoji_files.items())])
    L += [*fmt_table(["Note", "Em dashes"], [[f"`{f}`", n] for f, n in sorted(emdash_files.items(), key=lambda kv: -kv[1])[:20]]), ""]
    L += details(f"All {len(emdash_files)} notes with em dashes", [f"- `{f}`: {n}" for f, n in sorted(emdash_files.items())])

    L += ["## 8. Stale and scraped content", "", "### Vendored or imported repos",
          "", "Folders carrying their own LICENSE, `.gitignore`, `package.json`, or similar; outermost only.", "",
          *fmt_table(["Folder", "Files", "Last touched"],
                     [[f"`{v}`", n, vendored_last[v]] for v, n in sorted(vendored.items(), key=lambda kv: -kv[1])[:40]]), ""]
    if len(vendored) > 40:
        L += details(f"All {len(vendored)} vendored folders", [f"- `{v}`: {n} files, {vendored_last[v]}" for v, n in sorted(vendored.items())])
    L += ["### Large files", "", "Files of 5 MB or more; candidates for Git LFS, external links, or removal.", "",
          *fmt_table(["File", "MB"], [[f"`{f}`", mb(b)] for b, f in large[:25]]), ""]
    if len(large) > 25:
        L += details(f"All {len(large)} large files", [f"- `{f}`: {mb(b)} MB" for b, f in large])
    jt = Counter(top(f) for f in junk)
    L += ["### Tracked build junk", "", "`.DS_Store`, CMake build trees, object files, and similar that should be gitignored.", "",
          *fmt_table(["Folder", "Files"], [[f"`{k}`", v] for k, v in jt.most_common()]), ""]
    L += details(f"All {len(junk)} junk files", [f"- `{f}`" for f in sorted(junk)])

    path.write_text("\n".join(L) + "\n")


if __name__ == "__main__":
    sys.exit(main())
