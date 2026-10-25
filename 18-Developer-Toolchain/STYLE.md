---
type: concept
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "SDE-Interview-Prep contributors, Developer Toolchain Curriculum Specification, SDE Interview Prep, 2026, https://github.com/shreejitverma/SDE-Interview-Prep"
---

# Developer Toolchain Style Guide and Authoring Contract

This document defines the binding authoring specification, structural contracts, and verification requirements for all notes in `18-Developer-Toolchain/`.
Every note must adhere to these rules to maintain vault consistency and guarantee mechanical compilation into the future Developer Toolchain Zero to Godhood book.

## 1. General Formatting and Typography Rules

- **Sentence Structure:** Put each full sentence on its own line.
  Do not wrap multiple sentences onto one physical line.
- **Prohibited Characters:** Never use emojis in any note, commit message, code comment, or table.
- **Punctuation:** Never use em dashes or en dashes.
  Use a standard ASCII plain dash "-" surrounded by spaces.
- **Character Encoding:** Use strict ASCII characters only across all Markdown text, code blocks, and filenames.
- **Table Wikilinks:** Format all wikilinks inside Markdown tables with an escaped pipe: `[[Note\|Alias]]`.
- **Headings:** Use `#` for title (H1), `##` for major sections (H2), and `###` for sub-sections (H3).
  Never skip heading levels.

## 2. Note Kinds and Anatomy

Every note has an assigned `type` in its frontmatter.
The allowed types are `concept`, `playbook`, `problem`, `case-study`, `paper`, and `moc`.

### 2.1. `concept` (Mechanisms and Internals)
A concept note explains a specific toolchain mechanism, data structure, or algorithm.
It must contain the following sections in order:
1. `> [!summary] Claim:` A three to five line summary capturing the foundational mechanism that an expert knows cold.
2. Mermaid diagram:
   A clear architecture diagram, memory layout, or finite state machine.
   Use `<br/>` for line breaks inside node labels.
3. The Mechanism:
   Low-level data structures, system call sequences, algorithmic complexity, and the historical context explaining why the tool was designed this way.
   Cite primary sources at pinned versions.
4. Commands and Numbers to Know:
   A Markdown table of reference numbers, flag defaults, or environment variables.
   Every entry must be stamped `(as of 2026-10)` and identify its target platform (`Linux`, `macOS`, or `Both`).
5. Multi-Language Comparison:
   A C++ / Python / Rust comparative table when the concern spans languages.
6. Worked Lab Example:
   A step-by-step example linking directly to a reproducible lab in `code/`.
7. `> [!warning] Pitfalls:` Concrete anti-patterns, edge-case failures, and common traps.
8. At Scale:
   How operational trade-offs change from a single developer to 1,000-50,000 engineers, including clear decision rules.
9. `> [!tip] Practice Drill:` A recommended hands-on exercise or linked drill.
10. Interview Questions:
   Categorized interview questions tiered by level (L5, L6, L7, L8+) with model answers folded in `> [!success]- Answer` callouts.
11. Further Reading:
   Primary documentation, specifications, and papers, each with a single line explaining why it matters.
12. See Also:
   Links to related notes in `18-Developer-Toolchain/` and existing vault notes.

### 2.2. `playbook` (Operational Runbooks and Rebuilds)
A playbook note provides an actionable, repeatable procedure for resolving an incident, executing a migration, or implementing a toolchain rebuild.
It must contain:
1. Trigger and Symptoms: When to activate this procedure.
2. Mermaid Decision Tree: Visual branching logic for diagnosing and executing the workflow.
3. Prerequisites: Required tools, environment variables, and permission checks.
4. Step-by-Step Execution: Numbered, copy-pasteable commands with exact explanations.
5. Verification: Concrete check command and expected output following each step.
6. Rollback Procedure: How to cleanly undo changes if an intermediate step fails.
7. Failure Modes: Known ways the procedure can fail and how to recover.

### 2.3. `problem` (Drills and Scenarios)
A problem note presents a real-world debugging challenge or infrastructure design scenario.
It must contain:
1. Reproduction: Path to a reproducible reproduction script located in `code/`.
2. Observed Symptom: Terminal crash output, error message, or system bottleneck.
3. Progressive Hint Ladder: Hints folded in sequential `> [!tip]- Hint N` callouts.
4. Model Solution: Complete diagnosis and resolution folded in a `> [!success]- Solution` callout.
5. Core Takeaway: Architectural lesson and link to the owning concept note.

