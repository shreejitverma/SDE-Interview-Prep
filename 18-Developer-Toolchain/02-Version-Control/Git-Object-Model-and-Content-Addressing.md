---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Git Object Model and Content Addressing

> [!summary] Scope
> Owns the fundamental directed acyclic graph storage engine of Git, including loose object zlib deflation, header serialization, and SHA-1 content addressing.
> Reaches Explain and Rebuild rungs by defining exact binary byte layouts and parsing algorithms for blob, tree, commit, and annotated tag objects.
> Establishes the boundary with delta-compressed packfiles and mutable references, while documenting the SHA-1 to SHA-256 hash transition and collision mitigation via sha1dc.

## Related

- Prerequisites: none
- Next: [[Git-Refs-Reflog-and-Reftable]]
- See also: [[15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng|Binary Exploitation and Reverse Eng]]
- Lab: `code/02-Version-Control/object_store/`
