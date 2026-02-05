# C++14 ENHANCEMENTS


## C++14 Overview & Philosophy

C++14 (finalized in 2014) is a **refinement and maintenance release** of C++11.

### Timeline & Context
- **2011**: C++11 released (revolutionary)
- **2014**: C++14 released (refinement + useful features)
- **2017**: C++17 released (significant improvements)
- **2020**: C++20 released (revolutionary again)

### C++14 Philosophy
- **Smaller, focused** improvements rather than revolution
- **Fix** C++11 issues and limitations
- **Enhance** usability and convenience
- **Add** frequently-requested features
- **Simplify** compile-time computation

### Key Features
1. Generic lambdas with `auto` parameters
2. Return type deduction for all functions
3. Binary literals and digit separators
4. std::make_unique
5. Relaxed constexpr rules
6. Variable templates
7. Library improvements

### Why C++14 Matters
While smaller than C++11, C++14 makes C++11 more practical:
-  Fixes usability issues
-  Adds convenient features
-  Improves compile-time computation
-  Better template support
-  More standard library features

---

## GENERIC LAMBDAS

## 1.1 Auto Parameters in Lambdas

C++14 allows `auto` as lambda parameters, creating **generic lambdas**.

### Basic Generic Lambda

```cpp
#include <iostream>
#include <vector>
using namespace std;

// C++11: Type-specific lambda
auto add11 = [](int a, int b) { return a + b; };
cout << add11(5, 3) << "\n";           // 8
// cout << add11(2.5, 3.5) << "\n";   // ERROR - int only

// C++14: Generic lambda with auto
auto add14 = [](auto a, auto b) { return a + b; };
cout << add14(5, 3) << "\n";           // 8 (int)
cout << add14(2.5, 3.5) << "\n";       // 6 (double)
cout << add14(string("Hello"), string(" World")) << "\n";  // "Hello World"
```

### Generic Lambda Deduction

```cpp
// Each auto parameter is independently deduced
auto process = [](auto x, auto y) {
    // x and y can be different types
    cout << x << ", " << y << "\n";
};

process(5, 3.14);              // int, double
process("hello", 42);          // const char*, int
process(3.14, "world");        // double, const char*
```

### Generic Lambdas with std::vector

```cpp
vector<int> vi = {1, 2, 3};
vector<double> vd = {1.1, 2.2, 3.3};
vector<string> vs = {"a", "b", "c"};

// Single generic lambda works with all containers
auto print = [](auto val) {
    cout << val << " ";
};

for_each(vi.begin(), vi.end(), print);
cout << "\n";

for_each(vd.begin(), vd.end(), print);
cout << "\n";

for_each(vs.begin(), vs.end(), print);
cout << "\n";
```

### Generic Lambdas with Algorithms

```cpp
// Works with any type supporting operator*
auto square = [](auto x) { return x * x; };

vector<int> vi = {1, 2, 3};
vector<double> vd = {1.5, 2.5, 3.5};

transform(vi.begin(), vi.end(), vi.begin(), square);
// vi: {1, 4, 9}

transform(vd.begin(), vd.end(), vd.begin(), square);
// vd: {2.25, 6.25, 12.25}
```

### Generic Lambda Compile-Time Behavior

```cpp
// Type checking still happens at compile time
auto multiply = [](auto a, auto b) { return a * b; };

cout << multiply(5, 3) << "\n";        // 15 (int)
cout << multiply(2.5, 3.0) << "\n";    // 7.5 (double)

// This would compile-time error if * not defined:
// multiply("a", "b");                // ERROR - string doesn't support *
```

### When to Use Generic Lambdas

