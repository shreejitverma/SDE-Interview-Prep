# Chapter 76: The C++ Memory Model in Depth

The C++ memory model is the contract that makes multithreaded code *meaningful*: it defines, for a program with concurrent access to shared memory, which writes a read is permitted to observe. Without it, the compiler, the CPU, and the cache coherence protocol are each free to reorder memory operations for performance, and "the value of `x`" stops being a well-defined question. This chapter develops the model from the ground up — the abstract machine, the happens-before relation, the six memory orderings and what each actually costs on real hardware — because every lock-free structure, every atomic flag, and every correctness argument in the chapters that follow rests on it.

## Chapter Roadmap

- 76.1 Why a Memory Model Exists: Reordering and the Abstract Machine
- 76.2 Data Races and Undefined Behaviour
- 76.3 The Relations: Sequenced-Before, Synchronizes-With, Happens-Before
- 76.4 `std::atomic` and Atomicity
- 76.5 The Six Memory Orderings
- 76.6 The Release/Acquire Pattern in Depth
- 76.7 Relaxed Ordering and Its Legitimate Uses
- 76.8 Sequential Consistency and Its Cost
- 76.9 Fences
- 76.10 The Hardware Cost Model: x86 vs ARM/POWER
- 76.11 Common Bugs and Anti-Patterns

---

## 76.1 Why a Memory Model Exists: Reordering and the Abstract Machine

C++ is defined in terms of an **abstract machine**. The compiler must preserve the *observable behaviour* of a single thread (the "as-if" rule), but it is otherwise free to reorder, combine, and eliminate memory operations. The CPU does the same at runtime: an out-of-order core executes loads and stores whenever their operands are ready, store buffers delay writes from becoming globally visible, and the cache coherence protocol propagates changes between cores with latency. Three independent layers reorder memory:

1. **The compiler** — hoists loads out of loops, sinks stores, keeps values in registers, reorders independent operations.
2. **The CPU** — speculative and out-of-order execution issue memory ops out of program order.
3. **The memory subsystem** — store buffers, invalidation queues, and coherence traffic mean a store on one core becomes visible to another only after a delay, and not necessarily in program order.

> **Why this matters.** In a single thread none of this is observable — the hardware and compiler guarantee a thread sees its own operations in order. Across threads, all three layers are visible, and the *naive* assumption that thread B sees thread A's writes in the order A issued them is simply false. The memory model is the precise specification of what *is* guaranteed, so you can reason about concurrent code without modelling every microarchitecture.

---

## 76.2 Data Races and Undefined Behaviour

A **data race** occurs when two threads access the same memory location concurrently, at least one access is a write, and the accesses are not ordered by any synchronisation. **A data race is undefined behaviour** — full stop. Not "you read a stale value"; the entire program's behaviour is undefined, and the optimiser is entitled to assume races never happen.

```cpp
// BUGGY — data race, undefined behaviour. Min standard: C++11.
int data = 0;
bool ready = false;          // plain bool, not atomic

void producer() { data = 42; ready = true; }                 // two plain writes
void consumer() { while (!ready) {} assert(data == 42); }     // racy reads
```
*Listing 76.1 — A textbook data race. The compiler may hoist `ready` into a register, making the loop spin forever; or reorder the two writes so `ready` is seen true before `data == 42`.*

> **Why this matters / cost model.** The fix is not a `volatile` (which constrains the compiler but provides *no* inter-thread ordering or atomicity — a common and dangerous misconception) but `std::atomic` with an appropriate ordering. The reason UB is so severe here is that the optimiser, proving no race can exist, may delete the spin loop entirely, fold the two paths, or assume `ready` never changes. Correctness in concurrent C++ begins with: *every cross-thread shared access is either protected by a mutex or is an atomic operation.*

---

## 76.3 The Relations: Sequenced-Before, Synchronizes-With, Happens-Before

The model is built from three relations:

- **Sequenced-before** — the intra-thread program order between evaluations (within the limits of unspecified evaluation order). This is what gives single-threaded code its meaning.
- **Synchronizes-with** — the cross-thread relation established by a *release* operation paired with an *acquire* operation that reads the released value. This is the only way to manufacture an ordering edge *between* threads.
- **Happens-before** — the transitive closure of sequenced-before and synchronizes-with. If write W happens-before read R, then R is guaranteed to observe W (or a later write); if neither happens-before the other and they conflict, you have a data race.

> **Why this matters.** "Happens-before" is the single concept that makes concurrent reasoning tractable. You never reason about caches or store buffers directly; you ask: *is there a happens-before edge from this write to that read?* If yes, the value is visible. If no, it is a race (UB) or — for non-conflicting atomics — merely unordered. Every synchronisation primitive (mutex lock/unlock, atomic release/acquire, thread create/join) exists to create exactly these edges.

---

## 76.4 `std::atomic` and Atomicity

