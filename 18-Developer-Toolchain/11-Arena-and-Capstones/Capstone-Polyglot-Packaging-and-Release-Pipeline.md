---
type: playbook
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Capstone: Polyglot Library Packaging and Release Pipeline

> [!summary] Scope
> This capstone playbook details the construction of a production-grade CI/CD release pipeline shipping a core C++ shared library, a Python extension module with PyO3/maturin, and a published Rust crate from a single Git repository.
> It configures Conan 2 and CMake for native distribution, cibuildwheel for manylinux/musllinux and macOS universal wheels, and dry-run cargo publishing with MSRV checks.
> It integrates end-to-end cryptographic provenance: SPDX/CycloneDX SBOM generation, Cosign keyless signing with Rekor transparency, and GitHub Actions OIDC trusted publishing.

## Related

- Prerequisites: [[Python-Cross-Platform-Wheel-Pipelines-with-cibuildwheel]], [[Rust-Publishing-Crates-io-Trusted-Publishing-and-Private-Registries]], [[Cpp-Conan-2-Architecture-and-Binary-Model]], [[Sigstore-Keyless-Signing-Cosign-Fulcio-and-Rekor]], [[SBOM-Standards-SPDX-CycloneDX-and-VEX]]
- Next: [[Capstone-Hermetic-Cached-Monorepo-Build]]
- See also: [[10-Development-Practices/README|README]], [[17-Cloud-AI-Engineering/Volume_08_Security_Identity_and_the_Data_Perimeter/Chapter_05_Software_Supply_Chain_Security_SLSA_and_Sigstore|Chapter_05_Software_Supply_Chain_Security_SLSA_and_Sigstore]]
- Lab: `code/11-Arena-and-Capstones/capstone_polyglot_packaging/` (requires: docker, network)
