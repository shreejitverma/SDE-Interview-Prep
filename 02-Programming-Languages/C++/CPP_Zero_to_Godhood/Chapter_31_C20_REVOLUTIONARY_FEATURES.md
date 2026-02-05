# C++20 REVOLUTIONARY FEATURES


## C++20 Overview & Revolutionary Scope

C++20 (finalized in 2020) is a **revolutionary language update** rivaling C++11 in magnitude.

### Timeline & Context
- **2011**: C++11 (first modern standard)
- **2014**: C++14 (refinement)
- **2017**: C++17 (major improvements)
- **2020**: C++20 (revolutionary leap)
- **2023**: C++23 (latest)

### C++20 Philosophy
- **Revolutionize** generic programming with concepts
- **Simplify** iteration with ranges
- **Empower** asynchronous programming with coroutines
- **Standardize** previously non-standard patterns
- **Address** fundamental C++ limitations
- **Enable** modern programming paradigms

### Key Themes
1. **Concepts** - Readable, constrained templates
2. **Ranges** - Composable, lazy evaluation
3. **Coroutines** - Asynchronous, generator patterns
4. **Spaceship** - Three-way comparison
5. **Modules** - Modularity & faster compilation
6. **Designated Initializers** - Named struct initialization
7. **Format** - Type-safe string formatting
8. **Constraints** - Compile-time validation

### Why C++20 Matters
C++20 addresses fundamental limitations:
-  Readable generic programming (concepts)
-  Composable iteration (ranges)
-  Async/await patterns (coroutines)
-  Lazy evaluation (ranges with coroutines)
-  Modular code (modules)
-  Type-safe formatting (std::format)
-  Compile-time validation (consteval)
-  Powerful iteration patterns

---

## CONCEPTS & CONSTRAINTS

## 1.1 Introduction to Concepts

Concepts are constraints on template parameters that make templates readable and enable better error messages.

### Basic Concept Definition

```cpp
#include <concepts>
using namespace std;

// Define a concept
template<typename T>
concept Integral = is_integral_v<T>;

// Use concept as constraint
template<Integral T>
void process(T value) {
    cout << "Integer: " << value << "\n";
}

process(42);              // OK - int satisfies Integral
process(3.14);            // ERROR - double doesn't satisfy Integral
// Error message is clear: doesn't satisfy Integral concept
```

### Standard Library Concepts

```cpp
#include <concepts>

// Predefined concepts
template<typename T>
concept Integer = integral<T>;  // std::integral

template<typename T>
concept Floating = floating_point<T>;  // std::floating_point

template<typename T>
concept Numeric = integral<T> || floating_point<T>;

template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> convertible_to<bool>;
    { a == b } -> convertible_to<bool>;
};

// Usage
template<Comparable T>
T find_min(T a, T b) {
    return a < b ? a : b;
}

find_min(5, 3);           // OK
find_min("a", "b");       // OK - strings are comparable
// find_min(complex(1,2), complex(3,4));  // ERROR - complex not comparable
```

### Complex Concept Definition

```cpp
#include <concepts>
#include <ranges>

// Concept with multiple requirements
template<typename T>
concept Container = requires(T c) {
    typename T::value_type;
    typename T::iterator;
    typename T::const_iterator;
    { c.begin() } -> convertible_to<typename T::iterator>;
    { c.end() } -> convertible_to<typename T::iterator>;
    { c.size() } -> convertible_to<size_t>;
    { c.empty() } -> convertible_to<bool>;
};

// Use in function
template<Container C>
void print_container(const C& c) {
    for (const auto& elem : c) {
        cout << elem << " ";
    }
    cout << "\n";
}

vector<int> v{1, 2, 3};
print_container(v);  // OK

// Custom type
struct MyContainer {
    vector<int> data;
    using value_type = int;
    using iterator = vector<int>::iterator;
    using const_iterator = vector<int>::const_iterator;
    
    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    const_iterator begin() const { return data.begin(); }
    const_iterator end() const { return data.end(); }
    size_t size() const { return data.size(); }
    bool empty() const { return data.empty(); }
};

MyContainer mc;
print_container(mc);  // OK
```

