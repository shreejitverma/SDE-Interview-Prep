# C++17 MODERN FEATURES


## C++17 Overview & Significance

C++17 (finalized in 2017) is a **major language update** rivaling C++11 in scope.

### Timeline & Context
- **2011**: C++11 released (revolutionary)
- **2014**: C++14 released (refinement)
- **2017**: C++17 released (major overhaul)
- **2020**: C++20 released (revolutionary again)

### C++17 Philosophy
- **Fix** fundamental issues with C++11/14
- **Add** major features requested by industry
- **Improve** performance and expressivity
- **Simplify** complex code patterns
- **Standardize** common idioms

### Key Themes
1. **Pattern Matching** - Structured bindings
2. **Null Safety** - optional, variant
3. **Performance** - string_view, if constexpr
4. **Expressivity** - Fold expressions
5. **Safety** - Filesystem library
6. **Parallelism** - Parallel algorithms
7. **Type Deduction** - CTAD
8. **Flexibility** - std::any

### Why C++17 Matters
C++17 addresses real pain points:
-  Safer null handling (optional)
-  Type-safe unions (variant)
-  Zero-copy string operations (string_view)
-  Compile-time branching (if constexpr)
-  Pattern matching (structured bindings)
-  Safe filesystem access
-  Automatic template deduction
-  Flexible type storage (any)

---

## STRUCTURED BINDINGS

## 1.1 Introduction to Structured Bindings

Structured bindings allow unpacking objects into individual variables.

### Basic Structured Bindings

```cpp
#include <tuple>
#include <iostream>
using namespace std;

// Before C++17: Verbose
tuple<int, double, string> t = {42, 3.14, "hello"};
int a = get<0>(t);
double b = get<1>(t);
string c = get<2>(t);

// C++17: Clean and simple
auto [x, y, z] = t;
cout << x << ", " << y << ", " << z << "\n";  // 42, 3.14, hello
```

### Structured Bindings with Pairs

```cpp
#include <map>

map<string, int> ages{{"Alice", 30}, {"Bob", 25}};

// Before C++17: Awkward
for (const auto& pair : ages) {
    const string& name = pair.first;
    int age = pair.second;
    cout << name << " is " << age << "\n";
}

// C++17: Natural
for (const auto& [name, age] : ages) {
    cout << name << " is " << age << "\n";
}
```

### Structured Bindings with Arrays

```cpp
// Arrays
int arr[3] = {1, 2, 3};
auto [a, b, c] = arr;
cout << a << ", " << b << ", " << c << "\n";  // 1, 2, 3

// Fixed-size arrays
array<double, 4> coords = {1.0, 2.0, 3.0, 4.0};
auto [x, y, z, w] = coords;
```

### Structured Bindings with Structs

```cpp
struct Person {
    string name;
    int age;
    string city;
};

Person p{"Alice", 30, "NYC"};

// Before C++17
string name = p.name;
int age = p.age;
string city = p.city;

// C++17
auto [name, age, city] = p;
cout << name << " is " << age << " from " << city << "\n";
```

### Structured Bindings with Return Values

```cpp
pair<bool, int> divide(int a, int b) {
    if (b == 0) return {false, 0};
    return {true, a / b};
}

// Before C++17: Awkward
auto result = divide(10, 2);
if (result.first) {
    cout << "Result: " << result.second << "\n";
}

// C++17: Clear
auto [success, value] = divide(10, 2);
if (success) {
    cout << "Result: " << value << "\n";
}
```

### Structured Bindings with References

```cpp
int x = 5, y = 10;

// Modify through references
auto& [rx, ry] = tuple<int&, int&>(x, y);
rx = 20;
cout << x << "\n";  // 20 (modified)

// Const references
const auto& [cx, cy] = tuple<int, int>(5, 10);
// cx = 100;  // ERROR - const
```

### Practical Use Cases

```cpp
// Function returning multiple values
tuple<bool, string, int> parse_config(const string& path) {
    // Parse and return success, name, port
    return {true, "server", 8080};
}

auto [ok, name, port] = parse_config("/etc/config");
if (ok) {
    cout << "Server " << name << " on port " << port << "\n";
}

// Iterating over map with unpacking
map<string, vector<int>> data{
    {"a", {1, 2, 3}},
    {"b", {4, 5, 6}}
};

for (auto [key, values] : data) {
    cout << key << ": ";
    for (int v : values) cout << v << " ";
    cout << "\n";
}

// Database-like access
vector<tuple<int, string, double>> records{
    {1, "Alice", 95.5},
    {2, "Bob", 87.0},
    {3, "Carol", 92.5}
};

for (auto [id, name, score] : records) {
    cout << id << ": " << name << " scored " << score << "\n";
}
```

