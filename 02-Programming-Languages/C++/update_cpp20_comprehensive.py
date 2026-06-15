import re

def update_cpp20_comprehensive():
    with open("02-Programming-Languages/C++/Complete-CPP-Zero-to-Godhood.md", 'r', encoding='utf-8') as f:
        content = f.read()

    # --- CHAPTER 26: CONCEPTS & CONSTRAINTS ---
    ch26_new = r"""## CHAPTER 26: C20 CONCEPTS

# C++20 CONCEPTS & CONSTRAINTS

Concepts are the first of the "Four Great Pillars" of C++20. They revolutionize template programming by providing a formal way to specify requirements on template arguments, replacing cryptic SFINAE errors with clear, semantic diagnostics.

### 1. Concepts
Compile-time constraints on template parameters. They allow you to define an interface that a type must satisfy to be used with a template.
```cpp
#include <concepts>

template<typename T>
concept Addable = requires(T a, T b) {
    a + b; // T must support the + operator
};
```

### 2. Requires Expressions
An inline constraint block that tests the validity of expressions, types, or compound requirements.
```cpp
template<typename T>
concept Advanced = requires(T x) {
    x++;                        // Simple requirement: x++ must be valid
    typename T::value_type;      // Type requirement: T must have a value_type
    {*x} -> std::same_as<int>;   // Compound requirement: *x must return int
    requires sizeof(T) <= 4;     // Nested requirement: size constraint
};
```

### 3. Requires Clauses
Attaches a constraint to a template or function declaration using the `requires` keyword.
```cpp
template<typename T>
requires Addable<T>
void f(T a) { /* ... */ }
```

### 4. Constrained auto
`auto` parameters in regular functions and variables can be constrained with a concept, making the code both generic and safe.
```cpp
void f(std::integral auto x) { 
    // x must be an integer type (int, long, char, etc.)
}

std::floating_point auto pi = 3.14159; 
```

### 5. Partial Ordering by Constraints
Among multiple viable overloads, the compiler automatically selects the most-constrained one. This eliminates the need for manual dispatching via `std::enable_if`.
```cpp
void f(std::integral auto) { std::cout << "Integral\n"; }
void f(std::signed_integral auto) { std::cout << "Signed Integral\n"; }

f(1); // Prints "Signed Integral" (more specific constraint)
```
"""

    # --- CHAPTER 27: MODULES ---
    ch27_new = r"""## CHAPTER 27: C20 MODULES

# C++20 MODULES: THE MODERN ALTERNATIVE

Modules are the second "Pillar" of C++20. They replace the textual inclusion system (`#include`) with a binary semantic model, offering **macro isolation** and significant **build time improvements**.

### 1. Core Syntax
Organize code into import/export units. This eliminates macro leakage (macros defined in a module don't affect the importer).
```cpp
// math.cppm (Module Interface)
export module my.math;

export int add(int a, int b) { 
    return a + b; 
}

// client.cpp
import my.math;
int main() {
    return add(5, 10);
}
```

### 2. Physical Structure
*   **Global Module Fragment**: Used to include legacy headers that are not yet modularized.
    ```cpp
    module;
    #include <vector> // Legacy inclusion
    export module data_tool;
    ```
*   **Module Partitions**: Split a large module into multiple files while keeping them under a single module name.
    ```cpp
    export module my_big_lib:internal_part;
    ```
*   **Private Module Fragment**: Allows a module to hide its implementation details entirely from the BMI (Binary Module Interface), preventing unnecessary rebuilds.
    ```cpp
    module :private;
    void secret_helper() {}
    ```
"""

    # --- CHAPTER 28: COROUTINES ---
    ch28_new = r"""## CHAPTER 28: C20 COROUTINES

# C++20 COROUTINES: STACKLESS STATE MACHINES

Coroutines are the third "Pillar" of C++20. They are stackless functions that can suspend execution while retaining their state, only to be resumed later.

### 1. Key Keywords
*   **`co_await`**: Suspend execution until an awaited operation (task/future) completes.
*   **`co_yield`**: Suspend execution and return a value to the caller (used in generators).
*   **`co_return`**: Complete execution and return a final value.

### 2. The Infrastructure
A function is a coroutine if it contains any of the above keywords. It requires a `promise_type` and a `coroutine_handle` to manage its lifecycle.

```cpp
#include <coroutine>

struct Task {
    struct promise_type {
        Task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};
```

### 3. Practical Example: The Generator
(Note: `std::generator` was finalized in C++23, but the machinery is here).
```cpp
Generator sequence(int start) {
    while (true) {
        co_yield start++; // Suspend and return current value
    }
}
```
"""

    # --- CHAPTER 29: RANGES & VIEWS ---
    ch29_new = r"""## CHAPTER 29: C20 RANGES

# C++20 RANGES & VIEWS

The Ranges library is the fourth "Pillar" of C++20. it provides composable, lazy-evaluated algorithm pipelines that eliminate the need for verbose iterator pairs.

### 1. Composable Views
Views are non-owning, lazy wrappers. Computation only happens when you iterate over the result.
```cpp
#include <ranges>
#include <vector>

namespace views = std::views;

int main() {
    std::vector v = {1, 2, 3, 4, 5, 6};
    
    // Composable pipeline
    auto result = v | views::filter([](int x){ return x % 2 == 0; })
                    | views::transform([](int x){ return x * x; });
    
    // 4, 16, 36
}
```

### 2. Range Algorithms
All standard algorithms now have `std::ranges::` equivalents that take containers directly.
```cpp
std::ranges::sort(v);
std::ranges::find(v, 42);
```

### 3. Projections & Safety
*   **Projections**: Transform data on the fly before processing.
    ```cpp
    std::ranges::sort(users, {}, &User::name); // Sort by name member
    ```
*   **Dangling Iterator Protection**: Prevents returning iterators to temporary containers.
    ```cpp
    auto it = std::ranges::find(std::vector{1,2}, 1); // it is std::ranges::dangling
    ```
"""

    # --- CHAPTER 30: CORE LANGUAGE FEATURES ---
    ch30_new = r"""## CHAPTER 30: C20 CORE LANGUAGE FEATURES

# C++20 CORE LANGUAGE UPGRADES

Beyond the "Big Four," C++20 added 35+ essential tools for performance, safety, and syntactic clarity.

### 1. Comparison & Constants
*   **Three-way comparison (<=>)**: The "spaceship operator" returns ordering types. Defaulting it generates all 6 comparison operators automatically.
    ```cpp
    auto operator<=>(const S&) const = default;
    ```
*   **consteval**: Declares an "immediate function" that MUST be evaluated at compile time.
    ```cpp
    consteval int sq(int x) { return x * x; }
    ```
*   **constinit**: Ensures a variable is initialized with a constant expression at static initialization time; it remains mutable.
    ```cpp
    constinit int counter = 0;
    ```
*   **constexpr virtual functions**: Virtual functions can now be `constexpr`, enabling compile-time polymorphism.
*   **constexpr try-catch**: Allowed in `constexpr` functions (catch is ignored at compile time).
*   **constexpr dynamic_cast / typeid**: Now allowed during constant evaluation.
*   **constexpr allocations**: `new/delete` are allowed in `constexpr` if the memory is freed within the same evaluation.

### 2. Object Initialization & Layout
*   **Designated initializers**: C-style member initialization for aggregates.
    ```cpp
    Point p{.x = 1, .y = 2};
    ```
*   **[[no_unique_address]]**: Optimize layout by allowing empty members to share addresses with other members.
    ```cpp
    struct S { [[no_unique_address]] Empty e; int x; };
    ```
*   **Parenthesized aggregate initialization**: `Point p(1, 2);` now works for aggregates, fixing some `new` edge cases.
*   **Default member initializers for bit-fields**: `struct S { int x:4 = 1; };`
*   **Prohibit aggregates with user constructors**: Surprising behaviors are prevented by making classes with any user-declared constructor non-aggregates.

### 3. Lambda Evolution
*   **Template parameter list**: Explicit syntax for finer control.
    ```cpp
    []<typename T>(std::vector<T> v) { /* ... */ };
    ```
*   **Lambda [=, this] capture**: Explicitly capture `this` by reference; the old `[=]` capturing `this` is deprecated.
*   **Pack expansion in init-capture**: Direct capture of parameter packs without tuples.
    ```cpp
    [...args = std::forward<Args>(args)](){ f(args...); }
    ```
*   **Stateless lambdas**: Now default-constructible and assignable, allowing their use as comparators in containers without overhead.

### 4. Logic & Control
*   **Init-statements for range-for**: `for (auto& data = getData(); auto& x : data) { ... }`
*   **[[likely]] / [[unlikely]]**: Branch prediction hints for the optimizer.
*   **using enum**: Bring all enumerator names from an enum class into the current scope.
*   **Conditionally explicit constructors**: `explicit(bool_expr)` allows fine-grained control over implicit conversions.
*   **__VA_OPT__**: Macro helper that expands its argument only if the variadic pack is non-empty.
*   **Implicit move**: Return statements perform implicit move in more cases, including rvalue references.
*   **Two's complement**: The standard now mandates two's complement for signed integers.

### 5. Type System & Templates
*   **char8_t**: Dedicated type for UTF-8 character data to avoid accidental conversions.
*   **Class types as NTTP**: Literal class types can now be non-type template arguments.
    ```cpp
    template<std::string_view S> struct Tag {};
    ```
*   **CTAD for aggregates/aliases**: Template deduction now works for aggregates and alias templates.
*   **Optional typename**: In many dependent contexts, `typename` is now optional.
*   **Feature-test macros**: Standardized macros like `__cpp_concepts` to check compiler support.
"""

    # --- CHAPTER 31: LIBRARY ADDITIONS ---
    ch31_new = r"""## CHAPTER 31: C20 STANDARD LIBRARY ADDITIONS

# C++20 STANDARD LIBRARY EXPANSION

### 1. Formatting & Memory
*   **std::format**: Type-safe, high-performance string formatting.
    ```cpp
    std::string s = std::format("Hello {} {}", "C++", 20);
    ```
*   **std::span**: Lightweight non-owning view of contiguous memory.
    ```cpp
    void f(std::span<int> s) { /* ... */ }
    ```
*   **std::bit_cast**: Safe bit-level reinterpretation (constexpr-friendly).

### 2. Concurrency & Sync
*   **std::jthread**: Joining thread that supports cooperative cancellation via `std::stop_token`.
*   **std::latch / barrier**: Synchronization points for multi-threaded coordination.
*   **std::semaphore**: `counting_semaphore` and `binary_semaphore`.
*   **Atomic Wait/Notify**: Efficient wait/signal for atomics without condition variables.
*   **std::atomic_ref**: Apply atomic operations to non-atomic objects temporarily.
*   **Atomic shared_ptr**: `std::atomic<std::shared_ptr<T>>` is now fully supported.

### 3. Utilities & Strings
*   **std::source_location**: Capture call-site info (file, line, function) without macros.
*   **std::chrono (Calendars & Timezones)**: Massive expansion including `year_month_day` and IANA support.
*   **string::starts_with / ends_with**: Direct checks for `std::string` and `std::string_view`.
*   **std::erase / erase_if**: Free function versions of the erase-remove idiom.
*   **associative::contains**: `map` and `set` now have a `.contains(key)` member.
*   **std::ssize**: Returns signed size to avoid signed/unsigned comparison warnings.
*   **std::numbers**: Standard math constants (`pi`, `e`) in `<numbers>`.

### 4. Advanced Math & Pointers
*   **std::midpoint / lerp**: Numerically correct math without overflow.
*   **std::to_address**: Uniform way to get a raw pointer from any pointer-like object.
*   **std::endian**: Compile-time check for byte order.
"""

    # Replace chapters in main content
    content = re.sub(r'## CHAPTER 26: C20 CONCEPTS.*?## CHAPTER 32: C23 CORE LANGUAGE', 
                    ch26_new + "\n" + ch27_new + "\n" + ch28_new + "\n" + ch29_new + "\n" + ch30_new + "\n" + ch31_new + "\n## CHAPTER 32: C23 CORE LANGUAGE", 
                    content, flags=re.DOTALL)

    # --- LANDMARK TABLE ---
    cpp20_table = r"""
### C++20 LANDMARK FEATURES REFERENCE (FOR INTERVIEWS)
| Priority | Feature | Explanation | Code Example |
| :--- | :--- | :--- | :--- |
| 🔴 **Must Know** | **Concepts + requires** | Formal template constraints | `template<integral T>` |
| 🔴 **Must Know** | **Coroutines** | co_await/co_yield/co_return | `co_yield n++;` |
| 🔴 **Must Know** | **Ranges / Views** | Composable lazy pipelines | `v | views::filter(even)` |
| 🔴 **Must Know** | **std::format** | Modern type-safe formatting | `std::format("Val: {}", x)` |
| 🔴 **Must Know** | **std::span** | Non-owning contiguous view | `void f(span<int> s)` |
| 🔴 **Must Know** | **<=> Spaceship** | Auto-generate comparisons | `operator<=> = default` |
| 🔴 **Must Know** | **consteval** | Forced compile-time functions | `consteval int sq(int x)` |
| 🔴 **Must Know** | **Designated Init** | C-style aggregate init | `Point p = {.x=1, .y=2}` |
| 🟡 **Good to Know** | **Modules** | Semantic binary inclusion | `import std.core;` |
| 🟡 **Good to Know** | **std::jthread** | Auto-joining threads | `std::jthread t(task)` |
| 🟡 **Good to Know** | **Sync Primitives** | Latch, Barrier, Semaphore | `std::latch l{3};` |
| 🟡 **Good to Know** | **source_location** | Replaces \_\_FILE\_\_ / \_\_LINE\_\_ | `source_location::current()` |
| 🟡 **Good to Know** | **bit_cast / endian** | Low-level bit manipulation | `std::bit_cast<float>(u)` |
| 🟢 **Nice to Know** | **char8_t** | Dedicated UTF-8 type | `u8"text"` |
| 🟢 **Nice to Know** | **constinit** | Static constant initialization | `constinit int x = 0;` |
| 🟢 **Nice to Know** | **[[likely]]** | Branch prediction hints | `if (x) [[likely]]` |
"""
    content = re.sub(r'### C++20 LANDMARK LANGUAGE FEATURES REFERENCE.*?(?=\n#)', 
                    cpp20_table, content, flags=re.DOTALL)

    with open("02-Programming-Languages/C++/Complete-CPP-Zero-to-Godhood.md", 'w', encoding='utf-8') as f:
        f.write(content)

if __name__ == "__main__":
    update_cpp20_comprehensive()
    print("C++20 comprehensive expansion complete.")
