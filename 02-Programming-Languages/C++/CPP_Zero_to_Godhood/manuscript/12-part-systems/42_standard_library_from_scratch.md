# Chapter 42: The Standard Library from Scratch

> *To achieve Godhood, you must build the world yourself.*

Throughout this book, we have relied on `std::vector`, `std::shared_ptr`, and `std::function` as magical black boxes. They just work. 

But true mastery requires understanding exactly *how* they work. We are going to strip away the magic and build miniature versions of the three most important Standard Library components from scratch.

---

## 42.1 Implementing `std::vector`

A `std::vector` guarantees contiguous memory. When it runs out of space, it allocates a new, larger block of memory, moves the old elements over, and deletes the old block.

The secret to `std::vector` is that it separates **memory allocation** from **object construction**. If you reserve space for 1,000 elements, it allocates raw memory, but it *does not* call the default constructor 1,000 times.

We use `::operator new` to grab raw bytes, and **Placement `new`** to construct objects into those bytes.

```cpp
#include <new> // For placement new
#include <utility>

template <typename T>
class my_vector {
    T* data = nullptr;
    size_t sz = 0;   // Number of active elements
    size_t cap = 0;  // Total allocated capacity

public:
    ~my_vector() {
        clear();
        ::operator delete(data); // Free the raw memory
    }

    void push_back(const T& val) {
        if (sz == cap) reallocate(cap == 0 ? 1 : cap * 2);
        
        // Construct the object in the pre-allocated raw memory
        new (data + sz) T(val);
        sz++;
    }

    void clear() {
        // Destroy active elements, but DO NOT free the memory
        for (size_t i = 0; i < sz; ++i) {
            data[i].~T(); 
        }
        sz = 0;
    }

    size_t size() const { return sz; }
    size_t capacity() const { return cap; }

private:
    void reallocate(size_t new_cap) {
        // 1. Allocate raw uninitialized memory
        T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));

        // 2. Move existing elements over
        for (size_t i = 0; i < sz; ++i) {
            new (new_data + i) T(std::move(data[i])); // Placement move
            data[i].~T();                             // Destroy old
        }

        // 3. Free old raw memory
        ::operator delete(data);

        // 4. Update pointers
        data = new_data;
        cap = new_cap;
    }
};
```
*Note: A real `std::vector` uses `std::allocator` instead of `::operator new`, but the mechanism is identical.*

## 42.2 Implementing `std::shared_ptr`

How does `std::shared_ptr` know when the last copy has been destroyed? It uses a **Control Block**—a small, dynamically allocated struct that sits on the heap alongside your object. 
Every copy of the `shared_ptr` points to the exact same Control Block.

To ensure it works safely across multiple threads, the reference count inside the Control Block must be a `std::atomic<int>`.

```cpp
#include <atomic>

template <typename T>
class my_shared_ptr {
    T* ptr = nullptr;
    
    // The Control Block lives on the heap
    struct ControlBlock {
        std::atomic<int> ref_count{1};
    } *cb = nullptr;

public:
    // Constructor: Allocate the control block
    explicit my_shared_ptr(T* p) : ptr(p), cb(new ControlBlock()) {}

    // Copy Constructor: Point to the same block, increment count
    my_shared_ptr(const my_shared_ptr& other) {
        ptr = other.ptr;
        cb = other.cb;
        if (cb) {
            cb->ref_count++;
        }
    }

    // Destructor: Decrement count. If 0, destroy everything.
    ~my_shared_ptr() {
        if (cb && --cb->ref_count == 0) {
            delete ptr;
            delete cb;
        }
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
};
```
*Note: A real `std::shared_ptr` also contains a "weak count" to support `std::weak_ptr`, and `std::make_shared` optimizes this by allocating the object and the Control Block in a single chunk of memory!*

## 42.3 Implementing `std::function`

`std::function<void()>` can store a free function, a member function, a lambda, or a functor. How is it possible to store completely different types in the same variable without inheritance?

The answer is **Type Erasure**. The `my_function` class defines an abstract inner `Concept` interface. When you assign a lambda to the `my_function`, it creates a templated `Model` that inherits from `Concept` and wraps your specific lambda.

```cpp
#include <memory>
#include <iostream>

class my_function {
    // 1. The abstract interface (The Concept)
    struct Concept {
        virtual ~Concept() = default;
        virtual void call() = 0;
    };

    // 2. The templated wrapper (The Model)
    template <typename Callable>
    struct Model : Concept {
        Callable callable;
        Model(Callable c) : callable(std::move(c)) {}
        
        void call() override {
            callable(); // Invoke whatever it is
        }
    };

    // 3. The Type-Erased Pointer
    std::unique_ptr<Concept> pimpl;

public:
    // Templated constructor accepts ANYTHING
    template <typename Callable>
    my_function(Callable c) 
        : pimpl(std::make_unique<Model<Callable>>(std::move(c))) {}

    // The call operator forwards to the virtual interface
    void operator()() {
        if (pimpl) pimpl->call();
    }
};

int main() {
    // Stores a lambda!
    my_function f = []() { std::cout << "Hello from Type Erasure!\n"; };
    f(); 
}
```
This is the ultimate C++ design pattern. It provides dynamic polymorphism at runtime, but hides the inheritance away from the user so they can write clean, value-semantic code.

---

You now know how to build the tools you use every day. But what about the tools that build the tools? To achieve the highest echelon of systems programming, we must build a Compiler. We move to **Chapter 43: Writing a Compiler and a Garbage Collector**.
