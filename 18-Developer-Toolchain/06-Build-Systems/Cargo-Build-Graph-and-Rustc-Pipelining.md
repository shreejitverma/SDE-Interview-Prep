---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Cargo Build Graph and rustc Pipelining

> [!summary] Scope
> Owns the compilation dependency model of the Rust ecosystem.
> Reaches Explain, Debug, and Decide at Scale rungs by detailing Cargo unit graph construction, package versus crate versus target hierarchies, feature unification side-effects on rebuild invalidation, rustc rmeta metadata pipelined compilation, and build script execution hazards.
> Details the handoff between Cargo and Python PEP 517 build backends and C++ linkers.

## Related

- Prerequisites: [[Build-System-Theory-Schedulers-and-Rebuilders]]
- Next: [[Build-Performance-Economics-and-Decision-Record]]
- Lab: `code/06-Build-Systems/cargo_timings_inspect/`