### Concept Benefits

```cpp
// Before C++20: Complex error messages
template<typename T>
void process_old(T x) {
    // If T doesn't have operator+, error is confusing
    auto result = x + 5;
}

process_old("string");  // ERROR - cryptic, long error message

// After C++20: Clear error messages
template<typename T>
requires requires(T x) { x + 5; }
void process_new(T x) {
    auto result = x + 5;
}

process_new("string");  // ERROR - "string" doesn't satisfy concept
```

---

## 1.2 Requires Expressions

Requires expressions test compile-time properties of types.

### Basic Requires Expression

```cpp
#include <concepts>

template<typename T>
requires requires(T x) {
    x + 1;           // Must support addition
    x.size();        // Must have size() member
    { x == x };      // Must support equality
}
void process(T x);

// Can also write as concept
template<typename T>
concept Processable = requires(T x) {
    x + 1;
    x.size();
    { x == x };
};

template<Processable T>
void process2(T x);
```

### Requires with Return Type Checking

```cpp
#include <concepts>

template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> convertible_to<T>;
};

template<typename T>
concept Multipliable = requires(T a, T b) {
    { a * b } -> convertible_to<T>;
};

template<typename T>
concept Arithmetic = Addable<T> && Multipliable<T>;

template<Arithmetic T>
T compute(T x, T y) {
    return (x + y) * (x - y);
}

cout << compute(5, 3) << "\n";        // OK - int is Arithmetic
cout << compute(2.5, 1.5) << "\n";    // OK - double is Arithmetic
```

### Practical Requires Examples

```cpp
// Check for operator[] and size()
template<typename T>
concept Indexable = requires(T t, size_t i) {
    { t[i] };
    { t.size() } -> convertible_to<size_t>;
};

// Check for specific method
template<typename T>
concept HasValue = requires(T t) {
    { t.value() };
};

// Check for const and non-const versions
template<typename T>
concept ConstIterable = requires(const T& t) {
    t.begin();
    t.end();
};

// Multi-type concepts
template<typename Iter, typename Sentinel>
concept SentinelFor = requires(Iter it, Sentinel s) {
    { it == s } -> convertible_to<bool>;
};
```

## 1.3 Concepts & Overload Resolution

Concepts participate in overload resolution. The compiler selects the **most constrained** template.

```cpp
template<typename T>
void process(T x) {
    cout << "Generic\n";
}

template<typename T> requires std::integral<T>
void process(T x) {
    cout << "Integral\n";
}

template<typename T> requires (std::integral<T> && sizeof(T) >= 4)
void process(T x) {
    cout << "Large Integral\n";
}

process(3.14);      // "Generic"
process((short)10); // "Integral"
process(100);       // "Large Integral" (int is >= 4 bytes)
```

---

## RANGES LIBRARY

## 2.1 Introduction to Ranges

Ranges provide a composable, lazy way to work with sequences.

### Basic Range Operations

```cpp
#include <ranges>
#include <vector>
#include <iostream>
using namespace std;

vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Traditional algorithm
vector<int> result;
for (int x : v) {
    if (x % 2 == 0) {
        result.push_back(x * 2);
    }
}

// With ranges (composable, lazy)
auto result = v
    | ranges::views::filter([](int x) { return x % 2 == 0; })
    | ranges::views::transform([](int x) { return x * 2; });

// result is lazy - computation happens on iteration
for (int x : result) {
    cout << x << " ";  // 4 8 12 16 20
}
```

### Range Views

