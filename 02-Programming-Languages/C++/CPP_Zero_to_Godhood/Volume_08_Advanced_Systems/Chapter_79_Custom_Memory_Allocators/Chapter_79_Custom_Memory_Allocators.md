# Chapter 79: Custom Memory Allocators

The general-purpose allocator (`malloc`/`new`) is a marvel of engineering and the wrong tool for a latency-critical hot path: it is thread-safe (so it locks or uses per-thread arenas with their own costs), general (so it searches free lists and splits blocks), and unpredictable (so a single allocation can fault in a page, contend a lock, or take a microsecond). Custom allocators trade generality for control — O(1) allocation, perfect locality, zero fragmentation, and bounded latency — by exploiting what you know about *your* allocation pattern. This chapter builds the major allocator designs and the `std::pmr` framework that deploys them in production, with the cost model and lifetime hazards that govern their use.

## Chapter Roadmap

- 79.1 Why `malloc` Is the Wrong Default on the Hot Path
- 79.2 The Arena / Bump (Linear) Allocator
- 79.3 The Pool (Free-List) Allocator
- 79.4 The Slab Allocator
- 79.5 `std::pmr`: Polymorphic Memory Resources
- 79.6 Alignment
- 79.7 The Allocator Cost Model and Hazards

---

## 79.1 Why `malloc` Is the Wrong Default on the Hot Path

`malloc` must serve every size, from every thread, in any order, forever. To do so it maintains size-class free lists, coalesces freed blocks to fight fragmentation, and synchronises across threads (modern allocators like tcmalloc/jemalloc use per-thread caches, but the slow path still locks a central heap and may call `mmap`). The consequences for a hot path:

- **Latency variance.** Most allocations hit a thread cache (~tens of ns), but a cache miss falls through to a locked central free list, and a heap-growth falls through to an `mmap` syscall and a page fault — a thousandfold tail.
- **Lock contention.** Multi-threaded allocation-heavy code contends the allocator even when the *application* shares nothing.
- **Poor locality.** General allocators scatter related objects across the heap; pointer-chasing them thrashes the cache (Chapter 87).

> **Why this matters.** The fix is not "a faster `malloc`" but *not calling `malloc` on the hot path at all*. Custom allocators win by exploiting structure the general allocator cannot assume: that all allocations in a request share a lifetime (arena), that they are all the same size (pool), or that they are pre-allocated before the latency-critical phase begins (Chapter 97). The cost model is the whole point: a bump allocation is a pointer add and a compare; `malloc` is an unbounded search.

---

## 79.2 The Arena / Bump (Linear) Allocator

The **arena** (a.k.a. bump, linear, or monotonic) allocator owns a contiguous buffer and a cursor. Allocation advances the cursor; individual deallocation is not supported — you free the *entire* arena at once by resetting the cursor.

```cpp
// Min standard: C++11. Portable (alignment via std::align in C++11).
#include <cstddef>
#include <cstdint>
#include <memory>

class ArenaAllocator {
    std::byte* begin_;
    std::byte* cursor_;
    std::byte* end_;
public:
    ArenaAllocator(void* buffer, std::size_t size)
        : begin_(static_cast<std::byte*>(buffer)), cursor_(begin_), end_(begin_ + size) {}

    void* allocate(std::size_t n, std::size_t align = alignof(std::max_align_t)) {
        std::size_t space = static_cast<std::size_t>(end_ - cursor_);
        void* p = cursor_;
        if (!std::align(align, n, p, space)) return nullptr;   // out of space
        cursor_ = static_cast<std::byte*>(p) + n;
        return p;
    }
    void reset() { cursor_ = begin_; }   // frees everything at once — O(1)
};
```
*Listing 79.1 — A bump allocator: O(1) allocate, O(1) bulk reset, no per-object free.*

> **Why this matters / cost model.** Allocation is a few instructions: align the cursor, compare against the end, advance. There is no free list, no locking (when single-threaded or per-thread), no fragmentation, and perfect spatial locality — sequential allocations are adjacent in memory and cache-friendly. The defining trade-off is the *lifetime model*: every object in the arena must die at the same time. This fits request-scoped work perfectly — parse a request, allocate freely from the arena, reset at the end — and is the basis of game-engine frame allocators ("free everything at end of frame") and per-connection buffers. It is the wrong tool when objects have individual, unpredictable lifetimes.

---

## 79.3 The Pool (Free-List) Allocator