### Structured Bindings Rules

```cpp
// Auto deduction (copies)
tuple<int, int> t{1, 2};
auto [a, b] = t;           // a, b are copies

// References
auto& [x, y] = t;          // x, y reference t's elements
const auto& [cx, cy] = t;  // const references

// Move
auto&& [mx, my] = move(t); // rvalue references

// Partial binding (with operator[])
struct Container {
    int& operator[](size_t i);  // Must return reference
};

Container c;
auto [elem] = c;           // Gets copy via operator[]

// Multiple items
auto [a, b, c, d] = tuple{1, 2, 3, 4};
auto [x, y, z] = array{10, 20, 30};
```

## 1.2 Structured Bindings Under the Hood

The compiler generates a hidden variable.

**Code:**
```cpp
auto [x, y] = my_pair;
```

**Compiler Logic:**
```cpp
auto __hidden = my_pair;
auto& x = __hidden.first;  // Aliases
auto& y = __hidden.second;
```

**Implication**:
*   `x` and `y` are NOT variables; they are names referring to subobjects of the hidden variable.
*   If you use `auto& [x, y]`, the hidden variable is a reference.

---

## OPTIONAL & VARIANT

## 2.1 std::optional - Safe Nullable Values

`std::optional` represents a value that may or may not be present.

### Basic optional Usage

```cpp
#include <optional>
#include <iostream>
using namespace std;

// Function that might not return a value
optional<int> parse_int(const string& s) {
    try {
        return stoi(s);
    } catch (...) {
        return nullopt;  // No value
    }
}

// Usage
auto result1 = parse_int("42");
if (result1.has_value()) {
    cout << "Value: " << result1.value() << "\n";
}

// Or using operator*
if (result1) {
    cout << "Value: " << *result1 << "\n";
}

// Default value
auto value = result1.value_or(0);  // 0 if no value
```

### optional with Complex Types

```cpp
struct User {
    int id;
    string name;
};

optional<User> find_user(int id) {
    if (id < 0) return nullopt;
    return User{id, "User" + to_string(id)};
}

// Usage
if (auto user = find_user(42)) {
    cout << user->name << "\n";
} else {
    cout << "User not found\n";
}
```

### optional Operations

```cpp
optional<int> opt(42);

// Check
if (opt) cout << "Has value\n";          // true
if (opt.has_value()) cout << "Has\n";    // true

// Access
cout << opt.value() << "\n";              // 42
cout << *opt << "\n";                     // 42
cout << opt.value_or(0) << "\n";          // 42

// Modify
opt.value() = 100;
cout << *opt << "\n";                     // 100

// Reset
opt = nullopt;
if (opt) cout << "Has value\n";          // false

// Or assign
opt = 99;
cout << *opt << "\n";                     // 99
```

### optional with Chaining

```cpp
optional<int> process(optional<int> input) {
    if (!input) return nullopt;
    return input.value() * 2;
}

auto result = process(optional<int>(5));
if (result) {
    cout << result.value() << "\n";  // 10
}

// Chain operations
auto chain = parse_int("42")
    .and_then([](int x) -> optional<int> { return x * 2; })
    .or_else([]() { return optional<int>(0); });
```

---

## 2.2 std::variant - Type-Safe Union

`std::variant` is a type-safe union that holds one of several types.

### Basic variant Usage

```cpp
#include <variant>
#include <iostream>
using namespace std;

// Can hold int, double, or string
variant<int, double, string> value;

// Store int
value = 42;
cout << get<int>(value) << "\n";  // 42

// Store double
value = 3.14;
cout << get<double>(value) << "\n";  // 3.14

// Store string
value = string("hello");
cout << get<string>(value) << "\n";  // hello

// Check type
cout << value.index() << "\n";  // 2 (string is third)
```

### variant with Type Checking

```cpp
void process(variant<int, double, string> value) {
    if (holds_alternative<int>(value)) {
        cout << "Integer: " << get<int>(value) << "\n";
    } else if (holds_alternative<double>(value)) {
        cout << "Double: " << get<double>(value) << "\n";
    } else if (holds_alternative<string>(value)) {
        cout << "String: " << get<string>(value) << "\n";
    }
}

process(42);              // "Integer: 42"
process(3.14);            // "Double: 3.14"
process("hello");         // "String: hello"
```

### variant with Visitor Pattern

```cpp
struct Visitor {
    void operator()(int i) const {
        cout << "Integer: " << i << "\n";
    }
    
    void operator()(double d) const {
        cout << "Double: " << d << "\n";
    }
    
    void operator()(const string& s) const {
        cout << "String: " << s << "\n";
    }
};

variant<int, double, string> v = 42;
visit(Visitor(), v);  // "Integer: 42"

v = 3.14;
visit(Visitor(), v);  // "Double: 3.14"

v = string("hello");
visit(Visitor(), v);  // "String: hello"
```