```cpp
#include <ranges>
#include <vector>
using namespace std;

vector<int> v = {1, 2, 3, 4, 5};

// filter view
auto evens = v | ranges::views::filter([](int x) { return x % 2 == 0; });

// transform view
auto doubled = v | ranges::views::transform([](int x) { return x * 2; });

// take view (first N elements)
auto first3 = v | ranges::views::take(3);

// drop view (skip first N elements)
auto skip2 = v | ranges::views::drop(2);

// reverse view
auto reversed = v | ranges::views::reverse;

// iota view (generate sequence)
auto seq = ranges::views::iota(1, 11);  // 1..10

// join view (flatten nested ranges)
vector<vector<int>> matrix = {{1, 2}, {3, 4}, {5, 6}};
auto flattened = matrix | ranges::views::join;

// zip view (pair elements from two ranges)
vector<int> a = {1, 2, 3};
vector<string> b = {"a", "b", "c"};
auto zipped = ranges::views::zip(a, b);

for (auto [num, str] : zipped) {
    cout << num << ":" << str << " ";  // 1:a 2:b 3:c
}
```

### Range Algorithms

```cpp
#include <ranges>
#include <vector>
#include <algorithm>
using namespace std;

vector<int> v = {3, 1, 4, 1, 5, 9};

// Range algorithms (work with ranges, not iterators)
ranges::sort(v);                    // In-place sort
ranges::reverse(v);                 // In-place reverse
ranges::fill(v, 0);                 // Fill with value

// Range algorithms with predicates
ranges::sort(v, ranges::greater{});  // Sort descending
auto it = ranges::find(v, 5);        // Find element
auto count = ranges::count_if(v, [](int x) { return x > 3; });

// Range operations
ranges::rotate(v.begin(), v.begin() + 2, v.end());
ranges::partition(v, [](int x) { return x % 2 == 0; });
```

### Composing Multiple Views

```cpp
#include <ranges>
#include <vector>
#include <iostream>
using namespace std;

vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Chain multiple operations
auto result = v
    | ranges::views::filter([](int x) { return x > 2; })      // > 2
    | ranges::views::transform([](int x) { return x * x; })   // Square
    | ranges::views::take(4);                                   // First 4

// Process
for (int x : result) {
    cout << x << " ";  // 9 16 25 36
}

// All operations are lazy - no temporary vectors created
// Composition is clear and readable
```

## 2.2 Ranges Deep Dive

### Projections
Most range algorithms accept a "projection" argument to transform data *before* comparison.

```cpp
struct User { int id; string name; };
vector<User> users = {{2, "Bob"}, {1, "Alice"}};

// Sort by ID
ranges::sort(users, {}, &User::id);

// Sort by Name (descending)
ranges::sort(users, ranges::greater{}, &User::name);
```

### Dangling Iterators
Algorithms return `std::ranges::dangling` if the range is an rvalue (temporary) to prevent use-after-free.

```cpp
auto get_vector() { return vector{1, 2, 3}; }

auto it = ranges::find(get_vector(), 2); 
// Compile Error! 'it' would be dangling.
// The vector is destroyed at the end of the statement.
```

---

## COROUTINES

## 3.1 Introduction to Coroutines

Coroutines enable asynchronous, generator, and lazy evaluation patterns.

### Basic Generator Coroutine

