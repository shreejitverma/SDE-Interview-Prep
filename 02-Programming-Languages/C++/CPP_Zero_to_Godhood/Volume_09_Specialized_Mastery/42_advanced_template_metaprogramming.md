# Chapter 42: Advanced Template Metaprogramming

# ADVANCED TEMPLATE METAPROGRAMMING

## 1. The Evolution of TMP

Template Metaprogramming (TMP) is the art of using the compiler to generate code.

*   **C++98:** Recursive struct instantiation, `enum` hacks. (Hard).
*   **C++11:** `constexpr`, `static_assert`, `using`, Variadic Templates. (Better).
*   **C++14:** Variable templates, `auto` return type. (Cleaner).
*   **C++17:** `if constexpr`, Fold expressions, `void_t`. (Powerful).
*   **C++20:** Concepts (`requires`). (The Holy Grail).

## 2. SFINAE (Substitution Failure Is Not An Error)

Before Concepts, SFINAE was the only way to constrain templates.

### 2.1 `std::enable_if`

```cpp
#include <type_traits>
#include <iostream>

// Enable only for integral types
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
process(T t) {
    std::cout << "Integral: " << t << "\n";
}

// Enable only for floating point
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, void>::type
process(T t) {
    std::cout << "Float: " << t << "\n";
}
```

### 2.2 The `void_t` Trick (C++17)

Detecting if a type has a member function.

```cpp
template <typename T, typename = void>
struct has_print : std::false_type {};

template <typename T>
struct has_print<T, std::void_t<decltype(std::declval<T>().print())>> : std::true_type {};

static_assert(has_print<MyClass>::value, "MyClass must have print()");
```

## 3. Curiously Recurring Template Pattern (CRTP)

Static polymorphism. The base class knows the derived class type at compile time.

```cpp
template <typename Derived>
class Base {
public:
    void interface() {
        // Compile-time dispatch
        static_cast<Derived*>(this)->implementation();
    }
};

class Derived : public Base<Derived> {
public:
    void implementation() {
        std::cout << "Derived impl\n";
    }
};
```

**Use Case:** Mixins, adding functionality (like equality operators) without virtual overhead.

## 4. Policy-Based Design

Designing classes that take "policies" (strategy classes) as template arguments to define behavior.

```cpp
template <typename OutputPolicy, typename LanguagePolicy>
class HelloWorld : public OutputPolicy, public LanguagePolicy {
public:
    void run() {
        print(message()); // OutputPolicy::print, LanguagePolicy::message
    }
};
```

## 5. Modern TMP with Concepts (C++20)

Replacing SFINAE with readable constraints.

```cpp
template<typename T>
concept Printable = requires(T t) {
    { t.print() } -> std::same_as<void>;
};

void process(Printable auto& obj) {
    obj.print();
}
```



#### std::jthread (Auto-joining Thread)

```cpp
#include <thread>
#include <iostream>
using namespace std;

void worker(std::stop_token st) {
    while (!st.stop_requested()) {
        cout << "Working...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    cout << "Worker stopped\n";
}

int main() {
    // jthread automatically joins on destruction
    // and supports stop_token
    std::jthread t(worker);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // t.request_stop() called automatically or manually
    return 0;
}
```

***

## C++20 BEST PRACTICES

### What's Better with C++20

```cpp
// 1. Use concepts for readable templates
template<integral T>
void process(T x);

// 2. Use ranges for composable operations
auto result = v
    | ranges::views::filter([](int x) { return x > 0; })
    | ranges::views::transform([](int x) { return x * 2; });

// 3. Use coroutines for generators
Generator<int> count(int n) {
    for (int i = 0; i < n; i++) {
        co_yield i;
    }
}

// 4. Use spaceship for comparisons
auto cmp = a <=> b;

// 5. Use designated initializers
Config cfg{.host = "localhost", .port = 8080};

// 6. Use std::format for formatting
cout << format("Value: {:.2f}", value);

// 7. Use consteval for compile-time guarantees
consteval int compile_time_only(int x);

// 8. Use modules for better organization
export module app;
```


***


### <a name="chapter-12-c23latestfeatures"></a>CHAPTER 12: C++23 LATEST FEATURES

### C++23 Overview & Direction

C++23 (finalized in 2023) is a **refinement and enhancement** of C++20 with practical improvements.

#### Timeline & Context
- **2011**: C++11 (revolutionary)
- **2014**: C++14 (refinement)
- **2017**: C++17 (major improvements)
- **2020**: C++20 (revolutionary leap)
- **2023**: C++23 (practical enhancements)

#### C++23 Philosophy
- **Enhance** existing C++20 features
- **Fill gaps** in C++20 design
- **Improve** convenience and usability
- **Optimize** common patterns
- **Standardize** frequently-requested features
- **Fix** issues discovered in C++20

#### Key Themes
1. **Output & Formatting** - std::print for easy output
2. **Error Handling** - std::expected for results
3. **Loop Control** - Enhanced for loops with ranges
4. **Memory Safety** - Better pointer/array handling
5. **Debugging** - Stack traces
6. **Templates** - Deducing this improvements
7. **Constexpr** - More compile-time power
8. **Library** - Quality of life improvements

#### Why C++23 Matters
C++23 builds on C++20 strengths:
- ✅ Easier output without iostream overhead
- ✅ Type-safe error handling (std::expected)
- ✅ Better for loop control
- ✅ Debugging support (stack traces)
- ✅ More flexible subscript operator
- ✅ Improved constexpr capabilities
- ✅ More convenient library features
- ✅ Better optional support

***

## STD::PRINT & FORMATTED OUTPUT

### 1.1 std::print - Simple Output

`std::print` provides easy, fast output without iostream overhead.

#### Basic print Usage

```cpp
#include <print>
#include <iostream>

// Simple output (no newline by default)
std::print("Hello, World!");

// With newline
std::println("Hello, World!");

// With format
std::println("Number: {}, Float: {:.2f}", 42, 3.14159);

// To stderr
std::print(std::cerr, "Error: {}\n", "something went wrong");
std::println(std::cerr, "Error: {}", "something went wrong");
```


#### print vs format vs iostream

```cpp
#include <print>
#include <format>
#include <iostream>

std::string msg = "Hello";
int value = 42;

// iostream (slow, verbose)
std::cout << msg << ": " << value << "\n";

// format (creates string, then print)
std::cout << std::format("{}: {}\n", msg, value);

// print (direct output, fast)
std::println("{}: {}", msg, value);
```


#### print with File Streams

```cpp
#include <print>
#include <fstream>

std::ofstream file("output.txt");

// Direct to file
std::println(file, "Line 1: {}", 42);
std::println(file, "Line 2: {}", "test");

// Simpler than:
// file << "Line 1: " << 42 << "\n";
```


***

### 1.2 std::format Enhancements

#### Format Improvements in C++23

```cpp
#include <format>

// More format options
double pi = 3.14159;

std::format("{:.2%}", 0.25);         // "25.00%" (percentage)
std::format("{:g}", 0.0001);         // General format
std::format("{:#x}", 255);           // "0xff" (with prefix)
std::format("{:_^10}", "test");      // "_____test" (custom fill)
```


***

## DEDUCING THIS

### 2.1 Explicit Member Function Parameters

`Deducing this` allows capturing the type and constness of the object.

#### Basic Deducing This

```cpp
#include <iostream>
using namespace std;

struct Counter {
    int count = 0;
    
    // Traditional
    void increment() {
        count++;
    }
    
    // With deducing this
    void increment_new(this auto& self) {
        self.count++;
    }
    
    // Value vs reference overloads now simple
    auto get_data(this auto& self) {
        return self.count;
    }
};

Counter c;
c.increment_new();
cout << c.get_data() << "\n";  // 1
```


#### Deducing This for Const/Non-Const

```cpp
struct Data {
    int value = 42;
    
    // Single function handles both const and non-const
    auto& get(this auto& self) {
        return self.value;
    }
    
    // Before C++23: Need two overloads
    // int& get() { return value; }
    // const int& get() const { return value; }
};

Data d;
d.get() = 100;              // Mutable reference
cout << d.get() << "\n";    // 100

const Data cd;
cout << cd.get() << "\n";   // 100 (const reference)
```


#### Practical Deducing This

```cpp
template<typename T>
struct Optional {
    T value;
    bool has_value_flag = false;
    
    // Works for both optional<T> and optional<const T>
    auto* get_if_value(this auto* self) {
        return self->has_value_flag ? &self->value : nullptr;
    }
};

Optional<int> opt;
opt.value = 42;
opt.has_value_flag = true;

if (auto* ptr = opt.get_if_value()) {
    cout << *ptr << "\n";  // 42
}
```


### 2.2 Deducing This - Beyond the Basics

#### Recursive Lambdas
Previously, lambdas couldn't easily call themselves. Now they can via the explicit object parameter.

```cpp
auto fib = [](this auto&& self, int n) {
    if (n <= 1) return n;
    return self(n - 1) + self(n - 2);
};

cout << fib(10) << "\n"; // 55
```

#### Replacing CRTP
The Curiously Recurring Template Pattern (CRTP) was used to inject functionality into derived classes. `Deducing this` simplifies it.

**Old CRTP:**
```cpp
template <typename Derived>
struct Addable {
    Derived& operator+=(const Derived& other) {
        static_cast<Derived*>(this)->value += other.value;
        return *static_cast<Derived*>(this);
    }
};
struct Int : Addable<Int> { int value; };
```


**New C++23 Way:**
```cpp
struct Addable {
    template <typename Self>
    auto& operator+=(this Self&& self, const Self& other) {
        self.value += other.value;
        return self;
    }
};
struct Int : Addable { int value; }; // No template parameter needed!
```


***

## RANGE-BASED FOR LOOP ENHANCEMENTS

### 3.1 For Loop Initializers

C++23 allows initialization in range-based for loops.

#### For Loop with Init

```cpp
#include <vector>
#include <iostream>
using namespace std;

vector<int> v1 = {1, 2, 3};
vector<int> v2 = {4, 5, 6};

// Traditional
vector<int> combined;
for (int x : v1) combined.push_back(x);
for (int x : v2) combined.push_back(x);

// C++23: Initialize in loop
for (auto v = vector<int>{1, 2, 3, 4, 5, 6}; int x : v) {
    cout << x << " ";
}

// More practical
for (auto file = open_file("data.txt"); auto line : file.lines()) {
    cout << line << "\n";
}
```


#### For Loop with Init and Structured Binding

```cpp
map<string, vector<int>> data{
    {"a", {1, 2, 3}},
    {"b", {4, 5, 6}}
};

// Initialize and use with structured binding
for (auto it = data.begin(); auto [key, values] : data) {
    cout << key << ": ";
    for (int v : values) cout << v << " ";
    cout << "\n";
}
```

***

## STD::EXPECTED

### 4.1 Result Type for Error Handling

`std::expected` represents either a value or an error.

#### Basic expected Usage

```cpp
#include <expected>
#include <string>
#include <iostream>
using namespace std;

enum class ParseError { InvalidFormat, OutOfRange };

// Function returning expected
expected<int, ParseError> parse_int(const string& s) {
    try {
        return stoi(s);
    } catch (const invalid_argument&) {
        return unexpected(ParseError::InvalidFormat);
    } catch (const out_of_range&) {
        return unexpected(ParseError::OutOfRange);
    }
}

// Usage
auto result = parse_int("42");

if (result) {
    cout << "Value: " << result.value() << "\n";
} else {
    cout << "Error\n";
}
```


#### expected with Transform

```cpp
#include <expected>

expected<int, string> get_value();

// Chain operations with transform
auto result = get_value()
    .transform([](int x) { return x * 2; })
    .transform_error([](const string& e) { return "Failed: " + e; });

if (result) {
    cout << result.value() << "\n";
} else {
    cout << result.error() << "\n";
}
```


#### expected vs optional

```cpp
// optional: Has value or nothing
optional<int> opt = parse_int("42");
if (!opt) {
    // But why did it fail?
}

// expected: Has value or specific error
expected<int, ParseError> exp = parse_int("42");
if (!exp) {
    cout << "Error: " << static_cast<int>(exp.error()) << "\n";
}
```

***

## STD::OPTIONAL IMPROVEMENTS

### 5.1 Enhanced optional Operations

#### optional with Deref Operator

```cpp
#include <optional>

optional<int> opt{42};

// C++23: Monadic operations
auto result = opt
    .and_then([](int x) -> optional<int> { return x * 2; })
    .or_else([]() { return optional<int>(0); });

// C++23: Chaining
opt.transform([](int x) { return x + 10; })
   .and_then([](int x) -> optional<int> {
       if (x > 50) return x;
       return nullopt;
   });
```


#### optional::value_or_else

```cpp
#include <optional>

optional<int> opt;

// Get value or call function to generate default
int value = opt.value_or_else([]() { return compute_default(); });

// More flexible than value_or
// value_or: int value = opt.value_or(0);
// value_or_else: int value = opt.value_or_else([]() { return expensive_computation(); });
```


***

## MULTIDIMENSIONAL SUBSCRIPT OPERATOR

### 6.1 Multiple Index Support

C++23 allows multiple indices in subscript operator.

#### Multi-Index Subscript

```cpp
#include <iostream>
using namespace std;

struct Matrix {
    int data[10][10];
    
    // C++23: Multi-index operator
    int& operator[](int row, int col) {
        return data[row][col];
    }
    
    int& operator[](int row, int col) const {
        return data[row][col];
    }
};

Matrix m;
m[2, 3] = 42;           // Two indices
cout << m[2, 3] << "\n"; // 42
```


#### Dynamic 2D Array Wrapper

```cpp
template<typename T>
struct Array2D {
    vector<T> data;
    size_t width, height;
    
    Array2D(size_t w, size_t h) : width(w), height(h), data(w * h) {}
    
    T& operator[](size_t row, size_t col) {
        return data[row * width + col];
    }
    
    const T& operator[](size_t row, size_t col) const {
        return data[row * width + col];
    }
};

Array2D<int> grid(3, 3);
grid[1, 1] = 5;
cout << grid[1, 1] << "\n";  // 5
```


***

### 6.2 std::mdspan (Multidimensional View)

`std::mdspan` provides a non-owning multidimensional view of contiguous data.

#### Basic mdspan Usage

```cpp
#include <mdspan>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> data = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12
    };

    // Create a 3x4 view over the data (C++23)
    // using MatrixView = std::mdspan<int, std::dextents<size_t, 2>>;
    // MatrixView m(data.data(), 3, 4);
    
    // m[1, 2] == 7 (row 1, col 2)
    // No copying of data involved!
    
    return 0;
}
```


### 6.3 mdspan Layouts

