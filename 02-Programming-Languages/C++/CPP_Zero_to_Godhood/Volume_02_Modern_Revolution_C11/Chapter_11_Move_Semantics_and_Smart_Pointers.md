# MOVE SEMANTICS & SMART POINTERS

## 1. Rvalue References & Move Semantics

C++11 solves the problem of unnecessary copying with **Move Semantics**.

### 1.1 Lvalues vs Rvalues
*   **Lvalue (Left Value):** Has a name, has an address (identity). Persists beyond the expression.
*   **Rvalue (Right Value):** Temporary, no name (or about to die). Cannot take address.

```cpp
int x = 10; // x is lvalue, 10 is rvalue
int y = x;  // y is lvalue
int z = x + y; // (x+y) is rvalue
```

### 1.2 Rvalue References (`T&&`)
A reference that binds *only* to rvalues. Represents an object we can "steal" from.

```cpp
void f(int& x)  { cout << "Lvalue ref"; }
void f(int&& x) { cout << "Rvalue ref"; }

int a = 5;
f(a); // Lvalue ref
f(5); // Rvalue ref
```

### 1.3 Move Constructor & Move Assignment
Instead of copying data (slow), we steal pointers (fast).

```cpp
class Buffer {
    int* data;
    size_t size;
public:
    // Move Constructor
    Buffer(Buffer&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr; // Nullify source to prevent double free
        other.size = 0;
    }

    // Move Assignment
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data;       // Free own resources
            data = other.data;   // Steal resources
            size = other.size;
            other.data = nullptr;// Nullify source
            other.size = 0;
        }
        return *this;
    }
};
```

### 1.4 `std::move`
Casts an lvalue to an rvalue, enabling move semantics.

```cpp
Buffer b1;
Buffer b2 = std::move(b1); // Calls Move Constructor
// b1 is now in valid but unspecified state (empty)
```

**Warning:** Do not use `b1` after moving from it.

---

## 2. Smart Pointers (RAII)

Manual `new`/`delete` is prone to leaks. C++11 introduces strict ownership models.

### 2.1 `std::unique_ptr`
*   **Ownership:** Exclusive. Only one pointer owns the object.
*   **Copying:** Disabled (`delete`).
*   **Moving:** Allowed.
*   **Overhead:** Zero (same as raw pointer).

```cpp
#include <memory>

std::unique_ptr<int> p1(new int(5));
// std::unique_ptr<int> p2 = p1; // ERROR: Cannot copy
std::unique_ptr<int> p3 = std::move(p1); // OK: Ownership transferred
```

### 2.2 `std::shared_ptr`
*   **Ownership:** Shared (Reference Counted).
*   **Destruction:** Object deleted when last `shared_ptr` dies.
*   **Overhead:** Control block on heap (ref counts).

```cpp
auto sp1 = std::make_shared<int>(10); // Efficient allocation
auto sp2 = sp1; // Ref count = 2
```

### 2.3 `std::weak_ptr`
*   **Ownership:** Non-owning observer of `shared_ptr`.
*   **Use Case:** Break cyclic references (A->B, B->A).

```cpp
std::shared_ptr<int> sp = std::make_shared<int>(42);
std::weak_ptr<int> wp = sp;

if (auto locked = wp.lock()) { // Check if object exists
    std::cout << *locked;
}
```

### 2.4 `std::make_shared` vs `new`
Always use `std::make_shared`. It allocates the object and control block in a single heap allocation (cache efficient).

---

## 3. Perfect Forwarding

Used in templates to preserve value category (lvalue vs rvalue).

```cpp
template<typename T>
void wrapper(T&& arg) {
    // std::forward passes arg as lvalue if given lvalue,
    // rvalue if given rvalue.
    func(std::forward<T>(arg)); 
}
```
