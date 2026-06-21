# Chapter 87: The Cache Hierarchy, False Sharing, and Cache-Conscious Layout

For most real workloads the bottleneck is not the CPU's ability to compute but its ability to *feed* the computation with data, and the cache hierarchy is the entire apparatus for doing so. A single miss to main memory costs as much as ~300 instructions; whether your data is laid out so the hardware can predict and prefetch it is, for memory-bound code, the difference between fast and unusably slow. This chapter explains what the cache actually does — lines, levels, associativity, prefetching, coherence — and turns it into concrete layout disciplines: locality, hot/cold splitting, and the elimination of false sharing.

## Chapter Roadmap

- 87.1 Why the Cache Exists: The Memory Wall
- 87.2 Cache Lines: The Unit of Transfer
- 87.3 The Levels: L1, L2, L3, and Inclusivity
- 87.4 Associativity and Conflict Misses
- 87.5 Spatial and Temporal Locality
- 87.6 Hardware Prefetching
- 87.7 Cache Coherence and the Cost of Sharing
- 87.8 False Sharing and Interference Sizes
- 87.9 Cache-Conscious Data Layout

---

## 87.1 Why the Cache Exists: The Memory Wall

CPU speed grew far faster than DRAM speed for decades, opening the **memory wall**: a core can execute ~4 instructions per ~0.3 ns cycle, but a DRAM access takes ~100 ns — time for ~1000+ instructions. If every memory access went to DRAM, the core would be idle >99% of the time. **Caches** are small, fast SRAM memories that hold recently- and soon-to-be-used data close to the core, bridging the gap.

> **Why this matters.** The cache exists because of a brute physical fact: fast memory is small and expensive, large memory is slow. The entire art of cache-conscious programming is arranging your data so that the small fast memory holds what you need *when* you need it. You cannot make DRAM faster, but you can dramatically change your *hit rate* by how you lay out and traverse data — and hit rate, more than any algorithmic constant, governs memory-bound performance.

---

## 87.2 Cache Lines: The Unit of Transfer

The cache does not move individual bytes; it moves fixed-size **cache lines** — almost universally **64 bytes** on x86-64 and AArch64. Touching a single byte fetches its entire 64-byte line. Adjacent data is therefore brought in "for free," and data you touch together should be *stored* together.

> **Why this matters / cost model.** The 64-byte line is the most important number in cache-conscious design. It means: (1) a struct smaller than 64 bytes that you access fully costs one miss, not one per field; (2) iterating an array of contiguous elements amortises one miss over many elements (the line holds 16 `int`s or 8 `double`s); (3) a linked list whose nodes are scattered pays a full miss *per node*, because each node sits on a different line with nothing useful alongside it. The line is also the unit of *coherence* (§87.7) — which is why two unrelated variables on the same line cause false sharing (§87.8). Almost every layout rule in this chapter is a consequence of the 64-byte granularity.

---

## 87.3 The Levels: L1, L2, L3, and Inclusivity

Caches form a hierarchy, each level larger and slower than the one above:

| Level | Typical size | Latency | Scope |
|---|---|---|---|
| L1 (split I/D) | 32–64 KB each | ~4 cycles (~1 ns) | Per core |
| L2 | 256 KB–2 MB | ~12 cycles (~4 ns) | Per core (usually) |
| L3 (LLC) | 8–64 MB | ~40 cycles (~15 ns) | Shared across cores |
| DRAM | GBs | ~100 ns | All cores |

An access checks L1, then L2, then L3, then DRAM, paying the cumulative latency on a miss. The L1 instruction and data caches are separate; L3 is shared and is where inter-core data sharing happens.

> **Why this matters.** The order-of-magnitude latency *steps* between levels are what make working-set size matter so much: a hot loop whose data fits in L1 runs an order of magnitude faster than one whose data spills to L2, and another order faster than one that spills to DRAM. This is why **blocking/tiling** (restructuring a computation to operate on cache-sized chunks) can transform matrix and array kernels — it keeps the active working set inside L1/L2 instead of streaming from DRAM. Knowing your data structure's size relative to L1/L2/L3 is the first question in memory-bound optimization.

