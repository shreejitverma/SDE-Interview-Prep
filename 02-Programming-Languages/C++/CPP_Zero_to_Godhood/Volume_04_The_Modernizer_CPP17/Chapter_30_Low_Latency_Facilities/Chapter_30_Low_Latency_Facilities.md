# Chapter 30: Low-Latency Facilities

> *Three C++17 features matter disproportionately to high-frequency trading, kernel, and lock-free systems code, yet sit far enough apart in the standard that they are easy to overlook: the `hardware_*_interference_size` constants that let you align away false sharing portably, the language-level support for over-aligned (extended-alignment) dynamic allocation so `new` finally respects `alignas`, and the `shared_ptr` refinements — array support, `weak_type`, and `reinterpret_pointer_cast` — that close gaps in the smart-pointer toolkit. This chapter gathers them into one focused treatment of the C++17 facilities you reach for when cache lines and alignment are first-class concerns.*

This chapter is authored to complete the C++17 coverage; it has no predecessor source material. Each feature here is about **the memory below the abstraction**. `std::hardware_destructive_interference_size` puts a number on the cache-line padding you previously hard-coded as a magic `64`. Over-aligned `new` makes the language allocate SIMD vectors and cache-line-aligned control blocks correctly, instead of silently under-aligning them and inviting a fault or a slow unaligned access. And the `shared_ptr` improvements let a `shared_ptr` own an array, expose its companion `weak_ptr` type generically, and be `reinterpret_cast` while preserving the shared ownership — small fixes that remove real friction in performance-critical ownership graphs. Together they are the C++17 toolkit for code where the cache hierarchy and the allocator are part of the design, not an afterthought.

---

## Table of Contents

