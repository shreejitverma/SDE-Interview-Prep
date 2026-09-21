# Vault conventions

This repo is a public Obsidian vault and a Git repository at the same time.
Everything committed here is public; private job-search data lives in the private `career-ops` repo.

## Private data

- `16-Interview-Command-Center/{02-Companies,03-Pipeline,04-Retrospectives,05-Behavioral,06-Daily-Log}` are symlinks into `~/github/career-ops/command-center/` and are gitignored.
- `tools/private_paths.py` is the single list of private locations; change it there, never in individual tools.
- Never copy recruiter names, contact details, compensation, application status, or email content into a public note.
- The Obsidian git plugin commits the whole vault automatically, so keep the pre-commit guard installed: `git config core.hooksPath tools/hooks`.
- Third-party books, course notes, and paid course PDFs are never committed; they live in `career-ops/library/`. `.gitignore` blocks `*.pdf` and allowlists only the vault's own books and openly licensed files.

## Notes

Every knowledge note starts with this frontmatter (`tools/vault_schema.py`):

```yaml
type: concept | pattern | problem | case-study | paper | playbook | moc
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level: L3 | L4 | L5 | L6 | L7 | L8+   # blank until a human assigns it
status: seed | draft | solid | canonical
last_reviewed: YYYY-MM-DD             # blank until a human reviews it
sources: []
```

- A note reaches `solid` when it has a TL;DR, core concepts, a worked example or runnable code, pitfalls, interview questions, and further reading.
- `seed` notes are stubs that other notes already cite; they are the writing backlog.
- Every note folder has an entry note (`README.md`, or `00 Home` / `MOC - ` in the low-latency vault); `tools/build_mocs.py` keeps the generated part current.
- Superseded drafts go in `_archive/` folders; they are excluded from indexes, link repair, and orphan counts.
- Inside a Markdown table, write a wikilink alias as `[[Note\|Alias]]`; a bare `|` splits the cell.

## Writing style

- No emojis and no em dashes anywhere, including code comments; use a plain "-".
- One sentence per line in long Markdown.
- Tool source stays ASCII; write non-ASCII characters as `\u` escapes.

## Checks

Run these before committing structural changes; CI runs the same commands from `.no-mistakes.yaml`:

```sh
python3 tools/audit_vault.py --out - --check links
python3 tools/audit_vault.py --out - --check style
python3 tools/audit_pii.py --check
python3 -m unittest discover tools/tests
python3 tools/audit_vault.py        # refresh AUDIT.md
```

Changes reach GitHub only through the `no-mistakes` pipeline, never a bare `git push`.