```cpp
#include <coroutine>
#include <iostream>
using namespace std;

template<typename T>
class Generator {
public:
    struct promise_type {
        T current_value;
        
        Generator get_return_object() {
            return Generator{coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        suspend_never initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        
        suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }
        
        void return_void() {}
        void unhandled_exception() {}
    };
    
    struct iterator {
        coroutine_handle<promise_type> handle;
        
        iterator(coroutine_handle<promise_type> h, bool done) 
            : handle(h) {
            if (done) {
                handle = nullptr;
            }
        }
        
        iterator& operator++() {
            handle.resume();
            if (handle.done()) {
                handle = nullptr;
            }
            return *this;
        }
        
        bool operator==(const iterator& other) const {
            return handle == other.handle;
        }
        
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
        
        T operator*() const {
            return handle.promise().current_value;
        }
    };
    
    iterator begin() {
        if (handle) {
            handle.resume();
        }
        return iterator{handle, !handle || handle.done()};
    }
    
    iterator end() {
        return iterator{nullptr, true};
    }
    
private:
    coroutine_handle<promise_type> handle;
    
    Generator(coroutine_handle<promise_type> h) : handle(h) {}
};

// Generator coroutine
Generator<int> count_up(int max) {
    for (int i = 1; i <= max; i++) {
        co_yield i;  // Yield value and suspend
    }
}

// Usage
int main() {
    for (int i : count_up(5)) {
        cout << i << " ";  // 1 2 3 4 5
    }
    return 0;
}
```

### Async Coroutine

```cpp
#include <coroutine>
#include <iostream>
#include <chrono>
using namespace std;

class Task {
public:
    struct promise_type {
        Task get_return_object() {
            return Task{coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        suspend_never initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        
        void return_void() {}
        void unhandled_exception() {}
    };
    
    coroutine_handle<promise_type> handle;
    
    Task(coroutine_handle<promise_type> h) : handle(h) {}
    
    ~Task() {
        if (handle) {
            handle.destroy();
        }
    }
};

// Async coroutine
Task async_work() {
    cout << "Starting work\n";
    co_await std::suspend_always{};  // Suspend and resume later
    cout << "Continuing work\n";
}

int main() {
    auto task = async_work();  // Starts coroutine
    // Coroutine is suspended
    task.handle.resume();       // Resume execution
    return 0;
}
```

### Practical Coroutine: Fibonacci Generator

```cpp
#include <coroutine>
#include <iostream>
using namespace std;

template<typename T>
class Generator { /* ... implementation ... */ };

Generator<int> fibonacci(int limit) {
    int a = 0, b = 1;
    while (a < limit) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    for (int i : fibonacci(100)) {
        cout << i << " ";  // 0 1 1 2 3 5 8 13 21 34 55 89
    }
    return 0;
}
```

## 3.2 Coroutines Deep Dive

A coroutine is a function that can suspend and resume.

### The Awaitable Interface
To `co_await x`, `x` must be an Awaitable.

```cpp
struct Awaiter {
    bool await_ready() { return false; } // Always suspend?
    
    void await_suspend(std::coroutine_handle<> h) {
        // Schedule resumption (e.g., on a thread pool)
        // h.resume(); 
    }
    
    int await_resume() { return 42; } // Result of co_await
};

Task coroutine() {
    int result = co_await Awaiter{}; // result = 42
}
```

### Symmetric Transfer
Returning a `coroutine_handle` from `await_suspend` performs a "tail-call" to resume another coroutine without consuming stack space.

```cpp
std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) {
    return other_handle; // Switch to other coroutine immediately
}
```

---

## SPACESHIP OPERATOR (THREE-WAY COMPARISON)

## 4.1 The Spaceship Operator <=>

The spaceship operator performs three-way comparison and returns comparison category.

### Basic Spaceship Usage

```cpp
#include <compare>
#include <iostream>
using namespace std;

int a = 5, b = 10;

// Spaceship operator returns ordering
auto cmp = a <=> b;

if (cmp < 0) {
    cout << "a < b\n";
} else if (cmp > 0) {
    cout << "a > b\n";
} else {
    cout << "a == b\n";
}
```

### Spaceship with Custom Types

```cpp
#include <compare>

struct Person {
    string name;
    int age;
    
    // Default spaceship (compares as tuple)
    auto operator<=>(const Person&) const = default;
};

Person p1{"Alice", 30};
Person p2{"Bob", 25};

auto cmp = p1 <=> p2;
if (cmp < 0) cout << "p1 < p2\n";
if (cmp > 0) cout << "p1 > p2\n";
```

### Defaulted Comparison

