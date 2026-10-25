---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Rust Publishing: crates.io, RFC 3691 Trusted Publishing, and Private Registries

> [!summary] Scope
> Package verification with cargo package, sparse HTTP index protocol operations via RFC 2789, and OIDC Trusted Publishing via RFC 3691.
> Details crates.io yank semantics, Cargo min-publish-age resolution cooldowns (on beta, slated for stable Rust 1.100), and hosting enterprise private registries with authentication and source replacement.
> Establishes release checklist invariants for public and proprietary crates.
> Outlines token security and credential management in continuous deployment runners.

## Related

- Prerequisites: [[Release-Trains-Backports-and-Artifact-Registries]], [[Rust-Cargo-Model-Packages-Targets-Features-and-Workspaces]]
- Next: [[Rust-Binary-Distribution-with-dist-and-Cross-Compilation]]
