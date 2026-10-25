---
type: concept
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Authoring Style and Quality Standards for Shell and Unix Engineering

These rules govern every chapter note in `18-Shell-and-Unix-Engineering/`; the appendices follow the prose and verification rules but not the chapter template.
They extend the vault conventions in the repository `CLAUDE.md` and mirror the Cloud AI curriculum's contract, adapted to a subject whose claims can be executed.

## Prose and Formatting

One full sentence per physical line in long Markdown blocks.
Never use emojis anywhere in notes, diagrams, code comments, or commit messages.
Never use em dashes or en dashes; use a plain dash "-".
Keep notes, labs, and code ASCII; write arrows as "->" and comparisons as ">=".
Wrap every piece of shell syntax in prose in backticks, including `$var`, `$?`, and `"$@"`: Obsidian renders text between two dollar signs as math.
Wikilink aliases inside Markdown tables must be escaped as `[[Note\|Alias]]`.
Dense, factual, and technically precise prose; no motivational filler and no introductory fluff.
Explain mechanism first: name the system call, the specification section, or the source function that causes a behavior.
State trade-offs explicitly, including the failure modes of every recommended pattern.

## Note Template: The Solid Bar

Every chapter note that reaches `status: solid` contains these ten level-2 sections in this exact order.
A chapter seed already carries the ten headings; drafting fills them in and removes the seed callout.
Appendices are reference pages with their own form: a Scope section, then content suited to a glossary, map, cheat sheet, or plan, as in `17-Cloud-AI-Engineering`.

1. TL;DR.
Five lines a staff engineer must know cold: the mechanism, the rule, the trap, the portability fact, and when not to use it.

2. Mental Model.
One Mermaid diagram (a process tree, a descriptor table, the expansion pipeline, a state machine, or a data flow) and one paragraph of intuition.

3. Internals.
The mechanism down to the system call and the source.
Cite the specification by section, for example `(POSIX.1-2024 XCU 2.6.5)`, and implementations by release, file, and function, for example `(bash-5.3 subst.c, expand_word_internal)`.
Cite kernel behavior by source path or man page, for example `(Linux fs/pipe.c)` or `(pipe(7), Linux man-pages 6.x)`, and macOS behavior by its own man page.
Every function name, file path, and section number must be checked against the actual source or document before it is written.

4. Numbers and Limits to Know.
A table of the limits and costs that matter: sizes, counts, timings, and defaults.
Every number is either sourced and dated, for example `(getconf ARG_MAX on Linux 6.x, as of 2026-10)`, or measured in the lab with the command, machine, operating system, and shell version stated.

5. Portability Matrix.
A table of how the construct behaves under POSIX sh, macOS bash 3.2, current bash, zsh (native and in sh emulation), dash, ksh93, mksh, yash, posh, and BusyBox ash, plus GNU, BSD and macOS, BusyBox, and uutils differences for every utility used.
Every cell is backed by the chapter's lab running under the matrix; a cell the matrix cannot test (for example a commercial Unix) is marked with its source.

6. Worked Example and Lab.
A runnable lab in `labs/vNN-cNN-<slug>/` (see [labs/README.md](labs/README.md)), linked from the chapter.
The chapter shows the code and the real output captured from a run, never output written by hand.
Performance claims include the `hyperfine` command and its raw summary.

7. Failure Modes and Pitfalls.
Production failure patterns with the exact trigger and the symptom.
Reference public incidents, postmortems, CVEs, and bug reports by their primary source where they exist.

8. Security and Robustness.
Injection surfaces, quoting, argument injection, environment and filesystem attacks, privilege, secrets exposure, idempotency, atomicity, and cleanup, as they apply to this construct.

9. Interview Questions and Model Answers.
At least eight questions tiered across senior (L5), staff (L6), and principal (L7+) levels.
Include at least one predict-the-output question and one debug-this-script question.
Each question has a thorough model answer and a realistic follow-up question.

10. Further Reading.
Primary sources only (specifications, source trees, man pages, papers, maintainer writing, and canonical books), each with one line on why it matters.

Chapters in Volumes 19 to 21 may adapt sections 5 to 8 to their form (a paper, a drill, or a capstone), but keep the ten headings so the structure stays uniform.

## Verification and Accuracy Rules

Shell behavior is checkable, so check it: run it in the matrix, read the man page, read the specification, read the source.
Never invent a flag, option, specification section, source function, CVE, version, benchmark, or citation.
State versions for every behavior that varies between releases, for example "bash 5.2 and later".
Date volatile facts such as current releases and distribution defaults: `(as of 2026-10)`.
If a claim cannot be verified against a primary source or the lab matrix, omit it or mark it `UNVERIFIED` with a few words on why.
Prioritize sources in this order: the POSIX specification and its rationale, implementation source code and maintainer documentation, man pages, peer-reviewed papers, maintainer writing, and then reputable secondary sources.

## Lifecycle

`seed`: frontmatter, scope, and, for a chapter, the ten empty headings.
`draft`: all ten sections written and the lab passing, with `UNVERIFIED` markers allowed.
`solid`: an independent verification pass (by an agent or person who did not draft the chapter) has rerun the lab, re-executed every portability claim, recomputed every number, and left no `UNVERIFIED` marker.
`canonical`: a human has reviewed the chapter and set `last_reviewed`; `level` is also set only by a human.
Append every primary source the chapter relies on to the `sources:` frontmatter list.