```cpp
// Good use case: Works with any comparable type
auto find_min = [](auto a, auto b) { return a < b ? a : b; };

int min_int = find_min(5, 3);          // 3
double min_double = find_min(2.5, 1.5);  // 1.5
string min_str = find_min("cat", "apple");  // "apple"

// Bad use case: Type-specific logic
auto process = [](auto x) {
    if (is_integral_v<decltype(x)>) {
        cout << "Integer\n";
    } else if (is_floating_point_v<decltype(x)>) {
        cout << "Float\n";
    }
    // Too complex - use template or function overloads instead
};
```

---

## RETURN TYPE DEDUCTION FOR ALL FUNCTIONS

## 2.1 Return Type Deduction (C++14 Enhancement)

C++11 allowed return type deduction with `-> auto`, but C++14 simplifies it.

### Basic Return Type Deduction

```cpp
// C++11: Must use trailing return type
auto add_11(int a, int b) -> int { return a + b; }
auto divide_11(double a, double b) -> double { return a / b; }

// C++14: Can deduce from return statement
auto add_14(int a, int b) { return a + b; }      // Returns int
auto divide_14(double a, double b) { return a / b; }  // Returns double

auto get_string() { return string("hello"); }    // Returns string
auto get_vector() { return vector<int>{1, 2, 3}; }  // Returns vector<int>
```

### Multiple Return Statements

```cpp
// C++14: All returns must be consistent type
auto absolute(int x) {
    if (x >= 0) {
        return x;          // int
    } else {
        return -x;         // Must also be int
    }
}

// ERROR: Different return types
// auto mixed(int x) {
//     if (x > 0) {
//         return x;      // int
//     } else {
//         return 3.14;   // double - ERROR!
//     }
// }
```

### Return Type Deduction with Complex Types

```cpp
#include <vector>
using namespace std;

// Deduce vector
auto get_data() {
    return vector<int>{1, 2, 3, 4, 5};
}

// Deduce map
auto get_map() {
    return map<string, int>{{"a", 1}, {"b", 2}};
}

// Deduce function
auto get_comparator() {
    return [](int a, int b) { return a > b; };
}

// Works seamlessly
vector<int> v = get_data();
map<string, int> m = get_map();
auto cmp = get_comparator();
```

### Return Type Deduction in Templates

```cpp
template<typename T, typename U>
auto add(T a, U b) {
    return a + b;  // Type deduced from a + b
}

cout << add(5, 3) << "\n";              // int
cout << add(2.5, 3.0) << "\n";          // double
cout << add(5, 3.14) << "\n";           // double (int + double = double)

// Return type varies by input types
static_assert(is_same_v<decltype(add(5, 3)), int>);
static_assert(is_same_v<decltype(add(5.0, 3)), double>);
```

### Recursion with Return Type Deduction

```cpp
// C++14: Recursive functions can use auto return type
// (But compiler may need hints for some cases)

auto factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

cout << factorial(5) << "\n";  // 120

// More complex recursion
auto fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

cout << fibonacci(10) << "\n";  // 55
```

### Benefits of Return Type Deduction

```cpp
// Less redundant code
// Before: Must specify return type
int add_old(int a, int b) -> int { return a + b; }

// After: Auto-deduced
auto add_new(int a, int b) { return a + b; }

// Refactoring friendly - type changes automatically
auto get_value() { return 42; }        // int
// Later: auto get_value() { return 3.14; }  // Changes to double - easy!
```

---

## AUTO FOR VARIABLES IN LAMBDAS

## 3.1 Init Capture with Auto (C++14)

Lambda capture allows variables to be initialized inside the capture list.

### Basic Init Capture

```cpp
#include <memory>
#include <iostream>
using namespace std;

// Create a unique_ptr (move-only type)
auto ptr = make_unique<int>(42);

// C++11: Can't capture unique_ptr (can't copy)
// auto lambda11 = [ptr]() { };  // ERROR - can't copy unique_ptr

// C++14: Init capture allows move
auto lambda = [ptr = move(ptr)]() {
    cout << *ptr << "\n";
};

lambda();  // 42
// ptr is now nullptr (moved into lambda)
```

### Init Capture with Values

