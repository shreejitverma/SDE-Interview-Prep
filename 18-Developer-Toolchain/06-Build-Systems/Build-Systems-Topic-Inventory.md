---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Andrey Mokhov, Neil Mitchell, Simon Peyton Jones, Build Systems a la Carte: Theory and Practice, Journal of Functional Programming, Vol. 30, e11, 2020, https://doi.org/10.1017/S0956796820000088"
  - "Kitware, CMake Reference Documentation Manuals, v4.4.4, 2026, https://cmake.org/cmake/help/v4.4/"
  - "Evan Martin, The Ninja Build System Manual, v1.13.2, 2026, https://ninja-build.org/manual.html"
  - "Jussi Pakkanen et al., The Meson Build System User Manual, v1.12.1, 2026, https://mesonbuild.com/Manual.html"
  - "The Bazel Authors, Bazel Documentation, Bazel 9.3.1 (prerelease 10.0.0rc2), 2026, https://bazel.build/docs"
  - "Meta Open Source, Buck2 Documentation, 2026, https://buck2.build/docs/"
  - "Bazel Remote APIs Working Group, Remote Execution API, v2.x, 2026, https://github.com/bazelbuild/remote-apis"
  - "Joel Rosdahl, Ccache Manual, v4.14.2, 2026, https://ccache.dev/manual/latest.html"
  - "Mozilla, Sccache Documentation, v0.18.0, 2026, https://github.com/mozilla/sccache"
  - "Reproducible Builds Project, Reproducible Builds Documentation, 2026, https://reproducible-builds.org/docs/"
---

# Build Systems Topic Inventory

This inventory maps the authoritative documentation, formal build theory, execution engine specifications, and remote caching APIs for build systems to notes in 18-Developer-Toolchain.

