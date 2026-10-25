---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Remove Secrets and Rewrite Git History with git-filter-repo

> [!summary] Scope
> Owns the complete security procedure for permanently purging accidentally committed credentials, private keys, and oversized binary blobs across all branches, tags, and reflogs.
> Reaches Use, Debug, and Decide at Scale rungs by operating git-filter-repo, handling replacement commits, rewriting commit messages, sanitizing mailmaps, purging cached remote PR refs on forges, and orchestrating downstream force-push notifications.
> Emphasizes that exposed secrets must be revoked immediately regardless of history rewriting.

## Related

- Prerequisites: [[Git-Rebase-Internals-and-Stacked-Branches]], [[Git-GC-Maintenance-MIDX-and-Commit-Graph]]
- Next: [[Case-Study-SHAttered-SHA1-Collision-and-Git-Hardening]]
- See also: [[11-Security-And-Cryptography/README|README]]
- Lab: `code/02-Version-Control/filter_repo_secrets/`