You can control how 2D indices map to the 1D memory.

*   `std::layout_right`: Row-major (default in C++). Index `(i, j)` is `i * N + j`.
*   `std::layout_left`: Column-major (Fortran/MATLAB). Index `(i, j)` is `j * M + i`.

```cpp
using RowMajor = std::mdspan<double, std::dextents<size_t, 2>, std::layout_right>;
using ColMajor = std::mdspan<double, std::dextents<size_t, 2>, std::layout_left>;

// Same data, different interpretation
std::vector<double> v = {1, 2, 3, 4}; // 2x2 matrix
RowMajor m_row(v.data(), 2, 2); // [[1, 2], [3, 4]]
ColMajor m_col(v.data(), 2, 2); // [[1, 3], [2, 4]]
```


***


## STD::STACKTRACE

### 7.1 Runtime Stack Trace Capture

`std::stacktrace` provides runtime stack trace information.

#### Basic Stacktrace

```cpp
#include <stacktrace>
#include <print>
#include <iostream>

void deep_function() {
    // Capture current stack trace
    auto trace = std::stacktrace::current();
    
    // Print trace
    std::println("Stack trace:");
    for (const auto& entry : trace) {
        std::println("  {}", entry);
    }
}

void middle_function() {
    deep_function();
}

void top_function() {
    middle_function();
}

int main() {
    top_function();
    return 0;
}

// Output:
// Stack trace:
//   top_function()
//   middle_function()
//   deep_function()
```


#### Stacktrace in Error Handling

```cpp
#include <stacktrace>
#include <exception>

class TracedException : public std::exception {
    std::stacktrace trace;
    std::string message;
    
public:
    TracedException(const std::string& msg) 
        : trace(std::stacktrace::current()), message(msg) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    const std::stacktrace& get_trace() const {
        return trace;
    }
};

// Usage
void operation() {
    if (error_condition) {
        throw TracedException("Operation failed");
    }
}

try {
    operation();
} catch (const TracedException& e) {
    std::println("Error: {}", e.what());
    std::println("Trace: {}", e.get_trace());
}
```


***

## CONSTEXPR ENHANCEMENTS

### 8.1 More Compile-Time Capabilities

#### constexpr std::string

```cpp
#include <string>

// C++23: Can use std::string in constexpr context
constexpr std::string concat(const char* a, const char* b) {
    std::string result = a;
    result += b;
    return result;
}

constexpr auto msg = concat("Hello", " World");
// msg = "Hello World" at compile-time
```


#### constexpr vector Operations

```cpp
#include <vector>

// C++23: vector works in constexpr
constexpr std::vector<int> make_sequence(int n) {
    std::vector<int> result;
    for (int i = 0; i < n; i++) {
        result.push_back(i);
    }
    return result;
}

constexpr auto seq = make_sequence(5);
// seq = {0, 1, 2, 3, 4} at compile-time
```


#### constexpr Algorithms

```cpp
#include <algorithm>

// C++23: Algorithms work in constexpr
constexpr int compute() {
    std::vector<int> v{3, 1, 4, 1, 5, 9};
    std::sort(v.begin(), v.end());
    int sum = 0;
    for (int x : v) sum += x;
    return sum;
}

constexpr int result = compute();  // Computed at compile-time
```


***

## ADAPTOR IMPROVEMENTS

### 9.1 Ranges::to Conversion

`std::ranges::to` converts ranges to containers.

#### Basic ranges::to

```cpp
#include <ranges>
#include <vector>
#include <string>

vector<int> v = {1, 2, 3, 4, 5};

// Convert filtered range to vector
auto evens = v
    | std::ranges::views::filter([](int x) { return x % 2 == 0; })
    | std::ranges::to<std::vector>();

// Or with map
map<int, int> m{{1, 10}, {2, 20}, {3, 30}};
auto keys = m
    | std::ranges::views::keys
    | std::ranges::to<std::vector>();
```


#### ranges::to with Construction Args

```cpp
#include <ranges>
#include <set>

vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

// Convert to sorted unique set
auto unique_sorted = v
    | std::ranges::to<std::set>();

// Or to deque
auto d = v | std::ranges::to<std::deque>();

// With custom construction
auto s = v
    | std::ranges::to<std::set>(std::greater{});  // Descending
```


***

## LIBRARY IMPROVEMENTS

### 10.1 Utility Improvements

#### std::out_ptr & std::inout_ptr

```cpp
#include <memory>

// For converting unique_ptr to C API
unique_ptr<int> ptr;

// Before C++23: Complicated
// int* tmp = ptr.release();
// legacy_function(&tmp);
// ptr.reset(tmp);

// C++23: Simple
legacy_c_function(std::out_ptr(ptr));

// For bidirectional
inout_ptr(ptr);
```


#### std::move_iterator Improvements

```cpp
#include <iterator>
#include <algorithm>

vector<string> v = {"a", "b", "c"};

// C++23: Cleaner move semantics
auto result = v
    | std::views::transform([](auto& s) { return std::move(s); });
```


#### Bit Manipulation Improvements

```cpp
#include <bit>

unsigned int x = 5;  // 0b0101

// C++23 additions
std::byteswap(x);              // Byte swap
std::has_single_bit(x);        // Check if power of 2
std::bit_width(x);             // Bits needed
std::popcount(x);              // Count 1 bits
```


***

## ATTRIBUTES & FEATURES

### 11.1 [[assume]] Attribute

`[[assume]]` allows providing hints to optimizer.

#### Using assume

```cpp
void process(int x) {
    // Tell compiler to assume x > 0 (for optimization)
    [[assume(x > 0)]];
    
    // Compiler can optimize based on this
    if (x < 0) {
        // This branch won't be taken
        std::cout << "Negative\n";
    }
}

// Useful for performance-critical code
int* find_first(int* arr, int size) {
    [[assume(size > 0)]];  // Array has elements
    
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) return &arr[i];
    }
    return nullptr;
}
```


### 11.2 [[stdcall]] and ABI Attributes

```cpp
// Platform-specific calling conventions
#ifdef _WIN32
void __stdcall legacy_function() { }
[[gnu::stdcall]] void c_function();
#endif
```

***

## STANDARD LIBRARY ADDITIONS

### 12.1 Container & Utility Additions

#### std::debug_assert (Conditional Assertion)

```cpp
void function(int x) {
    // Debug assertion (disabled in release)
    _ASSERT(x > 0, "x must be positive");
    
    // Work with x
}
```


#### std::repeat_view

```cpp
#include <ranges>

// Repeat a value
auto repeated = std::views::repeat(42, 5);
for (int x : repeated) {
    cout << x << " ";  // 42 42 42 42 42
}
```


#### std::stride_view

```cpp
#include <ranges>

vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

// Take every Nth element
auto every_other = v | std::views::stride(2);
for (int x : every_other) {
    cout << x << " ";  // 0 2 4 6 8
}
```


#### std::chunk_by

```cpp
#include <ranges>

vector<int> v = {1, 2, 2, 3, 3, 3, 4, 4};

// Group by predicate
auto chunks = v | std::views::chunk_by(std::equal_to{});
for (auto chunk : chunks) {
    cout << "[";
    for (int x : chunk) cout << x << " ";
    cout << "] ";
}
// Output: [1 ] [2 2 ] [3 3 3 ] [4 4 ]
```


#### std::flat_map and std::flat_set

`std::flat_map` is a container adaptor that stores elements in sorted order in contiguous memory (like a vector).

```cpp
#include <flat_map>
#include <iostream>
#include <string>
#include <vector>

int main() {
    // Stores keys and values in separate vectors
    // Cache-friendly, fast iteration, binary search
    std::flat_map<int, std::string> map;
    
    map[1] = "one";
    map[3] = "three";
    map[2] = "two";  // Inserted in correct sorted position
    
    for (const auto& [key, val] : map) {
        std::cout << key << ": " << val << "\n";
    }
    // Output: 1: one, 2: two, 3: three
    
    return 0;
}
```


#### std::generator (Synchronous Coroutine)

`std::generator` is the standard coroutine generator for synchronous sequences.

```cpp
#include <generator>
#include <iostream>
#include <ranges>

std::generator<int> fib(int n) {
    int a = 0, b = 1;
    while (n-- > 0) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    for (int x : fib(10)) {
        std::cout << x << " ";
    }
    // Output: 0 1 1 2 3 5 8 13 21 34
    
    // Composable with ranges
    auto gen = fib(100) | std::views::filter([](int x) { return x % 2 == 0; });
    
    return 0;
}
```


#### 12.3 Generator Internals

`std::generator` is a coroutine that:
1.  **Suspends on yield**: `co_yield` suspends execution and returns value to caller.
2.  **Promise Type**: Handles the `yield_value` call.
3.  **Recursive**: Can `co_yield` another generator (unlike basic coroutines).

```cpp
// Pseudocode of how it works
struct promise_type {
    int current_value;
    std::suspend_always yield_value(int value) {
        current_value = value;
        return {}; // Suspend
    }
};
```


***


## C++23 BEST PRACTICES

### What's Better with C++23

```cpp
// 1. Use std::print for simple output
std::println("Value: {}", value);

// 2. Use std::expected for error handling
expected<int, Error> result = operation();

// 3. Use deducing this for simpler overloads
auto get(this auto& self) { return self.value; }

// 4. Use range-based for with init
for (auto data = load_data(); auto item : data) { }

// 5. Use multi-index subscript
matrix[row, col] = value;

// 6. Use std::stacktrace for debugging
auto trace = std::stacktrace::current();

// 7. Use constexpr string/vector
constexpr auto msg = std::string("hello");

// 8. Use std::ranges::to for conversions
auto set_result = v | std::ranges::to<std::set>();

// 9. Use [[assume]] for optimization hints
[[assume(pointer != nullptr)]];

// 10. Use monadic operations on optional
opt.transform([](int x) { return x * 2; });
```


***


### <a name="chapter-13-thefuturec26preview"></a>CHAPTER 13: THE FUTURE - C++26 PREVIEW

As of 2026, the C++26 standard is nearing finalization. Here are the transformative features likely to be included.

#### 13.1 Static Reflection (std::meta)
Reflection allows a program to inspect and modify itself at compile-time. This eliminates the need for external code generators or macros for serialization, ORMs, and enum-to-string conversions.

```cpp
#include <meta>
#include <iostream>
#include <string_view>

struct Person {
    std::string name;
    int age;
    double salary;
};

// Generic serialization using C++26 Reflection
template<typename T>
void serialize(const T& obj) {
    constexpr auto type_info = ^T; // Reflection operator
    
    template for (constexpr auto member : std::meta::members_of(type_info)) {
        std::cout << std::meta::name_of(member) << ": " 
                  << obj.[:member:] << "\n"; // Splicing
    }
}

int main() {
    Person p{"Alice", 30, 95000.0};
    serialize(p); 
    // Output:
    // name: Alice
    // age: 30
    // salary: 95000
}
```


#### 13.2 Contracts
Contracts provide a standardized way to specify preconditions, postconditions, and assertions, improving safety and optimizer information.

```cpp
// pre: Precondition (Caller must ensure)
// post: Postcondition (Function ensures upon return)
// assert: Internal check

int safe_divide(int a, int b) 
    pre { b != 0 }             // Contract: b must not be zero
    post(r) { r * b == a }     // Contract: result * divisor equals dividend
{
    return a / b;
}

// Modes:
// - enforce: Terminate if violated
// - observe: Log/Debug but continue
// - ignore: Optimizer hint (assume true)
```




#### 13.3 Senders & Receivers (std::execution)
A unified framework for asynchronous execution, replacing raw threads, futures, and callbacks with a composable pipeline model.

```cpp
#include <execution>
#include <iostream>

using namespace std::execution;

int main() {
    scheduler auto sch = thread_pool_scheduler{};

    sender auto work = schedule(sch)
        | then([]{ return 42; })
        | then([](int i){ return i * 2; })
        | then([](int i){ std::cout << "Result: " << i << "\n"; });

    // Launch execution
    std::this_thread::sync_wait(std::move(work));
    
    return 0;
}
```


#### 13.4 Linear Algebra (std::linalg)
Standardized BLAS (Basic Linear Algebra Subprograms) support for high-performance math.

```cpp
#include <linalg>
#include <mdspan>
#include <vector>

int main() {
    std::vector<double> A_vec(9), B_vec(3), C_vec(3);
    // ... fill vectors ...

    std::mdspan A(A_vec.data(), 3, 3);
    std::mdspan B(B_vec.data(), 3);
    std::mdspan C(C_vec.data(), 3);

    // Matrix-Vector Multiplication: C = A * B
    std::linalg::matrix_vector_product(A, B, C);
    
    return 0;
}
```


***

## Volume IV: Systems & Architecture


***
#### Professional Insights: Low Level & Safety

##### Bit Operators

Section 5.1: | - bitwise OR
int a = 5;     // 0101b  (0x05)
int b = 12;    // 1100b  (0x0C)
int c = a | b; // 1101b  (0x0D)
std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
Output
a = 5, b = 12, c = 13
Why
A bit wise OR operates on the bit level and uses the following Boolean truth table:
true OR true = true
true OR false = true
false OR false = false
When the binary value for a (0101) and the binary value for b (1100) are OR'ed together we get the binary value of
1101:
int a = 0 1 0 1
int b = 1 1 0 0 |
        *********
int c = 1 1 0 1
The bit wise OR does not change the value of the original values unless speciﬁcally assigned to using the bit wise
assignment compound operator |=:
int a = 5;  // 0101b  (0x05)
a |= 12;    // a = 0101b | 1101b
Section 5.2: ^ - bitwise XOR (exclusive OR)
int a = 5;     // 0101b  (0x05)
int b = 9;     // 1001b  (0x09)
int c = a ^ b; // 1100b  (0x0C)
std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
Output
a = 5, b = 9, c = 12
Why
A bit wise XOR (exclusive or) operates on the bit level and uses the following Boolean truth table:
true OR true = false
true OR false = true
false OR false = false
Notice that with an XOR operation true OR true = false where as with operations true AND/OR true = true,
hence the exclusive nature of the XOR operation.
Using this, when the binary value for a (0101) and the binary value for b (1001) are XOR'ed together we get the binary
value of 1100:
int a = 0 1 0 1
int b = 1 0 0 1 ^
        *********
