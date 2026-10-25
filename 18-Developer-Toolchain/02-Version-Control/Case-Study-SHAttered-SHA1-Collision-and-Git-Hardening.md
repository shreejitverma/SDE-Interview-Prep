---
type: case-study
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Case Study: The 2017 SHAttered SHA-1 Collision and Git Hardening

> [!summary] Scope
> Owns the postmortem analysis of the 2017 SHAttered attack that produced two distinct PDF documents with identical SHA-1 cryptographic hashes and its impact on Git's content-addressable storage.
> Reaches Explain and Decide at Scale rungs by detailing identical-prefix collision mechanics, distinguishing them from 2020 chosen-prefix attacks, examining how colliding Git blobs can corrupt repository integrity, evaluating the integration of sha1dc, and charting the engineering hurdles of the multi-year SHA-256 transition.
> Sets the boundary with general supply chain security.

## Related

- Prerequisites: [[Git-Object-Model-and-Content-Addressing]]
- Next: [[Rebuild-Mini-Git-Lab]]
- See also: [[11-Security-And-Cryptography/README|README]]
- Lab: `code/02-Version-Control/shattered_detect_demo/`
