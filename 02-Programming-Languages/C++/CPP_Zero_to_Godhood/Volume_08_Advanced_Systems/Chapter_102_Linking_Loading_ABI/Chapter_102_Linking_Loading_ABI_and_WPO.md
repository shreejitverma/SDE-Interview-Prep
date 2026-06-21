# Chapter 102: Linking, Loading, ABI, and Whole-Program Optimisation

The compiler sees one translation unit at a time; the **linker** assembles them into a program, the **loader** maps that program into memory at run time, and the **ABI** is the binary contract that lets independently-compiled pieces interoperate. This layer is invisible until it isn't — an undefined-reference error, a silent ODR violation, a slow startup from relocations, or a failed cross-TU inline that left an abstraction non-free. This chapter explains the linking and loading model, symbol visibility and ABI stability, and the whole-program optimisations (LTO, PGO) that recover the cross-TU performance the separate-compilation model otherwise forfeits.

## Chapter Roadmap

- 102.1 The Compilation and Linking Model
- 102.2 Symbol Visibility and Name Mangling
- 102.3 Static vs Dynamic Linking and the Loader
- 102.4 PIC, PIE, and Relocations
- 102.5 The ABI and Its Stability
- 102.6 Whole-Program Optimisation: LTO and PGO
- 102.7 Build Hazards and the Discipline

---

## 102.1 The Compilation and Linking Model

C++ uses **separate compilation**: each `.cpp` (translation unit) is compiled independently to an object file containing machine code plus a symbol table — *defined* symbols (functions/variables it provides) and *undefined* symbols (those it references but does not define). The **linker** then resolves every undefined symbol against some object's definition, merges the sections, and produces an executable or library.

```bash
# Min: any toolchain. The two phases.
g++ -c a.cpp -o a.o      # compile: a.o has defined+undefined symbols
g++ -c b.cpp -o b.o
g++ a.o b.o -o app       # link: resolve undefined symbols across objects
```
*Listing 102.1 — Separate compilation then linking. The compiler never sees more than one TU; the linker sees them all (but only their object code).*

> **Why this matters.** Separate compilation is why C++ builds scale (only changed TUs recompile) and why two classic errors exist. **`undefined reference to X`** means a symbol was *declared and used* but never *defined* (a missing source file, an unlinked library, or a definition that didn't match the declaration's mangled name). **`multiple definition of X`** means two TUs both provided a non-inline definition — an ODR violation the linker caught (Chapter 104). It is also why the optimizer, working per-TU, cannot inline across TU boundaries by default (§102.6) — the compiler simply does not have the other TU's body when compiling one. The whole linking layer is the consequence of the compiler's deliberately narrow, per-TU view.

---

## 102.2 Symbol Visibility and Name Mangling

C++ **mangles** names — encoding the function signature (namespaces, parameter types, const-ness) into the linker symbol — so that overloaded functions get distinct symbols and the linker can enforce type-safe linkage. `extern "C"` disables mangling for a function so it can interoperate with C (which has no overloading).

```cpp
// Min standard: C++98. Mangling and its suppression.
void f(int);        // mangles to e.g. _Z1fi
void f(double);     // mangles to e.g. _Z1fd  — distinct symbol, hence overloading works

extern "C" void c_api(int);   // NOT mangled: symbol is literally "c_api" — C-compatible, no overloading
```
*Listing 102.2 — Mangling distinguishes overloads; `extern "C"` produces a plain C symbol.*

**Symbol visibility** controls which symbols a shared library *exports*. By default (on many toolchains) all symbols are exported; marking them `__attribute__((visibility("hidden")))` (or compiling with `-fvisibility=hidden` and explicitly exporting an API) keeps internal symbols private.

> **Why this matters / cost model.** Mangling is why a C++ symbol's "undefined reference" sometimes shows a cryptic `_ZN...` name — and why mismatched declarations (different signatures in two TUs) fail to link rather than silently misbehaving. Visibility has a real performance and correctness payoff: a shared library that exports *every* symbol has a huge dynamic symbol table (slow load, slow relocation — §102.4), risks ODR clashes between libraries, and prevents the optimizer from inlining or eliding "exported" functions (it must assume they're called externally). Hiding internal symbols (`-fvisibility=hidden` + explicit `[[gnu::visibility("default")]]` on the public API) shrinks the table, speeds loading, and unlocks more optimization — standard practice for well-engineered libraries.

---

## 102.3 Static vs Dynamic Linking and the Loader

A library can be linked **statically** (its code is copied into the executable at link time) or **dynamically** (the executable records a *dependency*, and the **loader** maps the shared library — `.so`/`.dll`/`.dylib` — into memory at run time, resolving symbols then).

| | Static | Dynamic |
|---|---|---|
| Binary size | Larger (code copied in) | Smaller (shared) |
| Startup | Fast (no resolution) | Slower (load + relocate + resolve) |
| Memory across processes | Duplicated | Shared (one copy of the `.so`) |
| Deployment | Self-contained | Needs the libs present/compatible |
| Updates | Recompile to update | Swap the `.so` |

> **Why this matters / cost model.** The dynamic loader does real work at startup: it maps each `.so`, applies **relocations** (§102.4), and resolves symbols (by default *lazily* on first call via the PLT, or eagerly with `-z now`). For a program with many shared-library dependencies, this is measurable startup latency, and the indirect calls through the PLT have a tiny per-call cost. Static linking eliminates all of it — no load-time resolution, direct calls, and the optimizer/LTO can see the library code — which is why latency-critical and container-deployed binaries often link statically (or mostly so). The trade-off is binary size, the loss of shared memory across processes, and the inability to patch a library without rebuilding. Choose static for self-contained, fast-start, optimizable binaries; dynamic for memory-shared, independently-updatable system components.