int c = 1 1 0 0
The bit wise XOR does not change the value of the original values unless speciﬁcally assigned to using the bit wise
assignment compound operator ^=:
int a = 5;  // 0101b  (0x05)
a ^= 9;    // a = 0101b ^ 1001b
The bit wise XOR can be utilized in many ways and is often utilized in bit mask operations for encryption and
compression.
Note: The following example is often shown as an example of a nice trick. But should not be used in production
code (there are better ways std::swap() to achieve the same result).
You can also utilize an XOR operation to swap two variables without a temporary:
int a = 42;
int b = 64;
// XOR swap
a ^= b;
b ^= a;
a ^= b;
std::cout << "a = " << a << ", b = " << b << "\n";
To productionalize this you need to add a check to make sure it can be used.
void doXORSwap(int& a, int& b)
{
    // Need to add a check to make sure you are not swapping the same
    // variable with itself. Otherwise it will zero the value.
    if (&a != &b)
    {
        // XOR swap
        a ^= b;
        b ^= a;
        a ^= b;
    }
}
So though it looks like a nice trick in isolation it is not useful in real code. xor is not a base logical operation,but a
combination of others: a^c=~(a&c)&(a|c)
also in 2015+ compilers variables may be assigned as binary:
int cn=0b0111;
Section 5.3: & - bitwise AND
int a = 6;     // 0110b  (0x06)
int b = 10;    // 1010b  (0x0A)
int c = a & b; // 0010b  (0x02)
std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
Output
a = 6, b = 10, c = 2
Why
A bit wise AND operates on the bit level and uses the following Boolean truth table:
TRUE  AND TRUE  = TRUE
TRUE  AND FALSE = FALSE
FALSE AND FALSE = FALSE
When the binary value for a (0110) and the binary value for b (1010) are AND'ed together we get the binary value of
0010:
int a = 0 1 1 0
int b = 1 0 1 0 &
        *********
int c = 0 0 1 0
The bit wise AND does not change the value of the original values unless speciﬁcally assigned to using the bit wise
assignment compound operator &=:
int a = 5;  // 0101b  (0x05)
a &= 10;    // a = 0101b & 1010b
Section 5.4: << - left shift
int a = 1;      // 0001b
int b = a << 1; // 0010b
std::cout << "a = " << a << ", b = " << b << std::endl;
Output
a = 1, b = 2
Why
The left bit wise shift will shift the bits of the left hand value (a) the number speciﬁed on the right (1), essentially
padding the least signiﬁcant bits with 0's, so shifting the value of 5 (binary 0000 0101) to the left 4 times (e.g. 5 <<
4) will yield the value of 80 (binary 0101 0000). You might note that shifting a value to the left 1 time is also the same
as multiplying the value by 2, example:
int a = 7;
while (a < 200) {
    std::cout << "a = " << a << std::endl;
    a <<= 1;
}
a = 7;
while (a < 200) {
    std::cout << "a = " << a << std::endl;
    a *= 2;
}
But it should be noted that the left shift operation will shift all bits to the left, including the sign bit, example:
int a = 2147483647; // 0111 1111 1111 1111 1111 1111 1111 1111
int b = a << 1;     // 1111 1111 1111 1111 1111 1111 1111 1110
std::cout << "a = " << a << ", b = " << b << std::endl;
Possible output: a = 2147483647, b = -2
While some compilers will yield results that seem expected, it should be noted that if you left shift a signed number
so that the sign bit is aﬀected, the result is undeﬁned. It is also undeﬁned if the number of bits you wish to shift by
is a negative number or is larger than the number of bits the type on the left can hold, example:
int a = 1;
int b = a << -1;  // undefined behavior
char c = a << 20; // undefined behavior
The bit wise left shift does not change the value of the original values unless speciﬁcally assigned to using the bit
wise assignment compound operator <<=:
int a = 5;  // 0101b
a <<= 1;    // a = a << 1;
Section 5.5: >> - right shift
int a = 2;      // 0010b
int b = a >> 1; // 0001b
std::cout << "a = " << a << ", b = " << b << std::endl;
Output
a = 2, b = 1
Why
The right bit wise shift will shift the bits of the left hand value (a) the number speciﬁed on the right (1); it should be
noted that while the operation of a right shift is standard, what happens to the bits of a right shift on a signed
negative number is implementation deﬁned and thus cannot be guaranteed to be portable, example:
int a = -2;    
int b = a >> 1; // the value of b will be depend on the compiler
It is also undeﬁned if the number of bits you wish to shift by is a negative number, example:
int a = 1;
int b = a >> -1;  // undefined behavior
The bit wise right shift does not change the value of the original values unless speciﬁcally assigned to using the bit
wise assignment compound operator >>=:
int a = 2;  // 0010b
a >>= 1;    // a = a >> 1;

##### Bit Manipulation

Section 6.1: Remove rightmost set bit
C-style bit-manipulation
template <typename T>
T rightmostSetBitRemoved(T n)
{
    // static_assert(std::is_integral<T>::value && !std::is_signed<T>::value, "type should be
unsigned"); // For c++11 and later
    return n & (n - 1);
}
Explanation
if n is zero, we have 0 & 0xFF..FF which is zero
else n can be written 0bxxxxxx10..00 and n - 1 is 0bxxxxxx011..11, so n & (n - 1) is 0bxxxxxx000..00.
Section 6.2: Set all bits
C-style bit-manipulation
x = -1; // -1 == 1111 1111 ... 1111b
(See here for an explanation of why this works and is actually the best approach.)
Using std::bitset
std::bitset<10> x;
x.set(); // Sets all bits to '1'
Section 6.3: Toggling a bit
C-style bit-manipulation
A bit can be toggled using the XOR operator (^).
// Bit x will be the opposite value of what it is currently
number ^= 1LL << x;
Using std::bitset
std::bitset<4> num(std::string("0100"));
num.flip(2); // num is now 0000
num.flip(0); // num is now 0001
num.flip();  // num is now 1110 (flips all bits)
Section 6.4: Checking a bit
C-style bit-manipulation
The value of the bit can be obtained by shifting the number to the right x times and then performing bitwise AND
(&) on it:
(number >> x) & 1LL;  // 1 if the 'x'th bit of 'number' is set, 0 otherwise
The right-shift operation may be implemented as either an arithmetic (signed) shift or a logical (unsigned) shift. If
number in the expression number >> x has a signed type and a negative value, the resulting value is
implementation-deﬁned.
If we need the value of that bit directly in-place, we could instead left shift the mask:
(number & (1LL << x));  // (1 << x) if the 'x'th bit of 'number' is set, 0 otherwise
Either can be used as a conditional, since all non-zero values are considered true.
Using std::bitset
std::bitset<4> num(std::string("0010"));
bool bit_val = num.test(1);  // bit_val value is set to true;
Section 6.5: Counting bits set
The population count of a bitstring is often needed in cryptography and other applications and the problem has
been widely studied.
The naive way requires one iteration per bit:
unsigned value = 1234;
unsigned bits = 0;  // accumulates the total number of bits set in `n`
for (bits = 0; value; value >>= 1)
  bits += value & 1;
A nice trick (based on Remove rightmost set bit ) is:
unsigned bits = 0;  // accumulates the total number of bits set in `n`
for (; value; ++bits)
  value &= value - 1;