### variant with Lambdas (C++20 or using overload trick)

```cpp
// C++17 overload trick
template<typename... Ts>
struct overload : Ts... { using Ts::operator()...; };
template<typename... Ts>
overload(Ts...) -> overload<Ts...>;

variant<int, double, string> v = 42;

// Visit with lambdas
visit(overload{
    [](int i) { cout << "Integer: " << i << "\n"; },
    [](double d) { cout << "Double: " << d << "\n"; },
    [](const string& s) { cout << "String: " << s << "\n"; }
}, v);  // "Integer: 42"
```

### Practical variant Example

```cpp
variant<int, string> parse_value(const string& input) {
    try {
        return stoi(input);  // Try as int
    } catch (...) {
        return input;        // Return as string
    }
}

auto result = parse_value("42");
if (holds_alternative<int>(result)) {
    cout << "Parsed as int: " << get<int>(result) << "\n";
} else {
    cout << "Parsed as string: " << get<string>(result) << "\n";
}
```

---

## STD::ANY

## 3.1 Type-Erased Storage with std::any

`std::any` can hold any copyable type.

### Basic any Usage

```cpp
#include <any>
#include <iostream>
using namespace std;

any value;

// Store different types
value = 42;
cout << any_cast<int>(value) << "\n";  // 42

value = 3.14;
cout << any_cast<double>(value) << "\n";  // 3.14

value = string("hello");
cout << any_cast<string>(value) << "\n";  // hello

// Check type
if (value.type() == typeid(string)) {
    cout << "It's a string\n";
}
```

### any with Type Checking

```cpp
any value = 42;

if (value.type() == typeid(int)) {
    cout << "Value: " << any_cast<int>(value) << "\n";
}

// Safe cast (throws if wrong type)
try {
    double d = any_cast<double>(value);  // Wrong type
} catch (const bad_any_cast& e) {
    cout << "Type mismatch: " << e.what() << "\n";
}

// Check before casting
if (value.type() == typeid(int)) {
    int i = any_cast<int>(value);
}
```

### any in Collections

```cpp
vector<any> data;
data.push_back(42);
data.push_back(3.14);
data.push_back(string("hello"));
data.push_back(vector<int>{1, 2, 3});

// Process
for (auto& item : data) {
    if (item.type() == typeid(int)) {
        cout << "Int: " << any_cast<int>(item) << "\n";
    } else if (item.type() == typeid(double)) {
        cout << "Double: " << any_cast<double>(item) << "\n";
    } else if (item.type() == typeid(string)) {
        cout << "String: " << any_cast<string>(item) << "\n";
    }
}
```

### any vs variant

```cpp
// variant<int, double, string>: Fixed types, type-safe
variant<int, double, string> v = 42;
cout << get<int>(v) << "\n";  // Type-safe, no runtime check needed

// any: Any type, runtime type checking
any a = 42;
cout << any_cast<int>(a) << "\n";  // Runtime check, potential exception

// Use variant when types are known
// Use any when types are truly dynamic
```

---

## STD::STRING_VIEW

## 4.1 Non-Owning String References

`std::string_view` provides efficient, zero-copy string operations.

### Basic string_view

```cpp
#include <string_view>
#include <iostream>
using namespace std;

string s = "Hello, World!";
string_view sv = s;

cout << sv << "\n";           // "Hello, World!"
cout << sv.length() << "\n";  // 13
cout << sv[0] << "\n";        // 'H'
cout << sv.data() << "\n";    // "Hello, World!"
```

### string_view from Different Sources

```cpp
// From std::string
string s = "hello";
string_view sv1 = s;

// From C-string
const char* cstr = "world";
string_view sv2 = cstr;

// From string literal
string_view sv3 = "test";

// Substring
string_view sv4 = sv1.substr(1, 3);  // "ell"
```

### string_view Operations

```cpp
string_view sv = "Hello, World!";

// Searching
cout << sv.find("World") << "\n";      // 7
cout << sv.find(',') << "\n";          // 5

// Comparing
cout << sv.compare("Hello, World!") << "\n";  // 0 (equal)

// Prefix/suffix
cout << sv.starts_with("Hello") << "\n";  // true
cout << sv.ends_with("!") << "\n";        // true

// Substrings
auto hello = sv.substr(0, 5);           // "Hello"
auto world = sv.substr(7);              // "World!"

// Remove prefix/suffix (C++20)
// sv.remove_prefix(7);
// sv.remove_suffix(1);
```

### Performance Benefits of string_view

