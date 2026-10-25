---
type: problem
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Fix an Embedded Workspace Cargo Feature Unification Hazard

> [!summary] Scope
> Diagnostic scenario where a dev-dependency that enables a std feature on a shared dependency leaks into normal builds under legacy resolver v1 feature unification, breaking a no_std embedded compilation target.
> Walks through analyzing cargo tree dependency inversions, activating Cargo Resolver v2 or v3, and decoupling shared workspace dependencies.
> Demonstrates how workspace-wide feature unification can silently invalidate target-specific constraints.
> Provides complete workspace reproduction files and verified remediation commands.

## Related

- Prerequisites: [[Rust-Cargo-Model-Packages-Targets-Features-and-Workspaces]]
- Next: [[Drill-Packaging-PubGrub-Conflict-Diagnosis]]
- Lab: `code/07-Packaging-and-Release/cargo_feature_hazard/`
