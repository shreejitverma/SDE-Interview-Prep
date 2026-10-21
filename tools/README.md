# Vault tools

Standard-library Python 3 scripts that audit and maintain the vault.
Run them from the repo root; each reads only git-tracked files.
Every script that edits notes is a dry run unless you pass `--apply`.

## Audits and guards

| Script | Output | Purpose |
| :--- | :--- | :--- |
| `audit_vault.py` | `AUDIT.md` | Counts, duplicates, broken links, table-splitting wikilinks, orphans, frontmatter, entry notes, emoji and em-dash use, vendored and large files. |
| `audit_pii.py` | a path you pass with `--out` | Inventory of private job-search data and PII; refuses to write inside this repo. `--check` is the CI and pre-commit guard. |
| `private_paths.py` | - | Private locations: gitignored here, symlinked in from the private `career-ops` repo. |

## Fixers

| Script | Purpose |
| :--- | :--- |
| `fix_links.py` | Repairs broken links (code in prose, renamed notes via `link_map.tsv`, stale paths, folder links) and seeds stubs for cited but unwritten notes. |
| `build_mocs.py` | Gives every note folder an entry note (Map of Content); generated text sits between `moc` markers. |
| `add_frontmatter.py` | Applies the frontmatter schema from `vault_schema.py`; never changes existing values. |
| `fix_style.py` | Removes emojis and em dashes while keeping their meaning. |
| `gen_book_summary.py` | Regenerates a Zero to Godhood book's `SUMMARY.md` from its chapter files. |

## API pipelines

| Script | Purpose |
| :--- | :--- |
| `cloud_batch.py` | Drafts (Opus 5.5 + web search), fact-checks (Sonnet 5.5 + web search), and flashcards (Haiku 5.5) the `17-Cloud-AI-Engineering` chapters listed in its `SUMMARY.md` through the Message Batches API at half price. |

`cloud_batch.py` is the one tool with a third-party dependency (the `anthropic` SDK), so run its API commands with `uv run tools/cloud_batch.py ...`; `plan` and `stubs` work under plain `python3`.
It spends real money: `submit` is a dry run without `--apply`, prints a cost estimate, and refuses to exceed `--budget` across everything spent and in flight.
It writes only chapters it owns (missing or `seed` files, then its own unchanged drafts), keeps state, raw results, and per-chapter reports in `build/cloud_batch/`, and reads the API key from `~/.config/sde-vault/anthropic.env`, never from this public repo.

```sh
python3 tools/cloud_batch.py plan                                  # chapters ready per phase + cost range
uv run tools/cloud_batch.py submit --phase draft --limit 3 --apply # pilot, then drop --limit
uv run tools/cloud_batch.py status
uv run tools/cloud_batch.py collect --apply                        # write notes, reports, real spend
```

## Commands

```sh
python3 tools/audit_vault.py                               # regenerate AUDIT.md
python3 tools/audit_vault.py --out - --check links         # exit 1 on a broken or table-splitting link
python3 tools/audit_vault.py --out - --check style         # exit 1 on an emoji or em dash
python3 tools/audit_pii.py --check                         # exit 1 on private data in tracked files
python3 tools/audit_pii.py --out ~/github/career-ops/command-center/PII-INVENTORY.md
python3 -m unittest discover tools/tests                   # tool behavior tests
git config core.hooksPath tools/hooks                      # once per clone: pre-commit private-data guard
```

CI runs the same `lint` and `test` commands from `.no-mistakes.yaml` (`.github/workflows/ci.yml`).
`AUDIT.md` is generated, so never edit it by hand; re-run the script after structural changes.
Links resolve the way Obsidian resolves them: wikilinks by vault path or unique basename, Markdown links relative to the note and then to the vault root, and links inside code are ignored.
Links in `_Templates/` folders are neither checked nor repaired, because Templater placeholders only resolve in the notes a template creates.
