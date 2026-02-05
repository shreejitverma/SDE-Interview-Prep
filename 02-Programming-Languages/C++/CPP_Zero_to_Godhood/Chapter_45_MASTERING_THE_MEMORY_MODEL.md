# MASTERING THE MEMORY MODEL


The C++ Memory Model defines how threads interact through memory.

### 15.1 Atomicity vs Ordering
*   **Atomicity**: An operation is indivisible (all or nothing).
*   **Ordering**: The order in which operations are observed by other threads.

`std::atomic<int>` guarantees atomicity, but `memory_order` controls ordering.

### 15.2 Memory Orders Deep Dive

1.  **`memory_order_relaxed`**: No ordering constraints. Only atomicity.
    *   Use for: Incrementing stats counters.
    ```cpp
    cnt.fetch_add(1, std::memory_order_relaxed);
    ```

2.  **`memory_order_acquire`**: Read operation.
    *   Guarantee: No reads/writes in the current thread can be reordered *before* this load.
    *   Use with: Release.

3.  **`memory_order_release`**: Write operation.
    *   Guarantee: No reads/writes in the current thread can be reordered *after* this store.
    *   Use for: Publishing data.

4.  **`memory_order_seq_cst`** (Default): Sequentially Consistent.
    *   Guarantee: A total global ordering exists. Expensive.

### 15.3 The Happens-Before Relationship
If Operation A *happens-before* Operation B:
1.  A is sequenced before B (same thread).
2.  A *synchronizes-with* B (inter-thread, e.g., A releases, B acquires).

**Example: Lock-Free Flag**
```cpp
std::atomic<int> data = 0;
std::atomic<bool> ready = false;

void producer() {
    data.store(42, std::memory_order_relaxed);
    ready.store(true, std::memory_order_release); // "Publish"
}

void consumer() {
    while (!ready.load(std::memory_order_acquire)); // "Acquire"
    assert(data.load(std::memory_order_relaxed) == 42); // Guaranteed 42
}
```

---
