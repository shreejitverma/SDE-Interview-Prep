# Chapter 97: Object Lifetime, Placement, and Allocation-Free Hot Paths

A latency-critical hot path must not call `malloc` — not because allocation is always slow, but because it is *unpredictable*: most allocations are fast, but one in a million falls through to a locked central heap, an `mmap`, or a page fault, and that one blows the tail-latency budget. Achieving an allocation-free steady state requires mastering the C++ object lifetime model — placement new, explicit destruction, `std::launder`, trivial relocatability — and the preallocation patterns (object pools, fixed buffers) built on it. This chapter develops the lifetime machinery beneath the allocators of Chapter 79 and turns it into the discipline of a zero-allocation hot path.

## Chapter Roadmap

- 97.1 Why Allocation-Free
- 97.2 The Object Lifetime Model
- 97.3 Placement New and Explicit Destruction
- 97.4 `std::launder` and the Lifetime Rules
- 97.5 `start_lifetime_as` and Mapping Bytes to Objects
- 97.6 Trivial Relocatability and Alignment
- 97.7 Allocation-Free Patterns: Pools and Preallocation
- 97.8 Finding and Eliminating Hidden Allocations

---

## 97.1 Why Allocation-Free

`malloc`/`new` has a bimodal cost (Chapter 79): a fast path (thread-cache hit, tens of ns) and a slow path (central-heap lock, `mmap`, or first-touch page fault — microseconds to milliseconds). On a hot path the *average* is irrelevant; the *tail* is everything, and the slow path is a latency cliff you cannot predict or schedule.

> **Why this matters.** A trading system that allocates on its order path will, eventually and unpredictably, hit a slow allocation in the middle of a trade — a millisecond stall that costs money. The defence is not a faster allocator but *no allocation at all* in steady state: allocate everything the hot path needs *before* the hot phase begins, and reuse it. That requires constructing and destroying objects in pre-owned storage by hand, which is precisely what the object lifetime model governs. Allocation-free hot paths are the bridge from the allocator chapter to the determinism the volume demands (Chapters 88, 96, 106).

---

## 97.2 The Object Lifetime Model

C++ distinguishes **storage** (a region of bytes) from an **object** (a typed entity with a lifetime occupying storage). An object's lifetime *begins* when its initialisation completes and *ends* when its destructor is called (or its storage is reused/released). Accessing an object outside its lifetime — before construction or after destruction — is undefined behaviour, even if the bytes are still there.

> **Why this matters.** This separation is what allocators exploit: `::operator new` provides storage *without* an object; placement new starts an object's lifetime *in* existing storage; an explicit destructor call *ends* it without freeing the storage. Manual memory management lives entirely in this gap between storage and object. The hazards all stem from confusing the two: treating raw storage as if it held a live object (using before construction), or a destroyed object's bytes as if still live (use-after-destroy). Every technique below is a precise operation on object lifetime within fixed storage.

---

## 97.3 Placement New and Explicit Destruction

**Placement new** constructs an object at a given address without allocating; the matching operation is an **explicit destructor call** (the storage is managed separately).

```cpp
// Min standard: C++11. Portable. Constructing/destroying in pre-owned storage.
#include <new>
#include <cstddef>

alignas(T) std::byte storage[sizeof(T)];     // raw storage, correctly aligned, no object yet

T* p = ::new (storage) T(args...);           // placement new: begin T's lifetime here
// ... use *p ...
p->~T();                                      // explicit destructor: end T's lifetime
// storage is now raw again; it may be reused for another object.
```
*Listing 97.1 — The placement-new / explicit-destructor pair. Storage and object lifetime are managed independently.*

> **Why this matters / cost model.** This pair is the foundation of every container and pool: `std::vector` placement-news elements into its buffer and explicitly destroys them (Chapter 83); an object pool placement-news into a free slot and destroys on return. The cost is *zero allocation* — construction in storage you already own is just the constructor's work. The hazards are symmetric and severe: forgetting `p->~T()` leaks whatever the object owns (its destructor never runs, even though the bytes are reused); calling it twice (double-destroy) is UB; using `storage` as a `T*` without the placement new is UB (no object exists). The discipline is to treat placement new and the destructor call as a matched pair, exactly as `new`/`delete` are matched.

---