```cpp
// OLD: Copy string
void process_old(const string& s) {
    // s might be copied
}

// NEW: No copy
void process_new(string_view sv) {
    // sv references the string, no copy
}

// Usage
string data = "important";
process_old(data);  // Might copy
process_new(data);  // No copy!

// Works with literals too
process_new("temporary");  // No copy, no allocation
```

### string_view Limitations

```cpp
string_view sv = "hello";

// What you CAN'T do:
// sv[0] = 'H';                    // ERROR - can't modify
// sv.resize(3);                   // ERROR - can't resize
// string s(sv);                   // OK - explicit conversion needed

// Works only while source exists
string_view sv2;
{
    string temp = "danger";
    sv2 = temp;
    // temp destroyed here
}
// sv2 now points to destroyed string - DANGER!

// Safe way: Make a copy if needed
string copy(sv2);
```

### string_view Use Cases

```cpp
// Parse tokens without copying
string_view parse_token(string_view& input) {
    size_t pos = input.find(' ');
    if (pos == string_view::npos) {
        string_view token = input;
        input = "";
        return token;
    }
    string_view token = input.substr(0, pos);
    input = input.substr(pos + 1);
    return token;
}

// Check file extensions efficiently
bool is_cpp_file(string_view filename) {
    return filename.ends_with(".cpp") || 
           filename.ends_with(".h") ||
           filename.ends_with(".hpp");
}

// Efficient URL parsing
void parse_url(string_view url) {
    size_t protocol_end = url.find("://");
    string_view protocol = url.substr(0, protocol_end);
    // ... continue parsing
}
```

---

## IF CONSTEXPR

## 5.1 Compile-Time Conditional Code

`if constexpr` allows branching at compile-time.

### Basic if constexpr

```cpp
#include <type_traits>

template<typename T>
void process(T value) {
    if constexpr (is_integral_v<T>) {
        cout << "Integer: " << value << "\n";
    } else if constexpr (is_floating_point_v<T>) {
        cout << "Float: " << value << "\n";
    } else {
        cout << "Other type\n";
    }
}

process(42);        // "Integer: 42" - int branch only compiled
process(3.14);      // "Float: 3.14" - double branch only compiled
process("hello");   // "Other type"
```

### if constexpr Advantages

```cpp
// Before C++17: SFINAE (complex, verbose)
template<typename T>
enable_if_t<is_integral_v<T>>
old_way(T x) { cout << "Int\n"; }

template<typename T>
enable_if_t<is_floating_point_v<T>>
old_way(T x) { cout << "Float\n"; }

// After C++17: if constexpr (clean, readable)
template<typename T>
void new_way(T x) {
    if constexpr (is_integral_v<T>) {
        cout << "Int\n";
    } else if constexpr (is_floating_point_v<T>) {
        cout << "Float\n";
    }
}
```

### if constexpr with Complex Logic

```cpp
template<typename T>
void serialize(const T& value) {
    if constexpr (is_arithmetic_v<T>) {
        // Serialize numbers efficiently
        write_binary(value);
    } else if constexpr (is_same_v<T, string>) {
        // Serialize strings
        write_string(value);
    } else if constexpr (requires { value.size(); }) {
        // Serialize containers
        for (const auto& item : value) {
            serialize(item);
        }
    }
}
```

### if constexpr with Concepts (C++20-like)

```cpp
template<typename T>
void print_info(const T& value) {
    cout << "Type: " << typeid(T).name() << "\n";
    
    if constexpr (requires { value.size(); }) {
        cout << "Size: " << value.size() << "\n";
    }
    
    if constexpr (requires { value.data(); }) {
        cout << "Data pointer available\n";
    }
    
    if constexpr (is_default_constructible_v<T>) {
        T temp;  // Can create temporary
    }
}
```

## 5.2 The Death of SFINAE?

`if constexpr` replaces SFINAE for *implementation details*, but not for *overload resolution*.

**SFINAE (Old Way - C++11/14):**
```cpp
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
gcd(T a, T b) { return b == 0 ? a : gcd(b, a % b); }

template <typename T>
typename std::enable_if<!std::is_integral<T>::value, T>::type
gcd(T a, T b) { static_assert(std::is_integral<T>::value, "GCD not for floats"); }
```

**if constexpr (New Way - C++17):**
```cpp
template <typename T>
T gcd(T a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return b == 0 ? a : gcd(b, a % b);
    } else {
        static_assert(always_false<T>, "GCD not for floats");
    }
}
```
*Result*: Much cleaner, easier to debug errors.

---

## FOLD EXPRESSIONS

## 6.1 Operating on Parameter Packs

Fold expressions simplify operations on variadic templates.

### Basic Fold Expressions

