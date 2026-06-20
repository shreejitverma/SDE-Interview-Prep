# Chapter 16: Concurrency with OpenMP

OpenMP (Open Multi-Processing) is an API that supports multi-platform shared-memory multiprocessing programming in C, C++, and Fortran. It is widely used in high-performance computing (HPC) for parallelizing loops and sections of code with simple directives.

## 16.1 Getting Started with OpenMP

OpenMP uses compiler directives (`#pragma omp`) to parallelize code.

### 1. Parallel Regions
The most basic directive is `#pragma omp parallel`. It creates a team of threads to execute the following block.
```cpp
#include <iostream>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        std::cout << "Hello from thread " << id << std::endl;
    }
    return 0;
}
```

### 2. Parallelizing Loops
OpenMP excels at parallelizing independent iterations of a loop.
```cpp
#pragma omp parallel for
for (int i = 0; i < 1000; i++) {
    results[i] = compute(i);
}
```

---

## 16.2 Data Sharing Attributes

*   **`shared`**: Variables are accessible by all threads.
*   **`private`**: Each thread has its own local copy of the variable.
*   **`reduction`**: Combines private copies into a single shared variable (e.g., sum, product).

```cpp
double total = 0;
#pragma omp parallel for reduction(+:total)
for (int i = 0; i < 100; i++) {
    total += data[i];
}
```

---
### Professional Notes: OpenMP Performance

#### 1. Scheduling Strategies
OpenMP provides different ways to distribute loop iterations:
*   `static`: Fixed-size chunks assigned at compile time (Low overhead).
*   `dynamic`: Chunks assigned at runtime as threads become free (Better for unbalanced workloads).
*   `guided`: Chunks start large and shrink over time to reduce tail-latency.

#### 2. False Sharing and Padding
**Godhood Warning**: Avoid "False Sharing," where multiple threads write to different variables that happen to be on the same CPU cache line. This causes the cache line to be repeatedly invalidated across cores, drastically reducing performance.
*   **Fix**: Pad your data structures or ensure threads work on data that is spaced apart in memory.

#### 3. Thread Affinity
Use environment variables like `OMP_PROC_BIND=true` to bind threads to specific physical CPU cores, improving cache hits by preventing threads from migrating between cores.

---
