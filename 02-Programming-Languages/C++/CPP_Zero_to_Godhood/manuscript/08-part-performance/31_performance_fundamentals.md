# Part VIII: Performance, Memory, and Optimization

*Making C++ blazingly fast.*

# Chapter 31: Performance Fundamentals

> *Understanding why code is fast or slow.*

C and C++ are famous for being "close to the metal." But what does that actually mean? To write truly high-performance C++, you can no longer think purely in terms of Big-O notation ($O(N)$, $O(\log N)$). You must start thinking about exactly how the CPU executes your code and how RAM feeds data to the CPU.

In modern hardware, an $O(N)$ algorithm can easily outperform an $O(1)$ algorithm if the $O(N)$ algorithm respects the CPU's architecture and the $O(1)$ algorithm does not.

---

## 31.1 The Golden Rule: Memory is Slow, Cache is King

Your CPU operates at roughly 4 GHz (4 billion cycles per second).
*   Executing an arithmetic instruction takes **1 cycle**.
*   Reading data from the L1 Cache takes **~4 cycles**.
*   Reading data from the L2 Cache takes **~12 cycles**.
*   Reading data from the L3 Cache takes **~40 cycles**.
*   Reading data from Main Memory (RAM) takes **~300 cycles**.

If your CPU asks for data and it isn't in the cache, it suffers a **Cache Miss**. The CPU sits completely idle for 300 cycles waiting for RAM to deliver the data. If you have a loop of 1,000 items and every item causes a cache miss, your code is thousands of times slower than it should be.

### 31.2 Spatial and Temporal Locality
How do you prevent cache misses? 
When the CPU requests a byte from RAM, RAM doesn't send just one byte. It sends a **Cache Line** (usually 64 bytes). 

*   **Spatial Locality**: If you process data sequentially (like iterating through a `std::vector`), the CPU pulls the first item, waits 300 cycles, and gets 64 bytes of items. The next 15 iterations will be instant (Cache Hits) because the data is already in the L1 cache.
*   **Temporal Locality**: If you access the same variable repeatedly, keep it in a small, local scope so it stays in the L1 cache or in a CPU register.

This is why **`std::vector` is almost always faster than `std::list`**. A `std::list` allocates nodes randomly across the heap. Iterating through it causes a cache miss on almost every node. A `std::vector` is contiguous memory.

## 31.3 Data-Oriented Design (DoD)

Object-Oriented Programming (OOP) teaches us to group data into logical structures. 

```cpp
// Array of Structures (AoS) - The OOP Way
struct Particle {
    float x, y, z;      // 12 bytes
    float velocity;     // 4 bytes
    int color;          // 4 bytes
    bool is_active;     // 1 byte (+ 3 bytes padding)
}; // Total: 24 bytes

std::vector<Particle> particles(1000);
```

If we write a loop to update the positions based on velocity, we only need `x, y, z` and `velocity`. But because the data is packed as a `Particle`, we pull `color` and `is_active` into the CPU cache as well. We are wasting 20% of our precious cache bandwidth on data we aren't using!

**Data-Oriented Design** teaches us to optimize for the cache by restructuring our data into a Structure of Arrays (SoA).

```cpp
// Structure of Arrays (SoA) - The DoD Way
struct ParticleSystem {
    std::vector<float> x, y, z;
    std::vector<float> velocity;
    std::vector<int> color;
    std::vector<bool> is_active;
};
```

Now, when we loop through `x, y, z, velocity`, our cache lines are packed with 100% useful data. This single change can double or triple the speed of a simulation.

## 31.4 Branch Prediction

Modern CPUs use a deep "pipeline." While the CPU is executing instruction 1, it is already decoding instruction 2 and fetching instruction 3.

But what happens when it hits an `if` statement?
```cpp
if (data[i] > 100) {
    do_a();
} else {
    do_b();
}
```
The CPU doesn't know which path to fetch. So, it uses a **Branch Predictor** to guess. If it guesses right, execution continues flawlessly. If it guesses wrong, it suffers a **Branch Misprediction Penalty**. It has to throw away all the work in its pipeline and fetch the correct instructions, wasting ~15-20 cycles.

If `data` is sorted, the branch predictor guesses correctly 99% of the time. If `data` is randomized, the branch predictor guesses wrong 50% of the time, devastating performance.

### Branchless Programming
To avoid the penalty, you can rewrite code using bitwise math to remove the branch entirely.

```cpp
// Branchy:
if (x > 0) y = 1; else y = 0;

// Branchless:
y = (x > 0); 
```
C++20 also introduced `[[likely]]` and `[[unlikely]]` attributes to give the compiler hints on how to lay out the assembly.

## 31.5 SIMD (Single Instruction, Multiple Data)

Modern CPUs have wide vector registers (e.g., 256-bit AVX registers). Instead of adding two floats together, the CPU can add eight pairs of floats together in a single clock cycle.

If your code is simple, continuous, and has no branches, the compiler's **Auto-Vectorizer** will automatically upgrade your loops to use SIMD instructions. (This is another reason why SoA architecture is so fast).

If the compiler fails, you can manually write SIMD code using compiler intrinsics (`_mm256_add_ps`), but this code is highly unreadable and tied to a specific CPU architecture.

## 31.6 How to Actually Optimize

Never guess where your code is slow. You will be wrong 90% of the time. 

1.  **Measure**: Use a profiling tool like **Linux `perf`**, **Intel VTune**, or **Valgrind/Callgrind** to see exactly which functions take the most CPU time.
2.  **Microbenchmark**: Use a framework like **Google Benchmark** to test specific functions in isolation.
3.  **Read the Assembly**: Use **Compiler Explorer (godbolt.org)**. Paste your C++ code and look at the assembly the compiler generates. Did it auto-vectorize? Did it optimize away the copies?

> [!TIP]
> **Godhood Tip: Small Object Optimization (SOO)**
> Many standard library components, like `std::string` and `std::function`, use SOO. They have a small internal buffer (usually ~15 bytes for a string). If your string fits in that buffer, it is stored directly on the stack. If it exceeds that buffer, it allocates on the heap. Keeping your strings short prevents expensive `malloc` calls and cache misses.

We've talked about how expensive RAM is. In the next chapter, we will take absolute control over how memory is allocated and freed.
