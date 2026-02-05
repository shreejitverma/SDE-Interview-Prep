# LOW-LATENCY C++ OPTIMIZATION


For HFT, Game Engines, and Real-Time Systems, every nanosecond counts.

### 17.1 CPU Pipelines & Branch Prediction
Modern CPUs are pipelined. A branch misprediction flushes the pipeline, costing 10-20 cycles.

**Optimization: Branchless Programming**
```cpp
// Branchy (Slow if unpredictable)
if (val > 100) val = 100;

// Branchless (Fast)
// Compiler might generate 'cmov' (Conditional Move) instruction
val = (val > 100) ? 100 : val;
```

**Benchmark: Sorted vs Unsorted Array Processing**
Processing a sorted array is faster due to successful branch prediction.

### 17.2 Data-Oriented Design (DoD)
Stop thinking in "Objects". Think in "Data Transforms".

**OOP (Array of Structures - AoS):**
```cpp
struct Entity {
    float x, y, z;
    int hp;
    // ...
};
vector<Entity> entities; 
// Updating 'x' loads 'hp' into cache (waste)
```

**DoD (Structure of Arrays - SoA):**
```cpp
struct Entities {
    vector<float> x, y, z;
    vector<int> hp;
};
// Updating 'x' loads only 'x' data (SIMD friendly, cache friendly)
```

### 17.3 Prefetching
Use `__builtin_prefetch` (GCC/Clang) or `_mm_prefetch` (Intel) to load data into L1 cache before it's needed.

```cpp
for (int i = 0; i < N; ++i) {
    __builtin_prefetch(&data[i + 16]); // Lookahead
    process(data[i]);
}
```

### 17.4 Micro-Benchmarking (Google Benchmark)
Don't guess; measure. `std::chrono` is often too noisy for nanosecond-scale operations.

```cpp
#include <benchmark/benchmark.h>

static void BM_StringCopy(benchmark::State& state) {
    std::string x = "hello";
    for (auto _ : state) {
        std::string copy = x;
        benchmark::DoNotOptimize(copy); // Prevent optimizing away
    }
}
BENCHMARK(BM_StringCopy);
```

### 17.5 System Warm-up
The first few thousand iterations of code are slow due to:
1.  **Instruction Cache Misses**: Code not yet in CPU cache.
2.  **Data Cache Misses**: Data not yet in L1/L2.
3.  **Branch Predictor**: Hasn't learned the patterns yet.
4.  **OS Page Faults**: Memory pages not yet committed.

**Strategy**: Run a "dummy" loop of your critical path 10,000 times before enabling the network listener or trading signal.

### 17.6 False Sharing Prevention
When two threads modify variables on the same cache line (64 bytes), they invalidate each other's L1 cache.

```cpp
#include <new>

struct SharedData {
    // Bad: a and b likely share a cache line
    std::atomic<int> a;
    std::atomic<int> b;
};

struct PaddedData {
    alignas(std::hardware_destructive_interference_size) std::atomic<int> a;
    alignas(std::hardware_destructive_interference_size) std::atomic<int> b;
};
```

---