```cpp
#include <iostream>
using namespace std;

// Sum with fold
template<typename... Args>
int sum(Args... args) {
    return (... + args);  // Left fold: ((a + b) + c) + d
}

cout << sum(1, 2, 3, 4) << "\n";  // 10

// Product with fold
template<typename... Args>
int product(Args... args) {
    return (... * args);  // Left fold: ((a * b) * c) * d
}

cout << product(2, 3, 4) << "\n";  // 24

// Logical AND
template<typename... Args>
bool all_true(Args... args) {
    return (... && args);
}

cout << all_true(true, true, true) << "\n";      // true
cout << all_true(true, false, true) << "\n";     // false
```

### Fold Directions

```cpp
// Left fold: (... + args) = ((a + b) + c) + d
template<typename... Args>
int left_fold(Args... args) {
    return (... + args);
}

// Right fold: (args + ...) = a + (b + (c + d))
template<typename... Args>
int right_fold(Args... args) {
    return (args + ...);
}

// For addition, result is the same
left_fold(1, 2, 3);   // ((1 + 2) + 3) = 6
right_fold(1, 2, 3);  // (1 + (2 + 3)) = 6

// For subtraction, different!
// left: ((1 - 2) - 3) = -4
// right: (1 - (2 - 3)) = 2
```

### Fold with Default Value

```cpp
// No default value
template<typename... Args>
int sum_no_default(Args... args) {
    return (... + args);  // Error if no args
}

// With default value
template<typename... Args>
int sum_default(Args... args) {
    return (args + ... + 0);  // 0 if no args
}

sum_default();              // 0
sum_default(1, 2, 3);       // 6
```

### Practical Fold Examples

```cpp
// Print all arguments
template<typename... Args>
void print_all(Args... args) {
    ((cout << args << " "), ...);
}

print_all(1, "hello", 3.14, "world");
// Output: 1 hello 3.14 world

// Check if any true
template<typename... Args>
bool any_true(Args... args) {
    return (... || args);
}

any_true(false, false, true, false);  // true

// Maximum of values
template<typename... Args>
int maximum(Args... args) {
    return max({args...});  // Fold into initializer
}

cout << maximum(3, 1, 4, 1, 5, 9) << "\n";  // 9
```

## 6.2 Advanced Fold Expressions

The comma operator `,` is the most powerful fold operator. It evaluates LHS, discards it, then RHS.

### Calling Function on Pack
```cpp
template<typename... Args>
void log_all(Args... args) {
    (..., (cout << "[LOG] " << args << "\n")); 
}
```

### Validating All Arguments
```cpp
template<typename... Args>
bool validate_all(Args... args) {
    // Returns true if ALL args are valid
    return (... && (args.is_valid()));
}
```

---

## CLASS TEMPLATE ARGUMENT DEDUCTION (CTAD)

## 7.1 Automatic Template Type Deduction

CTAD allows deduction of class template parameters from constructor arguments.

### Before CTAD (C++14)

```cpp
#include <vector>
#include <map>
using namespace std;

// Must specify types explicitly
vector<int> v{1, 2, 3};
pair<string, int> p("hello", 42);
map<string, int> m{{"a", 1}, {"b", 2}};
```

### With CTAD (C++17)

```cpp
// Types deduced automatically!
vector v{1, 2, 3};              // Deduced as vector<int>
pair p("hello", 42);             // Deduced as pair<string, int>
map m{pair("a", 1), pair("b", 2)}; // Deduced as map<string, int>

// Works with custom classes too
template<typename T, typename U>
struct Pair {
    T first;
    U second;
};

Pair p{42, 3.14};  // Deduced as Pair<int, double>
```

### CTAD with Custom Deduction Guides

```cpp
template<typename T>
struct Container {
    vector<T> data;
};

// Without guide: Would fail
// Container c{1, 2, 3};  // ERROR - can't deduce T

// With deduction guide
template<typename T>
Container(initializer_list<T>) -> Container<T>;

// Now it works!
Container c{1, 2, 3};  // Deduced as Container<int>
```

### Practical CTAD Examples

```cpp
// Multiple parameters
template<typename T, typename U>
class Pair {
public:
    Pair(T first, U second) : first(first), second(second) {}
    T first;
    U second;
};

Pair p{1, "hello"};  // Deduced as Pair<int, const char*>

// Arrays
array a{1, 2, 3, 4, 5};  // Deduced as array<int, 5>

// Aggregates
struct Point {
    int x, y;
};

Point pt{10, 20};  // No deduction needed (aggregate), but works
```

### CTAD Benefits

```cpp
// Reduces verbosity
auto v1 = vector<int>{1, 2, 3};  // C++11 way
auto v2 = vector{1, 2, 3};       // C++17 way

// More readable with complex types
map m1{pair<const int, string>{1, "a"}};  // Verbose
map m2{pair{1, "a"}};                      // Clean

// Especially useful with templates
template<typename Iter>
auto process(Iter first, Iter last) {
    vector v(first, last);  // Deduced type!
    return v;
}
```