```cpp
int original = 10;

// Capture with transformation
auto lambda = [copy = original * 2]() {
    return copy;  // 20
};

cout << lambda() << "\n";  // 20
cout << original << "\n";  // Still 10 (original unchanged)

// Useful for expensive copies
vector<int> large_vector = {1, 2, 3, 4, 5};
auto process = [copy = vector<int>(large_vector)]() {
    // Use copy (independent of large_vector)
};
```

### Init Capture with Move

```cpp
class Resource {
public:
    Resource() { cout << "Created\n"; }
    ~Resource() { cout << "Destroyed\n"; }
    void use() { cout << "Using\n"; }
};

auto res = make_unique<Resource>();

// Move resource into lambda
auto lambda = [res = move(res)]() {
    if (res) {
        res->use();
    }
};

lambda();  // Using, Destroyed
// res is now nullptr
```

### Init Capture with Complex Types

```cpp
#include <map>

map<string, int> data{{"a", 1}, {"b", 2}};

// Copy map into lambda
auto process = [data_copy = data]() {
    for (const auto& [k, v] : data_copy) {
        cout << k << ": " << v << "\n";
    }
};

// Modify copy without affecting original
auto modify = [data_copy = move(data)]() {
    data_copy["c"] = 3;
    // data is moved
};

modify();
```

### Init Capture Patterns

```cpp
// Pattern 1: Capture with computation
auto compute = [val = 2 + 3]() { return val; };  // val = 5

// Pattern 2: Capture with function call
auto get_timestamp = [time = chrono::high_resolution_clock::now()]() {
    return time;
};

// Pattern 3: Capture with complex initialization
auto setup = [config = []() {
    map<string, string> m;
    m["key"] = "value";
    return m;
}()]() {
    // config is initialized with map
};

// Pattern 4: Move-only types
auto factory = [ptr = make_unique<int>(42)]() {
    return *ptr;
};
```

---

## BINARY LITERALS & DIGIT SEPARATORS

## 4.1 Binary Literals

C++14 introduces `0b` prefix for binary literals.

### Binary Literal Syntax

```cpp
#include <iostream>
using namespace std;

// Decimal (C++98)
int dec = 42;

// Hexadecimal (C++98)
int hex = 0x2A;

// Octal (C++98)
int oct = 052;

// Binary (C++14)
int bin = 0b101010;

cout << dec << "\n";  // 42
cout << hex << "\n";  // 42
cout << oct << "\n";  // 42
cout << bin << "\n";  // 42
```

### Binary Literals Use Cases

```cpp
// Bitwise operations are clearer with binary
unsigned char flags = 0b11010110;
unsigned char mask = 0b00001111;

unsigned char result = flags & mask;  // Much clearer than 0xD6 & 0x0F

// Single bit operations
unsigned int option1 = 0b00000001;
unsigned int option2 = 0b00000010;
unsigned int option3 = 0b00000100;

unsigned int enabled = option1 | option3;  // 0b00000101

// Permission bits
unsigned char read = 0b100;    // 4
unsigned char write = 0b010;   // 2
unsigned char execute = 0b001; // 1

unsigned char permissions = read | write;
```

## 4.2 Digit Separators

C++14 allows single quotes `'` as digit separators for readability.

### Digit Separator Examples

```cpp
// Large numbers are clearer with separators
long large = 1'000'000'000;      // One billion
double pi = 3.141'592'653;       // Pi

// Binary with separators (very clear)
unsigned char bits = 0b1111'0000;
unsigned short value = 0xDEAD'BEEF;

// All numeric literals support separators
int decimal = 123'456'789;
long long big = 9'223'372'036'854'775'807;  // Max int64

double d = 1'000.123'456;                   // Works with decimals
double e = 1.234'567e3;                     // Works with exponents
```

### Readability Improvement

