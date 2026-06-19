# Chapter 18: Concurrency with OpenMP

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

***

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

***
### Professional Insights: OpenMP Performance

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

# VOLUME 02: GODHOOD SUMMARY


### C++11 LANDMARK LIBRARY FEATURES


| # | Feature | Explanation | Code Example |
| :--- | :--- | :--- | :--- |
| 41 | **std::unique_ptr** | Sole-ownership smart pointer with RAII; replaces raw new/delete | `auto p = std::unique_ptr<int>(new int(5));` |
| 42 | **std::shared_ptr** | Reference-counted shared ownership smart pointer | `auto p = std::make_shared<int>(10);` |
| 43 | **std::weak_ptr** | Non-owning observer of a shared_ptr; breaks reference cycles | `std::weak_ptr<int> w = p;` |
| 44 | **std::make_shared** | Creates a shared_ptr with a single combined allocation | `auto p = std::make_shared<MyClass>(args);` |
| 45 | **std::move** | Casts an object to an rvalue so move construction is selected | `std::string b = std::move(a);` |
| 46 | **std::forward** | Preserves lvalue/rvalue-ness in forwarding-reference code | `template<class T> void wrap(T&& x){ use(std::forward<T>(x)); }` |
| 47 | **std::thread** | Standard portable threads | `std::thread t([]{ work(); }); t.join();` |
| 48 | **std::mutex** | Basic mutual exclusion primitive | `std::mutex m; std::lock_guard<std::mutex> lk(m);` |
| 49 | **std::recursive_mutex** | Mutex that can be locked multiple times by same thread | `std::recursive_mutex m; m.lock(); m.lock();` |
| 50 | **std::timed_mutex** | Mutex with try_lock_for and try_lock_until | `m.try_lock_for(std::chrono::milliseconds(10));` |
| 51 | **std::lock_guard** | RAII wrapper that locks on construction, unlocks on destruction | `std::lock_guard<std::mutex> lk(m);` |
| 52 | **std::unique_lock** | Flexible mutex ownership supporting deferred/timed locking | `std::unique_lock<std::mutex> lk(m, std::defer_lock);` |
| 53 | **std::condition_variable**| Allows threads to wait until notified by another thread | `cv.wait(lock, []{ return ready; });` |
| 54 | **std::atomic<T>** | Atomic types for lock-free access to shared variables | `std::atomic<int> cnt{0}; cnt.fetch_add(1);` |
| 55 | **std::future / promise** | Communicate results asynchronously between threads | `std::promise<int> p; auto f = p.get_future();` |
| 56 | **std::async** | Launches a callable asynchronously and returns a future | `auto f = std::async([]{ return compute(); });` |
| 57 | **std::packaged_task** | Wraps a callable so its result can be retrieved via a future | `std::packaged_task<int()> task(compute);` |
| 58 | **std::chrono** | Strongly typed clocks, durations, and time points | `auto t0 = std::chrono::steady_clock::now();` |
| 59 | **std::tuple** | Heterogeneous fixed-size collection of values | `auto t = std::make_tuple(1, 2.5, "hi");` |
| 60 | **std::tie** | Unpacks a tuple into named variables | `int a; double b; std::tie(a, b) = my_tuple;` |
| 61 | **std::array** | Fixed-size STL-style array with zero overhead | `std::array<int,3> a{{1,2,3}};` |
| 62 | **std::forward_list** | Singly linked list optimized for minimal memory use | `std::forward_list<int> xs = {1,2,3};` |
| 63 | **std::unordered_map** | Hash-table based map with average O(1) lookup | `std::unordered_map<string,int> mp;` |
| 64 | **std::unordered_set** | Hash-table based set | `std::unordered_set<int> s{1,2,3};` |
| 65 | **Type traits** | Compile-time type property queries for metaprogramming | `static_assert(std::is_integral<int>::value);` |
| 66 | **std::regex** | Standard regular expression library | `std::regex r("\\d+");` |
| 67 | **std::function** | Type-erased wrapper for any callable | `std::function<int(int)> f;` |
| 68 | **std::bind** | Binds arguments to a callable | `auto f = std::bind(std::plus<int>{}, _1, 10);` |
| 69 | **std::begin / end** | Generic free functions for arrays and containers | `auto it = std::begin(arr);` |
| 70 | **std::to_string** | Converts numeric types to std::string | `std::string s = std::to_string(123);` |
| 71 | **std::stoi / stof** | String to numeric type conversions | `int n = std::stoi("42");` |
| 72 | **std::initializer_list** | Sequence of elements for {} initialization | `void f(std::initializer_list<int> il);` |
| 73 | **std::exception_ptr** | Stores and transfers exception objects between threads | `auto ep = std::current_exception();` |
| 74 | **std::random** | Professional engines and distributions | `std::mt19937 rng(42);` |
| 75 | **std::ratio** | Compile-time rational arithmetic | `using half = std::ratio<1, 2>;` |
| 76 | **std::enable_if** | SFINAE helper for conditional templates | `template<class T, class=std::enable_if_t<...>>` |
| 77 | **std::declval** | Create fake reference for decltype | `decltype(std::declval<T>().member)` |

### C++11 LANDMARK LANGUAGE FEATURES REFERENCE