```cpp
#include <compare>

struct Point {
    int x, y;
    
    // Default spaceship - compares lexicographically
    auto operator<=>(const Point&) const = default;
};

Point p1{1, 2};
Point p2{1, 2};
Point p3{2, 1};

cout << (p1 <=> p2 == 0) << "\n";     // true (equal)
cout << (p1 <=> p3 < 0) << "\n";      // true (p1 < p3)
```

### Comparison Categories

```cpp
#include <compare>
#include <iostream>

// Different comparison categories
struct Comparable {
    int value;
    
    // Returns std::strong_ordering (can do all operations)
    strong_ordering operator<=>(const Comparable& other) const {
        return value <=> other.value;
    }
};

struct PartiallyComparable {
    double value;
    
    // Returns std::partial_ordering (NaN is not comparable)
    partial_ordering operator<=>(const PartiallyComparable& other) const {
        return value <=> other.value;
    }
};

Comparable c1{5}, c2{10};
cout << (c1 <=> c2 < 0) << "\n";  // true

PartiallyComparable p1{1.5}, p2{2.5};
cout << (p1 <=> p2 < 0) << "\n";  // true
```

### Spaceship Benefits

```cpp
// Before C++20: Must define all comparison operators
struct Person {
    string name;
    int age;
    
    bool operator<(const Person& other) const {
        if (name != other.name) return name < other.name;
        return age < other.age;
    }
    
    bool operator<=(const Person& other) const { /* ... */ }
    bool operator>(const Person& other) const { /* ... */ }
    bool operator>=(const Person& other) const { /* ... */ }
    bool operator==(const Person& other) const { /* ... */ }
    bool operator!=(const Person& other) const { /* ... */ }
};

// After C++20: Default spaceship does all of it
struct Person {
    string name;
    int age;
    
    auto operator<=>(const Person&) const = default;
};
```

---

## MODULES

## 5.1 Introduction to Modules

Modules provide better code organization and faster compilation.

### Module Definition

```cpp
// math_module.cppm (module interface unit)
export module math;

export int add(int a, int b) {
    return a + b;
}

export int multiply(int a, int b) {
    return a * b;
}

// Helper function (not exported)
int helper(int x) {
    return x * 2;
}
```

### Using Modules

```cpp
// main.cpp
import math;
#include <iostream>
using namespace std;

int main() {
    cout << add(5, 3) << "\n";              // OK - exported
    cout << multiply(5, 3) << "\n";         // OK - exported
    // cout << helper(5) << "\n";           // ERROR - not exported
    
    return 0;
}
```

### Module Partitions

```cpp
// math.cppm (main interface)
export module math;
export import :impl;

// math-impl.cppm (partition)
export module math:impl;

export struct Complex {
    double real, imag;
    
    Complex operator+(const Complex& other) const {
        return {real + other.real, imag + other.imag};
    }
};
```

### Module Benefits

```
// Before modules (header files):
// - Recompilation overhead
// - Macro pollution
// - Circular dependencies
// - Header guards boilerplate

// After modules:
// - Faster compilation (parse once)
// - No macro pollution
// - No circular dependency issues
// - Clean interface definition
```

## 5.2 Modules Deep Dive

### Global Module Fragment
For legacy headers that must be included before the module declaration.

```cpp
module; // Start fragment
#include <vector>
#include <string>

export module my_app; // End fragment, start module

export void process(std::vector<int>& v);
```

### Private Module Partition
Hiding implementation details within the same file.

```cpp
export module calculator;

export int add(int a, int b);

module :private; // Start private implementation

int helper(int x) { return x + 1; }

int add(int a, int b) {
    return helper(a) + helper(b) - 2;
}
```

---

## DESIGNATED INITIALIZERS

## 6.1 Named Member Initialization

Designated initializers allow initializing struct/class members by name.

### Basic Designated Initializers

