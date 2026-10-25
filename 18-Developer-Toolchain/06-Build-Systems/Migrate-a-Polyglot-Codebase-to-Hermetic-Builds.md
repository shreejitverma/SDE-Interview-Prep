---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Migrate a Polyglot Codebase to Hermetic Builds

> [!summary] Scope
> Owns the multi-stage migration strategy for transitioning legacy CMake, Cargo, and pip repositories into hermetic Bazel or Buck2 builds.
> Reaches Decide at Scale rungs by detailing strangler-fig migration patterns, sysroot encapsulation, coexisting build generator shims, remote cache shadow-mode verification, and developer workstation onboarding.
> Emphasizes maintaining feature velocity while progressively tightening build hermeticity and cache guarantees.

## Related

- Prerequisites: [[Build-Performance-Economics-and-Decision-Record]], [[Diagnose-Bazel-and-Buck2-Cache-Misses]]
- Next: [[Rebuild-Content-Hashed-Build-System-Lab]]
