# Chapter 80: High-Performance Optimization

Optimization is the discipline of spending effort where the machine actually spends time — which is almost never where intuition says. This chapter is the synthesis hub of the volume: it surveys the full optimization toolbox (cache behaviour, branch prediction, SIMD, LTO/PGO, and the library-level optimizations of copy elision, small-object optimization, and the empty base) and, crucially, the *methodology* — measure first, optimize the hot path, and respect the cost model — that prevents the classic failure of making correct code slower and uglier. Several topics here open into dedicated chapters (87 caches, 91 branchless, 92 SIMD, 102 LTO/PGO, 103 benchmarking); this chapter ties them together and covers the optimizations that have no other home.

## Chapter Roadmap

- 80.1 The Methodology: Measure First, and the Cost of Premature Optimization
- 80.2 Cache Is King
- 80.3 Branch Prediction and Branchless Code
- 80.4 SIMD: Single Instruction, Multiple Data
- 80.5 Executing Less Code
- 80.6 Choosing Efficient Containers
- 80.7 Copy Elision and RVO
- 80.8 Small Object / Small String Optimization
- 80.9 The Empty Base Class Optimization
- 80.10 Whole-Program Optimization: LTO and PGO
- 80.11 The Optimization Discipline

---

## 80.1 The Methodology: Measure First, and the Cost of Premature Optimization

C and C++ are high-performance languages largely because they let the programmer choose the data structures and memory layout that determine performance. But that power is a trap without discipline. The first priority is **correct and maintainable code**; optimization comes second, guided by measurement.

The classic mistakes:

- **Premature optimization.** Complex "optimized" code may perform *worse* and wastes engineering time. Write it correctly first.
- **Optimizing the wrong use case.** Adding overhead to help the 1% case can slow the 99% case.
- **Micro-optimization by hand.** The compiler does this extremely well; hand-twiddling can *defeat* the optimizer by obscuring the code it would otherwise transform.

Optimization also has negative side effects to weigh: higher memory usage, harder-to-read code, and compromised API design. The legitimate goals are to **do less work**, **use more efficient algorithms/structures**, and **make better use of the hardware** — in that order of leverage.

> **Why this matters.** Profiling exists because programmer intuition about where time goes is wrong far more often than right. A **sampling profiler** (`perf`, VTune) interrupts the CPU periodically to see what is running — low overhead, finds hot spots. An **instrumentation profiler** (`gprof`, Valgrind/callgrind) records exact call counts and timings — high overhead, exact call graphs. **Microbenchmarks** (Google Benchmark) isolate one function. Never optimize without one of these telling you *where* the time is (Chapter 103 covers the pitfalls of measuring correctly).

---

## 80.2 Cache Is King

Main memory is slow (~100 ns, hundreds of cycles); registers are ~1 cycle; the L1/L2/L3 caches bridge the gap. **A cache miss is the single most expensive routine event on a modern CPU** — a full miss to DRAM stalls the core for hundreds of cycles, during which it might have retired hundreds of instructions.

- **Data-oriented design.** Lay data out for the access pattern. *Structure-of-Arrays* (SoA) often beats *Array-of-Structures* (AoS) because it brings only the fields you touch into cache and vectorizes cleanly (Chapter 90).
- **Linear access beats pointer chasing.** A `std::vector` traversal prefetches beautifully; a `std::list` or tree scatters nodes across the heap, and each `->next` is a likely cache miss.

> **Why this matters / cost model.** The rule of thumb — *prefer `std::vector` over `std::list`, prefer contiguous linear access, avoid pointer chasing across the heap* — follows directly from the miss cost. A linked list with the same big-O as a vector can be 10–50× slower in practice because the vector's accesses are predicted and prefetched while the list's are serialized cache misses. The cache hierarchy is developed in full in Chapter 87; the headline is that *memory layout, not instruction count, dominates most real workloads*.

---

## 80.3 Branch Prediction and Branchless Code

A modern CPU is deeply pipelined and must *guess* the direction of each conditional branch to keep the pipeline full. A correct guess is free; a **misprediction** flushes the pipeline (~15–20 cycles wasted).

