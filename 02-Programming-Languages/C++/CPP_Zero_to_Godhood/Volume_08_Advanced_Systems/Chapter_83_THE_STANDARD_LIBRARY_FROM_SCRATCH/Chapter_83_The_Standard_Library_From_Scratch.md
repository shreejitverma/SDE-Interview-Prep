# Chapter 83: The Standard Library From Scratch

The standard library looks like magic until you implement it — then it becomes a set of precise engineering decisions about memory, lifetime, exception safety, and atomic synchronisation, every one of which has a cost you can name. This chapter reconstructs the two most instructive components, `vector` and `shared_ptr`, from raw memory up. The goal is not to replace the standard library (it is faster and more correct than yours will be) but to understand *what every line of it is buying you*, so you can read its performance, predict its allocations, and know when its guarantees cost more than your hot path can afford.

## Chapter Roadmap

- 83.1 Why Reimplement the Standard Library
- 83.2 `my::vector`: Raw Memory, Growth, and Placement New
- 83.3 Exception Safety in `reallocate`
- 83.4 Move-vs-Copy on Growth and `noexcept`
- 83.5 `my::shared_ptr`: The Control Block
- 83.6 `make_shared`, `weak_ptr`, and the Cost Model
- 83.7 What Reimplementation Teaches

---

## 83.1 Why Reimplement the Standard Library

Implementing core STL components reveals their cost. `std::vector`'s amortised-O(1) `push_back` hides a reallocation strategy; `std::shared_ptr`'s convenience hides an atomic control block and a second allocation. You cannot reason about the performance of code built on these abstractions without knowing what they do underneath.

> **Why this matters.** Every `push_back` might reallocate and move; every `shared_ptr` copy is an atomic increment; every `shared_ptr` you pass by value is two atomic operations (copy + destroy). On a hot path, those are exactly the costs you must see through. Reimplementing them once turns "the vector grew" and "I shared a pointer" from invisible into measurable.

---

## 83.2 `my::vector`: Raw Memory, Growth, and Placement New

A `vector` separates two concepts the language usually fuses: **allocation** (obtaining raw storage) and **construction** (running a constructor in that storage). It allocates capacity for more elements than it currently holds, and constructs elements into that raw storage on demand using **placement new**.

```cpp
// Min standard: C++11. Portable. Simplified; omits allocator, full API.
#include <cstddef>
#include <new>
#include <utility>

template <typename T>
class Vector {
    T* data_ = nullptr;
    std::size_t sz_ = 0;
    std::size_t cap_ = 0;
public:
    void push_back(const T& val) {
        if (sz_ == cap_) reallocate(cap_ == 0 ? 1 : cap_ * 2);  // geometric growth
        ::new (data_ + sz_) T(val);   // placement new: construct in place, no allocation
        ++sz_;
    }
    std::size_t size() const { return sz_; }
    std::size_t capacity() const { return cap_; }
    T& operator[](std::size_t i) { return data_[i]; }
private:
    void reallocate(std::size_t new_cap);   // see Listing 83.2
};
```
*Listing 83.1 — `vector` core: raw storage (`::operator new`) plus in-place construction (placement new).*

Two facilities make this work:

- **`::operator new(n)`** allocates `n` raw bytes *without* constructing anything — unlike `new T[]`, which would default-construct every element. This lets capacity exist without live objects in it.
- **Placement new** `::new (p) T(val)` runs `T`'s constructor at address `p`, constructing exactly one object in pre-allocated storage. Symmetrically, you must call `p->~T()` explicitly to destroy it (the storage is freed separately).

> **Why this matters / cost model.** **Geometric growth** (doubling) is what makes `push_back` *amortised* O(1): N pushes trigger O(log N) reallocations moving O(N) elements total, so the average per-push is constant. Linear growth (adding a fixed amount) would make it O(N) per push and O(N²) overall — a classic performance bug. The separation of allocation from construction is the whole reason `vector` is efficient: it allocates once for many elements and constructs them lazily, rather than allocating per element like a linked list. This is also why `reserve()` (Chapter 80) is so valuable — it pre-pays the geometric growth in one shot.

