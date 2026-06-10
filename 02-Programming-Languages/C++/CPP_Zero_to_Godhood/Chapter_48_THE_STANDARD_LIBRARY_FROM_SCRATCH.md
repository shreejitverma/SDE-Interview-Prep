# THE STANDARD LIBRARY FROM SCRATCH


# THE STANDARD LIBRARY FROM SCRATCH


Implementing core STL components to understand their cost.

### 19.1 Implementing my::vector
Managing raw memory, growth, and construction.

```cpp
template<typename T>
class Vector {
    T* data = nullptr;
    size_t sz = 0;
    size_t cap = 0;
    
public:
    void push_back(const T& val) {
        if (sz == cap) {
            reallocate(cap == 0 ? 1 : cap * 2);
        }
        new (data + sz) T(val); // Placement new
        sz++;
    }
    
private:
    void reallocate(size_t new_cap) {
        T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        // Move old elements...
        // Delete old memory...
        data = new_data;
        cap = new_cap;
    }
};
```

### 19.2 Implementing my::shared_ptr
Understanding the Control Block.

```cpp
template<typename T>
class SharedPtr {
    T* ptr;
    struct ControlBlock {
        std::atomic<int> ref_count{1};
    } *cb;
    
public:
    SharedPtr(T* p) : ptr(p), cb(new ControlBlock()) {}
    
    SharedPtr(const SharedPtr& other) {
        ptr = other.ptr;
        cb = other.cb;
        if (cb) cb->ref_count++;
    }
    
    ~SharedPtr() {
        if (cb && --cb->ref_count == 0) {
            delete ptr;
            delete cb;
        }
    }
};
```

---



---

# Chapter 49: Design Patterns in C++

Design patterns are reusable solutions to common software design problems. In C++, these patterns often leverage strong typing, templates, and the object model to achieve high performance and flexibility.

## 49.1 Creational Patterns

### 1. The Singleton Pattern
Ensures a class has only one instance and provides a global point of access.
```cpp
class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance; // Thread-safe in C++11+
        return instance;
    }
private:
    Singleton() {} // Private constructor
    Singleton(const Singleton&) = delete; // No copying
    void operator=(const Singleton&) = delete;
};
```

### 2. Factory Method
Defines an interface for creating an object, but lets subclasses decide which class to instantiate.

---

## 49.2 Structural Patterns

### 1. Adapter Pattern
Converts the interface of a class into another interface clients expect.

### 2. Composite Pattern
Composes objects into tree structures to represent part-whole hierarchies.

---

## 49.3 Behavioral Patterns

### 1. Strategy Pattern
Defines a family of algorithms, encapsulates each one, and makes them interchangeable.

### 2. Observer Pattern
Defines a one-to-many dependency between objects so that when one object changes state, all its dependents are notified.

---
### Professional Notes: Pattern Implementation

#### 1. CRTP (Curiously Recurring Template Pattern)
A powerful C++ idiom for achieving "Static Polymorphism" without the overhead of virtual functions.
```cpp
template <typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};

class Derived : public Base<Derived> {
public:
    void implementation() {
        std::cout << "Derived implementation" << std::endl;
    }
};
```

#### 2. Pimpl Idiom (Pointer to Implementation)
A technique to hide the implementation details of a class from its header file, reducing compilation dependencies and improving build times.
```cpp
// In Header
class Widget {
    class Impl;
    std::unique_ptr<Impl> pImpl;
public:
    Widget();
    ~Widget();
    void draw();
};
```

---


---

# Chapter 50: ODR, ADL, and Undefined Behavior

This chapter deconstructs the most subtle and dangerous aspects of the C++ language specification. Understanding these rules is what separates a senior engineer from a novice.

## 50.1 The One Definition Rule (ODR)

ODR states that a program shall contain exactly one definition for any variable, function, class type, enumeration type, or template in a given scope.

