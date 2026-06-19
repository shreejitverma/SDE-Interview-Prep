# Chapter 35: The C++ Core Guidelines

> *Writing Modern C++ as its creators intended.*

C++ is a massive language. It contains the legacy of C, the object-oriented revolution of the 90s, the template metaprogramming tricks of the 2000s, and the functional/constexpr capabilities of Modern C++. 

Because C++ never removes old features (to maintain backwards compatibility), it is entirely possible to write terrible, unsafe C++ code using patterns from 1985, and the compiler will happily accept it.

To solve this, Bjarne Stroustrup (the creator of C++) and Herb Sutter (Chair of the ISO C++ Committee) started the **C++ Core Guidelines**. It is a living, open-source document that acts as the ultimate authority on how to write safe, fast, and modern C++.

---

## 35.1 Philosophy

The Core Guidelines are broken down into sections (Philosophy, Interfaces, Functions, Classes, Enums, Resource Management, etc.).

The philosophical rules (The "P" rules) set the tone for the entire document:
*   **P.1: Express ideas directly in code.** (Don't use a comment if a variable name or type can explain it).
*   **P.3: Express intent.** (Use standard library algorithms like `std::find` instead of raw `for` loops so the reader knows exactly what you are doing).
*   **P.4: Ideally, a program should be statically type safe.** (Avoid `void*`, unions, and C-style casts).
*   **P.8: Don't leak any resources.** (Use RAII).
*   **P.9: Don't waste time or space.** (The Zero-Overhead Principle).

## 35.2 Resource Management (The "R" Rules)

Resource management is the heart of C++. The Guidelines are extremely strict here.

*   **R.1: Manage resources automatically using resource handles and RAII.**
    *   Never use raw `new` and `delete`. Never manually call `.lock()` and `.unlock()` on a mutex. Use `std::unique_ptr`, `std::vector`, and `std::lock_guard`.
*   **R.11: Avoid calling `new` and `delete` explicitly.**
    *   Use `std::make_unique` and `std::make_shared` to create objects.
*   **R.20: Use `std::unique_ptr` or `std::shared_ptr` to represent ownership.**
*   **R.30: Take smart pointers as parameters only to explicitly express lifetime semantics.**
    *   If a function just needs to *read* an object, it should take a `const T&` or `T*`, **not** a `std::unique_ptr<T>&`. Passing smart pointers implies the function is going to take ownership of the object.

## 35.3 Interfaces (The "I" Rules)

How should functions pass data back and forth?

*   **I.2: Avoid non-`const` global variables.** (Global state makes code untestable and creates data races in multithreading).
*   **I.11: Never transfer ownership by a raw pointer (`T*`).**
    *   If you return a `T*`, the caller doesn't know if they are supposed to call `delete` on it. Return a `std::unique_ptr` instead.
*   **I.13: Do not pass an array as a single pointer.**
    *   `void process(int* arr)` is dangerous because the function doesn't know how long the array is. Use `std::span` or pass the size explicitly.

## 35.4 Functions and Error Handling (The "F" and "E" Rules)

*   **F.15: Prefer simple and conventional ways of passing information.**
    *   Return by value for cheap objects (let Return Value Optimization do its job).
    *   Pass by `const T&` for large objects you only want to read.
    *   Pass by value and `std::move` for objects you intend to consume.
*   **E.2: Throw an exception to signal that a function can't perform its assigned task.**
    *   Don't return error codes (`-1` or `false`) if the system is fundamentally broken (e.g., failed to allocate memory, missing config file).
*   **E.16: Destructors, deallocation, and swap must never fail.**
    *   If a destructor throws an exception while another exception is already unwinding the stack, `std::terminate()` is called and your program instantly crashes. Mark destructors `noexcept`.

## 35.5 The Guideline Support Library (GSL)

Some of the rules in the Core Guidelines require helper classes that are not yet in the standard library. Microsoft and others maintain the GSL (Guideline Support Library) to provide these.

Key components of the GSL include:
*   **`gsl::owner<T*>`**: An alias for a raw pointer that explicitly states "I own this memory, you must free it." Used for migrating legacy code where you can't switch to `std::unique_ptr` yet.
*   **`gsl::not_null<T*>`**: A pointer wrapper that guarantees the pointer is never `nullptr`.
*   **`gsl::Expects()` and `gsl::Ensures()`**: Macros for Design-by-Contract. `Expects` checks pre-conditions at the top of a function. `Ensures` checks post-conditions at the bottom.

## 35.6 Enforcing the Guidelines (Clang-Tidy)

The Core Guidelines are over 100 pages long. No human can memorize them all.

Because the rules are designed to be mechanically verifiable, compiler tools can automatically check your code against the Guidelines. The most famous tool is **Clang-Tidy**.

If you run Clang-Tidy on your codebase and enable the `cppcoreguidelines-*` checks, it will flag every raw `new`, every naked array, and every uninitialized variable, guiding you step-by-step toward writing perfect Modern C++.

---

By adhering to the C++ Core Guidelines, you prevent 90% of memory leaks, data races, and segfaults before you even compile your code.

But what happens when the remaining 10% slips through? You need tools to dissect the running program. We move to **Chapter 36: Advanced Debugging and Tooling**.