---

## 83.3 Exception Safety in `reallocate`

The naive `reallocate` is a correctness minefield: it must allocate new storage, move/copy existing elements over, destroy the old ones, and free the old storage — and any of those copy/move constructors can *throw*. A correct implementation provides the **strong exception guarantee**: if anything throws, the vector is left exactly as it was.

```cpp
// Min standard: C++17. Portable.
#include <type_traits>
template <typename T>
void Vector<T>::reallocate(std::size_t new_cap) {
    T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
    std::size_t i = 0;
    try {
        for (; i < sz_; ++i) {
            if constexpr (std::is_nothrow_move_constructible_v<T>)
                ::new (new_data + i) T(std::move(data_[i]));   // move if it can't throw
            else
                ::new (new_data + i) T(data_[i]);              // else copy (strong guarantee)
        }
    } catch (...) {
        for (std::size_t j = 0; j < i; ++j) new_data[j].~T();  // unwind partial construction
        ::operator delete(new_data);                          // free new storage
        throw;                                                // old vector untouched
    }
    for (std::size_t j = 0; j < sz_; ++j) data_[j].~T();      // destroy old elements
    ::operator delete(data_);                                 // free old storage
    data_ = new_data;
    cap_ = new_cap;
}
```
*Listing 83.2 — Exception-safe reallocation with rollback. Note the placement-new/explicit-destructor symmetry.*

> **Why this matters.** This is the code most engineers never see and the reason the standard library is hard to beat. If a copy throws halfway through the move, you must destroy the objects you already constructed in the new buffer and free it, leaving the original intact — otherwise you leak or corrupt. The explicit `~T()` calls mirror the placement `new`: in raw-storage land, the compiler does not destroy for you. Getting this wrong is a leak on the rare exceptional path that no test exercises.

---

## 83.4 Move-vs-Copy on Growth and `noexcept`

Listing 83.2 contains a subtle but critical decision: it **moves** elements to the new buffer only if `T`'s move constructor is `noexcept`; otherwise it **copies**. This is exactly what `std::vector` does, and it is the practical reason `noexcept` move constructors matter.

> **Why this matters / cost model.** Moving is far cheaper than copying for resource-owning types (steal a pointer vs deep-copy a buffer). But if a *move* throws partway through reallocation, the source elements are already half-destroyed — the strong guarantee is impossible to restore. So `vector` only moves when it can prove the move *cannot* throw (`noexcept`); otherwise it falls back to the slower but recoverable copy. The lesson for your own types: **mark move constructors `noexcept`** whenever they truly cannot throw, or `std::vector` will silently copy them on every growth, turning an O(N) reallocation into a much more expensive one. This is one of the highest-value one-word annotations in C++.

---

## 83.5 `my::shared_ptr`: The Control Block

`shared_ptr` implements shared ownership via a separately-allocated **control block** holding a reference count. Copying the `shared_ptr` increments the count; destroying one decrements it; the managed object (and the block) are freed when the count reaches zero.

```cpp
// Min standard: C++11. Portable. Simplified; omits weak count, deleter, aliasing ctor.
#include <atomic>
template <typename T>
class SharedPtr {
    T* ptr_ = nullptr;
    struct ControlBlock { std::atomic<long> ref_count{1}; };
    ControlBlock* cb_ = nullptr;
public:
    explicit SharedPtr(T* p) : ptr_(p), cb_(new ControlBlock()) {}   // second allocation!

    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), cb_(other.cb_) {
        if (cb_) cb_->ref_count.fetch_add(1, std::memory_order_relaxed);   // share: bump count
    }
    ~SharedPtr() {
        // release+acquire: the last decrement must see all prior uses before deleting.
        if (cb_ && cb_->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete ptr_;
            delete cb_;
        }
    }
    T* operator->() const { return ptr_; }
    T& operator*()  const { return *ptr_; }
};
```
*Listing 83.3 — `shared_ptr` with an atomic control block.*

