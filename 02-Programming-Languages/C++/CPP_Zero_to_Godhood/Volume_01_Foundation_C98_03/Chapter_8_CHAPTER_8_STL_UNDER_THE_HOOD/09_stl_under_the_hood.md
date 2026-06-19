# Chapter 09: STL Under the Hood

# STL INTERNALS DEEP DIVE

<!-- Merged content from Chapter_21_STL_INTERNALS_DEEP_DIVE.md -->

# STL INTERNALS DEEP DIVE (C++98)

To master the STL, you must understand what happens under the hood.

### 1. The Truth About std::vector

`std::vector` is a dynamic array. It guarantees contiguous memory.

*   **Layout**: Three pointers: `start`, `finish`, `end_of_storage`.
    *   `start`: Points to first element.
    *   `finish`: Points to one-past-the-last active element (size).
    *   `end_of_storage`: Points to end of allocated buffer (capacity).

*   **Growth Strategy**: Geometric growth.
    *   When `size() == capacity()`, a new buffer is allocated (usually 2x or 1.5x larger).
    *   **Elements are Copied** to the new buffer (C++98 does not have Move semantics).
    *   Old buffer is deleted.
    *   *Cost*: Amortized O(1) push_back, but worst-case O(N).

*   **Iterator Invalidation**:
    *   **Reallocation**: Invalidates ALL iterators, pointers, and references.
    *   **Insertion/Erasure**: Invalidates iterators at and after the point of operation.

### 2. The std::deque Implementation

`std::deque` (Double-Ended Queue) is NOT a contiguous array.

*   **Layout**: A "Map" (dynamic array) of pointers to fixed-size "Chunks" (blocks).
    *   Iterators are smart pointers that know how to jump between chunks.
*   **Performance**:
    *   O(1) random access (double dereference).
    *   O(1) push/pop at BOTH ends (no full reallocation needed, just add a new chunk).
*   **Cache Locality**: Worse than vector, better than list.

### 3. Why std::list is (Almost) Always Wrong

`std::list` is a Doubly Linked List.

*   **Layout**: Nodes allocated individually on the heap.
    *   `struct Node { T val; Node* prev; Node* next; }`
*   **The Cache Problem**: Nodes are scattered in memory. Traversing a list causes constant **Cache Misses**.
*   **Benchmark**: Iterating a `vector` is orders of magnitude faster than a `list`, even for large types, due to prefetching.
*   **Use Case**: Only when you need **Reference Stability** (insertions never invalidate references to other elements) or frequent splicing.

### 4. Associative Containers (Map/Set)

`std::map`, `std::set`, `std::multimap`, `std::multiset`.

*   **Implementation**: Balanced Binary Search Tree (usually **Red-Black Tree**).
*   **Node Layout**: `struct Node { T val; Node* left; Node* right; Node* parent; Color color; }`
*   **Complexity**: O(log N) for insert, lookup, delete.
*   **Overhead**: 3 pointers + enum per element (heavy memory overhead).

### 5. Iterator Invalidation Cheat Sheet

| Container | Operation | Invalidates |
| :--- | :--- | :--- |
| **Vector** | Capacity Change | **ALL** |
| **Vector** | Insert/Erase | Current & After |
| **Deque** | Insert/Erase (ends) | Iterators only (Refs valid!) |
| **Deque** | Insert/Erase (middle) | **ALL** |
| **List** | Insert/Erase | Only deleted element |
| **Map/Set** | Insert/Erase | Only deleted element |

