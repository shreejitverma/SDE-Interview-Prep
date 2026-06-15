# CHAPTER 24: C17 PARALLEL ALGORITHMS AND CONCURRENCY


# C++17 PARALLEL ALGORITHMS & CONCURRENCY

## 1. Parallel Algorithms (`std::execution`)

Standard algorithms (`sort`, `transform`, `for_each`) now accept an execution policy.

```cpp
#include <algorithm>
#include <execution>
#include <vector>

std::vector<int> v(1'000'000);

// Sequential (default)
std::sort(std::execution::seq, v.begin(), v.end());

// Parallel (multi-threaded)
std::sort(std::execution::par, v.begin(), v.end());

// Parallel + Vectorized (SIMD allowed)
std::sort(std::execution::par_unseq, v.begin(), v.end());
```

**Note:** `par_unseq` allows interleaving of instructions, so user code must be vector-safe (no mutexes, no allocations).

## 2. `std::scoped_lock`

Multi-lock RAII wrapper. Prevents deadlocks by locking multiple mutexes safely (using a deadlock-avoidance algorithm).

```cpp
std::mutex m1, m2;

void swap_data() {
    // Locks both m1 and m2 atomically
    std::scoped_lock lock(m1, m2);
    // ...
}
```

## 3. `std::shared_mutex`

Standard reader-writer lock (was `shared_timed_mutex` in C++14).

```cpp
#include <shared_mutex>

std::shared_mutex smtx;

// Writer
std::unique_lock lock(smtx);

// Reader
std::shared_lock lock(smtx);
```
