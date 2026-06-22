# Chapter 109: High-Performance Data Structures

When `std::unordered_map` or `std::map` is the bottleneck, the answer is rarely a cleverer algorithm with the same big-O — it is a data structure designed around the *hardware*: one that minimises cache misses, exploits SIMD, avoids pointer chasing, and reuses memory without reallocation. This chapter presents the structures that production low-latency systems actually use — the Disruptor, the Swiss table, cache-conscious tries, and the slot map — each justified by the cache and concurrency cost models of Volume 8, not by asymptotic complexity alone.

## Chapter Roadmap

- 109.1 Why Big-O Is Not Enough
- 109.2 The Disruptor: A Ring Buffer on Steroids
- 109.3 The Swiss Table: Open Addressing with SIMD Metadata
- 109.4 Cache-Conscious Tries: Judy Arrays and Burst Tries
- 109.5 The Slot Map: Stable Handles and Generational Indices
- 109.6 Choosing a Structure

---

## 109.1 Why Big-O Is Not Enough

Asymptotic complexity assumes every operation costs the same — but on real hardware a cache miss costs ~100× a cache hit (Chapter 87), so an `O(n)` linear scan of contiguous memory routinely beats an `O(log n)` tree traversal that chases pointers across the heap. The high-performance data structures in this chapter are designed by the *constant factors* the cost model exposes: cache-line utilisation, branch predictability, prefetchability, SIMD width, and (for concurrent structures) coherence traffic.

