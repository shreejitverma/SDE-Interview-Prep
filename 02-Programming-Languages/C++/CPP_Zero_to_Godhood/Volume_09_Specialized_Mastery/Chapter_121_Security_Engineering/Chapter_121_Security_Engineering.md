# Chapter 121: Security Engineering

C++'s power — direct memory access, no runtime safety net — is also its security liability: the majority of critical vulnerabilities in C and C++ software are *memory-safety* bugs (buffer overflows, use-after-free), and the language's performance features (speculative execution, data-dependent timing) open whole categories of side-channel attacks. Writing secure C++ means treating every input as hostile, every allocation as a potential overflow, and every timing difference as a leak. This chapter covers the three pillars: finding bugs before attackers do (fuzzing), the cryptographic pitfalls unique to C++, and the hardware side-channels that defeat naive code.

## Chapter Roadmap

- 121.1 Why C++ Security Is Hard
- 121.2 Memory Safety as the Primary Battlefront
- 121.3 Fuzzing: Finding Bugs Before Attackers
- 121.4 Cryptography and Timing Attacks
- 121.5 Side-Channels: Spectre and Speculative Execution
- 121.6 The Security Discipline

---

## 121.1 Why C++ Security Is Hard

C++ gives you direct control over memory and the hardware, with no bounds checking, no garbage collector, and no runtime that catches mistakes — which is exactly why it is fast, and exactly why it is dangerous. Microsoft and Google have both reported that **~70% of their serious security vulnerabilities are memory-safety issues** in C/C++ code: buffer overflows, use-after-free, type confusion, and uninitialised reads — the undefined behaviours of Chapter 104, weaponised.

> **Why this matters.** The same property that makes C++ suitable for systems — unmediated access to memory and hardware — means a bug is not a caught exception but an *exploitable primitive*: a buffer overflow lets an attacker overwrite a return address, a use-after-free lets them control a freed object's vtable, an uninitialised read leaks secrets. Security in C++ is therefore inseparable from the correctness discipline of Volume 8: every UB (Chapter 104) is a potential vulnerability, and the tools that catch UB (sanitizers, Chapter 105) are security tools. The mindset shift is to treat all external input as adversarial and assume that any memory-safety bug *will* be found and exploited.

---

## 121.2 Memory Safety as the Primary Battlefront

The dominant vulnerability classes are all memory-safety failures, and modern C++ provides the tools to eliminate most of them *by construction*:

- **Buffer overflows** — writing past an array's bounds. Mitigated by `std::span` (C++20, carries bounds), `.at()` (checked access), `std::string`/`std::vector` over raw buffers, and never using `strcpy`/`sprintf` (use bounded forms or `std::format`).
- **Use-after-free / dangling** — accessing freed memory or an out-of-scope object. Mitigated by RAII and smart pointers (`unique_ptr`/`shared_ptr`), avoiding raw owning pointers, and lifetime-aware design (Chapter 97).
- **Integer overflow leading to undersized allocation** — a multiplication overflow yields a too-small buffer. Mitigated by checked arithmetic and validating sizes from untrusted input.
- **Uninitialised reads** — leaking stack/heap contents. Mitigated by always initialising and MSan (Chapter 105).

> **Why this matters.** The strategic point is that *modern C++ can be memory-safe in practice* if you use its safe constructs: a codebase that uses `std::span`/`std::vector`/`std::string` instead of raw pointers and lengths, RAII instead of manual `new`/`delete`, and `.at()` or bounds-checked spans on untrusted indices eliminates the *majority* of the exploitable bug classes. This is why the industry push (and government guidance) toward "memory-safe" patterns matters: the bugs are not inevitable, they are the result of using the *unsafe* subset. Where you must use raw memory (the hot path, FFI), wrap it in audited, bounds-checked abstractions and fuzz it relentlessly (§121.3). The C++ Core Guidelines and tools like the lifetime profile encode exactly these safe-subset rules.

---

## 121.3 Fuzzing: Finding Bugs Before Attackers

**Fuzzing** (Chapter 105) generates vast quantities of malformed input to drive code into the edge-case states where memory-safety bugs hide — and it is the single most effective technique for finding exploitable bugs in input-parsing code *before* attackers do.

```cpp
// Min standard: C++11 + libFuzzer (Clang). Fuzz a parser with ASan+UBSan to catch the bug at the crash.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    parse_untrusted_input(data, size);   // the fuzzer mutates `data` toward new code paths
    return 0;
}
// clang++ -fsanitize=fuzzer,address,undefined parser.cpp -o fuzz && ./fuzz
```
*Listing 121.1 — Fuzzing a parser. Coverage-guided fuzzing + sanitizers finds the input *and* localises the bug.*

