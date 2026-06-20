# Chapter 62: Flat Containers — `flat_map`, `flat_set`, and Their Multi Variants

> Every performance-conscious C++ programmer has, at some point, replaced a `std::map` with a *sorted `std::vector`* — trading the map's pointer-chasing red-black tree for contiguous storage that the cache loves. It is a well-worn idiom, and it is also a pile of hand-written, easy-to-break code. C++23 standardizes it as the **flat container adaptors**: `flat_map`, `flat_multimap`, `flat_set`, and `flat_multiset` — associative containers with the familiar map/set interface, backed by sorted contiguous sequences. They are the right default for read-heavy, lookup-dominated workloads where cache locality beats asymptotic insertion cost.

## Table of Contents

1. [Why Tree-Based Maps Hurt on Modern Hardware](#621-why-tree-based-maps-hurt-on-modern-hardware)
2. [The Four Flat Containers](#622-the-four-flat-containers)
3. [How a `flat_map` Is Built: Two Parallel Sequences](#623-how-a-flat_map-is-built-two-parallel-sequences)
4. [Using `flat_map` and `flat_set`](#624-using-flat_map-and-flat_set)
5. [The Performance Trade-Off, Quantified](#625-the-performance-trade-off-quantified)
6. [Bulk Construction and the `sorted_unique` Tag](#626-bulk-construction-and-the-sorted_unique-tag)
7. [Professional Insights](#627-professional-insights)

---

## 62.1 Why Tree-Based Maps Hurt on Modern Hardware

`std::map` and `std::set` are implemented as balanced binary search trees (red-black trees). Each node is a separate heap allocation holding the key, the value, three pointers (two children and a parent), and a color bit. This gives excellent asymptotic guarantees — O(log n) lookup, insert, and erase, with stable references and ordered iteration — but it is a disaster for the memory hierarchy:

- **Every node is a separate allocation**, so logically adjacent keys are scattered across the heap with no spatial locality.
- **A lookup is a pointer-chase** down the tree: each step dereferences a pointer to a likely-uncached node, and on modern hardware a last-level-cache miss costs hundreds of cycles — dwarfing the comparison itself.
- **The per-node overhead is large**: three pointers plus a color bit per element, often more memory in bookkeeping than in data for small keys.

The classic remedy is to keep the keys in a single sorted `std::vector` and binary-search it. Lookups then walk contiguous memory the prefetcher can stream, the per-element overhead vanishes, and ordered iteration is just a linear scan. The cost is that insertion and erasure in the middle become O(n) (elements must shift). For workloads that are built once and then queried many times — the common case for configuration tables, lookup dictionaries, and reference data — this is an enormous net win. C++23 makes the idiom a standard container.

---

## 62.2 The Four Flat Containers

C++23 adds four **container adaptors** (in `<flat_map>` and `<flat_set>`), each mirroring a tree-based counterpart:

| Flat container | Mirrors | Keys | Header |
|---|---|---|---|
| `std::flat_map` | `std::map` | unique | `<flat_map>` |
| `std::flat_multimap` | `std::multimap` | duplicates allowed | `<flat_map>` |
| `std::flat_set` | `std::set` | unique | `<flat_set>` |
| `std::flat_multiset` | `std::multiset` | duplicates allowed | `<flat_set>` |

They are *adaptors*, not first-class containers: each wraps one or more underlying sequence containers (by default `std::vector`) and maintains the sorted invariant. Their interface deliberately matches the ordered associative containers — `find`, `lower_bound`, `count`, `operator[]` (for `flat_map`), ordered iteration — so they are close to drop-in replacements.

---

## 62.3 How a `flat_map` Is Built: Two Parallel Sequences

A crucial implementation detail with real consequences: `std::flat_map` does **not** store `std::pair<Key, Value>` in one vector. It keeps **two parallel sorted sequences** — one of keys, one of values — index-aligned so that `keys[i]` maps to `values[i]`. This "structure-of-arrays" layout means a key-only operation (a lookup, a binary search) touches *only* the keys vector, packing far more keys per cache line than an array-of-`pair` would, because the values never share those cache lines.

`std::flat_set`, having no values, is simply one sorted sequence.

The consequences you must internalize:

- **`flat_map::value_type` is still `pair<const Key, Value>`**, but it is *materialized on access* (the iterator is a proxy over the two parallel sequences); the pairs are not stored contiguously.
- **References and iterators are invalidated by any insert or erase**, because the sorted sequences are reallocated/shifted — unlike `std::map`, where node addresses are stable. Code that holds a pointer into a `flat_map` across a modification is broken.
- **Lookups are maximally cache-efficient** precisely because the keys are dense and the values are out of the way.

---

## 62.4 Using `flat_map` and `flat_set`

For the common operations, flat containers look exactly like their tree counterparts:

**Listing 62.1: Basic `flat_map` and `flat_set` usage.**

```cpp
#include <flat_map>
#include <flat_set>
#include <string>
#include <print>

int main() {
    std::flat_map<std::string, int> scores;
    scores["alice"] = 50;          // same interface as std::map
    scores["bob"]   = 42;
    scores.insert({"cara", 71});

    if (auto it = scores.find("bob"); it != scores.end())
        std::println("bob -> {}", it->second);

    for (const auto& [name, score] : scores)   // ordered iteration: alice, bob, cara
        std::println("{}: {}", name, score);

    std::flat_set<int> seen{5, 1, 3, 1};       // duplicates dropped -> {1, 3, 5}
    std::println("contains 3? {}", seen.contains(3));
    std::println("size = {}", seen.size());    // 3
}
```

The `[]`, `find`, `insert`, `contains`, `lower_bound`/`upper_bound`, and ordered iteration all behave as with `std::map`/`std::set`. The behavioral differences are not in the interface but in the *complexity and invalidation* profile, covered next.

---

## 62.5 The Performance Trade-Off, Quantified

The flat containers are a textbook space/time/locality trade-off against the trees:

| Operation | `std::map` | `std::flat_map` |
|---|---|---|
| Lookup (`find`) | O(log n), pointer-chasing | O(log n), contiguous binary search — **far fewer cache misses** |
| Insert (random position) | O(log n) | **O(n)** — elements shift |
| Erase (random position) | O(log n) | **O(n)** — elements shift |
| Ordered iteration | O(n), pointer-chasing | O(n), **sequential** — prefetcher-friendly |
| Memory per element | key+value + 3 pointers + color | **key+value only** |
| Reference stability | stable across insert/erase | **invalidated** by insert/erase |

The headline: **`flat_map` wins decisively on lookup and iteration speed and on memory, and loses decisively on mid-sequence insert/erase.** The break-even depends on the workload's mutation rate. For a table that is populated once (ideally in bulk, see Section 62.6) and then queried repeatedly, `flat_map` is often several times faster on the query path and uses a fraction of the memory. For a container under a steady stream of random single-element insertions and deletions, the O(n) shifting makes `std::map` (or `std::unordered_map`) the better choice.

A third axis worth naming: if you do **not** need ordered iteration or range queries, `std::unordered_map` (O(1) average lookup) may beat both. The flat containers occupy the niche where you want *ordering* (or `lower_bound`-style range queries) **and** cache-friendly, lookup-heavy access.

---

## 62.6 Bulk Construction and the `sorted_unique` Tag

Because element-by-element insertion is O(n) each, the right way to build a large flat container is **in bulk**: hand it all the data at once so it can sort the underlying sequence a single time (O(n log n) total) rather than paying O(n) per insertion (O(n²) overall). The constructors and `insert(first, last)` ranges do exactly this.

When you already have data that is sorted and de-duplicated, you can skip the sort entirely by passing the `std::sorted_unique` tag (or `std::sorted_equivalent` for the multi-variants), promising the invariant and letting construction be linear.

**Listing 62.2: Efficient bulk construction.**

```cpp
#include <flat_map>
#include <vector>
#include <print>

int main() {
    // Bulk insert: one sort, not one shift per element.
    std::vector<std::pair<int, std::string>> data{
        {3, "c"}, {1, "a"}, {2, "b"}
    };
    std::flat_map<int, std::string> m(data.begin(), data.end());  // sorted once

    // If the input is already sorted and unique, promise it and skip the sort:
    std::vector<int> keys{1, 2, 3};
    std::vector<std::string> vals{"a", "b", "c"};
    std::flat_map<int, std::string> fast(
        std::sorted_unique, std::move(keys), std::move(vals));    // linear construction

    std::println("{} / {}", m.size(), fast.size());
}
```

You can also `reserve` capacity up front (the adaptor forwards it to the underlying vectors) and, where the API allows, extract/replace the underlying containers wholesale — the bulk-load pattern that makes flat containers shine.

> **Version-trap flag:** `std::flat_map`, `std::flat_multimap`, `std::flat_set`, `std::flat_multiset`, and the `std::sorted_unique`/`std::sorted_equivalent` tags are all **C++23**. They have no C++20 equivalent; the pre-C++23 idiom was a hand-maintained sorted `std::vector`. Standard-library support for the flat containers arrived later than most other C++23 library features — verify availability with `__cpp_lib_flat_map` / `__cpp_lib_flat_set` on your toolchain.

---

## 62.7 Professional Insights

**Reach for flat containers when the access pattern is build-then-query, not when it is churn.** The decisive question is the mutation rate after construction. A lookup table, a parsed configuration, an interned-symbol map, or any reference data loaded at startup and queried for the rest of the program's life is the ideal `flat_map` candidate — you pay the sort once and reap cache-friendly lookups forever. A container under continuous random insert/erase is the wrong fit; its O(n) shifting will erase the locality advantage. Decide by profiling the *mutation*-to-*lookup* ratio, not by reflex.

**Build in bulk and promise `sorted_unique` when you can — element-by-element insertion is the anti-pattern.** Filling a `flat_map` with a loop of `insert` calls is O(n²) and squanders the container's reason for existing. Gather the data, construct from the range in one shot (one O(n log n) sort), and if the data is already ordered and unique, pass `std::sorted_unique` to make construction linear. This single discipline is the difference between a flat container that is faster than `std::map` and one that is pathologically slower.

**Treat reference and iterator invalidation as a correctness hazard, because it differs from `std::map`.** `std::map` gives you node-stable references that survive insertions and erasures elsewhere; `flat_map` does not — any insert or erase can reallocate and shift the underlying sequences, dangling every outstanding pointer and iterator. Code ported from `std::map` that caches a reference across a modification will compile cleanly and then corrupt memory. Audit for held references when you switch a container to a flat variant.

**Remember the structure-of-arrays layout when reasoning about cache behavior.** `flat_map`'s split keys/values storage is *why* it is fast for lookups: a key-only search streams dense keys without dragging values through the cache. But it also means `value_type` pairs are synthesized on access, not stored, so do not expect to take the address of a contiguous `pair` array or to memcpy the map as one block. Understanding the two-parallel-sequences design lets you predict both the speed wins and the layout surprises.
