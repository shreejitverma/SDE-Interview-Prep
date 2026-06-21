# Chapter 93: Atomics, CAS, and the ABA Problem

Atomics are the bricks of every lock and every lock-free structure: indivisible operations on shared memory that the hardware guarantees no thread can observe half-completed. This chapter goes beneath the lock-free overview of Chapter 77 to the atomic toolkit itself — the full `compare_exchange` semantics, the weak/strong distinction, read-modify-write operations, `atomic_ref` and `atomic<shared_ptr>`, double-width CAS — and treats the ABA problem in the depth it demands, because ABA is the silent corrupter of naive atomic code and the reason safe reclamation (Chapter 94) exists.

## Chapter Roadmap

- 93.1 Atomic Operations: The Hardware Reality
- 93.2 Read-Modify-Write and `fetch_*`
- 93.3 Compare-Exchange: Semantics in Full
- 93.4 Weak vs Strong, and the Two Memory Orders
- 93.5 The CAS Loop Pattern
- 93.6 `atomic_ref`, `atomic<shared_ptr>`, and Atomic Max/Min
- 93.7 The ABA Problem in Depth
- 93.8 Defeating ABA: Tags and Double-Width CAS

---

## 93.1 Atomic Operations: The Hardware Reality

An **atomic operation** completes indivisibly: any other thread sees it as either entirely done or not begun, never partway. The hardware provides this for word-sized (and double-word) operations via the `lock` prefix on x86 (`lock xadd`, `lock cmpxchg`) or load-linked/store-conditional (LL/SC) on ARM (`ldxr`/`stxr`). `std::atomic<T>` exposes these, falling back to a hidden lock for types the hardware cannot handle atomically (check `is_lock_free()`).

> **Why this matters / cost model.** An atomic RMW is not a free annotation on a normal operation; on x86 a `lock`-prefixed instruction costs ~15–40 cycles and acts as a full memory barrier (Chapter 76), and it must acquire *exclusive* ownership of the cache line (Chapter 87) — so contended atomics serialise on coherence traffic. This is the fundamental cost that makes atomics cheaper than locks (no syscall, no context switch) but far from free, and why reducing the *number* of contended atomic operations — not just avoiding locks — is the real scalability lever. An atomic on an uncontended, cache-hot line is cheap; the same atomic under heavy contention is a bottleneck.

---

## 93.2 Read-Modify-Write and `fetch_*`

Beyond plain load/store, atomics provide indivisible **read-modify-write (RMW)** operations: `fetch_add`, `fetch_sub`, `fetch_and`, `fetch_or`, `fetch_xor`, and `exchange`. Each atomically reads the old value, applies the operation, stores the result, and returns the old value.

```cpp
// Min standard: C++11. Portable.
#include <atomic>
std::atomic<long> counter{0};
long prev = counter.fetch_add(5, std::memory_order_relaxed);  // atomically += 5, returns old value
long old  = counter.exchange(0, std::memory_order_acq_rel);   // atomically set to 0, return prior
```
*Listing 93.1 — Atomic RMW operations return the *previous* value.*

> **Why this matters.** `fetch_add` is the right tool for counters and sequence numbers — it is a *single* atomic instruction (`lock xadd`), strictly cheaper and simpler than a CAS loop achieving the same effect, and it cannot fail or spin. The general rule: if your update is expressible as one of the dedicated RMW operations, use it directly rather than a `compare_exchange` loop — it is one instruction instead of a retry loop, and contends the line for a shorter window. Reach for CAS only when the new value depends on the old in a way no dedicated RMW expresses (§93.5).

---

## 93.3 Compare-Exchange: Semantics in Full

`compare_exchange` is the universal primitive: it atomically compares the atomic's value against an `expected`, and *if they match* writes `desired`; *if they don't*, it loads the current value into `expected` and reports failure.

```cpp
// Min standard: C++11. The full signature.
// bool compare_exchange_strong(T& expected, T desired,
//                              memory_order success, memory_order failure);
//
// Semantics (atomically):
//   if (*this == expected) { *this = desired; return true; }     // uses `success` order
//   else                   { expected = *this; return false; }   // uses `failure` order
```
*Listing 93.2 — `compare_exchange` writes back the current value into `expected` on failure — the key to retry loops.*