> **Why this matters.** `std::map` (a red-black tree) and `std::unordered_map` (chained hashing) are *correct* and *general*, but both are pointer-chasing structures: each node is a separate heap allocation, so traversal is a sequence of cache misses (Chapter 80's "cache is king"). The structures here trade generality for hardware sympathy — contiguous storage, SIMD probing, handle-based access — and routinely run several times faster for the same big-O. The lesson, restated from Volume 8: for memory-bound workloads, *layout and access pattern* dominate *operation count*.

---

## 109.2 The Disruptor: A Ring Buffer on Steroids

The **Disruptor** (LMAX) is a pre-allocated, lock-free ring buffer for high-throughput inter-thread messaging — the canonical HFT hand-off structure. It generalises the SPSC ring of Chapter 77 to multiple consumers arranged in a dependency graph, using published sequence numbers and memory barriers instead of locks.

Its design choices map one-to-one onto the cost models:

- **Pre-allocated entries** — the ring's slots are allocated once at startup; the hot path never calls `malloc`, so there is no allocation latency, no fragmentation, no GC pause (Chapters 79, 97).
- **Sequence numbers** — a producer claims slot *n*, writes it, then *publishes* by advancing an atomic sequence with a release store; consumers spin on the sequence with an acquire load (Chapter 76's release/acquire publication).
- **Barriers, not locks** — consumers wait on the slowest upstream sequence rather than taking a lock, so a slow consumer never blocks a producer with a held mutex.
- **Cache-line padding** — every sequence counter is padded to 64 bytes (`alignas(64)`) to prevent false sharing between the producer's and consumers' counters (Chapter 87).
- **Batching** — a consumer that finds the published sequence has advanced by *k* processes all *k* entries in one batch, amortising the synchronisation cost.

> **Why this matters / cost model.** The Disruptor's insight is that a queue's real cost is not the enqueue/dequeue *logic* but the *allocation, the contended head/tail pointer, and the cache-line ping-pong* of a naive concurrent queue. By pre-allocating, giving each participant its own padded sequence, and batching, it reduces per-message cost to a handful of cache-hot operations and sustains tens of millions of messages per second at sub-microsecond latency. The trade-off (Chapter 78): it is a *fixed-capacity, busy-spinning* design that dedicates a core to polling — right only when latency dominates and a core can be spared (Chapter 96).

---

## 109.3 The Swiss Table: Open Addressing with SIMD Metadata

The **Swiss table** (Google's `absl::flat_hash_map`, and the inspiration for many modern hash maps) replaces `std::unordered_map`'s chained nodes with **open addressing** plus a separate array of **control bytes**, and probes them with SIMD.

The structure:

- Two parallel arrays: a **control byte** array (one byte of metadata per slot) and a **slot** array (the actual key/value pairs), all contiguous.
- Each **control byte** holds 7 bits of the key's hash plus 1 bit marking empty/deleted.
- **SIMD probing:** to look up a key, load 16 control bytes into a vector register (SSE/AVX) and compare all 16 against the key's 7-bit hash *in parallel* (Chapter 92), producing a bitmask of candidate slots — then verify the full key only for matches.

```cpp
// Min standard: C++17. Conceptual SIMD probe (x86 SSE2, non-portable). absl/folly do this for real.
// __m128i ctrl = _mm_loadu_si128(&control[group]);    // 16 control bytes at once
// __m128i want = _mm_set1_epi8(h2);                    // the 7-bit hash, broadcast
// int matches = _mm_movemask_epi8(_mm_cmpeq_epi8(ctrl, want));  // bitmask of matching slots
// for each set bit: compare the full key in the slot array (rarely needed)
```
*Listing 109.1 — Swiss-table probing: 16 candidate slots compared in one SIMD instruction. x86-specific intrinsics.*

> **Why this matters / cost model.** `std::unordered_map`'s chaining means each lookup follows a pointer to a heap-allocated node, then possibly another — a sequence of cache misses (Chapter 87). The Swiss table keeps everything contiguous (open addressing, no per-node allocation) and the control-byte array is *tiny* (one byte per slot), so a whole group of 16 candidates fits in a single cache line and is probed with one SIMD compare. The result is **drastically fewer cache misses** and far better load behaviour, typically 2–3× faster than `std::unordered_map` for the same operations. The costs: open addressing degrades under high load factors (it resizes to stay sparse, using more memory than chaining), tombstones from deletions accumulate (requiring periodic rehash), and the SIMD probe is ISA-specific (portable libraries provide fallbacks). For hot-path hashing, a Swiss-style flat map is the modern default over `std::unordered_map`.

---

## 109.4 Cache-Conscious Tries: Judy Arrays and Burst Tries

For *ordered* integer-keyed maps where `std::map`'s pointer-chasing tree is too slow, **adaptive tries** restructure themselves by population density to stay cache-efficient:

- **Judy arrays** are digital trees whose nodes *change representation* based on how full they are — a sparse node is a compact linear list, a denser one a bitmap, a full one a direct sub-array — so memory is proportional to population and access stays cache-friendly across the whole density range.
- **Burst tries** keep small collections of keys in cache-friendly leaf buckets and only "burst" a bucket into a sub-trie when it grows too large, amortising structure overhead.
- **ART (Adaptive Radix Tree)** similarly uses four node types of increasing capacity (Node4/16/48/256), chosen by child count, achieving B-tree-like performance with trie simplicity — widely used in modern databases.

> **Why this matters / cost model.** A naive trie wastes enormous memory (a 256-way node per level even for sparse data) and a naive balanced tree chases a pointer per level. Adaptive structures solve both by *changing the node representation to match the data*: dense regions get array-like O(1)-per-level nodes, sparse regions get compact ones, and small nodes (Node4/16) fit in one or two cache lines and can be SIMD-searched. The payoff is ordered-map performance approaching a hash map's, with range queries a hash map cannot do. The cost is implementation complexity — these are not structures you write casually; use a vetted library (the point is to know they exist and why they win). They are the data-structure analogue of the data-oriented-design principle (Chapter 90): match the layout to the data.

---

## 109.5 The Slot Map: Stable Handles and Generational Indices

A **slot map** provides O(1) insertion, deletion, and access while handing out stable **handles** (small integer keys) instead of pointers — solving the problem that pointers and indices into a `std::vector` are *invalidated* by reallocation or element removal (Chapters 83, 90).

The key trick is the **generational index**: a handle is `{index, generation}`. Each slot carries a generation counter that is incremented when the slot is freed; an access validates that the handle's generation matches the slot's current generation, so a stale handle to a *reused* slot is detected and rejected.

```cpp
// Min standard: C++11. Conceptual slot map with generational handles.
struct Handle { uint32_t index; uint32_t generation; };
template <typename T>
class SlotMap {
    struct Slot { T value; uint32_t generation = 0; bool alive = false; };
    std::vector<Slot> slots_;
    std::vector<uint32_t> free_list_;
public:
    Handle insert(T v) {
        uint32_t i; /* pop free slot or push_back */
        // ... set slots_[i].value = v; alive = true ...
        return {i, slots_[i].generation};
    }
    T* get(Handle h) {                       // returns nullptr if the handle is stale
        if (h.index >= slots_.size()) return nullptr;
        Slot& s = slots_[h.index];
        if (!s.alive || s.generation != h.generation) return nullptr;   // generation check
        return &s.value;
    }
    void erase(Handle h) {
        if (auto* p = get(h)) { slots_[h.index].alive = false; ++slots_[h.index].generation; /* free */ }
    }
};
```
*Listing 109.2 — A slot map: stable integer handles with a generation counter that detects use-after-free.*

> **Why this matters / cost model.** Slot maps are the backbone of game-engine entity systems and any system with churning objects referenced from many places (Chapter 90's ECS). They give you the *stability* of a handle (it survives the container growing or other elements being removed — unlike a raw pointer or index into a `vector`) and the *safety* of a generation check (accessing a handle whose slot was reused for a new object returns `nullptr` rather than the wrong object — the data-structure equivalent of catching a use-after-free, Chapter 97). Storage is contiguous (cache-friendly iteration), access is O(1), and handles are small (often 32-bit, half the size of a 64-bit pointer). The cost is a generation field per slot and an indirection through the index. For long-lived, frequently-referenced, churning objects, the slot map is the right structure where naive pointers would dangle.

---

## 109.6 Choosing a Structure

| Need | Standard structure | High-performance structure | Why it wins |
|---|---|---|---|
| Inter-thread message queue | `std::queue` + mutex | Disruptor / SPSC ring | No allocation, no lock, cache-padded |
| Hash map (hot path) | `std::unordered_map` | Swiss / flat hash map | Contiguous + SIMD probe, fewer misses |
| Ordered integer map | `std::map` | Judy array / ART | Adaptive nodes, cache-friendly, range queries |
| Stable references to churning objects | `vector` + indices (unsafe) | Slot map | Stable handles + generation safety |

> **The discipline.** Reaching past the standard containers is justified only when profiling (Chapter 103) shows one is the bottleneck *and* the cost model explains why — almost always cache misses from pointer chasing or contention from locking. The structures here win by being *hardware-sympathetic*: contiguous storage to maximise cache-line utilisation and prefetching, SIMD to probe many candidates at once, pre-allocation to avoid hot-path `malloc`, and handles to keep references stable and small. Prefer a vetted implementation (abseil, folly, boost) over hand-rolling these — they are subtle — but know which exists and why, so you can choose the structure whose cost model fits your access pattern. The remaining chapters apply this hardware-first mindset across the specialized domains where C++ dominates.
