# Chapter 110: Build Systems, Dependency Management, and Tooling

C++ has no built-in package manager and no canonical build system, so for decades "how do I build and depend on other code" was the language's most painful problem — and getting it wrong costs hours of every engineer's day in slow, non-reproducible, or broken builds. This chapter covers the modern answer: target-based CMake, reproducible builds with Bazel, dependency management with vcpkg and Conan, and the tooling (sanitizers, static analysis, compilation databases) that turns a C++ codebase from a fragile pile of source files into an engineered system.

## Chapter Roadmap

- 110.1 The C++ Build Problem
- 110.2 The Build Process and Make
- 110.3 Modern CMake: Targets and Usage Requirements
- 110.4 Bazel and Hermetic, Reproducible Builds
- 110.5 Dependency Management: vcpkg and Conan
- 110.6 Common Linker Errors
- 110.7 Tooling: Sanitizers, Static Analysis, PCH, and Compilation Databases

---

## 110.1 The C++ Build Problem

A build system is the **master contractor** of a C++ project: given the source files and their dependencies, it invokes the compiler and linker in the correct order, rebuilds only what changed, and produces the final artefacts. Unlike languages with one blessed toolchain (Cargo for Rust, npm for Node), C++ has *many* build systems and *no* standard dependency manager, so a project's build is itself a significant engineering artefact.

> **Why this matters.** The build is not incidental — it is where correctness (Chapter 104's ODR and ABI hazards live in build flags), performance (Chapter 102's LTO/PGO are build configuration), and developer velocity (incremental build time) are decided. A poorly-structured build recompiles the world on every change, mixes incompatible flags into ODR violations, or cannot reproduce a release binary. The modern tooling in this chapter exists to make builds *fast* (incremental, parallel, cached), *correct* (consistent flags, declared dependencies), and *reproducible* (the same inputs always produce the same binary).

---

## 110.2 The Build Process and Make

Conceptually, building is: **preprocess → compile each TU to an object file → link the objects (and libraries) into an executable** (Chapter 102). **Make** is the foundational build tool: a `Makefile` declares *targets*, their *prerequisites*, and the *recipe* to build each, and `make` rebuilds a target only if a prerequisite is newer.

```bash
# Min: any Make. A minimal Makefile — the dependency-and-recipe model.
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
app: main.o util.o          # target : prerequisites
	$(CXX) main.o util.o -o app          # recipe (must be TAB-indented)
main.o: main.cpp util.h
	$(CXX) $(CXXFLAGS) -c main.cpp
util.o: util.cpp util.h
	$(CXX) $(CXXFLAGS) -c util.cpp
```
*Listing 110.1 — A Makefile's target/prerequisite/recipe model: the basis of all build systems.*

> **Why this matters.** Make's model — a dependency graph where a node is rebuilt only when its inputs change — is the conceptual core of *every* build system, including the modern ones that hide it. Understanding it explains incremental builds (only changed TUs and their dependents rebuild), why a missing header dependency causes stale builds (the graph is incomplete), and why parallel builds work (`make -j` builds independent nodes concurrently). But hand-written Makefiles do not scale: they require manual dependency tracking, are platform-specific, and do not handle finding libraries — which is why modern projects use a *generator* (CMake) on top.

---

## 110.3 Modern CMake: Targets and Usage Requirements

**CMake** is the de-facto standard, but it is a **build-system generator**, not a build system: it reads `CMakeLists.txt` and generates the actual build files for Make, Ninja, Visual Studio, or Xcode. Modern ("target-based") CMake treats everything as a **target** with **usage requirements** that propagate to consumers.

```cmake
# Min: CMake 3.x. Target-based ("modern") CMake.
add_library(Network src/net.cpp)
target_include_directories(Network PUBLIC  include/)      # consumers also get include/
target_compile_definitions(Network PRIVATE USE_AVX=1)     # only Network sees USE_AVX
target_link_libraries(Network PUBLIC Threads::Threads)    # consumers also link Threads

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE Network)                # app inherits Network's PUBLIC requirements
```
*Listing 110.2 — Modern CMake: usage requirements (`PUBLIC`/`PRIVATE`/`INTERFACE`) propagate transitively.*

The three visibility keywords are the heart of modern CMake:

- **`PUBLIC`** — "I need this, *and* anyone who links me needs it too" (propagates to consumers).
- **`PRIVATE`** — "I use this internally; do not propagate it" (hidden from consumers).
- **`INTERFACE`** — "I don't use it, but my consumers must" (for header-only libraries).

> **Why this matters.** The pre-modern CMake style used global commands (`include_directories`, `add_definitions`) that polluted *every* target with *every* setting — the source of the inconsistent-flags ODR/ABI bugs of Chapter 104. Target-based CMake makes each target declare exactly what it needs and what it exposes, and CMake propagates those requirements transitively and consistently. This is *correctness* tooling: it ensures a consumer of `Network` automatically gets `Network`'s include paths and required libraries with the *same* flags, eliminating a whole class of "works on my machine" build bugs. Modern CMake is the single most important build skill for a C++ engineer.

---

## 110.4 Bazel and Hermetic, Reproducible Builds

**Bazel** (Google) takes a stricter approach: builds are **hermetic** — they depend *only* on explicitly-declared inputs, isolated from the host environment — so the same source produces the *exact* same binary on any machine. It is built for monorepos and aggressive caching.

