# Part XII: Advanced Systems and Metaprogramming

*Writing code that writes code.*

# Chapter 41: Advanced TMP Patterns

> *If you stare into the templates long enough, the templates stare back.*

Template Metaprogramming (TMP) is the dark art of C++. It allows you to write programs that execute entirely during compilation, manipulating types the way normal programs manipulate values. 

While C++20 Concepts made constraining templates easy and readable, there are millions of lines of C++11/14/17 code in the wild that rely on older, more arcane techniques. To achieve "Godhood" status, you must be able to read and understand them.

---

## 41.1 SFINAE and `std::enable_if`

Before C++20, how did you tell the compiler, *"Only use this template if the type is an integer"*? You had to use **SFINAE** (Substitution Failure Is Not An Error).

When the compiler tries to instantiate a template, it substitutes your type `T` into the function signature. If that substitution results in invalid C++ code, the compiler *does not throw an error*. Instead, it simply removes that function from the list of possible overloads and tries to find another one.

We exploit this using `<type_traits>` and `std::enable_if`.

```cpp
#include <type_traits>
#include <iostream>

// Overload 1: Only enabled if T is an integer
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
process(T t) {
    std::cout << "Processing an integer: " << t << '\n';
}

// Overload 2: Only enabled if T is a float
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, void>::type
process(T t) {
    std::cout << "Processing a float: " << t << '\n';
}

int main() {
    process(42);    // Calls Overload 1
    process(3.14);  // Calls Overload 2
    // process("Hi"); // ERROR: No matching overload found!
}
```
If `T` is `int`, `is_floating_point<int>::value` is `false`. `std::enable_if` then purposely fails to define a `::type` member. The substitution fails, and the compiler ignores Overload 2.

## 41.2 The `void_t` Trick (Detection Idiom)

What if you want to write a template that only works if a class has a specific member function, like `.serialize()`? 

In C++17, the committee formalized the "Detection Idiom" using `std::void_t`. It is notoriously difficult to read, but incredibly powerful.

```cpp
#include <type_traits>

// Default template (used if substitution fails)
template <typename T, typename = void>
struct has_serialize : std::false_type {};

// Specialized template (used if T.serialize() is valid code)
template <typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<T>().serialize())>> : std::true_type {};

// Test Classes
struct GoodClass { void serialize() {} };
struct BadClass {};

int main() {
    static_assert(has_serialize<GoodClass>::value, "Should be true!");
    static_assert(!has_serialize<BadClass>::value, "Should be false!");
}
```
*How it works:* The compiler tries to instantiate the specialized template. It evaluates `decltype(T.serialize())`. If `T` doesn't have a `serialize()` method, this expression is invalid C++. SFINAE kicks in, the specialization is discarded, and it falls back to the default `false_type`.

## 41.3 Tag Dispatching

`std::enable_if` makes function signatures very messy. An older, cleaner alternative is **Tag Dispatching**.

You create empty "Tag" structs to represent properties (like `std::true_type` and `std::false_type`), and let standard function overloading choose the right path.

```cpp
#include <iterator>

// The "Tags"
struct RandomAccessTag {};
struct ForwardTag {};

// Implementation for Random Access (O(1) jump)
template <typename Iter>
void advance_impl(Iter& it, int n, std::random_access_iterator_tag) {
    it += n;
}

// Implementation for Forward Access (O(N) loop)
template <typename Iter>
void advance_impl(Iter& it, int n, std::forward_iterator_tag) {
    while (n--) ++it;
}

// The public interface
template <typename Iter>
void my_advance(Iter& it, int n) {
    // Extract the category tag from the iterator and let overloading do the rest
    advance_impl(it, n, typename std::iterator_traits<Iter>::iterator_category{});
}
```

## 41.4 Recursive Template Instantiation

Before C++11 introduced Variadic Templates, processing lists of types required heavy recursion. Even with Variadic Templates, recursive instantiation is common.

A classic example is printing a `std::tuple`. You cannot use a normal `for` loop because a tuple's elements have different types. You must use compile-time recursion.

```cpp
#include <tuple>
#include <iostream>

// Base case: Stop recursion when Index == Tuple Size
template <size_t Index = 0, typename... Args>
typename std::enable_if<Index == sizeof...(Args), void>::type
print_tuple(const std::tuple<Args...>& t) {}

// Recursive case
template <size_t Index = 0, typename... Args>
typename std::enable_if<Index < sizeof...(Args), void>::type
print_tuple(const std::tuple<Args...>& t) {
    std::cout << std::get<Index>(t) << " ";
    print_tuple<Index + 1>(t); // Recursive call!
}

int main() {
    auto t = std::make_tuple(1, 3.14, "Hello");
    print_tuple(t);
}
```

## 41.5 The Modern Eraser: `if constexpr` and Concepts

If the examples above gave you a headache, you aren't alone. SFINAE is widely considered one of the worst design flaws in C++ history (though it was an accidental discovery, not a planned feature).

Modern C++ has systematically eradicated the need for SFINAE.

*   Instead of recursive template instantiation, C++17 gave us **Fold Expressions**.
*   Instead of Tag Dispatching, C++17 gave us **`if constexpr`**.
*   Instead of `std::enable_if` and `void_t`, C++20 gave us **Concepts**.

The tuple-printing nightmare above can be rewritten in C++17/20 as:
```cpp
template <typename... Args>
void print_tuple_modern(const std::tuple<Args...>& t) {
    std::apply([](const auto&... args) {
        ((std::cout << args << " "), ...); // C++17 Fold Expression
    }, t);
}
```

Understanding SFINAE is essential for reading legacy enterprise codebases. But when writing new code, leave SFINAE in the past. Embrace Concepts.

---

We have studied how the Standard Library containers (`vector`, `map`) work, and we have studied the templates that power them. But to truly achieve mastery, we must build them ourselves. We move to **Chapter 42: The Standard Library from Scratch**.