> **Why this matters.** Two details trip up nearly everyone. First, `expected` is an **in-out parameter**: on failure it is *overwritten* with the actual current value, which is precisely what lets a retry loop re-attempt against the fresh value without an extra load (§93.5). Second, there are **two memory orders** — one for the success case (the write happens) and one for failure (only a load happened, so it must be no stronger than success and cannot be `release`/`acq_rel`). Forgetting that `expected` is mutated, or over-ordering the failure case, are the two most common CAS bugs.

---

## 93.4 Weak vs Strong, and the Two Memory Orders

`compare_exchange_weak` may fail **spuriously** — return `false` even when the value matched — whereas `compare_exchange_strong` fails only on a genuine mismatch.

| Form | Spurious failure | Use when | Cost |
|---|---|---|---|
| `compare_exchange_weak` | Possible | Inside a retry loop | Cheaper on LL/SC (ARM): no inner retry |
| `compare_exchange_strong` | Never | One-shot, not in a loop | Adds an inner loop on LL/SC |

> **Why this matters / cost model.** The distinction is a leak of the hardware model. On ARM's LL/SC, the store-conditional can fail for reasons unrelated to the value — an interrupt, a cache eviction, another core touching the line — so a "weak" CAS exposes that directly. `strong` hides it by retrying internally, which costs an extra loop on LL/SC machines but is free on x86 (where `cmpxchg` never fails spuriously, so weak and strong are identical). The rule: **use `weak` inside a loop** (the spurious failure just costs one more cheap iteration you were going to do anyway) and **`strong` for a single, non-looping CAS** (where a spurious failure would force you to write a loop you otherwise wouldn't need).

---

## 93.5 The CAS Loop Pattern

When the new value depends on the old in a way no `fetch_*` expresses, the universal pattern is a `compare_exchange_weak` loop:

```cpp
// Min standard: C++11. Portable. Atomically apply an arbitrary function f to an atomic.
#include <atomic>
template <typename T, typename F>
void atomic_update(std::atomic<T>& a, F f) {
    T expected = a.load(std::memory_order_relaxed);
    T desired;
    do {
        desired = f(expected);                  // compute new value from current snapshot
    } while (!a.compare_exchange_weak(expected, desired,
                 std::memory_order_acq_rel, std::memory_order_relaxed));
    // On failure, `expected` is refreshed to the current value; recompute and retry.
}
// e.g. atomic_update(maxv, [&](long cur){ return std::max(cur, candidate); });  // atomic max
```
*Listing 93.3 — The canonical CAS loop. `f` is recomputed each retry against the latest value.*

> **Why this matters / cost model.** This pattern makes *any* single-word update atomic and lock-free, and it is the engine inside Treiber stacks, lock-free counters with custom logic, and atomic max/min before C++26 added them natively. Its cost model is contention-sensitive: under low contention it succeeds first try (one `lock cmpxchg`); under high contention it spins, and *each retry re-pays the barrier and re-reads the contended line* — so a hot CAS loop can degrade badly, sometimes worse than a lock. Keep `f` cheap (it runs every retry), minimise the contended footprint, and consider back-off under heavy contention.

---

## 93.6 `atomic_ref`, `atomic<shared_ptr>`, and Atomic Max/Min

Modern C++ broadens the atomic toolkit:

- **`std::atomic_ref<T>`** (C++20) applies atomic operations to an *existing non-atomic object* for the duration of the reference — letting you atomically access a member of a large array or struct without making the whole thing `atomic<>`. The object must be suitably aligned and accessed atomically *everywhere* while any `atomic_ref` to it exists.
- **`std::atomic<std::shared_ptr<T>>`** (C++20) provides lock-free (where supported) atomic shared-pointer operations, replacing the deprecated free-function `atomic_load(shared_ptr*)` overloads — the clean primitive for lock-free structures that need reference-counted nodes.
- **Atomic `fetch_max`/`fetch_min`** (C++26) add native atomic max/min RMW, removing the need for the CAS loop of Listing 93.3 for that common case.

```cpp
// Min standard: C++20. Portable.
#include <atomic>
void bump_element(long* big_array, size_t i) {
    std::atomic_ref<long> ref(big_array[i]);              // atomic view of one element
    ref.fetch_add(1, std::memory_order_relaxed);          // no need to make the whole array atomic
}
```
*Listing 93.4 — `atomic_ref` atomically updates one element of an otherwise-plain array.*

