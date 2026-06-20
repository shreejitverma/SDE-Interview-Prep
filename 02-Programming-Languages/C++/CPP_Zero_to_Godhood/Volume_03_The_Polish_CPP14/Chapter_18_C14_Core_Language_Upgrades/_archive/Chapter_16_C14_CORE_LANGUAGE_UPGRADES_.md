# C14 CORE LANGUAGE UPGRADES


# C++14 CORE LANGUAGE UPGRADES

While C++11 was a revolution, C++14 was the "refinement" releasepolishing the rough edges of modern C++. It turned `constexpr` from a toy into a powerful compile-time engine and added "quality of life" features that brought C++ syntax into the 21st century.

## 1. Relaxed constexpr: The Deep Dive

In C++11, `constexpr` functions were strictly functional: a single `return` statement, no loops, no local variables. C++14 lifted these "training wheels," allowing imperative logic.

### 1.1 The C++11 vs. C++14 Paradigm Shift

In C++11, you had to use recursion for almost everything. In C++14, you can use standard algorithmic patterns.

```cpp
// C++11: Functional/Recursive (Hard to read, heavy on stack during compilation)
constexpr int fib11(int n) {
    return (n <= 1) ? n : fib11(n - 1) + fib11(n - 2);
}

// C++14: Imperative/Iterative (Readable, efficient, familiar)
constexpr int fib14(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}
```

### 1.2 What is Now Allowed?
*   **Variable Declarations:** You can declare local variables (except `static` or `thread_local`).
*   **Branching & Loops:** `if`, `switch`, `for`, `while`, and `do-while` are all permitted.
*   **Mutation:** You can modify local variables within the function.
*   **Multiple Returns:** No longer restricted to a single expression.

### 1.3 Professional Note: The `constexpr` Constraint
Even in C++14, a `constexpr` function cannot:
1.  Call non-`constexpr` functions.
2.  Allocate memory (until C++20).
3.  Throw exceptions (though they can exist in branches that are never taken at compile-time).
4.  Use `asm` blocks or `goto` statements.

> **Godhood Tip:** Use `constexpr` for any computation that *can* be done at compile-time. It doesn't just save runtime; it allows the compiler to perform deeper optimizations on the resulting constants.

## 2. Binary Literals & Digit Separators

C++14 finally caught up with other languages by providing native binary support and a way to make large numbers readable.

### 2.1 Hardware-Level Clarity
For systems engineers and embedded developers, binary literals are a godsend for bitmasking.

```cpp
// Without Binary Literals (Hex/Octal required mental mapping)
uint8_t mask_old = 0x2A; 

// With C++14 Binary Literals (Directly maps to hardware registers)
uint8_t mask_new = 0b0010'1010; 

// Digit Separators (The single quote ')
// Can be placed anywhere to improve legibility
constexpr long double PLANCK_CONSTANT = 6.626'070'15e-34;
constexpr uint64_t MAX_CACHE_SIZE    = 0xFF'FF'FF'FF'FF'FF'FF'FF;
```

## 3. The `[[deprecated]]` Attribute

Standardizing how we tell other developers to stop using our old, broken code.

### 3.1 Usage Patterns
The attribute can be applied to functions, classes, typedefs, variables, and even namespaces.

```cpp
namespace [[deprecated("Namespace is messy, use v2")]] LegacyAPI {
    struct [[deprecated]] OldData {
        int x;
    };
}

class Database {
public:
    [[deprecated("Use execute(Query&&) for better performance")]]
    void runRawSQL(const char* sql);
};
```

**Deep Dive:** Unlike `#pragma message`, `[[deprecated]]` is part of the language standard. Compilers will emit a warning during the semantic analysis phase, ensuring that the message is seen exactly when the deprecated entity is utilized.

---
