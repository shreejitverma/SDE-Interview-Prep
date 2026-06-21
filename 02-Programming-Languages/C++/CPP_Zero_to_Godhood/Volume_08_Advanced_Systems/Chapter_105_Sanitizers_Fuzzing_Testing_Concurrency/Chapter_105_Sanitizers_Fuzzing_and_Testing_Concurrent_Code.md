# Chapter 105: Sanitizers, Fuzzing, and Testing Concurrent Code

Undefined behaviour and data races are bugs that *pass tests* — they corrupt silently, manifest only under specific inputs or interleavings, and may not surface until a compiler upgrade or a production load spike. Ordinary unit tests cannot find them, because the bug is the *absence* of a crash that should have happened. This chapter covers the tools built specifically to expose these latent defects: the sanitizers (ASan, TSan, UBSan, MSan) that instrument the program to catch UB and races at the moment they occur, fuzzing that generates the inputs to trigger them, and the special, genuinely-hard discipline of testing concurrent code.

## Chapter Roadmap

- 105.1 Why Ordinary Tests Miss These Bugs
- 105.2 AddressSanitizer (ASan)
- 105.3 UndefinedBehaviorSanitizer (UBSan)
- 105.4 ThreadSanitizer (TSan)
- 105.5 MemorySanitizer (MSan)
- 105.6 Fuzzing
- 105.7 Testing Concurrent Code
- 105.8 The Discipline

---

## 105.1 Why Ordinary Tests Miss These Bugs

A unit test checks that an input produces an expected output. But a use-after-free might *happen* to return the right value because the freed memory hasn't been reused yet; a data race might *happen* not to interleave badly in the test run; signed overflow might *happen* to wrap the way you expected at `-O0`. These bugs are **latent**: the defect is present, but its *symptom* depends on memory layout, timing, optimization level, or input that the test never exercises.

> **Why this matters.** The defining property of UB and race bugs is that they are *non-deterministic in their manifestation* — the same code, same test, passes a thousand times and fails the thousand-and-first, or passes in CI and fails in production, or passes at `-O0` and miscompiles at `-O2` (Chapter 104). You cannot find them by checking outputs, because the output is often *correct by luck*. The tools in this chapter work differently: instead of checking the result, they instrument the program to detect the *moment the contract is violated* — the instant memory is accessed out of bounds, the instant two threads race — turning a silent latent bug into a loud immediate failure with a diagnostic.

---

## 105.2 AddressSanitizer (ASan)

**ASan** instruments memory accesses to catch spatial and temporal memory errors at the moment they occur: heap/stack/global buffer overflows, use-after-free, use-after-return/scope, and double-free.

```bash
# Min: GCC/Clang. Compile and link with the sanitizer; run normally.
g++ -fsanitize=address -fno-omit-frame-pointer -g app.cpp -o app
./app    # aborts with a precise report (allocation site, free site, access site) on first error
```
*Listing 105.1 — ASan: a compile flag turns memory errors into immediate, well-located failures.*

> **Why this matters / cost model.** ASan works by surrounding allocations with **redzones** (poisoned guard regions) and maintaining **shadow memory** that records which bytes are valid; every load/store checks the shadow. It catches the exact access that goes out of bounds or touches freed memory, reporting all three relevant stack traces (allocation, free, and the bad access) — converting a heisenbug into a deterministic, debuggable one. The cost is ~2× slowdown and ~3× memory, so it runs in CI and testing, not production. It is the single highest-value tool for the lifetime bugs of Chapter 97 (dangling pointers, use-after-free in lock-free reclamation, Chapter 94) — run your whole test suite under ASan and most memory-safety bugs surface immediately.

---

## 105.3 UndefinedBehaviorSanitizer (UBSan)

**UBSan** instruments the operations that are UB (Chapter 104) to detect them at runtime: signed overflow, null dereference, misaligned access, invalid enum/bool values, shift overflow, and more.

```bash
# Min: GCC/Clang. Often combined with ASan.
g++ -fsanitize=undefined,address -fno-sanitize-recover=all -g app.cpp -o app
./app    # reports the exact UB operation and source location
```
*Listing 105.2 — UBSan catches the UB itself, not just its eventual corruption.*

> **Why this matters.** UBSan attacks the root cause that Chapter 104 warned about: rather than waiting for the optimizer to exploit signed overflow into a deleted check, UBSan flags the *overflow itself* the moment it executes, with file and line. This is invaluable because UB's symptoms are so far removed from their cause — a strict-aliasing violation or signed overflow may corrupt something unrelated much later, but UBSan points at the actual UB. With `-fno-sanitize-recover` it aborts on first violation (good for CI); by default it reports and continues. Combined with ASan in CI, UBSan closes most of the single-threaded UB surface.

---

## 105.4 ThreadSanitizer (TSan)

**TSan** detects data races (Chapter 76) — the hardest concurrency bug — by tracking the happens-before relationship between memory accesses across threads and flagging any conflicting access pair not ordered by synchronization.

```bash
# Min: GCC/Clang. Run the concurrent test suite under TSan.
g++ -fsanitize=thread -g app.cpp -o app
./app    # reports the racing accesses, both stack traces, and the missing synchronization
```
*Listing 105.3 — TSan reports data races even if they did not corrupt anything in this run.*

> **Why this matters / cost model.** TSan is transformative for concurrent code because it finds races *that did not manifest* — it detects the *absence of a happens-before edge* between two conflicting accesses (Chapter 76), regardless of whether the bad interleaving actually occurred in this run. It reports both racing accesses, both stack traces, and what synchronization was missing. This catches the under-synchronised atomics that "work on x86" before they reach an ARM machine (Chapter 76), the forgotten lock, the racy lazy-initialisation. The cost is high (~5–15× slowdown, large memory), and it only flags races on code paths actually executed, so it must be paired with good concurrent test coverage. But for the memory-model and lock-free chapters of this volume, TSan is the *only* practical way to validate correctness — race bugs are otherwise nearly impossible to find by inspection or ordinary testing.

