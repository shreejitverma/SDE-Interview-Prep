# Chapter 20: Concepts and Constraints

> *The evolution from SFINAE to beautiful, readable constraints.*

Templates are arguably the most powerful feature in C++, but for decades, they possessed a fatal flaw: the error messages.

If you passed the wrong type into a massive template hierarchy (like `std::sort`), the compiler wouldn't catch the error immediately. It would blindly substitute your type, dive 50 levels deep into the standard library code, realize that line 4,021 of `<algorithm>` failed because your class lacked an `operator<`, and then dump 200 lines of incomprehensible template gibberish onto your terminal.

C++ needed a way for a template to say: *"Wait! I only accept types that can be compared. Show me your ID at the door."*

---

## 20.1 The Dark Ages: SFINAE and `std::enable_if`

Before C++20, programmers used a hack called **SFINAE** (Substitution Failure Is Not An Error) to restrict templates.

SFINAE relies on a quirk of the compiler: if the compiler tries to substitute a template parameter and the resulting code is invalid, the compiler *doesn't throw an error*. Instead, it quietly discards that template from the list of valid options and looks for another one.

Using `<type_traits>` and `std::enable_if`, programmers forced the compiler to generate invalid code if a type didn't match their requirements.

```cpp
// C++11: The Dark Ages
#include <type_traits>

// Only enabled if T is an integral type (int, long, etc.)
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
add(T a, T b) {
    return a + b;
}

// Only enabled if T is a floating point type
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
add(T a, T b) {
    return a + b;
}
```

This code is horrific to read, horrific to write, and drastically slows down compile times. It was a workaround, not a feature.

## 20.2 The C++20 Revolution: Concepts

C++20 introduced **Concepts**, the first of the "Four Great Pillars" of modern C++ (Concepts, Ranges, Coroutines, Modules). Concepts provide a native, readable way to enforce constraints on template parameters.

No more SFINAE. No more `std::enable_if`.

```cpp
// C++20: The Renaissance
#include <concepts>

// Clean, readable, and native
template <typename T>
requires std::integral<T>
T add(T a, T b) {
    return a + b;
}
```

If you try to call `add(3.14, 2.71)`, the compiler immediately stops and says: *"Error: `double` does not satisfy concept `std::integral`."* 
One line. Beautiful.

### Abbreviated Syntax
You can make this even shorter by replacing `typename` with the Concept itself:

```cpp
template <std::integral T>
T add(T a, T b) { return a + b; }
```

Or, using C++20 Abbreviated Templates (from Chapter 19):

```cpp
auto add(std::integral auto a, std::integral auto b) {
    return a + b;
}
```

## 20.3 Standard Library Concepts

The `<concepts>` header provides a massive library of built-in constraints. You should almost never write your own concept if a standard one exists.

*   **Core Concepts**: `std::same_as`, `std::derived_from`, `std::convertible_to`.
*   **Math Concepts**: `std::integral`, `std::floating_point`, `std::signed_integral`, `std::unsigned_integral`.
*   **Object Concepts**: 
    *   `std::copyable` (can be copied)
    *   `std::movable` (can be moved)
    *   `std::semiregular` (copyable + default constructible)
    *   `std::regular` (semiregular + equality comparable)
*   **Callable Concepts**: `std::invocable` (can be called like a function), `std::predicate` (returns a boolean).

## 20.4 Defining Custom Concepts

What if you need a constraint that isn't in the standard library? You can define your own using the `concept` keyword.

A Concept is essentially a compile-time boolean expression.

```cpp
template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <Number T>
T multiply(T a, T b) { return a * b; }
```

## 20.5 Requires Expressions

Sometimes you don't just want to check a type's category; you want to check its *capabilities*. Does this class have a `.size()` method? Can it be added to another instance with `+`?

You can test this using a **Requires Expression**. A requires expression creates a dummy instance of the type and checks if specific code would be valid to compile.

```cpp
template <typename T>
concept HasSizeAndPush = requires(T container, int val) {
    // 1. Simple Requirement: Can we call .size()?
    container.size();      
    
    // 2. Simple Requirement: Can we push_back an int?
    container.push_back(val); 
    
    // 3. Type Requirement: Does it define a 'value_type' internally?
    typename T::value_type;   
    
    // 4. Compound Requirement: Does .size() return an unsigned integer?
    { container.size() } -> std::unsigned_integral; 
};

// Now we can use our concept!
template <HasSizeAndPush T>
void process_container(T& c) {
    c.push_back(42);
}
```

> [!NOTE]
> **Compile-Time Only**
> The code inside a `requires { }` block is *never executed*. The compiler simply parses it to see if it *would* compile. If it is valid syntax, the concept evaluates to `true`. If it is invalid (e.g., the class has no `.size()` method), the concept silently evaluates to `false`.

## 20.6 Partial Ordering by Constraints

What happens if you have two functions that both accept your type?

```cpp
void process(std::integral auto x) { 
    std::cout << "Any integer\n"; 
}

void process(std::signed_integral auto x) { 
    std::cout << "Strictly signed integer\n"; 
}
```

If you call `process(5)`, `5` is an `int`. An `int` satisfies *both* `std::integral` and `std::signed_integral`. Does the compiler throw an ambiguous overload error?

No! The compiler is smart enough to understand **Subsumption**. Because `std::signed_integral` is a stricter, more specific subset of `std::integral`, the compiler automatically selects the *most constrained* overload. 

`process(5)` will cleanly route to the `signed_integral` version.

---

Concepts have finally made Templates human-readable. But we have only scratched the surface of Metaprogramming. What if we want to pass an infinite number of arguments to a function? Or write code that calculates factorials entirely during the compilation phase, leaving zero runtime cost? 

In the next chapter, we descend into the deepest magic of C++: **Variadic Templates and Metaprogramming**.