`std::atomic<T>` guarantees that read-modify-write and load/store operations on the object are **indivisible**: no thread observes a half-written value, and the operation is free of data races by definition.

```cpp
// Min standard: C++11. Portable.
#include <atomic>
std::atomic<int> count{0};
count.fetch_add(1, std::memory_order_relaxed);   // indivisible increment
int c = count.load(std::memory_order_relaxed);
```
*Listing 76.2 — Atomic read-modify-write.*

Atomicity and ordering are **orthogonal**. `fetch_add` is always atomic regardless of the memory order argument; the order argument controls only what *other* memory operations are ordered relative to it. `atomic<T>::is_lock_free()` reports whether operations use hardware atomics or a hidden lock — for types wider than the platform's atomic width (e.g. a 16-byte struct without `cmpxchg16b`), the implementation may fall back to a mutex, which destroys the lock-free property you may have been relying on.

---

## 76.5 The Six Memory Orderings

Every atomic operation takes a `std::memory_order` controlling the ordering edges it creates:

| Order | Applies to | Guarantee |
|---|---|---|
| `relaxed` | load/store/RMW | Atomicity only. No ordering with other memory ops. |
| `acquire` | load, RMW | No load/store *after* it (in program order) may be reordered before it. Pairs with a release to synchronize. |
| `release` | store, RMW | No load/store *before* it may be reordered after it. Publishes prior writes. |
| `acq_rel` | RMW | Both acquire (on the read) and release (on the write) for read-modify-write ops. |
| `seq_cst` | all | Acquire/release **plus** a single global total order over all `seq_cst` operations. The default. |
| `consume` | load | Weaker acquire ordering only data-dependent operations; **discouraged/effectively deprecated** — compilers promote it to acquire. |

> **Why this matters.** These are not interchangeable knobs to twist for speed; each names a precise guarantee. Choosing one weaker than your algorithm requires is a correctness bug that may *never* reproduce on x86 (which is strongly ordered) and then fail in production on ARM. Choosing one stronger than required is a performance bug — `seq_cst` everywhere forces full barriers that serialise the store buffer. The discipline is: derive the *minimum* ordering each operation needs from a happens-before argument, then use exactly that.

---

## 76.6 The Release/Acquire Pattern in Depth

The canonical publication pattern: a producer writes data, then *releases* a flag; a consumer *acquires* the flag, then reads the data. The release/acquire pair creates a synchronizes-with edge, so everything sequenced-before the release in the producer happens-before everything sequenced-after the acquire in the consumer.

```cpp
// Min standard: C++11. Portable. Correct publication.
#include <atomic>
#include <cassert>
std::atomic<bool> ready{false};
int data = 0;                    // plain int — safe because release/acquire orders it

void producer() {
    data = 42;                                          // (1) ordinary write
    ready.store(true, std::memory_order_release);       // (2) publish: (1) cannot move after (2)
}
void consumer() {
    while (!ready.load(std::memory_order_acquire)) {}   // (3) acquire: (4) cannot move before (3)
    assert(data == 42);                                 // (4) guaranteed to read 42
}
```
*Listing 76.3 — Release/acquire publication. `data` need not be atomic because the happens-before edge orders it.*

> **Why this matters / cost model.** This is the workhorse of lock-free code — SPSC queues, lazy initialisation, sequence-locks all use it. The key insight is that `data` itself is a plain non-atomic `int`: the release/acquire pair on `ready` transitively orders the access to `data`, so there is no race. This is strictly cheaper than `seq_cst`: on x86 both the release store and acquire load are plain `mov` instructions (the hardware already gives the needed ordering), and on ARM they map to `stlr`/`ldar` — far cheaper than the full `dmb ish` barrier a `seq_cst` operation requires. The synchronisation is also *pairwise*: a release synchronizes only with the specific acquire that reads its value, not globally.

---

## 76.7 Relaxed Ordering and Its Legitimate Uses

`relaxed` provides atomicity with no ordering. It is correct precisely when you need an atomic counter or flag but no happens-before edge to other data.

```cpp
// Min standard: C++11. Portable.
#include <atomic>
std::atomic<long> hits{0};
void on_request() { hits.fetch_add(1, std::memory_order_relaxed); }   // pure statistic
```
*Listing 76.4 — A relaxed counter: no data is published through it, so no ordering is needed.*

Legitimate uses: monotonic statistics counters, reference counts on *increment* (the decrement that may trigger destruction needs `acq_rel`/release+acquire), and generating unique IDs. The hazard is using `relaxed` on the flag of Listing 76.3 — it would make `data` a race, because relaxed creates no synchronizes-with edge.

> **Why this matters.** Relaxed is the cheapest atomic (a bare `lock xadd` on x86, a plain `ldxr/stxr` loop on ARM with no barrier) and the easiest to misuse. The litmus test: *does any other memory access depend on the ordering of this atomic?* If no, relaxed is correct and optimal. If yes, you need at least release/acquire.

---

