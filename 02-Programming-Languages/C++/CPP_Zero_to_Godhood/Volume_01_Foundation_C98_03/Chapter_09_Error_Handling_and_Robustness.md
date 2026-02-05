# ERROR HANDLING & ROBUSTNESS



<!-- Merged content from Chapter_16_ERROR_HANDLING__DEBUGGING.md -->

# ERROR HANDLING & DEBUGGING (C++98)

## 1. ASSERTIONS

Use `assert` to catch programming logic errors during development.

```cpp
#include <iostream>
#include <cassert>

int divide(int a, int b) {
    assert(b != 0);  // Terminates program if b is 0
    return a / b;
}

int main() {
    std::cout << divide(10, 2) << "\n";
    return 0;
}
```

---

## 2. EXCEPTIONS

C++ uses exceptions to handle runtime errors gracefully.

### 2.1 Basic Try-Catch

```cpp
#include <iostream>

int safe_divide(int a, int b) {
    if (b == 0) {
        throw "Division by zero condition!";
    }
    return a / b;
}

int main() {
    try {
        int result = safe_divide(10, 0);
        std::cout << result << "\n";
    } catch (const char* msg) {
        std::cerr << "Error: " << msg << "\n";
    }
    return 0;
}
```

### 2.2 Catching Multiple Types

```cpp
try {
    // code...
} catch (int e) {
    std::cerr << "Integer exception: " << e << "\n";
} catch (const char* e) {
    std::cerr << "String exception: " << e << "\n";
} catch (...) {
    std::cerr << "Unknown exception caught!\n";
}
```

### 2.3 Standard Exceptions

Inherit from `std::exception` for custom exceptions.

```cpp
#include <iostream>
#include <exception>
#include <stdexcept> // runtime_error, logic_error

class MyError : public std::exception {
public:
    virtual const char* what() const throw() {
        return "My Custom Error";
    }
};

void test() {
    throw std::runtime_error("Standard Runtime Error");
}

int main() {
    try {
        test();
    } catch (const std::exception& e) {
        std::cerr << "Caught: " << e.what() << "\n";
    }
    return 0;
}
```

### 2.4 Exception Specifications (C++98)

In C++98, you can specify what a function might throw. (Note: Deprecated in C++11, but valid here).

```cpp
// Can throw only int
void func() throw(int) {
    throw 42;
}

// Cannot throw anything
void no_throw() throw() {
    // logic...
}
```

### 2.5 Stack Unwinding & RAII

When an exception is thrown, the stack is unwound, identifying destructors for all local objects.

```cpp
class Resource {
public:
    Resource() { std::cout << "Acquired\n"; }
    ~Resource() { std::cout << "Released\n"; }
};

void risky() {
    Resource r; // Destructor guaranteed to run
    throw 1;
}

int main() {
    try {
        risky();
    } catch (...) {
        std::cout << "Caught\n";
    }
    return 0;
}
// Output: Acquired -> Released -> Caught
```

---

## 3. DEBUGGING STRATEGIES

### 3.1 Debug Macros (C++98 Style)

```cpp
#include <iostream>

#ifdef DEBUG
    #define LOG(x) std::cout << "DEBUG: " << x << "\n"
#else
    #define LOG(x) 
#endif

int main() {
    int x = 10;
    LOG("x is " << x);
    return 0;
}
```