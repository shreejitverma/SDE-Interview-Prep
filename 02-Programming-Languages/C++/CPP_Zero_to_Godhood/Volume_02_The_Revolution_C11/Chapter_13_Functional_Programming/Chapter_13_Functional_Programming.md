# Chapter 13: Functional Programming

> *Lambdas, `std::function`, `std::bind`, and reference wrappers — the tools that let C++ treat behavior as a first-class value.*

C++11 brought functional-style programming into the mainstream of the language. **Lambda expressions** create function objects inline; **`std::function`** stores any callable behind a uniform type; **`std::bind`** performs partial application; and **`std::reference_wrapper`** lets references travel through these value-semantic facilities. This chapter covers all four, with the performance trade-offs that matter for systems code.

---

## Table of Contents

- [13.1 Lambda Expressions](#131-lambda-expressions)
- [13.2 Capture Lists](#132-capture-lists)
- [13.3 Mutable Lambdas](#133-mutable-lambdas)
- [13.4 The Full Lambda Anatomy](#134-the-full-lambda-anatomy)
- [13.5 `std::function`: The Polymorphic Callable Wrapper](#135-stdfunction-the-polymorphic-callable-wrapper)
- [13.6 `std::bind` and Partial Application](#136-stdbind-and-partial-application)
- [13.7 `std::reference_wrapper` and `std::ref`](#137-stdreference_wrapper-and-stdref)
- [13.8 Callback Patterns](#138-callback-patterns)
- [13.9 Professional Insights](#139-professional-insights)

---

## 13.1 Lambda Expressions

A **lambda expression** is a concise way to create an anonymous function object inline. Formally, a lambda is a **prvalue** whose result is a **closure object** that behaves like a functor (a class with `operator()`). The name comes from the *lambda calculus*, Alonzo Church's 1930s formalism that also underpinned LISP.

The general syntax is:

```
[captures](params) -> return_type { body }
```

```cpp
// Listing 13.1: a basic lambda
auto add = [](int a, int b) { return a + b; };
int result = add(2, 3);   // 5
```

A lambda is most valuable as an argument to functions that take a callable (algorithms, callbacks), where defining a named function would be overkill:

```cpp
// Listing 13.2: a lambda passed to an algorithm
std::vector<int> v = {5, 2, 8, 1};
std::sort(v.begin(), v.end(), [](int a, int b){ return a > b; }); // descending
```

The capture list (`[]`), the parameter list (`()`), and the body (`{}`) are the three parts; all can be empty (`[](){}` is a valid do-nothing lambda).

---

## 13.2 Capture Lists

By default a lambda cannot access variables from its enclosing scope. The **capture list** makes selected variables accessible, either by copy or by reference. Captured variables become part of the closure object — unlike parameters, they need not be passed at the call site.

| Capture | Meaning |
| :------ | :------ |
| `[]` | Capture nothing |
| `[x]` | Capture `x` by value (copy) |
| `[&x]` | Capture `x` by reference |
| `[=]` | Capture all used variables by value |
| `[&]` | Capture all used variables by reference |
| `[this]` | Capture the enclosing object (members accessible by reference) |
| `[=, &x]` | All by value, but `x` by reference |

```cpp
// Listing 13.3: capture by value vs reference
int a = 0;
auto f1 = [ ]() { return a * 9; }; // ERROR: 'a' is not captured
auto f2 = [a]() { return a * 9; }; // OK: 'a' captured by value (a copy)
auto f3 = [&a]() { return a++; };  // OK: 'a' captured by reference (modifies original)
```

> **Dangling-capture warning:** a reference (or `this`) capture must not outlive the captured object. Storing a `[&]` lambda that escapes the current scope — in a `std::function` member, a thread, or a container — is a classic use-after-free. Prefer value capture for anything that may outlive the enclosing frame.

---

## 13.3 Mutable Lambdas

Variables captured **by value** are `const` inside the body by default. The `mutable` keyword removes that `const`, allowing the lambda to modify its own copy. The modification persists across calls of the *same* closure object (it is state stored in the functor), but never touches the original variable.

```cpp
// Listing 13.4: a mutable lambda holds state
int x = 0;
auto increment = [x]() mutable { return ++x; }; // modifies the captured copy
increment(); // returns 1
increment(); // returns 2  (state retained); the outer x is still 0
```

---

## 13.4 The Full Lambda Anatomy

A lambda can carry several optional specifiers between the parameter list and the body:

| Part | Role |
| :--- | :--- |
| **default-capture** | `=` or `&` — how all non-listed variables are captured; must precede the list |
| **capture-list** | Per-variable capture (value by default, `&` for reference; `this` for members) |
| **argument-list** | The lambda's parameters |
| **`mutable`** | (optional) Makes value-captured variables non-`const` |
| **throw-specification** | (optional) e.g. `noexcept` |
| **attributes** | (optional) e.g. `[[noreturn]]` if the body always throws |
| **`-> return-type`** | (optional) Required only when the compiler cannot deduce it |
| **lambda-body** | The implementation |

```cpp
// Listing 13.5: an explicit return type and noexcept
auto divide = [](double a, double b) noexcept -> double {
    return b != 0.0 ? a / b : 0.0;
};
```

Each lambda has a unique, compiler-generated, unnamable type — which is exactly why you store them in `auto` variables, templates, or `std::function`.

---

## 13.5 `std::function`: The Polymorphic Callable Wrapper

`std::function<R(Args...)>` is a type-erased wrapper that can hold **any** callable matching its signature: a free function, a function pointer, a lambda, a functor (an object with `operator()`), or the result of `std::bind`.

```cpp
// Listing 13.6: one type, many callables
#include <functional>

double foo_fn(int x, float y, double z) { return x + y + z; }
struct Foo { double operator()(int x, float y, double z) { return x + y + z; } };

using fn_t = std::function<double(int, float, double)>;

fn_t a = foo_fn;                       // free function
fn_t b = Foo{};                        // functor
fn_t c = [](int x, float y, double z){ return x + y + z; }; // lambda
std::vector<fn_t> table = {a, b, c};   // store heterogeneous callables uniformly
for (auto& f : table) f(1, 2, 3);      // all callable through the same type
```

The power of `std::function` is that it decouples *who provides the behavior* from *who invokes it* — the foundation of callbacks, event systems, and command tables. The cost is discussed in §13.9.

---

## 13.6 `std::bind` and Partial Application

`std::bind` produces a new callable by fixing (binding) some arguments of an existing one — **partial application**. Unbound positions are marked with placeholders `_1`, `_2`, … from `std::placeholders`.

```cpp
// Listing 13.7: partial application with std::bind
#include <functional>
using namespace std::placeholders;

int add(int a, int b) { return a + b; }

auto add5 = std::bind(add, 5, _1); // fix the first argument to 5
int r = add5(10);                  // calls add(5, 10) -> 15
```

`std::bind` can also **reorder** arguments and bind member functions to an instance:

```cpp
// Listing 13.8: binding a member function and reordering arguments
struct Calc {
    double weighted(int x, double z, float y) { return x + y + z; }
};
Calc c;
// bind 'this', reorder: caller passes (x, y, z) -> member sees (x, z, y)
auto g = std::bind(&Calc::weighted, &c, _1, _3, _2);
g(1, 2.0f, 3.0); // calls c.weighted(1, 3.0, 2.0f)
```

> **Modern guidance:** lambdas largely *replace* `std::bind` in modern C++ — they are more readable, easier for the compiler to inline, and avoid `bind`'s subtle placeholder/nested-bind rules. Reach for `bind` only when a lambda would be clumsier.

---

## 13.7 `std::reference_wrapper` and `std::ref`

The value-semantic facilities above (`std::function`, `std::bind`, containers, `std::thread`) **copy** their arguments by default. When you need a *reference* to travel through them, wrap it with **`std::reference_wrapper<T>`**, most easily via the helpers **`std::ref`** and **`std::cref`** (for `const`).

```cpp
// Listing 13.9: forcing reference semantics through a copying interface
#include <functional>

void increment(int& n) { ++n; }

int x = 0;
auto bound = std::bind(increment, std::ref(x)); // bind copies args -> use ref()
bound();                                         // x is now 1 (not a copy)
```

Without `std::ref`, `std::bind` would copy `x` and `increment` would modify the dead copy. `std::reference_wrapper` is a copyable, assignable object that behaves like `T&`: it implicitly converts back to `T&` and is callable if `T` is callable. This is also how you store references in a `std::vector` (`std::vector<std::reference_wrapper<T>>`), which cannot hold real references, and how you pass references to `std::thread`'s constructor (which otherwise copies).

```cpp
// Listing 13.10: a container of references
int a = 1, b = 2, c = 3;
std::vector<std::reference_wrapper<int>> refs = {a, b, c};
for (int& r : refs) r *= 10;   // a,b,c become 10,20,30
```

---

## 13.8 Callback Patterns

The combination of `std::function` (to store a callback) and `std::bind`/lambdas (to adapt a member function into one) is a powerful design idiom: one object registers a handler that another object later invokes.

```cpp
// Listing 13.11: object B registers a member-function callback on object A
class A {
public:
    std::function<void(int, const std::string&)> on_event;
    void fire() { if (on_event) on_event(100, "event fired"); }
};

class B {
    A a_;
public:
    B() {
        a_.on_event = [this](int i, const std::string& s){ handle(i, s); };
        // equivalently: std::bind(&B::handle, this, _1, _2)
    }
    void handle(int i, const std::string& s) { /* ... */ }
    void run() { a_.fire(); } // ultimately calls B::handle
};
```

For storing arguments to be applied later, `std::tuple` pairs naturally with a callable: pack the arguments into a `std::tuple`, then expand them into the call when the time comes (see Chapter 14 for the index-sequence technique that unpacks a tuple into a function call).

---

## 13.9 Professional Insights

**`std::function` is not free.** Because it has *value semantics*, it must copy or move the callable into itself, and since it accepts callables of arbitrary type it frequently **allocates on the heap** to do so. Many implementations have a *small-object optimization* (storing tiny callables like function pointers inline), but the standard does not require it, and it only applies to `noexcept`-move-constructible types. Worse, the call itself is **indirect** — roughly the cost of a virtual function call — because any `std::function` could hold any callable.

**Prefer a template parameter in hot paths.** If a function merely *invokes* a callable and does not need to store it (e.g. a sort comparator), take it as a template parameter (`template<class Pred> void sort_my(Pred p)`) rather than `std::function`. The template version is monomorphized and inlined; the `std::function` version may allocate and will call indirectly. Reserve `std::function` for genuine type erasure — when you must store heterogeneous callables behind one type (callback slots, command tables, plugin hooks).

**Lambdas inline; `bind`/`function` often do not.** A lambda passed directly to `std::sort` typically inlines to optimal code. The same logic routed through `std::function` defeats inlining. In latency-sensitive code, keep callables concrete (lambdas, functors, template parameters) right up to the boundary where erasure is genuinely required.

**Capture deliberately.** `[=]` and `[&]` are convenient but blunt. Default-value capture can silently copy expensive objects; default-reference capture can dangle. Naming captures explicitly documents intent and prevents both classes of bug.
