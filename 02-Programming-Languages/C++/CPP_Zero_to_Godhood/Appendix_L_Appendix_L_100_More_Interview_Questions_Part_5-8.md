# Appendix L: 100 More Interview Questions (Part 5-8)


These questions are designed to separate the "Senior Engineers" from the "Gods." If you can answer these without looking at the notes, you are ready for any HFT or Systems Architecture interview on the planet.

## Part 5: The C++ Memory Model & Atomics

### 1. What is the difference between `std::memory_order_relaxed` and `std::memory_order_seq_cst`?
**Answer**: `seq_cst` (Sequentially Consistent) provides a global total ordering of all operations. It is the safest but slowest. `relaxed` only guarantees atomicity of the operation itselfit provides no guarantees about the order of other memory operations.

### 2. Explain "Release-Acquire" semantics.
**Answer**: A `memory_order_release` store "synchronizes-with" a `memory_order_acquire` load of the same variable. All memory writes performed by the storing thread *before* the release store are guaranteed to be visible to the loading thread *after* the acquire load.

### 3. What is a "Fences" (Memory Barrier)?
**Answer**: A fence is an instruction that prevents the CPU or compiler from reordering instructions across the fence boundary. `std::atomic_thread_fence` can be used to establish synchronization without a specific atomic variable.

### 4. What is the ABA problem in lock-free programming?
**Answer**: It occurs when a thread reads a value A, another thread changes it to B and then back to A. The first thread thinks nothing has changed, but it might have (e.g., a node in a linked list was deleted and a new one was allocated at the same address).
**Fix**: Use versioned pointers (hazard pointers) or `std::atomic<T>::compare_exchange_strong` with a counter.

### 5. Why is `compare_exchange_weak` used in a loop instead of `strong`?
**Answer**: On some architectures (like ARM/Load-Link Store-Conditional), `weak` can fail spuriously even if the values match. However, `weak` is faster in a loop because it allows the compiler to generate more efficient code.

---

## Part 6: Lock-Free Structures & Concurrency

### 6. Implement a Lock-Free Stack (Treiber Stack).
```cpp
template <typename T>
class LockFreeStack {
    struct Node { T data; Node* next; };
    std::atomic<Node*> head;
public:
    void push(T val) {
        Node* newNode = new Node{val, head.load()};
        while (!head.compare_exchange_weak(newNode->next, newNode));
    }
};
```

### 7. What is "False Sharing" and how do you prevent it in C++17?
**Answer**: It happens when two independent atomic variables reside on the same CPU cache line. Updating one invalidates the cache for the other core.
**Fix**: Use `alignas(hardware_destructive_interference_size)` from `<new>`.

### 8. Explain the "Double-Checked Locking" pattern and why it was broken before C++11.
**Answer**: It was broken because the compiler could reorder the object allocation and the pointer assignment, leading a second thread to see a non-null pointer to an uninitialized object. C++11's memory model (and `std::atomic`) fixed this.

---

## Part 7: Template Metaprogramming (TMP)

### 9. What is SFINAE? Give a concrete example.
**Answer**: "Substitution Failure Is Not An Error." It allows the compiler to discard a template overload if the type substitution fails, instead of throwing a hard error.
```cpp
template <typename T>
auto func(T t) -> decltype(t.push_back(0)) { ... } // Only works for containers
```

### 10. How do C++20 Concepts improve upon SFINAE?
**Answer**: Concepts provide a formal, readable way to constrain templates. Instead of cryptic template vomit, you get clear errors: "Type X does not satisfy requirement 'HasPushBack'."

### 11. What is the Curiously Recurring Template Pattern (CRTP)?
**Answer**: A pattern where a class `Derived` inherits from `Base<Derived>`. It allows for "Static Polymorphism"achieving polymorphic behavior without the cost of virtual functions.

### 12. Explain `std::void_t` and how it's used for trait detection.
**Answer**: `void_t` is a template that always maps any list of types to `void`. It's used to check if a certain member or type exists within a class during template instantiation.

---

## Part 8: Systems & Performance

### 13. What is RTTI and why do HFT developers often disable it?
**Answer**: Runtime Type Information. It powers `dynamic_cast` and `typeid`. It's disabled (`-fno-rtti`) to save space in the binary and avoid the overhead of storing type info in the vtable.

### 14. What is the difference between `inline` and `__attribute__((always_inline))`?
**Answer**: `inline` is just a suggestion; the compiler can ignore it. `always_inline` (a GCC/Clang intrinsic) forces the compiler to inline the function unless it's physically impossible.

### 15. Explain "Instruction Cache Warming."
**Answer**: It's the practice of running a piece of code (like a trading strategy) with "dummy data" before the market opens, just to ensure the instructions are loaded into the CPU's L1-Instruction cache.

---

*Note: This is just the beginning. The next 85 questions in your journey will cover everything from SIMD intrinsics to Linux Kernel tuning. Keep pushing. The machine is waiting.*