---

## 102.4 PIC, PIE, and Relocations

Shared libraries (and, by default on modern systems, executables — **PIE**, position-independent executables, for ASLR security) are compiled as **position-independent code (PIC)**: code that runs correctly regardless of the address it is loaded at, by accessing globals indirectly through the **GOT** (Global Offset Table) and calling exported functions through the **PLT** (Procedure Linkage Table). **Relocations** are the fix-ups the loader applies to make these tables point at the right runtime addresses.

> **Why this matters / cost model.** PIC/PIE are a small but real tax: a global access becomes a GOT indirection rather than a direct address, and an inter-module call goes through the PLT (an extra indirect jump, resolved lazily on first use). For most code this is negligible; for a hot inner loop touching position-independent globals it can matter, and it is one reason `-fno-plt` and static linking are used in latency-sensitive builds. The far larger cost is at *load time*: a binary with many relocations (large dynamic symbol tables, many shared libraries) spends measurable time in the loader applying them before `main` runs. Reducing exported symbols (§102.2), preferring static linking, and using `-z now` vs lazy binding are the levers. ASLR (which PIE enables) is a security feature; weigh disabling it only with full awareness of the trade-off.

---

## 102.5 The ABI and Its Stability

The **Application Binary Interface (ABI)** is the binary-level contract between separately-compiled components: how arguments are passed (registers/stack — the calling convention), how names are mangled, how classes are laid out (member offsets, vtable layout), and how exceptions propagate. Two pieces of code interoperate only if they agree on the ABI.

> **Why this matters.** ABI stability is what lets you link a library built by a different compiler version, or load a plugin without recompiling your host — but it is also a *cage*. The C++ standard library's ABI has been effectively frozen for years because changing the layout of `std::string` or `std::unordered_map` would break every binary compiled against the old layout; this is why some standard improvements (a faster `std::unordered_map`, a smaller `std::regex`) cannot be adopted without an "ABI break" that the committee and vendors resist. The practical consequences: never mix objects compiled with incompatible ABIs (different `_GLIBCXX_USE_CXX11_ABI` settings is a classic silent-corruption trap), be cautious passing standard-library types across shared-library boundaries you don't control, and design *your own* library ABIs deliberately — use the Pimpl idiom or pure-virtual interfaces and `extern "C"` factory functions to present a stable ABI that hides your implementation's layout. An ABI break is a recompile-the-world event; treat your public ABI as a long-term commitment.

---

## 102.6 Whole-Program Optimisation: LTO and PGO

Separate compilation forfeits cross-TU optimization — the compiler cannot inline a function defined in another TU. **Link-Time Optimization (LTO)** recovers it: the compiler emits its intermediate representation into the object files instead of (or alongside) machine code, and the optimizer runs *again at link time* with the whole program visible, inlining and propagating across TU boundaries.

```bash
# Min: GCC/Clang. Enable LTO and PGO.
g++ -O2 -flto -c a.cpp b.cpp        # emit IR into objects
g++ -O2 -flto a.o b.o -o app        # optimize across TUs at link time

# PGO: instrument, run, rebuild with the profile.
g++ -O2 -fprofile-generate app.cpp -o app && ./app <representative-input>
g++ -O2 -fprofile-use     app.cpp -o app   # hot paths laid out & inlined per real data
```
*Listing 102.3 — LTO enables cross-TU inlining; PGO feeds real execution data back into codegen.*

> **Why this matters / cost model.** LTO is how the "zero-cost abstraction" promise survives the separate-compilation model: without it, a small accessor defined in `util.cpp` and called hotly in `main.cpp` cannot inline, leaving a real call-overhead and an optimization barrier (Chapter 89). LTO makes that call inline across the boundary, often a 5–15% whole-program win for free, plus binary-size reductions from cross-TU dead-code elimination. **PGO** complements it by telling the optimizer which branches and calls are actually hot (from a profiling run), so it lays out the binary for instruction-cache locality and inlines the genuinely-hot callees (Chapter 80). The costs: LTO dramatically increases link time and memory (the optimizer now processes the whole program at once), and PGO requires a *representative* workload — a profile from unrepresentative input can pessimize. Both are release-build optimizations: enable LTO broadly, add PGO for the hottest services where the profiling effort pays off.

---

## 102.7 Build Hazards and the Discipline

| Hazard | Cause | Fix |
|---|---|---|
| `undefined reference` | Used-but-undefined symbol; unlinked lib | Link the library; match declarations |
| `multiple definition` | Non-inline definition in 2 TUs (ODR) | `inline`/`static`; one definition (Chapter 104) |
| Silent ABI mismatch | Mixed ABI flags / compiler versions | Consistent toolchain & flags; stable public ABI |
| Slow startup | Many relocations / exported symbols | `-fvisibility=hidden`; static link; `-z now` |
| Abstraction not free | No cross-TU inlining | Enable LTO |
| Wrong hot-path layout | Optimizer guessing branch bias | PGO with representative input |

> **The discipline.** The linking/loading/ABI layer is where the per-TU compilation model meets the whole-program reality, and mastering it pays off in three ways: *correctness* (understanding mangling, the ODR, and ABI compatibility prevents undefined-reference, multiple-definition, and silent-corruption bugs), *startup latency* (visibility control and static linking reduce loader work), and *steady-state performance* (LTO recovers cross-TU inlining; PGO targets the real hot path). The defaults for a performance-critical binary: hide internal symbols, prefer static linking where deployment allows, enable LTO in release, add PGO for the hottest paths, and treat your public ABI as a deliberate, long-lived contract. With the build understood, the volume's final block turns to the disciplines that keep the running program correct and deterministic — undefined behaviour, sanitizers, and jitter elimination.
