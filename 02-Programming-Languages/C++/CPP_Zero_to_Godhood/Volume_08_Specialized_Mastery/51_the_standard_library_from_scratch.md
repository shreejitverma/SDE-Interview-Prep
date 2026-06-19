# Chapter 51: The Standard Library From Scratch

Developing standard library components from scratch is the ultimate test of a C++ engineer's grasp of memory management, exception safety, and the object lifecycle. This chapter implements production-grade versions of `std::vector`, `std::shared_ptr`, and `std::weak_ptr` from first principles.

***

## 51.1 Implementing `my::vector`

A C++ vector manages a contiguous block of dynamically allocated heap memory. Implementing `std::vector` requires strict separation between **allocating memory** and **constructing objects** (via placement new), as well as implementing the **Rule of Five** and guaranteeing **Strong Exception Safety**.

```cpp
#include <iostream>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

template<typename T>
class Vector {
private:
    T* data = nullptr;
    size_t sz = 0;
    size_t cap = 0;

    // Helper to allocate raw memory
    T* allocate(size_t n) {
        if (n == 0) return nullptr;
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    // Helper to free raw memory
    void deallocate(T* ptr) {
        ::operator delete(ptr);
    }

public:
    // Member Types
    using iterator = T*;
    using const_iterator = const T*;

    // 1. Default Constructor
    Vector() noexcept = default;

    // 2. Capacity Constructor
    explicit Vector(size_t capacity) : data(allocate(capacity)), sz(0), cap(capacity) {}

    // 3. Destructor
    ~Vector() {
        clear();
        deallocate(data);
    }

    // 4. Copy Constructor
    Vector(const Vector& other) : data(allocate(other.cap)), sz(other.sz), cap(other.cap) {
        size_t i = 0;
        try {
            for (; i < sz; ++i) {
                new (data + i) T(other.data[i]); // Copy construct element
            }
        } catch (...) {
            // Roll back construction on exception (Strong Exception Safety)
            for (size_t j = 0; j < i; ++j) {
                data[j].~T();
            }
            deallocate(data);
            throw;
        }
    }

    // 5. Move Constructor
    Vector(Vector&& other) noexcept : data(other.data), sz(other.sz), cap(other.cap) {
        other.data = nullptr;
        other.sz = 0;
        other.cap = 0;
    }

    // 6. Copy Assignment (Copy-and-Swap Idiom)
    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Vector temp(other);
            swap(temp);
        }
        return *this;
    }

    // 7. Move Assignment
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            clear();
            deallocate(data);
            data = other.data;
            sz = other.sz;
            cap = other.cap;
            other.data = nullptr;
            other.sz = 0;
            other.cap = 0;
        }
        return *this;
    }

    void swap(Vector& other) noexcept {
        std::swap(data, other.data);
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
    }

    // Elements access
    T& operator[](size_t index) noexcept { return data[index]; }
    const T& operator[](size_t index) const noexcept { return data[index]; }

    T& at(size_t index) {
        if (index >= sz) throw std::out_of_range("Vector index out of range");
        return data[index];
    }

    // Iterators
    iterator begin() noexcept { return data; }
    iterator end() noexcept { return data + sz; }
    const_iterator begin() const noexcept { return data; }
    const_iterator end() const noexcept { return data + sz; }

    size_t size() const noexcept { return sz; }
    size_t capacity() const noexcept { return cap; }
    bool empty() const noexcept { return sz == 0; }

    // Add elements
    void push_back(const T& val) {
        if (sz == cap) {
            reallocate(cap == 0 ? 1 : cap * 2);
        }
        new (data + sz) T(val); // Placement new
        sz++;
    }

    void push_back(T&& val) {
        if (sz == cap) {
            reallocate(cap == 0 ? 1 : cap * 2);
        }
        new (data + sz) T(std::move(val)); // Placement new with move
        sz++;
    }

    // Remove elements
    void pop_back() {
        if (sz > 0) {
            data[sz - 1].~T(); // Destroy last object
            sz--;
        }
    }

    void clear() noexcept {
        for (size_t i = 0; i < sz; ++i) {
            data[i].~T(); // Explicitly call destructor
        }
        sz = 0;
    }

private:
    // Reallocate heap memory with Strong Exception Safety
    void reallocate(size_t new_cap) {
        T* new_data = allocate(new_cap);
        size_t i = 0;
        try {
            for (; i < sz; ++i) {
                // If T's move constructor is guaranteed not to throw, move it.
                // Otherwise copy it to maintain the Strong Exception Guarantee.
                if constexpr (std::is_nothrow_move_constructible_v<T>) {
                    new (new_data + i) T(std::move(data[i]));
                } else {
                    new (new_data + i) T(data[i]);
                }
            }
        } catch (...) {
            // Cleanup on throw
            for (size_t j = 0; j < i; ++j) {
                new_data[j].~T();
            }
            deallocate(new_data);
            throw; // Re-throw exception
        }

        // Destroy old elements
        for (size_t j = 0; j < sz; ++j) {
            data[j].~T();
        }
        deallocate(data);

        data = new_data;
        cap = new_cap;
    }
};
```

> [!IMPORTANT]
> **Strong Exception Guarantee:** In `reallocate()`, we check `std::is_nothrow_move_constructible_v<T>`. If a type's move constructor throws an exception during memory reallocation, it is impossible to roll back state changes. In this scenario, we fallback to copying the objects, guaranteeing that if an exception is thrown, the original vector remains completely untouched.

