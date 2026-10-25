---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Bazel: Skyframe, Bzlmod, and the Execution Model

> [!summary] Scope
> Owns artifact-based enterprise monorepo build orchestration with fine-grained caching.
> Reaches Use, Explain, and Decide at Scale rungs by detailing Bazel Loading, Analysis, and Execution phases, the Skyframe evaluation graph with early cutoff, Starlark rules, providers, aspects, persistent multiplex workers, graph introspection, and modern external dependency resolution with Bzlmod.
> Leaves remote caching and execution protocols to the REAPI note.

## Related

- Prerequisites: [[Build-System-Theory-Schedulers-and-Rebuilders]], [[Hermetic-and-Reproducible-Builds]]
- Next: [[Buck2-DICE-Starlark-and-BXL]]
- Lab: `code/06-Build-Systems/bazel_skyframe_inspect/`