```cpp
#include <iostream>
using namespace std;

struct Point {
    int x;
    int y;
    int z;
};

// Before C++20: Order matters
Point p1{1, 2, 3};  // x=1, y=2, z=3

// After C++20: Can specify by name
Point p2{.x = 10, .y = 20, .z = 30};
Point p3{.y = 20, .x = 10, .z = 30};  // Order doesn't matter
Point p4{.x = 5, .z = 15};             // y defaults to 0

cout << p2.x << " " << p2.y << " " << p2.z << "\n";  // 10 20 30
```

### With Classes and Inheritance

```cpp
struct Base {
    int a;
};

struct Derived : Base {
    int b;
    int c;
};

// Designators for base and derived members
Derived d{.a = 1, .b = 2, .c = 3};

cout << d.a << " " << d.b << " " << d.c << "\n";  // 1 2 3
```

### Practical Designated Initializers

```cpp
struct Config {
    string name;
    int port;
    string host;
    bool ssl;
    int timeout;
};

// Clear intent - parameters obvious
Config cfg{
    .name = "server",
    .port = 8080,
    .host = "localhost",
    .ssl = true,
    .timeout = 30
};

// Much better than:
// Config cfg{"server", 8080, "localhost", true, 30};
```

---

## CALENDAR & TIME ZONES

## 7.1 Advanced Chrono Features

C++20 adds comprehensive calendar and timezone support.

### Calendar Types

```cpp
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

// Year, month, day
year y{2024};
month m{12};
day d{25};

// Construct date
auto date = y / m / d;  // 2024-12-25
cout << date << "\n";

// Current date
auto today = floor<days>(system_clock::now());
cout << "Today: " << today << "\n";

// Date arithmetic
auto tomorrow = date + days(1);
auto next_month = date + months(1);
auto next_year = date + years(1);
```

### Time Zones

```cpp
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

// Get timezone
const auto& tz = locate_zone("America/New_York");

// Current time in timezone
auto now = system_clock::now();
auto zoned_time = make_zoned(tz, now);

cout << "UTC: " << now << "\n";
cout << "NY: " << zoned_time << "\n";
```

### Formatted Time Output

```cpp
#include <chrono>
#include <format>
#include <iostream>
using namespace std;
using namespace chrono;

auto now = system_clock::now();

// Format with pattern
cout << format("{:%Y-%m-%d %H:%M:%S}", now) << "\n";
// Output: 2024-12-25 15:30:45
```

---

## STD::FORMAT

## 8.1 Type-Safe String Formatting

`std::format` provides Python-like formatting without type unsafety.

### Basic format Usage

```cpp
#include <format>
#include <iostream>
using namespace std;

// Simple substitution
cout << format("Hello {}, you are {} years old", "Alice", 30) << "\n";
// Output: Hello Alice, you are 30 years old

// Positional arguments
cout << format("{1} {0}", "World", "Hello") << "\n";
// Output: Hello World

// Argument access by index
cout << format("{0} + {0} = {}", 5, 10) << "\n";
// Output: 5 + 5 = 10
```

### Formatting Specifications

```cpp
#include <format>
#include <iostream>
using namespace std;

int num = 255;
double pi = 3.14159;

// Hex, binary, octal
cout << format("{:x}", num) << "\n";           // ff (hex)
cout << format("{:b}", num) << "\n";           // 11111111 (binary)
cout << format("{:o}", num) << "\n";           // 377 (octal)

// Floating point precision
cout << format("{:.2f}", pi) << "\n";          // 3.14
cout << format("{:.5f}", pi) << "\n";          // 3.14159

// Padding and alignment
cout << format("{:>10}", "hello") << "\n";     // "     hello" (right)
cout << format("{:<10}", "hello") << "\n";     // "hello     " (left)
cout << format("{:^10}", "hello") << "\n";     // "  hello   " (center)

// Number formatting
cout << format("{:,}", 1234567) << "\n";       // 1,234,567 (with separator)
cout << format("{:e}", pi) << "\n";            // 3.14e+00 (scientific)
```

