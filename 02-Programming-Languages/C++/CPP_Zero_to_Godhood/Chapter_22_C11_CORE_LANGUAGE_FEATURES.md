# C++11 CORE LANGUAGE FEATURES


The C++11 standard (originally known as C++0x) was a revolutionary update that transformed C++ into a modern language. It addressed verbosity, safety, and performance.

---

## 1. AUTO & TYPE DEDUCTION

### 1.1 The `auto` Keyword

In C++98, you had to explicitly declare types, which could be verbose, especially with iterators. C++11 introduced `auto` to ask the compiler to deduce the type from the initializer.

```cpp
// C++98
int x = 5;
std::vector<int> v;
std::vector<int>::iterator it = v.begin();

// C++11
auto x = 5;       // deduced as int
auto it = v.begin(); // deduced as std::vector<int>::iterator
```

**Key Rules:**
1.  **Must be initialized**: `auto x;` is invalid.
2.  **References**: `auto` strips references. Use `auto&` to keep them.
3.  **Const**: `auto` strips top-level const. Use `const auto` or `const auto&`.

```cpp
int a = 10;
int& ref = a;

auto x = ref;  // x is int (copy), not int&
auto& y = ref; // y is int& (reference)
```

### 1.2 `decltype`

`decltype` inspects the declared type of an entity or expression. Unlike `auto`, it does not strip references or const.

```cpp
int x = 5;
decltype(x) y = 10; // y is int

const int& z = x;
decltype(z) w = x; // w is const int&
```

---

## 2. RANGE-BASED FOR LOOPS

C++11 introduced a syntactic sugar for iterating over containers, arrays, and initializer lists.

```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};

// C++98
for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it << " ";
}

// C++11 Range-based for
for (int i : vec) {
    std::cout << i << " ";
}

// With auto (Common pattern)
for (auto i : vec) {
    std::cout << i << " ";
}

// By Reference (to modify elements)
for (auto& i : vec) {
    i *= 2;
}

// By Const Reference (read-only, avoids copy)
for (const auto& i : vec) {
    std::cout << i << " ";
}
```

---

## 3. UNIFORM INITIALIZATION

C++11 introduced a consistent syntax for initializing everything using curly braces `{}`.

### 3.1 Brace Initialization

```cpp
// C++98 inconsistency
int a = 5;
int arr[] = {1, 2};
std::vector<int> v; v.push_back(1); // Tedious

// C++11 Uniformity
int a{5};
int arr[]{1, 2};
std::vector<int> v{1, 2, 3}; // Initializer list!
std::string s{"Hello"};
```

### 3.2 Preventing Narrowing

Brace initialization forbids "narrowing conversions" where data loss might occur.

```cpp
int x = 3.14;  // C++98: Compiles (x becomes 3), implicit cast
// int y{3.14}; // C++11: Error! Narrowing conversion
```

### 3.3 `std::initializer_list`

Classes can now take a list of elements in their constructor.

```cpp
#include <initializer_list>

class MyClass {
public:
    MyClass(std::initializer_list<int> list) {
        for (auto elem : list) {
            // process elem
        }
    }
};

MyClass obj = {1, 2, 3, 4, 5};
```

---

## 4. NULLPTR

C++98 used `0` or `NULL` (macro for 0) for null pointers. This caused ambiguity with function overloading.

```cpp
void func(int x) { std::cout << "Integer"; }
void func(char* p) { std::cout << "Pointer"; }

// C++98
func(NULL); // Calls func(int)! Because NULL is 0.

// C++11
func(nullptr); // Calls func(char*).
```

`nullptr` is a keyword of type `std::nullptr_t`. It implicitly converts to any pointer type but *not* to integers (except bool `false`).

---

## 5. STRONGLY TYPED ENUMS

C++98 enums leaked their names into the surrounding scope and implicitly converted to integers. C++11 `enum class` fixes this.

```cpp
// C++98
enum Color { RED, GREEN, BLUE };
int r = RED; // Implicit conversion allowed

// C++11
enum class TrafficLight { RED, YELLOW, GREEN };

// TrafficLight t = RED; // Error: RED not in scope
TrafficLight t = TrafficLight::RED;
// int x = TrafficLight::RED; // Error: No implicit conversion

// Explicit underlying type
enum class Byte : unsigned char { A, B, C };
```

---

## 6. OTHER CORE FEATURES

-   **`constexpr`**: Allows functions and variables to be evaluated at compile-time (limited in C++11, expanded later).
-   **`static_assert`**: Compile-time assertion checking.
    ```cpp
    static_assert(sizeof(int) == 4, "Int must be 4 bytes");
    ```
-   **Delegating Constructors**: One constructor can call another.
-   **`default` and `delete` functions**:
    ```cpp
    class NonCopyable {
        NonCopyable(const NonCopyable&) = delete; // Ban copying
        NonCopyable() = default; // Explicitly request default ctor
    };
    ```
-   **`override` and `final`**: Virtual function controls (See Chapter 19).

