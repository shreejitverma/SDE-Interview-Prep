# Chapter 24: Template Metaprogramming

> *If C++14 made metaprogramming legible, C++17 makes it concise and, in places, unnecessary. Fold expressions collapse hand-rolled variadic recursion to a single line; class template argument deduction lets you drop the angle brackets the compiler can infer; `auto` non-type parameters free templates from spelling the type of a value parameter; and a cluster of new `<functional>`/`<type_traits>` utilities — `invoke`, `apply`, `make_from_tuple`, `is_invocable`, `not_fn`, `void_t`, and the logical trait combinators — turn previously bespoke machinery into standard tools.*

The C++17 template features attack the two biggest sources of metaprogramming pain: **variadic recursion** and **redundant type spelling**. Before C++17, summing a parameter pack meant a two-overload recursive template; now it is `(args + ...)`. Before C++17, `std::pair<int, double> p{1, 3.14}` named types the compiler could already see in the initializers; now `std::pair p{1, 3.14}` deduces them. The library additions complete the picture by standardizing the "call anything callable" and "expand a tuple into a call" primitives that every generic library had been re-implementing.

---

## Table of Contents

- [24.1 Fold Expressions](#241-fold-expressions)
- [24.2 Class Template Argument Deduction (CTAD)](#242-class-template-argument-deduction-ctad)
- [24.3 User-Defined Deduction Guides](#243-user-defined-deduction-guides)
- [24.4 `auto` Non-Type Template Parameters](#244-auto-non-type-template-parameters)
- [24.5 `typename` in Template Template Parameters](#245-typename-in-template-template-parameters)
- [24.6 `std::invoke` and the Uniform Call Mechanism](#246-stdinvoke-and-the-uniform-call-mechanism)
- [24.7 `std::apply` and `std::make_from_tuple`](#247-stdapply-and-stdmake_from_tuple)
- [24.8 Callable Traits: `is_invocable`, `invoke_result`, `not_fn`](#248-callable-traits-is_invocable-invoke_result-not_fn)
- [24.9 Logical Trait Combinators: `void_t`, `bool_constant`, `conjunction`/`disjunction`/`negation`](#249-logical-trait-combinators-void_t-bool_constant-conjunctiondisjunctionnegation)
- [24.10 Professional Insights](#2410-professional-insights)

---

## 24.1 Fold Expressions

A **fold expression** applies a binary operator across every element of a parameter pack, in one expression, with no recursion. It is the single most impactful variadic feature C++17 added. The four syntactic forms differ in associativity and whether an identity element is supplied:

| Form | Syntax | Expansion |
|------|--------|-----------|
| Unary right fold | `(args op ...)` | `a1 op (a2 op (... op aN))` |
| Unary left fold | `(... op args)` | `((a1 op a2) op ...) op aN` |
| Binary right fold | `(args op ... op init)` | `a1 op (... op (aN op init))` |
| Binary left fold | `(init op ... op args)` | `((init op a1) op ...) op aN` |

```cpp
// Listing 24.1: summing a pack — the C++11 recursion vs the C++17 fold
// C++11/14: two overloads, base case + recursive case
// template <typename T> T sum(T v) { return v; }
// template <typename T, typename... Rest>
// T sum(T first, Rest... rest) { return first + sum(rest...); }

// C++17: one line, a unary left fold
template <typename... Args>
auto sum(Args... args) {
    return (... + args);          // ((a1 + a2) + a3) + ...
}

int main() {
    return sum(1, 2, 3, 4);       // 10
}
```

The **binary** forms supply an identity so the fold is well-defined for an empty pack, and are the idiom for variadic printing:

```cpp
// Listing 24.2: a variadic printer via a binary left fold over operator<<
#include <iostream>

template <typename... Args>
void print(Args&&... args) {
    (std::cout << ... << args) << "\n";   // ((cout << a1) << a2) << ...
}

print("x = ", 42, ", y = ", 3.14);        // x = 42, y = 3.14
```

A fold over the comma operator runs an arbitrary statement per pack element — the general "do something for each argument" loop:

```cpp
// Listing 24.3: a comma fold invokes a side effect per element
template <typename... Args>
void call_each(Args&&... args) {
    (void(std::cout << args << ' '), ...);   // unary right fold over comma
}
```

Folds are supported over the 32 binary operators (arithmetic, logical, bitwise, comparison, assignment, comma, and the pointer-to-member operators). For an **empty** pack, only `&&` (yields `true`), `||` (yields `false`), and `,` (yields `void()`) have defined values in the unary forms; any other operator requires the binary form with an explicit identity.

---

## 24.2 Class Template Argument Deduction (CTAD)

**Class template argument deduction** lets the compiler deduce a class template's arguments from its constructor arguments, so you can omit the angle-bracket list entirely — the same convenience function templates have always had.

```cpp
// Listing 24.4: CTAD removes redundant type arguments
#include <utility>
#include <vector>
#include <mutex>

// C++14: the type arguments restate what the initializers already say
std::pair<int, double> p1(1, 3.14);

// C++17: deduced from the constructor arguments
std::pair p2(1, 3.14);             // -> std::pair<int, double>
std::vector v{1, 2, 3};            // -> std::vector<int>

std::mutex mtx;
std::lock_guard lk2(mtx);          // -> std::lock_guard<std::mutex>
```

This removes a class of `make_*` helper functions whose only job was to deduce: `std::make_pair(a, b)` becomes simply `std::pair(a, b)`, and RAII lock types no longer need their lock type spelled at every guard site. CTAD works from the visible constructors plus any deduction guides (Section 24.3); the standard library ships guides for `pair`, `tuple`, `vector`, the lock types, and most containers.

---

## 24.3 User-Defined Deduction Guides

When the compiler cannot deduce the template arguments correctly from the constructors alone — typically because a constructor parameter's type differs from the desired template argument — you supply an explicit **deduction guide**: a trailing-return-style mapping from constructor argument types to the intended specialization.

```cpp
// Listing 24.5: a deduction guide maps const char* to std::string
#include <string>

template <typename T>
struct Wrapper {
    T value;
    Wrapper(T v) : value(v) {}
};

// Without a guide, Wrapper w("hi") would deduce Wrapper<const char*>.
// This guide redirects a string-literal argument to Wrapper<std::string>:
Wrapper(const char*) -> Wrapper<std::string>;

int main() {
    Wrapper w("hello");        // deduced as Wrapper<std::string>, not const char*
    Wrapper n(42);             // deduced as Wrapper<int> from the constructor
}
```

The syntax `TemplateName(parameter-types) -> TemplateName<chosen-args>;` is written at namespace scope, alongside the class. Deduction guides are the mechanism behind the `overloaded{...}` visitor idiom of Chapter 23 and behind every container's ability to deduce its element type from an iterator pair or initializer list.

---

## 24.4 `auto` Non-Type Template Parameters

C++17 allows `template <auto V>`, letting a **non-type template parameter deduce its own type** from the argument. Previously you had to spell both the type and the value (`template <typename T, T V>`), naming the type redundantly.

```cpp
// Listing 24.6: auto NTTP deduces the value's type
template <auto Value>
struct Constant {
    static constexpr auto value = Value;
    using type = decltype(Value);
};

Constant<42>     ci;     // Value is int,  type == int
Constant<'A'>    cc;     // Value is char, type == char
Constant<true>   cb;     // Value is bool, type == bool
```

This is what makes heterogeneous compile-time constant lists and type-tag dispatch concise. Combined with a pack, `template <auto... Vs>` carries a list of values of possibly different types — the building block for compile-time value sequences:

```cpp
// Listing 24.7: a heterogeneous compile-time value pack
template <auto... Vs>
struct ValueList {
    static constexpr std::size_t size = sizeof...(Vs);
};

ValueList<1, 'c', true, 4L> mixed;   // size == 4
```

---

## 24.5 `typename` in Template Template Parameters

C++17 permits `typename` (in addition to `class`) when declaring a **template template parameter**. Before C++17 the grammar required the keyword `class` in that position even though the argument is a type-producing template, an inconsistency with every other place a type parameter is introduced.

```cpp
// Listing 24.8: typename now allowed for template template parameters
// Pre-C++17: only 'class' was accepted in this position
template <template <typename> class Container>
struct OldStyle {};

// C++17: 'typename' is accepted too, matching ordinary type-parameter syntax
template <template <typename> typename Container>
struct NewStyle {
    Container<int> data;
};

NewStyle<std::vector> nv;   // Container = std::vector
```

The change is purely about consistency — `typename` and `class` are interchangeable here — but it lets a codebase adopt a single keyword (`typename`) everywhere a type or type-template parameter is declared, removing a long-standing special case.

---

## 24.6 `std::invoke` and the Uniform Call Mechanism

`std::invoke(f, args...)` (in `<functional>`) calls *any* **Callable** with a single, uniform syntax: a free function, a function object or lambda, a pointer to member function, or a pointer to member data. It encapsulates the awkward `INVOKE` rules — where calling a member function needs `(obj.*pmf)(args)` and accessing a member needs `obj.*pmd` — behind one call.

```cpp
// Listing 24.9: one call form for every kind of callable
#include <functional>

struct Foo {
    int bar(int x) const { return x * 2; }
    int data = 7;
};

int free_fn(int x) { return x + 1; }

int main() {
    Foo f;

    // Pointer to member function — invoke supplies the (obj.*pmf)(args) form:
    int a = std::invoke(&Foo::bar, f, 21);    // 42

    // Pointer to member data — invoke yields obj.*pmd:
    int b = std::invoke(&Foo::data, f);       // 7

    // Free function and lambda — ordinary call:
    int c = std::invoke(free_fn, 10);         // 11
    int d = std::invoke([](int x){ return -x; }, 5);   // -5
}
```

`std::invoke` is the foundation the rest of the callable machinery builds on: `std::apply`, `std::bind`, `std::thread`, `std::async`, and `std::function` all use the same `INVOKE` semantics. In generic code that must store and later call an unknown callable, `std::invoke` is the correct, member-pointer-safe way to do it.

---

## 24.7 `std::apply` and `std::make_from_tuple`

**`std::apply(f, tuple)`** unpacks a tuple's elements and forwards them as the arguments to `f` — standardizing the C++14 "indices trick" (`integer_sequence` + an `_impl` helper) as a single library call.

```cpp
// Listing 24.10: std::apply replaces the hand-written index-sequence unpacker
#include <tuple>
#include <functional>

int add3(int a, int b, int c) { return a + b + c; }

int main() {
    auto args = std::make_tuple(1, 2, 3);
    int sum = std::apply(add3, args);          // add3(1, 2, 3) == 6

    // Works with any callable, including a lambda over a heterogeneous tuple:
    auto t = std::make_tuple(2, 3.5, std::string("x"));
    std::apply([](int i, double d, const std::string& s) {
        // use i, d, s ...
    }, t);
}
```

**`std::make_from_tuple<T>(tuple)`** constructs a `T` by unpacking the tuple as constructor arguments — the construction analogue of `apply`. It is the precise tool for building an object from a tuple of saved constructor parameters (deferred construction, factory replay, `emplace`-style forwarding).

```cpp
// Listing 24.11: constructing an object from a tuple of arguments
#include <tuple>
#include <memory>

struct Vec3 { double x, y, z; Vec3(double a, double b, double c); };

auto params = std::make_tuple(1.0, 2.0, 3.0);
Vec3 v = std::make_from_tuple<Vec3>(params);   // Vec3(1.0, 2.0, 3.0)
```

Both are implemented in terms of `std::invoke` and `index_sequence`, but you no longer write that scaffolding: the tuple-to-call and tuple-to-constructor bridges are now one call each.

---

## 24.8 Callable Traits: `is_invocable`, `invoke_result`, `not_fn`

C++17 modernizes the callable-introspection traits to match `std::invoke`'s semantics and deprecates the flawed `std::result_of`.

**`std::is_invocable<F, Args...>`** and **`std::is_invocable_r<R, F, Args...>`** ask, at compile time, whether `F` can be called with `Args...` (and whether the result is convertible to `R`):

```cpp
// Listing 24.12: detecting callability before committing to a call
#include <type_traits>

int f(int);

static_assert( std::is_invocable_v<decltype(f), int>);      // f(int) is valid
static_assert(!std::is_invocable_v<decltype(f), std::string>);
static_assert( std::is_invocable_r_v<long, decltype(f), int>); // result -> long ok
```

**`std::invoke_result_t<F, Args...>`** gives the return type of that call — the correct, member-pointer-aware replacement for `std::result_of_t`:

```cpp
// Listing 24.13: deducing a callable's return type the modern way
template <typename F, typename... Args>
auto call_and_log(F&& f, Args&&... args)
    -> std::invoke_result_t<F, Args...> {     // not result_of_t (deprecated)
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}
```

**`std::not_fn(callable)`** returns a callable that negates the result of the wrapped one — the general replacement for the deprecated `std::not1`/`std::not2` negators, with no fixed arity:

```cpp
// Listing 24.14: negating a predicate without not1/not2
#include <functional>
#include <algorithm>
#include <vector>

bool is_even(int n) { return n % 2 == 0; }

std::vector<int> v{1, 2, 3, 4, 5};
auto odd_count = std::count_if(v.begin(), v.end(), std::not_fn(is_even));  // 3
```

---

## 24.9 Logical Trait Combinators: `void_t`, `bool_constant`, `conjunction`/`disjunction`/`negation`

C++17 standardizes the metaprogramming combinators that libraries had been re-deriving.

**`std::void_t<...>`** maps any list of well-formed types to `void`. It is the enabling trick for **detection idioms**: a partial specialization that mentions a possibly-ill-formed expression inside `void_t` is selected only when that expression is valid (SFINAE), otherwise the primary template is chosen.

```cpp
// Listing 24.15: void_t detection idiom — does T have a ::value_type?
#include <type_traits>

template <typename T, typename = void>
struct has_value_type : std::false_type {};

template <typename T>
struct has_value_type<T, std::void_t<typename T::value_type>>
    : std::true_type {};

static_assert( has_value_type<std::vector<int>>::value);
static_assert(!has_value_type<int>::value);
```

**`std::bool_constant<B>`** is shorthand for `std::integral_constant<bool, B>`, the base most boolean traits derive from.

**`std::conjunction`, `std::disjunction`, `std::negation`** are the logical AND/OR/NOT over traits, and crucially they **short-circuit** during instantiation — `conjunction` stops at the first `false`, `disjunction` at the first `true` — which avoids instantiating (and erroring on) later traits.

```cpp
// Listing 24.16: composing traits with short-circuiting combinators
#include <type_traits>

// "all of Ts are integral":
template <typename... Ts>
using all_integral = std::conjunction<std::is_integral<Ts>...>;

static_assert( all_integral<int, long, char>::value);
static_assert(!all_integral<int, double>::value);

// negation:
static_assert(std::negation_v<std::is_pointer<int>>);

// disjunction in an enable_if constraint:
template <typename T,
          typename = std::enable_if_t<
              std::disjunction_v<std::is_integral<T>,
                                 std::is_floating_point<T>>>>
T twice(T x) { return x + x; }
```

Together these turn what were error-prone hand-written recursive traits into composable, short-circuiting one-liners — the same role fold expressions play for runtime variadics.

---

## 24.10 Professional Insights

**Replace every variadic recursion you can with a fold expression.** A two-overload recursive template to sum, print, or AND-reduce a pack is now a single fold — less code, faster to compile, and clearer intent. Reserve recursion for genuinely position-dependent processing; for "combine all of them with an operator," fold.

**Let CTAD delete your `make_*` helpers.** `std::pair(a, b)`, `std::lock_guard(m)`, and `std::vector{1, 2, 3}` are now self-deducing; the `make_pair`/`make_tuple` family exists mainly for backward compatibility and for the rare case where you want decay semantics that CTAD does not apply. Write a deduction guide when your own class type's constructors don't lead the compiler to the specialization you intend.

**Use `std::invoke` whenever you call a stored, unknown callable.** It is the only spelling that correctly handles pointers-to-member alongside ordinary callables, and it is what `apply`, `thread`, and `function` use internally. Generic "call this thing" code that uses a raw `f(args...)` will silently fail to compile for member pointers; `std::invoke` will not.

**Prefer `invoke_result_t` and `is_invocable_v` over `result_of`.** `std::result_of` is deprecated in C++17 (and removed later) because it cannot express all `INVOKE` cases; the `invoke_*` traits are the correct, future-proof replacements. Likewise reach for `std::not_fn` over `not1`/`not2`.

**`void_t` and the conjunction/disjunction/negation combinators are your standard detection toolkit.** Before writing a bespoke recursive trait, ask whether `void_t` plus a partial specialization expresses the detection, and whether the logical combinators express the constraint with short-circuiting. They instantiate fewer types, fail more cleanly, and read as the boolean logic they implement.