A **pool** allocator serves fixed-size blocks from a pre-carved buffer, threading a free list through the free blocks themselves (an *intrusive* free list — the `next` pointer lives in the unused memory). Allocate pops the head; free pushes onto the head. Both are O(1) and there is no fragmentation because every block is interchangeable.

```cpp
// Min standard: C++11. Portable. Fixed block size, single-threaded.
#include <cstddef>
#include <new>

class PoolAllocator {
    struct Node { Node* next; };
    Node* free_list_ = nullptr;
public:
    PoolAllocator(void* buffer, std::size_t count, std::size_t block_size) {
        // block_size must be >= sizeof(Node) and suitably aligned.
        auto* p = static_cast<std::byte*>(buffer);
        for (std::size_t i = 0; i < count; ++i) {
            auto* node = reinterpret_cast<Node*>(p + i * block_size);
            node->next = free_list_;
            free_list_ = node;
        }
    }
    void* allocate() {
        if (!free_list_) return nullptr;
        Node* n = free_list_; free_list_ = n->next; return n;   // pop
    }
    void deallocate(void* p) {
        Node* n = static_cast<Node*>(p); n->next = free_list_; free_list_ = n;  // push
    }
};
```
*Listing 79.2 — Pool allocator with an intrusive free list. Allocate/free are both O(1) with zero fragmentation.*

> **Why this matters / cost model.** The pool is the right allocator when you churn many objects of *one* type with individual lifetimes — network packets, list nodes, particle objects, order objects. Because every block is the same size, there is no fragmentation and no search: allocate and free are each a single pointer swap. Storing the free list *inside* the free blocks costs zero extra memory. The constraints: one fixed size per pool, and exhaustion when the pool is full (production pools chain additional chunks). For multi-threaded use, give each thread its own pool (thread-local) to keep allocation contention-free — the same principle as thread-per-core (Chapter 96).

---

## 79.4 The Slab Allocator

The **slab** allocator (from the Solaris/Linux kernel) generalises the pool: it manages multiple "slabs" (page-sized chunks), each carved into objects of one size class, and crucially *caches constructed objects* so that allocation can return a pre-initialised object and free can retain it rather than destruct-and-reconstruct. Multiple size classes are served by multiple slab caches.

> **Why this matters / cost model.** The slab's two insights are (1) **size-class segregation** keeps each cache fragmentation-free and cache-line-friendly, and (2) **object caching** amortises constructor/destructor cost for expensive-to-initialise objects (a kernel `inode`, a connection object with sub-buffers). The kernel uses slabs because allocating an `inode` thousands of times per second cannot afford a general search or a full re-initialisation each time. In user space, jemalloc and tcmalloc are essentially sophisticated multi-size-class slab/arena hybrids with per-thread caches; understanding the slab explains *why* those allocators are fast. The trade-off is memory overhead (partially-full slabs) and complexity versus a plain pool.

---

## 79.5 `std::pmr`: Polymorphic Memory Resources

Before C++17, an allocator was a *template parameter* — `std::vector<int, MyAlloc>` is a different type from `std::vector<int>`, so a function could not accept "a vector using any allocator" without itself being a template. **`std::pmr`** (Polymorphic Memory Resources) fixes this: the allocator becomes a *runtime* polymorphic object (`std::pmr::memory_resource*`) held by value-erased allocator, so `std::pmr::vector<int>` is one type regardless of the underlying resource.

```cpp
// Min standard: C++17. Portable.
#include <memory_resource>
#include <vector>
#include <array>

void process() {
    std::array<std::byte, 1 << 16> buffer;                       // 64 KiB on the stack
    std::pmr::monotonic_buffer_resource arena{buffer.data(), buffer.size()};
    std::pmr::vector<int> v{&arena};                             // allocates from the stack arena
    std::pmr::vector<std::pmr::string> names{&arena};            // same arena, nested containers

    v.reserve(1000);                                             // no malloc — comes from `buffer`
    names.emplace_back("hot path with zero heap allocation");
    // Everything is freed at once when `arena` is destroyed — no per-element deallocation.
}
```
*Listing 79.3 — A stack-backed monotonic arena driving standard containers with zero heap allocation.*

The standard ships several resources: `monotonic_buffer_resource` (a bump arena), `unsynchronized_pool_resource` / `synchronized_pool_resource` (pools of size classes), `new_delete_resource` (the default), and `null_memory_resource` (allocation fails — useful to *prove* a region is allocation-free).

