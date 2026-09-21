# Vault tools

Standard-library Python 3 scripts that audit the vault.
Run them from the repo root; each reads only git-tracked files.

| Script | Output | Purpose |
| :--- | :--- | :--- |
| `audit_vault.py` | `AUDIT.md` | Counts, duplicates, broken links, orphans, frontmatter, READMEs, emoji and em-dash use, vendored and large files. |
| `audit_pii.py` | a path you pass with `--out` | Inventory of private job-search data and PII. Refuses to write inside this repo. |
| `private_paths.py` | - | Private locations: gitignored here, symlinked in from the private `career-ops` repo. Both audits read it. |

```sh
python3 tools/audit_vault.py                        # regenerate AUDIT.md
python3 tools/audit_vault.py --out - --json         # summary only, as JSON
python3 tools/audit_vault.py --out - --check links  # exit 1 on any broken link
python3 tools/audit_pii.py --out ~/github/career-ops/command-center/PII-INVENTORY.md
```

`AUDIT.md` is generated, so never edit it by hand; re-run the script after structural changes.
Links resolve the way Obsidian resolves them: wikilinks by vault path or unique basename, Markdown links relative to the note and then to the vault root, and links inside code are ignored.