## 97.4 `std::launder` and the Lifetime Rules

When you construct a new object in storage that *previously* held a different object (or const/reference members), the compiler may assume the old object is still there for optimisation purposes. **`std::launder`** (C++17) obtains a pointer the compiler treats as referring to the *new* object, defeating that assumption.

```cpp
// Min standard: C++17. Portable. launder after reusing storage that held a const member.
#include <new>
struct S { const int id; };
alignas(S) std::byte buf[sizeof(S)];

S* a = ::new (buf) S{1};
a->~S();
S* b = ::new (buf) S{2};                 // new object in the same storage
// int x = a->id;                        // UB: `a` may be assumed to still see id==1
int x = std::launder(b)->id;             // correct: launder gives a pointer to the NEW object
```
*Listing 97.2 — `std::launder` is required when reusing storage that held objects with const/reference members.*

> **Why this matters.** This is a subtle, optimization-driven rule, not a runtime cost: `std::launder` emits no code, it merely tells the optimizer "do not assume the old object's invariants hold here." It is *required* when you reuse storage and the type has `const` or reference members (whose values the compiler may cache), and when you obtain a pointer to an object via a different-typed pointer to its storage. Most pool/container code that reuses storage for objects *without* const/reference members does not need it, but the moment such members appear, omitting `launder` is UB that may only manifest at high optimization levels. Knowing *when* it is needed is the mark of correct manual-lifetime code.

---

## 97.5 `start_lifetime_as` and Mapping Bytes to Objects

A recurring systems need is to interpret a buffer of bytes (from the network, a file, shared memory) *as* a C++ object. Historically this was done with `reinterpret_cast`, which is technically UB (no object of that type was ever created in those bytes). C++23's **`std::start_lifetime_as<T>`** legitimises it: it begins the lifetime of a `T` in suitable existing storage *without* running a constructor, for implicit-lifetime types (trivially copyable aggregates).

```cpp
// Min standard: C++23. Portable. Legitimately viewing received bytes as a struct.
#include <memory>
struct Packet { uint32_t seq; uint16_t len; uint16_t flags; };   // implicit-lifetime type

void on_bytes(std::byte* buf, size_t n) {
    Packet* p = std::start_lifetime_as<Packet>(buf);   // begins a Packet's lifetime in buf, no ctor
    use(p->seq);                                       // well-defined, no UB, no copy
}
```
*Listing 97.3 — `start_lifetime_as` (C++23) makes byte-buffer-as-object well-defined for implicit-lifetime types.*

> **Why this matters.** Zero-copy parsing — reading a struct directly out of a received buffer with no deserialization (Chapter 84) — is the standard low-latency technique, and before C++23 it relied on `reinterpret_cast` that was formally UB the optimizer could (and occasionally did) miscompile under strict aliasing. `start_lifetime_as` makes the *intended* operation *legal*: no constructor runs (so no cost), but a real object now exists in those bytes, so accessing it is defined. It only works for **implicit-lifetime types** (trivially copyable, no non-trivial construction) — which is exactly what wire formats should be. This is the correct primitive for the allocation-free, copy-free hot path that reads structured data from buffers.

---

## 97.6 Trivial Relocatability and Alignment

**Relocation** is the move-construct-then-destroy-the-source operation that containers perform when they grow (Chapter 83). A **trivially relocatable** type is one for which this is equivalent to a `memcpy` of the bytes — no per-object move constructor or destructor needs to run. Most types (anything whose move is "steal a pointer" and whose destructor on a moved-from object is a no-op) are trivially relocatable in practice, and a proposed C++ feature makes it a queryable, opt-in property.

