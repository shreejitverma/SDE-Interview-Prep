#!/usr/bin/env python3
# /// script
# requires-python = ">=3.11"
# dependencies = ["anthropic>=1.17"]
# ///
"""Draft, fact-check, and flashcard the Cloud AI curriculum through the Message Batches API.

The chapter list comes from `17-Cloud-AI-Engineering/SUMMARY.md`.
Each phase is one batch at 50% of standard API prices:

    draft   Opus 5.5 with web search writes every chapter that is missing or a `seed`.
    verify  Sonnet 5.5 with web search fact-checks each pipeline draft and fixes errors in place.
    cards   Haiku 5.5 turns each finished chapter into Anki flashcards (TSV per volume).

Every command is a dry run unless you pass `--apply`.
`submit` prints a cost estimate and refuses to exceed `--budget` (spent + in flight + this batch).
Run state, raw results, and reports live in `build/cloud_batch/` (gitignored).
The pipeline only writes a chapter it owns: a missing or `seed` file for `draft`, and for
`verify` only a file whose content is unchanged since the pipeline wrote it.

The API key is read from ANTHROPIC_API_KEY, else from `~/.config/sde-vault/anthropic.env`
(KEY=value lines), else from an `ant auth login` profile; keep it out of this public repo.
A key that is not scoped to a workspace also needs ANTHROPIC_WORKSPACE_ID (same file).

Usage (from the repo root; `uv run` installs the SDK for this script only):
    python3 tools/cloud_batch.py plan                          # chapters per phase + cost estimate
    uv run tools/cloud_batch.py submit --phase draft --limit 3 --apply    # pilot
    uv run tools/cloud_batch.py status
    uv run tools/cloud_batch.py collect --apply                # write notes, reports, spend ledger
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from dataclasses import dataclass
from datetime import date, datetime, timezone
from pathlib import Path

CURRICULUM = "17-Cloud-AI-Engineering"
STATE_DIR = Path("build/cloud_batch")
ENV_FILE = Path.home() / ".config/sde-vault/anthropic.env"

# Existing vault folders a chapter may link into (by vault path, without ".md").
RELATED_FOLDERS = (
    "04-System-Design/05-Data-Stores",
    "04-System-Design/06-Messaging-and-Streaming",
    "04-System-Design/07-Infrastructure-and-Orchestration",
    "08-Distinguished-Engineering/02-Distributed-Systems-Internals",
    "13-Agentic-AI/Agentic_AI_Zero_to_Godhood/Volume_10_Evaluation_and_Observability",
    "13-Agentic-AI/Agentic_AI_Zero_to_Godhood/Volume_12_Production_Engineering",
    "15-Technical-Whitepapers",
)

# USD per million tokens (input, output), Anthropic list prices as of 2026-10.
PRICES = {
    "claude-opus-5-5": (4.00, 20.00),
    "claude-sonnet-5-5": (2.00, 10.00),
    "claude-haiku-5-5": (0.10, 0.50),
}
BATCH_DISCOUNT = 0.5
CACHE_WRITE, CACHE_READ = 1.25, 0.10
WEB_SEARCH_USD = 10.00 / 1000

# The ten level-2 headings of a chapter, matching the curriculum's hand-written chapters.
# A heading counts as present when it starts with the text before any ":" (e.g. "Worked Example: ...").
SECTIONS = (
    "TL;DR", "Mental Model", "Internals", "Numbers to Know", "AWS vs GCP Mechanism Parity",
    "Worked Example", "Failure Modes and Pitfalls", "Security and Cost Angles",
    "Interview Questions and Model Answers", "Further Reading",
)


@dataclass(frozen=True)
class PhaseConfig:
    model: str
    effort: str
    max_tokens: int
    web_searches: int  # max_uses; 0 disables the tool
    # Heuristic per-request token ranges for the dry-run estimate: (low, high).
    extra_input: tuple[int, int]  # search results and other input beyond the prompt
    output: tuple[int, int]  # visible output plus thinking
    searches: tuple[int, int]


PHASES = {
    "draft": PhaseConfig("claude-opus-5-5", "high", 64000, 8, (20_000, 60_000), (12_000, 30_000), (3, 8)),
    "verify": PhaseConfig("claude-sonnet-5-5", "high", 64000, 10, (20_000, 60_000), (14_000, 30_000), (4, 10)),
    "cards": PhaseConfig("claude-haiku-5-5", "medium", 16000, 0, (0, 0), (2_000, 6_000), (0, 0)),
}


# ---------------------------------------------------------------- curriculum


@dataclass(frozen=True)
class Chapter:
    cid: str  # batch custom_id stem, e.g. "v02c08" or "appA"
    path: str  # vault path, e.g. "17-Cloud-AI-Engineering/Volume_02_.../Chapter_08_....md"
    title: str
    scope: str
    volume: str  # volume or "Appendices" title
    volume_dir: str

    @property
    def link(self) -> str:
        return self.path.removesuffix(".md")

    @property
    def is_appendix(self) -> bool:
        return self.volume_dir == "Appendices"


VOLUME_RE = re.compile(r"^###\s+\[(?P<title>[^\]]+)\]\(\./(?P<dir>[^)/]+)/?\)")
CHAPTER_RE = re.compile(r"^\s*(?:\d+\.|-)\s+\[(?P<title>[^\]]+)\]\(\./(?P<path>[^)]+\.md)\)\s*(?:-\s*(?P<scope>.*))?$")


def parse_summary(text: str) -> list[Chapter]:
    chapters: list[Chapter] = []
    volume, volume_dir = "", ""
    for line in text.splitlines():
        if m := VOLUME_RE.match(line):
            volume, volume_dir = m["title"], m["dir"]
            continue
        if not (m := CHAPTER_RE.match(line)):
            continue
        rel = m["path"]
        top = rel.split("/", 1)[0]
        if top == "Appendices":
            letter = re.search(r"Appendix_([A-Z])", rel)
            cid = f"app{letter[1] if letter else len(chapters)}"
            vol_title, vol_dir = "Appendices", "Appendices"
        else:
            vnum = re.search(r"Volume_(\d+)", rel)
            cnum = re.search(r"Chapter_(\d+)", rel)
            cid = f"v{vnum[1] if vnum else 'xx'}c{cnum[1] if cnum else len(chapters)}"
            vol_title, vol_dir = volume, volume_dir or top
        chapters.append(Chapter(cid, f"{CURRICULUM}/{rel}", m["title"].strip(), (m["scope"] or "").strip(), vol_title, vol_dir))
    ids = [c.cid for c in chapters]
    dupes = {i for i in ids if ids.count(i) > 1}
    if dupes:
        raise ValueError(f"duplicate chapter ids in SUMMARY.md: {sorted(dupes)}")
    return chapters


FRONTMATTER_RE = re.compile(r"\A---\n(.*?)\n---\n", re.S)


def note_status(path: Path) -> str:
    """'missing', or the frontmatter status ('' when absent)."""
    if not path.exists():
        return "missing"
    m = FRONTMATTER_RE.match(path.read_text(errors="ignore"))
    if not m:
        return ""
    s = re.search(r"^status:\s*(\S*)", m[1], re.M)
    return s[1] if s else ""


def note_type(ch: Chapter) -> str:
    if "Volume_13" in ch.volume_dir:
        return "paper"
    if "Volume_14" in ch.volume_dir:
        return "playbook"
    if "Volume_15" in ch.volume_dir:
        return "case-study"
    return "concept"


def note_tracks(ch: Chapter) -> list[str]:
    tracks = ["cloud-ai"]
    if any(f"Volume_{n}" in ch.volume_dir for n in ("10", "11", "12")):
        tracks.append("ai-eng")
    if any(f"Volume_{n}" in ch.volume_dir for n in ("01", "13", "14")):
        tracks.append("distinguished")
    return tracks


def render_note(ch: Chapter, status: str, body: str, sources: list[str]) -> str:
    src = "[]" if not sources else "\n" + "\n".join(f"  - {json.dumps(s)}" for s in sources)
    return (
        f"---\ntype: {note_type(ch)}\ntrack: [{', '.join(note_tracks(ch))}]\nlevel:\n"
        f"status: {status}\nlast_reviewed:\nsources: {src}\n---\n\n{body.strip()}\n"
    )


# ---------------------------------------------------------------- prompts

DRAFT_ROLE = """You are a principal cloud and AI-infrastructure engineer who has run production platforms on AWS and Google Cloud at hyperscale and trained and served large models on GPU and TPU fleets.
You write like the AWS Builders' Library and the Google SRE book: precise, mechanism-first, with numbers and failure modes.
You are writing one chapter of "Cloud AI Engineering: Zero to Godhood", a public study vault whose goal is to take a strong software engineer to the top-1% Cloud AI Engineer bar."""

OUTPUT_CONTRACT = """# Output contract

