# Chapter 48: High Performance Optimization

# HIGH-PERFORMANCE OPTIMIZATION

## 1. CPU Caching

Memory is slow. CPU registers are fast. Caches (L1, L2, L3) bridge the gap.

*   **Cache Miss:** CPU waits hundreds of cycles for RAM.
*   **Data-Oriented Design:** Structure of Arrays (SoA) vs Array of Structures (AoS). SoA is often friendlier to cache and SIMD.

## 2. Branch Prediction

CPUs guess which way an `if` will go. If they guess wrong, pipeline flush (expensive).

*   **Sorted Data:** Branch prediction loves patterns (TTTTFFFF).
*   **Branchless Programming:** Using bitwise ops to avoid branches.
```cpp
    // Branchy
    if (x > 0) y = 1; else y = 0;
    // Branchless
    y = (x > 0);
```
*   `[[likely]]` / `[[unlikely]]` (C++20).

## 3. SIMD (Single Instruction, Multiple Data)

Doing math on 4, 8, or 16 numbers at once.

*   **Intrinsics:** `_mm256_add_ps` (AVX). Hard to read.
*   **Auto-vectorization:** Compiler does it if code is simple enough.
*   **Libraries:** `std::experimental::simd` (C++26?), Highway, Vc.

## 4. Link Time Optimization (LTO)

Allows the compiler to inline functions across translation units (object files).

## 5. Profile-Guided Optimization (PGO)

1.  Compile with instrumentation.
2.  Run the program on representative data.
3.  Recompile using the profile data. Optimizes hot paths heavily.

***
### Professional Insights: High-Performance Engineering

#### 1. Small Object Optimization (SOO) / Small String Optimization (SSO)

Many standard library components (like `std::string` or `std::function`) use a small internal buffer to store data without heap allocation if the data is small enough (typically 15-22 bytes for strings).
*   **Benefit**: Avoids the expensive `malloc`/`free` cycle and improves cache locality.
*   **Verification**: Check your compiler's implementation by printing `sizeof(std::string)`.

#### 2. Copy Elision and RVO (Return Value Optimization)

The compiler can often omit copying an object when it's returned from a function, even if move semantics are available.
*   **NRVO**: Named Return Value Optimization.
*   **Mandatory Copy Elision (C++17)**: The standard now requires the compiler to omit copies in many return scenarios, making it safe to return large objects by value.

#### 3. Profiling: Measuring before Optimizing

Never optimize without data.
*   **Sampling Profilers (e.g., `perf`, `VTune`)**: Periodically interrupt the CPU to see which function is running. Low overhead, identifies hot spots.
*   **Instrumentation Profilers (e.g., `gprof`, `Valgrind`)**: Add code to every function call to measure exact timings. High overhead, but provides exact call graphs.
*   **Micro-benchmarking**: Use tools like **Google Benchmark** to measure individual functions in isolation.

#### 4. The "Godhood" Rule: Cache is King

On modern CPUs, a cache miss is the single most expensive operation.
*   **Rule of Thumb**: Prefer `std::vector` over `std::list`. Prefer linear data access patterns. Avoid "pointer chasing" across the heap.

***

## Professional Insights: Optimization in C++

Section 143.1: Introduction to performance
C and C++ are well known as high-performance languages - largely due to the heavy amount of code customization,
allowing a user to specify performance by choice of structure.
When optimizing it is important to benchmark relevant code and completely understand how the code will be used.
Common optimization mistakes include:
Premature optimization: Complex code may perform worse after optimization, wasting time and eﬀort.
First priority should be to write correct and maintainable code, rather than optimized code.
Optimization for the wrong use case: Adding overhead for the 1% might not be worth the slowdown for
the other 99%
Micro-optimization: Compilers do this very eﬃciently and micro-optimization can even hurt the compilers
ability to further optimize the code
Typical optimization goals are:
To do less work
To use more eﬃcient algorithms/structures
To make better use of hardware
Optimized code can have negative side eﬀects, including:
Higher memory usage
Complex code -being diﬃcult to read or maintain
Compromised API and code design
Section 143.2: Empty Base Class Optimization
An object cannot occupy less than 1 byte, as then the members of an array of this type would have the same
address. Thus sizeof(T)>=1 always holds. It's also true that a derived class cannot be smaller than any of its base
classes. However, when the base class is empty, its size is not necessarily added to the derived class:
```cpp
class Base {};
class Derived : public Base
{
public:
    int i;
};
In this case, it's not required to allocate a byte for Base within Derived to have a distinct address per type per
object. If empty base class optimization is performed (and no padding is required), then sizeof(Derived) ==
sizeof(int), that is, no additional allocation is done for the empty base. This is possible with multiple base classes
as well (in C++, multiple bases cannot have the same type, so no issues arise from that).
```

