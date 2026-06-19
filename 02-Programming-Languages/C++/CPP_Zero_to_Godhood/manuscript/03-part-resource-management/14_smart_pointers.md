# Chapter 14: Smart Pointers

> *Delegating memory management to the compiler.*

In Chapter 12, we learned that we should use RAII wrappers like `std::vector` to manage arrays of data. But what if we need to manage a *single* object on the heap? What if we are using Polymorphism and need an `Animal*` pointer that safely deletes itself when we are done?

Prior to C++11, developers wrote their own wrappers or used `std::auto_ptr` (which was disastrously flawed and is now deleted from the language).

C++11 introduced three official Smart Pointers. They are simply Class wrappers around raw pointers that implement RAII. They live in the `<memory>` header.

---

## 14.1 Ownership (The Only Axis that Matters)

If you try to memorize the syntax of smart pointers, you will fail. Instead, you must understand the philosophy behind them: **Ownership**.

When designing software, you must ask: *"Who owns this object?"*
*   **Owning Pointer**: Responsible for eventually deleting the resource.
*   **Observing Pointer**: Allowed to look at the resource, but strictly forbidden from deleting it.

| Smart Pointer | Ownership | Copyable? | Main Use Case |
| :--- | :--- | :--- | :--- |
| `std::unique_ptr` | **Exclusive** | No (Move-only) | The default. One clear owner. |
| `std::shared_ptr` | **Shared** | Yes | Multiple owners. Last one to leave turns off the lights. |
| `std::weak_ptr` | **None** | Yes | Observer. Checks if the object is still alive before looking at it. |

If you internalize this table, you will rarely write a memory leak again.

## 14.2 `std::unique_ptr` (Exclusive Ownership)

A non-null `std::unique_ptr` exclusively owns what it points to. Because it has exclusive ownership, it **cannot be copied**. If you could copy it, you would have two pointers claiming exclusive ownership, leading to a Double Free error.

Ownership can only be transferred using `std::move`.

It is incredibly lightweight. A `unique_ptr` has essentially **zero performance overhead** compared to a raw pointer. It should be your default choice 95% of the time.

```cpp
#include <memory>
#include <iostream>

class Engine {
public:
    Engine() { std::cout << "Engine built.\n"; }
    ~Engine() { std::cout << "Engine destroyed.\n"; }
    void start() { std::cout << "Vroom!\n"; }
};

int main() {
    // std::make_unique is the C++14 way to safely create a unique_ptr
    std::unique_ptr<Engine> p1 = std::make_unique<Engine>();
    
    p1->start(); // Use it exactly like a raw pointer!

    // std::unique_ptr<Engine> p2 = p1; // ERROR! Cannot copy!
    
    // Transfer ownership to p2. p1 is now null.
    std::unique_ptr<Engine> p2 = std::move(p1); 
    
    if (!p1) {
        std::cout << "p1 is now empty.\n";
    }
} // p2 goes out of scope. The Engine is automatically destroyed here.
```

## 14.3 `std::shared_ptr` and the Control Block

Sometimes, an object doesn't have a single clear owner. For example, in a game, both the `RenderingSystem` and the `PhysicsSystem` might hold a pointer to a `Player` object. The `Player` should only be deleted when *both* systems are completely done with it.

`std::shared_ptr` implements shared ownership using **Reference Counting**. 

When you create a `shared_ptr`, C++ secretly allocates a **Control Block** on the heap alongside your object. This Control Block contains an integer counter.
*   Every time you copy the `shared_ptr`, the counter goes up (`++`).
*   Every time a `shared_ptr` is destroyed, the counter goes down (`--`).
*   When the counter reaches exactly `0`, the object (and the Control Block) are deleted.

```cpp
#include <memory>
#include <iostream>

int main() {
    // make_shared allocates the Object AND the Control Block together for performance
    std::shared_ptr<int> sp1 = std::make_shared<int>(100);
    std::cout << "Count: " << sp1.use_count() << "\n"; // Prints 1

    {
        std::shared_ptr<int> sp2 = sp1; // COPY! Count goes to 2
        std::cout << "Count: " << sp1.use_count() << "\n"; // Prints 2
    } // sp2 is destroyed. Count goes back to 1.

    std::cout << "Count: " << sp1.use_count() << "\n"; // Prints 1
} // sp1 is destroyed. Count goes to 0. The integer is deleted!
```

> [!CAUTION]
> **The Cost of Sharing**
> `std::shared_ptr` is heavier than `unique_ptr`. It requires an extra heap allocation for the Control Block, and modifying the reference counter requires an atomic, thread-safe instruction which takes CPU cycles. Do not use `shared_ptr` just because it feels "safer". Use it only when ownership is genuinely shared.

## 14.4 `std::weak_ptr` (Breaking Cycles)

`shared_ptr` has a fatal flaw: **Cyclic References**.

Imagine a `Player` has a `shared_ptr` pointing to their `Inventory`. The `Inventory` has a `shared_ptr` pointing back to its `Player`. 
Even if the game deletes all external pointers to these objects, their reference counts will never drop below `1` because they are keeping each other alive! This is a memory leak.

To solve this, use `std::weak_ptr`. It observes an object managed by a `shared_ptr` *without* increasing the reference count. 

```cpp
#include <memory>
#include <iostream>

struct Session {
    int id = 42;
};

int main() {
    std::shared_ptr<Session> sp = std::make_shared<Session>();
    
    // Create a weak_ptr. The reference count of 'sp' remains 1!
    std::weak_ptr<Session> wp = sp; 

    // To actually use a weak_ptr, you must "lock" it to temporarily upgrade it
    // to a shared_ptr. This prevents the object from being deleted while you look at it.
    if (std::shared_ptr<Session> locked = wp.lock()) {  
        std::cout << "Session ID: " << locked->id << "\n";
    }

    sp.reset(); // The shared_ptr is destroyed. The Session is deleted.

    if (std::shared_ptr<Session> locked = wp.lock()) {
        std::cout << "Session is alive.\n";
    } else {
        std::cout << "Session has expired.\n"; // This will print!
    }
}
```
**Rule of Thumb:** In a Parent-Child relationship, the Parent should hold a `shared_ptr` (or `unique_ptr`) to the Child. If the Child needs to look at the Parent, it should hold a `weak_ptr`.

## 14.5 Custom Deleters for Legacy C-APIs

Smart pointers aren't just for memory allocated with `new`. They can manage *any* resource that needs cleanup, such as a file handle from C or a texture from the SDL graphics library.

You can provide a **Custom Deleter**—a function that the smart pointer will call instead of `delete` when it's time to clean up.

```cpp
#include <memory>
#include <cstdio>

int main() {
    // We open a file using the legacy C 'fopen'
    // We tell the unique_ptr: "When you are destroyed, call 'fclose' on this pointer."
    std::unique_ptr<FILE, int(*)(FILE*)> my_file(fopen("data.txt", "w"), fclose);

    if (my_file) {
        fprintf(my_file.get(), "Hello Legacy C-API!\n");
    }
    // No memory leaks! fclose is automatically called here.
}
```

---

You now wield the ultimate tools of Modern C++ resource management. By combining RAII, Move Semantics, and Smart Pointers, your code is both blazing fast and mathematically proven to be free of memory leaks.

But what do we actually put inside these smart pointers? In Part IV, we will explore the massive toolkit provided by the language: **The Standard Template Library (STL)**.