```cpp
// Before (hard to count zeros)
unsigned int ip = 192168001001;  // What is this?

// After (clear structure)
unsigned int ip = 192'168'001'001;  // IP address: 192.168.1.1

// Before (hard to verify)
long big = 9223372036854775807;

// After (easy to verify)
long big = 9'223'372'036'854'775'807;  // Max int64

// Before (unclear magnitude)
double money = 1000000000;

// After (clear)
double money = 1'000'000'000;  // One billion
```

### Digit Separator Rules

```cpp
// Valid usage
int a = 1'000'000;
int b = 0xDEAD'BEEF;
int c = 0b1111'0000;

// NOT at start or end
// int bad1 = '123;          // ERROR
// int bad2 = 123';          // ERROR

// NOT adjacent to decimal point or exponent
// double bad3 = 1.'5;       // ERROR
// double bad4 = 1e'10;      // ERROR

// Multiple separators are OK
int d = 1'000'000'000;
int e = 0xFF'FF'FF'FF;
```

---

## STD::MAKE_UNIQUE

## 5.1 std::make_unique (C++14)

`std::make_unique` creates `unique_ptr` safely and efficiently.

### Before C++14

```cpp
#include <memory>
using namespace std;

// C++11: Two-step process
unique_ptr<int> ptr1(new int(42));
unique_ptr<string> ptr2(new string("hello"));
unique_ptr<vector<int>> ptr3(new vector<int>{1, 2, 3});

// Problem: New and unique_ptr are separate
// If exception between new and unique_ptr, memory leaks
```

### With std::make_unique

```cpp
#include <memory>
using namespace std;

// C++14: One-step, exception-safe
auto ptr1 = make_unique<int>(42);
auto ptr2 = make_unique<string>("hello");
auto ptr3 = make_unique<vector<int>>(initializer_list<int>{1, 2, 3});

// Automatically determines type
// Exception-safe: if constructor throws, no memory leak
```

### make_unique with Classes

```cpp
class Person {
public:
    string name;
    int age;
    
    Person(string n, int a) : name(n), age(a) {
        cout << "Person created\n";
    }
    ~Person() {
        cout << "Person destroyed\n";
    }
};

// Create unique_ptr with constructor arguments
auto person = make_unique<Person>("Alice", 30);
cout << person->name << " is " << person->age << "\n";

// Automatic cleanup when going out of scope
```

### make_unique with Arrays (C++20)

```cpp
// C++14: Dynamic sized arrays need manual approach
unique_ptr<int[]> arr1(new int[10]);

// C++20: make_unique supports arrays
// auto arr2 = make_unique<int[]>(10);  // C++20 only

// For C++14, use the manual approach:
auto arr3 = make_unique<int[]>();  // C++20
```

### make_unique vs new

```cpp
// Old way (manual, error-prone)
function<unique_ptr<int>()> factory_old = []() {
    return unique_ptr<int>(new int(42));
};

// New way (cleaner, safer)
function<unique_ptr<int>()> factory_new = []() {
    return make_unique<int>(42);
};

// Exception safety benefit:
class Dangerous {
public:
    Dangerous(unique_ptr<Resource> r) : resource(move(r)) { }
private:
    unique_ptr<Resource> resource;
};

// Safe: If Dangerous constructor throws, r is still managed
auto danger = make_unique<Dangerous>(make_unique<Resource>());

// Unsafe: If Dangerous constructor throws, new Resource() leaks
// auto danger = unique_ptr<Dangerous>(
//     new Dangerous(unique_ptr<Resource>(new Resource())));
```

### make_unique Best Practices

```cpp
// Prefer make_unique over new + unique_ptr
// Exception-safe
auto ptr1 = make_unique<MyClass>(arg1, arg2);

// More concise
auto ptr2 = make_unique<MyClass>();

// Automatic type deduction
auto ptr3 = make_unique<string>("hello");

// Use in containers
vector<unique_ptr<Resource>> resources;
resources.push_back(make_unique<Resource>());
resources.push_back(make_unique<Resource>());
// Automatic cleanup when vector destroyed
```