1. Research first. Use web search to verify every service name, limit, quota, price, instance type, accelerator generation, GA/preview status, and paper citation you intend to state. Prefer official AWS and Google Cloud docs, pricing pages, launch posts, the AWS Builders' Library, Google Research, and the papers themselves.
2. Write no prose before your research is done. Your final answer must start with the line `# <exact chapter title>` and contain only the chapter in Markdown: no frontmatter, no preamble, no closing remarks.
3. Use exactly these level-2 headings, in this order: {sections}. `## Worked Example` may carry a subtitle after a colon. Use `###` subsections freely. Under the interview heading write at least six questions as `### Question N (L5 - Senior)`, `(L6 - Staff)`, or `(L7+ - Principal)`, each followed by `#### Model Answer` and `#### Follow-up Interviewer Question`.
4. One full sentence per physical line. No emojis. No em dashes; use a plain "-". Keep the chapter ASCII where practical.
5. Tag every volatile fact with its source and date, e.g. `(AWS docs, as of {month})`. If you cannot verify a claim, drop it or mark it `UNVERIFIED` inline.
6. Links: you may link only to (a) other chapters of this curriculum and (b) the existing vault notes listed below, always as wikilinks by vault path without ".md", e.g. `[[{example}|Alias]]`. Inside a Markdown table, escape the pipe: `[[path\\|Alias]]`. External sources are plain Markdown links to URLs. Any other internal link will be deleted.
7. After the chapter, on its own line, write `<!-- UNVERIFIED -->` followed by a bullet list of claims you could not verify, or `- none`.
"""

APPENDIX_CONTRACT = """# Output contract