Note that this can only be performed if the ﬁrst member of Derived diﬀers in type from any of the base classes.
This includes any direct or indirect common bases. If it's the same type as one of the bases (or there's a common
base), at least allocating a single byte is required to ensure that no two distinct objects of the same type have the
same address.

Section 143.3: Optimizing by executing less code
The most straightforward approach to optimizing is by executing less code. This approach usually gives a ﬁxed
speed-up without changing the time complexity of the code.
Even though this approach gives you a clear speedup, this will only give noticable improvements when the code is
called a lot.
Removing useless code
void func(const A *a); // Some random function
// useless memory allocation + deallocation for the instance
```cpp
auto a1 = std::make_unique<A>();
func(a1.get());
// making use of a stack object prevents
auto a2 = A{};
func(&a2);
Version ≥ C++14
From C++14, compilers are allowed to optimize this code to remove the allocation and matching deallocation.
Doing code only once
std::map<std::string, std::unique_ptr<A>> lookup;
// Slow insertion/lookup
// Within this function, we will traverse twice through the map lookup an element
// and even a thirth time when it wasn't in
const A *lazyLookupSlow(const std::string &key) {
    if (lookup.find(key) != lookup.cend())
        lookup.emplace_back(key, std::make_unique<A>());
    return lookup[key].get();
}
// Within this function, we will have the same noticeable effect as the slow variant while going at
double speed as we only traverse once through the code
const A *lazyLookupSlow(const std::string &key) {
    auto &value = lookup[key];
    if (!value)
        value = std::make_unique<A>();
    return value.get();
}
A similar approach to this optimization can be used to implement a stable version of unique
std::vector<std::string> stableUnique(const std::vector<std::string> &v) {
    std::vector<std::string> result;
    std::set<std::string> checkUnique;
    for (const auto &s : v) {
        // As insert returns if the insertion was successful, we can deduce if the element was
already in or not
        // This prevents an insertion, which will traverse through the map for every unique element
        // As a result we can almost gain 50% if v would not contain any duplicates
        if (checkUnique.insert(s).second)
            result.push_back(s);
    }
    return result;
}

Preventing useless reallocating and copying/moving
In the previous example, we already prevented lookups in the std::set, however the std::vector still contains a
growing algorithm, in which it will have to realloc its storage. This can be prevented by ﬁrst reserving for the right
size.
std::vector<std::string> stableUnique(const std::vector<std::string> &v) {
    std::vector<std::string> result;
    // By reserving 'result', we can ensure that no copying or moving will be done in the vector
    // as it will have capacity for the maximum number of elements we will be inserting
    // If we make the assumption that no allocation occurs for size zero
    // and allocating a large block of memory takes the same time as a small block of memory
    // this will never slow down the program
    // Side note: Compilers can even predict this and remove the checks the growing from the
generated code
    result.reserve(v.size());
    std::set<std::string> checkUnique;
    for (const auto &s : v) {
        // See example above
        if (checkUnique.insert(s).second)
            result.push_back(s);
    }
    return result;
}
Section 143.4: Using ecient containers
```

