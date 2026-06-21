# Chapter 45: Lock Free Programming

# LOCK-FREE PROGRAMMING

Lock-free programming is the art of designing concurrent data structures without mutexes. It is essential for low-latency systems (like HFT) where a thread stalling on a lock (due to an OS context switch or page fault) is unacceptable.

### 1. Progress Guarantees

Concurrency algorithms are classified by their progress guarantees when multiple threads contend:

1.  **Blocking (Mutexes):** If a thread holding the lock dies or is suspended, all other threads stall forever. No progress guarantee.
2.  **Obstruction-Free:** A thread makes progress if it executes in isolation (no contention). Rarely used in practice.
3.  **Lock-Free:** If multiple threads contend, at least **one** thread is guaranteed to make progress in a finite number of steps. The system as a whole always moves forward, even if individual threads starve.
4.  **Wait-Free:** Every thread is guaranteed to make progress in a finite number of steps, regardless of contention. The Holy Grail (and incredibly difficult to achieve).

### 2. The Atomic Primitive: Compare-And-Swap (CAS)

The heart of lock-free programming is the atomic **Compare-And-Swap** (CAS) operation. In C++, this is `compare_exchange_weak` and `compare_exchange_strong`.

**The Mechanism:**
1.  You read the current value (`expected`).
2.  You calculate a `new_value`.
3.  You execute CAS: "If the atomic variable still equals `expected`, atomically change it to `new_value` and return `true`. Otherwise, update `expected` to the *actual* current value and return `false`."

#### Weak vs. Strong

*   **`compare_exchange_weak`:** Can fail *spuriously* (return false even if the value matches), usually due to CPU cache dynamics (e.g., a context switch occurring exactly during the instruction). It is faster on ARM/PowerPC. **Always use inside a loop.**
*   **`compare_exchange_strong`:** Never fails spuriously. Costs more on some architectures. Use when the logic is outside a loop.

### 3. Anatomy of a Lock-Free Stack (The Treiber Stack)

Let's build the quintessential lock-free structure: a thread-safe LIFO stack.

```cpp
template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
        Node(const T& data) : data(data), next(nullptr) {}
    };

    std::atomic<Node*> head{nullptr};

public:
    // PUSH: Wait-Free (usually)
    void push(const T& data) {
        Node* new_node = new Node(data);
        new_node->next = head.load(std::memory_order_relaxed);

        // CAS Loop
        while (!head.compare_exchange_weak(new_node->next, new_node,
                                           std::memory_order_release,
                                           std::memory_order_relaxed)) {
            // If CAS fails, new_node->next is automatically updated to the new head.
            // We just loop and try again.
        }
    }

    // POP: Lock-Free
    std::unique_ptr<T> pop() {
        Node* old_head = head.load(std::memory_order_acquire);
        
        while (old_head && 
               !head.compare_exchange_weak(old_head, old_head->next,
                                           std::memory_order_acquire,
                                           std::memory_order_relaxed)) {
            // Loop until we successfully swap head to head->next
        }

        if (old_head) {
            std::unique_ptr<T> res(new T(std::move(old_head->data)));
            delete old_head; // DANGER: See Chapter 58 (ABA Problem)
            return res;
        }
        return nullptr;
    }
};
```

**Why it works:**
If two threads try to `push` simultaneously, both read the same `head`. The hardware ensures only *one* CAS succeeds. The winner's node becomes the new head. The loser's CAS fails, its `new_node->next` is updated to the winner's node, and it tries again. The system made progress (one node was pushed).

**The Lethal Flaw:**
Look at `delete old_head` in the `pop()` method. If Thread A reads `old_head`, gets suspended, and Thread B deletes that node, Thread A will wake up and try to read `old_head->next` during its CAS. This is a Use-After-Free segfault.

Worse, it leads to the **ABA Problem**, which we must solve using advanced Memory Reclamation techniques.