> **Why this matters / cost model.** When a `vector` of trivially-relocatable elements grows, it can `memcpy` the whole buffer instead of move-constructing and destroying each element — turning O(N) constructor/destructor calls into one bulk copy, often several times faster and vectorisable. This matters for hot data structures that resize. Until the language feature lands, libraries (folly's `fbvector`, BSL) detect or annotate trivial relocatability to get this win. **Alignment** (Chapter 79) is the companion concern: placement new and `start_lifetime_as` require the storage be aligned to the type's `alignof`, or the access is UB / faults on strict architectures — so the `alignas(T)` on the storage in Listings 97.1–97.2 is not optional.

---

## 97.7 Allocation-Free Patterns: Pools and Preallocation

The patterns that deliver a zero-allocation hot path:

```cpp
// Min standard: C++11. Object pool: preallocated slots, placement-new on acquire, ~T on release.
template <typename T, size_t N>
class ObjectPool {
    alignas(T) std::byte storage_[N * sizeof(T)];
    std::array<T*, N> free_;
    size_t free_count_ = N;
public:
    ObjectPool() { for (size_t i = 0; i < N; ++i)
        free_[i] = reinterpret_cast<T*>(storage_ + i * sizeof(T)); }

    template <typename... A>
    T* acquire(A&&... a) {                                   // O(1), no malloc
        if (free_count_ == 0) return nullptr;               // pool exhausted (no hidden growth!)
        T* slot = free_[--free_count_];
        return ::new (slot) T(std::forward<A>(a)...);        // placement new into a free slot
    }
    void release(T* p) { p->~T(); free_[free_count_++] = p; }// explicit dtor, return slot
};
```
*Listing 97.4 — A fixed-capacity object pool: construct/destroy in preallocated storage, never allocating on the hot path.*

The complete recipe:

- **Preallocate** all buffers, pools, and containers during startup/warmup, sized for the worst case.
- **`reserve()`** every `std::vector`/`std::string` to its maximum so it never reallocates (Chapter 80).
- **Object pools** (Listing 97.4) for churned objects; placement new / explicit destroy, fixed capacity.
- **Ring buffers** (Chapter 77) for queues — fixed storage, no per-message allocation.
- **Stack / `monotonic_buffer_resource`** (Chapter 79) for transient per-operation scratch.

> **Why this matters / cost model.** Each pattern replaces an *unbounded, unpredictable* allocation with a *bounded, O(1), allocation-free* operation. The pool's `acquire` is a pointer pop and a placement new — no lock, no syscall, no page fault (the storage was pre-faulted, Chapter 88). The deliberate `return nullptr` on exhaustion (rather than growing) is a feature: a hot path must have a *bounded* worst case, so it fails loudly rather than silently allocating. The cost is up-front memory (sized for worst case) and the discipline of sizing correctly — exactly the determinism-for-memory trade of the whole volume.

---

## 97.8 Finding and Eliminating Hidden Allocations

Allocations hide in innocent-looking code:

- `std::string` / `std::vector` operations that exceed SSO/capacity (concatenation, `push_back` past `capacity()`, `+`).
- `std::function` capturing too much (heap-allocates beyond its small-buffer).
- `std::shared_ptr(new T)` (two allocations; use `make_shared`, or better a pool).
- Exceptions (throwing allocates the exception object).
- `std::map`/`std::unordered_map` node insertion (per-node allocation).
- Returning containers by value where the callee allocates.

```cpp
// Min standard: C++17. Detect stray allocations on a "hot" path during testing.
#include <memory_resource>
std::pmr::monotonic_buffer_resource arena{buffer, size, std::pmr::null_memory_resource()};
// Containers using `arena` will THROW std::bad_alloc on overflow instead of silently heap-allocating,
// turning a hidden allocation into a loud test failure.
```
*Listing 97.5 — `null_memory_resource` as the upstream turns any unexpected allocation into a hard failure.*

> **Why this matters.** Hidden allocations are the reason a "zero-allocation" path is rarely zero-allocation on the first try — a `std::string` temporary here, a `std::function` capture there. The tools to catch them: route hot-path containers through a `pmr` arena backed by `null_memory_resource` (Listing 97.5) so any overflow throws; hook `operator new` to log/abort on the hot path in test builds; and use heap profilers (`heaptrack`, `tcmalloc`'s profiler) to count allocations per request. You cannot eliminate what you cannot see; making stray allocation a *loud failure* in testing is how you achieve and *keep* an allocation-free hot path.

> **The discipline.** An allocation-free hot path is built by mastering the storage/object distinction: preallocate the storage, construct into it with placement new, destroy with explicit destructor calls, reuse it through pools and rings, and view received bytes with `start_lifetime_as` rather than copies. Then *prove* it is allocation-free by making any stray allocation throw in testing. This closes the memory-management block: with allocation off the hot path and memory pre-faulted and NUMA-local (Chapter 88), the remaining latency villain is the OS boundary — which the next chapters confront.