> **Why this matters.** Attack surfaces are almost always **parsers of untrusted input** — network protocols, file formats, deserializers (Chapter 84) — because that is where attacker-controlled data meets memory operations. Fuzzing is uniquely suited to this: **coverage-guided** fuzzers (libFuzzer, AFL++) instrument the binary to discover which inputs reach new code paths and evolve toward unexplored branches, finding deep bugs that hand-written tests never reach. The force multiplier is running fuzzing *with sanitizers* (ASan, UBSan — Chapter 105): the fuzzer generates the malicious input, and the sanitizer catches the memory violation at the *instant* it occurs, with a precise diagnostic. Continuous fuzzing (Google's OSS-Fuzz runs this on critical open-source projects indefinitely) has found tens of thousands of vulnerabilities. The rule: anything that parses untrusted input must be fuzzed continuously, under sanitizers, as a security requirement.

---

## 121.4 Cryptography and Timing Attacks

The first rule of cryptography in C++ is: **never write your own crypto** — use audited libraries (`libsodium`, BoringSSL). Even *using* crypto correctly has a subtle C++ pitfall: **timing attacks**, where the *time* a comparison takes leaks information.

```cpp
// Min standard: C++11. The timing-attack trap and the constant-time fix.
// VULNERABLE: memcmp returns early on the first differing byte -> time leaks how many bytes matched.
//   if (memcmp(received_mac, expected_mac, 32) == 0) { /* authenticated */ }
//
// SAFE: constant-time comparison — always examines all bytes, time independent of the data.
//   if (crypto_verify_32(received_mac, expected_mac) == 0) { /* authenticated */ }   // libsodium
// (Or hand-rolled: OR together per-byte XORs and check the accumulator once at the end.)
```
*Listing 121.2 — `memcmp` leaks via early-exit timing; a constant-time comparison does not. Use a vetted crypto library.*

> **Why this matters / cost model.** The timing attack is a beautiful, counterintuitive vulnerability: `memcmp` is *optimised* to return as soon as it finds a difference (Chapter 80's "execute less code"), so comparing a received authentication tag against the expected one takes *longer* the more leading bytes match. An attacker who can measure that time can guess the secret one byte at a time, turning an infeasible 2^256 brute force into a feasible byte-by-byte search. The fix is a **constant-time comparison** that always examines every byte regardless of where they differ — the *opposite* of normal optimization, deliberately doing constant work to avoid the leak. This is why you must use crypto libraries: they implement constant-time primitives correctly (and resist the compiler's attempts to "optimise" them back into variable-time code). The deeper lesson is that in security, *performance optimizations can be vulnerabilities* — the early-exit that makes `memcmp` fast makes it insecure.

---

## 121.5 Side-Channels: Spectre and Speculative Execution

The hardware itself leaks. **Spectre** exploits **speculative execution** (Chapter 86): the CPU speculatively executes past a bounds check it predicts will pass, and even though the speculation is discarded when the check fails, it leaves a *trace in the cache* that an attacker can measure to read memory they should not access.

```cpp
// Min standard: C++11. The Spectre v1 gadget and a masking mitigation.
// VULNERABLE: the CPU may speculatively execute the body even when x >= array_len,
//   loading array[x] (x = secret out-of-bounds index) into the cache as a side effect.
//   if (x < array_len) { value = array2[array[x] * 64]; }   // cache trace leaks array[x]
//
// MITIGATION: mask the index so an out-of-bounds x is clamped, even under speculation.
//   if (x < array_len) { x &= mask; value = array2[array[x] * 64]; }   // or an LFENCE barrier
```
*Listing 121.2b — A Spectre v1 gadget: speculation leaves a cache trace that leaks out-of-bounds data.*

> **Why this matters / cost model.** Spectre broke a fundamental assumption: that a failed bounds check *protects* the data behind it. The CPU's speculative execution (the very feature that makes it fast, Chapter 86) executes the protected access *before* the check resolves, and the microarchitectural side effect (a cache line loaded based on secret data) survives the rollback and is measurable. This is a *hardware* vulnerability that software must mitigate: `LFENCE` barriers to stop speculation past a check, index *masking* (clamping the index so even speculative out-of-bounds access reads in-bounds, the branchless-masking idea of Chapter 91), and the kernel-level mitigations (KPTI, retpolines) that, as Chapter 98 noted, made syscalls measurably slower. The lesson for security engineers is humbling: even *correct* code with a *valid* bounds check can leak, because the leak happens at the microarchitectural level beneath the language's abstract machine (Chapter 85). Defending sensitive code (crypto, kernels, multi-tenant sandboxes) against side-channels requires reasoning about the *hardware*, not just the source.

---

## 121.6 The Security Discipline

| Threat | Mechanism | Defence |
|---|---|---|
| Buffer overflow | Write past bounds | `std::span`/`.at()`; never `strcpy` |
| Use-after-free | Access freed memory | RAII, smart pointers (Ch 97) |
| Input-triggered memory bug | Malformed input → UB | Fuzzing + sanitizers (Ch 105) |
| Timing attack | Data-dependent comparison time | Constant-time comparison; vetted crypto |
| Spectre / side-channel | Speculative execution leaks via cache | `LFENCE`, index masking, kernel mitigations |

> **The discipline.** Security in C++ is the recognition that the language's power is double-edged: direct memory access and aggressive optimization (including the hardware's speculation) are both performance features *and* attack surfaces. The defence is layered: use the *memory-safe subset* (`span`, RAII, smart pointers) to eliminate the bug classes by construction; *fuzz* every untrusted-input parser under sanitizers to find what slips through; use *audited crypto libraries* with constant-time primitives rather than rolling your own; and, for the most sensitive code, reason about *hardware* side-channels beneath the abstract machine. Crucially, security and correctness are the same discipline from a different angle — every undefined behaviour (Chapter 104) is a potential exploit, so the sanitizer-and-fuzzing rigour of Chapter 105 *is* security engineering. Treat all input as hostile, and assume every bug will be found. The volume now closes with a reference and a capstone that synthesises everything.