---

## RELAXED CONSTEXPR RESTRICTIONS

## 6.1 Enhanced constexpr Functions

C++14 relaxes constexpr restrictions, allowing more complex compile-time computation.

### C++11 constexpr Limitations

```cpp
// C++11: constexpr function must have exactly one statement
constexpr int square_11(int x) {
    return x * x;  // Only return statement allowed
}

// C++11: Can't use local variables or loops
// constexpr int factorial_11(int n) {
//     int result = 1;           // ERROR - variable not allowed
//     for (int i = 2; i <= n; i++) {  // ERROR - loops not allowed
//         result *= i;
//     }
//     return result;
// }
```

### C++14 constexpr Enhancements

```cpp
// C++14: Local variables allowed
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

cout << factorial(5) << "\n";  // Computed at compile-time if possible

// Compile-time constant
int arr[factorial(5)];  // Array of size 120

// C++14: More complex logic allowed
constexpr bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

static_assert(is_prime(7));    // Compile-time check
static_assert(!is_prime(4));   // Compile-time check
```

### C++14 constexpr Features

```cpp
// Control flow statements
constexpr int abs_diff(int a, int b) {
    if (a > b) {
        return a - b;
    } else {
        return b - a;
    }
}

// Multiple return points
constexpr int sign(int x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

// Loops
constexpr int sum_range(int start, int end) {
    int total = 0;
    for (int i = start; i < end; i++) {
        total += i;
    }
    return total;
}

cout << sum_range(1, 10) << "\n";  // 45

// Fibonacci with better performance
constexpr int fib(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int next = a + b;
        a = b;
        b = next;
    }
    return b;
}

cout << fib(20) << "\n";  // 6765
```

### constexpr Still Has Limitations

```cpp
// C++14: Still can't do
// - Dynamic memory allocation (new/delete)
// - Floating-point in some contexts (limited)
// - Most library functions

constexpr void* bad() {
    // return new int(42);  // ERROR
}

// But can call other constexpr functions
constexpr int helper() { return 42; }
constexpr int caller() {
    return helper() * 2;  // OK
}
```

### Practical constexpr Uses

```cpp
// Compile-time lookup table
constexpr int digit_to_value(char d) {
    if (d >= '0' && d <= '9') return d - '0';
    if (d >= 'a' && d <= 'f') return d - 'a' + 10;
    if (d >= 'A' && d <= 'F') return d - 'A' + 10;
    return -1;
}

// Compile-time string parsing
constexpr int hex_to_int(const char* str) {
    int result = 0;
    for (int i = 0; str[i]; i++) {
        int digit = digit_to_value(str[i]);
        if (digit < 0) break;
        result = result * 16 + digit;
    }
    return result;
}

constexpr int hex_value = hex_to_int("FF");  // 255, computed at compile-time

// Compile-time array generation
constexpr int powers[10] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

// Compile-time validation
constexpr bool validate_date(int year, int month, int day) {
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
    return true;
}

static_assert(validate_date(2024, 12, 25));
```

---

## VARIABLE TEMPLATES

## 7.1 Template Variables (C++14)

Variables can be templates, not just functions and classes.

### Basic Variable Template

```cpp
#include <iostream>
using namespace std;

// Template variable
template<typename T>
constexpr T pi = T(3.141592653589793);

// Use with different types
cout << pi<double> << "\n";        // 3.14159 (double)
cout << pi<float> << "\n";         // 3.14159 (float)
cout << pi<int> << "\n";           // 3 (int)

// Can use in computations
double circle_area = pi<double> * 5 * 5;  // Area of circle with radius 5
float sphere_volume = (4.0/3.0) * pi<float> * 3 * 3 * 3;
```

### Variable Template with Type Traits