> **Why this matters.** `atomic_ref` solves a long-standing pain: previously, atomically updating one slot of a large buffer meant declaring the whole buffer `atomic<>` (and losing bulk non-atomic access) or hand-rolling intrinsics. `atomic<shared_ptr>` is the correct, standard way to do lock-free node management with reference counting, superseding error-prone manual control blocks. These additions let you express more lock-free designs in standard, portable code rather than dropping to intrinsics — but each carries the same cost model (contended atomics serialise on coherence) and the same alignment requirements.

---

## 93.7 The ABA Problem in Depth

**ABA** is the failure where a CAS succeeds because the value matches, even though the value *changed and changed back* in between, so the success is semantically wrong. The CAS checks the *value*, but the algorithm's correctness depends on *no intervening modification* — and a value can return to its original while everything it referred to has changed.

```text
Atomic head holds pointer A. Thread 1 reads head == A, prepares CAS(A -> A.next).
Thread 1 is preempted.
Thread 2: pop A; pop B; free A; allocate a new node — malloc returns A's old address; push it.
           head == A again, but A.next now points into a different list.
Thread 1 resumes: CAS(head, expected=A, desired=A.next) SUCCEEDS (head is A).
           But A.next is stale — the structure is now corrupt.
```

> **Why this matters.** ABA is specifically a hazard of **pointer-reusing, manually-reclaimed** lock-free structures — the allocator handing back a just-freed address is what closes the loop. It does *not* occur in monotonic-counter algorithms (a `fetch_add` sequence number never repeats) or in structures where freed memory is never reused while a CAS is in flight (which is exactly what hazard pointers and RCU guarantee, Chapter 94). It passes every single-threaded and lightly-contended test and corrupts only under the precise interleaving of free-and-reallocate during a stalled CAS — making it one of the hardest bugs to reproduce. Recognising *whether your design is ABA-susceptible* (does a CAS'd value get freed and possibly reallocated?) is the key diagnostic skill.

---

## 93.8 Defeating ABA: Tags and Double-Width CAS

The classic defence is a **tagged (versioned) pointer**: pack the pointer with a monotonically-increasing counter and CAS the pair atomically. Even if the pointer returns to A, the tag has advanced, so the CAS correctly fails.

```cpp
// Min standard: C++11. x86-64 needs cmpxchg16b for 128-bit atomics; check is_lock_free().
#include <atomic>
#include <cstdint>
struct TaggedPtr {
    void*    ptr;
    uint64_t tag;          // bumped on every successful modification
};
std::atomic<TaggedPtr> head;   // 128-bit atomic on x86-64 (cmpxchg16b)

// On push/pop: CAS the WHOLE {ptr, tag} pair; tag++ each success.
// Because tag is monotonic, A-with-tag-7 != A-with-tag-9 -> ABA is impossible.
```
*Listing 93.5 — Tagged pointer. The double-width CAS compares pointer *and* version, defeating ABA. Requires `cmpxchg16b` (x86-64) / LSE `casp` (ARMv8.1).*

> **Why this matters / cost model.** Tagging is cheap and effective *if* the platform supports double-width CAS — `cmpxchg16b` on x86-64, the `casp` instruction on ARMv8.1 LSE. Where it doesn't, `atomic<TaggedPtr>` silently falls back to a lock (kills lock-freedom — always check `is_lock_free()`). The tag must be wide enough that it cannot realistically wrap during a CAS window (64 bits is ample; narrow tags reintroduce ABA on wraparound). The alternatives — **hazard pointers** and **RCU** (Chapter 94) — defeat ABA differently, by *preventing reuse* of memory a thread might still be examining, which also solves the deeper reclamation problem (when is it safe to free?). Tagging solves ABA but not reclamation; hazard pointers/RCU solve both. That trade-off is exactly why Chapter 94 exists.

> **The discipline.** Atomics are deceptively simple and ruthlessly unforgiving. Use the dedicated `fetch_*` when one exists; reach for a `weak` CAS loop only when the update is genuinely value-dependent; keep the loop body cheap and the contended footprint small; and, the moment your design CAS-es a pointer that can be freed and reallocated, recognise the ABA hazard and choose a defence (tag, hazard pointer, or RCU) deliberately. The next chapter develops the two defences that also answer the reclamation question that atomics alone cannot.