> **Why this matters.** `pmr` is how custom allocators reach production without rewriting the standard library: you keep `std::vector`/`std::string`/`std::unordered_map`, but back them with an arena or pool chosen at runtime. The killer pattern is a **stack-backed `monotonic_buffer_resource`** feeding a small computation — the containers behave normally but never touch the heap, eliminating the latency variance of §79.1. The cost is one indirect call per allocation (the virtual `do_allocate`), which is negligible compared to the `malloc` it replaces. `null_memory_resource` is a testing superpower: wrap a hot path with it and any stray allocation becomes a hard failure instead of a silent latency spike.

---

## 79.6 Alignment

Every allocator must return suitably-aligned memory. **Alignment** is the requirement that an object's address be a multiple of its `alignof`. Misaligned access is a performance penalty on x86 (an access straddling a cache line costs an extra cycle or a split) and a *fault* (`SIGBUS`) on stricter architectures (some ARM, SPARC). SIMD raises the stakes: AVX loads may require 32-byte and AVX-512 64-byte alignment, and an aligned-load instruction on misaligned data faults.

```cpp
// Min standard: C++17 (over-aligned new). Portable.
#include <cstddef>
struct alignas(64) CacheLineAligned { std::atomic<long> counter; };  // own cache line
// C++17: new CacheLineAligned uses aligned operator new automatically.

// For raw aligned buffers:
void* p = ::operator new(size, std::align_val_t{64});   // C++17 aligned allocation
::operator delete(p, std::align_val_t{64});
```
*Listing 79.4 — Over-alignment via `alignas` and C++17 aligned `new`.*

Key facilities: `alignof(T)`, `alignas(N)`, `std::max_align_t` (the strictest fundamental alignment, what plain `malloc` guarantees), `std::align` (adjust a pointer within a buffer), and C++17's `std::align_val_t` aligned `new`/`delete`.

> **Why this matters.** A custom allocator that ignores alignment is a latent crash: it works for `char` and `int`, then faults the day someone stores a `__m256` or an `alignas(64)` type in it. The arena in Listing 79.1 uses `std::align` precisely to be correct here. Conversely, *deliberate* over-alignment (`alignas(64)`) is the tool for cache-line isolation (false sharing, Chapter 87) and SIMD. Alignment is where the allocator's correctness and the cache hierarchy's performance meet.

---

## 79.7 The Allocator Cost Model and Hazards

| Allocator | Allocate | Free | Fragmentation | Lifetime model | Best for |
|---|---|---|---|---|---|
| `malloc`/`new` | Unbounded (search/lock/syscall) | Unbounded | Fights it | Arbitrary | General code, cold paths |
| Arena/bump | O(1) cursor add | None (bulk reset) | None | All-die-together | Request/frame scoped |
| Pool | O(1) pop | O(1) push | None | Individual, one size | Same-type churn |
| Slab | O(1) + cached ctor | O(1) + cached dtor | Low (per size class) | Individual, few sizes | Kernel objects, hybrids |
| `pmr` (monotonic) | O(1) | None | None | All-die-together | Containers without heap |

**Hazards to respect:**

- **Lifetime bugs.** An arena frees everything on reset; any pointer into it dangles afterward. Returning an arena-allocated object past the arena's scope is a use-after-free.
- **Trivial-destructibility.** A monotonic arena does not call destructors on reset; non-trivially-destructible objects (those owning resources) must be destroyed explicitly or not placed in one — otherwise you leak the resources they own (Chapter 97).
- **Alignment.** Always honour the requested alignment; ignoring it is a portability-dependent crash.
- **Thread safety.** The simple allocators here are single-threaded by design; share them across threads only with external synchronisation, or (better) give each thread its own.
- **Exhaustion.** Fixed buffers run out; decide whether that returns `nullptr`, chains a new chunk, or falls back to `malloc`.

> **The discipline.** Match the allocator to the *lifetime pattern* of the data, not to a vague desire for speed. If all the objects of a phase die together, use an arena; if you churn one type, use a pool; if you need standard containers without heap latency, back them with a stack `monotonic_buffer_resource`. The next chapter on object lifetime and allocation-free hot paths shows how to combine these with placement-`new` and preallocation to reach genuinely zero-allocation steady state — the prerequisite for the deterministic latency that Chapters 101 and 106 demand.
