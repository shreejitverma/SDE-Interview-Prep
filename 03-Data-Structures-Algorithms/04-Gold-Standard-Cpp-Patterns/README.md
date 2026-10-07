---
id: gold-standard-cpp-patterns
title: "Gold-Standard C++20 DSA Reference Patterns"
tags:
  - dsa
  - cpp
  - modern-cpp
  - gold-standard
  - systems-programming
level: advanced
type: moc
status: solid
created_date: 2026-10-07
updated_date: 2026-10-07
sources:
  - "https://en.cppreference.com/w/cpp/20"
---

# Gold-Standard C++20 DSA Reference Patterns

## 1. Architectural Mission and Design Principles

This directory provides gold-standard, production-grade reference implementations of fundamental data structures and competitive algorithms written in idiomatic Modern C++20.
Every implementation adheres to strict systems engineering principles:
1. **Zero-Overhead Abstractions**: Leveraging templates, type traits, and compile-time concepts rather than runtime polymorphism.
2. **Deterministic Resource Management**: Strict RAII guarantees with zero raw `new`/`delete` calls.
3. **Hardware Cache Friendliness**: Favoring contiguous flat vectors over node-pointer graphs where possible to minimize L1/L2 cache miss latency.
4. **Const-Correctness and Value Semantics**: Explicit move semantics (`std::move`) and immutable views (`std::string_view`, `std::span`).

---

## 2. Catalog of Reference Implementations

### Dynamic Programming
- **[0/1 Knapsack (Classic 2D Table)](./Dynamic-Programming/0_1_knapsack.cpp)**: Canonical $O(N \cdot W)$ time and $O(N \cdot W)$ space tabulation.
- **[0/1 Knapsack (Space-Optimized 1D & Reconstruction)](./Dynamic-Programming/space_optimized_knapsack.cpp)**: Strict $O(W)$ auxiliary memory using reverse inner loop iterations and bitset-backed subset reconstruction.
- **[Longest Common Subsequence (Rolling Array & Reconstruction)](./Dynamic-Programming/longest_common_subsequence.cpp)**: Space-optimized $O(\min(M, N))$ rolling buffer evaluation alongside complete string reconstruction.

### Graphs & Network Algorithms
- **[Dijkstra's Shortest Path](./Graphs/dijkstra.cpp)**: Single-Source Shortest Path using `std::priority_queue` with stale node pruning in $O(E \log V)$ time.
- **[Disjoint Set Union (Union-Find)](./Graphs/union_find.cpp)**: Disjoint Set Union with near $O(1)$ amortized query cost via path compression and union by rank.
- **[Topological Sort (Kahn's Algorithm & Lexicographical Ordering)](./Graphs/topological_sort.cpp)**: Directed Acyclic Graph linear extension with cycle detection and optional min-heap lexicographical sequencing.
- **[Bellman-Ford Algorithm](./Graphs/bellman_ford.cpp)**: Single-Source Shortest Path with negative weight support, negative cycle detection, and predecessor path recovery.

### Advanced Data Structures
- **[LRU Cache (Least Recently Used)](./Data-Structures/lru_cache.cpp)**: Template-specialized $O(1)$ cache using `std::list` node splicing and `std::unordered_map` iterators with zero reallocations.
- **[Fenwick Tree (Binary Indexed Tree)](./Data-Structures/fenwick_tree.cpp)**: $O(\log N)$ point updates, prefix sums, and range queries with $O(\log N)$ binary lifting lower bound.
- **[Iterative Segment Tree](./Data-Structures/segment_tree.cpp)**: Flat $2N$ array non-recursive Segment Tree for arbitrary associative monoid range queries.

---

## 3. Algorithmic Complexity and Trade-Off Matrix

| Category | Pattern | Time Complexity | Space Complexity | Primary Use Case |
|:---|:---|:---:|:---:|:---|
| DP | 0/1 Knapsack (Space-Opt) | $O(N \cdot W)$ | $O(W)$ | Resource allocation with discrete budget constraints |
| DP | LCS (Rolling Buffer) | $O(M \cdot N)$ | $O(\min(M, N))$ | Diff tools, bioinformatics sequence alignment |
| Graph | Dijkstra | $O(E \log V)$ | $O(V + E)$ | Non-negative edge routing, road networks |
| Graph | Disjoint Set Union | $O(\alpha(N))$ | $O(N)$ | Kruskal MST, dynamic connectivity, cycle detection |
| Graph | Topological Sort | $O(V + E)$ | $O(V + E)$ | Build systems, task scheduling, dependency resolution |
| Graph | Bellman-Ford | $O(V \cdot E)$ | $O(V)$ | Currency arbitrage, negative weight routing |
| Structures | LRU Cache | $O(1)$ | $O(\text{Capacity})$ | Memory caching, buffer pools, page replacement |
| Structures | Fenwick Tree | $O(\log N)$ | $O(N)$ | Dynamic frequency tables, inversion counting |
| Structures | Segment Tree | $O(\log N)$ | $O(2N)$ | Range min/max/sum queries with dynamic point updates |

---

## 4. Hardware, Systems, and Memory Optimization Highlights

1. **Iterative Flat Trees vs Pointer-Linked Nodes**: The iterative segment tree packs all tree levels into a single contiguous `std::vector<T>` of size $2N$.
This eliminates heap allocation fragmentation and ensures child nodes $2i$ and $2i + 1$ reside on the exact same 64-byte L1 cache line for deeper tree levels.
2. **Iterator Preservation in LRU Cache**: Splicing an existing node in `std::list` using `items_.splice(items_.begin(), items_, it->second)` relocates the linked list node pointers without destroying or re-allocating heap memory.
This guarantees that iterators stored in the hash map remain permanently valid.
3. **Bitwise Indexing in Fenwick Tree**: The expression `i & (-i)` extracts the least significant set bit using two's complement in a single CPU instruction, executing with zero branch penalties.

---

## 5. Standalone Implementation Links

- [LRU Cache C++ Implementation](./Data-Structures/lru_cache.cpp)
- [Fenwick Tree C++ Implementation](./Data-Structures/fenwick_tree.cpp)
- [Segment Tree C++ Implementation](./Data-Structures/segment_tree.cpp)
- [Dijkstra C++ Implementation](./Graphs/dijkstra.cpp)
- [Union-Find C++ Implementation](./Graphs/union_find.cpp)
- [Topological Sort C++ Implementation](./Graphs/topological_sort.cpp)
- [Bellman-Ford C++ Implementation](./Graphs/bellman_ford.cpp)
- [0/1 Knapsack Classic C++ Implementation](./Dynamic-Programming/0_1_knapsack.cpp)
- [0/1 Knapsack Space-Optimized C++ Implementation](./Dynamic-Programming/space_optimized_knapsack.cpp)
- [LCS C++ Implementation](./Dynamic-Programming/longest_common_subsequence.cpp)