***

## 51.2 Implementing Thread-Safe `shared_ptr` & `weak_ptr`

Implementing smart pointers requires modeling a shared **Control Block** that maintains atomic counts for both strong references (`shared_ptr`) and weak references (`weak_ptr`).

```cpp
#include <atomic>
#include <iostream>
#include <utility>

// Forward declaration
template<typename T> class WeakPtr;

// The Control Block structure
struct ControlBlock {
    std::atomic<int> shared_count{1};
    std::atomic<int> weak_count{0};
};

template<typename T>
class SharedPtr {
private:
    T* ptr = nullptr;
    ControlBlock* cb = nullptr;

    friend class WeakPtr<T>;

    // Private constructor for WeakPtr::lock() promotion
    SharedPtr(T* p, ControlBlock* c) : ptr(p), cb(c) {
        if (cb) {
            cb->shared_count++;
        }
    }

public:
    // 1. Default Constructor
    SharedPtr() noexcept = default;

    // 2. Raw Pointer Constructor
    explicit SharedPtr(T* p) : ptr(p), cb(p ? new ControlBlock() : nullptr) {}

    // 3. Destructor
    ~SharedPtr() {
        dec_ref();
    }

    // 4. Copy Constructor
    SharedPtr(const SharedPtr& other) noexcept : ptr(other.ptr), cb(other.cb) {
        if (cb) {
            cb->shared_count++;
        }
    }

    // 5. Move Constructor
    SharedPtr(SharedPtr&& other) noexcept : ptr(other.ptr), cb(other.cb) {
        other.ptr = nullptr;
        other.cb = nullptr;
    }

    // 6. Copy Assignment
    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (this != &other) {
            dec_ref();
            ptr = other.ptr;
            cb = other.cb;
            if (cb) {
                cb->shared_count++;
            }
        }
        return *this;
    }

    // 7. Move Assignment
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            dec_ref();
            ptr = other.ptr;
            cb = other.cb;
            other.ptr = nullptr;
            other.cb = nullptr;
        }
        return *this;
    }

    // Dereference Operators
    T& operator*() const noexcept { return *ptr; }
    T* operator->() const noexcept { return ptr; }
    T* get() const noexcept { return ptr; }

    int use_count() const noexcept {
        return cb ? cb->shared_count.load(std::memory_order_relaxed) : 0;
    }

private:
    void dec_ref() noexcept {
        if (cb) {
            // Decrement strong reference count
            if (cb->shared_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                // Last strong reference: delete managed pointer
                delete ptr;
                ptr = nullptr;

                // If no weak references exist, delete the control block too
                if (cb->weak_count.load(std::memory_order_acquire) == 0) {
                    delete cb;
                }
            }
        }
    }
};
```

### The `WeakPtr` implementation:

`WeakPtr` holds a non-owning pointer to the control block. It allows inspecting the reference counts without modifying the strong pointer's lifetime, preventing circular dependencies (memory leaks).

```cpp
template<typename T>
class WeakPtr {
private:
    T* ptr = nullptr;
    ControlBlock* cb = nullptr;

public:
    WeakPtr() noexcept = default;

    // Construct from SharedPtr
    WeakPtr(const SharedPtr<T>& sptr) noexcept : ptr(sptr.ptr), cb(sptr.cb) {
        if (cb) {
            cb->weak_count++;
        }
    }

    // Copy constructor
    WeakPtr(const WeakPtr& other) noexcept : ptr(other.ptr), cb(other.cb) {
        if (cb) {
            cb->weak_count++;
        }
    }

    // Move constructor
    WeakPtr(WeakPtr&& other) noexcept : ptr(other.ptr), cb(other.cb) {
        other.ptr = nullptr;
        other.cb = nullptr;
    }

    ~WeakPtr() {
        dec_ref();
    }

    WeakPtr& operator=(const WeakPtr& other) noexcept {
        if (this != &other) {
            dec_ref();
            ptr = other.ptr;
            cb = other.cb;
            if (cb) {
                cb->weak_count++;
            }
        }
        return *this;
    }

    WeakPtr& operator=(const SharedPtr<T>& sptr) noexcept {
        dec_ref();
        ptr = sptr.ptr;
        cb = sptr.cb;
        if (cb) {
            cb->weak_count++;
        }
        return *this;
    }

    bool expired() const noexcept {
        return !cb || cb->shared_count.load(std::memory_order_relaxed) == 0;
    }

    // Promote weak_ptr to a strong shared_ptr
    SharedPtr<T> lock() const noexcept {
        if (expired()) {
            return SharedPtr<T>();
        }
        // Increment strong count only if it's not already zero (thread-safe check)
        int current = cb->shared_count.load(std::memory_order_relaxed);
        while (current > 0) {
            if (cb->shared_count.compare_exchange_weak(current, current + 1,
                                                       std::memory_order_acq_rel,
                                                       std::memory_order_relaxed)) {
                return SharedPtr<T>(ptr, cb);
            }
        }
        return SharedPtr<T>();
    }

private:
    void dec_ref() noexcept {
        if (cb) {
            if (cb->weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                // If last weak reference and strong count is zero, delete control block
                if (cb->shared_count.load(std::memory_order_acquire) == 0) {
                    delete cb;
                }
            }
        }
    }
};
```