- [30.1 False Sharing and the Interference-Size Constants](#301-false-sharing-and-the-interference-size-constants)
- [30.2 `hardware_destructive_interference_size`: Padding Against False Sharing](#302-hardware_destructive_interference_size-padding-against-false-sharing)
- [30.3 `hardware_constructive_interference_size`: Packing for Locality](#303-hardware_constructive_interference_size-packing-for-locality)
- [30.4 Over-Aligned (Extended-Alignment) Dynamic Allocation](#304-over-aligned-extended-alignment-dynamic-allocation)
- [30.5 Aligned `operator new` and `operator delete`](#305-aligned-operator-new-and-operator-delete)
- [30.6 `shared_ptr` to Arrays](#306-shared_ptr-to-arrays)
- [30.7 `shared_ptr::weak_type`](#307-shared_ptrweak_type)
- [30.8 `reinterpret_pointer_cast`](#308-reinterpret_pointer_cast)
- [30.9 Professional Insights](#309-professional-insights)

---

## 30.1 False Sharing and the Interference-Size Constants

Modern CPUs move memory between cores in **cache-line** units (commonly 64 bytes). **False sharing** occurs when two cores repeatedly write to *different* variables that happen to live in the *same* cache line: the hardware's coherence protocol treats the whole line as contended, bouncing it between the cores' caches and serializing what the programmer thought were independent writes. The classic symptom is a multithreaded counter array or a producer/consumer pair of flags that scales *negatively* with core count.

The fix is to **separate the contended objects onto different cache lines** by padding. The problem, pre-C++17, was that the line size was a platform detail you hard-coded as `64` and hoped was right. C++17 standardizes two constants in `<new>`:

- **`std::hardware_destructive_interference_size`** — the minimum offset between two objects to **avoid** false sharing (i.e., to guarantee they fall on different cache lines). Pad *up to* this to separate contended data.
- **`std::hardware_constructive_interference_size`** — the maximum size of contiguous memory likely to share a cache line, i.e. the granularity within which you should **pack** data you want fetched together.

Both are `constexpr std::size_t`, so they drive `alignas` and array sizing at compile time.

---

## 30.2 `hardware_destructive_interference_size`: Padding Against False Sharing

To stop two hot, independently-written members from sharing a line, align (or pad) each to `std::hardware_destructive_interference_size`. The idiom is an `alignas` on the member or the type:

```cpp
// Listing 30.1: separating two contended atomics onto distinct cache lines
#include <new>
#include <atomic>

struct alignas(std::hardware_destructive_interference_size) PaddedCounter {
    std::atomic<long> value{0};
};

// Two counters written by two different threads — now provably on different lines:
PaddedCounter producer_count;
PaddedCounter consumer_count;
```

Equivalently, when two fields live in one struct, force the second onto the next line:

```cpp
// Listing 30.2: aligning a member to break false sharing within a struct
#include <new>
#include <atomic>

struct SpscQueue {
    std::atomic<size_t> head{0};
    // Push 'tail' onto its own cache line so producer (tail) and consumer (head)
    // writes never contend the same line:
    alignas(std::hardware_destructive_interference_size)
        std::atomic<size_t> tail{0};
};
```

This is the *single highest-leverage* use of the constant: in a single-producer/single-consumer ring buffer, false sharing between the head and tail indices is a textbook throughput killer, and one `alignas` removes it portably — without the magic `64` that breaks on a platform with 128-byte lines (some have effectively that, due to adjacent-line prefetch, which is exactly why `hardware_destructive_interference_size` may be *larger* than the raw line size).

---

## 30.3 `hardware_constructive_interference_size`: Packing for Locality

The complementary goal is **locality**: data that is always accessed together should fit within one cache line so a single fetch brings all of it in. `std::hardware_constructive_interference_size` is the size budget for such co-located data.

```cpp
// Listing 30.3: keeping a hot struct within one cache line
#include <new>
#include <cstdint>

// Keep the frequently-co-accessed fields together under the constructive size,
// so touching one pulls the rest into cache on the same fetch:
struct alignas(std::hardware_constructive_interference_size) HotRecord {
    std::uint64_t key;
    std::uint64_t timestamp;
    std::uint32_t flags;
    // ... keep total size <= hardware_constructive_interference_size ...
};

static_assert(sizeof(HotRecord) <= std::hardware_constructive_interference_size,
              "HotRecord must fit in one cache line for single-fetch locality");
```

The two constants thus express opposite intents — `destructive` says "keep these *apart*," `constructive` says "keep these *together*" — and both replace a hard-coded line size with a value the implementation chooses for the target. They are *hints* (the standard does not require them to equal the true line size), but they are the portable best estimate, and the `static_assert` pattern catches a struct that has grown past a line.

---

## 30.4 Over-Aligned (Extended-Alignment) Dynamic Allocation

A type with an alignment requirement greater than `alignof(std::max_align_t)` — a SIMD vector (`alignas(32)` for AVX, `alignas(64)` for AVX-512), a cache-line-aligned control block, or anything carrying the `alignas` from Section 30.2 — is **over-aligned** (the standard calls it *extended alignment*). Before C++17, `new` ignored such alignment: `new Vec256` could return memory aligned only to 16 bytes, silently under-aligning the object and causing a fault or a slow unaligned access. You had to call `posix_memalign`/`_aligned_malloc` by hand and wrap it in a custom allocator.

**C++17 makes `new` honor extended alignment.** When you `new` an over-aligned type, the compiler routes the allocation to an **alignment-aware `operator new`** overload that takes a `std::align_val_t` argument, so the returned storage meets the type's alignment.

```cpp
// Listing 30.4: new now respects extended alignment automatically
#include <new>
#include <cstddef>

struct alignas(64) CacheLineAligned {   // over-aligned: 64-byte alignment
    std::byte data[64];
};

// C++17: this allocation is guaranteed 64-byte aligned. Pre-C++17 it was NOT.
CacheLineAligned* p = new CacheLineAligned;
// ... use p ...
delete p;   // routed to the matching aligned operator delete

// Arrays of over-aligned types are handled too:
CacheLineAligned* arr = new CacheLineAligned[16];   // each element 64-aligned
delete[] arr;
```

This is the change that makes `alignas` and `new` finally agree. SIMD code, lock-free structures with cache-line-aligned members, and hardware-DMA buffers can be heap-allocated with plain `new`/`make_unique`/`make_shared` and trust the alignment — no bespoke aligned-allocation plumbing.

---

## 30.5 Aligned `operator new` and `operator delete`

The mechanism behind Section 30.4 is a new family of allocation-function overloads (header `<new>`) that take a `std::align_val_t` (a scoped-enum wrapper over `std::size_t`):

```cpp
// Listing 30.5: the aligned allocation-function signatures C++17 added
void* operator new  (std::size_t size, std::align_val_t alignment);
void* operator new[](std::size_t size, std::align_val_t alignment);
void  operator delete  (void* ptr, std::align_val_t alignment) noexcept;
void  operator delete[](void* ptr, std::align_val_t alignment) noexcept;
// (plus the sized-delete and nothrow variants)
```

The compiler selects these overloads automatically whenever the allocated type's alignment exceeds `__STDCPP_DEFAULT_NEW_ALIGNMENT__`. You can also call them explicitly or, more usefully, **override them** to route over-aligned allocations through your own aligned allocator while leaving normally-aligned allocations on the default path:

```cpp
// Listing 30.6: a class-specific aligned operator new
#include <new>
#include <cstdlib>

struct Avx512Block {
    alignas(64) float lanes[16];

    // Custom aligned allocation for this type:
    static void* operator new(std::size_t n, std::align_val_t al) {
        return ::operator new(n, al);   // forward to the global aligned new
    }
    static void operator delete(void* p, std::align_val_t al) noexcept {
        ::operator delete(p, al);
    }
};
```

The matching aligned `operator delete` **must** be used to free aligned storage — which the compiler arranges automatically for `delete`, and which is why over-aligned types now get their own delete overload. Mixing an aligned allocation with a non-aligned deallocation is undefined behavior, so when overriding, provide the pair.

---

## 30.6 `shared_ptr` to Arrays

C++11's `shared_ptr` could technically point at an array only with a hand-written deleter (`shared_ptr<T>(new T[n], [](T* p){ delete[] p; })`), and it offered no `operator[]`. C++17 adds **first-class array support**: `std::shared_ptr<T[]>` (and fixed-extent `T[N]`) knows to call `delete[]`, and exposes `operator[]` for element access.

```cpp
// Listing 30.7: shared_ptr that owns an array correctly
#include <memory>

// C++17: the T[] specialization calls delete[] automatically and indexes:
std::shared_ptr<int[]> arr(new int[100]);

arr[0] = 42;            // operator[] — no .get()[0] dance
arr[1] = 7;
// On destruction, delete[] is invoked correctly (not delete).
```

This brings `shared_ptr` to parity with `unique_ptr<T[]>` (which had array support since C++11) and removes the two classic array-`shared_ptr` bugs: forgetting the custom `delete[]` deleter (which invokes `delete` on an array — undefined behavior) and the verbose `ptr.get()[i]` indexing. For shared ownership of a buffer — a reference-counted ring of frames, a shared lookup table — `shared_ptr<T[]>` is now the correct, terse spelling. (`std::make_shared<T[]>(n)` array support followed in C++20; in C++17 construct from `new T[n]`.)

---

## 30.7 `shared_ptr::weak_type`

C++17 adds the nested alias **`std::shared_ptr<T>::weak_type`**, which names the corresponding `std::weak_ptr<T>`. This lets generic code obtain the companion weak-pointer type from a `shared_ptr` type **without re-spelling the element type** — the same convenience `value_type`/`iterator` aliases provide for containers.

```cpp
// Listing 30.8: deriving the weak_ptr type generically
#include <memory>

template <typename SharedPtr>
class Cache {
    // Get the matching weak_ptr type without naming SharedPtr's element type:
    using Weak = typename SharedPtr::weak_type;
    std::vector<Weak> observers_;   // weak references that don't keep objects alive
public:
    void observe(const SharedPtr& sp) { observers_.push_back(sp); }
};
```

Without `weak_type`, generic code had to extract the element type (via `SharedPtr::element_type`) and re-form `std::weak_ptr<element_type>` — workable but noisy and easy to get wrong with arrays or cv-qualifiers. `weak_type` is a small completeness fix that makes ownership-graph templates (caches, observer registries, parent/child back-references) cleaner.

---

## 30.8 `reinterpret_pointer_cast`

C++11 provided `static_pointer_cast`, `dynamic_pointer_cast`, and `const_pointer_cast` to convert a `shared_ptr<T>` to a `shared_ptr<U>` **while sharing ownership** (incrementing the same control block). C++17 completes the set with **`std::reinterpret_pointer_cast`**, the shared-ownership analogue of `reinterpret_cast`.

```cpp
// Listing 30.9: reinterpreting a shared_ptr while preserving the control block
#include <memory>
#include <cstdint>

std::shared_ptr<std::uint8_t> bytes(new std::uint8_t[sizeof(Header)],
                                    std::default_delete<std::uint8_t[]>());

// Reinterpret the shared byte buffer as a Header, sharing the SAME ownership:
std::shared_ptr<Header> hdr = std::reinterpret_pointer_cast<Header>(bytes);
// hdr and bytes now share one control block; the buffer lives until BOTH release.
```

The crucial property is that the cast produces a `shared_ptr` that **shares the original's reference count**, rather than a raw `reinterpret_cast<Header*>(bytes.get())` that would point into the buffer with no ownership and dangle when `bytes` died. This matters in low-level code that overlays a typed view on a shared raw buffer — parsing a network/DMA buffer as a header type, or aliasing a shared byte arena — where you need the typed pointer to keep the underlying storage alive. As with the C++11 pointer casts, the underlying conversion must be a valid `reinterpret_cast`, and the usual aliasing and lifetime caveats of `reinterpret_cast` apply.

---

## 30.9 Professional Insights

**Pad contended data with `hardware_destructive_interference_size`, not a magic `64`.** False sharing between independently-written hot variables — SPSC queue head/tail, per-thread counters, lock-free node fields — silently serializes throughput and scales negatively with cores. One `alignas(std::hardware_destructive_interference_size)` separates them portably, and because the constant may exceed the raw line size (to account for adjacent-line prefetch), it is more correct than the hard-coded value it replaces. This is the highest-impact, lowest-effort latency fix in the chapter.

**Rely on C++17's aligned `new` instead of bespoke aligned allocators.** Over-aligned types — SIMD vectors, cache-line-aligned control blocks, DMA buffers — are now allocated correctly by plain `new`, `make_unique`, and `make_shared`, because the compiler routes them to the `align_val_t` overloads. Drop the `posix_memalign`/`_aligned_malloc` wrappers; and when you *do* override `operator new` for an over-aligned type, always provide the matching aligned `operator delete`, since mismatched alignment in free is undefined behavior.

**Use `shared_ptr<T[]>` for shared array ownership, and prefer the pointer-cast family over raw casts.** The array specialization calls `delete[]` and indexes with `operator[]`, eliminating the two classic hand-rolled-deleter bugs. And when you must retype a `shared_ptr`, use `static_pointer_cast`/`dynamic_pointer_cast`/`const_pointer_cast`/`reinterpret_pointer_cast` rather than casting `get()` — only the pointer-cast family preserves the shared control block, so the storage stays alive as long as any typed view references it. A raw `reinterpret_cast<U*>(sp.get())` is a dangling pointer waiting to happen.

**Treat `weak_type` and the interference constants as the "writes intent into the type system" tools they are.** `SharedPtr::weak_type` lets ownership-graph templates name their non-owning references without re-deriving element types; the `static_assert(sizeof(T) <= hardware_constructive_interference_size)` pattern makes "this struct must stay within one cache line" a compile-time contract rather than a comment. In latency-critical code, encoding these layout and ownership intentions where the compiler can check them is what keeps a fast design fast as it evolves.