---

## STD::FILESYSTEM

## 8.1 Portable File System Operations

`std::filesystem` provides safe, portable file system access.

### Basic Path Operations

```cpp
#include <filesystem>
#include <iostream>
using namespace std;
namespace fs = filesystem;

// Create path
fs::path p1 = "/home/user/file.txt";
fs::path p2 = "relative/path/file.txt";

// Query components
cout << p1.filename() << "\n";      // "file.txt"
cout << p1.parent_path() << "\n";   // "/home/user"
cout << p1.extension() << "\n";     // ".txt"
cout << p1.stem() << "\n";          // "file"

// Normalize
cout << p1.lexically_normal() << "\n";  // Remove .. and .
```

### File Existence & Type Checking

```cpp
namespace fs = filesystem;

fs::path p = "/path/to/file";

if (fs::exists(p)) {
    cout << "Path exists\n";
}

if (fs::is_regular_file(p)) {
    cout << "Is a regular file\n";
    cout << "Size: " << fs::file_size(p) << " bytes\n";
}

if (fs::is_directory(p)) {
    cout << "Is a directory\n";
}

if (fs::is_symlink(p)) {
    cout << "Is a symbolic link\n";
}
```

### Directory Operations

```cpp
namespace fs = filesystem;

fs::path dir = "/path/to/directory";

// List directory contents
for (const auto& entry : fs::directory_iterator(dir)) {
    cout << entry.path().filename() << "\n";
    if (entry.is_directory()) {
        cout << "  (directory)\n";
    }
}

// Recursive directory listing
for (const auto& entry : fs::recursive_directory_iterator(dir)) {
    cout << entry.path().relative_path(dir) << "\n";
}
```

### File Operations

```cpp
namespace fs = filesystem;

fs::path src = "original.txt";
fs::path dst = "copy.txt";

// Copy file
fs::copy_file(src, dst);

// Move/rename
fs::rename(src, dst);

// Delete file
fs::remove(dst);

// Delete directory (must be empty)
fs::path dir = "empty_directory";
fs::remove(dir);

// Create directory
fs::path newdir = "new_directory";
fs::create_directory(newdir);

// Create nested directories
fs::create_directories("a/b/c/d");
```

### Practical filesystem Example

```cpp
#include <filesystem>
#include <fstream>
using namespace std;
namespace fs = filesystem;

// Find all C++ files in directory
void find_cpp_files(const fs::path& dir) {
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension();
            if (ext == ".cpp" || ext == ".h" || ext == ".hpp") {
                cout << entry.path() << "\n";
            }
        }
    }
}

// Count lines in a file
int count_lines(const fs::path& file) {
    ifstream f(file);
    int count = 0;
    string line;
    while (getline(f, line)) count++;
    return count;
}

// Safe file backup
void backup_file(const fs::path& original) {
    if (!fs::exists(original)) {
        throw runtime_error("File not found");
    }
    
    fs::path backup = original;
    backup.replace_extension(
        backup.extension().string() + ".bak"
    );
    
    fs::copy_file(original, backup, 
                  fs::copy_options::overwrite_existing);
}
```

## 8.5 Filesystem Deep Dive

### Exception-Free API
Most `std::filesystem` functions have an overload taking `std::error_code&` to avoid exceptions.

```cpp
std::error_code ec;
if (fs::exists("/tmp/ghost", ec)) {
    // ...
}
if (ec) {
    std::cerr << "Error: " << ec.message() << "\n";
}
```

### Space & Permissions
```cpp
auto space = fs::space("/");
cout << "Free: " << space.free / 1024 / 1024 << " MB\n";

fs::permissions("file.txt", 
    fs::perms::owner_read | fs::perms::owner_write,
    fs::perm_options::add);
```

---

## STD::INVOKE

## 9.1 Uniform Callable Invocation

`std::invoke` provides uniform way to call functions, methods, and functors.

### Basic invoke

```cpp
#include <functional>
#include <iostream>
using namespace std;

// Regular function
int add(int a, int b) { return a + b; }

// Invoke function
cout << invoke(add, 5, 3) << "\n";  // 8

// Function pointer
int (*fp)(int, int) = add;
cout << invoke(fp, 5, 3) << "\n";   // 8

// Lambda
auto lambda = [](int a, int b) { return a * b; };
cout << invoke(lambda, 5, 3) << "\n";  // 15

// Functor
struct Multiplier {
    int operator()(int a, int b) const { return a * b; }
};

Multiplier m;
cout << invoke(m, 5, 3) << "\n";  // 15
```

