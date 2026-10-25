---
type: paper
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Paper: AddressSanitizer: A Fast Address Sanity Checker (Serebryany et al. 2012)

> [!summary] Scope
> Owns the USENIX ATC 2012 paper by Konstantin Serebryany, Derek Bruening, Alexander Potapenko, and Dmitry Vyukov introducing compile-time memory safety instrumentation.
> Explains the direct scale-and-offset virtual shadow memory mapping, poisoned redzones around allocations, quarantine allocator queues, and how ASan achieved low average slowdown compared to binary translation.
> Grounds memory bug diagnosis across C++ and Rust.

## Related

- Prerequisites: none
- Next: [[Paper-MemorySanitizer]]