> **Why this matters / cost model.** Hermeticity buys two things that matter at scale. First, **reproducibility**: if your build is hermetic, a release binary can be rebuilt bit-for-bit from its source revision, which is essential for debugging production incidents (you can build the *exact* binary that failed) and for supply-chain security (you can verify the binary matches the source). Second, **caching**: because Bazel knows every input to every action, it can cache build outputs across machines and developers — if anyone has built a target with identical inputs, everyone gets the cached result, making huge monorepo builds fast. The cost is rigidity (every dependency must be declared; "just use a system library" is forbidden) and a steeper learning curve. Bazel is used by Google and many HFT firms precisely because reproducibility and cross-machine caching are worth that rigidity at large scale; for smaller projects CMake's flexibility usually wins.

---

## 110.5 Dependency Management: vcpkg and Conan

Because C++ has no built-in `npm`/`pip`, for thirty years dependencies meant manually downloading and building `.zip` files. Two package managers now fill the gap:

- **vcpkg** (Microsoft) — source-based, simple, integrates tightly with CMake and Visual Studio. `vcpkg install openssl` fetches, builds, and exposes a library.
- **Conan** (JFrog) — Python-based, decentralised, and strong at distributing *pre-compiled binary* packages with full version/configuration resolution — better suited to large enterprises with many platform/compiler combinations.

```bash
# Min: vcpkg / Conan. Declarative dependency acquisition.
vcpkg install fmt:x64-linux            # vcpkg: source-based, CMake-integrated
# conan install . --build=missing      # Conan: resolves a dependency graph, can fetch prebuilt binaries
```
*Listing 110.3 — A package manager turns "download and build a dependency by hand" into one declarative command.*

> **Why this matters.** Manual dependency management is a correctness and security hazard: hand-built dependencies drift in version and flags (ABI mismatches, Chapter 102), are not reproducible, and are not auditable for vulnerabilities. A package manager makes dependencies *declarative* (listed in a manifest, versioned, reproducible) and *consistent* (built with compatible flags, resolved as a coherent graph). The choice between vcpkg and Conan is largely ecosystem fit — vcpkg for CMake-centric projects wanting simplicity, Conan for enterprises needing binary distribution and complex version resolution. The key advance is that "what does this project depend on" is now a *file in the repository*, not tribal knowledge.

---

## 110.6 Common Linker Errors

The two errors every C++ engineer must diagnose instantly, both rooted in the ODR (Chapter 104):

- **`undefined reference to 'X'`** — a symbol was *declared and used* but never *defined*, or its definition was not linked. Causes: a missing source file in the build, an unlinked library, a signature mismatch (so the mangled names differ, Chapter 102), or a missing `extern "C"` when mixing C and C++.
- **`multiple definition of 'X'`** — two translation units both provided a *non-inline* definition of the same symbol. Causes: a function or variable defined in a header without `inline`, or a definition (not just declaration) in a header included by multiple TUs.

> **Why this matters.** These are the build system surfacing the ODR: every entity needs exactly one definition (Chapter 104). `undefined reference` means *zero* definitions reached the linker; `multiple definition` means *more than one* did. The fixes follow directly: for `undefined reference`, link the missing library/object or fix the declaration to match the definition; for `multiple definition`, mark header-defined functions `inline` (or move the definition to one `.cpp`), and use `inline` variables (C++17) for header constants. Reading the *mangled* name in the error (and demangling with `c++filt`) tells you the exact signature the linker wanted — often revealing a `const` or namespace mismatch.

---

## 110.7 Tooling: Sanitizers, Static Analysis, PCH, and Compilation Databases

A modern C++ build integrates a toolchain beyond the compiler:

- **Sanitizers** (ASan, TSan, UBSan, MSan — Chapter 105) are compiler flags (`-fsanitize=...`) wired into CI builds to catch memory errors, races, and UB at runtime.
- **Static analysers** (clang-tidy, cppcheck, the compiler's own `-Wall -Wextra -Werror`) catch bugs *without running* the code — uninitialised variables, suspicious casts, style violations.
- **Precompiled headers (PCH)** compile a set of common, rarely-changing headers once and reuse the result, cutting compile time on header-heavy codebases.
- **Compilation databases** (`compile_commands.json`, emitted by CMake with `CMAKE_EXPORT_COMPILE_COMMANDS=ON`) record the exact compile command for each TU, enabling editor tooling (clangd), clang-tidy, and refactoring tools to understand the code exactly as the compiler does.

```bash
# Min: GCC/Clang + CMake. The tooling integration points.
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..    # emit compile_commands.json for clangd/clang-tidy
g++ -std=c++20 -Wall -Wextra -Werror -fsanitize=address,undefined -c file.cpp   # warnings-as-errors + sanitizers
clang-tidy file.cpp                            # static analysis using the compilation database
```
*Listing 110.4 — Wiring sanitizers, warnings-as-errors, and a compilation database into the build.*

> **Why this matters.** The build system is where the *quality gates* live. Warnings-as-errors (`-Werror`) turns the compiler's static analysis into a hard requirement, catching bugs at compile time (Chapter 104). Sanitizers in CI catch the runtime UB and races that tests miss (Chapter 105). The compilation database is what makes editor intelligence (go-to-definition, accurate autocomplete, inline diagnostics via clangd) and automated refactoring work, because those tools need the *exact* flags each file was compiled with. Together these turn the build from a mere "make it compile" step into the project's primary defence against the bug classes this book has catalogued — wired in once, enforced on every commit.

> **The discipline.** A C++ project's build and tooling are not overhead; they are infrastructure that determines correctness, performance, and velocity. The modern baseline: target-based CMake (or Bazel for monorepo reproducibility), a declarative package manager (vcpkg/Conan) so dependencies are versioned and auditable, and a tooling pipeline — warnings-as-errors, sanitizers in CI, a compilation database for editor and analysis tools — that enforces quality on every build. Invest in the build early; a project that builds fast, reproducibly, and with the quality gates wired in pays that investment back every single day. The remaining chapters turn to the domains where C++ is deployed, beginning with networking.