```cpp
#include <type_traits>

// Type trait as variable template
template<typename T>
constexpr bool is_integral_v = is_integral<T>::value;

template<typename T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;

// Usage (cleaner than ::value)
if (is_integral_v<int>) { }        // true
if (is_integral_v<double>) { }     // false
if (is_floating_point_v<double>) { }  // true
```

### Useful Variable Templates

```cpp
// Concept-like variable template
template<typename T>
constexpr bool is_arithmetic_type = 
    is_integral_v<T> || is_floating_point_v<T>;

static_assert(is_arithmetic_type<int>);
static_assert(is_arithmetic_type<double>);
// static_assert(is_arithmetic_type<string>);  // false

// Size information
template<typename T>
constexpr size_t sizeof_v = sizeof(T);

cout << sizeof_v<int> << "\n";      // 4
cout << sizeof_v<double> << "\n";   // 8

// Min/max values
template<typename T>
constexpr T max_value = numeric_limits<T>::max();

template<typename T>
constexpr T min_value = numeric_limits<T>::min();

cout << max_value<int> << "\n";
cout << max_value<unsigned char> << "\n";
```

### C++17 Standard Variable Templates

```cpp
// C++17 standard library additions
#include <type_traits>

// These are now variable templates in C++17
is_integral_v<int>;            // true
is_floating_point_v<double>;   // true
is_same_v<int, int>;           // true
remove_const_v<const int>;     // int
is_pointer_v<int*>;            // true

// Work like the old ::value but more concise
// is_integral<int>::value;      // Old way
is_integral_v<int>;            // New way (C++17)
```

---

## AGGREGATE MEMBER INITIALIZATION

## 8.1 Extended Aggregate Initialization

C++14 extends what can be aggregate-initialized.

### Basic Aggregate Initialization (C++98)

```cpp
#include <iostream>
using namespace std;

struct Point {
    int x;
    int y;
};

// C++98: Brace initialization
Point p1 = {10, 20};
Point p2{30, 40};

cout << p1.x << ", " << p1.y << "\n";  // 10, 20
```

### With Base Classes (C++14)

```cpp
struct Base {
    int b;
};

struct Derived : Base {
    int d;
};

// C++14: Can initialize base class members
Derived obj{1, 2};  // b=1, d=2
cout << obj.b << ", " << obj.d << "\n";  // 1, 2
```

### Nested Aggregates

```cpp
struct Address {
    string street;
    string city;
};

struct Person {
    string name;
    int age;
    Address address;
};

// Nested initialization
Person p{"Alice", 30, {"123 Main St", "NYC"}};

cout << p.name << " lives at " << p.address.street << "\n";
```

### C++14 vs C++11 Differences

```cpp
struct Point {
    int x = 0;  // Default member initializer (C++11)
    int y = 0;
};

// C++11: Default initializer sets x=0, y=0
Point p1;           // x=0, y=0
Point p2{};         // x=0, y=0

// Explicit initialization
Point p3{10, 20};   // x=10, y=20

// Partial initialization with defaults
// Behavior similar between C++11 and C++14
```

---

## MEMBER FUNCTION REF/CONST-REF QUALIFIERS

## 9.1 Lvalue vs Rvalue Member Functions

C++11 introduced, C++14 standardized: member functions can be qualified as `&` or `&&`.

### Member Function Overloading

```cpp
#include <iostream>
#include <string>
using namespace std;

class Text {
private:
    string data;

public:
    Text(string s) : data(s) {}

    // For lvalue (normal objects)
    string& get_data() & {
        cout << "Lvalue version\n";
        return data;
    }

    // For rvalue (temporaries)
    string get_data() && {
        cout << "Rvalue version\n";
        return move(data);
    }

    // Const lvalue
    const string& get_data() const& {
        cout << "Const lvalue version\n";
        return data;
    }
};

int main() {
    Text t("hello");
    
    // Calls lvalue version
    auto& result1 = t.get_data();      // "Lvalue version"
    
    // Calls const lvalue version
    const auto& result2 = t.get_data();  // "Const lvalue version"
    
    // Calls rvalue version
    auto result3 = Text("world").get_data();  // "Rvalue version"
    
    return 0;
}
```

