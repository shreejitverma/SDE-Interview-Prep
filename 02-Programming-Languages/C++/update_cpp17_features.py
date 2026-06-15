import re

def update_cpp17():
    with open("02-Programming-Languages/C++/Complete-CPP-Zero-to-Godhood.md", 'r', encoding='utf-8') as f:
        content = f.read()

    # --- CHAPTER 20: C17 CORE LANGUAGE FEATURES ---
    ch20_new = r"""## CHAPTER 20: C17 CORE LANGUAGE FEATURES

# C++17 CORE LANGUAGE FEATURES

C++17 focuses on **Simplification and Vocabulary**, removing boilerplate and making the language more expressive for daily tasks.

### 1. Fundamental Syntactic Sugar
*   **Structured bindings**: Decompose structs, pairs, tuples, and arrays into named variables in one declaration.
    ```cpp
    auto [id, val, name] = get_data();
    for (const auto& [key, value] : my_map) { /* ... */ }
    ```
*   **Init-statements in if/switch**: Declare a variable inside an if or switch condition, scoped strictly to that block.
    ```cpp
    if (auto it = m.find(key); it != m.end()) {
        return it->second;
    }
    ```
*   **Inline variables**: Variables can be declared `inline` in headers, eliminating the need for a separate `.cpp` definition (prevents ODR violations).
    ```cpp
    struct Config {
        static inline int counter = 0;
    };
    ```
*   **Simplified nested namespaces**: Write `A::B::C` directly instead of nesting three separate namespace blocks.
    ```cpp
    namespace A::B::C { void f() {} }
    ```
*   **New auto rules for direct-list-init**: `auto x{1}` now deduces `int`, not `std::initializer_list<int>`. Multiple elements are ill-formed.
    ```cpp
    auto x{42}; // int
    ```

### 2. Modern Compilation & Performance
*   **if constexpr**: Compile-time branch selection inside templates. The discarded branch is not even compiled, allowing code that would otherwise be ill-formed.
    ```cpp
    template<typename T>
    void process(T t) {
        if constexpr (std::is_integral_v<T>) {
            process_int(t);
        } else {
            process_generic(t);
        }
    }
    ```
*   **Guaranteed copy elision (RVO)**: Return value optimization is now mandatory even for non-movable types in certain cases.
    ```cpp
    auto x = NonMoveable(42); // No copy or move constructor called
    ```
*   **constexpr lambda**: Lambdas can now be declared `constexpr` and evaluated at compile time.
    ```cpp
    constexpr auto sq = [](int x){ return x*x; };
    static_assert(sq(5) == 25);
    ```
*   **Lambda [*this] capture**: Captures the current object by value inside a lambda (safe for asynchronous execution).
    ```cpp
    auto f = [*this](){ return x; };
    ```
*   **Dynamic allocation of over-aligned data**: `new` now correctly handles types with alignments beyond default using `std::align_val_t`.
    ```cpp
    alignas(16) float4* p = new float4[1000];
    ```

### 3. Attributes & Metadata
*   **[[nodiscard]]**: Warns if the return value of a function or a function returning a marked type is discarded.
    ```cpp
    [[nodiscard]] int compute();
    ```
*   **[[maybe_unused]]**: Suppresses compiler warnings for unused variables, functions, or parameters.
    ```cpp
    [[maybe_unused]] int debug_flag = 0;
    ```
*   **[[fallthrough]]**: Marks intentional fall-through in a `switch` case to suppress warnings.
    ```cpp
    case 1: [[fallthrough]]; case 2: /* ... */
    ```
*   **__has_include**: Preprocessor directive to check if a header is available before including it.
    ```cpp
    #if __has_include(<optional>)
    #  include <optional>
    #endif
    ```
*   **Ignore unknown attributes**: Compilers must now silently ignore attribute namespaces they don't support.

### 4. Logic & Mechanics
*   **noexcept as part of type system**: `noexcept` specification is now part of the function's type, enabling overload differentiation.
    ```cpp
    void(*p)() noexcept; // Distinct type from void(*)()
    ```
*   **Stricter expression evaluation order**: Postfix left-to-right, assignment right-to-left, shift left-to-right. Reduces UB in chained calls.
    ```cpp
    m[0] = m.size(); // Defined behavior in C++17
    ```
*   **static_assert with no message**: The message argument is now optional.
    ```cpp
    static_assert(sizeof(int) >= 4);
    ```
*   **Hexadecimal floating-point literals**: Floating-point numbers expressed in hex for precise bit-level representation.
    ```cpp
    double x = 0x1.8p+1; // 1.5 * 2^1 = 3.0
    ```
*   **Aggregate initialization with base classes**: Aggregates derived from base classes can now use brace initialization.
    ```cpp
    struct D : Base { int b; }; D d{{1,2}, 3};
    ```
*   **Direct-list-initialization of enums**: Enum class with a fixed underlying type can be initialized with brace syntax.
    ```cpp
    enum class Handle : uint32_t { Invalid=0 }; Handle h{42};
    ```
*   **Different begin/end types in range-for**: Allows `begin` and `end` to be different types, enabling the Sentinel pattern.
*   **Improved inheriting constructors**: Inheriting a constructor now acts like inheriting any other base class member—no extra copy made.
*   **Removal of register keyword**: The `register` keyword is officially removed.
*   **Removal of operator++(bool)**: Pre/post increment on `bool` is officially removed.
*   **Removing deprecated exception specs**: Old-style `throw(type)` dynamic specifications are removed; only `noexcept` remains.
*   **std::uncaught_exceptions()**: Returns the number of currently active uncaught exceptions (useful for scope guards).
"""
    content = re.sub(r'## CHAPTER 20: C17 CORE LANGUAGE FEATURES.*?## CHAPTER 21: C17 TEMPLATE METAPROGRAMMING', 
                    ch20_new + "\n## CHAPTER 21: C17 TEMPLATE METAPROGRAMMING", 
                    content, flags=re.DOTALL)

    # --- CHAPTER 21: C17 TEMPLATE METAPROGRAMMING ---
    ch21_new = r"""## CHAPTER 21: C17 TEMPLATE METAPROGRAMMING

# C++17 TEMPLATE METAPROGRAMMING

C++17 revolutionized template code by making it more readable and reducing the need for complex SFINAE patterns.

### 1. Fold Expressions
Variadic parameter packs can now be reduced using binary operators with `...` syntax.
```cpp
template<class... Ts> 
auto sum(Ts... xs) { 
    return (xs + ...); // Unary right fold
}

template<class... Ts> 
bool all(Ts... xs) { 
    return (xs && ...); // Binary fold
}
```

### 2. Class Template Argument Deduction (CTAD)
Constructor arguments alone can now deduce class template parameters; no `std::make_*` helpers needed.
```cpp
std::pair p{1, 2.5};        // Deduced as pair<int, double>
std::vector v{1, 2, 3};      // Deduced as vector<int>
std::lock_guard lk(mtx);     // Deduced as lock_guard<mutex>
```

### 3. Template Power Features
*   **auto non-type template params**: Non-type template parameters can use `auto` to accept any integral/pointer non-type.
    ```cpp
    template<auto N> struct S { int a[N]; };
    S<10> s1; S<'c'> s2;
    ```
*   **typename in template template params**: Allows `typename` instead of only `class` in template template parameters.
    ```cpp
    template<template<typename...> typename C> struct Foo;
    ```
*   **Pack expansions in using-declarations**: Inject names from all types in a parameter pack via a single using declaration.
    ```cpp
    template<class... Ts> struct Overloader : Ts... { using Ts::operator()...; };
    ```
*   **Allow constant eval for all non-type template args**: Pointers, references, and pointers-to-members can now be used as non-type template args via `constexpr`.
    ```cpp
    constexpr int* p() { return &n; } A<p()> b;
    ```
*   **Matching of template template-args**: A template template-parameter can now bind to compatible templates with more parameters (DR fix). `A<std::vector>` now works even though vector takes 2 parameters.
*   **Attribute namespaces without repetition**: Group attributes in the same namespace using the `using` prefix.
    ```cpp
    [[using rpr: kernel, target(cpu,gpu)]]
    ```

### 4. Metaprogramming Utilities
*   **std::void_t**: Maps any type list to `void`; used in SFINAE detection idioms.
    ```cpp
    template<class T, class=std::void_t<>> struct has_x : std::false_type {};
    ```
*   **std::conjunction / disjunction / negation**: Short-circuit logical metafunctions combining type traits.
    ```cpp
    std::conjunction<std::is_integral<T>, std::is_signed<T>>{}
    ```
"""
    content = re.sub(r'## CHAPTER 21: C17 TEMPLATE METAPROGRAMMING.*?## CHAPTER 22: C17 VOCABULARY TYPES', 
                    ch21_new + "\n## CHAPTER 22: C17 VOCABULARY TYPES", 
                    content, flags=re.DOTALL)

    # --- CHAPTER 22: C17 VOCABULARY TYPES ---
    ch22_new = r"""## CHAPTER 22: C17 VOCABULARY TYPES

# C++17 VOCABULARY TYPES

These types form the standard "Standard English" of modern C++, providing type-safe alternatives to pointers, unions, and raw character arrays.

### 1. The Core Types
*   **std::string_view**: Lightweight non-owning view into a character sequence; avoids unnecessary heap copies.
    ```cpp
    void f(std::string_view sv) { sv.substr(0, 3); }
    ```
*   **std::optional**: Represents a value that may or may not be present; safer alternative to sentinel values or raw pointers.
    ```cpp
    std::optional<int> f(int x) { 
        if (x > 0) return x; 
        return std::nullopt; 
    }
    ```
*   **std::variant**: Type-safe union holding exactly one of a fixed set of types at a time.
    ```cpp
    std::variant<int, double, std::string> v = "hi";
    std::visit([](auto&& arg){ std::cout << arg; }, v);
    ```
*   **std::any**: Type-safe container for a single value of any copy-constructible type.
    ```cpp
    std::any a = 42; 
    a = std::string{"hello"};
    ```

### 2. Advanced Utilities
*   **std::monostate**: Acts as a default-constructible placeholder for `std::variant`'s first alternative.
    ```cpp
    std::variant<std::monostate, NonDefaultCtorType> v;
    ```
*   **std::as_const**: Returns a const reference to an object; useful in generic code to force const overloads.
    ```cpp
    auto& cv = std::as_const(v);
    ```
*   **Searchers**: Efficient string search algorithms usable with `std::search` (`boyer_moore`, `knuth_morris_pratt`).
    ```cpp
    std::search(s.begin(), s.end(), std::boyer_moore_searcher(p.begin(), p.end()));
    ```
"""
    content = re.sub(r'## CHAPTER 22: C17 VOCABULARY TYPES.*?## CHAPTER 23: C17 FILESYSTEM AND IO', 
                    ch22_new + "\n## CHAPTER 23: C17 FILESYSTEM AND IO", 
                    content, flags=re.DOTALL)

    # --- CHAPTER 23: C17 FILESYSTEM AND IO ---
    ch23_new = r"""## CHAPTER 23: C17 FILESYSTEM AND IO

# C++17 FILESYSTEM & I/O

C++17 finally standardized interaction with the operating system's file systems.

### 1. std::filesystem
A full library for path manipulation, directory traversal, and file operations.
```cpp
namespace fs = std::filesystem;
fs::path p = "output/logs/main.log";
fs::create_directories(p.parent_path());
if (fs::exists(p)) { /* ... */ }
```

### 2. High-Performance I/O
*   **std::from_chars / std::to_chars**: Low-level, non-throwing, non-allocating, locale-independent number <-> string conversions. The gold standard for HFT.
    ```cpp
    std::to_chars(buf, buf + 8, 1986);
    ```
"""
    content = re.sub(r'## CHAPTER 23: C17 FILESYSTEM AND IO.*?## CHAPTER 24: C17 PARALLEL ALGORITHMS AND CONCURRENCY', 
                    ch23_new + "\n## CHAPTER 24: C17 PARALLEL ALGORITHMS AND CONCURRENCY", 
                    content, flags=re.DOTALL)

    # --- CHAPTER 24: C17 PARALLEL ALGORITHMS AND CONCURRENCY ---
    ch24_new = r"""## CHAPTER 24: C17 PARALLEL ALGORITHMS AND CONCURRENCY

# C++17 PARALLELISM & CONCURRENCY

### 1. Parallel Algorithms
Standard algorithms now accept execution policies to leverage multi-core CPUs and vector units automatically.
*   **Policies**: `std::execution::seq`, `std::execution::par`, `std::execution::par_unseq`.
    ```cpp
    std::sort(std::execution::par, v.begin(), v.end());
    ```
*   **std::reduce**: Like `std::accumulate` but parallelizable and allows operation reordering.
    ```cpp
    auto s = std::reduce(std::execution::par, v.begin(), v.end(), 0);
    ```
*   **std::transform_reduce**: Combines transform and reduce in one parallelizable pass.
*   **std::for_each (parallel)**: Standard overload for parallel iteration.

### 2. Thread Synchronization
*   **std::shared_mutex**: Untimed reader-writer mutex for shared read, exclusive write access.
    ```cpp
    std::shared_mutex m; 
    std::shared_lock lk(m); // Reader
    ```
*   **std::scoped_lock**: Locks multiple mutexes simultaneously in a deadlock-free manner using a variadic constructor.
    ```cpp
    std::scoped_lock lk(m1, m2);
    ```
"""
    content = re.sub(r'## CHAPTER 24: C17 PARALLEL ALGORITHMS AND CONCURRENCY.*?## CHAPTER 25: C17 STANDARD LIBRARY ADDITIONS', 
                    ch24_new + "\n## CHAPTER 25: C17 STANDARD LIBRARY ADDITIONS", 
                    content, flags=re.DOTALL)

    # --- CHAPTER 25: C17 STANDARD LIBRARY ADDITIONS ---
    ch25_new = r"""## CHAPTER 25: C17 STANDARD LIBRARY ADDITIONS

# C++17 STANDARD LIBRARY EXPANSION

### 1. Generic Utilities
*   **std::apply**: Calls a function with the elements of a tuple unpacked as arguments.
    ```cpp
    std::apply(sum, std::tuple{1, 2, 3});
    ```
*   **std::invoke**: Uniformly calls any callable—function object, member function pointer, or member data pointer.
    ```cpp
    std::invoke(&Foo::bar, obj, arg);
    ```
*   **std::make_from_tuple**: Constructs an object by unpacking a tuple into its constructor.
*   **std::not_fn**: Returns the logical negation of a callable.
*   **std::size / std::empty / std::data**: Generic free functions working across both arrays and containers.

### 2. Advanced Performance & Math
*   **Polymorphic allocators (std::pmr)**: Memory resource-based allocators for runtime polymorphism over allocation strategies.
    ```cpp
    std::pmr::vector<int> v(&pool);
    ```
*   **Splicing maps and sets**: Move nodes between map/set containers without allocation via `extract` and `merge`.
    ```cpp
    auto node = m1.extract("key"); m2.insert(std::move(node));
    ```
*   **std::shared_ptr array support**: `std::shared_ptr<T[]>` now natively handles arrays without a custom deleter.
*   **std::sample**: Samples N elements from a range randomly, respecting forward-iterator constraints.
*   **std::gcd / std::lcm**: Compile-time or runtime math utilities.
*   **std::clamp**: Clamps a value between a low and high bound.
*   **Mathematical special functions**: New `<cmath>` functions including `std::riemann_zeta`, `std::beta`, `std::hermite`, etc.

### 3. Container Improvements
*   Improved `std::pair` and `std::tuple` with more constructors and conversions.
*   `std::string_view` now works natively with standard algorithms.
"""
    content = re.sub(r'## CHAPTER 25: C17 STANDARD LIBRARY ADDITIONS.*?# VOLUME 04: GODHOOD SUMMARY', 
                    ch25_new + "\n# VOLUME 04: GODHOOD SUMMARY", 
                    content, flags=re.DOTALL)

    # --- C++17 LANDMARK TABLE ---
    cpp17_table = r"""
### C++17 LANDMARK FEATURES REFERENCE (FOR INTERVIEWS)
| Priority | Feature | Explanation | Code Example |
| :--- | :--- | :--- | :--- |
| 🔴 **Must Know** | **Structured bindings** | Unpack tuples/structs in one declaration | `auto [a, b] = pair;` |
| 🔴 **Must Know** | **if constexpr** | Compile-time branch selection | `if constexpr (is_int_v<T>)` |
| 🔴 **Must Know** | **std::optional** | Represents a maybe-present value | `std::optional<int> res;` |
| 🔴 **Must Know** | **std::variant** | Type-safe discriminated union | `std::variant<int, float> v;` |
| 🔴 **Must Know** | **std::string_view** | Non-owning string reference | `void f(string_view sv);` |
| 🔴 **Must Know** | **CTAD** | Class template argument deduction | `std::vector v{1, 2, 3};` |
| 🟡 **Good to Know** | **std::filesystem** | Standard file system manipulation | `fs::exists("path");` |
| 🟡 **Good to Know** | **std::apply / invoke** | Uniform callable invocation | `std::invoke(f, args...);` |
| 🟡 **Good to Know** | **Parallel Algos** | Multi-threaded standard algorithms | `sort(std::execution::par, b, e);` |
| 🟡 **Good to Know** | **std::from/to_chars** | Fast, non-allocating string conversions | `to_chars(buf, buf+8, 42);` |
| 🟡 **Good to Know** | **std::scoped_lock** | Deadlock-free multi-mutex locking | `scoped_lock lk(m1, m2);` |
| 🟢 **Nice to Know** | **std::pmr** | Polymorphic Memory Resources | `pmr::vector<int> v(&pool);` |
| 🟢 **Nice to Know** | **Fold Expressions** | Reduction of parameter packs | `(args + ...)` |
"""
    # Replace existing Landmark table if present, else append to summary
    if "### C++17 LANDMARK FEATURES REFERENCE" in content:
        content = re.sub(r'### C++17 LANDMARK FEATURES REFERENCE.*?(\n#|$)', 
                        cpp17_table + r"\1", content, flags=re.DOTALL)
    else:
        content = content.replace("# VOLUME 04: GODHOOD SUMMARY", "# VOLUME 04: GODHOOD SUMMARY\n" + cpp17_table)

    with open("02-Programming-Languages/C++/Complete-CPP-Zero-to-Godhood.md", 'w', encoding='utf-8') as f:
        f.write(content)

if __name__ == "__main__":
    update_cpp17()
    print("C++17 features integrated.")