Optimizing by using the right data structures at the right time can change the time-complexity of the code.
// This variant of stableUnique contains a complexity of N log(N)
// N > number of elements in v
// log(N) > insert complexity of std::set
```cpp
std::vector<std::string> stableUnique(const std::vector<std::string> &v) {
    std::vector<std::string> result;
    std::set<std::string> checkUnique;
    for (const auto &s : v) {
        // See Optimizing by executing less code
        if (checkUnique.insert(s).second)
            result.push_back(s);
    }
    return result;
}
By using a container which uses a diﬀerent implementation for storing its elements (hash container instead of tree),
we can transform our implementation to complexity N. As a side eﬀect, we will call the comparison operator for
std::string less, as it only has to be called when the inserted string should end up in the same bucket.
// This variant of stableUnique contains a complexity of N
// N > number of elements in v
// 1 > insert complexity of std::unordered_set
std::vector<std::string> stableUnique(const std::vector<std::string> &v) {
    std::vector<std::string> result;
    std::unordered_set<std::string> checkUnique;
    for (const auto &s : v) {
        // See Optimizing by executing less code
        if (checkUnique.insert(s).second)
            result.push_back(s);
    }
    return result;

}
Section 143.5: Small Object Optimization
Small object optimization is a technique which is used within low level data structures, for instance the std::string
(Sometimes referred to as Short/Small String Optimization). It's meant to use stack space as a buﬀer instead of
some allocated memory in case the content is small enough to ﬁt within the reserved space.
By adding extra memory overhead and extra calculations, it tries to prevent an expensive heap allocation. The
beneﬁts of this technique are dependent on the usage and can even hurt performance if incorrectly used.
Example
A very naive way of implementing a string with this optimization would the following:
#include <cstring>

class string final
{
    constexpr static auto SMALL_BUFFER_SIZE = 16;
    bool _isAllocated{false};                       ///< Remember if we allocated memory
    char *_buffer{nullptr};                         ///< Pointer to the buffer we are using
    char _smallBuffer[SMALL_BUFFER_SIZE]= {'\\0'};   ///< Stack space used for SMALL OBJECT
OPTIMIZATION
public:
    ~string()
    {
        if (_isAllocated)
            delete [] _buffer;
    }
    explicit string(const char *cStyleString)
    {
        auto stringSize = std::strlen(cStyleString);
        _isAllocated = (stringSize > SMALL_BUFFER_SIZE);
        if (_isAllocated)
            _buffer = new char[stringSize];
        else
            _buffer = &_smallBuffer[0];
        std::strcpy(_buffer, &cStyleString[0]);
    }
    string(string &&rhs)
       : _isAllocated(rhs._isAllocated)
       , _buffer(rhs._buffer)
       , _smallBuffer(rhs._smallBuffer) //< Not needed if allocated
    {
        if (_isAllocated)
        {
           // Prevent double deletion of the memory
           rhs._buffer = nullptr;
        }
        else
        {
            // Copy over data
            std::strcpy(_smallBuffer, rhs._smallBuffer);
            _buffer = &_smallBuffer[0];
        }

    }
    // Other methods, including other constructors, copy constructor,
    // assignment operators have been omitted for readability
};
As you can see in the code above, some extra complexity has been added in order to prevent some new and delete
operations. On top of this, the class has a larger memory footprint which might not be used except in a couple of
cases.
Often it is tried to encode the bool value _isAllocated, within the pointer _buffer with bit manipulation to reduce
the size of a single instance (intel 64 bit: Could reduce size by 8 byte). An optimization which is only possible when
its known what the alignment rules of the platform is.
When to use?
As this optimization adds a lot of complexity, it is not recommended to use this optimization on every single class. It
will often be encountered in commonly used, low-level data structures. In common C++11 standard library
implementations one can ﬁnd usages in std::basic_string<> and std::function<>.
As this optimization only prevents memory allocations when the stored data is smaller than the buﬀer, it will only
give beneﬁts if the class is often used with small data.
A ﬁnal drawback of this optimization is that extra eﬀort is required when moving the buﬀer, making the move-
operation more expensive than when the buﬀer would not be used. This is especially true when the buﬀer contains
a non-POD type.
```




##### Optimization

When compiling, the compiler will often modify the program to increase performance. This is permitted by the as-if
rule, which allows any and all transformations that do not change observable behavior.
Section 144.1: Inline Expansion/Inlining
Inline expansion (also known as inlining) is compiler optimisation that replaces a call to a function with the body of
that function. This saves the function call overhead, but at the cost of space, since the function may be duplicated
several times.
// source:
```cpp
int process(int value)
{
    return 2 * value;
}
int foo(int a)
{
    return process(a);
}
// program, after inlining:
int foo(int a)
{
    return 2 * a; // the body of process() is copied into foo()
}
Inlining is most commonly done for small functions, where the function call overhead is signiﬁcant compared to the
size of the function body.
Section 144.2: Empty base optimization
The size of any object or member subobject is required to be at least 1 even if the type is an empty class type (that
is, a class or struct that has no non-static data members), in order to be able to guarantee that the addresses of
distinct objects of the same type are always distinct.
However, base class subobjects are not so constrained, and can be completely optimized out from the object
layout:
#include <cassert>
struct Base {}; // empty class
struct Derived1 : Base {
    int i;
};
int main() {
    // the size of any object of empty class type is at least 1
    assert(sizeof(Base) == 1);
    // empty base optimization applies
    assert(sizeof(Derived1) == sizeof(int));
}
Empty base optimization is commonly used by allocator-aware standard library classes (std::vector,
std::function, std::shared_ptr, etc) to avoid occupying any additional storage for its allocator member if the
allocator is stateless. This is achieved by storing one of the required data members (e.g., begin, end, or capacity
pointer for the vector).
Reference: cppreference
```




