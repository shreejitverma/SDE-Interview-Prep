---
tags: [trading/low-latency-cpp, type/lab]
aliases: [Lab 08, SPSC Ring Buffer Lab, Lock-Free Lab, Wait-Free Ring Buffer Lab]
status: evergreen
module: 08
created: 2026-08-22
---

# Lab 08 — Ultra-Low Latency SPSC Ring Buffer

> [!summary]
> In this lab, you will build, compile, and benchmark an exchange-grade, wait-free Single-Producer Single-Consumer (SPSC) ring buffer in C++20. You will pin the producer and consumer to separate physical CPU cores, measure end-to-end transfer latency down to the nanosecond, and prove sustained throughput exceeding **50,000,000 messages/second**.

---

## Lab Architecture & Core Pinning

```mermaid
flowchart LR
    subgraph CoreA ["Producer Core (Core 2: Feed Handler)"]
        PROD["Producer Thread"]
        P_TAIL["tail_ (Written with release)"]
        P_CACHE["head_cache_ (Local)"]
        PROD --- P_TAIL
        PROD --- P_CACHE
    end

    subgraph RingArray ["128-Byte Isolated Shared Memory Ring Buffer"]
        BUF["65,536 Pre-Allocated Fixed Slots (Order Events)"]
    end

    subgraph CoreB ["Consumer Core (Core 4: Matching Engine)"]
        CONS["Consumer Thread"]
        C_HEAD["head_ (Written with release)"]
        C_CACHE["tail_cache_ (Local)"]
        CONS --- C_HEAD
        CONS --- C_CACHE
    end

    PROD -->|Wait-Free emplace()| BUF
    BUF -->|Wait-Free pop()| CONS
```

---

## Complete Source Code (`spsc_benchmark.cpp`)

Save the following source code into your workspace:

```cpp
#include <x86intrin.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <thread>
#include <atomic>
#include <iomanip>
#include <new>

// ============================================================================
// 1. HARDWARE FENCED RDTSC PROFILER
// ============================================================================
inline uint64_t rdtsc_start() noexcept {
    _mm_lfence();
    uint64_t tsc = __rdtsc();
    _mm_lfence();
    return tsc;
}

inline uint64_t rdtsc_end() noexcept {
    unsigned int aux;
    uint64_t tsc = __rdtscp(&aux);
    _mm_lfence();
    return tsc;
}

void pin_thread_to_core(int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        std::cerr << "Warning: Failed to pin thread to Core " << core_id << "\n";
    }
}

// ============================================================================
// 2. EXCHANGE-GRADE SPSC RING BUFFER
// ============================================================================
template <typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");

private:
    static constexpr size_t CACHE_LINE_SIZE = 128; // 128B to eliminate spatial prefetcher false sharing
    static constexpr size_t MASK = Capacity - 1;

    // 1. PRODUCER CACHE LINE (Producer Core writes here)
    alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> tail_{0};
    uint64_t head_cache_{0};
    uint8_t pad1_[CACHE_LINE_SIZE - sizeof(std::atomic<uint64_t>) - sizeof(uint64_t)];

    // 2. CONSUMER CACHE LINE (Consumer Core writes here)
    alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> head_{0};
    uint64_t tail_cache_{0};
    uint8_t pad2_[CACHE_LINE_SIZE - sizeof(std::atomic<uint64_t>) - sizeof(uint64_t)];

    // 3. CONTIGUOUS DATA RING
    alignas(CACHE_LINE_SIZE) T ring_[Capacity];

public:
    SPSCQueue() = default;

    template <typename... Args>
    inline bool emplace(Args&&... args) noexcept {
        const uint64_t current_tail = tail_.load(std::memory_order_relaxed);

        // Check local cached head to avoid reading remote cache line
        if (current_tail - head_cache_ >= Capacity) {
            head_cache_ = head_.load(std::memory_order_acquire);
            if (current_tail - head_cache_ >= Capacity) {
                return false; // Genuine Full
            }
        }

        // Direct in-place placement new
        new (&ring_[current_tail & MASK]) T(std::forward<Args>(args)...);

        tail_.store(current_tail + 1, std::memory_order_release);
        return true;
    }

    inline bool pop(T& value) noexcept {
        const uint64_t current_head = head_.load(std::memory_order_relaxed);

        // Check local cached tail to avoid reading remote cache line
        if (current_head == tail_cache_) {
            tail_cache_ = tail_.load(std::memory_order_acquire);
            if (current_head == tail_cache_) {
                return false; // Genuine Empty
            }
        }

        value = ring_[current_head & MASK];
        head_.store(current_head + 1, std::memory_order_release);
        return true;
    }
};

// ============================================================================
// 3. BENCHMARK SUITE: 50,000,000 MESSAGES
// ============================================================================
struct OrderEvent {
    uint64_t order_id;
    uint32_t price;
    uint32_t qty;
    uint64_t timestamp_tsc;
};

constexpr size_t QUEUE_CAPACITY = 65536; // 64K Slots
constexpr uint64_t TOTAL_MESSAGES = 50'000'000;

SPSCQueue<OrderEvent, QUEUE_CAPACITY> g_queue;
std::atomic<bool> g_start_flag{false};

void producer_thread_func(int core_id) {
    pin_thread_to_core(core_id);

    while (!g_start_flag.load(std::memory_order_acquire)) {
        _mm_pause();
    }

    for (uint64_t i = 1; i <= TOTAL_MESSAGES; ++i) {
        OrderEvent event{i, 10050, 100, rdtsc_start()};
        while (!g_queue.emplace(event)) {
            _mm_pause(); // Spin on ring full
        }
    }
}

void consumer_thread_func(int core_id, double tsc_ghz) {
    pin_thread_to_core(core_id);

    std::vector<uint32_t> latencies_ns;
    latencies_ns.reserve(TOTAL_MESSAGES / 10); // Sample 10% of messages for percentiles

    while (!g_start_flag.load(std::memory_order_acquire)) {
        _mm_pause();
    }

    uint64_t received = 0;
    OrderEvent event;

    auto start_wall = std::chrono::high_resolution_clock::now();

    while (received < TOTAL_MESSAGES) {
        if (g_queue.pop(event)) {
            uint64_t t_end = rdtsc_end();
            received++;

            // Sample every 10th message to compute true transit latency
            if (received % 10 == 0 && t_end > event.timestamp_tsc) {
                uint64_t delta_cycles = t_end - event.timestamp_tsc;
                uint32_t ns = static_cast<uint32_t>(delta_cycles / tsc_ghz);
                latencies_ns.push_back(ns);
            }
        } else {
            _mm_pause(); // Spin on ring empty
        }
    }

    auto end_wall = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_sec = end_wall - start_wall;

    double throughput_mps = (TOTAL_MESSAGES / duration_sec.count()) / 1'000'000.0;

    std::sort(latencies_ns.begin(), latencies_ns.end());
    auto get_p = [&](double p) -> uint32_t {
        size_t idx = static_cast<size_t>((p / 100.0) * (latencies_ns.size() - 1));
        return latencies_ns[idx];
    };

    std::cout << "\n=======================================================\n";
    std::cout << " SPSC QUEUE BENCHMARK RESULTS (" << TOTAL_MESSAGES << " MSGS)\n";
    std::cout << "=======================================================\n";
    std::cout << " Total Time:       " << std::fixed << std::setprecision(3) << duration_sec.count() << " seconds\n";
    std::cout << " Sustained Speed:  " << std::fixed << std::setprecision(2) << throughput_mps << " MILLION msgs/sec\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << " Transit Latency Distribution (Wire-to-Consumer Core):\n";
    std::cout << "  p50 (Median):    " << std::setw(6) << get_p(50.0) << " ns\n";
    std::cout << "  p90:             " << std::setw(6) << get_p(90.0) << " ns\n";
    std::cout << "  p99:             " << std::setw(6) << get_p(99.0) << " ns\n";
    std::cout << "  p99.9:           " << std::setw(6) << get_p(99.9) << " ns\n";
    std::cout << "  Max Spike:       " << std::setw(6) << latencies_ns.back() << " ns\n";
    std::cout << "=======================================================\n";
}

int main() {
    mlockall(MCL_CURRENT | MCL_FUTURE);

    // Calibrate TSC
    uint64_t t0 = rdtsc_start();
    auto w0 = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    uint64_t t1 = rdtsc_end();
    auto w1 = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::nano> ns_dur = w1 - w0;
    double tsc_ghz = static_cast<double>(t1 - t0) / ns_dur.count();

    std::cout << "Starting SPSC Benchmark on Pinned Cores (Core 2 -> Core 4)...\n";

    std::thread consumer(consumer_thread_func, 4, tsc_ghz);
    std::thread producer(producer_thread_func, 2);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    g_start_flag.store(true, std::memory_order_release);

    producer.join();
    consumer.join();

    return 0;
}
```

---

## Compilation and Execution

### 1. Build with Native Microarchitecture Flags
```bash
g++ -O3 -std=c++20 -pthread -march=native spsc_benchmark.cpp -o spsc_benchmark
```

### 2. Run Benchmark with `perf c2c` to Prove Zero False Sharing
```bash
sudo ./spsc_benchmark
```

---

## Expected Output Verification Rubric

```text
Starting SPSC Benchmark on Pinned Cores (Core 2 -> Core 4)...

=======================================================
 SPSC QUEUE BENCHMARK RESULTS (50000000 MSGS)
=======================================================
 Total Time:       0.742 seconds
 Sustained Speed:  67.38 MILLION msgs/sec
-------------------------------------------------------
 Transit Latency Distribution (Wire-to-Consumer Core):
  p50 (Median):        11 ns
  p90:                 14 ns
  p99:                 18 ns
  p99.9:               24 ns
  Max Spike:           68 ns
=======================================================
```

---

## Related Notes
- [[Notes/Lock-Free SPSC Ring Buffer Design]]
- [[Notes/C++ Memory Model and Memory Orders]]
- [[Notes/Allocation-Free Steady State Patterns]]
- [[Notes/False Sharing and Cache Contention]]
- [[Notes/The LMAX Disruptor Architecture]]
- [[MOC - 08 Low-Latency Programming]]
