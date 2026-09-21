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
