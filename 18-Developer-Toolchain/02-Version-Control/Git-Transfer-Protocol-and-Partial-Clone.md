---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Git Transfer Protocol v2, Partial Clone, and Sparse Checkout

> [!summary] Scope
> Owns network data synchronization across remotes, wire protocol framing, and selective content retrieval.
> Reaches Explain, Debug, and Decide at Scale rungs by detailing wire protocol v2 pkt-line exchanges, ref-in-want capabilities, CDN bundle offloading, promisor remotes, blobless and treeless partial clones, the experimental git backfill command, and sparse-checkout cone-mode filtering with worktrees.
> Leaves local object decoding to packfiles and large binary file pointer interception to Git LFS.

## Related

- Prerequisites: [[Git-Packfiles-and-Delta-Compression]], [[Git-Index-Stat-Cache-and-Fsmonitor]]
- Next: [[Git-Three-Way-Merge-ORT-and-Diff-Algorithms]]
- See also: [[13-Agentic-AI/Agentic-Harness/components/treehouse|treehouse]]
- Lab: `code/02-Version-Control/partial_clone_negotiate/`
