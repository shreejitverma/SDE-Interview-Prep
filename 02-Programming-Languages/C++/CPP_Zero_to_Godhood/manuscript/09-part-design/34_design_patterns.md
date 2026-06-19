# Part IX: Software Architecture and Design

*Structuring massive codebases for maintainability and scale.*

# Chapter 34: Design Patterns in Modern C++

> *How to architect code that doesn't collapse under its own weight.*

In 1994, the "Gang of Four" (GoF) published *Design Patterns: Elements of Reusable Object-Oriented Software*. It became the bible of software architecture. However, it was written heavily with Java and Smalltalk in mind. 

Implementing classical GoF patterns in Modern C++ often results in slow, pointer-heavy code that ruins cache locality. Modern C++ has evolved its own unique patterns, leveraging templates and compile-time features to achieve high-level abstractions with zero runtime cost.

---

## 34.1 The Singleton (Meyers' Singleton)

The Singleton pattern ensures a class has only one instance and provides a global point of access to it.
Historically, Singletons were a nightmare in multithreaded C++ because initializing the static instance caused data races.

In C++11, Scott Meyers popularized a thread-safe implementation that relies on the rule that **Static local variables are initialized in a thread-safe manner**.

```cpp
class Database {
private:
    Database() {} // Private constructor
    ~Database() {}

    // Delete copy and move constructors to enforce singleton
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

public:
    static Database& get_instance() {
        // Guaranteed to be initialized safely exactly once
        static Database instance; 
        return instance;
    }
    
    void query() { /* ... */ }
};

// Usage
Database::get_instance().query();
```

## 34.2 The CRTP (Curiously Recurring Template Pattern)

Classical OOP uses `virtual` functions to achieve Polymorphism. But `virtual` functions require a `vtable` lookup at runtime, which costs a few CPU cycles and breaks branch prediction.

What if we want Polymorphism at **compile time**? We use CRTP.

In CRTP, a Base class is templated on the Derived class. The Base class can then safely cast `this` to the Derived class at compile time, eliminating the `virtual` keyword entirely.

```cpp
template <typename Derived>
class Animal {
public:
    void make_sound() {
        // Static cast at compile time. No vtable lookup!
        static_cast<Derived*>(this)->sound_impl();
    }
};

// The "Curious" part: Dog inherits from Animal<Dog>
class Dog : public Animal<Dog> {
public:
    void sound_impl() { std::cout << "Woof!\n"; }
};

int main() {
    Dog d;
    d.make_sound(); // Prints "Woof!" instantly
}
```
*(Note: As we saw in Chapter 26, C++23's "Deducing `this`" feature largely replaces the need to write CRTP, but you will see CRTP in millions of lines of legacy C++ code).*

## 34.3 Policy-Based Design

Proposed by Andrei Alexandrescu, Policy-Based Design allows you to compose complex behaviors by mixing and matching small template "Policy" classes.

Instead of creating a giant class hierarchy (`SmartPtr`, `ThreadSafeSmartPtr`, `CheckedThreadSafeSmartPtr`), you pass the behaviors as template arguments.

```cpp
// A policy for Thread Safety
struct SingleThreaded { void lock() {} void unlock() {} };
struct MultiThreaded  { std::mutex m; void lock() { m.lock(); } void unlock() { m.unlock(); } };

// A policy for Checking
struct NoCheck { void check(void* p) {} };
struct NullCheck { void check(void* p) { if(!p) throw std::runtime_error("Null!"); } };

// The Host Class
template <typename T, typename ThreadPolicy, typename CheckPolicy>
class SmartPointer : public ThreadPolicy, public CheckPolicy {
    T* ptr;
public:
    T* operator->() {
        lock();
        check(ptr);
        unlock();
        return ptr;
    }
};

// Usage: Assemble the exact class you need!
using FastPtr = SmartPointer<int, SingleThreaded, NoCheck>;
using SafePtr = SmartPointer<int, MultiThreaded, NullCheck>;
```

## 34.4 Type Erasure (Concept-Based Polymorphism)

Classical OOP requires inheritance. If you want a `std::vector<Animal*>`, then `Dog` and `Cat` MUST inherit from `Animal`. 

But what if you don't control the `Dog` class because it belongs to a third-party library? You can't force it to inherit from `Animal`.

**Type Erasure** allows you to achieve polymorphism *without* inheritance. This is exactly how `std::function` and `std::any` work under the hood.

```cpp
#include <memory>
#include <vector>

// The Type-Erased Wrapper
class Drawable {
    struct Concept {
        virtual ~Concept() = default;
        virtual void draw() const = 0;
    };

    template <typename T>
    struct Model : Concept {
        T data;
        Model(T d) : data(std::move(d)) {}
        void draw() const override { data.draw(); } // Calls the specific type's draw
    };

    std::unique_ptr<Concept> pimpl;

public:
    // Accept ANY type that has a .draw() method!
    template <typename T>
    Drawable(T x) : pimpl(std::make_unique<Model<T>>(std::move(x))) {}

    void draw() const { pimpl->draw(); }
};

// Third-party classes. No inheritance!
struct Circle { void draw() const { std::cout << "Circle\n"; } };
struct Square { void draw() const { std::cout << "Square\n"; } };

int main() {
    std::vector<Drawable> shapes;
    shapes.push_back(Circle{}); // Works!
    shapes.push_back(Square{}); // Works!

    for (const auto& shape : shapes) {
        shape.draw();
    }
}
```

## 34.5 Expression Templates (Lazy Evaluation)

In high-performance math libraries (like Eigen or Blaze), writing `Vector D = A + B + C;` is dangerous. 
Naively, C++ will do this:
1. Create a temporary Vector `tmp1 = A + B`. (Allocates memory, loops over elements).
2. Create a temporary Vector `tmp2 = tmp1 + C`. (Allocates memory, loops over elements).
3. Copy `tmp2` into `D`.

**Expression Templates** fix this by ensuring that `A + B` doesn't actually do any math. Instead, it returns a lightweight template struct called `Sum<A, B>`. The math is only executed when you finally assign it to `D`, resulting in a single loop with zero temporary memory allocations.

```cpp
template <typename L, typename R>
struct Sum {
    const L& left; 
    const R& right;
    
    // Evaluate the math lazily
    double operator[](size_t i) const { return left[i] + right[i]; }
};

template <typename L, typename R>
Sum<L, R> operator+(const L& left, const R& right) {
    return Sum<L, R>{left, right};
}
```
This pattern allows C++ matrix math to achieve speeds identical to hand-written FORTRAN or Assembly.

---

Design patterns shape how we structure our code. But how do we ensure the code we put inside those structures is safe, consistent, and readable? We look to the industry standard: **The C++ Core Guidelines**.
