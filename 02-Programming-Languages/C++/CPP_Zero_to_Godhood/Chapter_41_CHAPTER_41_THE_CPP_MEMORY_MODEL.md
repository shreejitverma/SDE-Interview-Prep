# CHAPTER 41: THE CPP MEMORY MODEL


# THE MEMORY MODEL & ATOMICS

## 1. The C++ Memory Model

Defined in C++11. It guarantees that if you have a data race, you have Undefined Behavior.

**Data Race:** Two threads access the same memory location concurrently, at least one is a write, and they are not synchronized (no mutex, no atomics).

## 2. Atomic Operations

`std::atomic<T>` ensures individual read/modify/write operations are indivisible.

```cpp
std::atomic<int> count = 0;
count++; // Thread-safe fetch-add
```

## 3. Memory Orderings

This is where C++ becomes "Godhood" level.

*   `memory_order_relaxed`: No ordering guarantees. Just atomicity.
*   `memory_order_acquire`: (Load) Subsequent reads/writes stay after this load.
*   `memory_order_release`: (Store) Prior reads/writes stay before this store.
*   `memory_order_acq_rel`: Both.
*   `memory_order_seq_cst`: (Default) Sequential Consistency. Global total ordering. Expensive.

### 3.1 Synchronizes-With

If Thread A stores with `release` and Thread B loads with `acquire`, everything A did before the store is visible to B after the load.

```cpp
std::atomic<bool> ready = false;
int data = 0;

void producer() {
    data = 42;
    ready.store(true, std::memory_order_release); // "Publish" data
}

void consumer() {
    while (!ready.load(std::memory_order_acquire)); // Wait and "Acquire"
    assert(data == 42); // Guaranteed to see 42
}
```

## 4. Fences

`std::atomic_thread_fence`. Used to enforce ordering without an atomic operation, or to combine with `relaxed` operations.