### invoke with Methods

```cpp
struct Calculator {
    int value = 0;
    
    int add(int x) { return value + x; }
    int multiply(int x) const { return value * x; }
};

Calculator calc{10};

// Invoke member function
cout << invoke(&Calculator::add, calc, 5) << "\n";     // 15
cout << invoke(&Calculator::multiply, calc, 3) << "\n"; // 30

// With pointer
Calculator* ptr = &calc;
cout << invoke(&Calculator::add, ptr, 5) << "\n";      // 15

// With shared_ptr, unique_ptr
auto up = make_unique<Calculator>();
up->value = 20;
cout << invoke(&Calculator::add, up.get(), 5) << "\n"; // 25
```

### invoke with Data Members

```cpp
struct Person {
    string name;
    int age;
};

Person p{"Alice", 30};

// Invoke data member access
cout << invoke(&Person::name, p) << "\n";  // "Alice"
cout << invoke(&Person::age, p) << "\n";   // 30

// Modify through invoke
invoke(&Person::age, p) = 31;
cout << p.age << "\n";  // 31
```

### Practical invoke Pattern

```cpp
// Create callable wrapper that works with anything
template<typename Func, typename... Args>
auto call_wrapper(Func&& f, Args&&... args) {
    return invoke(forward<Func>(f), forward<Args>(args)...);
}

// Use with different callables
call_wrapper(add, 5, 3);              // Function
call_wrapper(lambda, 5, 3);           // Lambda
call_wrapper(&Calculator::add, calc, 5); // Member function
```

---

## PARALLEL ALGORITHMS

## 10.1 Parallel Algorithm Execution

C++17 adds parallel execution to standard algorithms.

### Execution Policies

```cpp
#include <algorithm>
#include <execution>
#include <vector>
using namespace std;

vector<int> v = {5, 2, 8, 1, 9, 3};

// Sequential (traditional)
sort(v.begin(), v.end());

// Parallel (may use multiple threads)
sort(execution::par, v.begin(), v.end());

// Parallel unsequenced (even less ordering guarantee)
sort(execution::par_unseq, v.begin(), v.end());

// Sequenced unsequenced (no vectorization)
sort(execution::unseq, v.begin(), v.end());
```

### Parallel Algorithm Examples

```cpp
#include <algorithm>
#include <execution>
#include <numeric>
#include <vector>

vector<int> v = {1, 2, 3, 4, 5};

// Parallel transform
transform(execution::par, v.begin(), v.end(), v.begin(),
    [](int x) { return x * 2; });

// Parallel accumulate
int sum = reduce(execution::par, v.begin(), v.end());

// Parallel find_if
auto it = find_if(execution::par, v.begin(), v.end(),
    [](int x) { return x > 5; });

// Parallel count_if
int even_count = count_if(execution::par, v.begin(), v.end(),
    [](int x) { return x % 2 == 0; });
```

### Performance Considerations

```cpp
vector<int> large(1000000);

// Sequential: Simple, predictable
sort(large.begin(), large.end());

// Parallel: Faster for large data (overhead for small)
sort(execution::par, large.begin(), large.end());

// Parallel unsequenced: Allows compiler optimizations
sort(execution::par_unseq, large.begin(), large.end());
```

---

## CORE LANGUAGE FEATURES

## 11.1 Additional C++17 Features

### Nested namespaces

```cpp
// C++14
namespace A {
    namespace B {
        namespace C {
            int x = 42;
        }
    }
}

// C++17
namespace A::B::C {
    int x = 42;
}
```

### Inline Variables

```cpp
// Header file
inline int global_var = 42;  // Definition, can be in header
inline vector<int> global_vec;

// No ODR (One Definition Rule) violation
// Can include this header in multiple translation units
```

### Structured Exception Handling (still mostly the same, but with improvements)

```cpp
try {
    // Code
} catch (const exception& e) {
    cout << e.what() << "\n";
}
```

### Deduced Return Types in Lambdas

```cpp
auto lambda = [](int x) -> auto {  // C++17
    return x * 2;  // Return type deduced
};
```

### std::byte

```cpp
#include <cstddef>

byte b1{42};
byte b2 = 0xAB_B;  // Literal
cout << (int)b1 << "\n";  // Cast to see value
```

---

## LIBRARY IMPROVEMENTS

## 12.1 STL Enhancements in C++17

### std::optional Algorithms

```cpp
optional<int> opt{42};

opt.and_then([](int x) -> optional<int> {
    return x * 2;
}).or_else([]() { return optional<int>(0); });
```

### Improved Algorithms

