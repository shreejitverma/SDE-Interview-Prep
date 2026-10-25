---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rust Dependency Hygiene: cargo-deny, cargo-audit, and cargo-vet

> [!summary] Scope
> Automated dependency policy enforcement in Rust supply chains.
> Details cargo-deny configuration for licenses, bans, and duplicate crate versions, cargo-audit and cargo-auditable for binary vulnerability scanning against the RustSec database, and cargo-vet v0.10.3 for federated differential source-code auditing.
> Establishes continuous integration gating rules for auditing third-party crates.
> Covers supply chain risk reduction in high-integrity software projects.

## Related

- Prerequisites: [[Rust-Cargo-Model-Packages-Targets-Features-and-Workspaces]]
