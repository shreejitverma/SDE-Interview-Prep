---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Repair a Corrupted Git Repository

> [!summary] Scope
> Owns operational remediation of filesystem, disk, and object-database corruptions in Git repositories.
> Reaches Debug and Rebuild rungs by diagnosing bitflipped loose objects via checksum mismatches, truncated pack and index files, broken multi-pack-index chains, corrupt commit-graph layers, and damaged index binary headers.
> Provides commands to rebuild indexes, excise dead objects, unpack valid objects from broken packs, and restore consistency while minimizing data loss.

## Related

- Prerequisites: [[Git-Packfiles-and-Delta-Compression]], [[Git-GC-Maintenance-MIDX-and-Commit-Graph]]
- Next: [[Remove-Secrets-and-Rewrite-Git-History]]
- Lab: `code/02-Version-Control/repo_repair/`