1. Research first with web search to verify every volatile fact you include.
2. Write no prose before your research is done. Your final answer must start with the line `# <exact appendix title>` and contain only the appendix in Markdown: no frontmatter, no preamble.
3. This is a reference appendix: choose the structure that makes it most useful (tables, grouped lists, week-by-week plans). The ten-section chapter template does not apply.
4. One full sentence per physical line. No emojis. No em dashes; use a plain "-".
5. Tag every volatile fact with its source and date, e.g. `(AWS docs, as of {month})`.
6. Links: only wikilinks by vault path to curriculum chapters and the listed vault notes, e.g. `[[{example}|Alias]]`; inside tables escape the pipe as `[[path\\|Alias]]`. External sources are plain Markdown links to URLs.
7. After the appendix, on its own line, write `<!-- UNVERIFIED -->` followed by a bullet list of claims you could not verify, or `- none`.
"""

VERIFY_SYSTEM = """You are a meticulous fact-checker for a public cloud and AI-infrastructure study vault, with deep AWS and Google Cloud expertise.
You receive one chapter. Verify every volatile or checkable claim with web search: service and product names, limits, quotas, prices, instance and accelerator specifications, GA/preview/deprecated status, dates, paper titles, authors, venues, and numbers attributed to papers.

Rules:
- Fix errors in place with the smallest possible edit and update the source tag to `(source, as of {month})`.
- Do not shorten, restructure, restyle, or summarize the chapter. Keep every section, sentence, table, and code block that is correct exactly as written.
- Keep one sentence per line, no emojis, no em dashes.
- If a claim cannot be verified and is not clearly correct, mark it `UNVERIFIED` inline rather than deleting it.

