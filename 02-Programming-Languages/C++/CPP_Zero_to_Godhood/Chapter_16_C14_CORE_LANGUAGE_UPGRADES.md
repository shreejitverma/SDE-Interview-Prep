# C14 CORE LANGUAGE UPGRADES


# C++14 CORE LANGUAGE UPGRADES

## 1. Relaxed constexpr

C++11 `constexpr` functions were extremely limited (single return statement). C++14 removed most restrictions.

### 1.1 What is Allowed?
*   Local variable declarations (not static/thread_local).
*   Mutation of local objects.
*   Control flow statements (`if`, `switch`, loops).
*   Multiple return statements.

```cpp
// C++11 Style (Recursive, single expression)
constexpr int factorial11(int n) {
    return n <= 1 ? 1 : n * factorial11(n - 1);
}

// C++14 Style (Imperative, readable)
constexpr int factorial14(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

static_assert(factorial14(5) == 120, "Math check");
```

## 2. Binary Literals

Native support for binary representation.

```cpp
int b1 = 0b101010; // 42
int b2 = 0b1111'0000; // 240 (with separator)
```

## 3. Digit Separators

Use single quotes (`'`) to make large numbers readable.

```cpp
long long billion = 1'000'000'000;
double pi = 3.14159'26535;
unsigned int address = 0xDEAD'BEEF;
```

## 4. `[[deprecated]]` Attribute

Mark functions, classes, or variables as deprecated.

```cpp
[[deprecated("Use newFunc() instead")]]
void oldFunc() {}

void foo() {
    oldFunc(); // Compiler warning
}
```


---
