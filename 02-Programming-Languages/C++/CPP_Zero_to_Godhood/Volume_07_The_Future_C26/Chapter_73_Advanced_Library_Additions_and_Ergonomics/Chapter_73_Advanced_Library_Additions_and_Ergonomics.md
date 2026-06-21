# Chapter 73: Advanced Library Additions and Ergonomics

The final piece of the C++26 puzzle lies in its library ergonomics. While metaprogramming, safety, and concurrency dominate the architectural discussions, day-to-day coding relies on the expressiveness of the standard library.

C++26 introduces a suite of features designed to eliminate common boilerplate, optimize dynamic dispatch, and simplify string processing. 

---

## 73.1 Value Semantics over Indirection

C++ has always championed value semantics (objects managing their own memory, like `std::vector` and `std::string`). However, when building polymorphic hierarchies or recursive data structures (like an Abstract Syntax Tree), developers were forced to use pointers (`std::unique_ptr`).

Using pointers breaks value semantics. If you copy a `std::unique_ptr`, it fails to compile. If you copy a raw pointer, you get a shallow copy, leading to double-free bugs.

### 73.1.1 `std::indirect`

`std::indirect<T>` is a smart pointer designed for recursive data structures. It strictly enforces value semantics: when you copy a `std::indirect`, it performs a **deep copy** of the underlying object.

```cpp
#include <indirect>
#include <iostream>

struct ASTNode {
    int value;
    // Recursive definition!
    std::indirect<ASTNode> left;
    std::indirect<ASTNode> right;
    
    ASTNode(int v) : value(v) {}
};

void test_indirect() {
    ASTNode root(10);
    root.left = std::make_indirect<ASTNode>(5);
    
    // Deep copy! The new tree has its own independent nodes.
    ASTNode copy = root; 
    
    copy.left->value = 99;
    std::cout << root.left->value << '
'; // Still 5
}
```

### 73.1.2 `std::polymorphic`

`std::polymorphic<T>` extends `std::indirect` to polymorphic class hierarchies. Before C++26, to deep-copy a base class pointer, every derived class had to implement a virtual `clone()` method. `std::polymorphic` eliminates this entirely by utilizing type erasure to remember the exact derived type that was allocated.

---

## 73.2 Lightweight Callables

`std::function` (C++11) is notoriously heavy. It requires dynamic memory allocation if the capturing lambda is too large, and it forces the lambda to be copyable.

C++26 introduces two highly optimized alternatives.

### 73.2.1 `std::function_ref`

A non-owning, zero-allocation reference to any callable. It is the perfect replacement for `std::function` in function parameters where the callback is invoked synchronously.

```cpp
#include <functional>
#include <vector>

// Zero overhead, no allocations, no template bloat in the header!
void process_items(const std::vector<int>& items, std::function_ref<void(int)> callback) {
    for (int x : items) callback(x);
}
```

### 73.2.2 `std::copyable_function`

A refined `std::function` that correctly supports CV-qualifiers (`const`) and enforces that the target is copyable.

---

## 73.3 Ranges Additions

C++26 continues to polish the `std::ranges` library.

*   `views::concat`: Glues multiple ranges of disparate types together lazily.
*   `views::generate`: Creates an infinite range by repeatedly invoking a callable.

---

## 73.4 Modern Text Encoding

Historically, text encoding in C++ was a disaster. `std::cout` on Windows expected the legacy OEM code page, leading to mangled UTF-8 characters.

C++26 introduces `<text_encoding>` and standardizes the assumption that `char` implies UTF-8 encoding. It provides OS-level integration to ensure `std::print` and `std::cout` correctly translate UTF-8 strings to the terminal's native API (e.g., `WriteConsoleW` on Windows).


## 73.5 Deep Dive: The Type Erasure Mechanics of std::function_ref
To achieve zero overhead, `std::function_ref` stores exactly two pointers: a `void*` to the object, and a function pointer to a thunk...