## 76.8 Sequential Consistency and Its Cost

`seq_cst` is the default and the easiest to reason about: all `seq_cst` operations across all threads appear to execute in one global total order consistent with each thread's program order. It is what most programmers intuitively assume memory "should" do.

```cpp
// Min standard: C++11. Portable. Dekker-style: seq_cst is required here.
std::atomic<bool> x{false}, y{false};
std::atomic<int> z{0};
void t1() { x.store(true); if (!y.load()) ++z; }   // default seq_cst
void t2() { y.store(true); if (!x.load()) ++z; }
// With seq_cst, z == 2 is impossible. With acquire/release, z == 2 CAN occur.
```
*Listing 76.5 — A case where only `seq_cst` gives the intuitive result (store-load ordering across two locations).*

> **Why this matters / cost model.** The single global order is exactly the guarantee release/acquire does *not* provide: release/acquire orders a store before a later load only within a synchronizes-with chain, not across independent locations. The cost is a full memory barrier on the store side: on x86 a `seq_cst` store compiles to `xchg` (or `mov` + `mfence`), draining the store buffer; on ARM it is `dmb ish`. That barrier serialises the pipeline against the store buffer and can dominate a hot path. Use `seq_cst` when you genuinely need the total order (some mutual-exclusion and consensus algorithms do); otherwise prefer release/acquire.

---

## 76.9 Fences

`std::atomic_thread_fence(order)` imposes ordering without being tied to a particular atomic object, letting you combine cheap `relaxed` atomics with a single barrier rather than paying for ordering on every operation.

```cpp
// Min standard: C++11. Portable.
#include <atomic>
std::atomic<bool> ready{false};
int data = 0;
void producer() {
    data = 42;
    std::atomic_thread_fence(std::memory_order_release);   // fence, not a release store
    ready.store(true, std::memory_order_relaxed);          // relaxed store after the fence
}
void consumer() {
    while (!ready.load(std::memory_order_relaxed)) {}
    std::atomic_thread_fence(std::memory_order_acquire);   // acquire fence
    assert(data == 42);
}
```
*Listing 76.6 — A standalone fence pairs with another fence (or operation) to create the synchronizes-with edge.*

> **Why this matters.** Fences are valuable when many relaxed operations share one ordering point — e.g. filling several fields then publishing once. A release *fence* followed by a relaxed store synchronizes with an acquire *fence* preceded by a relaxed load that reads the value. The subtlety: a fence's ordering is *positional* (it orders operations before/after it in program order), unlike an operation's ordering which is tied to that operation. Misplacing a fence relative to the atomic it is meant to order is a classic bug.

---

## 76.10 The Hardware Cost Model: x86 vs ARM/POWER

The same C++ code has wildly different costs depending on the target's memory model:

| Operation | x86-64 (TSO, strongly ordered) | AArch64 (weakly ordered) |
|---|---|---|
| `relaxed` load/store | plain `mov` | plain `ldr`/`str` |
| `acquire` load | plain `mov` (free) | `ldar` |
| `release` store | plain `mov` (free) | `stlr` |
| `seq_cst` store | `xchg` / `mov+mfence` (drains store buffer) | `dmb ish` or `stlr`+barrier |
| RMW (`fetch_add`) | `lock xadd` | `ldaxr/stlxr` retry loop |

> **Why this matters.** x86's Total Store Order means acquire/release are *free* — the hardware already forbids the reorderings that matter — so code that is actually under-synchronised often passes all tests on x86 and only fails when ported to ARM (Apple Silicon, Graviton, mobile). This is the single most common way concurrency bugs escape to production. Conversely, `seq_cst`'s cost is mostly hidden on x86 (only stores pay) but pervasive on ARM. The lesson: *reason from the C++ memory model, not from x86 behaviour*, and test on a weakly-ordered target.

---

## 76.11 Common Bugs and Anti-Patterns

- **Using `volatile` for synchronisation.** `volatile` prevents the compiler from eliding accesses (correct for MMIO) but provides no atomicity and no inter-thread ordering. It is never a substitute for `std::atomic`.
- **Under-ordering that passes on x86.** Using `relaxed` where release/acquire is needed; works on TSO, races on ARM.
- **Assuming release/acquire gives a global order.** It does not (Listing 76.5); store-load ordering across independent locations needs `seq_cst`.
- **`seq_cst` everywhere "to be safe."** Correct but slow; serialises the store buffer on every operation.
- **Non-lock-free atomics.** A wide `atomic<T>` may use a hidden lock; check `is_lock_free()` before relying on lock-free progress.
- **Mixed orderings on one variable without a happens-before argument.** If you cannot state the happens-before edge in words, the code is not yet correct.

> **The discipline.** For every atomic operation, write down the happens-before edge it participates in and the *minimum* ordering that establishes it. This single habit prevents the majority of memory-model bugs and is the foundation for the atomics, lock-free, reclamation, and lock-design chapters that follow.