```cpp
// Min standard: C++11 (branchless), C++20 ([[likely]]). Portable.
// Branchy — mispredicts on unpredictable x:
if (x > 0) y = 1; else y = 0;

// Branchless — the comparison yields 0/1 directly, no branch:
y = (x > 0);
```
*Listing 80.1 — Replacing an unpredictable branch with a data computation.*

- **Predictable branches are nearly free.** Sorting data so a branch goes `TTTT...FFFF` lets the predictor learn it; the famous "why is processing a sorted array faster" result is entirely branch prediction.
- **`[[likely]]` / `[[unlikely]]`** (C++20) hint the compiler to lay out the hot path for fall-through; use sparingly and only where profiling confirms the bias.

> **Why this matters.** Branchless code is not universally faster — it trades a *possible* misprediction for *guaranteed* work (the computation runs both "sides"). It wins only when the branch is genuinely unpredictable and the work is cheap. On a predictable branch, the branchy version is faster. This is developed, with the predication cost model and SIMD masking, in Chapter 91.

---

## 80.4 SIMD: Single Instruction, Multiple Data

**SIMD** performs the same operation on 4, 8, or 16 lanes at once using wide vector registers. Three routes to it:

- **Intrinsics** — `_mm256_add_ps` (AVX) and friends: maximal control, poor readability, non-portable across ISAs.
- **Auto-vectorization** — the compiler vectorizes simple, dependency-free loops automatically; the most maintainable route when it triggers.
- **Libraries** — `std::simd` (C++26, formerly `std::experimental::simd`), Google Highway, Vc: portable, readable, near-intrinsic performance.

> **Why this matters / cost model.** SIMD offers up to an N× speedup on data-parallel arithmetic, but only when the data layout cooperates (contiguous, aligned, SoA) and there are no loop-carried dependencies. It does *not* help branchy, pointer-chasing, or memory-bound code. The detailed mechanics, alignment requirements, and when vectorization does and does not pay are Chapter 92.

---

## 80.5 Executing Less Code

The most direct optimization is to run fewer instructions for the same result — a fixed speedup that does not change the algorithm's complexity, valuable on hot paths.

**Remove useless work.** From C++14, the compiler may even elide a `make_unique`/destroy pair, but the clearer fix is to not allocate at all:

```cpp
// Min standard: C++14. Portable.
void func(const A* a);
// Useless heap allocation + deallocation:
auto a1 = std::make_unique<A>();
func(a1.get());
// A stack object avoids the allocation entirely:
auto a2 = A{};
func(&a2);
```
*Listing 80.2 — Prefer stack objects to needless heap allocation.*

**Do the work once.** A lookup that probes a map three times is three times too slow:

```cpp
// Min standard: C++11. Portable.
std::map<std::string, std::unique_ptr<A>> lookup;

// Slow: find(), then operator[] inserts (second traversal), then get() (third).
const A* lazyLookupSlow(const std::string& key) {
    if (lookup.find(key) == lookup.cend())
        lookup.emplace(key, std::make_unique<A>());
    return lookup[key].get();
}

// Fast: one traversal via a single operator[] that returns a reference.
const A* lazyLookupFast(const std::string& key) {
    auto& value = lookup[key];
    if (!value) value = std::make_unique<A>();
    return value.get();
}
```
*Listing 80.3 — Collapsing three map traversals into one.*

**Use the return value you already computed.** `set::insert` returns whether the insert happened — use it instead of a separate membership check:

```cpp
// Min standard: C++11. Portable. Stable de-duplication in one pass.
std::vector<std::string> stableUnique(const std::vector<std::string>& v) {
    std::vector<std::string> result;
    result.reserve(v.size());              // prevent reallocation/copying entirely
    std::unordered_set<std::string> seen;  // O(1) expected insert (see 80.6)
    for (const auto& s : v)
        if (seen.insert(s).second)         // insert reports novelty — no separate find()
            result.push_back(s);
    return result;
}
```
*Listing 80.4 — `insert().second` avoids a second probe; `reserve` avoids growth reallocation.*

