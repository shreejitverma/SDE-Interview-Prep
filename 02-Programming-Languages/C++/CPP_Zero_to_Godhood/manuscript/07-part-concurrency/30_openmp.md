# Chapter 30: OpenMP and Parallel Computing

> *Parallelizing existing code with compiler directives.*

Writing multithreaded code with `std::thread`, mutexes, and atomics is complex. Sometimes, you don't need a complex architecture with thread pools and task queues. Sometimes, you just have a massive `for` loop that crunches numbers, and you want it to run on all 16 cores of your CPU instead of just 1.

This is where **OpenMP** (Open Multi-Processing) comes in. It is not a standard C++ library; it is a cross-language API (supported by GCC, Clang, and MSVC) that allows you to parallelize code using simple `#pragma` compiler directives.

---

## 30.1 What Is OpenMP?

OpenMP uses the **Fork-Join Model**. The program starts as a single "master" thread. When it hits a parallel region, it *forks* into a team of threads. They divide the work, and when they finish, they *join* back together, and the master thread continues alone.

If you compile your code without enabling OpenMP (e.g., omitting the `-fopenmp` flag in GCC), the compiler simply ignores the `#pragma` statements, and your code runs synchronously on a single thread. This makes OpenMP incredibly safe to retrofit into existing codebases.

## 30.2 Parallel Regions

The most basic directive is `#pragma omp parallel`. It creates a team of threads (usually matching your CPU core count) and has every thread execute the following block of code.

```cpp
#include <iostream>
#include <omp.h>

int main() {
    std::cout << "Starting program...\n";

    // Fork!
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        // If you have 8 cores, this prints 8 times simultaneously.
        std::cout << "Hello from thread " << thread_id << '\n'; 
    }
    // Join!
    
    std::cout << "Back to a single thread.\n";
}
```

## 30.3 Parallelizing Loops

The true power of OpenMP is loop parallelization. If you have a loop where the iterations are completely independent of each other, you can parallelize it with one line of code: `#pragma omp parallel for`.

```cpp
void process_images(std::vector<Image>& images) {
    
    #pragma omp parallel for
    for (int i = 0; i < images.size(); i++) {
        // OpenMP automatically divides the loop.
        // Thread 0 takes images 0-24
        // Thread 1 takes images 25-49, etc.
        apply_filter(images[i]); 
    }
}
```
*Note: The loop counter `i` must be an integer, and the bounds must be computable before the loop starts.*

## 30.4 Data Sharing Attributes

When threads enter a parallel region, what happens to the variables declared outside the region? OpenMP provides explicit controls:

*   **`shared`**: There is only one copy of the variable. All threads read/write to the exact same memory address (Requires synchronization if writing!).
*   **`private`**: Every thread gets its own uninitialized local copy of the variable.
*   **`firstprivate`**: Every thread gets its own local copy, initialized with the value from the master thread.

```cpp
int global_config = 42;
int temp_var = 0;

#pragma omp parallel for shared(global_config) private(temp_var)
for (int i = 0; i < 100; i++) {
    temp_var = global_config * i;
    // ...
}
```

## 30.5 The `reduction` Clause

What if you want to calculate the sum of an array?
```cpp
long total = 0;
#pragma omp parallel for
for (int i = 0; i < 1000; i++) {
    total += array[i]; // DATA RACE!
}
```
Because `total` is shared, multiple threads adding to it simultaneously causes a data race. You could use an `#pragma omp critical` block (which acts like a mutex), but that kills performance.

Instead, use **`reduction`**:
```cpp
long total = 0;
// Each thread gets a private 'total'. 
// At the end, OpenMP adds (+) all the private totals into the main 'total'.
#pragma omp parallel for reduction(+:total)
for (int i = 0; i < 1000; i++) {
    total += array[i]; 
}
```

## 30.6 Scheduling Strategies

By default, OpenMP divides a loop into equal, static chunks. If you have 100 iterations and 4 threads, each gets 25 iterations. 
But what if iteration #2 takes 1 second, and iteration #3 takes 10 minutes? The thread handling iteration #3 will still be working long after the others have finished, leaving 3 cores idle.

You can fix this by changing the schedule:

1.  **`schedule(static)`**: The default. Best when every iteration takes the exact same amount of time. Lowest overhead.
2.  **`schedule(dynamic, chunk_size)`**: Threads grab a chunk of work. When they finish, they come back and ask for another chunk. Best for unbalanced workloads.
3.  **`schedule(guided)`**: Starts with large chunks (for efficiency) and dynamically shrinks the chunk size down as it nears the end of the loop, balancing overhead with load distribution.

```cpp
#pragma omp parallel for schedule(dynamic, 10)
for (int i = 0; i < 1000; i++) {
    process_variable_time_task(i);
}
```

## 30.7 Vectorization (`#pragma omp simd`)

Modern CPUs have SIMD instructions (Single Instruction, Multiple Data) like AVX or NEON. These allow the CPU to perform the same math operation on 4 or 8 numbers in a single clock cycle.

OpenMP can force the compiler to vectorize a loop:

```cpp
#pragma omp simd
for (int i = 0; i < N; i++) {
    a[i] = b[i] * c[i];
}
```
If you combine threading and SIMD (`#pragma omp parallel for simd`), you are utilizing the absolute maximum computational throughput your CPU can physically provide.

---

> [!TIP]
> **Godhood Tip: False Sharing**
> If two threads are writing to two different `private` variables, but those variables happen to sit next to each other in memory (sharing a 64-byte CPU Cache Line), the CPU will constantly invalidate and reload the cache line across cores. This is called **False Sharing**, and it can make multithreaded code *slower* than single-threaded code. Always ensure heavily modified thread-local data is padded to 64 bytes to prevent cache line collisions.

Concurrency and Parallelism solve one side of the performance equation: using more cores. But the most profound performance gains in C++ come from writing code that runs faster on a *single* core. 

To achieve that, we must understand the hardware itself. We must move into **Part VIII: Performance and Optimization**.
