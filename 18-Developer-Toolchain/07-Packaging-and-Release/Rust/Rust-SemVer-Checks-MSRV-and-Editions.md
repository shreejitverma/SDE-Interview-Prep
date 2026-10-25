---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rust API Evolution: cargo-semver-checks, MSRV, and Editions

> [!summary] Scope
> Detecting breaking API changes in Rust libraries using cargo-semver-checks, rustdoc JSON compilation, and Trustfall graph queries.
> Analyzes subtle Rust SemVer hazards including non_exhaustive omissions, sealed traits, auto-trait leakage, and hidden doc items.
> Explains Minimum Supported Rust Version enforcement and edition migration boundaries.
> Covers automated CI gates for blocking inadvertent semver violations before publication.

## Related

- Prerequisites: [[Versioning-SemVer-CalVer-and-ABI]], [[Rust-Cargo-Model-Packages-Targets-Features-and-Workspaces]]
- Next: [[Rust-Publishing-Crates-io-Trusted-Publishing-and-Private-Registries]]
