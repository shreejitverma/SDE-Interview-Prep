---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Andrey Mokhov, Neil Mitchell, Simon Peyton Jones, Build Systems a la Carte: Theory and Practice, Journal of Functional Programming, Vol. 30, e11, 2020, https://doi.org/10.1017/S0956796820000088"
  - "Kitware, CMake Reference Documentation Manuals, v4.4.4, 2026, https://cmake.org/cmake/help/latest/"
  - "The Bazel Authors, Bazel Documentation, Bazel 9.3.1 (prerelease 10.0.0rc2), 2026, https://bazel.build/docs"
  - "Meta Open Source, Buck2 Documentation, 2026, https://buck2.build/docs/"
---

# Build Systems Skill Ladder

This skill ladder establishes observable, testable engineering behaviors for build systems from junior individual contributor (L4) through distinguished engineer and systems architect (L8+).

## L4

- Can write and maintain target-centric CMake configurations using PRIVATE, PUBLIC, and INTERFACE scopes without global directory-level mutations ([[Modern-CMake-Architecture-and-Targets]]).
- Can configure build environments using standard CMake presets for debug, release, and testing configurations ([[CMake-Presets-Toolchains-and-File-API]]).
- Can inspect and debug parallel compilation failures and job execution bottlenecks in Ninja build graphs ([[Make-and-Ninja-Mechanics]]).
- Can configure and inspect local compiler caches such as ccache and sccache to verify cache hits ([[Compiler-Caches-ccache-and-sccache]]).
- Can configure build targets in Meson using declare_dependency and manage subproject wraps ([[Meson-Build-System-and-WrapDB]]).

## L5

- Can author custom build rules with exact dynamic dependency depfiles that rebuild reliably without race conditions under high core counts ([[Make-and-Ninja-Mechanics]], [[Drill-Build-Systems-Fix-Broken-Incremental-Depfile]]).
- Can configure and debug cross-compilation toolchains for multiple architectures using hermetic sysroots ([[Cross-Compilation-and-Sysroots]]).
- Can configure C++20 named module builds with pre-scanning and dynamic BMI compilation in CMake and Ninja ([[Cpp-Modules-in-Build-Systems]]).
- Can analyze and optimize Cargo build graphs using compilation timing profiles and pipelining ([[Cargo-Build-Graph-and-Rustc-Pipelining]]).
- Can author Starlark rules, providers, and aspects in Bazel or Buck2 for multi-step artifact generation ([[Bazel-Skyframe-Bzlmod-and-Execution-Model]], [[Buck2-DICE-Starlark-and-BXL]]).
- Can audit and eliminate source-level non-determinism like embedded timestamps to produce reproducible object files ([[Hermetic-and-Reproducible-Builds]], [[Drill-Build-Systems-Hunt-Non-Determinism-with-Diffoscope]]).

## L6

- Can trace and diagnose root causes of remote cache misses across distributed Bazel action caches using execution logs and diffoscope within 15 minutes ([[Diagnose-Bazel-and-Buck2-Cache-Misses]], [[Drill-Build-Systems-Hunt-Non-Determinism-with-Diffoscope]]).
- Can profile critical-path build bottlenecks using execution traces and refactor target dependency graphs to cut wall-clock latency by more than 50% ([[Build-Performance-Economics-and-Decision-Record]], [[Drill-Build-Systems-Break-a-Dependency-Bottleneck]]).
- Can implement a working content-hashed build system prototype in Python or Rust with dynamic task graphs and early cutoff ([[Rebuild-Content-Hashed-Build-System-Lab]]).
- Can debug execution failures caused by OS sandboxing restrictions across Linux namespaces and macOS Seatbelt ([[Sandboxing-and-Hermetic-Execution-Internals]]).
- Can author BXL introspection scripts in Buck2 to query transitive dependencies and generate IDE project schemas dynamically ([[Buck2-DICE-Starlark-and-BXL]]).

## L7

- Can design and deploy a distributed remote execution and caching cluster utilizing the Remote Execution API v2 with Build without the Bytes optimization ([[Remote-Execution-API-and-Remote-Caching]]).
- Can architect an incremental migration strategy to transition a multi-million line polyglot codebase to Bazel or Buck2 without disrupting developer velocity ([[Migrate-a-Polyglot-Codebase-to-Hermetic-Builds]]).
- Can evaluate trade-offs and author an organizational decision record choosing between CMake, Meson, Bazel, and Buck2 based on team topology and graph scale ([[Build-Performance-Economics-and-Decision-Record]]).
- Can optimize Skyframe and DICE evaluation memory footprint and analysis-phase latency on 100,000-target enterprise build graphs ([[Bazel-Skyframe-Bzlmod-and-Execution-Model]], [[Buck2-DICE-Starlark-and-BXL]]).

## L8+

- Can formulate company-wide build infrastructure strategy balancing local developer hardware investment against cloud remote execution farm operational expenditure ([[Build-Performance-Economics-and-Decision-Record]]).
- Can design novel build engine abstractions that unify language-specific package resolution, monadic runtime actions, and virtualized filesystem materialization ([[Build-System-Theory-Schedulers-and-Rebuilders]], [[Remote-Execution-API-and-Remote-Caching]]).
- Can establish organizational bit-for-bit reproducible build requirements and independent verification attestations for regulated production software ([[Hermetic-and-Reproducible-Builds]]).
- Can resolve industry-level ecosystem impedance mismatches when adopting next-generation compiler features like C++20 modules across heterogeneous build tools ([[Cpp-Modules-in-Build-Systems]]).
