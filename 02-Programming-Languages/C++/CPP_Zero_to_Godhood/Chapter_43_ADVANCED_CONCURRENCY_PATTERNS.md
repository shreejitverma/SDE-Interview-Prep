# ADVANCED CONCURRENCY PATTERNS


# ADVANCED CONCURRENCY PATTERNS

## 1. Thread Pools

Spawning threads is expensive (syscall, stack allocation). Thread pools reuse threads.

```cpp
// Basic concept
class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    // ...
};
```

## 2. The Actor Model

No shared state. Actors communicate via messages.

*   Each Actor has a mailbox (queue) and a thread (or shared thread pool).
*   Eliminates locking issues by design.
*   Frameworks: CAF (C++ Actor Framework).

## 3. Disruptor Pattern

Ring buffer based, high-throughput, low-latency inter-thread messaging. Used in HFT.

*   Pre-allocated memory (avoid GC/allocations).
*   Single Writer / Multiple Reader or Multiple Writer scenarios.
*   Uses memory barriers/fences instead of locks.

## 4. Coroutines for Concurrency

Using C++20 Coroutines to write async code that looks sync.

```cpp
Task<int> async_algo() {
    int a = co_await fetch_a();
    int b = co_await fetch_b();
    co_return a + b;
}
```

## 5. False Sharing

When two threads write to different variables that happen to sit on the same cache line.

**Fix:** `alignas(64)` (typical cache line size).

```cpp
struct alignas(64) PaddedCounter {
    std::atomic<int> val;
};
```


---
