---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Break a Critical-Path Dependency Bottleneck

> [!summary] Scope
> Owns an optimization scenario where an enterprise monorepo build exhibits high parallel core starvation because of a serialized critical-path header bottleneck.
> Reaches the Debug and Use rungs by utilizing Ninja execution logs and build profiling traces to pinpoint the serialization blocker.
> Requires refactoring monolithic target boundaries into fine-grained interface targets and measuring an immediate major reduction in end-to-end wall-clock build time.

## Related

- Prerequisites: [[Build-Performance-Economics-and-Decision-Record]], [[Modern-CMake-Architecture-and-Targets]]
- Lab: `code/06-Build-Systems/drill_dependency_bottleneck/`