```cpp
vector<int> v = {1, 2, 3, 4, 5};

// uninitialized operations now work with ranges
vector<int> v2(5);
uninitialized_copy(v.begin(), v.end(), v2.begin());

// reduce (like accumulate but parallel-friendly)
int sum = reduce(v.begin(), v.end());
```

### std::charconv

```cpp
#include <charconv>

// Fast, exception-safe number conversion
int x = 42;
char buffer[100];
auto result = to_chars(buffer, buffer + 100, x);

string str("123");
int value;
auto [ptr, ec] = from_chars(str.data(), str.data() + str.size(), value);
if (ec == errc()) {
    cout << value << "\n";  // 123
}
```

---

## POLYMORPHIC MEMORY RESOURCES (PMR)

## 13.1 Introduction to std::pmr

C++17 introduces `std::pmr` (Polymorphic Memory Resources) in `<memory_resource>`, enabling efficient, customizable memory management without changing container types.

### The Problem with Traditional Allocators
Traditional allocators are part of the type signature: `std::vector<int, MyAlloc<int>>` is a different type from `std::vector<int>`.

`std::pmr` erases the allocator type, allowing containers with different allocation strategies to be used interchangeably.

```cpp
#include <vector>
#include <memory_resource>
#include <iostream>

// Use pmr namespace
namespace pmr = std::pmr;

void process_data(const pmr::vector<int>& data) {
    // Works with ANY allocator (stack, heap, pool)
    for (int x : data) std::cout << x << " ";
}

int main() {
    // 1. Default allocator (Heap)
    pmr::vector<int> heap_vec = {1, 2, 3};
    process_data(heap_vec);
    
    // 2. Stack allocator (Monotonic Buffer)
    std::array<std::byte, 1024> buffer;
    pmr::monotonic_buffer_resource pool{
        buffer.data(), buffer.size(), pmr::null_memory_resource()
    };
    
    pmr::vector<int> stack_vec(&pool);
    stack_vec.push_back(4);
    stack_vec.push_back(5);
    process_data(stack_vec);
    
    return 0;
}
```

## 13.2 Memory Resources

### Standard Memory Resources

```cpp
#include <memory_resource>

// 1. new_delete_resource (Global Heap)
auto* heap = std::pmr::new_delete_resource();

// 2. null_memory_resource (Throws bad_alloc)
auto* null = std::pmr::null_memory_resource();

// 3. monotonic_buffer_resource (Fast, no deallocation)
// Very fast for building complex structures, deallocates all at once
std::pmr::monotonic_buffer_resource fast_pool(heap);

// 4. synchronized_pool_resource (Thread-safe pool)
// Good for many small allocations of same size
std::pmr::synchronized_pool_resource thread_safe_pool(heap);

// 5. unsynchronized_pool_resource (Single-thread pool)
// Fastest for single-threaded small allocations
std::pmr::unsynchronized_pool_resource local_pool(heap);
```

### Chaining Resources

Memory resources can be chained. If a pool runs out of memory, it requests more from an "upstream" resource.

```cpp
#include <memory_resource>
#include <vector>

void chaining_example() {
    // Buffer on stack
    std::array<std::byte, 256> buffer;
    
    // Primary: Use stack buffer
    // Upstream: If buffer full, go to heap
    std::pmr::monotonic_buffer_resource mem_res(
        buffer.data(), buffer.size(), std::pmr::new_delete_resource()
    );
    
    std::pmr::vector<int> vec(&mem_res);
    
    // These go to stack buffer
    for (int i = 0; i < 50; i++) vec.push_back(i);
    
    // If we exceed buffer, it silently falls back to heap
}
```

### Performance Benefits

`std::pmr` allows easy implementation of **Arena Allocation** or **Stack Allocation** for standard containers, which can provide massive performance gains (cache locality, no malloc overhead) for short-lived complex data structures.

---

## C++17 BEST PRACTICES

## What's Better with C++17

```cpp
// 1. Use structured bindings to unpack
auto [x, y, z] = tuple{1, 2, 3};

// 2. Use optional for nullable values
optional<int> result = process();
if (result) { cout << result.value() << "\n"; }

// 3. Use variant for type-safe unions
variant<int, string> value = 42;

// 4. Use string_view for non-owning strings
void process(string_view sv);  // No copy!

// 5. Use if constexpr for compile-time branching
if constexpr (is_integral_v<T>) { }

// 6. Use fold expressions for parameter packs
auto sum = (... + args);

// 7. Use filesystem for file operations
for (const auto& entry : fs::directory_iterator(dir)) { }

// 8. Use invoke for uniform callable invocation
invoke(func, args...);

// 9. Use parallel algorithms for performance
sort(execution::par, v.begin(), v.end());

// 10. Use CTAD for cleaner code
vector v{1, 2, 3};  // Not vector<int>{...}
```


---

