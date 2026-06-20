# Chapter 09: Error Handling and Robustness

> *Expecting the unexpected without crashing the system.*

In an ideal world, network connections never drop, files are never missing, and users always type valid input. In production, software fails constantly. C++ provides two orthogonal mechanisms: **assertions** for catching programmer bugs at development time, and **exceptions** for handling runtime environmental failures gracefully. This chapter covers both, along with RAII's role in exception safety, the standard exception hierarchy, custom exception types, and debugging strategies for C++98/03.

---

## Table of Contents

- [9.1 The Philosophy of Failure](#91-the-philosophy-of-failure)
- [9.2 Assertions](#92-assertions)
- [9.3 Basic try-catch-throw](#93-basic-try-catch-throw)
- [9.4 Catching Multiple Exception Types](#94-catching-multiple-exception-types)
- [9.5 The Standard Exception Hierarchy](#95-the-standard-exception-hierarchy)
- [9.6 Custom Exception Classes](#96-custom-exception-classes)
- [9.7 Exception Specifications (C++98)](#97-exception-specifications-c98)
- [9.8 Stack Unwinding and RAII](#98-stack-unwinding-and-raii)
- [9.9 Rethrowing Exceptions](#99-rethrowing-exceptions)
- [9.10 Best Practices: Throw by Value, Catch by `const&`](#910-best-practices-throw-by-value-catch-by-const)
- [9.11 Nested Exceptions and Function Try Blocks](#911-nested-exceptions-and-function-try-blocks)
- [9.12 Debugging Strategies for C++98/03](#912-debugging-strategies-for-c9803)
- [9.13 Professional Insights: Exception Safety and Design](#913-professional-insights-exception-safety-and-design)

---

## 9.1 The Philosophy of Failure

Before writing any error-handling code, ask: **Whose fault is this?**

| Failure type | Cause | Mechanism |
| :----------- | :---- | :-------- |
| **Bug** | Programmer violated a precondition | `assert` — crash immediately in debug, removed in release |
| **Runtime error** | Environmental failure the code cannot prevent | Exception — survive and recover |

- If `calculate_speed(distance, time)` is called with `time == 0`, that is a programming error. Use `assert(time != 0)`.
- If `load_file("save.dat")` fails because the user deleted the file, that is not a bug. Use an exception.

Never use exceptions as a control-flow mechanism for normal program paths — the overhead and cognitive cost are not worth it.

---

## 9.2 Assertions

`assert` from `<cassert>` evaluates a condition at runtime. If the condition is false, it prints a diagnostic and calls `abort()`, terminating the program immediately. In release builds (`-DNDEBUG`), all `assert` calls are compiled out to zero overhead.

```cpp
// Listing 9.1: Using assert to enforce preconditions
#include <iostream>
#include <cassert>

int divide(int a, int b) {
    assert(b != 0); // Crash immediately if b == 0
    return a / b;
}

int main() {
    std::cout << divide(10, 2) << "\n"; // OK: 5
    // divide(10, 0); // Would assert-fail in debug builds
    return 0;
}
```

You can add a message using the comma operator (a common idiom in C++98/03):

```cpp
// Listing 9.2: Assert with descriptive message
#include <cassert>

int divide(int a, int b) {
    assert(b != 0 && "Denominator cannot be zero!");
    return a / b;
}
```

**Debug Macros (C++98 style)** provide conditional logging beyond assertions:

```cpp
// Listing 9.3: Conditional debug logging macro
#include <iostream>

#ifdef DEBUG
    #define LOG(x) std::cout << "DEBUG: " << x << "\n"
#else
    #define LOG(x)
#endif

int main() {
    int x = 10;
    LOG("x is " << x); // Printed only when compiled with -DDEBUG
    return 0;
}
```

---

## 9.3 Basic try-catch-throw

The exception model has three keywords:
- `throw` — signals a failure, exits the current scope immediately.
- `try` — marks a block whose execution may produce exceptions.
- `catch` — handles an exception of a specific type.

```cpp
// Listing 9.4: Basic exception flow
#include <iostream>
#include <stdexcept>

void connect_to_server() {
    bool network_down = true;
    if (network_down)
        throw std::runtime_error("No internet connection.");
    std::cout << "Connected!\n";
}

int main() {
    try {
        std::cout << "Attempting to connect...\n";
        connect_to_server();
        std::cout << "This line never prints on exception.\n";
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
```

When `throw` executes, control immediately leaves `connect_to_server`, travels up the call stack (unwinding each scope), and arrives at the matching `catch` block.

---

## 9.4 Catching Multiple Exception Types

Multiple `catch` clauses are tried in order. Place more-specific (derived) types **before** more-general (base) types:

```cpp
// Listing 9.5: Multiple catch clauses
#include <iostream>
#include <stdexcept>
#include <string>

void risky(int choice) {
    if (choice == 1) throw std::out_of_range("Index out of bounds");
    if (choice == 2) throw std::runtime_error("Network failure");
    if (choice == 3) throw 42; // throwing a raw int (avoid this)
}

int main() {
    for (int i = 1; i <= 4; ++i) {
        try {
            risky(i);
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Range error: " << e.what() << "\n";
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Runtime error: " << e.what() << "\n";
        }
        catch (int e) {
            std::cerr << "Integer exception: " << e << "\n";
        }
        catch (...) {
            std::cerr << "Unknown exception caught!\n";
        }
    }
    return 0;
}
```

**Catch-all `catch (...)`** handles any exception not matched above. Use it as a last-resort safety net, then rethrow or log. Never use it to silently swallow unknown errors.

---

## 9.5 The Standard Exception Hierarchy

Never throw raw integers, strings, or pointers. Always throw objects that derive from `std::exception`:

```
std::exception (base)
├── std::logic_error     — programming errors (detectable by inspection)
│   ├── std::invalid_argument
│   ├── std::domain_error
│   ├── std::length_error
│   └── std::out_of_range
└── std::runtime_error   — runtime errors (not detectable in advance)
    ├── std::range_error
    ├── std::overflow_error
    └── std::underflow_error
std::bad_alloc           — thrown by operator new when heap exhausted
std::bad_cast            — thrown by dynamic_cast on reference types
std::bad_typeid          — thrown by typeid on null pointer
```

```cpp
// Listing 9.6: Using standard exceptions
#include <iostream>
#include <exception>
#include <stdexcept>

void test(int choice) {
    if (choice == 1)
        throw std::runtime_error("Standard Runtime Error");
    if (choice == 2)
        throw std::logic_error("Bad argument provided");
    if (choice == 3)
        throw std::out_of_range("Index 99 out of range [0,10)");
}

int main() {
    for (int i = 1; i <= 3; ++i) {
        try {
            test(i);
        }
        catch (const std::exception& e) {
            // Catch any std::exception or derived type
            std::cerr << "Caught: " << e.what() << "\n";
        }
    }
    return 0;
}
```

---

## 9.6 Custom Exception Classes

Create custom exception types by inheriting from `std::exception` or a derived standard class. Override `what()` to return a descriptive message.

### 9.6.1 Simple custom exception

```cpp
// Listing 9.7: Simple custom exception
#include <exception>
#include <iostream>

class DatabaseError : public std::exception {
public:
    virtual const char* what() const throw() {
        return "Database connection failed";
    }
};

int main() {
    try {
        throw DatabaseError();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }
    return 0;
}
```

### 9.6.2 Custom exception with extra data

```cpp
// Listing 9.8: Custom exception carrying structured error data
#include <stdexcept>
#include <string>
#include <iostream>

class AppError : public std::runtime_error {
    int    error_code;
    int    line_number;
public:
    AppError(const std::string& msg, int code, int line)
        : std::runtime_error(msg),
          error_code(code),
          line_number(line) {}

    virtual ~AppError() throw() {}

    int getCode()       const throw() { return error_code; }
    int getLineNumber() const throw() { return line_number; }
};

int main() {
    try {
        throw AppError("Parse failed", -12, 42);
    }
    catch (const AppError& e) {
        std::cerr << e.what()
                  << " (code=" << e.getCode()
                  << ", line=" << e.getLineNumber() << ")\n";
    }
    return 0;
}
```

---

## 9.7 Exception Specifications (C++98)

C++98 allows a function to declare what it may throw using an exception specification:

```cpp
// Listing 9.9: C++98 exception specifications
void func() throw(int) {
    throw 42; // May throw int
}

void no_throw() throw() {
    // Guarantees no exceptions. Equivalent to C++11 noexcept.
}

void anything() {
    // No specification: may throw anything (C++98 default)
}
```

**Important warnings about C++98 exception specifications:**
- If a function with `throw(T)` throws a type not listed, `std::unexpected()` is called (which by default calls `std::terminate()`).
- Exception specifications were **deprecated in C++11** and **removed in C++17**. They were replaced by `noexcept`.
- `throw()` (empty specification) is equivalent to C++11's `noexcept` in practice, though the mechanism differs.

---

## 9.8 Stack Unwinding and RAII

When an exception is thrown, C++ **unwinds the call stack**: it exits each scope between the `throw` and the matching `catch`, calling the **destructor** of every local object in reverse construction order.

```cpp
// Listing 9.10: Stack unwinding guarantees destructors run
#include <iostream>

class Resource {
    const char* name;
public:
    explicit Resource(const char* n) : name(n) {
        std::cout << name << ": Acquired\n";
    }
    ~Resource() {
        std::cout << name << ": Released\n";
    }
};

void risky() {
    Resource r("File handle");
    throw std::runtime_error("Boom!");
    // r's destructor IS called even though throw exits the function
}

int main() {
    try {
        risky();
    }
    catch (...) {
        std::cout << "Caught exception\n";
    }
    return 0;
}
/* Output:
   File handle: Acquired
   File handle: Released     <- destructor called during unwinding
   Caught exception */
```

**RAII is critical for exception safety.** Raw `new`/`delete` inside a function is dangerous:

```cpp
// Listing 9.11: Raw new/delete — memory leak on exception
void dangerous() {
    int* buf = new int[1000]; // Allocated
    throw std::runtime_error("Error!");
    delete[] buf;             // NEVER REACHED — 4000 bytes leaked
}
```

Wrapping the resource in a class whose destructor frees it (RAII) prevents the leak:

```cpp
// Listing 9.12: RAII ensures cleanup on exception
#include <vector>
void safe() {
    std::vector<int> buf(1000); // Destructor called automatically during unwind
    throw std::runtime_error("Error!");
} // buf's destructor runs here, no leak
```

---

## 9.9 Rethrowing Exceptions

Use `throw;` (bare, without an argument) inside a `catch` block to rethrow the **current exception without copying it**. This preserves the original dynamic type.

```cpp
// Listing 9.13: Correct rethrow vs. slicing rethrow
#include <iostream>
#include <stdexcept>

void log_and_rethrow() {
    try {
        throw std::runtime_error("Original error");
    }
    catch (const std::exception& e) {
        std::cerr << "Logging: " << e.what() << "\n";
        throw;     // Correct: rethrows std::runtime_error, not std::exception copy
        // throw e; // WRONG: copies e as std::exception, slicing off runtime_error info
    }
}

int main() {
    try {
        log_and_rethrow();
    }
    catch (const std::exception& e) {
        std::cerr << "Final handler: " << e.what() << "\n";
    }
    return 0;
}
```

---

## 9.10 Best Practices: Throw by Value, Catch by `const&`

**The Golden Rule:** throw by **value**, catch by **const reference**.

```cpp
// Listing 9.14: Correct throw/catch pattern
try {
    throw std::runtime_error("Disk full"); // Throw by value
}
catch (const std::runtime_error& e) {     // Catch by const reference
    std::cout << e.what() << "\n";
}
```

**Why catch by const reference?**
1. **No copy:** catching by value creates an unnecessary copy of the exception object.
2. **No slicing:** if you throw a `MyDatabaseError` (derived from `std::runtime_error`) and catch by value as `std::runtime_error`, the derived-class data is sliced off. Catching by reference preserves the full object through polymorphism.

**Why not throw by pointer?**

```cpp
// Listing 9.15: Throwing by pointer — ownership nightmare
// throw new std::runtime_error("Error!"); // Who deletes this?
// The catcher would have to: catch (std::runtime_error* e) { ... delete e; }
// Manual memory management in exception handlers is error-prone.
```

Always throw by value — the exception mechanism manages the exception object's lifetime.

---

## 9.11 Nested Exceptions and Function Try Blocks

### 9.11.1 Nested try-catch

Exception handlers may be nested — the inner `catch` handles specific errors; if it rethrows, the outer `catch` handles the remainder:

```cpp
// Listing 9.16: Nested exception handling
#include <iostream>
#include <stdexcept>

int main() {
    try {
        try {
            throw std::out_of_range("inner error");
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Inner: " << e.what() << "\n";
            throw; // Propagate to outer handler
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Outer: " << e.what() << "\n";
    }
    return 0;
}
```

### 9.11.2 Function Try Block

A function try block attaches `try`/`catch` to the entire function body, including the member initialiser list in constructors:

```cpp
// Listing 9.17: Constructor function try block
#include <stdexcept>
#include <iostream>

class Widget {
    int* data;
public:
    Widget(int n)
    try : data(new int[n])
    {
        if (n == 0) throw std::invalid_argument("Size must be > 0");
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "Allocation failed: " << e.what() << "\n";
        // data is already null; nothing to free
        throw; // Re-throw: constructor cannot "recover" — object not fully built
    }

    ~Widget() { delete[] data; }
};

int main() {
    try {
        Widget w(0);
    }
    catch (const std::exception& e) {
        std::cerr << "Caught: " << e.what() << "\n";
    }
    return 0;
}
```

---

## 9.12 Debugging Strategies for C++98/03

### 9.12.1 GDB / LLDB Command Reference

| Command | Effect |
| :------ | :----- |
| `break file.cpp:42` | Set breakpoint at line 42 |
| `run` | Start the program |
| `print varname` | Print variable value |
| `backtrace` (or `bt`) | Show current call stack |
| `watch varname` | Pause whenever `varname` changes |
| `step` / `next` | Step into / step over function |
| `continue` | Resume to next breakpoint |

### 9.12.2 Unit Testing Frameworks

Testing is essential for catching regressions:
- **Google Test (GTest)**: Industry standard. Uses macros like `EXPECT_EQ(a, b)`, `ASSERT_TRUE(cond)`.
- **Catch2**: Header-only. Natural assertion style: `REQUIRE(x == 42)`.
- **Manual test harness** (C++98): Write small `main()` programs that `assert()` expected outputs.

### 9.12.3 Defensive Programming Techniques

```cpp
// Listing 9.18: Compile-time assertion (C++98 trick)
// In C++98, there is no static_assert. Use a typedef trick:
template<bool B> struct CompileTimeCheck {};
template<>       struct CompileTimeCheck<true> { typedef void type; };

// Fails at compile time if sizeof(int) != 4
typedef CompileTimeCheck<sizeof(int) == 4>::type INT_IS_4_BYTES;
```

C++11 introduced `static_assert(condition, "message")` — a cleaner replacement.

### 9.12.4 Core Dumps (Linux)

Enable core dumps to capture the full memory state of a crash for post-mortem analysis:

```bash
# Enable core dumps (shell session)
ulimit -c unlimited

# After a crash, load with gdb:
gdb ./my_program core
```

---

## 9.13 Professional Insights: Exception Safety and Design

### 9.13.1 Exception Safety Guarantees

Well-written C++ code provides one of three exception safety levels:

| Guarantee | What it means |
| :-------- | :------------ |
| **No-throw** | The operation never throws. Guaranteed by `throw()` or (C++11) `noexcept`. |
| **Strong** | If an exception is thrown, the operation has no visible effect — the program state is exactly as it was before the call (rollback semantics). |
| **Basic** | If an exception is thrown, no resources are leaked, and all objects are in a valid (though not necessarily the original) state. |

The standard library guarantees at least the basic guarantee for all operations. Functions that modify a single object in place without allocating often achieve the strong guarantee.

### 9.13.2 `noexcept` (C++11 Forward Reference)

C++11 introduced `noexcept` to replace C++98's empty exception specification:

```cpp
// noexcept tells the compiler the function will never throw
void increment(int& x) noexcept { x++; }
// If it does throw, std::terminate() is called immediately
```

`noexcept` allows the compiler to skip exception-unwinding bookkeeping in the function, producing smaller, faster code. It is especially important for **move constructors and move assignment operators** — the standard library's move-aware containers require `noexcept` moves to guarantee the strong exception safety during reallocation.

In C++98/03, use `throw()` to mark non-throwing functions.

### 9.13.3 Design Rules for Exception Hierarchies

- Derive all exceptions from `std::exception` so catch-all handlers can use `const std::exception&`.
- Prefer inheriting from `std::runtime_error` for recoverable conditions, `std::logic_error` for programming errors.
- Keep exception message strings short and descriptive — they survive across call stack boundaries.
- Never throw in destructors. A destructor called during stack unwinding that throws produces `std::terminate()`.

### 9.13.4 When Not to Use Exceptions

Exceptions add overhead to functions in the "exceptional" case path. They are inappropriate for:
- Functions on **hot paths** called millions of times per second (HFT, game physics).
- **Signal handlers** and **interrupt service routines**.
- Deeply embedded systems where the exception unwinding tables increase binary size.

In these contexts, use error codes (`int`, `enum`) or sentinel values to communicate failures, with explicit caller-side checking.
