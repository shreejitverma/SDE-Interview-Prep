---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Git Packfiles and Delta Compression

> [!summary] Scope
> Owns the binary representation of packed object stores, including .pack payload framing and .idx version 2 fanout tables.
> Reaches Explain, Debug, and Rebuild rungs by detailing sliding-window delta compression heuristics, type codes, byte offset encoding, CRC32 verification, and name-hash sorting.
> Establishes the boundary with multi-pack indexes, cruft packs, and long-term garbage collection.

## Related

- Prerequisites: [[Git-Object-Model-and-Content-Addressing]]
- Next: [[Git-GC-Maintenance-MIDX-and-Commit-Graph]]
- Lab: `code/02-Version-Control/packfile_parser/`
