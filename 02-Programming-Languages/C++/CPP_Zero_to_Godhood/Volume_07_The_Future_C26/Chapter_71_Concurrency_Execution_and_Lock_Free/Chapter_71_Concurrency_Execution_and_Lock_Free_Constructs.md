# Chapter 71: Concurrency, Execution, and Lock-Free Constructs

For over a decade, the C++ concurrency story has been a tale of two extremes: either you use high-level, heavy abstractions like `std::async` and `std::thread`, which incur massive OS overhead and lack composability, or you drop down to the hyper-complex world of `std::atomic` and memory orders, fighting cache-coherence protocols and ABA problems manually.

C++26 fundamentally rewrites this narrative. It introduces the `std::execution` framework—a unified, zero-overhead paradigm for asynchronous execution based on Senders and Receivers. Simultaneously, it delivers powerful lock-free primitives, including Hazard Pointers and Read-Copy-Update (RCU), directly into the standard library.

This chapter is the definitive guide to modern C++26 concurrency. We will explore how to write non-blocking, multi-core algorithms that scale linearly, avoiding both thread-spawning overhead and lock contention.

---

## 71.1 The Failure of `std::async` and Futures

Before diving into C++26, we must understand why `std::async` and `std::future` (introduced in C++11) failed to scale for systems engineering.

When you call `std::async(std::launch::async, func)`, the standard library typically spawns a brand new OS thread. An OS thread requires a massive context switch, allocates a 1-8MB stack, and introduces scheduling latency. 

If you wanted to chain operations (e.g., "download a file, *then* parse it, *then* save it"), `std::future` provided no `.then()` method in C++11. You were forced to block a thread waiting for the future to resolve:

```cpp
// Pre-C++26 blocking code (bad for scalability)
std::future<Data> f1 = std::async(download);
Data d = f1.get(); // BLOCKS THE THREAD!
std::future<Result> f2 = std::async(parse, d);
```

Blocking threads destroys scalability in highly concurrent servers because thread pools quickly become exhausted.

---

## 71.2 `std::execution`: The Sender/Receiver Paradigm

C++26 introduces the `std::execution` namespace, built on a completely different philosophy: **Senders and Receivers**.

Instead of eagerly launching work, you construct a graph of asynchronous operations (a Sender). Work only begins when a Receiver is attached to the Sender and `std::execution::start` is called.

### 71.2.1 The Three Core Concepts

1. **Sender:** An object that describes *what* work needs to be done. It is a lazy recipe. It produces values, errors, or a stopped signal.
2. **Receiver:** An object that consumes the output of a Sender. It has three channels: `set_value`, `set_error`, and `set_stopped`.
3. **Scheduler:** A lightweight handle to an execution context (e.g., a thread pool, an I/O ring, a GPU stream). It determines *where* the work happens.

### 71.2.2 Building a Sender Chain

Let's look at how to cleanly chain operations without blocking any threads.

```cpp
#include <execution>
#include <iostream>

namespace ex = std::execution;

// Assume we have a thread pool
extern ex::scheduler auto my_thread_pool;

void execute_pipeline() {
    // 1. Schedule work on the thread pool
    auto sender = ex::schedule(my_thread_pool)
        // 2. Chain operations lazily
        | ex::then([] { return "Data downloaded"; })
        | ex::then([](std::string data) { return data + " and parsed"; });
        
    // At this point, NOTHING has executed. 'sender' is just a description of work.
    // In fact, 'sender' is likely a massive, zero-cost template expression.
    
    // 3. Connect a receiver and start the work
    // ex::sync_wait is a utility that blocks the CURRENT thread until the sender completes,
    // but internally, the sender executes non-blocking on the thread pool.
    auto [result] = ex::sync_wait(sender).value();
    
    std::cout << result << '
';
}
```

This pipeline `sender | ex::then(...)` generates no virtual function calls, allocates no memory on the heap (unless explicitly requested), and allows the compiler to inline the entire chain of lambdas into a single, highly optimized state machine.

---

## 71.3 Advanced Sender Algorithms

The standard library provides numerous algorithms to compose Senders.

### 71.3.1 `when_all`

Executes multiple senders concurrently and aggregates their results.