| # | Feature | Explanation | Code Example |
| :--- | :--- | :--- | :--- |
| 1 | **auto type deduction** | Compiler deduces the type of a variable from its initializer; reduces verbosity especially with iterators | `auto x = 42; auto it = v.begin();` |
| 2 | **decltype** | Queries the declared type of an expression without evaluating it | `int x = 0; decltype(x) y = 1;` |
| 3 | **Trailing return types** | Return type is written after the parameter list using `->`, useful when return type depends on parameters | `auto add(int a, int b) -> int { return a + b; }` |
| 4 | **nullptr** | New null pointer constant replacing 0 and NULL; eliminates overload resolution ambiguity | `int* p = nullptr;` |
| 5 | **Strongly typed enums** | Scoped enumerations (`enum class`) that don't leak names; prevent implicit integer conversion | `enum class Color { Red, Green };` |
| 6 | **Range-based for loop** | Clean iteration over containers and arrays without explicit iterators | `for (auto& x : v) x *= 2;` |
| 7 | **Lambda expressions** | Anonymous inline function objects with capture lists | `auto sq = [](int x){ return x * x; };` |
| 8 | **static_assert** | Compile-time assertion that stops compilation with a message if a condition is false | `static_assert(sizeof(int) >= 4, "msg");` |
| 9 | **constexpr** | Functions and objects evaluated at compile time; enables stronger optimization | `constexpr int square(int x){ return x*x; }` |
| 10 | **Rvalue references** | `T&&` distinguishes temporaries from lvalues; enables move semantics | `void f(std::string&& s) { /* move */ }` |
| 11 | **Move semantics** | Objects can transfer ownership of resources instead of making expensive deep copies | `std::vector<int> b = std::move(a);` |
| 12 | **Universal references** | `T&&` in template context can bind to both lvalues and rvalues | `template<class T> void g(T&& x);` |
| 13 | **Variadic templates** | Templates accepting any number of arguments via parameter packs | `template<class... Ts> void log(Ts... xs) {}` |
| 14 | **Uniform initialization** | Consistent brace initialization syntax `{}` for all types; introduces `initializer_list` | `std::vector<int> v{1,2,3};` |
| 15 | **Delegating constructors**| A constructor can call another constructor in the same class | `A():A(0){}` |
| 16 | **Inherited constructors** | `using Base::Base` imports constructors from base into derived class | `struct D : B { using B::B; };` |
| 17 | **Defaulted functions** | `default` asks the compiler to generate standard implementation | `A() = default;` |
| 18 | **Deleted functions** | `delete` explicitly forbids a function from being used | `A(const A&) = delete;` |
| 19 | **Member initializers** | Data members can be initialized directly where they are declared | `int x = 10;` |
| 20 | **override** | Ensures a virtual function in a derived class actually overrides a base method | `void f() override;` |
| 21 | **final** | Prevents further overriding or inheritance | `virtual void f() final;` |
| 22 | **noexcept** | Marks a function as non-throwing; critical for move operations | `void h() noexcept {}` |
| 23 | **Explicit conversion** | Conversion operators that only trigger when explicitly cast | `explicit operator bool() const;` |
| 24 | **Ref-qualified members** | Overload based on whether `*this` is lvalue or rvalue | `void f() & {} void f() && {}` |
| 25 | **Type aliases (using)** | Cleaner alternative to `typedef`; supports alias templates | `using ll = long long;` |
| 26 | **Raw string literals** | Strings without backslash escaping using `R"(...)"` | `std::string s = R"(C:\temp)";` |
| 27 | **char16_t / char32_t** | Dedicated types for Unicode UTF-16 and UTF-32 code units | `char16_t c = u'a';` |
| 28 | **User-defined literals** | Custom meaning to literal suffixes | `long double operator"" _km(long double x);` |
| 29 | **[[attributes]] syntax** | Standard double-bracket attribute syntax | `[[noreturn]] void fail();` |
| 30 | **Right-angle bracket fix**| `>>` in nested templates no longer needs to be written as `> >` | `std::vector<std::vector<int>> grid;` |
| 31 | **alignas / alignof** | Control and query alignment requirements | `struct alignas(16) Vec4;` |
| 32 | **Inline namespaces** | Names are visible from enclosing namespace; useful for versioning | `inline namespace v1 { void f(); }` |
| 33 | **Unrestricted unions** | Unions can contain types with non-trivial members | `union U { int i; double d; };` |
| 34 | **Extern templates** | Suppresses implicit template instantiation to reduce compile time | `extern template class std::vector<int>;` |
| 35 | **std::unique_ptr** | Sole-ownership smart pointer with RAII; replaces raw new/delete | `auto p = std::unique_ptr<int>(new int(5));` |
| 36 | **std::shared_ptr** | Reference-counted shared ownership smart pointer | `auto p = std::make_shared<int>(10);` |
| 37 | **std::weak_ptr** | Non-owning observer of a shared_ptr; breaks reference cycles | `std::weak_ptr<int> w = p;` |
| 38 | **std::thread** | Standard portable threads | `std::thread t([]{ work(); });` |
| 39 | **std::atomic<T>** | Atomic types for lock-free access to shared variables | `std::atomic<int> cnt{0};` |
| 40 | **std::future / promise** | Communicate results asynchronously between threads | `std::promise<int> p; auto f = p.get_future();` |


C++11 was the **Modern Revolution**. It transformed C++ from a "Better C" into a high-level, expressive language without sacrificing a single byte of performance.
1. **Move Semantics**: The end of unnecessary copies.
2. **Smart Pointers**: The end of the "Memory Leak Era."
3. **The Threading Model**: Standardized concurrency for a multi-core world.
4. **Auto & Lambdas**: Syntactic sugar that allowed for more functional and readable code.

**The Golden Rule of C++11**: Prefer `std::unique_ptr` over raw pointers, and use `std::move` to transfer ownership. You have transcended the manual memory management of the past.

# VOLUME 03 REFINEMENT GENERICS C14

