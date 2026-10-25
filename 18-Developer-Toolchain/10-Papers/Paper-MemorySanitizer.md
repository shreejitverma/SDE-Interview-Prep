---
type: paper
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Paper: MemorySanitizer: Fast Detector of Uninitialized Memory Use in C++ (Stepanov and Serebryany 2015)

> [!summary] Scope
> Owns the CGO 2015 paper by Evgeniy Stepanov and Konstantin Serebryany on detecting reads of uninitialized memory.
> Explains bit-exact 1-to-1 shadow memory bit propagation through ALU operations, origin tracking for identifying allocation call stacks, and why MSan requires all program code, including the C++ standard library, to be instrumented, with libc covered by interceptors.
> Grounds memory safety diagnostics and uninitialized data detection in compiled languages.

## Related

- Prerequisites: [[Paper-AddressSanitizer]]
- Next: [[Paper-Tricorder-Program-Analysis-Ecosystem]]
