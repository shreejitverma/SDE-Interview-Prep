# C++11 SMART POINTERS & MEMORY MANAGEMENT


C++11 revolutionized memory management by introducing smart pointers, which strictly define ownership semantics and automate memory reclamation, effectively making `new` and `delete` unnecessary in user code.

---

## 1. THE PROBLEM WITH RAW POINTERS

In C++98, dynamic memory required manual management:
1.  **Memory Leaks**: Forgetting `delete`.
2.  **Dangling Pointers**: Accessing deleted memory.
3.  **Double Free**: Deleting the same memory twice.
4.  **Exception Safety**: If an exception throws before `delete`, memory leaks.

Smart pointers solve these by using **RAII (Resource Acquisition Is Initialization)**.

---

## 2. UNIQUE_PTR (Exclusive Ownership)

`std::unique_ptr` represents exclusive ownership. An object can have only one `unique_ptr` pointing to it. When the `unique_ptr` is destroyed, the object is deleted.

### 2.1 Basic Usage

```cpp
#include <memory>

void func() {
    std::unique_ptr<int> ptr(new int(10));
    // or better: auto ptr = std::make_unique<int>(10); (C++14)
    
    *ptr = 20;
    // No delete needed. Memory freed when ptr goes out of scope.
}
```

### 2.2 Move Only

You cannot copy a `unique_ptr`. You must **move** it. This ensures uniqueness.

```cpp
std::unique_ptr<int> p1(new int(5));
// std::unique_ptr<int> p2 = p1; // Error! Copy deleted.

std::unique_ptr<int> p2 = std::move(p1); // OK. p1 is now empty/null.
```

### 2.3 Custom Deleters

Useful for managing C-style resources (files, sockets).

```cpp
auto deleter = [](FILE* f) { fclose(f); };
std::unique_ptr<FILE, decltype(deleter)> file(fopen("test.txt", "r"), deleter);
```

---

## 3. SHARED_PTR (Shared Ownership)

`std::shared_ptr` allows multiple pointers to own the same resource. The resource is deleted only when the *last* `shared_ptr` is destroyed.

### 3.1 Reference Counting

It maintains a "control block" with a reference count.

```cpp
auto p1 = std::make_shared<int>(100); // Ref count = 1
{
    auto p2 = p1; // Copy allowed. Ref count = 2
} // p2 destroyed. Ref count = 1

// p1 destroyed. Ref count = 0. Memory freed.
```

### 3.2 Performance Cost

`shared_ptr` is heavier than `unique_ptr` (2x size usually, plus atomic ref-count increment/decrement overhead). Use only when ownership is truly shared.

---

## 4. WEAK_PTR (Non-Owning Reference)

`std::weak_ptr` observes a `shared_ptr` without keeping it alive. It breaks **circular references**.

### 4.1 Circular Reference Problem

If A has a `shared_ptr` to B, and B has a `shared_ptr` to A, the reference count never drops to zero.

### 4.2 Using weak_ptr

```cpp
struct B;
struct A {
    std::shared_ptr<B> b_ptr;
};
struct B {
    std::weak_ptr<A> a_ptr; // Use weak_ptr back to A
};
```

To use a `weak_ptr`, you must convert it to `shared_ptr` via `.lock()`.

```cpp
if (auto shared = weak.lock()) {
    // safe to use shared
} else {
    // object died
}
```

---

## 5. BEST PRACTICES

1.  **Prefer `unique_ptr`** by default. It has zero overhead.
2.  **Use `make_unique`** (C++14) and **`make_shared`**. They are cleaner and exception-safe. `make_shared` is also more efficient (allocates object and control block in one chunk).
3.  **Avoid `new` and `delete`**.