### Practical Use Case

```cpp
class Vector {
private:
    int* data;
    int size;

public:
    Vector() : data(nullptr), size(0) {}
    Vector(int n) : data(new int[n]), size(n) {}

    // Cheap copy for lvalue - return reference
    int* get_data() & {
        return data;
    }

    // Cheap move for rvalue - return by value
    int* get_data() && {
        int* tmp = data;
        data = nullptr;
        return tmp;
    }

    ~Vector() { delete[] data; }
};

Vector createVector(int n) {
    return Vector(n);
}

int main() {
    Vector v(100);
    
    // Lvalue: efficient reference
    int* ptr1 = v.get_data();
    
    // Rvalue: efficient move
    int* ptr2 = createVector(100).get_data();
    
    return 0;
}
```

### Const/Volatile Combinations

```cpp
class Object {
public:
    // All combinations possible:
    void method() & { }           // Lvalue
    void method() const& { }      // Const lvalue
    void method() && { }          // Rvalue
    void method() const&& { }     // Const rvalue
    void method() volatile& { }   // Volatile lvalue
    // ... more combinations
};
```

---

## STD::INTEGER_SEQUENCE

## 10.1 Compile-Time Integer Sequences

`std::integer_sequence` provides compile-time sequences of integers.

### Basic Usage

```cpp
#include <utility>
#include <iostream>
using namespace std;

// Create a sequence 0, 1, 2, 3, 4
using seq = integer_sequence<int, 0, 1, 2, 3, 4>;

// More practical: Generate sequence
using seq5 = make_integer_sequence<int, 5>;  // 0, 1, 2, 3, 4

// Use with function
template<typename T, T... Is>
void print_sequence(integer_sequence<T, Is...>) {
    ((cout << Is << " "), ...);  // C++17 fold expression
    cout << "\n";
}

print_sequence(make_integer_sequence<int, 10>());  // 0 1 2 3 4 5 6 7 8 9
```

### Unpacking Tuple

```cpp
#include <tuple>

// Convert tuple to function arguments
template<typename F, typename Tuple, size_t... Is>
auto apply_impl(F&& f, Tuple&& t, index_sequence<Is...>) {
    return forward<F>(f)(get<Is>(forward<Tuple>(t))...);
}

template<typename F, typename Tuple>
auto apply(F&& f, Tuple&& t) {
    return apply_impl(
        forward<F>(f),
        forward<Tuple>(t),
        make_index_sequence<tuple_size_v<decay_t<Tuple>>>()
    );
}

// Usage
auto add = [](int a, int b, int c) { return a + b + c; };
auto result = apply(add, make_tuple(1, 2, 3));  // 6
```

### Array Initialization

```cpp
template<typename T, size_t N, size_t... Is>
void fill_array_impl(array<T, N>& arr, index_sequence<Is...>) {
    (..., (arr[Is] = Is * Is));  // Fill with squares
}

template<typename T, size_t N>
void fill_array(array<T, N>& arr) {
    fill_array_impl(arr, make_index_sequence<N>());
}

array<int, 5> arr;
fill_array(arr);
// arr: {0, 1, 4, 9, 16}
```

---

## LIBRARY IMPROVEMENTS

## 11.1 STL Enhancements in C++14

### std::quoted for String I/O

```cpp
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

string text = "Hello \"World\"";

// Without quoted
cout << text << "\n";
// Output: Hello "World"

// With quoted (C++14)
cout << quoted(text) << "\n";
// Output: "Hello \"World\""

// Useful for CSV and JSON
cout << quoted("value with spaces") << "\n";
```

### std::less and Comparators

