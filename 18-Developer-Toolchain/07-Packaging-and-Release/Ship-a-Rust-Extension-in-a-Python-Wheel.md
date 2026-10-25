---
type: playbook
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Ship a Rust Extension in a Multi-Platform Python Wheel

> [!summary] Scope
> End-to-end execution guide for compiling, testing, repairing, and publishing a hybrid PyO3 Rust extension as a multi-platform wheel.
> Walks through CI workflow definition, cibuildwheel and maturin invocation, auditwheel repair, free-threaded cp314t target compilation, and Sigstore attestation.
> Provides complete configuration files, build command recipes, and automated verification steps.
> Covers edge cases in dynamic library linking and cross-compilation toolchain setup.

## Related

- Prerequisites: [[Rust-PyO3-and-Maturin-Internals]], [[Python-Cross-Platform-Wheel-Pipelines-with-cibuildwheel]], [[Python-Trusted-Publishing-and-PEP-740-Attestations]]
- Next: [[Execute-Zero-Downtime-Shared-Library-SONAME-Migration]]
- Lab: `code/07-Packaging-and-Release/maturin_wheel/` (requires: docker, network)
