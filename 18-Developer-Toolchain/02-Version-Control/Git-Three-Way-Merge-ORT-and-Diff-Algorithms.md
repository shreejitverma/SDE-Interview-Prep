---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Git Three-Way Merge, ORT Strategy, and Diff Algorithms

> [!summary] Scope
> Owns textual difference generation and branch convergence algorithms.
> Reaches Explain, Rebuild, and Decide at Scale rungs by formalizing Myers, patience, and histogram diff algorithms, common ancestor detection via merge base walks, and the ort merge engine with directory rename detection, conflict markers, git add --resolved safety, and rerere recorded resolutions.
> Leaves interactive rebasing state machines to rebase internals and server-side worktree-free merges to git replay.

## Related

- Prerequisites: [[Git-Index-Stat-Cache-and-Fsmonitor]], [[Git-Revision-Selection-and-Graph-Walks]]
- Next: [[Git-Rebase-Internals-and-Stacked-Branches]]
- Lab: `code/02-Version-Control/merge_diff/`
