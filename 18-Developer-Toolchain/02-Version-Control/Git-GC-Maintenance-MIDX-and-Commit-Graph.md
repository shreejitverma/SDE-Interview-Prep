---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Git GC, Maintenance, Multi-Pack-Index, and Commit-Graph

> [!summary] Scope
> Owns long-term repository maintenance, reachability acceleration, and chunk-based format architectures.
> Reaches Explain, Debug, and Decide at Scale rungs by detailing commit-graph chunk formats with corrected commit-date generation numbers, monolithic and incremental multi-pack-index chains, reachability and pseudo-merge bitmaps, cruft pack expiration, and background geometric maintenance.
> Leaves single-packfile binary decoding to packfile internals and initial clone wire transmission to protocol notes.

## Related

- Prerequisites: [[Git-Packfiles-and-Delta-Compression]], [[Git-Revision-Selection-and-Graph-Walks]]
- Next: [[Git-Transfer-Protocol-and-Partial-Clone]]
- Lab: `code/02-Version-Control/midx_inspect/`