| Source heading group | Owning note | Depth target | Existing vault coverage |
| :--- | :--- | :--- | :--- |
| Build Systems a la Carte: Abstract Model, Keys, Static vs Dynamic Tasks, Minimality | [[Build-System-Theory-Schedulers-and-Rebuilders]] | Explain, Rebuild | none |
| Build Systems a la Carte: Schedulers (Topological, Restarting, Suspending) | [[Build-System-Theory-Schedulers-and-Rebuilders]] | Explain, Rebuild | none |
| Build Systems a la Carte: Rebuilders (Dirty Bit, Verifying Traces, Constructive Traces) | [[Build-System-Theory-Schedulers-and-Rebuilders]], [[10-Papers/Paper-Build-Systems-a-la-Carte\|Paper-Build-Systems-a-la-Carte]] | Explain, Rebuild | none |
| Reproducible Builds: Deterministic Systems, Variation Taxonomy, Timestamps (`SOURCE_DATE_EPOCH`) | [[Hermetic-and-Reproducible-Builds]] | Explain, Debug, Decide at Scale | [[19-Shell-and-Unix-Engineering/18-Build-Packaging-and-Bootstrap/Nix-Builders-and-stdenv-Phases.md\|Nix-Builders-and-stdenv-Phases]] |
| Reproducible Builds: Path Remapping (`-ffile-prefix-map`), Archive Normalization, Verification Tools | [[Hermetic-and-Reproducible-Builds]], [[Drill-Build-Systems-Hunt-Non-Determinism-with-Diffoscope]] | Explain, Debug | none |
| Ninja Manual: Architecture, Execution, GNU Jobserver Client Support (since 1.13) | [[Make-and-Ninja-Mechanics]] | Use, Explain, Rebuild | [[19-Shell-and-Unix-Engineering/18-Build-Packaging-and-Bootstrap/Make-Recipe-Execution.md\|Make-Recipe-Execution]] |
| Ninja Manual: Build Log (`.ninja_log`), Binary Deps Log (`.ninja_deps`), `depfile`, Pools, `dyndep` | [[Make-and-Ninja-Mechanics]], [[Cpp-Modules-in-Build-Systems]], [[Drill-Build-Systems-Fix-Broken-Incremental-Depfile]] | Explain, Debug, Rebuild | none |
| CMake Reference: `cmake-buildsystem(7)`, Targets, Properties, Usage Requirements | [[Modern-CMake-Architecture-and-Targets]] | Use, Explain, Debug | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Appendices/Appendix_T_Appendix_T_THE_MASTERS_GUIDE_TO_CMAKE.md\|Appendix_T_THE_MASTERS_GUIDE_TO_CMAKE]] |
| CMake Reference: `cmake-generator-expressions(7)`, Generator Expressions, Compile Features | [[Modern-CMake-Architecture-and-Targets]] | Use, Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_110_Build_Systems_and_Tooling/Chapter_110_Build_Systems_Dependency_Management_and_Tooling.md\|Chapter_110_Build_Systems]] |
| CMake Reference: `cmake-presets(7)`, `cmake-toolchains(7)`, `cmake-file-api(7)`, `cmake-configure-log(7)` | [[CMake-Presets-Toolchains-and-File-API]] | Use, Explain | none |
| CMake Reference: `cmake-cxxmodules(7)`, C++20 Named Modules Compilation | [[Cpp-Modules-in-Build-Systems]] | Explain, Debug | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/manuscript/10-part-compilation/39_build_systems_and_modules.md\|39_build_systems_and_modules]] |
| Meson Manual: Target Declarations, `declare_dependency()`, WrapDB Subprojects | [[Meson-Build-System-and-WrapDB]] | Use, Explain | [[19-Shell-and-Unix-Engineering/18-Build-Packaging-and-Bootstrap/Autoconf-and-configure.md\|Autoconf-and-configure]] |
| Meson Manual: Cross-Compilation (`--cross-file`), Native Files, Toolchain Setup | [[Meson-Build-System-and-WrapDB]], [[Cross-Compilation-and-Sysroots]] | Use, Explain | [[19-Shell-and-Unix-Engineering/18-Build-Packaging-and-Bootstrap/Portable-sh-for-configure.md\|Portable-sh-for-configure]] |
| Bazel Docs: Concepts, Loading, Analysis, Execution Phases, Starlark Rules, Providers | [[Bazel-Skyframe-Bzlmod-and-Execution-Model]] | Use, Explain, Decide at Scale | none |
| Bazel Docs: Skyframe Evaluation Graph, Invalidation, Early Cutoff | [[Bazel-Skyframe-Bzlmod-and-Execution-Model]] | Explain, Decide at Scale | none |
| Bazel Docs: Bzlmod (`MODULE.bazel`), Module Extensions, Central Registry, MVS Resolution | [[Bazel-Skyframe-Bzlmod-and-Execution-Model]] | Use, Explain | none |
| Bazel Docs: Querying (`query`, `cquery`, `aquery`), Persistent Multiplex Workers | [[Bazel-Skyframe-Bzlmod-and-Execution-Model]], [[Diagnose-Bazel-and-Buck2-Cache-Misses]] | Use, Explain, Debug | none |
| Bazel Docs: Sandboxing (`linux-sandbox`, `darwin-sandbox`), Action Roots | [[Sandboxing-and-Hermetic-Execution-Internals]] | Explain, Debug | [[19-Shell-and-Unix-Engineering/22-Shell-and-AI-Agents/Agent-Shell-Sandboxing-Seatbelt-and-Landlock.md\|Agent-Shell-Sandboxing-Seatbelt-and-Landlock]] |
| Buck2 Docs: DICE Incremental Engine, Single-Phase Evaluation, Monadic Rules (`dynamic_output`) | [[Buck2-DICE-Starlark-and-BXL]] | Explain, Decide at Scale | none |
| Buck2 Docs: Buck eXtension Language (BXL), Target Graphs, Critical Path Profiling | [[Buck2-DICE-Starlark-and-BXL]] | Use, Explain, Debug | none |
| Remote Execution API: Services (`ActionCache`, `ContentAddressableStorage`, `Execution`) | [[Remote-Execution-API-and-Remote-Caching]] | Explain, Decide at Scale | none |
| Remote Execution API: Merkle Tree Digests, ByteStream Chunking, Build without the Bytes | [[Remote-Execution-API-and-Remote-Caching]] | Explain, Debug, Decide at Scale | none |
| Ccache Manual: Direct Mode vs Preprocessor Mode, Inode Cache, Base Directory Normalization | [[Compiler-Caches-ccache-and-sccache]] | Use, Explain, Debug | none |
| Sccache Docs: Client-Server Daemon, Cloud Storage Backends, Rust Crate Granularity | [[Compiler-Caches-ccache-and-sccache]] | Use, Explain, Debug | none |
| Build Toolchain: Cross-Compilation, Target Triples, Hermetic Sysroot Construction | [[Cross-Compilation-and-Sysroots]] | Use, Explain, Debug | none |
| Build Toolchain: Cargo Unit Graphs, rustc Pipelining (`.rmeta`), Build Scripts | [[Cargo-Build-Graph-and-Rustc-Pipelining]] | Explain, Debug, Decide at Scale | none |
| Build Engineering Economics: Critical Path Analysis, Machine ROI, Selection Matrix | [[Build-Performance-Economics-and-Decision-Record]], [[Drill-Build-Systems-Break-a-Dependency-Bottleneck]] | Debug, Decide at Scale | none |
| Build Engineering Runbooks: Remote Cache Miss Root-Cause Analysis, Action Diffing | [[Diagnose-Bazel-and-Buck2-Cache-Misses]] | Debug, Decide at Scale | none |
| Build Engineering Runbooks: Incremental Polyglot Migration to Hermetic Builds | [[Migrate-a-Polyglot-Codebase-to-Hermetic-Builds]] | Decide at Scale | none |
| Build Systems Rebuild Lab: Content-Hashed Task Graph, Coroutine Schedulers, Trace Cache | [[Rebuild-Content-Hashed-Build-System-Lab]] | Rebuild | none |

## Out of scope

- CMake Qt moc integration (`cmake-qt(7)`), removed `cmake-server(7)` (dropped in CMake 3.20), and GUI dialogs (`cmake-gui(1)`) are out of scope because framework-specific Qt code generation rules and deprecated GUI interfaces are outside general toolchain architecture.
- Meson language-specific helper modules (`Java`, `Vala`, `Hotdoc`, `i18n`, `Windows`) are out of scope because language-specific Meson helper modules are outside the core C++, Python, and Rust toolchain focus; Python build packaging (`meson-python`) is owned by 07-Packaging-and-Release.