### 1. Translation Units
A translation unit (TU) is the result of preprocessing a single source file.
*   **Variable/Function**: Can be declared in multiple TUs but defined in only one.
*   **Class/Inline**: Can be defined in multiple TUs as long as the definitions are token-for-token identical.

### 2. Violations
Violating ODR often leads to **Linker Errors** or, worse, **Undefined Behavior** if the linker chooses one of several conflicting definitions.

---

## 50.2 Argument Dependent Lookup (ADL)

ADL (also known as Koenig Lookup) allows the compiler to find functions in the namespaces of their arguments.

```cpp
namespace MyNamespace {
    struct MyType {};
    void func(MyType) {}
}

int main() {
    MyNamespace::MyType obj;
    func(obj); // OK: ADL finds func in MyNamespace
}
```

### The "std::swap" Idiom
ADL is the reason we write:
```cpp
using std::swap;
swap(obj1, obj2);
```
This allows the compiler to use a custom `swap` in the object's namespace if it exists, falling back to `std::swap` otherwise.

---

## 50.3 Undefined Behavior (UB)

UB is code for which the C++ standard imposes no requirements. The compiler is free to assume UB never happens, leading to aggressive (and potentially catastrophic) optimizations.

### Common Sources of UB:
*   **Dereferencing NULL**: `*ptr` when `ptr == nullptr`.
*   **Out-of-bounds access**: `arr[10]` for an array of size 10.
*   **Use after free**: Accessing memory after `delete`.
*   **Signed integer overflow**: `INT_MAX + 1`.
*   **Data Races**: Unsynchronized access from multiple threads.

---
### Professional Notes: Language Formalisms

#### 1. Unspecified vs. Implementation-Defined Behavior
*   **Unspecified**: The standard provides multiple options, and the compiler chooses one (e.g., order of argument evaluation).
*   **Implementation-Defined**: The compiler must choose one and document it (e.g., size of `int`).

#### 2. Strict Aliasing Rule
Compilers assume that pointers of different types (e.g., `int*` and `float*`) do not point to the same memory location. Violating this with `reinterpret_cast` can lead to UB. Use `std::bit_cast` (C++20) or `memcpy` for safe type punning.

---
### Professional Notes: Subtle Quirks

