# Chapter 19: Templates — The Cookie Cutter

> *Write once, compile for any type.*

One of the foundational principles of software engineering is **DRY** (Don't Repeat Yourself). But in a strongly typed language like C++, how do you avoid repeating yourself when you need the exact same logic for different types?

Imagine writing a function to find the maximum of two numbers:

```cpp
int max(int a, int b) { return a > b ? a : b; }
```

What if you need to compare two `double`s? You have to write an overload:

```cpp
double max(double a, double b) { return a > b ? a : b; }
```

What about `float`? `long`? Custom `Player` objects? If you write an overload for every type, your codebase will explode in size, and if you find a bug in the logic, you have to fix it in 20 different places.

C++ solves this with **Templates**. A template is not code; it is a *blueprint* that tells the compiler how to generate code for you.

---

## 19.1 Function Templates

You define a template using the `template` keyword followed by angle brackets `< >`. Inside the brackets, you declare **Template Parameters**.

```cpp
template <typename T>
T max(T a, T b) {
    return a > b ? a : b;
}
```

Think of `T` as a placeholder. When you call `max(5, 10)`, the compiler says, "Ah, they are passing `int`s. I will magically generate an `int` version of this function." 

When you call `max(3.14, 2.71)`, the compiler generates a `double` version. 

## 19.2 Class Templates

Templates aren't just for functions. Entire classes can be templated. This is exactly how `std::vector<int>` and `std::vector<std::string>` work.

```cpp
template <typename T>
class Box {
private:
    T item;
public:
    void put(T new_item) { item = new_item; }
    T get() { return item; }
};

int main() {
    Box<int> intBox;
    intBox.put(42);

    Box<std::string> strBox;
    strBox.put("Godhood");
}
```

When writing member functions *outside* the class definition, you must redeclare the template:

```cpp
template <typename T>
void Box<T>::put(T new_item) {
    item = new_item;
}
```

## 19.3 Template Argument Deduction

In the `Box` example, we explicitly wrote `Box<int>`. But for functions, the compiler is usually smart enough to **deduce** the type from the arguments.

```cpp
// The compiler deduces T = int
int highest = max(5, 10); 

// The compiler deduces T = double
double highest_d = max(3.14, 2.71); 
```

But what happens if you mix types?

```cpp
// ERROR! Does T = int, or does T = double?
auto highest = max(5, 3.14); 
```

The compiler refuses to guess. You must resolve the ambiguity by explicitly specifying the type:

```cpp
auto highest = max<double>(5, 3.14); // Forces the int '5' to become a double
```

## 19.4 Explicit Template Instantiation and `extern template` [C++11]

Normally, the compiler generates the code for a template in *every single .cpp file* that uses it. If 50 files use `std::vector<int>`, the compiler generates the exact same `std::vector<int>` code 50 times, and the linker throws away 49 duplicates at the end. This drastically slows down compilation.

C++11 introduced `extern template` to solve this.

```cpp
// In a header file:
template <typename T> void heavy_function(T val) { /* massive code */ }

// Tell all .cpp files: "Do NOT instantiate this for int. I already did it elsewhere."
extern template void heavy_function<int>(int); 

// In exactly ONE .cpp file:
// Explicitly instantiate it
template void heavy_function<int>(int); 
```

## 19.5 Template Specialization: Full and Partial

Sometimes, the generic blueprint works for 99% of types, but for one specific type, you need to do something completely different. This is called **Specialization**.

### Full Specialization
```cpp
template <typename T>
void print(T val) {
    std::cout << "Generic: " << val << "\n";
}

// Full Specialization for 'bool'
template <>
void print<bool>(bool val) {
    std::cout << "Boolean: " << (val ? "TRUE" : "FALSE") << "\n";
}
```

### Partial Specialization (Classes Only)
Functions can only be fully specialized. Classes can be *partially* specialized. For example, you can write a generic `Storage<T>`, but write a specialized version specifically for *any* pointer type `Storage<T*>`.

```cpp
template <typename T>
class Storage { /* Generic Implementation */ };

// Partial Specialization: Matches ANY pointer
template <typename T>
class Storage<T*> { /* Pointer-specific Implementation */ };
```
*Note: This is exactly how `std::vector<bool>` was implemented, optimizing booleans to use single bits instead of full bytes (though this is widely considered a historical mistake).*

## 19.6 Non-Type Template Parameters

Templates don't just accept types (`typename T`); they can also accept compile-time values (like integers).

```cpp
// N is a compile-time constant
template <typename T, int N>
class Array {
private:
    T data[N]; // The size is baked into the type!
public:
    int size() const { return N; }
};

int main() {
    Array<int, 5> scores;
    // Array<int, 5> and Array<int, 6> are completely different, incompatible types!
}
```
This is exactly how `std::array<T, N>` works.

## 19.7 Variable Templates [C++14]

In C++14, you can template variables, not just functions and classes. This is extremely useful for mathematical constants.

```cpp
template <typename T>
constexpr T PI = T(3.1415926535897932385L);

int main() {
    float pi_f = PI<float>;   // Gets float precision
    double pi_d = PI<double>; // Gets double precision
}
```

## 19.8 Alias Templates (`using`) [C++11]

Historically, C and C++ used `typedef` to rename types. But `typedef` does not work well with templates. C++11 introduced the `using` syntax, which allows **Alias Templates**.

```cpp
#include <map>
#include <string>

// Old, clumsy way
typedef std::map<std::string, int> IntMap;

// Modern, beautiful way (works with templates!)
template <typename T>
using Dictionary = std::map<std::string, T>;

int main() {
    Dictionary<int> ages; // Equivalent to std::map<std::string, int>
    Dictionary<float> weights; 
}
```

## 19.9 Default Template Arguments

Just like functions can have default arguments, templates can have default types.

```cpp
template <typename T = int>
class Counter {
    T count;
};

Counter<> int_counter;    // Defaults to Counter<int>
Counter<double> d_counter; 
```

## 19.10 Class Template Argument Deduction (CTAD) [C++17]

Before C++17, you had to explicitly specify types for classes, even if the constructor made it obvious.

```cpp
// C++14
std::pair<int, double> p1(5, 3.14); // Redundant! 
auto p2 = std::make_pair(5, 3.14);  // Workaround using a function
```

C++17 introduced **CTAD**, allowing classes to deduce types exactly like functions do.

```cpp
// C++17
std::pair p(5, 3.14);     // Deduces std::pair<int, double> automatically
std::vector v = {1, 2, 3}; // Deduces std::vector<int> automatically
```

## 19.11 Deduction Guides [C++17]

How does CTAD know *how* to deduce the type? The compiler looks at the constructors. But sometimes, you need to manually tell the compiler how to deduce a type. You do this using a **Deduction Guide**.

```cpp
template <typename T>
struct Wrapper {
    T value;
};

// Deduction Guide: If I pass a const char*, deduce T as std::string!
Wrapper(const char*) -> Wrapper<std::string>;

int main() {
    Wrapper w{"Hello"}; // w is Wrapper<std::string>, not Wrapper<const char*>!
}
```

## 19.12 Abbreviated Function Templates [C++20]

C++20 introduced a massive syntactic shortcut. Instead of typing `template <typename T>`, you can just use `auto` in the parameter list.

```cpp
// C++17 and older
template <typename T>
void print(T value) { std::cout << value; }

// C++20 Abbreviated Template
void print(auto value) { std::cout << value; }
```
Under the hood, the compiler transforms the C++20 `auto` version into the exact same template as the C++17 version.

> [!WARNING]
> **Code Bloat**
> Templates are amazing, but they have a dark side: **Code Bloat**.
> If you instantiate `std::vector<int>`, `std::vector<float>`, and `std::vector<double>`, the compiler generates *three separate copies* of the vector class in your executable. Extensive use of templates can cause your final `.exe` or `.binary` size to inflate massively, and it severely increases compilation times.

---

Templates allow us to write code that works with any type. But what if we *don't* want it to work with *any* type? What if we want a template to only accept numbers, and reject strings? 

For 20 years, C++ developers used dark, hacky magic called SFINAE to enforce these rules. But in C++20, we finally received a clean, readable solution. Turn the page to enter the era of **Concepts**.
