# TEMPLATE METAPROGRAMMING PATTERNS


Moving computation from runtime to compile-time saves cycles and enables zero-cost abstractions.

### 1. Expression Templates (Lazy Evaluation)
Avoid temporary objects in math operations.
**Naive:** `Vector sum = A + B + C;` creates `tmp = A+B`, then `sum = tmp+C`. Allocations!
**Expression Template:** `A + B` returns a lightweight `Sum<Vec, Vec>` object.
```cpp
template <typename L, typename R>
struct Sum {
    const L& l; const R& r;
    auto operator[](size_t i) const { return l[i] + r[i]; }
};

template <typename L, typename R>
auto operator+(const L& l, const R& r) {
    return Sum<L, R>{l, r};
}

// Usage
// Vector result = A + B + C; 
// Becomes: result[i] = A[i] + B[i] + C[i] in a single loop!
```

### 2. Type Erasure (The `std::any` Pattern)
Polymorphism without inheritance.
*   **Technique:** Hold a `void*` or a `unique_ptr<Base>`, where `Base` is an abstract class inside a templated wrapper.
*   **Example:** `std::function`, `std::any`.

### 3. The Detection Idiom (void_t)
Check if a type has a member function or typedef at compile time.
```cpp
template <typename, typename = std::void_t<>>
struct has_serialize : std::false_type {};

template <typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<T>().serialize())>> : std::true_type {};

static_assert(has_serialize<MyClass>::value, "MyClass must implement serialize()");
```
*Note: In C++20, simply use Concepts.*

### 4. Policy-Based Design
Compose behavior via template arguments.
```cpp
template <typename T, typename CheckingPolicy, typename ThreadingPolicy>
class SmartPtr : public CheckingPolicy, public ThreadingPolicy {
    T* ptr;
    // ...
};
// User chooses: SmartPtr<int, NoCheck, MultiThreaded>
```

---
