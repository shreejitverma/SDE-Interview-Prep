---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Compiler Caches: ccache and sccache

> [!summary] Scope
> Owns compiler-wrapper caching technologies that accelerate builds transparently across Make, CMake, and Cargo without altering build-system engines.
> Reaches Use, Explain, and Debug rungs by dissecting ccache direct mode versus preprocessor mode, path normalization, and sccache client-server daemon architecture with cloud object storage backends.
> Explains why Rust crate-level compilation granularity limits sccache effectiveness compared to fine-grained C++ translation units.

## Related

- Prerequisites: [[Hermetic-and-Reproducible-Builds]], [[Make-and-Ninja-Mechanics]]
- Next: [[Cross-Compilation-and-Sysroots]]
- Lab: `code/06-Build-Systems/ccache_hit_rate/`