> **Why this matters / cost model.** Two memory-model facts make this correct and are easy to get wrong. The *increment* can be `relaxed` — sharing creates no happens-before requirement on the pointee. The *decrement* must be `acq_rel` (or release on decrement + an acquire fence before deletion): the thread that drops the count to zero must observe every other thread's prior use of the object before it runs the destructor, or it could delete while another core's writes are still in flight (Chapter 76). A naive `relaxed` decrement is a textbook concurrency bug that passes on x86 and fails on ARM. The thread-safety of the count is also why every `shared_ptr` copy is an *atomic* RMW — cheaper than a lock, but not free, and contended when many threads share one object.

---

## 83.6 `make_shared`, `weak_ptr`, and the Cost Model

Listing 83.3 performs **two** allocations: one for the object (by the caller of `new T`) and one for the control block. `std::make_shared` fuses them into a *single* allocation holding both object and control block contiguously.

```cpp
// Min standard: C++11. Portable.
auto p = std::make_shared<Widget>(args);   // ONE allocation: object + control block together
// vs:
std::shared_ptr<Widget> q(new Widget(args));  // TWO allocations: object, then control block
```
*Listing 83.4 — `make_shared` halves the allocations and improves locality.*

`weak_ptr` requires the control block to carry a *second* count (the weak count): the object is destroyed when the strong count hits zero, but the control block itself survives until the weak count also reaches zero — which is why `make_shared`'s single block can keep the object's *storage* alive (though destructed) as long as any `weak_ptr` exists.

| Operation | Cost |
|---|---|
| `make_shared` | One allocation (object + block) |
| `shared_ptr(new T)` | Two allocations |
| Copy `shared_ptr` | One relaxed atomic increment |
| Destroy `shared_ptr` | One acq_rel atomic decrement (+ possible free) |
| Pass by value | Copy + destroy = two atomics |

> **Why this matters.** `make_shared` is the right default — one allocation, better cache locality (object and count adjacent), fewer atomics on the allocator. Its one caveat: because the object's storage and the control block share an allocation, a lingering `weak_ptr` keeps the *whole* block (including the object's now-unused bytes) alive until the last weak reference dies — so a huge object pinned by a tiny `weak_ptr` wastes memory. The broader lesson: prefer `unique_ptr` (zero overhead, no atomics, no control block) unless ownership is genuinely *shared*; reach for `shared_ptr` only when multiple owners with no clear last-owner truly exist, and pass it by `const&` to avoid the per-call atomic traffic.

---

## 83.7 What Reimplementation Teaches

| Component | The hidden cost it exposes |
|---|---|
| `vector` growth | Geometric reallocation; move-vs-copy; why `reserve`/`noexcept` matter |
| Placement new | Allocation ≠ construction; explicit destructor symmetry |
| Exception safety | The strong guarantee and rollback on throw |
| `shared_ptr` | A second allocation; atomic count; the acq_rel decrement |
| `make_shared` / `weak_ptr` | Fused allocation; the weak count pinning storage |

> **The discipline.** Knowing how `vector` and `shared_ptr` are built lets you predict their cost from the call site: this `push_back` may reallocate-and-move, this `shared_ptr` copy is two atomics, this `make_shared` is one allocation not two. That predictive power is the whole point — it lets you choose `reserve`, `unique_ptr`, `noexcept`, and `make_shared` deliberately rather than by cargo cult. The placement-new and explicit-lifetime machinery here is exactly what Chapter 97 (object lifetime and allocation-free hot paths) builds on to eliminate these costs entirely on the critical path.

> **Editorial note.** This chapter's original source file additionally concatenated unrelated material (Design Patterns, ODR/ADL/UB, Linkage and Attributes, Build Systems). That material is preserved in the volume-level `_archive` as its own source files and is folded into the dedicated systems chapters — undefined behaviour and the ODR into Chapter 104, and linkage/ABI/build into Chapter 102 — rather than buried inside the standard-library chapter.
