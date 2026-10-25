---
type: moc
track: [sde, distinguished, low-latency, quant-dev]
level:
status: draft
last_reviewed:
sources:
  - "Semantic Versioning Specification, SemVer 2.0.0, SemVer.org, 2013, https://semver.org/"
  - "The Rust Project Developers, The Cargo Book (rustc/cargo 1.99.1), Rust Foundation, 2026, https://doc.rust-lang.org/cargo/"
  - "Python Packaging Authority, Python Packaging User Guide (PyPUG), PyPA, 2026, https://packaging.python.org/en/latest/"
  - "Kitware, cmake-packages(7) Manual, Kitware Inc., 2026, https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html"
  - "Debian Policy Group, Debian Policy Manual - Chapter 8 Shared Libraries, Software in the Public Interest, 2026, https://www.debian.org/doc/debian-policy/ch-sharedlibs.html"
---

# Packaging and Release Engineering Skill Ladder

Five-tier competency framework defining observable engineering behaviors from L4 to L8+ in packaging, binary interface governance, and release engineering.

## L4

- Authors valid declarative project metadata in `pyproject.toml` or `Cargo.toml` without relying on deprecated setup scripts: [[Python-Packaging-History-and-pyproject-toml]], [[Rust-Cargo-Model-Packages-Targets-Features-and-Workspaces]].
- Generates and commits deterministic lockfiles in application repositories to ensure bit-identical dependency resolution across team environments: [[Lockfiles-Vendoring-and-Offline-Mirrors]], [[Python-Dependency-Resolution-pip-uv-and-PEP-751-Lockfiles]].
- Builds standard Python source distributions and pure-Python wheels using modern isolated build frontends: [[Python-Packaging-History-and-pyproject-toml]], [[Python-Wheels-Platform-Tags-Manylinux-and-ABI3]].
- Configures multi-crate Cargo workspaces with shared dependency inheritance and target-specific dependencies: [[Rust-Cargo-Model-Packages-Targets-Features-and-Workspaces]].
- Inspects basic dynamic library dependencies on Linux using `ldd` and `readelf` to identify missing runtime shared objects: [[Cpp-ABI-Stability-Symbol-Visibility-and-SONAME]].

## L5

- Triages and resolves dependency resolution conflicts and backtracking limits using PubGrub error derivation trees: [[Dependency-Resolution-SAT-PubGrub-and-MVS]], [[Drill-Packaging-PubGrub-Conflict-Diagnosis]].
- Builds and repairs binary Python wheels targeting the `manylinux_2_28` baseline using `cibuildwheel` and `auditwheel`: [[Python-Wheels-Platform-Tags-Manylinux-and-ABI3]], [[Python-Cross-Platform-Wheel-Pipelines-with-cibuildwheel]], [[Drill-Packaging-Manylinux-GLIBC-Symbol-Leak]].
- Authors relocatable CMake package configuration files using modern target export sets and installation rules: [[Cpp-CMake-Package-Configs-pkg-config-and-CPS]].
- Diagnoses and eliminates non-additive feature unification hazards across Cargo workspace crates and embedded targets: [[Rust-Cargo-Model-Packages-Targets-Features-and-Workspaces]], [[Drill-Packaging-Cargo-Feature-Unification-Hazard]].
- Enforces Minimum Supported Rust Version constraints and automated SemVer breaking change detection in CI pipelines: [[Rust-SemVer-Checks-MSRV-and-Editions]].
- Configures containerized multi-stage OCI builds using distroless runtime bases to minimize production image footprint: [[OCI-Images-Layers-Multi-Stage-and-Distroless]].

## L6

- Authors native extension modules using modern backends such as `scikit-build-core`, `meson-python`, or `maturin`: [[Python-Native-Build-Backends-scikit-build-core-meson-python-and-maturin]], [[Rust-PyO3-and-Maturin-Internals]], [[Ship-a-Rust-Extension-in-a-Python-Wheel]].
- Configures OIDC Trusted Publishing and cryptographic attestations across PyPI and crates.io release workflows: [[Python-Trusted-Publishing-and-PEP-740-Attestations]], [[Rust-Publishing-Crates-io-Trusted-Publishing-and-Private-Registries]].
- Manages shared library ABI compatibility using default hidden symbol visibility, version scripts, and SONAME tagging: [[Cpp-ABI-Stability-Symbol-Visibility-and-SONAME]], [[Cpp-Glibc-Symbol-Versioning-and-Portable-Linux-Binaries]], [[Drill-Packaging-Cpp-Silent-ABI-Break-Across-Shared-Libraries]].
- Distributes C++ libraries via package managers including Conan 2, vcpkg manifests, or Spack environments: [[Cpp-Conan-2-Architecture-and-Binary-Model]], [[Cpp-vcpkg-Manifests-Registries-and-Binary-Caching]], [[Cpp-Spack-and-Nix-for-Native-Dependencies]].
- Implements automated macOS universal2 binary signing, entitlement configuration, and notarization via `notarytool`: [[Cpp-macOS-Universal-Binaries-Code-Signing-and-Notarization]].
- Packages and exports native Rust components as C-compatible shared or static libraries with automated header generation: [[Rust-FFI-Packaging-cdylib-staticlib-and-cbindgen]].

## L7

- Orchestrates zero-downtime shared library migrations using inline namespaces, dual-SONAME staging, or symbol versioning: [[Execute-Zero-Downtime-Shared-Library-SONAME-Migration]], [[Diamond-Dependency-and-Incompatible-ABI-Mitigation]].
- Designs progressive delivery pipelines with automated canary evaluation, error budget abort triggers, and rollbacks: [[Progressive-Delivery-Rollbacks-and-Feature-Flags]], [[Contain-and-Roll-Back-a-Bad-Production-Release]], [[Case-Study-CrowdStrike-Falcon-Channel-File-291]].
- Solves cross-language diamond dependency conflicts using linker isolation flags, symbol mangling, and Cargo multi-version linking: [[Diamond-Dependency-and-Incompatible-ABI-Mitigation]].
- Authors distribution packages for free-threaded CPython and specialized hardware-accelerated wheel variants: [[Python-Free-Threaded-Wheels-and-Wheel-Variants]].
- Establishes enterprise release train schedules, backport automation, and changelog generation across distributed service fleets: [[Release-Trains-Backports-and-Artifact-Registries]].
- Formulates architectural decision records evaluating package managers, binary models, and versioning schemes: [[Packaging-Decision-Records]], [[Case-Study-Knight-Capital-Deployment-Failure]].

## L8+

- Defines organization-wide ABI governance and binary compatibility verification standards across thousands of repositories: [[Cpp-ABI-Stability-Symbol-Visibility-and-SONAME]], [[Diamond-Dependency-and-Incompatible-ABI-Mitigation]].
- Architects multi-region artifact registry topologies with immutable retention, regional caching proxies, and air-gapped mirrors: [[Lockfiles-Vendoring-and-Offline-Mirrors]], [[Case-Study-Left-Pad-and-Registry-Immutability]].
- Establishes enterprise release engineering guardrails that separate code deployment from runtime feature activation to prevent fleet-wide outages: [[Progressive-Delivery-Rollbacks-and-Feature-Flags]], [[Case-Study-CrowdStrike-Falcon-Channel-File-291]], [[Case-Study-Knight-Capital-Deployment-Failure]].
- Authors industry-standard packaging specifications or contributes directly to toolchain package resolution backends: [[Packaging-Decision-Records]], [[Dependency-Resolution-SAT-PubGrub-and-MVS]].