##### Proﬁling

Section 145.1: Proﬁling with gcc and gprof
The GNU gprof proﬁler, gprof, allows you to proﬁle your code. To use it, you need to perform the following steps:
1.
Build the application with settings for generating proﬁling information
2.
Generate proﬁling information by running the built application
3.
View the generated proﬁling information with gprof
In order to build the application with settings for generating proﬁling information, we add the -pg ﬂag. So, for
example, we could use
$ gcc -pg *.cpp -o app
or
$ gcc -O2 -pg *.cpp -o app
and so forth.
Once the application, say app, is built, execute it as usual:
$ ./app
This should produce a ﬁle called gmon.out.
To see the proﬁling results, now run
$ gprof app gmon.out
(note that we provide both the application as well as the generated output).
Of course, you can also pipe or redirect:
$ gprof app gmon.out | less
and so forth.
The result of the last command should be a table, whose rows are the functions, and whose columns indicate the
number of calls, total time spent, self time spent (that is, time spent in the function excluding calls to children).
Section 145.2: Generating callgraph diagrams with gperf2dot
For more complex applications, ﬂat execution proﬁles may be diﬃcult to follow. This is why many proﬁling tools
also generate some form of annotated callgraph information.
gperf2dot converts text output from many proﬁlers (Linux perf, callgrind, oproﬁle etc.) into a callgraph diagram.
You can use it by running your proﬁler (example for gprof):



## translate profiling data to text, create image

gprof ./main | gprof2dot -s | dot -Tpng -o output.png
Section 145.3: Proﬁling CPU Usage with gcc and Google Perf
Tools
Google Perf Tools also provides a CPU proﬁler, with a slightly friendlier interface. To use it:
1.
2.
3.
4.
Install Google Perf Tools
Compile your code as usual
Add the libprofiler proﬁler library to your library load path at runtime
Use pprof to generate a ﬂat execution proﬁle, or a callgraph diagram
For example:



## compile code

g++ -O3 -std=c++11 main.cpp -o main



## run with profiler