---

## 87.4 Associativity and Conflict Misses

A cache cannot place any line anywhere; an **N-way set-associative** cache maps each address to a specific *set* of N possible slots (using middle address bits). If more than N hot lines map to the same set, they evict each other — a **conflict miss** — even though the cache as a whole has room.

> **Why this matters / cost model.** Conflict misses are the explanation for the notorious "power-of-two stride" pathology: iterating an array with a stride that is a large power of two (or accessing columns of a 2D array sized to a power of two) makes many addresses map to the same set, thrashing it while the rest of the cache sits idle. The classic fix is **padding** array dimensions to avoid power-of-two strides, so accesses spread across sets. This is a subtle, profiler-only bug (it looks like an inexplicable slowdown at a specific size); awareness of associativity is what lets you recognise and fix it.

---

## 87.5 Spatial and Temporal Locality

Two forms of locality make caches effective:

- **Spatial locality:** if you access an address, you are likely to access nearby addresses soon. Satisfied by contiguous, sequential access (arrays, struct fields).
- **Temporal locality:** if you access an address, you are likely to access it again soon. Satisfied by reusing data while it is still cached (loop blocking, keeping hot data small).

> **Why this matters.** These are the two properties your data structures and access patterns must exhibit to be cache-friendly, and they directly explain the volume's recurring advice. *Prefer `std::vector` over `std::list`*: the vector has spatial locality (contiguous), the list does not (scattered nodes). *Prefer structure-of-arrays for partial-field access*: it gives spatial locality over the fields you actually touch (Chapter 90). *Block your loops*: blocking creates temporal locality by reusing a tile before evicting it. Cache-conscious design is, operationally, the pursuit of spatial and temporal locality.

---

## 87.6 Hardware Prefetching

The CPU includes **hardware prefetchers** that detect access patterns — sequential streams, constant strides — and fetch lines *before* the program requests them, hiding the DRAM latency behind useful work. A predictable linear scan is therefore far faster than its miss count suggests: the prefetcher stays ahead of the loop.

```cpp
// Min standard: C++11. Sequential access the prefetcher loves.
long sum = 0;
for (size_t i = 0; i < n; ++i) sum += a[i];   // unit stride: prefetcher hides DRAM latency

// Min standard: C++11. Random access defeats the prefetcher.
long sum2 = 0;
for (size_t i = 0; i < n; ++i) sum2 += a[index[i]];  // random index: a miss per access, no prefetch
```
*Listing 87.1 — Sequential access is prefetched; random/pointer-chasing access is not.*

> **Why this matters / cost model.** The prefetcher is why the *pattern* of access matters as much as the *amount*: a sequential scan of 1 GB can be near-bandwidth-limited (fast), while randomly accessing 1 GB pays a full ~100 ns miss per access (catastrophically slow) — same data, 50× difference. This is the deep reason pointer-chasing (linked lists, trees, hash tables with chaining) is slow: each `->next` is a data-dependent load the prefetcher cannot predict, serialising full misses. Software prefetch hints (`__builtin_prefetch`, non-portable) can help irregular-but-known-ahead patterns, but the first-order fix is to make access sequential.

---

## 87.7 Cache Coherence and the Cost of Sharing

When multiple cores cache the same memory, hardware **cache coherence** (typically a MESI-family protocol: Modified/Exclusive/Shared/Invalid) keeps them consistent: before a core writes a line, it must gain *exclusive* ownership, **invalidating** every other core's copy. A subsequent read by another core then misses and re-fetches the modified line.