### 2.4. `case-study` (Postmortems and War Stories)
A case study analyzes a notable real-world toolchain failure or security breach.
It must contain:
1. Incident Timeline: Chronological sequence of events leading up to and during the incident.
2. Mechanism Root Cause: Exact technical failure at the system call, parser, linker, or compiler level.
3. Detection Gap: Why existing unit tests, integration suites, and linters failed to detect the defect.
4. Remediation and Fix: Code or configuration changes applied to resolve the incident.
5. Architectural Lessons: Lasting industry consequences and prevention mechanisms for modern engineering organizations.
6. References: Pinned links to official public postmortems, security advisories, and CVE trackers.

### 2.5. `paper` (Academic and Industry Canon)
A paper note distills a seminal research paper or industry technical report.
It must contain:
1. Bibliographic Metadata: Full citation, publication venue, year, and primary URL or DOI.
2. The Core Problem: Technical bottleneck or theoretical limitation motivating the work.
3. Key Innovation: Novel insight, algorithm, or architectural abstraction introduced by the authors.
4. Mechanism Details: Concrete design, execution models, and data structures.
5. Evaluation and Limitations: Experimental results, measured overheads, and unaddressed constraints.
6. Industry Impact: How this work influenced modern production toolchains.
7. Concept Grounding: Links to the concept notes in this curriculum that rely on its findings.

### 2.6. `moc` (Maps of Content and Ladders)
A MOC note structures reading paths and organizes topics within a pillar.
Each pillar contains three specific MOC notes:
- `README.md`: The pillar entry note containing scope, internal dependency flowchart, and linear learning order.
- `<Pillar>-Topic-Inventory.md`: Coverage table mapping authoritative topic headings against owning notes, depth targets, and vault coverage.
- `<Pillar>-Skill-Ladder.md`: Concrete, observable behaviors spanning levels L4 through L8+.

## 3. The Depth Ladder

Every topic in `18-Developer-Toolchain/` is developed across five progressive depth rungs:
1. **Use:** Daily command-line syntax, flags, configuration files, and workflows used by proficient engineers.
2. **Explain:** Underlying system calls, kernel subsystems, file formats, algorithms, and historic design constraints.
3. **Debug:** Failure modes, error interpretation, diagnostic tool usage, and recovery runbooks.
4. **Rebuild:** Minimum viable implementation details demonstrating how to write a toy version from scratch.
5. **Decide at Scale:** Trade-off analysis, performance economics, organizational paved roads, and migration strategies for large engineering fleets.

## 4. The Seed Shape

When a new note is initially stubbed during curriculum planning, it must follow this template:

```markdown
---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Note Title Here

> [!summary] Scope
> This note owns [exact mechanism].
> It reaches the [Use, Explain, Debug, Rebuild, Decide at Scale] rungs by [what it covers].
> It hands [adjacent topic] to [neighbor note].

## Related

- Prerequisites: [[Prerequisite-Note]]
- Next: [[Next-Note]]
- See also: [[Overlapping-Note]]
- Lab: `code/<pillar>/<lab_name>/` (requires: linux, pmu)
```

- The `> [!summary] Scope` callout is one short paragraph, one sentence per line.
- `## Related` always carries a `Prerequisites:` line.
- `Next:`, `See also:`, and `Lab:` appear only where they apply: a following note, a real overlapping note, or a runnable lab.
- A `Lab:` line carries a `(requires: ...)` suffix listing every requirement from section 8 that the lab needs, and none when it runs on the recording platform as is.
- A seed may leave `sources: []` empty until its prose is written.

## 5. Solid Bar Checklist

A note can only be promoted from `draft` to `solid` status after satisfying every item in this checklist:
- [ ] Unique basename verified against all Markdown files across the entire study vault.
- [ ] Frontmatter conforms strictly to `tools/vault_schema.py` with valid `type` and `track` lists.
- [ ] No emojis, no em dashes, ASCII only, and each sentence placed on its own line.
- [ ] All table wikilinks use escaped pipe format: `[[Target-Note\|Display-Alias]]`.
- [ ] All claims and commands verified against local tool execution or authoritative documentation.
- [ ] Volatile flags, defaults, and benchmarks stamped with `(as of 2026-10)` and target platform.
- [ ] Code examples backed by executable scripts in `code/<pillar>/<lab>/` that pass `make verify`.
- [ ] At least three outbound wikilinks within `18-Developer-Toolchain/`.
- [ ] Inbound link confirmed from the parent pillar's `README.md` and topic inventory.
- [ ] Exactly one corresponding entry registered in `18-Developer-Toolchain/manifest.tsv`.

## 6. Accuracy, Licensing, and Safety

- **Primary Source Grounding:** Do not invent CLI flags, configuration fields, PEP numbers, kernel versions, or benchmark metrics.
  Verify every assertion against official reference documentation or source code.
