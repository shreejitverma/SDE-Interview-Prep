---
type: concept
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Note Contract for Shell and Unix Engineering

These rules govern every note in `19-Shell-and-Unix-Engineering/`.
They extend the vault conventions in the repository `CLAUDE.md` and `00-Start-Here/How-This-Vault-Works.md`, adapted to a subject whose claims can be executed.

## Shape of the knowledge base

One concept per note.
Split a note when its core idea needs more than one diagram.
File names are hyphenated, for example `Field-Splitting-and-IFS.md`, and unique across the whole vault, so a bare wikilink resolves.
The H1 is the human title; the file stem never changes once other notes link to it.
Each folder README is a map of content with the folder's scope and a numbered Learning order, which is the canonical reading sequence.

## Prose and formatting

One full sentence per physical line.
No emojis anywhere, and no em dashes or en dashes; use a plain dash "-".
ASCII only in notes and labs; write arrows as "->" and comparisons as ">=".
Every piece of shell syntax in prose goes in backticks, including `$var`, `$?`, and `"$@"`, because Obsidian renders text between two dollar signs as math.
Inside a Markdown table, write a wikilink alias as `[[Note\|Alias]]`.
In Mermaid labels, write a line break as `<br/>`; Mermaid prints `\n` literally.
Dense, factual prose; mechanism first; name the trade-off and the failure mode of every recommendation.

## The solid bar

A note reaches `status: solid` when it has these sections, in this order:

1. `# Title`, then a `> [!summary] Claim` callout with the one sentence a tired reader needs.
2. `## TL;DR`: five lines covering the mechanism, the rule, the trap, the portability fact, and when not to use it.
3. `## Mental model`: one Mermaid diagram (a flowchart, `stateDiagram-v2`, or `sequenceDiagram`) and one paragraph narrating its arrows.
4. `## How it works`: the mechanism down to the system call, the specification section, and the implementation.
   Cite POSIX by section, for example `(POSIX.1-2024 XCU 2.6.5)`, implementations by release, file, and function, for example `(bash-5.3 subst.c, <function>)`, and kernel behavior by source path or man page, each checked against the real document.
5. `## Numbers and limits`: a table; every number is sourced and dated, or measured in the lab with the command, machine, operating system, and shell version stated.
6. `## Portability`: a table across POSIX sh, macOS bash 3.2, current bash, zsh (native and as sh), dash, ksh93, mksh, yash, posh, and BusyBox ash, plus GNU, BSD and macOS, BusyBox, and uutils differences for every utility used; every cell is proven by the lab matrix or cites a source.
7. `## Worked example`: a link to the note's lab in [labs/](labs/README.md), the code, and output captured from a real run.
8. `## Pitfalls`: `> [!warning]` callouts with trigger, symptom, and fix, citing real incidents, CVEs, and postmortems by primary source.
9. `## Security and robustness`.
10. `## The principal's view`: the decision a staff-plus engineer makes about this topic, its trade-off, and when to use a different tool or language.
11. `## Interview questions`: at least eight across L5, L6, and L7+, each a `> [!question]` callout with a folded `> > [!success]- Answer` and a follow-up, including one predict-the-output and one debug-this-script question.
12. `## Related`: `Prerequisites:`, `Next:`, and `See also:` lines of wikilinks, inside this section and across the vault.
13. `## Further reading`: primary sources only, one line each on why it matters.

Notes in `26-Projects` and `27-Reference` keep sections 1, 2, 12, and 13 and adapt the rest to their form (a specification and test plan, or a reference table).

## Verification

Shell behavior is checkable, so check it: run it in the matrix, read the man page, the specification, and the source.
Never invent a flag, option, specification section, source function, CVE, version, benchmark, or citation.
State the versions of every behavior that varies between releases, and date volatile facts: `(as of 2026-10)`.
A claim that cannot be verified is omitted or marked `UNVERIFIED: <why>`.
Source priority: the POSIX specification and rationale, implementation source and maintainer documentation, man pages, peer-reviewed papers, maintainer writing, then reputable secondary sources.

## Lifecycle

`seed`: frontmatter, the claim placeholder, empty section headings, and Related links.
`draft`: every section written and the lab passing; `UNVERIFIED` markers allowed.
`solid`: an independent fact-check by a reviewer who did not draft the note has rerun the lab, re-executed every portability claim, rechecked every number and citation, and left no `UNVERIFIED` marker.
`canonical`, `level`, and `last_reviewed` are set only by a human.
Every primary source a note relies on is listed in its `sources:` frontmatter.

## Later: the book

These notes are the source for a future Zero to Godhood book, compiled separately outside this vault.
Keep that compile mechanical: folder order is part order, each folder's Learning order is chapter order, notes stay self-contained with stable headings, and labs are referenced by path.
