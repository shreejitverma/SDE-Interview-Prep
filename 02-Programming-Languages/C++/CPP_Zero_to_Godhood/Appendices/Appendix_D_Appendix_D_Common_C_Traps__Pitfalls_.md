# Appendix D: Common C++ Traps & Pitfalls


### I. General & Syntax Traps
1.  **Most Vexing Parse**
    *   *Issue*: `MyClass obj();` declares a function returning `MyClass`, not a default-constructed object.
    *   *Fix*: Use brace initialization: `MyClass obj{};`.

2.  **The "dangling else" Problem**
    *   *Issue*: Nested `if-else` without braces can associate `else` with the wrong `if`.
    *   *Fix*: Always use braces `{}` for control structures.

3.  **Integer Division**
    *   *Issue*: `1/2` results in `0` (integer), not `0.5`.
    *   *Fix*: Cast one operand to float/double: `1.0/2` or `static_cast<double>(1)/2`.

4.  **Loop Variable Type Mismatch**
    *   *Issue*: `for (unsigned i = v.size() - 1; i >= 0; --i)` causes an infinite loop because `unsigned` is never negative.
    *   *Fix*: Use `int` (and cast size) or standard iterators/ranges.

5.  **Shadowing Variables**
    *   *Issue*: Declaring a local variable with the same name as a member or outer variable hides the outer one.
    *   *Fix*: Enable compiler warnings (`-Wshadow`) and use `this->member` if necessary.

### II. Pointers, References & Memory
6.  **Object Slicing**
    *   *Issue*: Assigning a `Derived` object to a `Base` value slices off the derived part.
    *   *Fix*: Use pointers `Base*` or references `Base&` for polymorphism.

7.  **Dangling References**
    *   *Issue*: Returning a reference to a local stack variable.
    *   *Fix*: Return by value or use smart pointers/dynamic allocation.

8.  **Iterator Invalidation**
    *   *Issue*: Adding elements to a `std::vector` may reallocate memory, invalidating all pointers/iterators to elements.
    *   *Fix*: Don't cache iterators across mutating operations; use `reserve()` if possible.

9.  **`delete` vs `delete[]`**
    *   *Issue*: Mismatching `new` with `delete[]` or `new[]` with `delete` causes undefined behavior.
    *   *Fix*: Use `std::vector` or `std::unique_ptr` instead of manual management.

10. **Use-After-Move**
    *   *Issue*: Accessing an object after `std::move()` (except for reassignment/destruction).
    *   *Fix*: Treat moved-from objects as empty; do not read their state.

### III. Classes & OOP
11. **Virtual Destructor Missing**
    *   *Issue*: Deleting a derived class via a base pointer when the base destructor is not `virtual` leaks derived resources.
    *   *Fix*: Always mark base class destructors `virtual` (or `protected` if non-polymorphic).

12. **Calling Virtual Functions in Constructor/Destructor**
    *   *Issue*: Calls the *base* class version, not the derived one, because the derived part isn't initialized/is already destroyed.
    *   *Fix*: Use two-phase initialization or factory methods.

13. **Copy Constructor/Assignment Missing**
    *   *Issue*: Classes managing raw pointers will default to shallow copy (double free error).
    *   *Fix*: Follow the **Rule of Three/Five/Zero**.

14. **Initialization Order**
    *   *Issue*: Members are initialized in *declaration order*, not initializer list order.
    *   *Fix*: Keep initializer list order identical to member declaration order to avoid warnings.

### IV. Concurrency
15. **Data Races**
    *   *Issue*: Multiple threads accessing shared memory without synchronization (at least one writer).
    *   *Fix*: Use `std::mutex`, `std::atomic`, or `std::shared_mutex`.

16. **Deadlocks**
    *   *Issue*: Two threads waiting on each other's locks.
    *   *Fix*: Acquire locks in a consistent global order; use `std::scoped_lock` (C++17) to lock multiple mutexes safely.

17. **False Sharing**
    *   *Issue*: Independent atomic variables on the same cache line degrade performance due to cache coherency protocols.
    *   *Fix*: Use `alignas(hardware_destructive_interference_size)` to pad variables.

### V. Modern C++ & Macros
18. **`std::vector<bool>` Weirdness**
    *   *Issue*: It's a template specialization (bitfield), not a vector of bools. Returns a proxy object, not `bool&`.
    *   *Fix*: Use `std::deque<bool>` or `std::vector<char>` if you need real references.

19. **Auto Type Deduction**
    *   *Issue*: `auto` drops references and `const`.
    *   *Fix*: Use `auto&` or `const auto&` explicitly when needed.

20. **Macro Side Effects**
    *   *Issue*: `#define MAX(a,b) ((a) > (b) ? (a) : (b))` evaluates arguments twice. `MAX(x++, y)` increments `x` twice.
    *   *Fix*: Use `inline` functions or templates instead of macros.

21. **Static Initialization Order Fiasco**
    *   *Issue*: Global objects in different files have undefined initialization order.
    *   *Fix*: Use the "Construct On First Use" idiom (Meyers Singleton).

---
