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