> **Why this matters / cost model.** Coherence makes *writes to shared lines* expensive: each write that another core has cached triggers an invalidation and a coherence round-trip (tens to hundreds of cycles, via the shared L3 or across sockets). A line repeatedly written by one core and read by another "ping-pongs" between them. This is the hardware reality beneath the memory model (Chapter 76): an atomic RMW or a `seq_cst` store is expensive partly because it must win exclusive ownership of the line. The design lesson: minimise *shared mutable* cache lines — the cheapest concurrency keeps each core writing its own lines (thread-per-core, Chapter 96; per-thread counters, Chapter 78).

---

## 87.8 False Sharing and Interference Sizes

**False sharing** is the pathological case of §87.7: two cores write to *logically independent* variables that happen to share one 64-byte line. The coherence protocol does not know the variables are independent — it tracks lines — so every write by one core invalidates the other's cached line, forcing a coherence round-trip even though no data is actually shared.

```cpp
// Min standard: C++17. Portable.
#include <atomic>
#include <new>

// BAD: both counters on one line -> false sharing serialises two threads.
struct Bad { std::atomic<long> a; std::atomic<long> b; };   // a and b within 64 bytes

// GOOD: each counter on its own line.
struct Good {
    alignas(std::hardware_destructive_interference_size) std::atomic<long> a;
    alignas(std::hardware_destructive_interference_size) std::atomic<long> b;
};
```
*Listing 87.2 — Padding to `hardware_destructive_interference_size` eliminates false sharing.*

C++17 provides two named constants in `<new>`:

- **`hardware_destructive_interference_size`** — the minimum offset to put two objects on *different* lines (avoid false sharing). Pad hot per-thread data to this.
- **`hardware_constructive_interference_size`** — the maximum size to keep two objects on the *same* line (promote true sharing / locality). Group data used together within this.

> **Why this matters / cost model.** False sharing can make a parallel program *slower than serial*: N threads each incrementing their "own" counter, all on one line, serialise on coherence and run worse than one thread, because every increment ping-pongs the line. It is invisible in the source (the variables are distinct) and only `perf c2c` (cache-to-cache) or a layout audit reveals it. The fix is mechanical — pad each hot per-thread datum to its own line — but you must *know to look*. This is why every per-thread counter, queue index (Chapter 77's SPSC ring), and lock in this volume is cache-line aligned.

---

## 87.9 Cache-Conscious Data Layout

The synthesis: lay out data so the bytes the hardware fetches are the bytes you use.

- **Contiguous over linked.** Arrays/`vector` give spatial locality and prefetching; lists/trees pay a miss per node.
- **Hot/cold splitting.** Separate frequently-accessed ("hot") fields from rarely-accessed ("cold") ones so a hot scan does not drag cold bytes into cache. Put the cold fields behind a pointer or in a parallel array.
- **Structure-of-arrays** when you access one field across many objects (Chapter 90).
- **Pack hot data within a line; pad shared-mutable data across lines** (`constructive`/`destructive` sizes).
- **Size working sets to the cache.** Block/tile computations to keep the active set in L1/L2.

```cpp
// Min standard: C++11. Hot/cold splitting.
// BAD: cold 'description' bloats the line during a hot scan over 'price'.
struct OrderBad { double price; int qty; char description[200]; };   // > 3 lines each

// GOOD: hot fields packed; cold data behind a pointer (touched only when needed).
struct OrderGood { double price; int qty; const char* description; }; // hot part < 1 line
```
*Listing 87.3 — Hot/cold splitting keeps the hot working set dense.*

> **The discipline.** Memory-bound performance is won at design time by data layout, not at the end by micro-optimization. Ask of every hot data structure: *Is it contiguous? Does a cache line carry only bytes I use? Are independent per-thread writes on separate lines? Does the working set fit in cache?* These questions, answered with the 64-byte line and the level latencies in mind, deliver the largest performance gains in this volume — often 2–50×. The next chapter extends the model below the cache, to the virtual-memory machinery (TLB, pages, NUMA) that turns your pointers into physical addresses.
