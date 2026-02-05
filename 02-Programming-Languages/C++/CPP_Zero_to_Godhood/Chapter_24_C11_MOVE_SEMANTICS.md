# C++11 MOVE SEMANTICS


Move semantics is arguably the most significant performance feature in C++11. It allows resources to be "transferred" (moved) from temporary objects rather than copied.

---

## 1. LVALUES AND RVALUES

### 1.1 Lvalues
An **lvalue** (locator value) represents an object that occupies an identifiable location in memory (has an address).
- Example: `int x = 5;` (`x` is lvalue).
- You can take its address: `&x`.

### 1.2 Rvalues
An **rvalue** is everything else: temporary values, literals, or results of expressions.
- Example: `5`, `x + 2`, `funcReturningVal()`.
- You cannot take its address.

---

## 2. RVALUE REFERENCES

C++11 introduced the rvalue reference: `T&&`. It binds *only* to rvalues.

```cpp
int x = 10;
int& lref = x;      // Lvalue ref binds to lvalue
// int&& rref = x;  // Error: cannot bind rvalue ref to lvalue

int&& rref2 = 20;   // OK: 20 is rvalue
```

---

## 3. MOVE CONSTRUCTOR & ASSIGNMENT

This allows a class to steal resources from a temporary object instead of making a deep copy.

### 3.1 Deep Copy (The Old Way)

```cpp
class Vector {
    int* data;
    size_t size;
public:
    // Copy Constructor
    Vector(const Vector& other) : size(other.size) {
        data = new int[size];
        std::copy(other.data, other.data + size, data);
    }
};
```

### 3.2 Move Constructor (The C++11 Way)

```cpp
    // Move Constructor
    Vector(Vector&& other) noexcept : data(other.data), size(other.size) {
        // Steal the pointer
        other.data = nullptr; // Null out source
        other.size = 0;
    }
```

If `other` is a temporary, the compiler selects the Move Constructor. This is O(1) instead of O(N).

---

## 4. STD::MOVE

`std::move(x)` does exactly one thing: it casts `x` to an rvalue reference (`T&&`). It essentially says, "I am done with this object, you can steal from it."

```cpp
Vector v1(100);
Vector v2 = std::move(v1); // Calls Move Constructor
// v1 is now empty (if implemented correctly)
```

---

## 5. PERFECT FORWARDING

Used in templates to preserve the value category (lvalue vs rvalue) of arguments.

### 5.1 Universal References (Forwarding References)

If `T` is a template parameter, `T&&` is a **universal reference**, not just an rvalue reference. It can bind to anything.

### 5.2 std::forward

```cpp
template<typename T>
void wrapper(T&& arg) {
    func(std::forward<T>(arg));
}
```

- If `wrapper` is called with lvalue, `arg` is lvalue, `forward` keeps it lvalue.
- If `wrapper` is called with rvalue, `arg` is lvalue (as a named variable), but `forward` casts it back to rvalue.

This enables `emplace_back` to work efficiently.

