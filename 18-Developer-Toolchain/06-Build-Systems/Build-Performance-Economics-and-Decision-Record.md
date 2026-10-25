---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Decision Record: Build Performance Economics and Engine Selection

> [!summary] Scope
> Owns the organizational economics of build latency and the architectural decision framework for selecting build engines.
> Reaches Decide at Scale rungs by modeling engineer wait time financial waste, critical-path analysis versus total worker CPU time, and developer machine hardware ROI.
> Formulates a rigorous decision matrix choosing between Make, Ninja, CMake, Meson, Bazel, and Buck2, detailing defaults, organizational failure modes, and migration triggers.

## Related

- Prerequisites: [[Bazel-Skyframe-Bzlmod-and-Execution-Model]], [[Modern-CMake-Architecture-and-Targets]]
- Next: [[Diagnose-Bazel-and-Buck2-Cache-Misses]]
