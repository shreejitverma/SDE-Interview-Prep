# ADVANCED MOVE SEMANTICS & VALUE CATEGORIES


"Move Semantics" is often misunderstood. It's not magic; it's type casting.

### 4.5.1 The C++17 Value Category Taxonomy
Everything in C++ is an Expression, and every expression has a **Type** and a **Value Category**.

```text
        Expression
       /          \
   glvalue      rvalue
   /    \      /     \
lvalue   xvalue   prvalue
```

1.  **lvalue (Identity + Movable? No)**: Has a name, persists beyond expression.
    *   `int x; x` is an lvalue.
    *   `std::string s; s` is an lvalue.
2.  **prvalue (Pure Rvalue)**: No name, temporary, initializes an object.
    *   `10`, `true`, `nullptr`.
    *   `std::string("hello")` (constructor call).
3.  **xvalue (eXpiring Value)**: Has identity, but can be moved from.
    *   Result of `std::move(x)`.
    *   Rvalue reference cast `static_cast<T&&>(x)`.

**glvalue** = lvalue + xvalue (Has Identity)
**rvalue** = prvalue + xvalue (Can be moved from)

### 4.5.2 std::move and std::forward Internals

**`std::move`**: Does NOT move. It unconditionally casts to rvalue reference.
```cpp
template<typename T>
typename remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(t);
}
```

**`std::forward`**: Conditionally casts to rvalue reference *only if* the argument was initialized with an rvalue.
Used for **Perfect Forwarding**.

```cpp
template<typename T>
T&& forward(typename remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}
```

### 4.5.3 Reference Collapsing Rules
When templates meet references, types collapse:

*   `T& &`   -> `T&`
*   `T& &&`  -> `T&`
*   `T&& &`  -> `T&`
*   `T&& &&` -> `T&&` (The only way to get an rvalue reference)

This is why `T&&` in a template is a **Universal Reference** (Forwarding Reference). It can become `T&` (lvalue) or `T&&` (rvalue).

---
