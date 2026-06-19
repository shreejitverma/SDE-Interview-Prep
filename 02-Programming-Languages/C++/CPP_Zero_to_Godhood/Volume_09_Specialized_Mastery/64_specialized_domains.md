# Chapter 64: Specialized Domains

# SPECIALIZED DOMAINS

### 37.1 Game Development (Data-Oriented Design)

OOP is cache-poison. Games use **Entity-Component-Systems (ECS)**.
*   **AoS (Array of Structs):** `[Pos, Vel], [Pos, Vel]` -> Bad stride.
*   **SoA (Structure of Arrays):** `[Pos, Pos], [Vel, Vel]` -> SIMD friendly.

### 37.2 Embedded Systems

*   **Memory-Mapped I/O:** Casting integer addresses to pointers.
*   **`volatile`**: Tells compiler "Hardware changes this value, do not optimize reads".
*   **Freestanding Implementation:** No OS, no heap (`malloc` is a myth).

### 37.3 High-Frequency Trading (HFT)

*   **Kernel Bypass:** `Solarflare OpenOnload` maps NIC ring buffers directly to userspace, skipping the Linux Kernel (save ~2-3 microseconds).
*   **Warm-up:** Pinging order gateways to ensure the TCP congestion window is open and CPU is in C0 state (max turbo).

### 37.4 Automotive (MISRA C++ / AUTOSAR)

*   **No Dynamic Memory:** All containers have fixed capacity (`etl::vector` or `boost::static_vector`).
*   **No Exceptions:** `try-catch` adds binary size and non-deterministic unwind paths. Use `std::expected` or error codes.
*   **Static Analysis:** Code must pass MISRA-2008 rules (e.g., "Rule 5-0-15: Array indexing shall be checked").

```cpp
// Stack-only pattern (Automotive safe)
template <typename T, size_t N>
class FixedVector {
    T data[N];
    size_t count = 0;
public:
    void push_back(const T& val) {
        if (count < N) data[count++] = val;
    }
};
```

***

## FINAL COMPREHENSIVE CHECKLIST

### C++98/03 Foundation

-  Variables and basic types
-  Operators and control flow
-  Functions and overloading
-  Arrays and pointers
-  Classes and objects
-  Constructors and destructors
-  Inheritance
-  Virtual functions and polymorphism
-  STL containers (vector, list, map, set)
-  Algorithms
-  Strings and I/O

### C++11 Major Features

-  Auto type deduction
-  nullptr and nullptr_t
-  Uniform initialization {}
-  Range-based for loops
-  Smart pointers (unique_ptr, shared_ptr)
-  Move semantics
-  Rvalue references
-  Lambda functions
-  Variadic templates
-  std::array and std::tuple
-  std::unordered_map and std::unordered_set

### C++14 Improvements

-  Generic lambdas
-  Return type deduction
-  std::make_unique
-  Digit separators (1'000'000)
-  decltype(auto)

### C++17 Modern Features

-  Structured bindings
-  std::optional
-  std::variant
-  std::any
-  if constexpr
-  Fold expressions
-  Filesystem library
-  Parallel algorithms
-  std::string_view

### C++20 Revolutionary

-  Concepts and constraints
-  Ranges
-  Coroutines
-  Modules
-  Spaceship operator <=>
-  Designated initializers
-  Requires expressions

### C++23 Latest

-  Deducing this
-  std::expected
-  Literal classes in constexpr

### Advanced Concepts

-  Template metaprogramming
-  CRTP (Curiously Recurring Template Pattern)
-  SFINAE (Substitution Failure Is Not An Error)
-  Type traits
-  Memory management (stack vs heap)
-  Smart pointers (unique_ptr, shared_ptr, weak_ptr)
-  Move semantics and forwarding
-  Perfect forwarding with std::forward

### Concurrency

-  Threading basics
-  Mutexes and locks
-  Condition variables
-  Atomic operations
-  Memory ordering
-  Lock-free programming

### Performance & Optimization

-  Memory profiling
-  Cache optimization
-  SIMD and vectorization
-  Compiler flags (-O2, -O3)
-  Profiling tools (perf, valgrind)

### STL Mastery

-  All container types
-  All algorithms
-  Iterators
-  Custom comparators
-  Ranges (C++20)

### Design Patterns

-  Singleton
-  Factory
-  Observer
-  Strategy
-  CRTP

### Professional Development

-  CMake build system
-  Testing frameworks (Google Test)
-  Debugging (gdb)
-  Profiling
-  Code organization
-  RAII pattern
-  Error handling
-  Memory leak detection

***

## Key Insights for C++ Mastery

1. **RAII** = Guaranteed resource cleanup
2. **Smart Pointers** = No manual memory management
3. **Move Semantics** = Zero-copy optimization
4. **Const Correctness** = Prevents bugs, enables optimizations
5. **Templates** = Compile-time computation
6. **Concepts** (C++20) = Type-safe constraints
7. **Ranges** (C++20) = Composable algorithms
8. **Coroutines** (C++20) = Async programming made easy
9. **Atomic Operations** = Safe concurrent access
10. **Performance** = Measure, profile, then optimize

***

## Learning Path

1. **Week 1-2**: C++98 basics (variables, control flow, functions)
2. **Week 3-4**: Classes and OOP (constructors, inheritance, polymorphism)
3. **Week 5**: STL containers and algorithms
4. **Week 6-7**: C++11 (smart pointers, lambdas, move semantics)
5. **Week 8**: C++14 and C++17 features
6. **Week 9**: C++20 features (concepts, ranges)
7. **Week 10+**: Advanced topics (metaprogramming, concurrency, optimization)

***

## Resources

### Official Documentation

- cppreference.com - C++ standard library reference
- en.cppreference.com - Excellent resource
- isocpp.org - C++ standards committee

### Books

- "A Tour of C++" by Bjarne Stroustrup
- "Effective Modern C++" by Scott Meyers
- "C++ Concurrency in Action" by Anthony Williams

### Practice

- LeetCode.com
- HackerRank.com
- Codeforces.com
- ProjectEuler.net

***

**You are now equipped to master C++ from absolute zero to expert level!**

*Last Updated: December 2025*
*C++ Versions Covered: C++98 through C++23*