> **Why this matters / cost model.** Each of these is a constant-factor win, invisible unless the code is *hot*. `reserve(v.size())` is nearly free when correct (allocating one large block costs about the same as one small block) and removes every intermediate reallocation and the moves they trigger. The lesson: on a hot path, count the redundant traversals, allocations, and copies — the compiler removes some, but algorithmic redundancy (three map probes) is yours to eliminate.

---

## 80.6 Choosing Efficient Containers

Where §80.5 shaved constant factors, the right container changes the *complexity class*. The `stableUnique` of Listing 80.4 is O(N) expected with `std::unordered_set` (hash, O(1) insert) versus O(N log N) with `std::set` (balanced tree, O(log N) insert):

```cpp
// O(N log N): std::set is a balanced tree — O(log N) insert, ordered.
// O(N) expected: std::unordered_set is a hash table — O(1) insert, unordered.
// Swapping the container changes the asymptotic cost AND reduces comparisons:
// the hash set only compares strings that collide into the same bucket.
```
*Listing 80.5 — Container choice as a complexity-class decision.*

> **Why this matters.** This is the highest-leverage optimization after algorithm choice: a tree-to-hash swap turns N log N into N and, as a side effect, slashes the number of expensive `std::string` comparisons (only intra-bucket collisions compare). But it is not free — hashing has its own constant cost, unordered containers have worse locality than a sorted `vector`, and you lose ordering. For small N, a sorted `std::vector` with binary search often beats both. Match the container to the operation mix (insert/lookup/iterate/order) and the size, not to habit.

---

## 80.7 Copy Elision and RVO

The compiler may omit copies/moves when constructing an object directly into its destination. **NRVO** (Named Return Value Optimization) elides the copy of a named local returned by value; unnamed temporaries are elided even more readily.

```cpp
// Min standard: C++17 (mandatory elision for prvalues). Portable.
std::vector<int> make_big() {
    std::vector<int> v(1'000'000);
    // ... fill v ...
    return v;          // NRVO: constructed directly in the caller, no copy/move
}
auto data = make_big();  // no copy of a million ints
```
*Listing 80.6 — Returning large objects by value is cheap thanks to elision.*

> **Why this matters.** Before move semantics, "return by value" of a big object meant a deep copy, so programmers passed out-parameters by reference — uglier and error-prone. **Mandatory copy elision (C++17)** *guarantees* the elision for prvalue returns, making "return by value" the correct, readable default even for expensive types. The practical rule: write functions that return their result by value; do not pessimize by adding `std::move` on the return of a local (it can *disable* NRVO). The optimizer is better at this than manual gymnastics.

---

## 80.8 Small Object / Small String Optimization

**Small Object Optimization (SOO)** / **Small String Optimization (SSO)** stores small payloads in an inline buffer inside the object instead of on the heap, avoiding a `malloc`/`free` cycle and improving locality. Standard `std::string` (typically up to 15–22 bytes inline) and `std::function` use it.

```cpp
// Min standard: C++11. Illustrative naive SSO string.
#include <cstring>
class string final {
    constexpr static auto SMALL_BUFFER_SIZE = 16;
    bool  _isAllocated{false};
    char* _buffer{nullptr};
    char  _smallBuffer[SMALL_BUFFER_SIZE] = {'\0'};   // inline stack storage
public:
    ~string() { if (_isAllocated) delete[] _buffer; }
    explicit string(const char* cstr) {
        auto n = std::strlen(cstr);
        _isAllocated = (n > SMALL_BUFFER_SIZE);
        _buffer = _isAllocated ? new char[n + 1] : &_smallBuffer[0];
        std::strcpy(_buffer, cstr);
    }
    string(string&& rhs)
        : _isAllocated(rhs._isAllocated), _buffer(rhs._buffer) {
        if (_isAllocated) {
            rhs._buffer = nullptr;            // steal the heap buffer
        } else {
            std::strcpy(_smallBuffer, rhs._smallBuffer);  // must copy the inline bytes
            _buffer = &_smallBuffer[0];
        }
    }
    // Other constructors/operators omitted for brevity.
};
```
*Listing 80.7 — A naive SSO string: inline buffer for small strings, heap for large.*

