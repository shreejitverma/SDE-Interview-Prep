---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Decision Record: Merge Commits versus Rebase and Branching Strategy

> [!summary] Scope
> Owns the trade-off evaluation and organizational decision framework between non-fast-forward merge commits, squash-merging, and rebase fast-forward workflows.
> Reaches Decide at Scale rungs by establishing the default strategy, specifying exact conditions that flip the decision, modeling the downsides regarding bisection reliability and auditability, and providing repository migration scripts.
> Connects upstream branching concepts with downstream engineering system merge queues.

## Related

- Prerequisites: [[Git-Branching-Models-Merge-Queues-and-Code-Review]]
- Next: [[Git-At-Scale-VFS-Scalar-and-Monorepos]]