```cpp
// Transparent comparators (C++14)
set<int, less<>> s;  // Uses operator< for any comparable types

s.insert(5);
cout << s.count(5) << "\n";  // 1

// Can search with different type
cout << s.count(5.0) << "\n";  // Works with double too
```

### Algorithms Returning Pair

```cpp
#include <algorithm>

vector<int> v = {1, 2, 3, 4, 5};

// Functions returning pairs of iterators
auto [first, last] = equal_range(v.begin(), v.end(), 3);
// C++17: structured binding to unpack pair

// Alternative (C++14)
auto range = equal_range(v.begin(), v.end(), 3);
auto first_elem = range.first;
auto last_elem = range.second;
```

### std::exchange

```cpp
#include <utility>

int x = 5;
int old_value = exchange(x, 10);

cout << x << "\n";          // 10
cout << old_value << "\n";  // 5

// Useful for swapping
struct Object {
    Data data;
    Object& operator=(Object&& other) noexcept {
        data = exchange(other.data, Data());
        return *this;
    }
};
```

### std::get with Type

```cpp
#include <tuple>

tuple<int, double, string> t{42, 3.14, "hello"};

// Get by index (C++11)
auto a = get<0>(t);  // 42

// Get by type (C++14) - must be unique type
auto b = get<double>(t);  // 3.14
auto c = get<string>(t);  // "hello"

// ERROR if type appears twice
// tuple<int, int, string> t2;
// get<int>(t2);  // Ambiguous!
```

---

## DEPRECATED FEATURES & REMOVALS

## 12.1 Features Deprecated in C++14

```cpp
// 1. std::auto_ptr (deprecated)
// Use unique_ptr instead
// auto_ptr<int> old_ptr(new int(42));  // Deprecated
auto new_ptr = make_unique<int>(42);     // Modern

// 2. std::binary_function, unary_function
// No longer needed with lambdas
// struct Plus : binary_function<int, int, int> {
//     int operator()(int a, int b) const { return a + b; }
// };

auto plus = [](int a, int b) { return a + b; };  // Modern

// 3. std::bind1st, bind2nd
// Use std::bind or lambdas
// auto partial = bind1st(plus(), 5);  // Deprecated

auto partial = [](int x) { return 5 + x; };  // Modern
```

### 12.5 Shared Locks (Reader-Writer Mutex)

C++14 introduces `shared_timed_mutex` allowing multiple readers but exclusive writers.

```cpp
#include <shared_mutex>
#include <mutex>
#include <map>

class ThreadSafeCache {
    std::map<int, int> data;
    mutable std::shared_timed_mutex mtx; // C++14 (use shared_mutex in C++17)

public:
    // Reader: Multiple threads can hold shared_lock
    int get(int key) const {
        std::shared_lock<std::shared_timed_mutex> lock(mtx);
        if (data.find(key) != data.end()) {
            return data.at(key);
        }
        return -1;
    }

    // Writer: Only one thread can hold unique_lock
    void put(int key, int value) {
        std::unique_lock<std::shared_timed_mutex> lock(mtx);
        data[key] = value;
    }
};
```

---

## C++14 BEST PRACTICES

## What's Better with C++14

```cpp
// 1. Use generic lambdas for flexibility
auto process = [](auto x) { cout << x << "\n"; };
process(42);
process("hello");
process(3.14);

// 2. Use auto return types to avoid redundancy
auto add(int a, int b) { return a + b; }

// 3. Use make_unique for safety
auto ptr = make_unique<MyClass>(arg1, arg2);

// 4. Use binary literals for clarity
unsigned char mask = 0b11110000;

// 5. Use digit separators for readability
long big = 1'000'000'000'000;

// 6. Use init capture for move-only types
auto lambda = [ptr = move(ptr)]() { };

// 7. Use relaxed constexpr for compile-time computation
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) result *= i;
    return result;
}

// 8. Use variable templates for type information
template<typename T>
constexpr bool is_integral_v = is_integral<T>::value;
```

---