### Format with Custom Types

```cpp
#include <format>

struct Point {
    int x, y;
};

// Define formatter for Point
template<>
struct format_traits<Point> {
    static auto format(const Point& p) {
        return format_string("({}, {})", p.x, p.y);
    }
};

Point pt{10, 20};
cout << format("Point: {}", pt) << "\n";  // Point: (10, 20)
```

---

## CONSTEVAL & CONSTINIT

## 9.1 Immediate Functions and Constants

### consteval - Immediate Functions

```cpp
#include <iostream>
using namespace std;

// Must be evaluated at compile-time
consteval int square(int x) {
    return x * x;
}

int main() {
    int arr[square(5)];           // OK - computed at compile-time
    cout << square(10) << "\n";   // OK - 100
    
    int x = 5;
    // cout << square(x) << "\n"; // ERROR - x is not compile-time constant
    
    return 0;
}
```

### constinit - Compile-Time Initialization

```cpp
#include <iostream>
using namespace std;

// Thread-local with compile-time initialization
thread_local constinit int counter = 0;

int main() {
    counter = 10;  // Can be modified at runtime
    cout << counter << "\n";  // 10
    
    return 0;
}
```

### Difference: constexpr vs consteval

```cpp
// constexpr: Can be evaluated at compile-time OR runtime
constexpr int add_constexpr(int a, int b) {
    return a + b;
}

// consteval: MUST be evaluated at compile-time
consteval int add_consteval(int a, int b) {
    return a + b;
}

int main() {
    int x = 5, y = 10;
    
    int c1 = add_constexpr(x, y);      // Runtime evaluation
    int c2 = add_constexpr(5, 10);     // Compile-time evaluation
    
    // int d1 = add_consteval(x, y);   // ERROR - must be compile-time
    int d2 = add_consteval(5, 10);     // OK - compile-time
    
    return 0;
}
```

---

## LAMBDA ENHANCEMENTS

## 10.1 C++20 Lambda Improvements

### Default Constructible Lambdas

```cpp
#include <iostream>
using namespace std;

// C++20: Lambdas without captures can be default constructed
auto counter = [count = 0]() mutable { return ++count; };

// Can be default constructed
decltype(counter) c1;  // Default construct
c1();

// But lambdas with captures still can't
// auto [x] = 5;
// decltype([x]() {}) bad;  // ERROR
```

### Stateless Lambda as Template Parameter

```cpp
#include <iostream>
using namespace std;

template<auto F>
void call_func() {
    F();
}

// Stateless lambda as template argument
call_func<[]() { cout << "Hello\n"; }>();  // OK

// Stateful lambda (captures) can't be template argument
// auto y = 5;
// call_func<[y]() { cout << y; }>();  // ERROR
```

---

## ADVANCED FEATURES

## 11.1 Additional C++20 Features

### Spaceship Operator with Library Support

```cpp
#include <compare>
#include <vector>

// All standard library types support spaceship
vector<int> v1{1, 2, 3};
vector<int> v2{1, 2, 4};

auto cmp = v1 <=> v2;
if (cmp < 0) cout << "v1 < v2\n";
```

### Bit Operations

```cpp
#include <bit>
#include <iostream>
using namespace std;

unsigned int x = 12;  // 0b1100

cout << bit_width(x) << "\n";           // 4 (bits needed)
cout << popcount(x) << "\n";            // 2 (number of 1s)
cout << countl_zero(x) << "\n";         // 28 (leading zeros on 32-bit)
cout << rotl(x, 2) << "\n";             // Rotate left
cout << rotr(x, 2) << "\n";             // Rotate right
cout << (x & ~(x - 1)) << "\n";         // Lowest set bit

// std::bit_cast (Safe type punning)
float f = 3.14f;
auto i = std::bit_cast<uint32_t>(f);  // Safe reinterpretation of bits
cout << std::hex << i << "\n";
```

