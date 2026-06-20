# Chapter 65: ABA Problem & Memory Reclamation

# ABA PROBLEM & MEMORY RECLAMATION

In the rarefied air of lock-free programming, the **ABA Problem** is the dragon that guards the gate. We saw this in Chapter 42: lock-free algorithms rely on CAS (Compare-And-Swap) to safely update pointers. However, CAS only checks if the *value* of the pointer matches, not if the *object's history* matches. Conquering ABA requires understanding the very fabric of memory lifecycles.

### 1. The Anatomy of the ABA Disaster

Let's revisit the `pop()` method of our lock-free Treiber Stack from Chapter 42.

**The Setup:**
The stack currently holds: `Top -> [A] -> [B] -> [C] -> nullptr`

**The Disaster Sequence:**
1.  **Thread 1 (T1) begins a `pop()`**: It reads `old_head = A`. It reads `A->next` to prepare the new head, which is `B`.
2.  **T1 is preempted by the OS** right before executing the CAS.
3.  **Thread 2 (T2) wakes up and performs a full `pop()`**: It successfully pops `A`. The stack is now `[B] -> [C]`. T2 *deletes* `A`.
4.  **T2 performs another `pop()`**: It pops `B` and deletes it. The stack is now `[C]`.
5.  **T2 performs a `push()`**: It creates a new node containing data `X`.
6.  **The Allocator's Betrayal:** Because node `A` was recently freed, the memory allocator recycles that exact memory address for the new node `X`. T2 pushes this new node.
    The stack is now: `Top -> [A (recycled)] -> [C] -> nullptr`.
7.  **T1 wakes up and executes its CAS:** `head.compare_exchange_weak(A, B)`.
    T1 says: "Is the head still `A`?"
    The CPU checks: Yes, the address at head is `A`.
    The CAS **succeeds**. T1 updates the head to `B`.

**The Result:** Node `B` was deleted in Step 4. The stack head now points to freed memory (`B`). The next thread to touch the stack will segfault. Node `C` has been completely lost (leaked). The data structure is destroyed.

This happens because CAS saw `A -> B -> A`, and incorrectly assumed the world hadn't changed.

***

### 2. Solution I: Tagged Pointers (Double-Word CAS)

To solve ABA, CAS needs more information. Instead of just swapping a 64-bit pointer, we swap a 128-bit structure: a 64-bit pointer AND a 64-bit version counter.

Every time a node is pushed or popped, the counter increments.
*   Initial state: `{ptr: A, version: 1}`
*   After T2's interference: `{ptr: A, version: 4}`
*   T1's CAS now fails because it expects `{A, 1}` but sees `{A, 4}`.

**Implementation (C++11/C++17):**
Requires hardware support for 16-byte atomic CAS (`CMPXCHG16B` on x86_64).

```cpp
template <typename T>
struct TaggedPointer {
    T* ptr;
    uint64_t tag;
};

// Must be 16 bytes and trivially copyable
std::atomic<TaggedPointer<Node>> head;

// Inside push():
TaggedPointer<Node> expected = head.load();
TaggedPointer<Node> new_val = {new_node, expected.tag + 1};
while (!head.compare_exchange_weak(expected, new_val));
```
**Pros:** Easy to understand. Solves ABA definitively.
**Cons:** Double-word CAS is slower. Tag wrapping (overflow) is theoretically possible but practically impossible with 64-bit tags.

***

### 3. Solution II: Hazard Pointers (The Reader's Shield)

Hazard Pointers (invented by Maged Michael) decouple the *logical* removal of a node from the *physical* deletion of its memory.

A **Hazard Pointer (HP)** is a globally visible, thread-local signal saying: "I am actively looking at this memory address. Do not free it."

**The Protocol:**
1.  **Reader (`pop`):** Read the `head`. Publish it to a thread-local Hazard Pointer slot.
2.  **Verification:** Check if `head` changed while publishing. If it did, clear the HP and retry.
3.  **Logical Removal:** Perform the CAS. If successful, the node is logically removed.
4.  **Writer (Deleter):** Instead of `delete old_head`, you add `old_head` to a thread-local "Retire List".
5.  **Reclamation (The Sweep):** When the Retire List gets full, the thread scans *all* Hazard Pointers across all threads.
    *   If a retired pointer is in an HP slot, it is kept in the Retire List.
    *   If a retired pointer is NOT in any HP slot, it is completely safe to `delete`.

**Pros:** Wait-free reads. Strict bound on memory usage (max retired nodes = Threads * HP_Slots).
**Cons:** Complex to implement. Scanning the global HP array can be slow. Requires sequential consistency (heavy fences) to ensure the HP is published before the read.

***

### 4. Solution III: Epoch-Based Reclamation (EBR)

EBR is the backbone of many modern high-performance databases (e.g., Silo, FASTER) and OS kernels (as RCU - Read-Copy-Update). It is blisteringly fast because it relies on coarse-grained epochs rather than tracking individual pointers.

**The Concept:**
There is a Global Epoch counter ($E$) and per-thread Local Epochs ($e_t$).

**The Protocol:**
1.  **Grace Periods:** The Global Epoch $E$ increments periodically (e.g., every 10ms, or after N operations).
2.  **Reader Entry:** When a thread starts an operation, it reads the Global Epoch and sets its Local Epoch to match: $e_t = E$. It also marks itself as "Active".
3.  **Retiring Memory:** When a thread unlinks a node, it doesn't delete it. It places it in a garbage bin tagged with the *current* Global Epoch $E$.
4.  **Reader Exit:** When the thread finishes, it marks itself as "Inactive".
5.  **Reclamation:** A node retired in Epoch $E$ can be safely `delete`d ONLY when all "Active" threads have a Local Epoch of $E+1$ or higher. This guarantees no thread is still stuck in the past looking at the old node.

**Pros:** Extremely low overhead for readers (just a normal atomic store to update their local epoch). Read paths are purely wait-free and involve no heavy fences.
**Cons:** If one thread marks itself "Active" and then stalls (infinite loop, OS freeze), the Global Epoch can never safely advance. The garbage bins will grow infinitely until the system runs Out Of Memory (OOM).

**Godhood Summary:** Use Tagged Pointers for simple structs if 16-byte CAS is available. Use Epoch-Based Reclamation for maximum read throughput when you trust your threads not to stall. Use Hazard Pointers when you need absolute guarantees against OOM in highly antagonistic environments.