```cpp
auto s1 = ex::just(42); // A sender that immediately produces 42
auto s2 = ex::just(3.14);

auto combined = ex::when_all(s1, s2);
auto [val1, val2] = ex::sync_wait(combined).value();
```

### 71.3.2 `let_value`

Sometimes, the next asynchronous operation you want to run depends on the result of the previous one. `ex::then` is for synchronous continuations; `ex::let_value` is for asynchronous continuations (similar to a monad's `flatMap`).

```cpp
auto fetch_data = ex::schedule(my_thread_pool) | ex::then([]{ return 100; });

auto pipeline = fetch_data | ex::let_value([](int id) {
    // We return a NEW sender based on the 'id'
    return fetch_user_profile(id);
});
```

---

## 71.4 Parallel Range Algorithms and Execution Policies

C++17 introduced `std::execution::par`, but it was restricted to iterator-based algorithms. C++26 finally marries `std::execution` policies with `std::ranges`.

```cpp
#include <algorithm>
#include <execution>
#include <vector>

struct Trade { double price; };

void parallel_sort(std::vector<Trade>& trades) {
    // C++26: Parallel execution policy applied directly to a Range!
    std::ranges::sort(std::execution::par_unseq, trades, {}, &Trade::price);
}
```

The `par_unseq` policy tells the compiler it is free to use multiple CPU threads *and* SIMD vectorization to sort the range. Because it works with ranges, we can elegantly pass the `&Trade::price` projection instead of writing a verbose lambda.

---

## 71.5 Lock-Free Data Structures and Safe Memory Reclamation

While `std::execution` handles coarse-grained task scheduling, low-latency applications (like HFT and audio drivers) require fine-grained, lock-free data structures. 

A traditional `std::mutex` blocks a thread. A blocked thread is descheduled by the OS, taking microseconds to wake up. Lock-free programming avoids this, but it introduces a massive problem: **Memory Reclamation**.

If Thread A removes a node from a lock-free linked list, it cannot immediately `delete` the node, because Thread B might still be reading it. Before C++26, solving this required incredibly complex custom garbage collectors. C++26 introduces two standard solutions: Hazard Pointers and RCU.

---

## 71.6 Hazard Pointers (`std::hazard_pointer`)

Hazard Pointers provide safe, lock-free memory reclamation by allowing threads to "announce" which pointers they are currently reading.

If Thread A is reading `ptr`, it creates a Hazard Pointer pointing to `ptr`. When Thread B decides to delete `ptr`, it checks the global list of Hazard Pointers. Since Thread A is holding one, Thread B adds `ptr` to a "retired" list instead of deleting it. Once Thread A releases the Hazard Pointer, the system eventually reclaims the memory.

### 71.6.1 Using Hazard Pointers

```cpp
#include <hazard_pointer>
#include <atomic>
#include <iostream>

struct Node {
    int data;
    std::atomic<Node*> next;
};

std::atomic<Node*> head{nullptr};

void lock_free_read() {
    // 1. Acquire a hazard pointer
    std::hazard_pointer hz = std::make_hazard_pointer();
    
    // 2. Safely protect the read
    Node* current = hz.protect(head);
    
    if (current) {
        // We can safely read current->data! 
        // No other thread can delete 'current' while 'hz' is alive.
        std::cout << current->data << '
';
    }
    // 'hz' goes out of scope, releasing protection.
}

void lock_free_delete(Node* expected) {
    // Assume we successfully unlinked 'expected' from the list using CAS.
    // We cannot delete it immediately! We retire it.
    std::hazard_pointer_obj_base<Node>::retire(expected);
}
```

Hazard pointers are highly optimized. Reading (acquiring protection) is wait-free and requires no atomic read-modify-write loops. Retiring is also incredibly fast.

---

## 71.7 Read-Copy-Update (`std::rcu`)

Read-Copy-Update (RCU) is an alternative memory reclamation strategy heavily used in the Linux Kernel. It is optimized for data structures that are read *constantly* but updated *rarely* (e.g., routing tables, configuration maps).

RCU operates on the concept of "grace periods". Readers declare when they enter and exit a critical section. Writers create a *copy* of the data, update the copy, and swap the atomic pointer. The old data is deleted only after a "grace period" passes—meaning all readers that started before the swap have finished.

### 71.7.1 RCU in C++26

```cpp
#include <rcu>
#include <atomic>
#include <iostream>

struct Config {
    int timeout_ms;
};

std::atomic<Config*> global_config{new Config{100}};

void read_config() {
    // 1. Enter an RCU read-side critical section
    std::scoped_lock lock(std::rcu_default_domain());
    
    // 2. Read the pointer
    Config* cfg = global_config.load(std::memory_order_acquire);
    
    // 3. Safe to use 'cfg'. Even if a writer updates global_config,
    // the old 'cfg' memory won't be deleted until our lock is destroyed.
    std::cout << cfg->timeout_ms << '
';
}

void update_config(int new_timeout) {
    Config* new_cfg = new Config{new_timeout};
    
    // 1. Swap the pointer
    Config* old_cfg = global_config.exchange(new_cfg, std::memory_order_acq_rel);
    
    // 2. Retire the old pointer. It will be deleted asynchronously 
    // once all current RCU readers have finished.
    std::rcu_retire(old_cfg);
}
```

**RCU vs Hazard Pointers:**
*   **RCU Readers:** Absolutely zero overhead. Entering the RCU domain is effectively a no-op compiler barrier. It is infinitely scalable across cores.
*   **RCU Writers:** Slower, as retiring requires synchronizing grace periods across threads.
*   **Hazard Pointers:** Readers have a slight overhead (atomic write to the hazard array), but writers don't have to wait for global grace periods.

---

## 71.8 Lock-Free Atomics: `fetch_max` and `fetch_min`

To round out the concurrency upgrades, C++26 adds hardware-accelerated `fetch_max` and `fetch_min` to `std::atomic`.

Previously, atomically updating a maximum value required a slow Compare-And-Swap (CAS) loop:

```cpp
// Pre-C++26 CAS Loop
void update_max(std::atomic<int>& max_val, int new_val) {
    int current = max_val.load(std::memory_order_relaxed);
    while (current < new_val && 
           !max_val.compare_exchange_weak(current, new_val, std::memory_order_relaxed)) {
        // loop
    }
}
```

Under heavy contention, CAS loops cause massive cache-line bouncing (the MESI protocol is hammered). 

In C++26, you simply write:

```cpp
// C++26
void update_max(std::atomic<int>& max_val, int new_val) {
    max_val.fetch_max(new_val, std::memory_order_relaxed);
}
```

On ARM architectures, this maps directly to a single `LDAMAX` instruction. On x86, it might compile to an optimized microcode loop, but it provides a clean, standardized intent.

---

## 71.9 Deep Dive: The MESI Protocol and False Sharing

To truly master C++26 concurrency, you must understand why `std::execution` and RCU are necessary at the hardware level.

Modern CPUs do not read directly from RAM; they read from L1/L2/L3 caches in 64-byte chunks called **cache lines**. To keep multiple CPU cores synchronized, processors use the **MESI** (Modified, Exclusive, Shared, Invalid) protocol.

If Thread A and Thread B continuously atomic-write to the same cache line (even different variables in that same line), they trigger **False Sharing**. Core A modifies the line, invalidating Core B's cache. Core B modifies it, invalidating Core A's cache. The CPUs spend all their time passing cache lines over the ring bus instead of doing math.

**How C++26 Solves This:**
1. `std::execution` Senders are localized to specific thread queues. Data is processed locally without atomic contention until the very end.
2. `std::rcu` allows readers to keep cache lines in the **Shared** state. Because readers do not perform atomic writes to tracking structures (unlike `std::shared_ptr` ref counts), the cache line is never invalidated! RCU allows linear scaling to 128+ cores precisely because it respects the MESI protocol.

## 71.10 Conclusion

The concurrency landscape in C++26 is breathtaking. By discarding the heavy, blocking semantics of `std::async` and embracing the lazy, composable nature of Senders/Receivers, we can build pipelines that compile down to pure state machines. By integrating `std::rcu` and `std::hazard_pointer`, the standard library finally gives systems engineers the tools to safely manage memory in wait-free algorithms.

In Chapter 72, we will look at how C++26 revolutionizes the data structures we actually process concurrently: `std::simd`, `inplace_vector`, `hive`, and multidimensional numerics.

## 71.11 Advanced Execution Schedulers: GPU and I/O Rings
Beyond simple thread pools, the true power of `std::execution` lies in its ability to schedule work on specialized hardware or OS APIs...