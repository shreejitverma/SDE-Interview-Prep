---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# The Cargo Model: Packages, Targets, Feature Unification, and Workspaces

> [!summary] Scope
> Architectural structure of Cargo workspaces, package manifests, and compiler target definitions.
> Analyzes additive feature unification dynamics under Cargo Resolver v2 and v3, workspace dependency inheritance, and lockfile synchronization policies.
> Identifies common pitfalls in multi-crate workspaces with optional dependencies and dev-dependency feature leakage.
> Covers workspace patch tables and vendor directories for offline builds.

## Related

- Prerequisites: [[Versioning-SemVer-CalVer-and-ABI]], [[Lockfiles-Vendoring-and-Offline-Mirrors]]
- Next: [[Rust-Build-Scripts-and-Sys-Crates]]
