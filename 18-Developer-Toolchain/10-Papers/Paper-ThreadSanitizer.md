---
type: paper
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Paper: ThreadSanitizer: Data Race Detection in Practice (Serebryany and Iskhodzhanov 2009)

> [!summary] Scope
> Owns the seminal data race detection paper by Konstantin Serebryany and Timur Iskhodzhanov and the architectural evolution to TSan version 2.
> Analyzes vector-clock happens-before algorithms, lockset state machines, fixed shadow memory cells per 8-byte application word, and dynamic synchronization annotations.
> Grounds concurrent debugging tools and compiler race sanitizers across C++, Rust, and Go.

## Related

- Prerequisites: none
- Next: [[Paper-Google-Wide-Profiling]]
