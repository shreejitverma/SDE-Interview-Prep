# Appendix H: Professional C++ Idioms



### 1. RAII (Resource Acquisition Is Initialization)
*   **Concept**: Bind resource lifecycle to object lifecycle. Constructor acquires, destructor releases.
*   **Use Case**: Memory, file handles, mutex locks, sockets.
*   **Example**: `std::lock_guard`, `std::unique_ptr`.

### 2. Pimpl (Pointer to Implementation)
*   **Concept**: Hide private members in a separate class/struct, accessed via a pointer.
*   **Benefit**: ABI stability, reduced compilation times (header dependency changes don't trigger rebuilds of clients).
*   **Pattern**:
    ```cpp
    class Widget {
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    public:
        Widget();
        ~Widget(); // Defined in .cpp where Impl is visible
    };
    ```

### 3. Copy-and-Swap
*   **Concept**: Implement assignment operator in terms of copy constructor and swap.
*   **Benefit**: Strong Exception Safety guarantee; removes code duplication.
*   **Pattern**:
    ```cpp
    T& operator=(T other) { // Pass by value (copy)
        swap(*this, other);
        return *this;
    }
    ```

### 4. NVI (Non-Virtual Interface)
*   **Concept**: Public interface is non-virtual; virtual functions are private/protected.
*   **Benefit**: Separation of interface (pre/post-conditions) from implementation.
*   **Pattern**:
    ```cpp
    class Base {
    public:
        void doWork() {
            // Pre-condition logic
            doWorkImpl();
            // Post-condition logic
        }
    private:
        virtual void doWorkImpl() = 0;
    };
    ```

### 5. Erase-Remove Idiom
*   **Concept**: Standard way to remove elements from a `std::vector` (before C++20 `std::erase`).
*   **Pattern**: `v.erase(std::remove(v.begin(), v.end(), value), v.end());`

### 6. SFINAE (Substitution Failure Is Not An Error)
*   **Concept**: Remove functions from overload resolution set if types don't match constraints.
*   **Modern Replacement**: C++20 Concepts (`requires`).

### 7. CRTP (Curiously Recurring Template Pattern)
*   **Concept**: Class `Derived` inherits from `Base<Derived>`.
*   **Use Case**: Static polymorphism (compile-time), adding functionality (mixins) without vtable overhead.
*   **Example**: `std::enable_shared_from_this`.

---


---

