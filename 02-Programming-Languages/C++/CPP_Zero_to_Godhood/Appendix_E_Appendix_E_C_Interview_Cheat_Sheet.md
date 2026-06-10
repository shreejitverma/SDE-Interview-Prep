# Appendix E: C++ Interview Cheat Sheet



### Core Concepts
1.  **Virtual Functions**: Enable runtime polymorphism via vtable/vptr. Destructors must be virtual in base classes.
2.  **Smart Pointers**:
    *   `unique_ptr`: Exclusive ownership, no overhead.
    *   `shared_ptr`: Shared ownership, ref-counted (atomic), control block overhead.
    *   `weak_ptr`: Non-owning reference to `shared_ptr` (breaks cycles).
3.  **Move Semantics**: Transfers resources (pointers) instead of deep copying. Enabled by rvalue references (`&&`) and `std::move`.
4.  **RAII**: Resource Acquisition Is Initialization. Constructor acquires, destructor releases. Core to C++ safety.
5.  **Cast Types**:
    *   `static_cast`: Compile-time safe conversions.
    *   `dynamic_cast`: Runtime checked downcasting (requires RTTI).
    *   `reinterpret_cast`: Bitwise reinterpretation (unsafe).
    *   `const_cast`: Remove/add constness.

### Modern C++ (C++11/14/17/20)
1.  **Lambdas**: Anonymous function objects. Capture `[=]`, `[&]`, or move-only `[x = std::move(y)]`.
2.  **Auto**: Type deduction. Always initialize.
3.  **Structured Bindings (C++17)**: `auto [x, y] = pair;`
4.  **Concepts (C++20)**: Constrain templates for better errors/readability.
5.  **Coroutines (C++20)**: Functions that can suspend/resume.

### System Design Questions
1.  **Vector vs List**: Vector (contiguous, cache-friendly) is almost always better than List (node-based, cache misses) unless aggressive splicing is needed.
2.  **Map vs Unordered Map**: Map (BST, O(log n), sorted) vs Unordered Map (Hash Table, O(1) avg, unsorted).
3.  **Handling 1M connections**: Use non-blocking I/O (epoll/kqueue) or `io_uring`, not one thread per connection.
4.  **Memory Layout**: Stack (local vars) vs Heap (dynamic) vs Data (globals) vs Text (code).

### Quick Coding
*   **Implement Singleton**: Use static local variable (Thread-safe in C++11+).
*   **Implement String Class**: Handle deep copy, move semantics, and destructor.
*   **Reverse Linked List**: Classic pointer manipulation.

---


---
