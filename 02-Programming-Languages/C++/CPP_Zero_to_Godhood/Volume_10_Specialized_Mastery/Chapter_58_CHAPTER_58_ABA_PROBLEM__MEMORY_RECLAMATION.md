# CHAPTER 58: ABA PROBLEM  MEMORY RECLAMATION


# ABA PROBLEM & MEMORY RECLAMATION

In the rarefied air of lock-free programming, the **ABA Problem** is the dragon that guards the gate. Conquering it requires understanding the very fabric of memory lifecycles.

### 1. The ABA Problem Explained
The Compare-And-Swap (CAS) primitive (`std::atomic::compare_exchange_weak`) checks if a value is *equal* to an expected value. It does **not** check if it is the *same* object.

**The Scenario:**
1.  Thread 1 reads pointer `A` from a lock-free stack top.
2.  Thread 1 is preempted.
3.  Thread 2 pops `A`, frees it, then pushes `B`, then pushes a *new* object allocated at address `A` (recycled memory).
4.  Thread 1 wakes up, performs CAS. The address is still `A`. CAS succeeds.
5.  **Catastrophe:** Thread 1 has popped the *new* `A`, but its local logic assumes it's the *old* `A` (e.g., pointing to `B` as the next node). The stack is now corrupted.

### 2. Solution I: Tagged Pointers (Version Counters)
Pack a version counter into the unused bits of a pointer (usually top 16 bits on 64-bit systems).
*   **Mechanism:** Every modification increments the counter. `Ptr(A, v1)` != `Ptr(A, v2)`.
*   **Limitation:** Reduces addressable memory space; requires platform-specific bit manipulation.

```cpp
// Example of a 64-bit tagged pointer
struct TaggedPtr {
    uint64_t data; // 48 bits pointer, 16 bits tag

    TaggedPtr(void* ptr, uint16_t tag) {
        data = (reinterpret_cast<uint64_t>(ptr) & 0x0000FFFFFFFFFFFF) | (static_cast<uint64_t>(tag) << 48);
    }

    void* get_ptr() const { return reinterpret_cast<void*>(data & 0x0000FFFFFFFFFFFF); }
    uint16_t get_tag() const { return static_cast<uint16_t>(data >> 48); }
};
```

### 3. Solution II: Hazard Pointers (The Gold Standard)
A **Hazard Pointer (HP)** is a thread-local signal saying "I am reading this object, do not delete it."

**The Protocol:**
1.  **Reader:** publish the pointer `P` to a thread-local HP slot.
2.  **Reader:** Verify `P` is still in the data structure. If not, retry.
3.  **Writer (Deleter):** Unlink `P` from the structure.
4.  **Writer:** Check all other threads' HPs.
    *   If `P` is found in any HP, add `P` to a "Retire List" (do not `delete` yet).
    *   If `P` is not found, `delete` immediately.
5.  **Cleanup:** Periodically scan the Retire List and free objects no longer protected by HPs.

*   **Pros:** Wait-free readers, deterministic memory bound.
*   **Cons:** Heavy memory barrier usage (Store-Load fence needed after publishing HP).

### 4. Solution III: Epoch-Based Reclamation (EBR)
Used by `malloc` implementations and databases (like Silo).
*   **Concept:** A Global Epoch counter (E) and per-thread Local Epochs (e_t).
*   **Operation:**
    1.  Global Epoch `E` increments periodically.
    2.  Threads update `e_t = E` when entering a critical section.
    3.  Objects retired in Epoch `E` can be safely deleted when all threads have reached Epoch `E+1` or higher.
*   **Pros:** Extremely fast (just checking integers).
*   **Cons:** One stalled thread prevents *all* memory reclamation (OOM risk).