> **Why this matters / cost model.** SSO trades a larger `sizeof` and extra branching for the elimination of heap traffic on the common case of short strings. The trade-offs are real: the object is bigger (wasting space when strings are large), and — as Listing 80.7's move constructor shows — **moving an SSO object is more expensive than moving a heap-only one**, because the inline bytes must be copied rather than the pointer merely stolen (especially painful for non-POD inline contents). Implementations often pack `_isAllocated` into a spare bit of the pointer to shrink the object, which depends on the platform's alignment guarantees. SSO is worth its complexity only in heavily-used low-level types with predominantly small data — exactly where the standard library applies it.

---

## 80.9 The Empty Base Class Optimization

An object cannot occupy zero bytes — distinct objects of the same type need distinct addresses, so `sizeof(T) >= 1`. But an **empty base class** need not add to the derived size: the **Empty Base Class Optimization (EBO)** lets a base contributing no data members occupy no space in the derived object.

```cpp
// Min standard: C++98 (EBO); C++20 [[no_unique_address]] generalizes it to members.
class Base {};                 // empty: no data members
class Derived : public Base {  // EBO: Base contributes 0 bytes
public:
    int i;
};
// sizeof(Derived) == sizeof(int)  — the empty Base costs nothing.
```
*Listing 80.8 — EBO collapses an empty base to zero size in the derived object.*

EBO applies only if the first member of `Derived` differs in type from any (direct or indirect) base — otherwise a byte must be allocated so two distinct objects of the same type cannot share an address.

> **Why this matters.** EBO is why stateless policies (Chapter 74) and stateless allocators/comparators add *zero* bytes to the objects that use them: `std::vector` stores its allocator as an EBO base, `std::unique_ptr` stores a stateless deleter for free, and policy-based designs compose many empty policies into a single-word object. C++20's `[[no_unique_address]]` extends the same saving to *members*, not just bases. The payoff is dense, cache-friendly objects: a comparator or allocator you pass for flexibility costs nothing at runtime or in memory.

---

## 80.10 Whole-Program Optimization: LTO and PGO

**Link-Time Optimization (LTO)** defers final code generation to link time, when the optimizer can see *all* translation units at once — enabling inlining and constant propagation across object-file boundaries that ordinary per-TU compilation cannot.

**Profile-Guided Optimization (PGO)** feeds real execution data back into the compiler:

1. Compile with instrumentation.
2. Run the program on representative input to collect a profile.
3. Recompile using the profile, so the optimizer lays out hot paths for fall-through, inlines hot callees, and orders code for instruction-cache locality.

> **Why this matters / cost model.** LTO and PGO are the highest-leverage *free* optimizations: no source change, often 5–20% across the board, because they fix things local optimization cannot see — cross-TU inlining (LTO) and which branch/call is actually hot (PGO). Their cost is build complexity and longer link/build times, and PGO requires a representative workload (a misrepresentative profile can pessimize). These are developed, with the linking and ABI mechanics that make cross-TU inlining possible, in Chapter 102.

---

## 80.11 The Optimization Discipline

| Lever | Typical payoff | When to apply |
|---|---|---|
| Better algorithm / container | Complexity class (asymptotic) | Always first — biggest lever |
| Cache-friendly layout (SoA, contiguous) | 2–50× on memory-bound code | Hot data structures |
| Executing less code (fewer probes/allocs/copies) | Constant factor | Hot paths |
| Branchless / predication | Avoids mispredict on unpredictable branches | Measured hot branch |
| SIMD | Up to N× on data-parallel arithmetic | Vectorizable inner loops |
| LTO / PGO | 5–20% global | Release builds, free |
| Copy elision / SSO / EBO | Removes copies/allocs/bytes | Library and hot-object design |

> **The discipline.** Optimize in order of leverage: get the algorithm and data layout right, then eliminate redundant work on the measured hot path, then apply hardware-level techniques (branchless, SIMD) where profiling justifies them, and turn on LTO/PGO for free. At every step, *measure* — the rest of this volume gives you the cache, branch, syscall, and latency models to predict what will help, and Chapters 103 and 106 give you the measurement rigor to confirm it actually did.
