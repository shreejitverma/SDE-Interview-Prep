# CHAPTER 42: LOCK FREE PROGRAMMING


# LOCK-FREE PROGRAMMING

## 1. The Concept

Programming without Mutexes. Guarantees system-wide progress.

*   **Lock-Free:** At least one thread always makes progress.
*   **Wait-Free:** Every thread makes progress in finite steps.

## 2. Compare-And-Swap (CAS)

The primitive of lock-free. `compare_exchange_weak` vs `compare_exchange_strong`.

```cpp
std::atomic<int> head;

void push(int new_val) {
    int old_head = head.load();
    // Loop until we successfully swap head with new_val
    while (!head.compare_exchange_weak(old_head, new_val)) {
        // old_head is updated to current head value automatically
    }
}
```

## 3. The ABA Problem

1.  Thread 1 reads A.
2.  Thread 2 changes A to B, then back to A.
3.  Thread 1 CAS(A, new) succeeds, thinking nothing changed.

**Solutions:**
*   **Versioned Pointers:** Store `{ptr, count}`. `std::atomic<uint128_t>` (if supported).
*   **Hazard Pointers:** Protect pointers currently being read.
*   **RCU (Read-Copy-Update):** Wait for all readers to finish before reclaiming memory.

## 4. Lock-Free Data Structures

*   **Lock-Free Stack:** Easy (CAS on head).
*   **Lock-Free Queue:** Harder (Head and Tail). Use Michael-Scott Queue algorithm.
*   **Lock-Free Hash Map:** Very hard (Split-Ordered Lists).