Output, with no preamble:
1. The full corrected chapter, starting with its `# ` title line.
2. A line `<!-- VERIFICATION REPORT -->`.
3. Bullets: `- CORRECTED: <old> -> <new> (<source URL>)` and `- UNVERIFIED: <claim>`; or `- no changes`.
4. A final line `UNRESOLVED: <number of claims still marked UNVERIFIED>`.
"""

CARDS_SYSTEM = """You turn one chapter of a cloud and AI-infrastructure study vault into Anki flashcards for a senior engineer preparing for staff-plus interviews.
Write 15 to 30 cards covering mechanisms (how it works underneath), the numbers to know (keep each number's as-of date), trade-offs and failure modes, AWS vs GCP differences, and the interview angles.
Each card tests one idea; the answer is at most three sentences.
Output only tab-separated lines `front<TAB>back`: no header, no numbering, no Markdown, no tabs or newlines inside a field.
"""


def related_notes(repo: Path) -> list[str]:
    try:
        out = subprocess.run(["git", "-C", str(repo), "ls-files", "--", *RELATED_FOLDERS],
                             capture_output=True, text=True, check=True).stdout
    except (OSError, subprocess.CalledProcessError):
        return []
    return sorted(p.removesuffix(".md") for p in out.splitlines()
                  if p.endswith(".md") and "/_archive/" not in p)


def draft_system(repo: Path, chapters: list[Chapter], appendix: bool) -> list[dict]:
    root = repo / CURRICULUM
    style = (root / "STYLE.md").read_text()
    summary = (root / "SUMMARY.md").read_text()
    month = date.today().strftime("%Y-%m")
    example = chapters[0].link if chapters else f"{CURRICULUM}/SUMMARY"
    contract = (APPENDIX_CONTRACT if appendix else OUTPUT_CONTRACT).format(
        sections=", ".join(f"`## {s}`" for s in SECTIONS), month=month, example=example)
    chapter_paths = "\n".join(f"- {c.link} - {c.title}" for c in chapters)
    vault = "\n".join(f"- {p}" for p in related_notes(repo)) or "- (none)"
    return [
        {"type": "text", "text": DRAFT_ROLE},
        {"type": "text", "text": (
            f"# Style guide (STYLE.md)\n\n{style}\n\n{contract}\n"
            f"# Curriculum table of contents (SUMMARY.md)\n\n{summary}\n\n"
            f"# Curriculum chapter link targets\n\n{chapter_paths}\n\n"
            f"# Existing vault notes you may link to\n\n{vault}\n"),
         "cache_control": {"type": "ephemeral"}},
    ]


def draft_user(ch: Chapter, chapters: list[Chapter]) -> str:
    same = [c for c in chapters if c.volume_dir == ch.volume_dir]
    i = same.index(ch)
    prev = same[i - 1].title if i > 0 else "(first in volume)"
    nxt = same[i + 1].title if i + 1 < len(same) else "(last in volume)"
    return (
        f"Write this {'appendix' if ch.is_appendix else 'chapter'} now.\n\n"
        f"Title: {ch.title}\nVolume: {ch.volume}\nScope: {ch.scope}\n"
        f"Previous chapter: {prev}\nNext chapter: {nxt}\nToday: {date.today().isoformat()}\n\n"
        "Go deeper than the product docs: explain at least one mechanism a reader will not find in a getting-started guide, "
        "and make the interview answers pass an L6 bar raiser."
    )


# ---------------------------------------------------------------- requests


def tools_for(cfg: PhaseConfig) -> list[dict]:
    if not cfg.web_searches:
        return []
    return [{"type": "web_search_20260209", "name": "web_search", "max_uses": cfg.web_searches}]


def build_params(phase: str, system: list[dict] | str, messages: list[dict], model: str = "") -> dict:
    cfg = PHASES[phase]
    params = {
        "model": model or cfg.model,
        "max_tokens": cfg.max_tokens,
        "system": system,
        "messages": messages,
        "output_config": {"effort": cfg.effort},
    }
    if tools := tools_for(cfg):
        params["tools"] = tools
    return params


def strip_frontmatter(text: str) -> str:
    return FRONTMATTER_RE.sub("", text, count=1).lstrip()


def build_requests(phase: str, repo: Path, chapters: list[Chapter], todo: list[Chapter], model: str = "") -> list[dict]:
    reqs = []
    month = date.today().strftime("%Y-%m")
    if phase == "draft":
        systems = {flag: draft_system(repo, chapters, flag) for flag in (False, True)}
        for ch in todo:
            reqs.append({"custom_id": f"draft-{ch.cid}", "params": build_params(
                phase, systems[ch.is_appendix], [{"role": "user", "content": draft_user(ch, chapters)}], model)})
    elif phase == "verify":
        system = VERIFY_SYSTEM.format(month=month)
        for ch in todo:
            body = strip_frontmatter((repo / ch.path).read_text())
            reqs.append({"custom_id": f"verify-{ch.cid}", "params": build_params(
                phase, system, [{"role": "user", "content": f"Fact-check this chapter.\n\n{body}"}], model)})
    elif phase == "cards":
        for ch in todo:
            body = strip_frontmatter((repo / ch.path).read_text())
            reqs.append({"custom_id": f"cards-{ch.cid}", "params": build_params(
                phase, CARDS_SYSTEM, [{"role": "user", "content": body}], model)})
    else:
        raise ValueError(f"unknown phase {phase}")
    return reqs


# ---------------------------------------------------------------- results


def response_text(message: dict) -> str:
    # Separate blocks: a short progress note between tool calls must not glue onto the "# Title" line.
    return "\n\n".join(b.get("text", "") for b in message.get("content", []) if b.get("type") == "text")


def cited_urls(message: dict) -> list[str]:
    urls: list[str] = []
    for b in message.get("content", []):
        for c in (b.get("citations") or []) if b.get("type") == "text" else []:
            if (u := c.get("url")) and u not in urls:
                urls.append(u)
    return urls


URL_RE = re.compile(r"\]\((https?://[^)\s]+)\)")


def further_reading_urls(body: str) -> list[str]:
    """URLs linked under the Further Reading heading (web search with dynamic filtering returns no citations)."""
    m = re.search(r"^## Further Reading\s*$(.*?)(?=^## |\Z)", body, re.M | re.S)
    return list(dict.fromkeys(URL_RE.findall(m[1]))) if m else []


def extract_note(text: str, title: str) -> tuple[str, str]:
    """Return (body starting at the '# ' title line, trailing marker section)."""
    lines = text.splitlines()
    start = next((i for i, ln in enumerate(lines) if ln.strip() == f"# {title}"), None)
    if start is None:  # first level-1 heading outside code fences
        fenced = False
        for i, ln in enumerate(lines):
            if ln.lstrip().startswith(("```", "~~~")):
                fenced = not fenced
            elif not fenced and ln.startswith("# "):
                start = i
                break
    if start is None:
        return "", text
    body = "\n".join(lines[start:])
    for marker in ("<!-- UNVERIFIED -->", "<!-- VERIFICATION REPORT -->"):
        if marker in body:
            body, tail = body.split(marker, 1)
            return body.strip() + "\n", marker + tail
    return body.strip() + "\n", ""


def missing_sections(body: str) -> list[str]:
    heads = [ln[3:].strip().lower() for ln in body.splitlines() if ln.startswith("## ")]
    return [s for s in SECTIONS if not any(h.lstrip("0123456789. ").startswith(s.lower()) for h in heads)]


WIKILINK_RE = re.compile(r"\[\[([^\[\]\n]+?)\]\]")


def sanitize_links(body: str, allowed: set[str]) -> tuple[str, list[str]]:
    """Unlink wikilinks whose target is not an allowed vault path; return the dropped targets."""
    dropped: list[str] = []

    def fix(m: re.Match) -> str:
        target, _, alias = m[1].partition("|")
        target = target.removesuffix("\\").split("#", 1)[0].strip()
        if target in allowed:
            return m[0]
        dropped.append(target)
        return (alias or target.rsplit("/", 1)[-1]).strip()

    return WIKILINK_RE.sub(fix, body), dropped


def clean_style(body: str) -> str:
    return body.replace(" \u2014 ", " - ").replace("\u2014", " - ")


def usage_cost(model: str, usage: dict) -> float:
    pin, pout = PRICES[model]
    m = 1e-6 * BATCH_DISCOUNT
    cost = (usage.get("input_tokens", 0) * pin
            + usage.get("cache_creation_input_tokens", 0) * pin * CACHE_WRITE
            + usage.get("cache_read_input_tokens", 0) * pin * CACHE_READ
            + usage.get("output_tokens", 0) * pout) * m
    searches = (usage.get("server_tool_use") or {}).get("web_search_requests", 0) or 0
    return cost + searches * WEB_SEARCH_USD


def estimate(phase: str, reqs: list[dict], state: dict | None = None) -> tuple[float, float]:
    """Cost range for reqs: measured per-request average (+-25%) once a batch of this phase is collected."""
    model = reqs[0]["params"]["model"] if reqs else PHASES[phase].model
    measured = [b for b in (state or {}).get("batches", [])
                if b["phase"] == phase and b.get("model", PHASES[phase].model) == model and b.get("collected") and b.get("actual_usd")]
    if measured:
        avg = sum(b["actual_usd"] for b in measured) / sum(len(b["custom_ids"]) for b in measured)
        return 0.75 * avg * len(reqs), 1.25 * avg * len(reqs)
    cfg = PHASES[phase]
    pin, pout = PRICES[model]
    lo = hi = 0.0
    for r in reqs:
        prompt = len(json.dumps(r["params"]["system"])) // 4 + len(json.dumps(r["params"]["messages"])) // 4
        for bound, cached in ((0, True), (1, False)):
            inp = prompt + cfg.extra_input[bound]
            rate = pin * (CACHE_READ if cached and phase == "draft" else 1)
            usd = (inp * rate + cfg.output[bound] * pout) * 1e-6 * BATCH_DISCOUNT + cfg.searches[bound] * WEB_SEARCH_USD
            if bound == 0:
                lo += usd
            else:
                hi += usd
    return lo, hi


# ---------------------------------------------------------------- state


def load_state(repo: Path) -> dict:
    p = repo / STATE_DIR / "state.json"
    return json.loads(p.read_text()) if p.exists() else {"batches": [], "spent_usd": 0.0, "owned": {}, "done": {}}


def save_state(repo: Path, state: dict) -> None:
    p = repo / STATE_DIR / "state.json"
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(json.dumps(state, indent=2, sort_keys=True) + "\n")


def sha(text: str) -> str:
    return hashlib.sha256(text.encode()).hexdigest()


def in_flight(state: dict) -> set[str]:
    return {cid for b in state["batches"] if not b.get("collected") for cid in b["custom_ids"]}


def select(phase: str, repo: Path, chapters: list[Chapter], state: dict) -> list[Chapter]:
    busy = in_flight(state)
    done = set(state["done"].get(phase, []))
    verified = set(state["done"].get("verify", []))
    out = []
    for ch in chapters:
        if f"{phase}-{ch.cid}" in busy or ch.path in done:
            continue
        status = note_status(repo / ch.path)
        if phase == "draft" and status in ("missing", "seed"):
            out.append(ch)
        elif phase == "verify" and status == "draft" and owned_unchanged(repo, ch, state):
            out.append(ch)
        elif phase == "cards" and (
                (status in ("draft", "solid", "canonical") and ch.path in verified)
                or (status in ("solid", "canonical") and ch.path not in state["owned"])):
            out.append(ch)
    return out


def mark_done(state: dict, phase: str, path: str) -> None:
    done = state["done"].setdefault(phase, [])
    if path not in done:
        done.append(path)


def owned_unchanged(repo: Path, ch: Chapter, state: dict) -> bool:
    p = repo / ch.path
    return p.exists() and state["owned"].get(ch.path) == sha(p.read_text())


# ---------------------------------------------------------------- API


def client():
    try:
        import anthropic
    except ImportError:
        sys.exit("anthropic SDK not installed: run this command with `uv run tools/cloud_batch.py ...`")
    if ENV_FILE.exists():
        for line in ENV_FILE.read_text().splitlines():
            k, sep, v = line.strip().partition("=")
            if sep and k.strip() in ("ANTHROPIC_API_KEY", "ANTHROPIC_WORKSPACE_ID"):
                os.environ.setdefault(k.strip(), v.strip().strip("'\""))
    # A key that is not scoped to a workspace must name one on every request.
    workspace = os.environ.get("ANTHROPIC_WORKSPACE_ID")
    return anthropic.Anthropic(default_headers={"anthropic-workspace-id": workspace} if workspace else None)


def to_dict(obj) -> dict:
    return obj.model_dump(mode="json") if hasattr(obj, "model_dump") else obj


# ---------------------------------------------------------------- commands


def matches(ch: Chapter, only: str) -> bool:
    return any(part.strip() and part.strip() in ch.path for part in only.split(","))


def cmd_plan(args, repo, chapters, state) -> int:
    print(f"{len(chapters)} chapters in SUMMARY.md; spent so far ${state['spent_usd']:.2f}")
    total_lo = total_hi = 0.0
    for phase in PHASES:
        todo = select(phase, repo, chapters, state)
        if args.only:
            todo = [c for c in todo if matches(c, args.only)]
        reqs = build_requests(phase, repo, chapters, todo) if todo else []
        lo, hi = estimate(phase, reqs, state)
        total_lo, total_hi = total_lo + lo, total_hi + hi
        print(f"{phase:7s} {len(todo):4d} ready  model={PHASES[phase].model:18s} est ${lo:6.2f} - ${hi:6.2f}")
    print(f"total   est ${total_lo:.2f} - ${total_hi:.2f} (heuristic; `collect` records the real spend)")
    print("note: verify and cards become ready only after the previous phase is collected.")
    return 0


def cmd_submit(args, repo, chapters, state) -> int:
    todo = select(args.phase, repo, chapters, state)
    if args.only:
        todo = [c for c in todo if matches(c, args.only)]
    if args.limit:
        todo = todo[: args.limit]
    if not todo:
        print(f"nothing ready for phase {args.phase}")
        return 0
    reqs = build_requests(args.phase, repo, chapters, todo, args.model)
    lo, hi = estimate(args.phase, reqs, state)
    committed = sum(b.get("estimate_high", 0.0) for b in state["batches"] if not b.get("collected"))
    print(f"{args.phase}: {len(reqs)} requests, est ${lo:.2f} - ${hi:.2f}; spent ${state['spent_usd']:.2f}, in flight up to ${committed:.2f}")
    for ch in todo[:10]:
        print(f"  {ch.cid:8s} {ch.title}")
    if len(todo) > 10:
        print(f"  ... and {len(todo) - 10} more")
    if state["spent_usd"] + committed + hi > args.budget:
        print(f"refusing: spent + in flight + this batch (high) exceeds --budget ${args.budget:.2f}; lower --limit")
        return 1
    if not args.apply:
        print("dry run; pass --apply to submit")
        return 0
    batch = client().messages.batches.create(requests=reqs)
    state["batches"].append({
        "id": batch.id, "phase": args.phase, "model": reqs[0]["params"]["model"], "created": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "custom_ids": {r["custom_id"]: ch.path for r, ch in zip(reqs, todo, strict=True)},
        "estimate_high": round(hi, 2), "collected": False,
    })
    save_state(repo, state)
    print(f"submitted {batch.id}; run `status`, then `collect --apply` once it has ended")
    return 0


def cmd_status(args, repo, chapters, state) -> int:
    open_batches = [b for b in state["batches"] if not b.get("collected")]
    if not open_batches:
        print(f"0 open batches; spent ${state['spent_usd']:.2f}")
        return 0
    api = client()
    for b in open_batches:
        live = api.messages.batches.retrieve(b["id"])
        rc = live.request_counts
        print(f"{b['id']} {b['phase']:6s} {live.processing_status:11s} ok={rc.succeeded} err={rc.errored} "
              f"processing={rc.processing} expired={rc.expired} canceled={rc.canceled}")
    return 0


def apply_result(phase: str, ch: Chapter, message: dict, repo: Path, state: dict, allowed: set[str],
                 write: bool, report: list[str]) -> float:
    model = message.get("model") or PHASES[phase].model
    cost = usage_cost(model if model in PRICES else PHASES[phase].model, message.get("usage") or {})
    stop = message.get("stop_reason")
    text = response_text(message)
    reports = repo / STATE_DIR / "reports" / phase
    reports.mkdir(parents=True, exist_ok=True)
    if stop not in ("end_turn", "stop_sequence"):
        (reports / f"{ch.cid}.raw.md").write_text(text)
        report.append(f"- {ch.cid} {ch.title}: stop_reason={stop}; not written (raw text saved)")
        return cost
    path = repo / ch.path

    if phase == "cards":
        rows = [ln for ln in text.splitlines() if ln.count("\t") == 1 and all(f.strip() for f in ln.split("\t"))]
        tag = Path(ch.path).stem
        out = repo / CURRICULUM / "Flashcards" / f"{ch.volume_dir}.tsv"
        if write and rows:
            out.parent.mkdir(parents=True, exist_ok=True)
            kept = [ln for ln in (out.read_text().splitlines() if out.exists() else []) if not ln.endswith(f"\t{tag}")]
            out.write_text("\n".join(kept + [f"{r}\t{tag}" for r in rows]) + "\n")
            mark_done(state, phase, ch.path)
        report.append(f"- {ch.cid} {ch.title}: {len(rows)} cards")
        return cost

    body, tail = extract_note(text, ch.title)
    if not body:
        (reports / f"{ch.cid}.raw.md").write_text(text)
        report.append(f"- {ch.cid} {ch.title}: no title line found; not written")
        return cost
    body, dropped = sanitize_links(clean_style(body), allowed)
    gaps = [] if ch.is_appendix else missing_sections(body)
    (reports / f"{ch.cid}.md").write_text(f"# {ch.title}\n\n{tail.strip()}\n\nDropped links: {dropped or 'none'}\n")
    note = ""

    if phase == "draft":
        if note_status(path) not in ("missing", "seed") and not owned_unchanged(repo, ch, state):
            report.append(f"- {ch.cid} {ch.title}: skipped, file changed to a non-seed note meanwhile")
            return cost
        sources = list(dict.fromkeys(cited_urls(message) + further_reading_urls(body)))[:30]
        note = render_note(ch, "draft", body, sources)
        unverified = len(re.findall(r"^- (?!none)", tail.split("-->", 1)[-1], re.M))
        report.append(f"- {ch.cid} {ch.title}: {len(body.splitlines())} lines, {len(sources)} cited sources, "
                      f"{unverified} unverified, missing sections {gaps or 'none'}, dropped links {len(dropped)}")
    else:  # verify
        if not owned_unchanged(repo, ch, state):
            report.append(f"- {ch.cid} {ch.title}: skipped, edited since the pipeline wrote it (report saved)")
            (reports / f"{ch.cid}.corrected.md").write_text(body)
            return cost
        old = path.read_text()
        old_body = strip_frontmatter(old)
        if len(body) < 0.85 * len(old_body):
            (reports / f"{ch.cid}.corrected.md").write_text(body)
            report.append(f"- {ch.cid} {ch.title}: corrected text is {len(body) / max(len(old_body), 1):.0%} of the draft; kept the draft, saved the correction for review")
            mark_done(state, phase, ch.path)
            return cost
        m = re.search(r"UNRESOLVED:\s*(\d+)", tail)
        unresolved = int(m[1]) if m else -1
        status = "solid" if unresolved == 0 and not gaps else "draft"
        old_sources = re.findall(r'^\s+-\s+"([^"]+)"', FRONTMATTER_RE.match(old)[1], re.M) if FRONTMATTER_RE.match(old) else []
        sources = list(dict.fromkeys(old_sources + cited_urls(message) + further_reading_urls(body)))[:40]
        note = render_note(ch, status, body, sources)
        corrections = len(re.findall(r"^- CORRECTED:", tail, re.M))
        report.append(f"- {ch.cid} {ch.title}: {corrections} corrections, unresolved {unresolved}, status {status}")

    if write:
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(note)
        state["owned"][ch.path] = sha(note)
        if phase == "verify":
            mark_done(state, phase, ch.path)
    return cost


def cmd_collect(args, repo, chapters, state) -> int:
    by_path = {c.path: c for c in chapters}
    known = {c.link for c in chapters} | set(related_notes(repo)) | {f"{CURRICULUM}/SUMMARY", f"{CURRICULUM}/README", f"{CURRICULUM}/STYLE"}
    api = None
    redo = set(args.reprocess.split(",")) if args.reprocess else set()
    for b in [b for b in state["batches"] if not b.get("collected") or b["id"] in redo]:
        again = b.get("collected", False)
        raw = repo / STATE_DIR / "results" / f"{b['id']}.jsonl"
        if not raw.exists():
            api = api or client()
            if api.messages.batches.retrieve(b["id"]).processing_status != "ended":
                print(f"{b['id']}: still processing")
                continue
            raw.parent.mkdir(parents=True, exist_ok=True)
            part = raw.with_name(raw.name + ".part")
            with part.open("w") as f:
                for r in api.messages.batches.results(b["id"]):
                    f.write(json.dumps(to_dict(r)) + "\n")
            part.replace(raw)
        report: list[str] = [f"# {b['phase']} batch {b['id']}", ""]
        spent = 0.0
        for line in raw.read_text().splitlines():
            r = json.loads(line)
            ch = by_path.get(b["custom_ids"].get(r["custom_id"], ""))
            res = r["result"]
            if ch is None:
                report.append(f"- {r['custom_id']}: chapter no longer in SUMMARY.md")
                continue
            if res["type"] != "succeeded":
                err = (res.get("error") or {}).get("error", {}) or res.get("error") or {}
                report.append(f"- {ch.cid} {ch.title}: {res['type']} {err.get('type', '')} {err.get('message', '')}".rstrip())
                continue
            spent += apply_result(b["phase"], ch, res["message"], repo, state, known, args.apply, report)
            if args.apply:
                save_state(repo, state)
        report.append("")
        report.append(f"Actual spend: ${spent:.2f}")
        out = repo / STATE_DIR / "reports" / f"{b['phase']}-{b['id']}.md"
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text("\n".join(report) + "\n")
        print("\n".join(report))
        if args.apply and not again:
            b["collected"] = True
            b["actual_usd"] = round(spent, 4)
            state["spent_usd"] = round(state["spent_usd"] + spent, 4)
            save_state(repo, state)
    if not args.apply:
        print("dry run; pass --apply to write notes and record spend")
    return 0


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo", type=Path, default=Path(__file__).resolve().parent.parent)
    sub = ap.add_subparsers(dest="cmd", required=True)
    for name in ("plan", "submit", "status", "collect"):
        p = sub.add_parser(name)
        p.add_argument("--apply", action="store_true")
        p.add_argument("--only", default="", help="comma-separated substrings of chapter paths, e.g. Volume_10,Volume_11")
        if name == "submit":
            p.add_argument("--phase", choices=list(PHASES), required=True)
            p.add_argument("--limit", type=int, default=0)
            p.add_argument("--model", choices=list(PRICES), default="", help="override the phase's default model")
            p.add_argument("--budget", type=float, default=100.0, help="USD cap for spent + in flight + this batch")
        if name == "collect":
            p.add_argument("--reprocess", default="", help="comma-separated collected batch ids to re-apply from stored results "
                           "(rewrites only chapters the pipeline wrote and nobody edited; spend is not counted again)")
    args = ap.parse_args(argv)
    repo = args.repo.resolve()
    summary = repo / CURRICULUM / "SUMMARY.md"
    if not summary.exists():
        print(f"missing {summary}")
        return 1
    chapters = parse_summary(summary.read_text())
    state = load_state(repo)
    return {"plan": cmd_plan, "submit": cmd_submit,
            "status": cmd_status, "collect": cmd_collect}[args.cmd](args, repo, chapters, state)


if __name__ == "__main__":
    sys.exit(main())
