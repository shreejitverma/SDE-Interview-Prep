---
type: playbook
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Capstone: Hermetic Cached Monorepo Build for Polyglot Workspaces

> [!summary] Scope
> This capstone playbook documents the architecture and implementation of a hermetic, remotely cached multi-language build system spanning C++, Python, and Rust services.
> It implements content-addressable action inputs, strict sandboxing via Linux user namespaces or bubblewrap, and deterministic toolchain pinning.
> It integrates a distributed cache using the Remote Execution API (REAPI) and demonstrates strict build reproducibility verified by bitwise diffoscope comparisons across independent build runners.

## Related

- Prerequisites: [[Hermetic-and-Reproducible-Builds]], [[Sandboxing-and-Hermetic-Execution-Internals]], [[Remote-Execution-API-and-Remote-Caching]], [[Bazel-Skyframe-Bzlmod-and-Execution-Model]], [[Compiler-Caches-ccache-and-sccache]]
- Next: [[Capstone-PubGrub-Dependency-Resolver]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/manuscript/10-part-compilation/39_build_systems_and_modules|39_build_systems_and_modules]]
- Lab: `code/11-Arena-and-Capstones/capstone_hermetic_monorepo/` (requires: linux, docker)