---

## 105.5 MemorySanitizer (MSan)

**MSan** detects reads of **uninitialized memory** — values used before being written — by tracking the initialization state of every bit (Clang only).

> **Why this matters.** Uninitialized reads are UB (Chapter 104) and a classic source of non-determinism and security leaks (an uninitialized buffer sent over the network may leak stack/heap contents). MSan tracks "uninitialized" through computations and flags the point where an uninitialized value affects observable behaviour. Its caveat is strict: it requires *all* code, including the standard library and dependencies, to be instrumented (an uninstrumented library appears to "initialize" everything), which makes it harder to deploy than ASan/UBSan — but for code handling sensitive data or chasing non-deterministic bugs, it closes a gap the others don't.

---

## 105.6 Fuzzing

Sanitizers detect a violation *when it occurs*; **fuzzing** generates the inputs to *make* it occur. A fuzzer feeds the program large volumes of generated/mutated input, guided by code coverage, to drive it into states and edge cases hand-written tests never reach.

```cpp
// Min standard: C++11 + libFuzzer (Clang). The entry point a coverage-guided fuzzer drives.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    parse_packet(data, size);   // the fuzzer mutates `data`, guided by coverage, to find crashes
    return 0;
}
// clang++ -fsanitize=fuzzer,address,undefined parser.cpp -o fuzz && ./fuzz
```
*Listing 105.4 — A libFuzzer harness. Combined with ASan/UBSan, it finds the input *and* the bug.*

> **Why this matters / cost model.** Fuzzing is extraordinarily effective on code that parses untrusted input (network protocols, file formats, deserializers — Chapter 84) because that is exactly where edge-case-driven memory bugs hide and where security matters most. **Coverage-guided** fuzzers (libFuzzer, AFL++) instrument the program to see which inputs reach new code paths and evolve their corpus toward unexplored branches, finding deep bugs efficiently rather than by blind random input. The multiplier is combining fuzzing *with* sanitizers: the fuzzer generates the input that triggers a latent bug, and ASan/UBSan catch the violation the instant it happens — together they find and localise bugs that neither could alone. Continuous fuzzing (OSS-Fuzz-style) runs this indefinitely in CI. The cost is setup (writing harnesses, seed corpora) and compute, justified for any code on a security or robustness boundary.

---

## 105.7 Testing Concurrent Code

Concurrency is the hardest thing to test because the bug depends on a specific *interleaving* among astronomically many, and the test scheduler rarely hits the bad one. The techniques:

- **Stress testing under TSan** — run many threads hammering the structure under TSan, which detects races even on interleavings that didn't corrupt. The default workhorse.
- **Randomized/forced scheduling** — inject random delays/yields, or use tools (`rr` for deterministic record-replay, controlled schedulers) to explore more interleavings.
- **Model checking** — exhaustively explore *all* interleavings of a small concurrent algorithm: CDSChecker / GenMC for the C++ memory model, TLA+/Spin for protocol-level designs.
- **Deterministic replay** (`rr`) — record a failing run and replay it identically, turning a one-in-a-million race into a reproducible debugging session.

> **Why this matters.** The fundamental problem is that a concurrent bug may require a 1-in-10⁹ interleaving, so running the test "a lot" and seeing it pass proves almost nothing — you may simply never have hit the bad schedule. The escalating answers: TSan finds races *without* needing the bad interleaving (it reasons about happens-before, §105.4), which is why it is the first line of defence; model checkers (CDSChecker, GenMC) *exhaustively* verify a small lock-free algorithm against the C++ memory model, the gold standard for validating a hand-rolled structure (Chapters 77, 93, 94); and `rr` makes the rare failure reproducible once you have one. This is also the strongest argument, restated from the lock-free chapters, for *using vetted concurrent libraries* — they have been through exactly this gauntlet, and reproducing it for your own structure is a major undertaking.

---

## 105.8 The Discipline

| Tool | Catches | Cost | When |
|---|---|---|---|
| ASan | OOB, use-after-free/scope, double-free | ~2× | All test runs / CI |
| UBSan | Signed overflow, null, misalign, bad enum | Low–moderate | All test runs / CI |
| TSan | Data races (happens-before) | ~5–15× | Concurrent test suites |
| MSan | Uninitialized reads | High; needs full instrumentation | Sensitive/non-determinism hunts |
| Fuzzing + sanitizers | Input-triggered memory/UB bugs | Compute + harness setup | Parsers, untrusted input |
| Model checking / `rr` | Interleaving bugs; reproduction | High; small scope | Hand-rolled lock-free; rare races |

> **The discipline.** UB and race bugs are invisible to output-checking tests, so they require tools that detect the *violation*, not the symptom. The baseline for any serious C++ codebase: run the test suite under **ASan+UBSan** continuously in CI (catches the single-threaded UB and memory bugs of Chapter 104), run concurrent tests under **TSan** (catches the races of Chapter 76 before they reach a weakly-ordered machine), **fuzz** any code that parses untrusted input, and reserve **model checking** for hand-rolled lock-free structures you cannot avoid writing. These tools are not optional polish — for the systems this volume targets, where a single latent UB or race is a production incident, they are the only practical way to know the code is actually correct. The final chapter unifies the volume's performance and correctness threads into the discipline that matters most for these systems: determinism.