- **Dating Volatile Claims:** Any assertion subject to toolchain evolution must be stamped `(as of 2026-10)` and specify the platform (`Linux`, `macOS`, or `Both`).
- **Status Tagging:** Explicitly mark features that are experimental, nightly-only, deprecated, or proposed but unreleased.
- **Unverified Claims:** If a fact cannot be mechanically verified, mark it with an `UNVERIFIED` tag and submit it to the phase verification report.
- **Licensing and Quotation:** Respect copyright.
  Do not reproduce long verbatim excerpts from third-party books or proprietary courseware.
  Cite sources with full attribution and URLs.
- **Security and Privacy:** Never include private credentials, internal hostnames, or confidential company data in code listings or examples.

## 7. Export Rules for Book Compilation

To ensure mechanical compilation into the Zero to Godhood book format:
- **One Owner Per Mechanism:** Each toolchain mechanism has exactly one owning note.
  Adjacent notes may summarize the mechanism in two sentences but must link to the owning note rather than re-deriving it.
- **Self-Contained Navigation:** A reader must be able to understand any note using only notes inside `18-Developer-Toolchain/`.
  Links to external vault notes are supplementary "See also" references only.
- **No Positional References:** Never use relative positional language like "as discussed above" or "in the previous chapter".
  Refer to concepts and notes exclusively by title and wikilink.
- **Plain Markdown Compatibility:** Notes must retain their complete semantic meaning when rendered as plain text or standard Markdown.
  Obsidian plugins and Dataview queries are prohibited from carrying required knowledge.
- **Full Frontmatter Citations:** Every source entry in frontmatter must be a complete, publication-grade citation including author, title, venue, year, and URL.

## 8. The Lab and Code Harness Contract

All runnable code, reproduction test cases, and benchmarks must reside inside the `code/` directory:
- **Directory Layout:** Labs are organized strictly as `code/<pillar>/<lab_name>/`.
- **Golden Outputs:** Expected execution output must be stored in `code/<pillar>/<lab_name>/expected/<platform>.txt`, where platform is `<os>-<arch>` as computed by `code/_harness/verify.py` (for example `darwin-arm64`, `linux-aarch64`, `linux-amd64`).
- **Harness Scripts:**
  - `make verify`: Executes every lab (a directory holding `run.sh`) whose requirements are met and validates that its stdout matches `expected/<platform>.txt`.
  - `make record`: Runs labs and updates the golden output files.
  - `make verify LAB=<name>`: Runs verification only for labs whose path contains `<name>`.
- **Environment Gating:** Every lab's `run.sh` must declare its environmental requirements in a `# requires:` comment within its first 10 lines:
  - `# requires: linux`: Linux system call dependencies (e.g., ptrace, namespaces, epoll); the lab runs only on a Linux host and is skipped on macOS.
  - `# requires: docker`: The lab itself drives Docker (building or running images) and needs a working daemon.
  - An unknown requirement fails `make verify`, so a typo cannot silently stop a lab from running.
  - `# requires: pmu`: Hardware performance counters or PMU virtualization, opted in with `TOOLCHAIN_LABS_PMU=1`.
  - `# requires: nightly`: Nightly language compiler features.
  - `# requires: network`: External network or package registry access, opted in with `TOOLCHAIN_LABS_NETWORK=1`.

## 9. The Manifest Contract (`manifest.tsv`)

The file `18-Developer-Toolchain/manifest.tsv` is the single source of truth for the curriculum's reading order and structural taxonomy:
- **File Format:** Tab-separated values (TSV) with UTF-8 encoding and Unix LF line endings.
- **Columns (in exact order):**
  1. `order`: 1-based sequential integer defining global reading order.
  2. `pillar_label`: Two-digit pillar identifier (e.g., `00`, `01`, ..., `11`), `root` for the folder-level `README.md` and `STYLE.md`, or `code` for `code/README.md`.
  3. `pillar_title`: Clean pillar title (e.g., `Program Execution`, `Arena and Capstones`).
  4. `note_id`: The note's `path` without `.md` (for example `00-Program-Execution/README`), unique by construction.
  5. `kind`: Note type (`concept`, `playbook`, `problem`, `case-study`, `paper`, `moc`), equal to the note's frontmatter `type`.
  6. `path`: Relative path from `18-Developer-Toolchain/` (e.g., `11-Arena-and-Capstones/README.md`).
- **Integrity Constraints:**
  - Every note in `18-Developer-Toolchain/` must appear in `manifest.tsv` exactly once.
  - Every file path listed in `manifest.tsv` must exist on disk.
  - No orphaned files or duplicate note IDs are permitted.