LD_PRELOAD=/usr/local/lib/libprofiler.so CPUPROFILE=main.prof CPUPROFILE_FREQUENCY=100000 ./main
where:
CPUPROFILE indicates the output ﬁle for proﬁling data
CPUPROFILE_FREQUENCY indicates the proﬁler sampling frequency;
Use pprof to post-process the proﬁling data.
You can generate a ﬂat call proﬁle as text:
$ pprof --text ./main main.prof
PROFILE: interrupts/evictions/bytes = 67/15/2016
pprof --text --lines ./main main.prof
Using local file ./main.
Using local file main.prof.
Total: 67 samples
22  32.8%  32.8%       67 100.0% longRunningFoo ??:0
20  29.9%  62.7%       20  29.9% __memmove_ssse3_back
/build/eglibc-3GlaMS/eglibc-2.19/string/../sysdeps/x86_64/multiarch/memcpy-ssse3-back.S:1627
4   6.0%  68.7%        4   6.0% __memmove_ssse3_back
/build/eglibc-3GlaMS/eglibc-2.19/string/../sysdeps/x86_64/multiarch/memcpy-ssse3-back.S:1619
3   4.5%  73.1%        3   4.5% __random_r /build/eglibc-3GlaMS/eglibc-2.19/stdlib/random_r.c:388
3   4.5%  77.6%        3   4.5% __random_r /build/eglibc-3GlaMS/eglibc-2.19/stdlib/random_r.c:401
2   3.0%  80.6%        2   3.0% __munmap
/build/eglibc-3GlaMS/eglibc-2.19/misc/../sysdeps/unix/syscall-template.S:81
2   3.0%  83.6%       12  17.9% __random /build/eglibc-3GlaMS/eglibc-2.19/stdlib/random.c:298
2   3.0%  86.6%        2   3.0% __random_r /build/eglibc-3GlaMS/eglibc-2.19/stdlib/random_r.c:385
2   3.0%  89.6%        2   3.0% rand /build/eglibc-3GlaMS/eglibc-2.19/stdlib/rand.c:26
1   1.5%  91.0%        1   1.5% __memmove_ssse3_back
/build/eglibc-3GlaMS/eglibc-2.19/string/../sysdeps/x86_64/multiarch/memcpy-ssse3-back.S:1617
1   1.5%  92.5%        1   1.5% __memmove_ssse3_back
/build/eglibc-3GlaMS/eglibc-2.19/string/../sysdeps/x86_64/multiarch/memcpy-ssse3-back.S:1623
1   1.5%  94.0%        1   1.5% __random /build/eglibc-3GlaMS/eglibc-2.19/stdlib/random.c:293
1   1.5%  95.5%        1   1.5% __random /build/eglibc-3GlaMS/eglibc-2.19/stdlib/random.c:296
1   1.5%  97.0%        1   1.5% __random_r /build/eglibc-3GlaMS/eglibc-2.19/stdlib/random_r.c:371
1   1.5%  98.5%        1   1.5% __random_r /build/eglibc-3GlaMS/eglibc-2.19/stdlib/random_r.c:381
1   1.5% 100.0%        1   1.5% rand /build/eglibc-3GlaMS/eglibc-2.19/stdlib/rand.c:28
0   0.0% 100.0%       67 100.0% __libc_start_main /build/eglibc-3GlaMS/eglibc-2.19/csu/libc-
start.c:287
0   0.0% 100.0%       67 100.0% _start ??:0
0   0.0% 100.0%       67 100.0% main ??:0
0   0.0% 100.0%       14  20.9% rand /build/eglibc-3GlaMS/eglibc-2.19/stdlib/rand.c:27
0   0.0% 100.0%       27  40.3% std::vector::_M_emplace_back_aux ??:0
... or you can generate an annotated callgraph in a pdf with:
pprof --pdf ./main main.prof > out.pdf


For HFT, Game Engines, and Real-Time Systems, every nanosecond counts.



#### 17.1 CPU Pipelines & Branch Prediction

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



#### 17.2 Data-Oriented Design (DoD)

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



#### 17.3 Prefetching

Use `__builtin_prefetch` (GCC/Clang) or `_mm_prefetch` (Intel) to load data into L1 cache before it's needed.

```cpp
for (int i = 0; i < N; ++i) {
    __builtin_prefetch(&data[i + 16]); // Lookahead
    process(data[i]);
}
```



#### 17.4 Micro-Benchmarking (Google Benchmark)

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



#### 17.5 System Warm-up

The first few thousand iterations of code are slow due to:
1.  **Instruction Cache Misses**: Code not yet in CPU cache.
2.  **Data Cache Misses**: Data not yet in L1/L2.
3.  **Branch Predictor**: Hasn't learned the patterns yet.
4.  **OS Page Faults**: Memory pages not yet committed.

**Strategy**: Run a "dummy" loop of your critical path 10,000 times before enabling the network listener or trading signal.



#### 17.6 False Sharing Prevention

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

***



#### 31.3 Compiler Optimizations (The "Free Lunch")

*   `-O3`: Aggressive optimization.
*   `-march=native`: Use instructions available on the build machine (AVX2, AVX-512).
*   `-flto` (Link Time Optimization): Optimize across translation units (inlining across .cpp files).
*   **PGO (Profile Guided Optimization)**:
    1.  Compile with `-fprofile-generate`.
    2.  Run the app (training run).
    3.  Recompile with `-fprofile-use`.



#### 14.2 Small String Optimization (SSO)

`std::string` doesn't always allocate heap memory.

```cpp
std::string s = "Hello"; // 5 chars
// Layout typically (24-32 bytes):
// [ size (8) ] [ capacity (8) ] [ pointer (8) ]  <-- Normal mode
// [ size (1) ] [ ... chars 22 bytes ...     ]  <-- SSO mode (Union)
```
Strings shorter than 15-22 chars (depending on libc++) live entirely on the stack.



#### 14.3 Return Value Optimization (RVO)

Copy elision is mandatory in C++17.

```cpp
struct BigObject { int data[1000]; };

BigObject create() {
    BigObject obj;
    // ... fill obj ...
    return obj; // No copy, no move. Constructed directly in caller's stack frame.
}

BigObject x = create();
```

***

