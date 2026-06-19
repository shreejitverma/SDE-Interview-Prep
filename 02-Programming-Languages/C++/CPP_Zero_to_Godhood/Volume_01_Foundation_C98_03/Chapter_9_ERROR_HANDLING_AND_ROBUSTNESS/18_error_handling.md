# Chapter 18: Error Handling

> *Expecting the unexpected without crashing the system.*

In an ideal world, the network never disconnects, files are never missing, and users never type `"hello"` when asked for their age. In the real world, software fails constantly. 

C++ provides two distinct mechanisms for dealing with failure: **Assertions** (for when the programmer messes up) and **Exceptions** (for when the environment messes up).

---

## 18.1 The Philosophy of Failure

Before you write any error handling code, you must ask: *"Whose fault is this?"*

### Assertions (The Programmer's Fault)
If a function `calculate_speed(distance, time)` is called with `time = 0`, that is a bug in the code. A programmer made a logical error. 
You should use an **Assertion**. An assertion instantly crashes the program during development, forcing the programmer to fix the bug.

```cpp
#include <cassert>

int divide(int a, int b) {
    assert(b != 0 && "Denominator cannot be zero!"); 
    return a / b;
}
```
*Note: Assertions are completely removed by the compiler in Release builds to ensure maximum performance.*

### Exceptions (The Environment's Fault)
If a function `load_save_file("save1.dat")` fails because the user deleted the file from their hard drive, that is *not* a bug in your code. You cannot prevent users from deleting files. 
You should use an **Exception**. Exceptions allow the program to survive the error and gracefully recover (e.g., by showing a pop-up warning to the user).

## 18.2 `try`, `catch`, and `throw`

When a function encounters an environmental error it cannot handle, it **throws** an exception. This immediately stops the function and launches an invisible flare into the air.

Somewhere higher up in the program, a **try-catch** block sees the flare and handles it.

```cpp
#include <iostream>
#include <stdexcept>

void connect_to_server() {
    bool network_down = true;
    if (network_down) {
        // Launch the flare!
        throw std::runtime_error("No internet connection."); 
    }
    std::cout << "Connected!\n";
}

int main() {
    try {
        std::cout << "Attempting to connect...\n";
        connect_to_server();
        std::cout << "This line will NEVER print if an exception is thrown.\n";
    } 
    catch (const std::runtime_error& e) {
        // Catch the flare and handle it gracefully
        std::cerr << "Error occurred: " << e.what() << "\n";
    }
}
```

## 18.3 Stack Unwinding and RAII

What happens to all the local variables when an exception is thrown? 

C++ performs **Stack Unwinding**. It aggressively exits functions, searching upwards for a `catch` block. As it exits each scope, it mathematically guarantees that the **Destructor** of every local object is called.

This is why RAII (Chapter 12) is so critical. If you use `std::unique_ptr` and `std::vector`, your memory will be perfectly cleaned up during an exception. If you use raw `new` and `delete`, the `delete` will be skipped, and your program will leak memory.

```cpp
void risky_function() {
    std::vector<int> numbers = {1, 2, 3}; // RAII: Safe
    int* raw_array = new int[100];        // MANUAL: Dangerous!

    throw std::runtime_error("Boom!");    // Exception thrown!

    delete[] raw_array; // This is skipped. Massive Memory Leak.
} // Destructor of 'numbers' is called automatically here!
```

## 18.4 The Standard Exceptions

Never throw raw numbers (`throw 404;`) or raw strings (`throw "Error";`). 

Always throw objects that inherit from `std::exception`. The `<stdexcept>` header provides a set of standard exceptions that all implement the `.what()` method to return a descriptive string.

*   `std::runtime_error`: General errors that only happen at runtime (e.g., hardware failure, network loss).
*   `std::logic_error`: Errors that could theoretically be detected by reading the code (e.g., invalid mathematical arguments).
*   `std::out_of_range`: Thrown by `std::vector::at()` when accessing invalid indexes.
*   `std::bad_alloc`: Thrown by `new` when the computer completely runs out of RAM.

## 18.5 Catching by `const &` (The Object Slicing Danger)

**Rule of Godhood:** Always `throw` by value, but always `catch` by `const` reference.

```cpp
try {
    throw std::runtime_error("Disk Full"); 
} 
catch (const std::runtime_error& e) { // ALWAYS USE 'const &'
    std::cout << e.what();
}
```

Why? Two reasons:
1.  **Performance**: Catching by value creates an unnecessary, slow copy of the exception object.
2.  **Object Slicing**: If you throw a custom `MyDatabaseError` (which inherits from `std::runtime_error`), but you catch it by value as a `std::runtime_error`, the custom database data is violently sliced off and destroyed. Catching by reference preserves the original object perfectly through Polymorphism.

## 18.6 `noexcept` and Performance

Exceptions are not free. Setting up the invisible try-catch machinery adds a slight overhead to your program size. 

If you are absolutely certain that a function will *never* throw an exception, you should mark it `noexcept`.

```cpp
void increment(int& x) noexcept {
    x++;
}
```

This acts as an ironclad contract. The compiler sees `noexcept` and completely strips out all the hidden exception-handling machinery for that function, resulting in smaller, faster code. (As we learned in Chapter 13, this is especially critical for Move Constructors).

If a `noexcept` function *does* somehow throw an exception, C++ will instantly terminate the entire program (`std::terminate`).

---

We have now covered the vast majority of the Standard Library. You know how to store data, move it, manipulate it, and recover when things go wrong.

But wait. How does `std::vector` manage to hold an `int`, a `std::string`, or a custom `Player` class using the exact same code? In the next chapter, we descend into the dark arts of C++: **Templates**.
