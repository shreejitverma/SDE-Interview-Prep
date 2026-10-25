---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Hunt Non-Determinism with Diffoscope

> [!summary] Scope
> Owns a practical debugging scenario where a C++ and Rust compilation unit fails remote cache hits due to hidden non-determinism.
> Reaches the Debug and Explain rungs under timed conditions by tasking the engineer with running diffoscope across successive builds, identifying embedded macros, unsorted dictionary iterations in code generators, and leaked build paths.
> Requires modifying the build configuration to achieve bit-for-bit identity across repeated builds.

## Related

- Prerequisites: [[Hermetic-and-Reproducible-Builds]], [[Diagnose-Bazel-and-Buck2-Cache-Misses]]
- Next: [[Drill-Build-Systems-Fix-Broken-Incremental-Depfile]]
- Lab: `code/06-Build-Systems/drill_diffoscope_determinism/` (requires: docker)