It goes through as many iterations as there are set bits, so it's good when value is expected to have few nonzero
bits.
The method was ﬁrst proposed by Peter Wegner (in CACM 3 / 322 - 1960) and it's well known since it appears in C
Programming Language by Brian W. Kernighan and Dennis M. Ritchie.
This requires 12 arithmetic operations, one of which is a multication:
unsigned popcount(std::uint64_t x)
{
  const std::uint64_t m1  = 0x5555555555555555;  // binary: 0101...
  const std::uint64_t m2  = 0x3333333333333333;  // binary: 00110011..
  const std::uint64_t m4  = 0x0f0f0f0f0f0f0f0f;  // binary: 0000111100001111
  x -= (x >> 1) & m1;             // put count of each 2 bits into those 2 bits
  x = (x & m2) + ((x >> 2) & m2); // put count of each 4 bits into those 4 bits
  x = (x + (x >> 4)) & m4;        // put count of each 8 bits into those 8 bits
  return (x * h01) >> 56;  // left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}
This kind of implementation has the best worst-case behavior (see Hamming weight for further details).
Many CPUs have a speciﬁc instruction (like x86's popcnt) and the compiler could oﬀer a speciﬁc (non standard)
built in function. E.g. with g++ there is:
int __builtin_popcount (unsigned x);
Section 6.6: Check if an integer is a power of 2
The n & (n - 1) trick (see Remove rightmost set bit) is also useful to determine if an integer is a power of 2:
bool power_of_2 = n && !(n & (n - 1));
Note that without the ﬁrst part of the check (n &&), 0 is incorrectly considered a power of 2.
Section 6.7: Setting a bit
C-style bit manipulation
A bit can be set using the bitwise OR operator (|).
// Bit x will be set
number |= 1LL << x;
Using std::bitset
set(x) or set(x,true) - sets bit at position x to 1.
std::bitset<5> num(std::string("01100"));
num.set(0);      // num is now 01101
num.set(2);      // num is still 01101
num.set(4,true); // num is now 11110
Section 6.8: Clearing a bit
C-style bit-manipulation
A bit can be cleared using the bitwise AND operator (&).
// Bit x will be cleared
number &= ~(1LL << x);
Using std::bitset
reset(x) or set(x,false) - clears the bit at position x.
std::bitset<5> num(std::string("01100"));
num.reset(2);     // num is now 01000
num.reset(0);     // num is still 01000
num.set(3,false); // num is now 00000
Section 6.9: Changing the nth bit to x
C-style bit-manipulation
// Bit n will be set if x is 1 and cleared if x is 0.
number ^= (-x ^ number) & (1LL << n);
Using std::bitset
set(n,val) - sets bit n to the value val.
std::bitset<5> num(std::string("00100"));
num.set(0,true);  // num is now 00101
num.set(2,false); // num is now 00001
Section 6.10: Bit Manipulation Application: Small to Capital
Letter
One of several applications of bit manipulation is converting a letter from small to capital or vice versa by choosing
a mask and a proper bit operation. For example, the a letter has this binary representation 01(1)00001 while its
capital counterpart has 01(0)00001. They diﬀer solely in the bit in parenthesis. In this case, converting the a letter
from small to capital is basically setting the bit in parenthesis to one. To do so, we do the following:
/****************************************
convert small letter to captial letter.
========================================
     a: 01100001
  mask: 11011111  <-- (0xDF)  11(0)11111
      :---------
a&mask: 01000001  <-- A letter
*****************************************/
The code for converting a letter to A letter is
#include <cstdio>
int main()
{
    char op1 = 'a';  // "a" letter (i.e. small case)
    int mask = 0xDF; // choosing a proper mask
    printf("a (AND) mask = A\n");
    printf("%c   &   0xDF = %c\n", op1, op1 & mask);
    return 0;
}
The result is
$ g++ main.cpp -o test1
$ ./test1
a (AND) mask = A
a   &   0xDF = A

##### Undeﬁned Behavior

Section 104.1: Reading or writing through a null pointer 
Section 104.2: Using an uninitialized local variable 
Section 104.3: Accessing an out-of-bounds index 
Section 104.4: Deleting a derived object via a pointer to a base class that doesn't have a virtual destructor
Section 104.5: Extending the `std` or `posix` Namespace 
Section 104.6: Invalid pointer arithmetic 
Section 104.7: No return statement for a function with a non-void return type 
Section 104.8: Accessing a dangling reference 
Section 104.9: Integer division by zero 
Section 104.10: Shifting by an invalid number of positions 
Section 104.11: Incorrect pairing of memory allocation and deallocation 
Section 104.12: Signed Integer Overﬂow 
Section 104.13: Multiple non-identical deﬁnitions (the One Deﬁnition Rule) 
Section 104.14: Modifying a const object 
Section 104.15: Returning from a [[noreturn]] function 
Section 104.16: Inﬁnite template recursion 
Section 104.17: Overﬂow during conversion to or from ﬂoating point type 
Section 104.18: Modifying a string literal 
Section 104.19: Accessing an object as the wrong type 
Section 104.20: Invalid derived-to-base conversion for pointers to members 
Section 104.21: Destroying an object that has already been destroyed 
Section 104.22: Access to nonexistent member through pointer to member 
Section 104.23: Invalid base-to-derived static cast 
Section 104.24: Floating point overﬂow 
Section 104.25: Calling (Pure) Virtual Members From Constructor Or Destructor 
Section 104.26: Function call through mismatched function pointer type 

### <a name="chapter-14-advancedtopics"></a>CHAPTER 14: ADVANCED TOPICS


***
#### Professional Insights: Exception Handling Mastery

##### Exceptions

Section 72.1: Catching exceptions
A try/catch block is used to catch exceptions. The code in the try section is the code that may throw an exception,
and the code in the catch clause(s) handles the exception.
#include <iostream>
#include <string>
#include <stdexcept>
int main() {
  std::string str("foo");
  try {
      str.at(10); // access element, may throw std::out_of_range
  } catch (const std::out_of_range& e) {
      // what() is inherited from std::exception and contains an explanatory message
      std::cout << e.what();
  }
}
Multiple catch clauses may be used to handle multiple exception types. If multiple catch clauses are present, the
exception handling mechanism tries to match them in order of their appearance in the code:
std::string str("foo");
try {
    str.reserve(2); // reserve extra capacity, may throw std::length_error
    str.at(10); // access element, may throw std::out_of_range
} catch (const std::length_error& e) {
    std::cout << e.what();
} catch (const std::out_of_range& e) {
    std::cout << e.what();
}
Exception classes which are derived from a common base class can be caught with a single catch clause for the
common base class. The above example can replace the two catch clauses for std::length_error and
std::out_of_range with a single clause for std:exception:
std::string str("foo");
try {
    str.reserve(2); // reserve extra capacity, may throw std::length_error
    str.at(10); // access element, may throw std::out_of_range
} catch (const std::exception& e) {
    std::cout << e.what();
}
Because the catch clauses are tried in order, be sure to write more speciﬁc catch clauses ﬁrst, otherwise your
exception handling code might never get called:
try {
    /* Code throwing exceptions omitted. */
} catch (const std::exception& e) {
    /* Handle all exceptions of type std::exception. */
} catch (const std::runtime_error& e) {
    /* This block of code will never execute, because std::runtime_error inherits
       from std::exception, and all exceptions of type std::exception were already
       caught by the previous catch clause. */
}
Another possibility is the catch-all handler, which will catch any thrown object:
try {
    throw 10;
} catch (...) {
    std::cout << "caught an exception";
}
Section 72.2: Rethrow (propagate) exception
Sometimes you want to do something with the exception you catch (like write to log or print a warning) and let it
bubble up to the upper scope to be handled. To do so, you can rethrow any exception you catch:
try {
    ... // some code here
} catch (const SomeException& e) {
    std::cout << "caught an exception";
    throw;
}
Using throw; without arguments will re-throw the currently caught exception.
Version ≥ C++11
To rethrow a managed std::exception_ptr, the C++ Standard Library has the rethrow_exception function that
can be used by including the <exception> header in your program.
#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
void handle_eptr(std::exception_ptr eptr) // passing by value is ok
{
    try {
        if (eptr) {
            std::rethrow_exception(eptr);
        }
    } catch(const std::exception& e) {
        std::cout << "Caught exception \"" << e.what() << "\"\n";
    }
}
int main()
{
    std::exception_ptr eptr;
    try {
        std::string().at(1); // this generates an std::out_of_range
    } catch(...) {
        eptr = std::current_exception(); // capture
    }
    handle_eptr(eptr);
} // destructor for std::out_of_range called here, when the eptr is destructed
Section 72.3: Best practice: throw by value, catch by const
reference
In general, it is considered good practice to throw by value (rather than by pointer), but catch by (const) reference.
try {
    // throw new std::runtime_error("Error!");   // Don't do this!
    // This creates an exception object
    // on the heap and would require you to catch the
    // pointer and manage the memory yourself. This can
    // cause memory leaks!
    throw std::runtime_error("Error!");
} catch (const std::runtime_error& e) {
    std::cout << e.what() << std::endl;
}
One reason why catching by reference is a good practice is that it eliminates the need to reconstruct the object
when being passed to the catch block (or when propagating through to other catch blocks). Catching by reference
also allows the exceptions to be handled polymorphically and avoids object slicing. However, if you are rethrowing
an exception (like throw e;, see example below), you can still get object slicing because the throw e; statement
makes a copy of the exception as whatever type is declared:
#include <iostream>
struct BaseException {
    virtual const char* what() const { return "BaseException"; }
};
struct DerivedException : BaseException {
    // "virtual" keyword is optional here
    virtual const char* what() const { return "DerivedException"; }
};
int main(int argc, char** argv) {
    try {
        try {
            throw DerivedException();
        } catch (const BaseException& e) {
            std::cout << "First catch block: " << e.what() << std::endl;
            // Output ==> First catch block: DerivedException
            throw e; // This changes the exception to BaseException
                     // instead of the original DerivedException!
        }
    } catch (const BaseException& e) {
        std::cout << "Second catch block: " << e.what() << std::endl;
        // Output ==> Second catch block: BaseException
    }
    return 0;
}
If you are sure that you are not going to do anything to change the exception (like add information or modify the
message), catching by const reference allows the compiler to make optimizations and can improve performance.
But this can still cause object splicing (as seen in the example above).
Warning: Beware of throwing unintended exceptions in catch blocks, especially related to allocating extra memory
or resources. For example, constructing logic_error, runtime_error or their subclasses might throw bad_alloc
due to memory running out when copying the exception string, I/O streams might throw during logging with
respective exception masks set, etc.
Section 72.4: Custom exception
You shouldn't throw raw values as exceptions, instead use one of the standard exception classes or make your
own.
Having your own exception class inherited from std::exception is a good way to go about it. Here's a custom
exception class which directly inherits from std::exception:
#include <exception>
class Except: virtual public std::exception {
protected:
    int error_number;               ///< Error number
    int error_offset;               ///< Error offset
    std::string error_message;      ///< Error message
public:
    /** Constructor (C++ STL string, int, int).
     *  @param msg The error message
     *  @param err_num Error number
     *  @param err_off Error offset
     */
    explicit
    Except(const std::string& msg, int err_num, int err_off):
        error_number(err_num),
        error_offset(err_off),
        error_message(msg)
        {}
    /** Destructor.
     *  Virtual to allow for subclassing.
     */
    virtual ~Except() throw () {}
    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *  is in possession of the Except object. Callers must
     *  not attempt to free the memory.
     */
    virtual const char* what() const throw () {
       return error_message.c_str();
    }
    /** Returns error number.
     *  @return #error_number
     */
    virtual int getErrorNumber() const throw() {
        return error_number;
    }
    /**Returns error offset.
     * @return #error_offset
     */
    virtual int getErrorOffset() const throw() {
        return error_offset;
    }
};
An example throw catch:
try {
    throw(Except("Couldn't do what you were expecting", -12, -34));
} catch (const Except& e) {
    std::cout<<e.what()
             <<"\nError number: "<<e.getErrorNumber()
             <<"\nError offset: "<<e.getErrorOffset();
}
As you are not only just throwing a dumb error message, also some other values representing what the error
exactly was, your error handling becomes much more eﬃcient and meaningful.
There's an exception class that let's you handle error messages nicely :std::runtime_error
You can inherit from this class too:
#include <stdexcept>
class Except: virtual public std::runtime_error {
protected:
    int error_number;               ///< Error number
    int error_offset;               ///< Error offset
public:
    /** Constructor (C++ STL string, int, int).
     *  @param msg The error message
     *  @param err_num Error number
     *  @param err_off Error offset
     */
    explicit
    Except(const std::string& msg, int err_num, int err_off):
        std::runtime_error(msg)
        {
            error_number = err_num;
            error_offset = err_off;
        }
    /** Destructor.
     *  Virtual to allow for subclassing.
     */
    virtual ~Except() throw () {}
    /** Returns error number.
     *  @return #error_number
     */
    virtual int getErrorNumber() const throw() {
        return error_number;
    }
    /**Returns error offset.
     * @return #error_offset
     */
    virtual int getErrorOffset() const throw() {
        return error_offset;
    }
};
Note that I haven't overridden the what() function from the base class (std::runtime_error) i.e we will be using
the base class's version of what(). You can override it if you have further agenda.
Section 72.5: std::uncaught_exceptions
Version ≥ c++17
C++17 introduces int std::uncaught_exceptions() (to replace the limited bool std::uncaught_exception()) to
know how many exceptions are currently uncaught. That allows for a class to determine if it is destroyed during a
stack unwinding or not.
#include <exception>
#include <string>
#include <iostream>
// Apply change on destruction:
// Rollback in case of exception (failure)
// Else Commit (success)
class Transaction
{
public:
    Transaction(const std::string& s) : message(s) {}
    Transaction(const Transaction&) = delete;
    Transaction& operator =(const Transaction&) = delete;
    void Commit() { std::cout << message << ": Commit\n"; }
    void RollBack() noexcept(true) { std::cout << message << ": Rollback\n"; }
    // ...
    ~Transaction() {
        if (uncaughtExceptionCount == std::uncaught_exceptions()) {
            Commit(); // May throw.
        } else { // current stack unwinding
            RollBack();
        }
    }
private:
    std::string message;
    int uncaughtExceptionCount = std::uncaught_exceptions();
};
class Foo
{
public:
    ~Foo() {
        try {
            Transaction transaction("In ~Foo"); // Commit,
                                            // even if there is an uncaught exception
            //...
        } catch (const std::exception& e) {
            std::cerr << "exception/~Foo:" << e.what() << std::endl;
        }
    }
};
int main()
{
    try {
        Transaction transaction("In main"); // RollBack
        Foo foo; // ~Foo commit its transaction.
        //...
        throw std::runtime_error("Error");
    } catch (const std::exception& e) {
        std::cerr << "exception/main:" << e.what() << std::endl;
    }
}
Output:
In ~Foo: Commit
In main: Rollback
exception/main:Error
Section 72.6: Function Try Block for regular function
void function_with_try_block()
try
{
    // try block body
}
catch (...)
{
    // catch block body
}
Which is equivalent to
void function_with_try_block()
{
    try
    {
        // try block body
    }
    catch (...)
    {
        // catch block body
    }
}
Note that for constructors and destructors, the behavior is diﬀerent as the catch block re-throws an exception
anyway (the caught one if there is no other throw in the catch block body).
The function main is allowed to have a function try block like any other function, but main's function try block will
not catch exceptions that occur during the construction of a non-local static variable or the destruction of any static
variable. Instead, std::terminate is called.
Section 72.7: Nested exception
Version ≥ C++11
During exception handling there is a common use case when you catch a generic exception from a low-level
function (such as a ﬁlesystem error or data transfer error) and throw a more speciﬁc high-level exception which
indicates that some high-level operation could not be performed (such as being unable to publish a photo on Web).
This allows exception handling to react to speciﬁc problems with high level operations and also allows, having only
error an message, the programmer to ﬁnd a place in the application where an exception occurred. Downside of this
solution is that exception callstack is truncated and original exception is lost. This forces developers to manually
include text of original exception into a newly created one.
Nested exceptions aim to solve the problem by attaching low-level exception, which describes the cause, to a high
level exception, which describes what it means in this particular case.
std::nested_exception allows to nest exceptions thanks to std::throw_with_nested:
#include <stdexcept>
#include <exception>
#include <string>
#include <fstream>
#include <iostream>
struct MyException
{
    MyException(const std::string& message) : message(message) {}
    std::string message;
};
void print_current_exception(int level)
{
    try {
        throw;
    } catch (const std::exception& e) {
        std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    } catch (const MyException& e) {
        std::cerr << std::string(level, ' ') << "MyException: " << e.message << '\n';
    } catch (...) {
        std::cerr << "Unkown exception\n";
    }
}
void print_current_exception_with_nested(int level =  0)
{
    try {
        throw;
    } catch (...) {
        print_current_exception(level);
    }    
    try {
        throw;
    } catch (const std::nested_exception& nested) {
        try {
            nested.rethrow_nested();
        } catch (...) {
            print_current_exception_with_nested(level + 1); // recursion
        }
    } catch (...) {
        //Empty // End recursion
    }
}
// sample function that catches an exception and wraps it in a nested exception
void open_file(const std::string& s)
{
    try {
        std::ifstream file(s);
        file.exceptions(std::ios_base::failbit);
    } catch(...) {
        std::throw_with_nested(MyException{"Couldn't open " + s});
    }
}
// sample function that catches an exception and wraps it in a nested exception
void run()
{
    try {
        open_file("nonexistent.file");
    } catch(...) {
        std::throw_with_nested( std::runtime_error("run() failed") );
    }
}
// runs the sample function above and prints the caught exception
int main()
{
    try {
        run();
    } catch(...) {
        print_current_exception_with_nested();
    }
}
Possible output:
exception: run() failed
 MyException: Couldn't open nonexistent.file
  exception: basic_ios::clear
If you work only with exceptions inherited from std::exception, code can even be simpliﬁed.
Section 72.8: Function Try Blocks In constructor
The only way to catch exception in initializer list:
struct A : public B
{
    A() try : B(), foo(1), bar(2)
    {
        // constructor body
    }
    catch (...)
    {
        // exceptions from the initializer list and constructor are caught here
        // if no exception is thrown here
        // then the caught exception is re-thrown.
    }
private:
    Foo foo;
    Bar bar;
};
Section 72.9: Function Try Blocks In destructor
struct A
{
    ~A() noexcept(false) try
    {
        // destructor body
    }
    catch (...)
    {
        // exceptions of destructor body are caught here
        // if no exception is thrown here
        // then the caught exception is re-thrown.
    }
};
Note that, although this is possible, one needs to be very careful with throwing from destructor, as if a destructor
called during stack unwinding throws an exception, std::terminate is called.



***
#### Professional Insights: Exception Handling

##### Exceptions

Section 72.1: Catching exceptions 
Section 72.2: Rethrow (propagate) exception 
Section 72.3: Best practice: throw by value, catch by const reference 
Section 72.4: Custom exception 
Section 72.5: std::uncaught_exceptions 
Section 72.6: Function Try Block for regular function 
Section 72.7: Nested exception 
Section 72.8: Function Try Blocks In constructor 
Section 72.9: Function Try Blocks In destructor 


### TEMPLATE METAPROGRAMMING

### 1.1 Compile-Time Computation

Template metaprogramming enables computation at compile-time.

#### Factorial at Compile-Time

```cpp
#include <iostream>
using namespace std;

// Base case
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N-1>::value;
};

// Specialization (base case)
template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

int arr[Factorial<5>::value];  // Array of size 120!

cout << Factorial<10>::value << "\n";  // 3628800
```


#### Fibonacci at Compile-Time

```cpp
template<int N>
struct Fib {
    static constexpr int value = Fib<N-1>::value + Fib<N-2>::value;
};

template<>
struct Fib<0> {
    static constexpr int value = 0;
};

template<>
struct Fib<1> {
    static constexpr int value = 1;
};

// Memoization to avoid exponential time
template<int N>
struct FibMemo {
    static constexpr int value = FibMemo<N-1>::value + FibMemo<N-2>::value;
};

static constexpr int fib20 = FibMemo<20>::value;  // Computed at compile-time
```


#### Compile-Time Power Check

```cpp
template<unsigned int N>
struct IsPowerOfTwo {
    static constexpr bool value = (N > 0) && ((N & (N - 1)) == 0);
};

static_assert(IsPowerOfTwo<16>::value);
static_assert(!IsPowerOfTwo<7>::value);
```


***

### 1.2 Template Specialization

#### Full Specialization

```cpp
// Primary template
template<typename T, typename U>
struct Pair {
    static void info() { cout << "Generic pair\n"; }
};

// Full specialization
template<>
struct Pair<int, string> {
    static void info() { cout << "int-string pair\n"; }
};

// Full specialization for pointers
template<typename T>
struct Pair<T*, T*> {
    static void info() { cout << "Two pointers\n"; }
};

Pair<int, string>::info();     // "int-string pair"
Pair<double, string>::info();  // "Generic pair"
Pair<int*, int*>::info();      // "Two pointers"
```




#### Partial Specialization

```cpp
// Primary
template<typename T, typename U>
struct Container {
    static void type() { cout << "Generic\n"; }
};

// Partial specialization - same types
template<typename T>
struct Container<T, T> {
    static void type() { cout << "Same type\n"; }
};

// Partial specialization - pointers
template<typename T, typename U>
struct Container<T*, U*> {
    static void type() { cout << "Two pointers\n"; }
};

// Partial specialization - array
template<typename T, int N>
struct Container<T[N], T> {
    static void type() { cout << "Array and element\n"; }
};

Container<int, int>::type();           // "Same type"
Container<int*, double*>::type();      // "Two pointers"
Container<int[5], int>::type();        // "Array and element"
```




### 1.3 Advanced Metaprogramming Patterns

#### Typelists
A list of types at compile-time, essential for ECS and Variant implementation.

```cpp
template<typename... Ts>
struct TypeList {};

// Length of list
template<typename List> struct Length;

template<typename... Ts>
struct Length<TypeList<Ts...>> {
    static constexpr size_t value = sizeof...(Ts);
};

// Access type at index
template<size_t N, typename List> struct At;

template<typename Head, typename... Tail>
struct At<0, TypeList<Head, Tail...>> {
    using type = Head;
};

template<size_t N, typename Head, typename... Tail>
struct At<N, TypeList<Head, Tail...>> {
    using type = typename At<N-1, TypeList<Tail...>>::type;
};

// Usage
using MyTypes = TypeList<int, float, double>;
static_assert(Length<MyTypes>::value == 3);
static_assert(std::is_same_v<At<1, MyTypes>::type, float>);
```


***

## SFINAE & TYPE TRAITS

### 2.1 SFINAE - Substitution Failure Is Not An Error

SFINAE enables overload resolution based on template parameter substitution.

#### Basic SFINAE

```cpp
#include <type_traits>

// Enable if T is integral
template<typename T>
enable_if_t<is_integral_v<T>>
process(T x) {
    cout << "Integer: " << x << "\n";
}

// Enable if T is floating point
template<typename T>
enable_if_t<is_floating_point_v<T>>
process(T x) {
    cout << "Float: " << x << "\n";
}

process(42);      // "Integer: 42"
process(3.14);    // "Float: 3.14"
```


#### Detector Pattern

```cpp
// Detect if type has value_type
template<typename T, typename = void>
struct HasValueType : false_type {};

template<typename T>
struct HasValueType<T, void_t<typename T::value_type>> : true_type {};

// Usage
static_assert(HasValueType<vector<int>>::value);
static_assert(!HasValueType<int>::value);
```




#### Advanced SFINAE with Multiple Conditions

```cpp
template<typename T>
enable_if_t<
    is_copy_constructible_v<T> &&
    is_move_constructible_v<T> &&
    is_equality_comparable_v<T>
>
smart_copy(const T& src, T& dst) {
    dst = src;
}
```


***

### 2.2 Type Traits Mastery

#### Custom Type Traits

```cpp
// Check if type has a begin() and end()
template<typename T, typename = void>
struct IsContainer : false_type {};

template<typename T>
struct IsContainer<T, void_t<
    decltype(declval<T>().begin()),
    decltype(declval<T>().end())
>> : true_type {};

// Check if callable with specific signature
template<typename F, typename... Args>
struct IsCallable : false_type {};

template<typename F, typename... Args>
struct IsCallable<F, 
    void_t<decltype(declval<F>()(declval<Args>()...))>
> : true_type {};

// Usage
static_assert(IsContainer<vector<int>>::value);
static_assert(!IsContainer<int>::value);

auto lambda = [](int x) { return x; };
static_assert(IsCallable<decltype(lambda), int>::value);
```


***


## EXPRESSION TEMPLATES

### 3.1 Lazy Evaluation Pattern

Expression templates defer computation until needed.

#### Vector Operations

```cpp
#include <vector>
#include <iostream>

// Expression template for vector operations
template<typename Expr>
class VectorExpr {
public:
    double operator[](int i) const {
        return static_cast<const Expr&>(*this)[i];
    }
    
    int size() const {
        return static_cast<const Expr&>(*this).size();
    }
};

class Vector : public VectorExpr<Vector> {
private:
    std::vector<double> data;
    
public:
    Vector(int n) : data(n) {}
    
    double operator[](int i) const { return data[i]; }
    int size() const { return data.size(); }
    double& operator[](int i) { return data[i]; }
};

// Addition expression (no computation yet)
template<typename L, typename R>
class VectorSum : public VectorExpr<VectorSum<L, R>> {
private:
    const L& lhs;
    const R& rhs;
    
public:
    VectorSum(const L& l, const R& r) : lhs(l), rhs(r) {}
    
    double operator[](int i) const {
        return lhs[i] + rhs[i];
    }
    
    int size() const { return lhs.size(); }
};

// Operator overloading creates expression tree
template<typename L, typename R>
VectorSum<L, R> operator+(const VectorExpr<L>& l, const VectorExpr<R>& r) {
    return VectorSum<L, R>(static_cast<const L&>(l), static_cast<const R&>(r));
}

// Scalar multiplication
template<typename Expr>
class VectorScale : public VectorExpr<VectorScale<Expr>> {
private:
    const Expr& expr;
    double scale;
    
public:
    VectorScale(const Expr& e, double s) : expr(e), scale(s) {}
    
    double operator[](int i) const {
        return expr[i] * scale;
    }
    
    int size() const { return expr.size(); }
};

template<typename Expr>
VectorScale<Expr> operator*(double s, const VectorExpr<Expr>& e) {
    return VectorScale<Expr>(static_cast<const Expr&>(e), s);
}

// Usage
int main() {
    Vector v1(3), v2(3), v3(3);
    v1[0] = 1; v1[1] = 2; v1[2] = 3;
    v2[0] = 4; v2[1] = 5; v2[2] = 6;
    
    // No computation yet!
    auto expr = v1 + v2 + 2.0 * v3;
    
    // Computation happens here
    for (int i = 0; i < 3; i++) {
        cout << expr[i] << " ";
    }
    
    return 0;
}
```


### 3.2 Triggering Computation (Assignment)

To make `v3 = v1 + v2` work efficiently, we add an assignment operator to `Vector` that takes any `VectorExpr`.

```cpp
// Inside class Vector
template <typename Expr>
Vector& operator=(const VectorExpr<Expr>& expr) {
    if (size() != expr.size()) {
        // resize...
    }
    for (int i = 0; i < size(); ++i) {
        data[i] = expr[i]; // Evaluates tree at index i
    }
    return *this;
}
```

**Why is this fast?**
It expands to: `v3[i] = v1[i] + v2[i]`.
There are **zero temporary vectors** created. No allocations. Just one loop.

***


## POLICY-BASED DESIGN

### 4.1 Template Policies

Policy-based design separates concerns using template parameters.

#### Thread Safety Policy

```cpp
#include <mutex>
#include <memory>

// Policy: No synchronization
struct NoSync {
    void lock() {}
    void unlock() {}
};

// Policy: Mutex-based
struct MutexSync {
private:
    mutable std::mutex m;
    
public:
    void lock() { m.lock(); }
    void unlock() { m.unlock(); }
};

// Generic container with synchronization policy
template<typename T, typename SyncPolicy = NoSync>
class SafeVector : private SyncPolicy {
private:
    std::vector<T> data;
    
public:
    void push_back(const T& value) {
        this->lock();
        data.push_back(value);
        this->unlock();
    }
    
    T pop_back() {
        this->lock();
        T value = data.back();
        data.pop_back();
        this->unlock();
        return value;
    }
};

// Usage
SafeVector<int> single_threaded;  // No synchronization
SafeVector<int, MutexSync> multi_threaded;  // Thread-safe
```


#### Storage Policy

```cpp
// Policy: Dynamic allocation
struct DynamicStorage {
    template<typename T>
    using Allocator = std::allocator<T>;
};

// Policy: Static allocation
template<int MaxSize>
struct StaticStorage {
    // Allocate from stack
};

template<typename T, typename StoragePolicy>
class Container : private StoragePolicy {
private:
    std::vector<T> data;
};
```




### 4.2 Policy-Based Smart Pointer (Advanced Example)

A smart pointer is defined by:
1.  **Storage**: How it stores the pointer (raw vs compressed).
2.  **Ownership**: Ref-counted vs Unique vs Linked.
3.  **Checking**: Assert on access vs No check.

```cpp
template <
    class T,
    template <class> class CheckingPolicy,
    template <class> class ThreadingPolicy
>
class SmartPtr : public CheckingPolicy<T>, public ThreadingPolicy<T> {
    T* ptr;
public:
    T* operator->() {
        CheckingPolicy<T>::check(ptr);
        ThreadingPolicy<T>::lock(); // Fake lock example
        return ptr;
    }
};
```

This approach allows generating thousands of smart pointer variants with zero runtime overhead (all resolved at compile-time).

***

## MEMORY MANAGEMENT & OPTIMIZATION

### 5.1 Custom Allocators

```cpp
#include <memory>

template<typename T>
class ArenaAllocator {
private:
    static constexpr size_t ARENA_SIZE = 10000;
    char arena[ARENA_SIZE];
    char* current;
    
public:
    ArenaAllocator() : current(arena) {}
    
    T* allocate(size_t n) {
        if (current + n * sizeof(T) > arena + ARENA_SIZE) {
            throw std::bad_alloc();
        }
        T* ptr = reinterpret_cast<T*>(current);
        current += n * sizeof(T);
        return ptr;
    }
    
    void deallocate(T* p, size_t n) {
        // No deallocation for arena allocator
    }
};

// Usage
vector<int, ArenaAllocator<int>> fast_vector;
```


### 5.2 Small Object Optimization (SOO)

```cpp
template<typename T, size_t SmallSize = 32>
class SmallVector {
private:
    union {
        T* ptr;              // Dynamic allocation
        std::aligned_storage_t<SmallSize, alignof(T)> small;
    };
    
    size_t size_val;
    bool is_small;
    
public:
    SmallVector() : size_val(0), is_small(true) {}
    
    void push_back(const T& value) {
        if (size_val < SmallSize / sizeof(T)) {
            // Use small storage
            new (&small) T(value);
            is_small = true;
        } else {
            // Switch to dynamic
            if (is_small) {
                // Copy small to dynamic
                ptr = new T[size_val + 1];
                is_small = false;
            }
            ptr[size_val] = value;
        }
        size_val++;
    }
    
    ~SmallVector() {
        if (!is_small) {
            delete[] ptr;
        }
    }
};
```


***

## CONCURRENCY & PARALLELISM

### 6.1 Advanced Threading Patterns

#### Thread Pool

```cpp
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
private:
    vector<std::thread> workers;
    queue<std::function<void()>> tasks;
    mutex task_mutex;
    condition_variable cv;
    bool stop = false;
    
public:
    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; i++) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    
                    {
                        unique_lock<mutex> lock(task_mutex);
                        cv.wait(lock, [this] { return !tasks.empty() || stop; });
                        
                        if (stop && tasks.empty()) return;
                        
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    
                    task();
                }
            });
        }
    }
    
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) {
        using return_type = std::invoke_result_t<F, Args...>;
        
        auto task = make_shared<std::packaged_task<return_type()>>(
            bind(forward<F>(f), forward<Args>(args)...)
        );
        
        auto result = task->get_future();
        
        {
            unique_lock<mutex> lock(task_mutex);
            tasks.emplace([task] { (*task)(); });
        }
        
        cv.notify_one();
        return result;
    }
    
    ~ThreadPool() {
        {
            unique_lock<mutex> lock(task_mutex);
            stop = true;
        }
        cv.notify_all();
        for (auto& worker : workers) {
            worker.join();
        }
    }
};

// Usage
ThreadPool pool(4);

auto future = pool.enqueue([](int x) { return x * 2; }, 42);
cout << future.get() << "\n";  // 84
```


#### Lock-Free Queue

```cpp
#include <atomic>

template<typename T>
class LockFreeQueue {
private:
    struct Node {
        T value;
        atomic<Node*> next{nullptr};
    };
    
    atomic<Node*> head;
    atomic<Node*> tail;
    
public:
    LockFreeQueue() {
        auto dummy = new Node();
        head.store(dummy, memory_order_relaxed);
        tail.store(dummy, memory_order_relaxed);
    }
    
    void push(const T& value) {
        auto new_node = new Node{value};
        Node* old_tail = tail.load(memory_order_acquire);
        
        old_tail->next.store(new_node, memory_order_release);
        tail.store(new_node, memory_order_release);
    }
    
    bool pop(T& value) {
        Node* old_head = head.load(memory_order_acquire);
        Node* next = old_head->next.load(memory_order_acquire);
        
        if (next == nullptr) return false;
        
        value = next->value;
        head.store(next, memory_order_release);
        delete old_head;
        
        return true;
    }
};
```


***

## TYPE ERASURE PATTERNS

### 7.1 Virtual Function-Based Type Erasure

```cpp
class AnyCallable {
private:
    struct Base {
        virtual ~Base() = default;
        virtual void call() = 0;
    };
    
    template<typename F>
    struct Derived : Base {
        F func;
        Derived(F f) : func(f) {}
        void call() override { func(); }
    };
    
    unique_ptr<Base> impl;
    
public:
    template<typename F>
    AnyCallable(F f) : impl(make_unique<Derived<F>>(f)) {}
    
    void operator()() {
        impl->call();
    }
};

// Usage
AnyCallable c1([](){ cout << "Lambda\n"; });
AnyCallable c2(std::bind(...));
AnyCallable c3(&function);

c1();  // "Lambda"
```


### 7.2 std::function Implementation

```cpp
#include <memory>

template<typename>
class Function;

template<typename R, typename... Args>
class Function<R(Args...)> {
private:
    struct Base {
        virtual ~Base() = default;
        virtual R call(Args...) = 0;
        virtual unique_ptr<Base> clone() = 0;
    };
    
    template<typename F>
    struct Derived : Base {
        F func;
        Derived(F f) : func(f) {}
        R call(Args... args) override {
            return func(args...);
        }
        unique_ptr<Base> clone() override {
            return make_unique<Derived>(*this);
        }
    };
    
    unique_ptr<Base> impl;
    
public:
    template<typename F>
    Function(F f) : impl(make_unique<Derived<F>>(f)) {}
    
    R operator()(Args... args) {
        return impl->call(args...);
    }
    
    Function(const Function& other) : impl(other.impl->clone()) {}
};
```


### 7.3 Concept-Based Type Erasure (C++20)

Instead of inheritance, we can erase types that satisfy a concept.

```cpp
#include <concepts>
#include <memory>

template<typename T>
concept Drawable = requires(T x) { x.draw(); };

class AnyDrawable {
    struct Concept {
        virtual ~Concept() = default;
        virtual void draw() = 0;
        virtual std::unique_ptr<Concept> clone() = 0;
    };

    template<Drawable T>
    struct Model : Concept {
        T data;
        Model(T x) : data(std::move(x)) {}
        void draw() override { data.draw(); }
        std::unique_ptr<Concept> clone() override { return std::make_unique<Model>(data); }
    };

    std::unique_ptr<Concept> pimpl;

public:
    template<Drawable T>
    AnyDrawable(T x) : pimpl(std::make_unique<Model<T>>(std::move(x))) {}
    
    AnyDrawable(const AnyDrawable& other) : pimpl(other.pimpl->clone()) {}
    
    void draw() { pimpl->draw(); }
};
```


***

## CRTP (CURIOUSLY RECURRING TEMPLATE PATTERN)

### 8.1 Static Polymorphism

```cpp
// Base class template
template<typename Derived>
class Shape {
public:
    void draw() {
        static_cast<Derived*>(this)->draw_impl();
    }
    
    double area() {
        return static_cast<Derived*>(this)->area_impl();
    }
};

// Derived classes
class Circle : public Shape<Circle> {
private:
    double radius;
    
public:
    Circle(double r) : radius(r) {}
    
    void draw_impl() {
        cout << "Drawing circle\n";
    }
    
    double area_impl() {
        return 3.14159 * radius * radius;
    }
};

class Square : public Shape<Square> {
private:
    double side;
    
public:
    Square(double s) : side(s) {}
    
    void draw_impl() {
        cout << "Drawing square\n";
    }
    
    double area_impl() {
        return side * side;
    }
};

// Usage - no virtual function overhead!
Circle c(5);
c.draw();
cout << c.area() << "\n";

Square s(4);
s.draw();
cout << s.area() << "\n";
```




### 8.2 CRTP for Comparisons

```cpp
template<typename Derived>
class Comparable {
public:
    bool operator<(const Comparable& other) const {
        return static_cast<const Derived*>(this)->compare(
            static_cast<const Derived&>(other)
        ) < 0;
    }
    
    bool operator>(const Comparable& other) const {
        return other < *this;
    }
    
    bool operator<=(const Comparable& other) const {
        return !(other < *this);
    }
    
    bool operator>=(const Comparable& other) const {
        return !(*this < other);
    }
    
    bool operator==(const Comparable& other) const {
        return !(*this < other) && !(other < *this);
    }
    
    bool operator!=(const Comparable& other) const {
        return !(*this == other);
    }
};

class Value : public Comparable<Value> {
private:
    int val;
    
public:
    Value(int v) : val(v) {}
    
    int compare(const Value& other) const {
        return val - other.val;
    }
};

// Usage
Value v1(5), v2(10);
cout << (v1 < v2) << "\n";   // true
cout << (v1 == v2) << "\n";  // false
```


***

## PERFECT FORWARDING & MOVE SEMANTICS

### 9.1 Forwarding Problems

```cpp
// The problem: losing information about lvalue/rvalue
template<typename T>
void bad_forward(T arg) {
    // arg is always lvalue
    sink(arg);  // Loses rvalue status
}

// Solution: universal references + std::forward
template<typename T>
void good_forward(T&& arg) {
    // Preserves lvalue/rvalue nature
    sink(std::forward<T>(arg));
}

// Double forwarding
template<typename T>
void wrapper(T&& arg) {
    process(std::forward<T>(arg));
}

template<typename T>
void double_wrapper(T&& arg) {
    wrapper(std::forward<T>(arg));
}
```




### 9.2 Move Semantics Implementation

```cpp
class String {
private:
    char* data;
    size_t size;
    
public:
    // Move constructor
    String(String&& other) noexcept 
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }
    
    // Move assignment
    String& operator=(String&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
    
    // Copy constructor (for lvalues)
    String(const String& other)
        : size(other.size), data(new char[size + 1]) {
        strcpy(data, other.data);
    }
    
    // Copy assignment (for lvalues)
    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new char[size + 1];
            strcpy(data, other.data);
        }
        return *this;
    }
};
```


***

## COMPILE-TIME PROGRAMMING

### 10.1 Tuple Operations at Compile-Time

```cpp
#include <tuple>
#include <iostream>

// Compile-time tuple iteration
namespace detail {
    template<typename F, typename Tuple, size_t... Is>
    void for_each_impl(F&& f, Tuple&& t, index_sequence<Is...>) {
        (..., f(get<Is>(forward<Tuple>(t))));
    }
}

template<typename F, typename Tuple>
void for_each(F&& f, Tuple&& t) {
    constexpr auto size = tuple_size_v<decay_t<Tuple>>;
    detail::for_each_impl(
        forward<F>(f),
        forward<Tuple>(t),
        make_index_sequence<size>{}
    );
}

// Usage
auto t = make_tuple(1, 2.5, "hello");
for_each([](auto x) { cout << x << " "; }, t);  // 1 2.5 hello
```


### 10.2 Compile-Time String Processing

```cpp
#include <array>
#include <string_view>

// Compile-time string hashing
constexpr size_t hash_compile_time(string_view s) {
    size_t h = 0;
    for (char c : s) {
        h = h * 31 + c;
    }
    return h;
}

// Compile-time string search
constexpr bool contains_compile_time(string_view s, string_view sub) {
    return s.find(sub) != string_view::npos;
}

// Usage in compile-time context
static_assert(contains_compile_time("hello world", "world"));
constexpr auto hash = hash_compile_time("key");
```


***

## META-OBJECT PROTOCOL

### 11.1 Reflection-Like Patterns

```cpp
// Manual reflection without std::meta
struct Member {
    string_view name;
    string_view type;
    size_t offset;
};

template<typename T>
constexpr auto get_members() {
    // Specialize for each type
    return array<Member, 0>{};
}

template<>
constexpr auto get_members<Person>() {
    return array<Member, 3>{{
        {"name", "string", offsetof(Person, name)},
        {"age", "int", offsetof(Person, age)},
        {"email", "string", offsetof(Person, email)}
    }};
}

// Serialize any type
template<typename T>
string serialize(const T& obj) {
    string result;
    for (const auto& member : get_members<T>()) {
        result += member.name;
        result += ": ";
        // Serialize value...
    }
    return result;
}
```




### 11.2 Magic Enum (Reflection Hack)

Before C++26 Reflection, we use compiler intrinsics to get enum names.

```cpp
template <auto V>
constexpr std::string_view get_enum_name() {
    // Compiler-specific macros
    #ifdef __clang__
        return __PRETTY_FUNCTION__;
    #elif defined(__GNUC__)
        return __PRETTY_FUNCTION__;
    #elif defined(_MSC_VER)
        return __FUNCSIG__;
    #endif
}

enum class Color { Red, Green, Blue };

int main() {
    // Output contains "Color::Red" buried in the string
    std::cout << get_enum_name<Color::Red>() << "\n";
}
```

*Note: Libraries like `magic_enum` automate parsing this string.*

***

## ADVANCED CONTAINER TECHNIQUES

### 12.1 COW (Copy-On-Write) String

```cpp
class CowString {
private:
    struct Buffer : public enable_shared_from_this<Buffer> {
        string data;
        
        Buffer(const string& s) : data(s) {}
    };
    
    shared_ptr<Buffer> buffer;
    
    void ensure_unique() {
        if (!buffer.unique()) {
            buffer = make_shared<Buffer>(*buffer);
        }
    }
    
public:
    CowString(const string& s = "")
        : buffer(make_shared<Buffer>(s)) {}
    
    const char* data() const {
        return buffer->data.c_str();
    }
    
    char& operator[](size_t i) {
        ensure_unique();
        return buffer->data[i];
    }
};
```


### 12.2 Intrusive Containers

```cpp
#include <list>

// Node that contains its own link
class IntrusiveNode {
public:
    IntrusiveNode* next = nullptr;
    IntrusiveNode* prev = nullptr;
};

template<typename T>
class IntrusiveList {
private:
    IntrusiveNode* head;
    IntrusiveNode* tail;
    
public:
    void push_back(T* node) {
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
    }
};
```


***

## ABI & BINARY COMPATIBILITY

### 13.1 Stable ABI Design

```cpp
// Version-stable interface
class StableObject {
private:
    // Use void* for opaque pointer
    void* impl;
    
public:
    StableObject();
    ~StableObject();
    
    // Stable functions
    void do_something(int x);
    int get_value() const;
    
    // Virtual table for future extensions
    virtual ~StableObject() = default;
};

// Implementation in separate library
class StableObjectImpl {
public:
    int value = 0;
    void do_something(int x);
    int get_value() const { return value; }
};

StableObject::StableObject() 
    : impl(new StableObjectImpl()) {}

StableObject::~StableObject() {
    delete static_cast<StableObjectImpl*>(impl);
}

void StableObject::do_something(int x) {
    static_cast<StableObjectImpl*>(impl)->do_something(x);
}

int StableObject::get_value() const {
    return static_cast<StableObjectImpl*>(impl)->get_value();
}
```


***


## PERFORMANCE PROFILING & OPTIMIZATION

### 14.1 Benchmarking Framework

```cpp
#include <chrono>
#include <vector>

class Benchmark {
private:
    using Clock = chrono::high_resolution_clock;
    vector<chrono::nanoseconds> times;
    
public:
    template<typename F>
    void run(F func, int iterations = 1000) {
        for (int i = 0; i < iterations; i++) {
            auto start = Clock::now();
            func();
            auto end = Clock::now();
            times.push_back(chrono::duration_cast<chrono::nanoseconds>(end - start));
        }
    }
    
    void report() {
        if (times.empty()) return;
        
        sort(times.begin(), times.end());
        
        auto sum = 0LL;
        for (auto t : times) sum += t.count();
        
        cout << "Min: " << times.front().count() << " ns\n";
        cout << "Max: " << times.back().count() << " ns\n";
        cout << "Avg: " << (sum / times.size()) << " ns\n";
        cout << "Median: " << times[times.size() / 2].count() << " ns\n";
    }
};

// Usage
Benchmark bm;
bm.run([]() { /* test code */ }, 10000);
bm.report();
```


### 14.2 Cache-Friendly Algorithms

```cpp
// Cache-friendly data access pattern
template<typename T>
void cache_friendly_process(vector<T>& data) {
    // Process in cache-line aligned chunks
    const size_t CACHE_LINE = 64;  // 64 bytes typical
    const size_t CHUNK_SIZE = CACHE_LINE / sizeof(T);
    
    for (size_t i = 0; i < data.size(); i += CHUNK_SIZE) {
        // Process one cache line worth of data
        for (size_t j = i; j < min(i + CHUNK_SIZE, data.size()); j++) {
            process_element(data[j]);
        }
    }
}

// SIMD-friendly loop
void simd_process(int* data, size_t size) {
    // Align data for SIMD operations
    alignas(32) int buffer[32];
    
    for (size_t i = 0; i < size; i += 32) {
        // Process 32-byte chunks (8 ints)
        for (int j = 0; j < 8; j++) {
            data[i + j] = process(data[i + j]);
        }
    }
}
```


***


## DOMAIN-SPECIFIC LANGUAGE DESIGN

### 15.1 Expression DSL

```cpp
// Domain-specific language for math expressions
class Expr {
public:
    virtual ~Expr() = default;
    virtual double evaluate() = 0;
};

class Constant : public Expr {
private:
    double value;
public:
    Constant(double v) : value(v) {}
    double evaluate() override { return value; }
};

class BinaryOp : public Expr {
private:
    shared_ptr<Expr> left, right;
    function<double(double, double)> op;
    
public:
    BinaryOp(shared_ptr<Expr> l, shared_ptr<Expr> r,
             function<double(double, double)> op)
        : left(l), right(r), op(op) {}
    
    double evaluate() override {
        return op(left->evaluate(), right->evaluate());
    }
};

// Operator overloading for DSL
auto operator+(shared_ptr<Expr> l, shared_ptr<Expr> r) {
    return make_shared<BinaryOp>(l, r, [](double a, double b) { return a + b; });
}

// Usage
auto expr = make_shared<Constant>(3) + make_shared<Constant>(4);
cout << expr->evaluate() << "\n";  // 7
```


***


## MODERN DESIGN PATTERNS

Traditional GoF patterns often use inheritance. Modern C++ favors composition, templates, and lambdas.

### 16.1 Strategy Pattern (Functional Approach)

Instead of a class hierarchy, use `std::function` or templates.

```cpp
#include <functional>
#include <iostream>
#include <vector>

// Traditional: abstract base class Strategy
// Modern: std::function
using SortStrategy = std::function<void(std::vector<int>&)>;

class Sorter {
    SortStrategy strategy;
public:
    Sorter(SortStrategy s) : strategy(s) {}
    
    void sort(std::vector<int>& data) {
        if (strategy) strategy(data);
    }
};

int main() {
    std::vector<int> data = {5, 2, 9, 1};
    
    // Strategy 1: Lambda
    Sorter s1([](auto& v) { std::sort(v.begin(), v.end()); });
    
    // Strategy 2: Different logic
    Sorter s2([](auto& v) { std::sort(v.rbegin(), v.rend()); });
    
    s1.sort(data);
    return 0;
}
```


### 16.2 Visitor Pattern (std::variant)

Replace virtual functions with `std::variant` and `std::visit` for closed sets of types.

```cpp
#include <variant>
#include <iostream>
#include <vector>

struct Circle { double radius; };
struct Square { double side; };
using Shape = std::variant<Circle, Square>;

// Visitor
struct AreaVisitor {
    double operator()(const Circle& c) { return 3.14159 * c.radius * c.radius; }
    double operator()(const Square& s) { return s.side * s.side; }
};

int main() {
    std::vector<Shape> shapes = { Circle{2.0}, Square{3.0} };
    
    for (const auto& s : shapes) {
        // Apply visitor
        double area = std::visit(AreaVisitor{}, s);
        std::cout << "Area: " << area << "\n";
    }
    
    // With lambda (overloaded pattern)
    // See "Helper for std::visit" in many codebases
    return 0;
}
```


***

## HARDWARE SYMPATHY

### 17.1 Cache Locality & False Sharing

CPUs load data in cache lines (typically 64 bytes).

#### False Sharing
When two threads modify independent variables that sit on the *same* cache line, they invalidate each other's cache, destroying performance.

```cpp
#include <new>
#include <atomic>

struct BadCounter {
    std::atomic<int> a; // Thread 1 modifies
    std::atomic<int> b; // Thread 2 modifies
    // Likely on same cache line -> ping-pong effect
};

struct GoodCounter {
    alignas(64) std::atomic<int> a; // Forced to own cache line
    alignas(64) std::atomic<int> b;
};
```


### 17.2 Branch Prediction

CPUs try to guess which way an `if` will go. Modern C++20 provides attributes to help.

```cpp
void process(int* ptr) {
    if (!ptr) [[unlikely]] {
        // Compiler optimizes this block to be "cold"
        // CPU assumes this won't happen
        throw std::runtime_error("Null pointer");
    }
    
    // This "hot" path is optimized for fall-through
    if (ptr) [[likely]] {
        *ptr = 42;
    }
}
```


### 17.3 SIMD (Single Instruction, Multiple Data)

Using intrinsics (or libraries like `std::simd` in future) to process data in parallel lanes.

```cpp
// Example: Manual unrolling for auto-vectorization
void add_arrays(float* a, float* b, float* c, int n) {
    // Tell compiler pointers don't alias (C99 restrict, or implementation specific)
    // #pragma omp simd 
    for (int i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}
```


***


### <a name="chapter-15-productionprofessional"></a>CHAPTER 15: PRODUCTION & PROFESSIONAL


***
#### Professional Insights: Namespaces & Organization

##### Namespaces

Used to prevent name collisions when using multiple libraries, a namespace is a declarative preﬁx for functions,
classes, types, etc.
Section 44.1: What are namespaces?
A C++ namespace is a collection of C++ entities (functions, classes, variables), whose names are preﬁxed by the
name of the namespace. When writing code within a namespace, named entities belonging to that namespace
need not be preﬁxed with the namespace name, but entities outside of it must use the fully qualiﬁed name. The
fully qualiﬁed name has the format <namespace>::<entity>. Example:
namespace Example
{
  const int test = 5;
  const int test2 = test + 12; //Works within `Example` namespace
}
const int test3 = test + 3; //Fails; `test` not found outside of namespace.
const int test3 = Example::test + 3; //Works; fully qualified name used.
Namespaces are useful for grouping related deﬁnitions together. Take the analogy of a shopping mall. Generally a
shopping mall is split up into several stores, each store selling items from a speciﬁc category. One store might sell
electronics, while another store might sell shoes. These logical separations in store types help the shoppers ﬁnd the
items they're looking for. Namespaces help c++ programmers, like shoppers, ﬁnd the functions, classes, and
variables they're looking for by organizing them in a logical manner. Example:
namespace Electronics
{
    int TotalStock;
    class Headphones
    {
        // Description of a Headphone (color, brand, model number, etc.)
    };
    class Television
    {
        // Description of a Television (color, brand, model number, etc.)
    };
}
namespace Shoes
{
    int TotalStock;
    class Sandal
    {
        // Description of a Sandal (color, brand, model number, etc.)
    };
    class Slipper
    {
        // Description of a Slipper (color, brand, model number, etc.)
    };
}
There is a single namespace predeﬁned, which is the global namespace that has no name, but can be denoted by
::. Example:
void bar() {
    // defined in global namespace
}
namespace foo {
    void bar() {
        // defined in namespace foo
    }
    void barbar() {
        bar();   // calls foo::bar()
        ::bar(); // calls bar() defined in global namespace
    }
}
Section 44.2: Argument Dependent Lookup
When calling a function without an explicit namespace qualiﬁer, the compiler can choose to call a function within a
namespace if one of the parameter types to that function is also in that namespace. This is called "Argument
Dependent Lookup", or ADL:
namespace Test
{
  int call(int i);
  class SomeClass {...};
  int call_too(const SomeClass &data);
}
call(5); //Fails. Not a qualified function name.
Test::SomeClass data;
call_too(data); //Succeeds
call fails because none of its parameter types come from the Test namespace. call_too works because
SomeClass is a member of Test and therefore it qualiﬁes for ADL rules.
When does ADL not occur
ADL does not occur if normal unqualiﬁed lookup ﬁnds a class member, a function that has been declared at block
scope, or something that is not of function type. For example:
void foo();
namespace N {
    struct X {};
    void foo(X ) { std::cout << '1'; }
    void qux(X ) { std::cout << '2'; }
}
struct C {
    void foo() {}
    void bar() {
        foo(N::X{}); // error: ADL is disabled and C::foo() takes no arguments
    }
};
void bar() {
    extern void foo(); // redeclares ::foo
    foo(N::X{});       // error: ADL is disabled and ::foo() doesn't take any arguments
}
int qux;
void baz() {
    qux(N::X{}); // error: variable declaration disables ADL for "qux"
}
Section 44.3: Extending namespaces
A useful feature of namespaces is that you can expand them (add members to it).
namespace Foo
{
    void bar() {}
}
//some other stuff
namespace Foo
{
    void bar2() {}
}
Section 44.4: Using directive
The keyword 'using' has three ﬂavors. Combined with keyword 'namespace' you write a 'using directive':
If you don't want to write Foo:: in front of every stuﬀ in the namespace Foo, you can use using namespace Foo; to
import every single thing out of Foo.
namespace Foo
{
    void bar() {}
    void baz() {}
}
//Have to use Foo::bar()
Foo::bar();
//Import Foo
using namespace Foo;
bar(); //OK
baz(); //OK
It is also possible to import selected entities in a namespace rather than the whole namespace:
using Foo::bar;
bar(); //OK, was specifically imported
baz(); // Not OK, was not imported
A word of caution: using namespaces in header ﬁles is seen as bad style in most cases. If this is done, the
namespace is imported in every ﬁle that includes the header. Since there is no way of "un-using" a namespace, this
can lead to namespace pollution (more or unexpected symbols in the global namespace) or, worse, conﬂicts. See
this example for an illustration of the problem:
/***** foo.h *****/
namespace Foo
{
    class C;
}
/***** bar.h *****/
namespace Bar
{
    class C;
}
/***** baz.h *****/
#include "foo.h"
using namespace Foo;
/***** main.cpp *****/
#include "bar.h"
#include "baz.h"
using namespace Bar;
C c; // error: Ambiguity between Bar::C and Foo::C
A using-directive cannot occur at class scope.
Section 44.5: Making namespaces
Creating a namespace is really easy:
//Creates namespace foo
namespace Foo
{
    //Declares function bar in namespace foo
    void bar() {}
}
To call bar, you have to specify the namespace ﬁrst, followed by the scope resolution operator :::
Foo::bar();
It is allowed to create one namespace in another, for example:
namespace A
{
    namespace B
    {
        namespace C
        {
            void bar() {}
        }
    }
}
Version ≥ C++17
The above code could be simpliﬁed to the following:
namespace A::B::C
{
    void bar() {}
}
Section 44.6: Unnamed/anonymous namespaces
An unnamed namespace can be used to ensure names have internal linkage (can only be referred to by the current
translation unit). Such a namespace is deﬁned in the same way as any other namespace, but without the name:
namespace {
    int foo = 42;
}
foo is only visible in the translation unit in which it appears.
It is recommended to never use unnamed namespaces in header ﬁles as this gives a version of the content for
every translation unit it is included in. This is especially important if you deﬁne non-const globals.
// foo.h
namespace {
    std::string globalString;
}
// 1.cpp
#include "foo.h" //< Generates unnamed_namespace{1.cpp}::globalString ...
globalString = "Initialize";
// 2.cpp
#include "foo.h" //< Generates unnamed_namespace{2.cpp}::globalString ...
std::cout << globalString; //< Will always print the empty string
Section 44.7: Compact nested namespaces
Version ≥ C++17
namespace a {
  namespace b {
    template<class T>
    struct qualifies : std::false_type {};
  }
}
namespace other {
  struct bob {};
}
namespace a::b {
  template<>
  struct qualifies<::other::bob> : std::true_type {};
}
You can enter both the a and b namespaces in one step with namespace a::b starting in C++17.
Section 44.8: Namespace alias
A namespace can be given an alias (i.e., another name for the same namespace) using the namespace identiﬁer =
syntax. Members of the aliased namespace can be accessed by qualifying them with the name of the alias. In the
following example, the nested namespace AReallyLongName::AnotherReallyLongName is inconvenient to type, so
the function qux locally declares an alias N. Members of that namespace can then be accessed simply using N::.
namespace AReallyLongName {
    namespace AnotherReallyLongName {
        int foo();
        int bar();
        void baz(int x, int y);
    }
}
void qux() {
    namespace N = AReallyLongName::AnotherReallyLongName;
    N::baz(N::foo(), N::bar());
}
Section 44.9: Inline namespace
Version ≥ C++11
inline namespace includes the content of the inlined namespace in the enclosing namespace, so
namespace Outer
{
    inline namespace Inner
    {
        void foo();
    }
}
is mostly equivalent to
namespace Outer
{
    namespace Inner
    {
        void foo();
    }
    using Inner::foo;
}
but element from Outer::Inner:: and those associated into Outer:: are identical.
So following is equivalent
Outer::foo();
Outer::Inner::foo();
The alternative using namespace Inner; would not be equivalent for some tricky parts as template specialization:
For
#include <outer.h> // See below
class MyCustomType;
namespace Outer
{
    template <>
    void foo<MyCustomType>() { std::cout << "Specialization"; }
}
The inline namespace allows the specialization of Outer::foo
// outer.h
// include guard omitted for simplification
namespace Outer
{
    inline namespace Inner
    {
        template <typename T>
        void foo() { std::cout << "Generic"; }
    }
}
Whereas the using namespace doesn't allow the specialization of Outer::foo
// outer.h
// include guard omitted for simplification
namespace Outer
{
    namespace Inner
    {
        template <typename T>
        void foo() { std::cout << "Generic"; }
    }
    using namespace Inner;
    // Specialization of `Outer::foo` is not possible
    // it should be `Outer::Inner::foo`.
}
Inline namespace is a way to allow several version to cohabit and defaulting to the inline one
namespace MyNamespace
{
    // Inline the last version
    inline namespace Version2
    {
        void foo(); // New version
        void bar();
    }
    namespace Version1 // The old one
    {
        void foo();
    }
}
And with usage
MyNamespace::Version1::foo(); // old version
MyNamespace::Version2::foo(); // new version
MyNamespace::foo();           // default version : MyNamespace::Version1::foo();
Section 44.10: Aliasing a long namespace
This is usually used for renaming or shortening long namespace references such referring to components of a
library.
namespace boost
{
    namespace multiprecision
    {
        class Number ...
    }
}
namespace Name1 = boost::multiprecision;
//    Both Type declarations are equivalent
boost::multiprecision::Number X   //    Writing the full namespace path, longer
Name1::Number Y                   //    using the name alias, shorter
Section 44.11: Alias Declaration scope
Alias Declaration are aﬀected by preceding using statements
namespace boost
{
    namespace multiprecision
    {
        class Number ...
    }
}
using namespace boost;
//   Both Namespace are equivalent
namespace Name1 = boost::multiprecision;
namespace Name2 = multiprecision;
However, it is easier to get confused over which namespace you are aliasing when you have something like this:
namespace boost
{
    namespace multiprecision
    {
        class Number ...
    }
}
namespace numeric
{
    namespace multiprecision
    {
        class Number ...
    }
}
using namespace numeric;
using namespace boost;
//    Not recommended as
//    its not explicitly clear whether Name1 refers to
//    numeric::multiprecision or boost::multiprecision
namespace Name1 = multiprecision;
//    For clarity, its recommended to use absolute paths
//    instead
namespace Name2 = numeric::multiprecision;
namespace Name3 = boost::multiprecision;


### LARGE-SCALE PROJECT ARCHITECTURE

### 1.1 Layered Architecture

```cpp
// src/layers/presentation/controller.h
#ifndef PRESENTATION_CONTROLLER_H
#define PRESENTATION_CONTROLLER_H

#include "../domain/user.h"
#include "../application/user_service.h"

namespace presentation {
    class UserController {
    private:
        application::UserService& service;
        
    public:
        UserController(application::UserService& s) : service(s) {}
        
        void create_user(const std::string& name, const std::string& email) {
            auto user = service.create(name, email);
            display_result(user);
        }
        
    private:
        void display_result(const domain::User& user);
    };
}

#endif
// src/layers/application/user_service.h
#ifndef APPLICATION_USER_SERVICE_H
#define APPLICATION_USER_SERVICE_H

#include "../domain/user.h"
#include "../infrastructure/user_repository.h"

namespace application {
    class UserService {
    private:
        infrastructure::UserRepository& repo;
        
    public:
        UserService(infrastructure::UserRepository& r) : repo(r) {}
        
        domain::User create(const std::string& name, const std::string& email) {
            // Business logic
            domain::User user(name, email);
            validate_user(user);
            return repo.save(user);
        }
        
    private:
        void validate_user(const domain::User& user);
    };
}

#endif
// src/layers/domain/user.h
#ifndef DOMAIN_USER_H
#define DOMAIN_USER_H

namespace domain {
    class User {
    private:
        int id;
        std::string name;
        std::string email;
        
    public:
        User(const std::string& n, const std::string& e)
            : id(0), name(n), email(e) {}
        
        // Domain methods
        bool is_valid() const;
        void update_email(const std::string& new_email);
    };
}

#endif
// src/layers/infrastructure/user_repository.h
#ifndef INFRASTRUCTURE_USER_REPOSITORY_H
#define INFRASTRUCTURE_USER_REPOSITORY_H

#include "../domain/user.h"
#include "database_connection.h"

namespace infrastructure {
    class UserRepository {
    private:
        DatabaseConnection& db;
        
    public:
        UserRepository(DatabaseConnection& d) : db(d) {}
        
        domain::User save(const domain::User& user);
        std::optional<domain::User> find_by_id(int id);
        std::vector<domain::User> find_all();
    };
}

#endif
```


### 1.2 Microservices Architecture

```cpp
// Service 1: User Service
namespace user_service {
    class UserAPI {
    private:
        application::UserService& service;
        http::Server& server;
        
    public:
        void setup_routes() {
            server.post("/users", [this](const auto& req) {
                auto user = service.create(req.name, req.email);
                return http::Response::ok(user.to_json());
            });
            
            server.get("/users/:id", [this](const auto& req) {
                auto user = service.find(req.id);
                return http::Response::ok(user.to_json());
            });
        }
    };
}

// Service 2: Order Service
namespace order_service {
    class OrderAPI {
    private:
        application::OrderService& service;
        http::Client& http_client;
        
    public:
        void create_order(int user_id, const Order& order) {
            // Call user service
            auto user = http_client.get("http://user-service/users/" + std::to_string(user_id));
            
            // Create order
            service.create(user_id, order);
        }
    };
}
```


***


## CODE ORGANIZATION & PROJECT STRUCTURE

### 2.1 Modern CMake Project Structure


### <a name="chapter-16-systemdesigncasestudiescedition"></a>CHAPTER 16: SYSTEM DESIGN CASE STUDIES (C++ EDITION)

Solving common interview system design problems using C++ primitives.



#### 10.5.1 LRU Cache

**Problem**: Design a Least Recently Used cache with O(1) get and put.
**Solution**: Combine `std::list` (ordering) and `std::unordered_map` (lookup).

```cpp
#include <list>
#include <unordered_map>
#include <iostream>

template<typename Key, typename Value>
class LRUCache {
    size_t capacity;
    std::list<std::pair<Key, Value>> items;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> lookup;

public:
    LRUCache(size_t cap) : capacity(cap) {}

    void put(Key key, Value val) {
        if (lookup.find(key) != lookup.end()) {
            // Update: Move to front, update value
            items.splice(items.begin(), items, lookup[key]);
            lookup[key]->second = val;
            return;
        }

        if (items.size() == capacity) {
            // Evict: Remove back
            lookup.erase(items.back().first);
            items.pop_back();
        }

        // Insert: Push front
        items.emplace_front(key, val);
        lookup[key] = items.begin();
    }

    std::optional<Value> get(Key key) {
        if (lookup.find(key) == lookup.end()) return std::nullopt;
        // Access: Move to front
        items.splice(items.begin(), items, lookup[key]);
        return lookup[key]->second;
    }
};
```



#### 10.5.2 Token Bucket Rate Limiter

**Problem**: Limit requests to N per second.
**Solution**: Refill tokens based on time elapsed.

```cpp
#include <chrono>
#include <mutex>

class TokenBucket {
    const long long capacity;
    const long long rate_per_sec;
    
    double tokens;
    std::chrono::steady_clock::time_point last_refill;
    std::mutex mtx;

public:
    TokenBucket(long long cap, long long rate) 
        : capacity(cap), rate_per_sec(rate), tokens(cap), 
          last_refill(std::chrono::steady_clock::now()) {}

    bool allow_request(int cost = 1) {
        std::lock_guard<std::mutex> lock(mtx);
        refill();
        
        if (tokens >= cost) {
            tokens -= cost;
            return true;
        }
        return false;
    }

private:
    void refill() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - last_refill).count();
        
        double new_tokens = (duration * rate_per_sec) / 1000000.0;
        tokens = std::min((double)capacity, tokens + new_tokens);
        last_refill = now;
    }
};
```



### <a name="chapter-20-lowlatencysystemarchitecture"></a>CHAPTER 20: LOW-LATENCY SYSTEM ARCHITECTURE

Designing systems where microseconds matter (Trading, Real-time AdTech).



#### 24.1 The Disruptor Pattern (C++ Implementation)

A high-performance inter-thread messaging library. Key concept: **Single-Writer Ring Buffer** with no locks.

```cpp
template<typename T, size_t Size>
class Disruptor {
    std::array<T, Size> ring_buffer;
    alignas(64) std::atomic<int64_t> cursor{-1}; // Cache line padded
    
public:
    template<typename F>
    void publish(F&& factory) {
        int64_t current = cursor.load(std::memory_order_relaxed);
        int64_t next = current + 1;
        
        // Write data (no contention for single writer)
        factory(ring_buffer[next & (Size - 1)]);
        
        // Commit
        cursor.store(next, std::memory_order_release);
    }
    
    // Consumer tracks its own sequence...
};
```



#### 24.2 Kernel Bypass Networking (Concept)

Standard OS networking (interrupts, context switches) adds 10-50us latency.
**Solution**: Map the NIC (Network Interface Card) directly to user-space memory (DPDK, Solarflare OpenOnload).

*   **Zero Copy**: Packet data goes from NIC -> CPU L3 Cache -> User Buffer.
*   **Polling**: Instead of interrupts, one CPU core spins (`while(true)`) checking the NIC ring.



#### 24.3 OS Tuning for C++

Your code is only as fast as the OS allows.

1.  **CPU Isolation (`isolcpus`)**: Isolate cores from the OS scheduler so your thread never gets preempted.
2.  **Huge Pages**: Use 2MB or 1GB pages to reduce TLB (Translation Lookaside Buffer) misses.
```cpp
    void* ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, 
                     MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB, -1, 0);
```
3.  **Disable C-States**: Prevent CPU from going to sleep (power save) which causes wake-up latency.



#### 24.4 Zero-Copy Serialization (Cap'n Proto / FlatBuffers)

Avoid parsing JSON/XML. Access data directly from the binary buffer.

```cpp
// FlatBuffers schema compiled to C++ header
// No parsing step! Pointers just point to the right offsets.
auto monster = GetMonster(buffer_pointer);
auto hp = monster->hp(); // Immediate access
auto pos = monster->pos();
```



#### 24.5 LMAX Disruptor Internals

The key to Disruptor's speed is the **Sequence Barrier**.

1.  **Cursor**: Monotonically increasing number (atomic).
2.  **Barrier**: Consumers wait until `cursor >= my_sequence`.
3.  **Wait Strategy**:
    *   `BusySpinWaitStrategy`: Loops `while(cursor < seq)`. 100% CPU, 0ns latency.
    *   `YieldingWaitStrategy`: Loops but calls `std::this_thread::yield()`.
    *   `BlockingWaitStrategy`: Uses `std::condition_variable` (slowest).

***



### <a name="chapter-21-extremelowlatencyhardwaremastery"></a>CHAPTER 21: EXTREME LOW LATENCY & HARDWARE MASTERY

To achieve sub-microsecond latency, you must program the hardware, not just the language.



#### 31.1 CPU Architecture & Cache Topology

*   **L1 Cache**: ~32KB, 3-4 cycles. Per core.
*   **L2 Cache**: ~256KB-1MB, 10-12 cycles. Per core.
*   **L3 Cache**: ~10MB+, 40-70 cycles. Shared across cores.
*   **RAM**: 100+ cycles.

**Optimization Goal**: Stay in L1/L2.
**Technique**: Minimize object size, use contiguous memory (arrays), align data to cache lines (64 bytes).



#### 31.2 NUMA (Non-Uniform Memory Access)

On multi-socket servers, accessing RAM attached to another CPU socket is slow.
*   **Solution**: Pin threads to cores. Allocate memory on the local node.
*   **Tool**: `numactl --cpunodebind=0 --membind=0 ./app`



#### 31.5 Measurable Performance Targets

Define Service Level Objectives (SLOs) in percentiles.
*   **p50 (Median)**: Typical case.
*   **p99**: The "slow" case (1 in 100).
*   **p99.9**: The tail latency (1 in 1000). Crucial for HFT.

**Example Target**:
"Order processing must have p99 latency < 5 microseconds."

***



### <a name="chapter-22-advancedsimdavx2avx512"></a>CHAPTER 22: ADVANCED SIMD (AVX2 & AVX-512)

Data Parallelism: Processing 8 or 16 numbers in a single CPU cycle.



#### 32.1 SIMD Basics & Registers

*   **SSE**: 128-bit (4 floats). XMM registers.
*   **AVX2**: 256-bit (8 floats). YMM registers.
*   **AVX-512**: 512-bit (16 floats). ZMM registers.



#### 32.2 Intrinsics Example (Vector Addition)

Using `<immintrin.h>`.

```cpp
#include <immintrin.h>

void add_avx2(float* a, float* b, float* c, int N) {
    // Process 8 floats at a time
    for (int i = 0; i < N; i += 8) {
        // Load
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        
        // Operation
        __m256 vc = _mm256_add_ps(va, vb);
        
        // Store
        _mm256_storeu_ps(&c[i], vc);
    }
}
```
*   `_mm256_loadu_ps`: Load Unaligned Packed Single-precision.
*   `_mm256_add_ps`: Add packed singles.



#### 32.3 Measurable Outcome

*   **Objective**: Convert a scalar loop to AVX2.
*   **Success Metric**: 4x-8x speedup on large arrays (memory bandwidth permitting).

***



### <a name="chapter-23-custommemoryallocators"></a>CHAPTER 23: CUSTOM MEMORY ALLOCATORS

`malloc` and `new` are general-purpose and slow (locks, fragmentation). Real-time systems use custom allocators.



#### 33.1 Linear Allocator (Arena)

The absolute fastest allocator. O(1). Zero overhead.

```cpp
class LinearAllocator {
    char* start;
    char* current;
    size_t size;
public:
    LinearAllocator(size_t s) : size(s) {
        start = new char[s];
        current = start;
    }
    
    void* allocate(size_t n) {
        if (current + n > start + size) return nullptr;
        void* ptr = current;
        current += n;
        return ptr;
    }
    
    void reset() { current = start; } // Free ALL at once
};
```
*   **Use Case**: Per-frame game memory, Request-scoped web server memory.



#### 33.2 Pool Allocator

Fixed-size blocks. No external fragmentation. O(1) malloc/free.

```cpp
struct Chunk { Chunk* next; };

class PoolAllocator {
    Chunk* head = nullptr;
public:
    void* allocate() {
        if (!head) return ::operator new(sizeof(Chunk)); // Or expand pool
        Chunk* ptr = head;
        head = head->next;
        return ptr;
    }
    
    void deallocate(void* ptr) {
        Chunk* chunk = static_cast<Chunk*>(ptr);
        chunk->next = head;
        head = chunk;
    }
};
```

***



##### Argument Dependent Name Lookup

Section 122.1: What functions are found



##### Copy Elision

Section 109.1: Purpose of copy elision 
Section 109.2: Guaranteed copy elision 
Section 109.3: Parameter elision 
Section 109.4: Return value elision 
Section 109.5: Named return value elision 
Section 109.6: Copy initialization elision 


To truly master C++, you must understand what the compiler generates.



#### 14.1 Object Layout & ABI (Itanium C++ ABI)

How does `virtual` work?

```cpp
class Base {
    int64_t id;
public:
    virtual void func() {}
};

class Derived : public Base {
    int64_t data;
public:
    void func() override {}
};
```

**Memory Layout (64-bit system):**
```text
[ vptr (8 bytes) ] -> [ vtable for Base ]
[ id   (8 bytes) ]
```
For `Derived`:
```text
[ vptr (8 bytes) ] -> [ vtable for Derived ]
[ id   (8 bytes) ]
[ data (8 bytes) ]
```
*   **vptr**: Hidden pointer added to classes with virtual functions.
*   **vtable**: Static table of function pointers.
*   **Alignment**: Data is padded to align with word boundaries.



### <a name="chapter-27-writingagarbagecollector"></a>CHAPTER 27: WRITING A GARBAGE COLLECTOR

C++ has RAII, but implementing a GC teaches you about the stack and object graph.



#### 29.1 Mark-and-Sweep Basics

1.  **Roots**: Pointers on the stack/globals.
2.  **Mark**: Traverse object graph from roots, marking reachable objects.
3.  **Sweep**: Iterate heap, free unmarked objects.

```cpp
struct GCObject {
    bool marked = false;
    virtual ~GCObject() = default;
};

class VM {
    std::vector<GCObject*> heap;
    std::vector<GCObject*> roots; // Pointers currently on stack
    
public:
    void mark() {
        for (auto* obj : roots) mark_object(obj);
    }
    
    void mark_object(GCObject* obj) {
        if (!obj || obj->marked) return;
        obj->marked = true;
        // ... traverse children ...
    }
    
    void sweep() {
        auto it = std::remove_if(heap.begin(), heap.end(), [](GCObject* obj) {
            if (!obj->marked) {
                delete obj;
                return true;
            }
            obj->marked = false; // Reset for next cycle
            return false;
        });
        heap.erase(it, heap.end());
    }
};
```

***



### <a name="chapter-28-thestandardlibraryfromscratch"></a>CHAPTER 28: THE STANDARD LIBRARY FROM SCRATCH

Implementing core STL components to understand their cost.



#### 19.1 Implementing my::vector

Managing raw memory, growth, and construction.

```cpp
template<typename T>
class Vector {
    T* data = nullptr;
    size_t sz = 0;
    size_t cap = 0;
    
public:
    void push_back(const T& val) {
        if (sz == cap) {
            reallocate(cap == 0 ? 1 : cap * 2);
        }
        new (data + sz) T(val); // Placement new
        sz++;
    }
    
private:
    void reallocate(size_t new_cap) {
        T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        // Move old elements...
        // Delete old memory...
        data = new_data;
        cap = new_cap;
    }
};
```



##### Type Erasure

Section 90.1: A move-only `std::function` 
Section 90.2: Erasing down to a Regular type with manual vtable 
Section 90.3: Basic mechanism 
Section 90.4: Erasing down to a contiguous buer of T 
Section 90.5: Type erasing type erasure with std::any 


Moving computation from runtime to compile-time saves cycles and enables zero-cost abstractions.



### <a name="chapter-46-capstoneprojecthighperformanceorderbook"></a>CHAPTER 46: CAPSTONE PROJECT - HIGH-PERFORMANCE ORDER BOOK

This capstone project integrates C++20/23 features into a realistic high-frequency trading (HFT) component. It demonstrates Modules, Concepts, Ranges, Coroutines, and modern error handling.



#### Project Structure

```text
order_book/
├── src/
│   ├── types.cppm        (Module: Common types)
│   ├── order.cppm        (Module: Order definition)
│   ├── book.cppm         (Module: OrderBook logic)
│   └── main.cpp          (Entry point)
├── CMakeLists.txt
└── README.md
```



#### 2. Order Module (order.cppm)

```cpp
export module order;

import types;
import <format>;
import <string>;

export namespace hft {
    struct Order {
        OrderId id;
        Side side;
        Price price;
        Quantity quantity;

        // C++20 Spaceship for easy comparison
        auto operator<=>(const Order&) const = default;
        
        // C++23 Deducing This for generic accessors (example)
        template<typename Self>
        auto&& get_price(this Self&& self) {
            return std::forward<Self>(self).price;
        }
    };
}

// C++20 Formatter specialization
template<>
struct std::formatter<hft::Order> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    auto format(const hft::Order& o, format_context& ctx) const {
        return std::format_to(ctx.out(), "[ID:{}] {} @ {}", 
            o.id, (o.side == hft::Side::Buy ? "BUY" : "SELL"), o.price);
    }
};
```



#### 3. Order Book Module (book.cppm)

```cpp
export module book;

import types;
import order;
import <vector>;
import <map>;
import <ranges>;
import <algorithm>;
import <expected>;
import <print>;
import <coroutine>;

export namespace hft {

    // C++20 Concept for Order Container
    template<typename T>
    concept OrderContainer = requires(T c) {
        c.push_back(std::declval<Order>());
        c.size();
    };

    class OrderBook {
    private:
        // Use std::flat_map (C++23) for cache locality if available, 
        // else std::map. Simulated here as vector for simplicity + ranges
        std::vector<Order> bids;
        std::vector<Order> asks;

    public:
        // C++23 std::expected for error handling
        std::expected<void, std::string> add_order(Order o) {
            if (o.quantity == 0) return std::unexpected("Invalid quantity");
            
            auto& side_vec = (o.side == Side::Buy) ? bids : asks;
            side_vec.push_back(o);
            
            // Keep sorted (simplified)
            std::ranges::sort(side_vec, {}, &Order::price);
            if (o.side == Side::Buy) std::ranges::reverse(side_vec);
            
            return {};
        }

        // C++20 Coroutine Generator to stream top orders
        // Note: Requires <generator> (C++23) or custom implementation
        // Here we simulate a simple generator pattern or use ranges
        auto top_levels(Side side, int depth) const {
            const auto& vec = (side == Side::Buy) ? bids : asks;
            return vec | std::views::take(depth);
        }

        void print_book() const {
            std::println("--- Order Book ---");
            std::println("ASKS:");
            for (const auto& o : asks | std::views::reverse) std::println("  {}", o);
            std::println("BIDS:");
            for (const auto& o : bids) std::println("  {}", o);
            std::println("------------------");
        }
    };
}
```

