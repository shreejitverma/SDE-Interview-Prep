---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Git Refs, Reflogs, and Reftable

> [!summary] Scope
> Owns the reference management subsystem including loose ref files in .git/refs, packed-refs sorting and binary searches, reflog circular-buffer journal mechanics, and atomicity via atomic ref transactions.
> Reaches Explain and Decide at Scale rungs by dissecting the binary block-based reftable storage format designed for millions of references and concurrent server transactions without filesystem lock contention.
> Leaves object reachability algorithms to graph walks and remote transfer negotiation to wire protocol notes.

## Related

- Prerequisites: [[Git-Object-Model-and-Content-Addressing]]
- Next: [[Git-Revision-Selection-and-Graph-Walks]]
- Lab: `code/02-Version-Control/reftable_reader/`
