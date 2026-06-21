# Chapter 44: The C++ Memory Model

# THE MEMORY MODEL & ATOMICS

Before C++11, the language specification did not acknowledge the existence of threads. Multithreading was handled by platform-specific libraries (pthreads, Windows API), leading to code that was non-portable and vulnerable to unpredictable compiler and CPU optimizations.

The C++11 Memory Model formalized how threads interact through memory, establishing the rules of the game for high-performance, concurrent programming.

### 1. The Core Rule: Data Races

The foundation of the memory model is a single, unforgiving rule: **If your program contains a Data Race, its behavior is completely Undefined.**

A **Data Race** occurs when:
1.  Two or more threads access the same memory location concurrently.
2.  At least one of the accesses is a write (mutation).
3.  The threads are **not synchronized** (e.g., no mutexes, no atomic operations).

If a data race exists, the compiler is permitted to generate code that does literally anything (crash, corrupt data, or appear to work perfectly until production).

### 2. Cache Coherency and the MESI Protocol

To understand *why* the memory model is necessary, you must understand modern hardware.

CPUs do not read directly from RAM; they read from their local caches (L1, L2, L3). If Thread A on Core 1 modifies a variable, that change is written to Core 1's L1 cache. Core 2 does not instantly see this change.

Hardware solves this using **Cache Coherency Protocols**, most commonly **MESI**:
*   **M (Modified):** This cache line is modified locally and is inconsistent with main memory. No other core has it.
*   **E (Exclusive):** This core has the only copy, and it matches main memory.
*   **S (Shared):** Multiple cores have this cache line. It is read-only and matches main memory.
*   **I (Invalid):** This cache line is out of date.

When Core 1 writes to a shared variable, it must broadcast an "Invalidate" message to all other cores, forcing them to drop their 'S' state copies and fetch the new 'M' state data from Core 1. This cross-core communication is slow.

#### The Nemesis: False Sharing

The CPU fetches memory in chunks called **Cache Lines** (typically 64 bytes). If Thread A frequently modifies `varA` and Thread B frequently modifies `varB`, and both variables happen to reside on the *same* 64-byte cache line, the cores will constantly invalidate each other's caches, even though they aren't sharing data. This is **False Sharing** and it destroys performance.

**The Godhood Fix (`alignas`):**
C++17 introduced `std::hardware_destructive_interference_size`.
```cpp
#include <new>

struct PaddedData {
    alignas(std::hardware_destructive_interference_size) std::atomic<int> counterA;
    alignas(std::hardware_destructive_interference_size) std::atomic<int> counterB;
};
```
This forces `counterA` and `counterB` onto separate cache lines, eliminating false sharing.

***

### 3. Memory Orderings (`std::memory_order`)

Even with cache coherency, compilers and CPUs aggressively **reorder instructions** to keep pipelines full. They will reorder reads and writes as long as it doesn't change the behavior of the *current, single thread*.

`std::atomic<T>` prevents data races. `std::memory_order` tells the compiler and CPU exactly which instruction reorderings are forbidden across *different* threads.

There are three primary models:

#### 3.1 Sequential Consistency (`memory_order_seq_cst`)

The default. It provides a single, global total order of all atomic operations. Everyone agrees on the exact sequence of events.
*   **Cost:** Heavy. On ARM/PowerPC, it issues full memory fences, draining the CPU's store buffers.
*   **Use when:** You have multiple independent variables that must be updated and checked together (e.g., Decker's algorithm).

#### 3.2 Acquire-Release Semantics

This is the workhorse of high-performance concurrency. It provides synchronization between specific pairs of threads, rather than a global total order.

*   **`memory_order_release` (Store):** No memory operations (reads or writes) that appear *before* this store in the source code can be reordered to happen *after* it. It "publishes" previous changes.
*   **`memory_order_acquire` (Load):** No memory operations that appear *after* this load in the source code can be reordered to happen *before* it. It "acquires" published changes.

**The "Synchronizes-With" Relationship:**
If Thread A performs a `release` store, and Thread B performs an `acquire` load of that *same* atomic variable, then everything Thread A did before the store **Happens-Before** everything Thread B does after the load.

```cpp
std::atomic<bool> data_ready(false);
int payload = 0; // Non-atomic payload

void producer() {
    payload = 42; // 1. Write payload
    // 2. Publish payload. The compiler/CPU CANNOT reorder step 1 after step 2.
    data_ready.store(true, std::memory_order_release); 
}

void consumer() {
    // 3. Wait for data. The compiler/CPU CANNOT reorder step 4 before step 3.
    while (!data_ready.load(std::memory_order_acquire)) {
        // spin
    }
    // 4. Safe to read payload. We are guaranteed to see 42.
    assert(payload == 42); 
}
```

#### 3.3 Relaxed Semantics (`memory_order_relaxed`)

No ordering guarantees whatsoever. The operation is simply atomic (no torn reads/writes).
*   **Cost:** Essentially zero. Just a normal assembly instruction.
*   **Use when:** You only need atomicity, not synchronization (e.g., a simple hit counter or stats aggregator where the exact temporal order doesn't matter).

```cpp
std::atomic<int> global_counter(0);

void do_work() {
    // We don't care about order, just that the count is accurate.
    global_counter.fetch_add(1, std::memory_order_relaxed);
}
```

***