#### 1. Unspecified Behavior vs. UB
*   **Order of Evaluation**: The order in which function arguments are evaluated is unspecified. `f(a++, a++)` is unspecified (but if it modifies the same variable twice, it's UB).
*   **Static Initialization Order Fiasco**: The order in which globals in different TUs are initialized is unspecified. Use the **Singleton pattern** or **Nifty Counter** idiom to solve this.

#### 2. Deep Recursion and Stack Frames
Every recursive call pushes a new frame onto the stack.
*   **Stack Depth**: Limited by the OS (e.g., 8MB on Linux).
*   **Godhood Solution**: Use a manual stack with `std::stack` on the heap to avoid stack overflow for extremely deep traversals.

---


---

# Chapter 51: Linkage, Attributes, and C Incompatibilities

This chapter covers the rules for how identifiers are shared across files, how to give hints to the compiler, and the subtle ways C++ differs from its parent language, C.

## 51.1 Linkage Specifications

Linkage determines whether a name refers to the same entity across different scopes or translation units.

### 1. Types of Linkage
*   **External Linkage**: The name can be referred to from other translation units (e.g., non-static global variables, non-inline functions).
*   **Internal Linkage**: The name is only visible within its own translation unit (e.g., `static` globals, variables in anonymous namespaces).
*   **No Linkage**: The name is local to its scope (e.g., local variables).

### 2. `extern "C"`
Tells the C++ compiler to use C-style linkage (no name mangling). This is essential for calling C functions from C++ or vice versa.

---

## 51.2 C++ Attributes (`[[...]]`)

Attributes provide a standardized way to provide extra information to the compiler to improve optimization or warnings.

### Common Attributes:
*   `[[nodiscard]]`: Warns if the return value of a function is ignored.
*   `[[maybe_unused]]`: Suppresses warnings for unused variables.
*   `[[deprecated("reason")]]`: Marks an entity as obsolete.
*   `[[fallthrough]]`: Signals intentional fallthrough in a switch statement.
*   `[[likely]]` / `[[unlikely]]` (C++20): Hints to the optimizer about branch probability.

---

## 51.3 C Incompatibilities

While C++ is mostly a superset of C, there are several "breaking" differences.

### 1. Implicit Conversions
*   **C**: Allows implicit conversion from `void*` to any other pointer type.
*   **C++**: Requires an explicit cast.

### 2. Struct Definitions
*   **C**: Requires the `struct` keyword every time you refer to the type (unless `typedef`'d).
*   **C++**: The struct name becomes a type name automatically.

### 3. Functions with No Arguments
*   **C**: `int func()` means a function taking an *unspecified* number of arguments.
*   **C++**: `int func()` means a function taking *no* arguments (equivalent to `int func(void)`).

---
### Professional Notes: Linking & Tooling

#### 1. Static vs. Dynamic Linking
*   **Static**: Object code is copied into the executable at build time. Leads to larger binaries but no external dependencies.
*   **Dynamic**: Code is loaded at runtime from `.so` or `.dll` files. Allows for smaller binaries and shared updates.

#### 2. Linker Symbols and Mangling
Use tools like `nm` or `objdump` on Linux, or `dumpbin` on Windows, to inspect the symbols in your object files. Use `c++filt` to demangle names.

---


---

# Chapter 52: Build Systems and Tooling

Mastering the C++ ecosystem requires knowledge of how to manage large-scale projects and automate the compilation of millions of lines of code.

## 52.1 The Build Process (Architectural View)

A build system automates the invocation of the compiler, assembler, and linker.

### 1. Makefile (The Foundation)
`make` uses a dependency graph to determine which files need recompilation. It only rebuilds files whose source has changed.
```make
# Simple Makefile
app: main.o utils.o
	g++ -o app main.o utils.o

main.o: main.cpp
	g++ -c main.cpp

utils.o: utils.cpp
	g++ -c utils.cpp
```

### 2. CMake (The Modern Standard)
CMake is a "Meta-build" system. It generates Makefiles, Ninja files, or Visual Studio solutions from a high-level `CMakeLists.txt`.
```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject)
add_executable(app main.cpp utils.cpp)
```

---

## 52.2 Linker Errors (Common Traps)

Linker errors happen after successful compilation when the linker cannot resolve symbols.

### 1. `undefined reference to 'X'`
The compiler saw a declaration of `X`, but the linker couldn't find its definition.
*   **Cause**: Missing source file in build, missing library in link command, or signature mismatch (e.g., `const` mismatch in parameters).

### 2. `multiple definition of 'X'`
Violates the One Definition Rule (ODR).
*   **Cause**: Defining a non-inline function in a header file included by multiple TUs.
*   **Fix**: Add `inline` or move definition to a `.cpp` file.

---
### Professional Notes: Tooling Mastery

#### 1. Sanitizers and Analyzers
Modern toolchains include powerful debugging tools:
*   **AddressSanitizer (ASan)**: Detects memory leaks, buffer overflows, and use-after-free.
*   **ThreadSanitizer (TSan)**: Detects data races.
*   **Clang-Tidy**: A static analysis tool for catching common errors and enforcing style.

#### 2. Precompiled Headers (PCH)
Compilation can be sped up significantly by pre-compiling stable headers (like `<vector>`, `<string>`) into a binary format that the compiler can load instantly.

#### 3. Compilation Databases
The `compile_commands.json` file is a standard way for build systems to tell IDEs (like VS Code or CLion) exactly how each file was compiled, enabling perfect IntelliSense and refactoring.

---


---


# VOLUME 08 SPECIALIZED MASTERY
