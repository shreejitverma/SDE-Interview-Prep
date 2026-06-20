# Chapter 45: std::span — Non-Owning Views over Contiguous Memory

> *`std::span` is C++20's answer to the oldest interface-design problem in C and C++: how to pass a contiguous block of elements to a function without committing to a specific container, without copying, and without decaying to a bare pointer that has lost its length. A span is a fat pointer — a pointer plus a size — that views any contiguous sequence: a C array, a `std::array`, a `std::vector`, or a raw buffer. This chapter covers span's construction, its fixed- and dynamic-extent forms, the subview operations, and the lifetime rules that make it safe.*

For decades the idiomatic C++ function that processed a range of `int`s had bad choices: take a `const std::vector<int>&` (forces the caller to own a vector and forbids arrays/`std::array`), take `(int* ptr, size_t len)` (two arguments that can disagree, and `ptr` decays so `sizeof` lies), or template on the container (leaks into the header, instantiates per type). `std::span<int>` solves all three: one parameter, no copy, no ownership, works with every contiguous source, and carries its length. It is the single most broadly useful library addition in C++20 for API design.

---

## Table of Contents

- [45.1 The Problem span Solves](#451-the-problem-span-solves)
- [45.2 Constructing a span](#452-constructing-a-span)
- [45.3 Dynamic Extent vs Fixed Extent](#453-dynamic-extent-vs-fixed-extent)
- [45.4 Accessing Elements and the span Interface](#454-accessing-elements-and-the-span-interface)
- [45.5 Subviews: first, last, subspan](#455-subviews-first-last-subspan)
- [45.6 const-Correctness and Byte Views](#456-const-correctness-and-byte-views)
- [45.7 Lifetime: span Does Not Own](#457-lifetime-span-does-not-own)
- [45.8 Professional Insights](#458-professional-insights)

---

## 45.1 The Problem span Solves

`std::span` is a lightweight, **non-owning view over contiguous memory** — arrays, `std::array`, `std::vector`, or any contiguous range. It bundles a pointer and a length into one object you pass by value.

```cpp
// Listing 45.1: one function signature accepts every contiguous source
#include <span>
#include <vector>
#include <array>

int sum(std::span<const int> s) {       // accepts ANY contiguous int sequence
    int total = 0;
    for (int x : s) total += x;          // span knows its own length
    return total;
}

int main() {
    int               c_array[] = {1, 2, 3};
    std::array<int,4> std_arr   = {1, 2, 3, 4};
    std::vector<int>  vec       = {1, 2, 3, 4, 5};

    sum(c_array);     // works
    sum(std_arr);     // works
    sum(vec);         // works — no copy, no ownership, no template
}
```

A `span<const int>` parameter replaces the entire menu of bad options: it does not own (so no copy), it carries the length (so no `(ptr, len)` desync), and it is a concrete type (so no template instantiation per container). This is the canonical "read a contiguous range" parameter type in modern C++.

---

## 45.2 Constructing a span

A span can be built from anything contiguous: a pointer-and-length pair, two iterators, a C array, a `std::array`, a `std::vector`, or another span. Construction is O(1) — it copies only the pointer and size.

```cpp
// Listing 45.2: the ways to make a span
#include <span>
#include <vector>
#include <array>

std::vector<int> v{10, 20, 30, 40, 50};

std::span<int> s1{v};                    // from a container (deduces size)
std::span<int> s2{v.data(), 3};          // from pointer + length (first 3)
std::span<int> s3{v.begin(), v.end()};   // from an iterator pair

int arr[] = {1, 2, 3};
std::span<int> s4{arr};                  // from a C array (size deduced = 3)

std::array<int, 4> a{1, 2, 3, 4};
std::span<int> s5{a};                    // from std::array
```

Class template argument deduction makes `std::span s{v};` work without spelling the element type. Note that constructing a span from a container takes a reference to that container's storage — the span is only valid while that storage lives (Section 45.7).

---

## 45.3 Dynamic Extent vs Fixed Extent

A span's **extent** — its size — can be carried at runtime (**dynamic extent**, the default) or baked into the type at compile time (**fixed extent**). The second template parameter selects this.

```cpp
// Listing 45.3: dynamic vs fixed extent
#include <span>
#include <array>

void dyn(std::span<int> s);                 // dynamic extent (size at runtime)
void fix(std::span<int, 3> s);              // fixed extent: exactly 3 elements

std::array<int, 3> a{1, 2, 3};
std::array<int, 5> b{1, 2, 3, 4, 5};

fix(a);          // OK: size 3 matches the fixed extent
// fix(b);       // ERROR: size 5 != 3, checked at compile time

// sizeof difference:
//   std::span<int>      stores a pointer AND a size  (2 words)
//   std::span<int, 3>   stores only a pointer        (1 word — size is in the type)
```

| Form | Spelling | Size stored | Checked |
|------|----------|-------------|---------|
| Dynamic extent | `std::span<T>` (or `std::span<T, std::dynamic_extent>`) | pointer + size (2 words) | at runtime |
| Fixed extent | `std::span<T, N>` | pointer only (1 word) | at compile time |

Fixed extent is both **smaller** (the size lives in the type, not the object) and **safer** (a size mismatch is a compile error). Use a fixed extent when the length is a compile-time constant — protocol headers, fixed-size frames, SIMD lanes — and dynamic extent for genuinely variable-length data.

---

## 45.4 Accessing Elements and the span Interface

A span offers the familiar container-style interface — indexing, iteration, `front`/`back`, `data`/`size` — all O(1) and all operating directly on the underlying memory (no copy).

```cpp
// Listing 45.4: the span access interface
#include <span>
#include <vector>
#include <cstddef>

void process(std::span<int> s) {
    if (s.empty()) return;

    int first = s.front();           // first element
    int last  = s.back();            // last element
    int third = s[2];                // indexed access (no bounds check, like vector)

    std::size_t n      = s.size();        // number of elements
    std::size_t nbytes = s.size_bytes();  // size in bytes (n * sizeof(int))
    int*        raw    = s.data();        // pointer to the first element

    for (int& x : s) x *= 2;         // mutating iteration writes through to storage
}
```

Because the span is a *view*, `for (int& x : s) x *= 2;` modifies the original container's elements — the span does not own a copy. `operator[]` is unchecked like `std::vector::operator[]` (C++20 has no `.at()` on span; bounds-checked access is a C++26 addition). `size_bytes()` is convenient for byte-level I/O and serialization.

---

## 45.5 Subviews: first, last, subspan

Span's most ergonomic feature is cheap **subviews** — windows into a portion of the viewed range, created without copying. `first(n)`, `last(n)`, and `subspan(offset, count)` each return a new span over a sub-range.

```cpp
// Listing 45.5: slicing a span into subviews
#include <span>
#include <vector>

std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7};
std::span<int> s{v};

auto head = s.first(3);          // {0, 1, 2}
auto tail = s.last(2);           // {6, 7}
auto mid  = s.subspan(2, 4);     // {2, 3, 4, 5}  (offset 2, count 4)
auto rest = s.subspan(5);        // {5, 6, 7}     (offset 5 to the end)

// Compile-time fixed-extent subviews when the bounds are constants:
auto head_fixed = s.first<3>();  // std::span<int, 3>
```

Each subview is O(1) and shares the same underlying storage. The template forms (`s.first<3>()`, `s.subspan<2, 4>()`) produce **fixed-extent** subspans when the offsets/counts are compile-time constants, propagating the size into the type. Subviews are the idiomatic way to parse framed data: take a span over a buffer, peel off a fixed-size header with `first<HeaderSize>()`, and pass the `subspan(HeaderSize)` payload onward — all without copying a byte.

---

## 45.6 const-Correctness and Byte Views

Span distinguishes between a *const span* (you cannot rebind it) and a *span of const* (you cannot modify the elements). The latter is what API parameters usually want.

```cpp
// Listing 45.6: span of const vs const span, and byte views
#include <span>
#include <vector>
#include <cstddef>

std::vector<int> v{1, 2, 3};

std::span<const int> read_only{v};   // elements are const: cannot write through
// read_only[0] = 9;                 // ERROR: element is const

std::span<int> writable{v};
const std::span<int> fixed_view{v};  // the SPAN is const (cannot reassign), but
fixed_view[0] = 9;                   // elements are still mutable — OK

// Reinterpret a span as raw bytes for serialization / I/O:
std::span<const std::byte> bytes  = std::as_bytes(writable);          // read-only bytes
std::span<std::byte>       wbytes = std::as_writable_bytes(writable); // mutable bytes
```

For function parameters, prefer `std::span<const T>` to express "I will read this range but not modify it" — the analogue of `const T&` for a single object. The `std::as_bytes` / `std::as_writable_bytes` helpers reinterpret any span as a span of `std::byte`, the standard, type-safe way to get a byte view of structured data for hashing, checksums, or writing to a socket — replacing error-prone `reinterpret_cast<char*>` plus a manually-tracked length.

---

## 45.7 Lifetime: span Does Not Own

The one rule that makes span safe: a span is a **borrowed view**, so the memory it points at must outlive the span. A span dangles exactly when the thing it views is destroyed or reallocated.

```cpp
// Listing 45.7: the dangling traps
#include <span>
#include <vector>

std::span<int> make_bad() {
    std::vector<int> local{1, 2, 3};
    return std::span<int>{local};      // BUG: 'local' dies at return -> dangling span
}

void reallocation_trap() {
    std::vector<int> v{1, 2, 3};
    std::span<int> s{v};               // s points at v's current buffer
    v.push_back(4);                    // may REALLOCATE -> s now dangles!
    // int x = s[0];                   // undefined behavior
}
```

Two failure modes dominate: (1) returning a span to a local (the storage dies at the end of the scope), and (2) mutating the viewed container in a way that **reallocates** — `push_back`, `resize`, `insert` on a `vector` can move the buffer, invalidating every span over it, exactly like iterator invalidation. The discipline is the same as for iterators and `string_view`: never store a span longer than the data it views, never return a span to local storage, and re-create spans after any operation that could reallocate the underlying container.

---

## 45.8 Professional Insights

**Make `std::span<const T>` the default parameter type for "read a contiguous range."** It is the modern replacement for `const std::vector<T>&`, `(const T*, size_t)`, and container templates all at once: zero-copy, ownership-free, length-carrying, and accepting of C arrays, `std::array`, `std::vector`, and raw buffers alike. Reserve `std::span<T>` (non-const) for functions that genuinely write through to the caller's storage, mirroring `const T&` versus `T&` for single objects.

**Prefer fixed-extent spans where the length is a compile-time constant.** `std::span<T, N>` stores no runtime size (it is a single pointer) and turns a length mismatch into a compile error rather than a runtime bug. For protocol headers, fixed frames, and SIMD lane groups, fixed extent is both smaller and safer — and `first<N>()`/`subspan<Off, Cnt>()` propagate that compile-time size through your parsing pipeline.

**Treat span lifetime exactly like iterator and `string_view` lifetime.** A span borrows; it never owns. Never return a span to a local, never store one past the lifetime of its backing storage, and re-derive spans after any container operation that can reallocate (`push_back`, `resize`, `insert`). The reallocation trap is especially insidious because the code compiles and often appears to work until the vector grows — audit any span held across a mutation of its source.

**Use `as_bytes`/`as_writable_bytes` instead of `reinterpret_cast` for byte-level I/O.** When serializing, hashing, or writing structured data to a socket, `std::as_bytes(span)` yields a `std::span<const std::byte>` with the correct length computed for you, replacing the classic `reinterpret_cast<const char*>(&obj)` plus a hand-tracked `sizeof`. It keeps the length honest and the intent (a byte view, not an arbitrary pointer cast) visible in the types.

**Slice with subviews rather than passing offset/length pairs.** `header = buf.first<N>(); payload = buf.subspan(N);` expresses framed-data parsing in terms that cannot desync a pointer from its length, because each subview carries its own size. This is both clearer and safer than threading `(ptr, len)` pairs through a parser, and every subview is O(1) with no copy — the idiomatic way to walk a binary buffer in C++20.