### std::atomic_ref

`std::atomic_ref` allows atomic operations on non-atomic objects.

```cpp
#include <atomic>
#include <thread>
#include <vector>

void process(int& counter) {
    // Treat 'counter' as atomic for this scope
    std::atomic_ref<int> atomic_counter(counter);
    atomic_counter++;
}

int main() {
    int val = 0;
    std::vector<std::thread> threads;
    for(int i=0; i<10; ++i) threads.emplace_back(process, std::ref(val));
    for(auto& t : threads) t.join();
    return 0;
}
```

### Concepts in Standard Library

```cpp
#include <concepts>
#include <iostream>

// Standard concepts
static_assert(integral<int>);
static_assert(floating_point<double>);
static_assert(invocable<int(*)(int), int>);
static_assert(copyable<int>);
static_assert(assignable_from<int&, int>);

template<typename T>
requires copyable<T>
void copy_safe(const T& src, T& dst) {
    dst = src;
}
```

---

## LIBRARY IMPROVEMENTS

## 12.1 STL Enhancements in C++20

### std::span (Non-owning Array View)

`std::span` provides a lightweight, non-owning view over a contiguous sequence of objects (like array, vector, or C-array).

```cpp
#include <span>
#include <vector>
#include <iostream>
#include <array>

void print_values(std::span<int> data) {
    for (int x : data) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

int main() {
    int arr[] = {1, 2, 3};
    std::vector<int> vec = {4, 5, 6};
    std::array<int, 3> std_arr = {7, 8, 9};

    // Works with all contiguous containers
    print_values(arr);        // 1 2 3
    print_values(vec);        // 4 5 6
    print_values(std_arr);    // 7 8 9
    
    // Sub-span (slicing)
    print_values(std::span(vec).subspan(1)); // 5 6
    
    return 0;
}
```

### std::semaphore

```cpp
#include <semaphore>
#include <thread>

counting_semaphore<3> sem(3);  // Max 3 concurrent

void worker() {
    sem.acquire();
    // Critical section (at most 3 threads)
    // Do work
    sem.release();
}
```

### std::latch & std::barrier

```cpp
#include <latch>
#include <barrier>
#include <thread>
#include <vector>

// Latch: one-time synchronization
latch finish(3);

void worker(latch& l) {
    // Do work
    l.count_down();
    l.wait();  // Wait for all to finish
}

// Barrier: reusable synchronization
barrier sync(3);

void barrier_worker(barrier& b) {
    while (true) {
        // Do work
        b.arrive_and_wait();  // Synchronize every iteration
    }
}
```

### std::source_location (Reflection for Logging)

```cpp
#include <source_location>
#include <iostream>

void log(const char* message, 
         const std::source_location location = std::source_location::current()) {
    std::cout << "Info: " << message << "\n"
              << "File: " << location.file_name() << "("
              << location.line() << ":" << location.column() << ")\n"
              << "Func: " << location.function_name() << "\n";
}

int main() {
    log("Something happened");
    return 0;
}
```

### std::osyncstream (Synchronized Output)

Prevents interleaved output from multiple threads.

```cpp
#include <syncstream>
#include <iostream>
#include <thread>

void worker(int id) {
    std::osyncstream(std::cout) << "Worker " << id << " is running\n";
}

int main() {
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    t1.join(); t2.join();
    return 0;
}
```

### Ranges with Algorithms

```cpp
#include <ranges>
#include <vector>
#include <algorithm>

vector<int> v = {3, 1, 4, 1, 5};

// Ranges algorithms with pipes
v | ranges::views::sort
  | ranges::views::unique
  | ranges::views::take(3)

### std::jthread (Auto-joining Thread)

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
```

---

## C++20 BEST PRACTICES

## What's Better with C++20

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

---
