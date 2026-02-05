# C++23 LATEST FEATURES


## C++23 Overview & Direction

C++23 (finalized in 2023) is a **refinement and enhancement** of C++20 with practical improvements.

### Timeline & Context
- **2011**: C++11 (revolutionary)
- **2014**: C++14 (refinement)
- **2017**: C++17 (major improvements)
- **2020**: C++20 (revolutionary leap)
- **2023**: C++23 (practical enhancements)

### C++23 Philosophy
- **Enhance** existing C++20 features
- **Fill gaps** in C++20 design
- **Improve** convenience and usability
- **Optimize** common patterns
- **Standardize** frequently-requested features
- **Fix** issues discovered in C++20

### Key Themes
1. **Output & Formatting** - std::print for easy output
2. **Error Handling** - std::expected for results
3. **Loop Control** - Enhanced for loops with ranges
4. **Memory Safety** - Better pointer/array handling
5. **Debugging** - Stack traces
6. **Templates** - Deducing this improvements
7. **Constexpr** - More compile-time power
8. **Library** - Quality of life improvements

### Why C++23 Matters
C++23 builds on C++20 strengths:
-  Easier output without iostream overhead
-  Type-safe error handling (std::expected)
-  Better for loop control
-  Debugging support (stack traces)
-  More flexible subscript operator
-  Improved constexpr capabilities
-  More convenient library features
-  Better optional support

---

## STD::PRINT & FORMATTED OUTPUT

## 1.1 std::print - Simple Output

`std::print` provides easy, fast output without iostream overhead.

### Basic print Usage

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

### print vs format vs iostream

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

### print with File Streams

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

---

## 1.2 std::format Enhancements

### Format Improvements in C++23

```cpp
#include <format>

// More format options
double pi = 3.14159;

std::format("{:.2%}", 0.25);         // "25.00%" (percentage)
std::format("{:g}", 0.0001);         // General format
std::format("{:#x}", 255);           // "0xff" (with prefix)
std::format("{:_^10}", "test");      // "_____test" (custom fill)
```

---

## DEDUCING THIS

## 2.1 Explicit Member Function Parameters

`Deducing this` allows capturing the type and constness of the object.

### Basic Deducing This

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

### Deducing This for Const/Non-Const

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

### Practical Deducing This

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

## 2.2 Deducing This - Beyond the Basics

### Recursive Lambdas
Previously, lambdas couldn't easily call themselves. Now they can via the explicit object parameter.

```cpp
auto fib = [](this auto&& self, int n) {
    if (n <= 1) return n;
    return self(n - 1) + self(n - 2);
};

cout << fib(10) << "\n"; // 55
```

### Replacing CRTP
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

---

## RANGE-BASED FOR LOOP ENHANCEMENTS

## 3.1 For Loop Initializers

C++23 allows initialization in range-based for loops.

### For Loop with Init

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

### For Loop with Init and Structured Binding

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

---

## STD::EXPECTED

## 4.1 Result Type for Error Handling

`std::expected` represents either a value or an error.

### Basic expected Usage

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

### expected with Transform

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

### expected vs optional

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

---

## STD::OPTIONAL IMPROVEMENTS

## 5.1 Enhanced optional Operations

### optional with Deref Operator

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

### optional::value_or_else

```cpp
#include <optional>

optional<int> opt;

// Get value or call function to generate default
int value = opt.value_or_else([]() { return compute_default(); });

// More flexible than value_or
// value_or: int value = opt.value_or(0);
// value_or_else: int value = opt.value_or_else([]() { return expensive_computation(); });
```

---

## MULTIDIMENSIONAL SUBSCRIPT OPERATOR

## 6.1 Multiple Index Support

C++23 allows multiple indices in subscript operator.

### Multi-Index Subscript

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

### Dynamic 2D Array Wrapper

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

---

## 6.2 std::mdspan (Multidimensional View)

`std::mdspan` provides a non-owning multidimensional view of contiguous data.

### Basic mdspan Usage

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

## 6.3 mdspan Layouts

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

---

## STD::STACKTRACE

## 7.1 Runtime Stack Trace Capture

`std::stacktrace` provides runtime stack trace information.

### Basic Stacktrace

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

### Stacktrace in Error Handling

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

---

## CONSTEXPR ENHANCEMENTS

## 8.1 More Compile-Time Capabilities

### constexpr std::string

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

### constexpr vector Operations

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

### constexpr Algorithms

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

---

## ADAPTOR IMPROVEMENTS

## 9.1 Ranges::to Conversion

`std::ranges::to` converts ranges to containers.

### Basic ranges::to

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

### ranges::to with Construction Args

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

---

## LIBRARY IMPROVEMENTS

## 10.1 Utility Improvements

### std::out_ptr & std::inout_ptr

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

### std::move_iterator Improvements

```cpp
#include <iterator>
#include <algorithm>

vector<string> v = {"a", "b", "c"};

// C++23: Cleaner move semantics
auto result = v
    | std::views::transform([](auto& s) { return std::move(s); });
```

### Bit Manipulation Improvements

```cpp
#include <bit>

unsigned int x = 5;  // 0b0101

// C++23 additions
std::byteswap(x);              // Byte swap
std::has_single_bit(x);        // Check if power of 2
std::bit_width(x);             // Bits needed
std::popcount(x);              // Count 1 bits
```

---

## ATTRIBUTES & FEATURES

## 11.1 [[assume]] Attribute

`[[assume]]` allows providing hints to optimizer.

### Using assume

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

## 11.2 [[stdcall]] and ABI Attributes

```cpp
// Platform-specific calling conventions
#ifdef _WIN32
void __stdcall legacy_function() { }
[[gnu::stdcall]] void c_function();
#endif
```

---

## STANDARD LIBRARY ADDITIONS

## 12.1 Container & Utility Additions

### std::debug_assert (Conditional Assertion)

```cpp
void function(int x) {
    // Debug assertion (disabled in release)
    _ASSERT(x > 0, "x must be positive");
    
    // Work with x
}
```

### std::repeat_view

```cpp
#include <ranges>

// Repeat a value
auto repeated = std::views::repeat(42, 5);
for (int x : repeated) {
    cout << x << " ";  // 42 42 42 42 42
}
```

### std::stride_view

```cpp
#include <ranges>

vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

// Take every Nth element
auto every_other = v | std::views::stride(2);
for (int x : every_other) {
    cout << x << " ";  // 0 2 4 6 8
}
```

### std::chunk_by

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

### std::flat_map and std::flat_set

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

### std::generator (Synchronous Coroutine)

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

### 12.3 Generator Internals

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

---

## C++23 BEST PRACTICES

## What's Better with C++23

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

---
