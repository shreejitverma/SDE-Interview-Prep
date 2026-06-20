# Chapter 14: Template Metaprogramming

> *Templates are a Turing-complete functional language that runs inside the compiler. C++11 gave it variadics, type traits, and `constexpr` — turning a clever trick into an engineering discipline.*

Template metaprogramming (TMP) generates and selects code at **compile time**. C++11 transformed TMP from arcane to practical with **variadic templates**, a real **`<type_traits>`** library, **alias templates**, **`constexpr`**, and **`std::tuple`**. This chapter develops the functional model of templates, then the toolkit: parameter packs, metafunctions, type traits, SFINAE, the detection idiom, tag dispatch, and tuples.

---

## Table of Contents

- [14.1 Templates as a Compile-Time Functional Language](#141-templates-as-a-compile-time-functional-language)
- [14.2 Variadic Templates and Parameter Packs](#142-variadic-templates-and-parameter-packs)
- [14.3 Iterating Over a Parameter Pack](#143-iterating-over-a-parameter-pack)
- [14.4 Template Metafunctions](#144-template-metafunctions)
- [14.5 Type Traits](#145-type-traits)
- [14.6 SFINAE](#146-sfinae)
- [14.7 The Detection Idiom (`void_t`)](#147-the-detection-idiom-void_t)
- [14.8 Tag Dispatching](#148-tag-dispatching)
- [14.9 Type Aliases and Alias Templates (`using`)](#149-type-aliases-and-alias-templates-using)
- [14.10 `std::tuple`](#1410-stdtuple)
- [14.11 Professional Insights](#1411-professional-insights)

---

## 14.1 Templates as a Compile-Time Functional Language

The C++ template system is, by itself, a **pure functional language evaluated by the compiler**. It has no mutation and no loops; "variables" are types and compile-time constants, and the only "looping" construct is **recursion**, terminated by a **base case** (a template specialization). By convention, a *template metafunction* exposes its result through a member named `::type` (for type results) or `::value` (for value results).

```cpp
// Listing 14.1: a metafunction computing a value at compile time
template<int N>
struct Factorial {
    static const int value = N * Factorial<N - 1>::value;
};
template<>
struct Factorial<0> {           // base case (recursion terminator)
    static const int value = 1;
};
// Factorial<5>::value == 120, computed entirely by the compiler
int buffer[Factorial<5>::value]; // usable as a compile-time constant
```

Because metafunction results are compile-time constants, they can size arrays, parameterize templates, and feed `static_assert`.

---

## 14.2 Variadic Templates and Parameter Packs

A **variadic template** accepts an arbitrary number of template arguments via a **parameter pack**. Three pieces of syntax govern packs:

- `typename... Args` — a **template parameter pack**
- `Args... args` — a **function parameter pack**
- `sizeof...(Args)` — the **number** of elements in the pack

A pack is consumed by **expansion** (`pattern...`), typically with recursion: peel off the first argument, recurse on the rest, stop at a non-variadic base case.

```cpp
// Listing 14.2: classic recursive variadic print
#include <iostream>

void print() {}                                   // base case

template<typename T, typename... Args>
void print(T first, Args... args) {
    std::cout << first << " ";
    print(args...);                               // expand the rest
}

int main() {
    print(1, "hello", 3.14);                      // 1 hello 3.14
}
```

Packs combine with perfect forwarding (Chapter 12): `std::forward<Args>(args)...` expands each argument while preserving its value category — the mechanism behind `make_unique`, `make_shared`, and `emplace_back`.

---

## 14.3 Iterating Over a Parameter Pack

Recursion (§14.2) is the portable C++11 way to process every element. A second C++11 technique is the **expander trick**, which performs the whole operation in one function using a brace-initialized dummy array to force left-to-right evaluation of the pack expansion:

```cpp
// Listing 14.3: the C++11 "expander" trick (no second overload needed)
template <class... Ts>
void print_all(std::ostream& os, Ts const&... args) {
    using expander = int[];
    (void)expander{ 0, (void(os << args), 0)... }; // evaluates os << arg for each
}
```

The expander is harder to read than recursion but avoids a base-case overload.

> **C++14/17 forward references:** C++17 **fold expressions** make this a one-liner — `((os << args), ...);` — and **`if constexpr`** lets the recursive form live in a single function by compiling the recursive call only when `sizeof...(rest) > 0`. Both are unavailable in C++11.

---

## 14.4 Template Metafunctions

The factorial example (§14.1) is the canonical metafunction. C++11 offers cleaner spellings of the same idea.

**Inheriting from `std::integral_constant`** gives you `::value`, a `value_type`, and a `constexpr` conversion for free:

```cpp
// Listing 14.4: a metafunction via std::integral_constant (C++11)
#include <type_traits>
template<long long n>
struct factorial : std::integral_constant<long long, n * factorial<n-1>::value> {};
template<>
struct factorial<0> : std::integral_constant<long long, 1> {};
// factorial<7>::value == 5040
```

**A `constexpr` function** (Chapter 11) is usually the cleanest tool, and — unlike a metafunction — it *also* works at runtime when its arguments are not compile-time constants:

```cpp
// Listing 14.5: constexpr replaces many metafunctions
constexpr long long factorial(long long n) {
    return n == 0 ? 1 : n * factorial(n - 1); // C++11: single return statement
}

template <typename T>
constexpr T power(T value, unsigned exp) {
    return exp == 0 ? 1 : value * power(value, exp - 1);
}

void use() {
    constexpr int compile_time = power(3, 3); // computed by the compiler
    int x; std::cin >> x;
    int run_time = power(x, 3);               // same code, evaluated at runtime
}
```

This dual nature is the key advantage of `constexpr` over struct-based metafunctions: no code duplication between compile-time and runtime paths.

> **C++14 forward reference:** C++14 lifts the single-return restriction, so `constexpr` factorials can use `if`/`for` directly.

---

## 14.5 Type Traits

`<type_traits>` provides compile-time **inspection** and **transformation** of types. Inspection traits expose `::value`; transformation traits expose `::type`.

```cpp
// Listing 14.6: inspecting types
#include <type_traits>
static_assert(std::is_integral<int>::value,  "int is integral");
static_assert(std::is_pointer<int*>::value,  "int* is a pointer");
static_assert(std::is_same<int, signed int>::value, "same type");
```

**Writing your own trait** follows a three-step pattern: a primary template inheriting `std::false_type`, a specialization inheriting `std::true_type`, and (optionally) a front-end that strips qualifiers:

```cpp
// Listing 14.7: implementing is_pointer from scratch
template <typename T> struct is_pointer_      : std::false_type {};
template <typename T> struct is_pointer_<T*>  : std::true_type  {};
template <typename T> struct is_pointer
    : is_pointer_<typename std::remove_cv<T>::type> {};   // strip const/volatile

static_assert(is_pointer<int* const>::value, "yes");
```

**Compile-time `if`/`else` on types** is `std::conditional`:

```cpp
// Listing 14.8: select a type at compile time
template<typename T>
struct ValueOrPointer {
    // store T directly if small, else store a T* — sizeof stays <= a pointer
    typename std::conditional<(sizeof(T) > sizeof(void*)), T*, T>::type vop;
};
```

Traits power generic decisions: e.g. choose `unordered_map` when a key is hashable, otherwise `map` (combining a detection trait — §14.7 — with `std::conditional`).

---

## 14.6 SFINAE

**SFINAE** — *Substitution Failure Is Not An Error* — is the rule that makes constrained overloading work: if substituting template arguments produces an ill-formed type or expression **in the immediate context**, that candidate is silently *removed* from overload resolution rather than causing a hard error.

```cpp
// Listing 14.9: SFINAE removes a non-viable candidate
template <class T>
auto begin(T& c) -> decltype(c.begin()) { return c.begin(); } // #1
template <class T, std::size_t N>
T* begin(T (&arr)[N]) { return arr; }                         // #2

int vals[10];
begin(vals); // #1 substitution fails (arrays have no .begin()); NOT an error.
             // #1 is discarded; #2 is selected.
```

Only failures **in the immediate context** of substitution are soft; an error deeper inside an instantiated body is a hard error.

**`std::enable_if`** is the standard switch for triggering SFINAE deliberately. `enable_if<true, R>::type` is `R`; `enable_if<false, R>::type` does not exist, so any overload depending on it vanishes:

```cpp
// Listing 14.10: constraining an overload with enable_if
template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
gcd(T a, T b) { return b == 0 ? a : gcd(b, a % b); } // only for integral T
```

Use `enable_if` when the *intent* of a constraint (`is_signed`, `is_sizeable`) reads more clearly as a named trait than as a raw `decltype` expression.

---

## 14.7 The Detection Idiom (`void_t`)

A recurring need is to ask "is this expression valid for type `T`?" — does `T` have a member, an operator, a nested type? The **detection idiom** answers it, built on **`void_t`**, a metafunction that maps any well-formed type list to `void`:

```cpp
// Listing 14.11: void_t (a C++11 one-liner; standardized as std::void_t in C++17)
template <class...> using void_t = void;
```

```cpp
// Listing 14.12: detect a member function foo()
#include <type_traits>
#include <utility>   // std::declval

template <class T, class = void>
struct has_foo : std::false_type {};

template <class T>
struct has_foo<T, void_t<decltype(std::declval<T&>().foo())>>
    : std::true_type {};

struct A {};
struct B { void foo(); };
static_assert(!has_foo<A>::value, "A has no foo");
static_assert( has_foo<B>::value, "B has foo");
```

The mechanism: when `T.foo()` is well-formed, the specialization's second argument resolves to `void` and is preferred (partial-ordering rules); otherwise substitution fails softly and the `false_type` primary remains. The same pattern detects operators (`decltype(std::declval<T>() < std::declval<T>())`), nested types, and `std::hash` specializations.

---

## 14.8 Tag Dispatching

**Tag dispatching** selects an overload at compile time by passing an empty *tag* type (often a trait result) as an extra argument. It is frequently more readable than nested `enable_if`. The standard's own `std::advance` dispatches on the iterator category:

```cpp
// Listing 14.13: tag dispatch on iterator category
namespace detail {
    template <class It, class D>
    void advance(It& it, D n, std::random_access_iterator_tag) { it += n; }      // O(1)
    template <class It, class D>
    void advance(It& it, D n, std::bidirectional_iterator_tag) {                 // O(n)
        for (; n > 0; --n) ++it;
        for (; n < 0; ++n) --it;
    }
    template <class It, class D>
    void advance(It& it, D n, std::input_iterator_tag) { for (; n > 0; --n) ++it; }
}

template <class It, class D>
void advance(It& it, D n) {
    detail::advance(it, n,
        typename std::iterator_traits<It>::iterator_category{}); // pick by tag
}
```

The tag argument is an empty struct that the optimizer erases entirely; its sole purpose is to steer overload resolution. Tag dispatch composes cleanly for "select the best of several" cases where chained `enable_if` negations would be unwieldy.

---

## 14.9 Type Aliases and Alias Templates (`using`)

C++11's **`using`** declaration replaces `typedef` and, crucially, supports **templated aliases** — something `typedef` cannot express.

```cpp
// Listing 14.14: alias and alias template
using Bytes = unsigned char;                 // like typedef, clearer left-to-right

template<typename T>
using Dictionary = std::map<std::string, T>; // alias TEMPLATE — impossible with typedef
Dictionary<int> scores;                      // std::map<std::string, int>
```

Alias templates are the idiomatic way to provide trait shortcuts (the `_t`/`_v` convention): `template<class T> using remove_cv_t = typename std::remove_cv<T>::type;`. The standard library added these `_t` aliases in C++14 and `_v` variable templates in C++17; in C++11 you write the `typename ...::type` form or define the aliases yourself.

---

## 14.10 `std::tuple`

`std::tuple<Ts...>` generalizes `std::pair` to any number of heterogeneous elements — a variadic product type.

```cpp
// Listing 14.15: tuple basics
#include <tuple>
std::tuple<int, double, std::string> t(1, 3.14, "hi");
int    i = std::get<0>(t);            // access by index
double d = std::get<1>(t);
auto   u = std::make_tuple(42, 'x');  // type-deduced construction
std::size_t n = std::tuple_size<decltype(t)>::value; // 3
```

**Unpacking a tuple into a function call** is the classic variadic exercise: generate a compile-time index sequence, then expand `std::get<Is>(tuple)...` as the call's arguments. (C++14 standardized `std::index_sequence`; C++17 standardized this operation as `std::apply`.)

```cpp
// Listing 14.16: calling a function with arguments stored in a tuple
namespace detail {
    template <class F, class Tuple, std::size_t... Is>
    auto apply_impl(F&& f, Tuple&& t, /*index seq*/ index_sequence<Is...>)
        -> decltype(std::forward<F>(f)(std::get<Is>(std::forward<Tuple>(t))...)) {
        return std::forward<F>(f)(std::get<Is>(std::forward<Tuple>(t))...);
    }
}
// apply(f, make_tuple(42,'x',3.14)) calls f(42,'x',3.14)
```

Tuples shine for returning multiple values, storing deferred call arguments (Chapter 13), and as the storage backbone of generic libraries.

---

## 14.11 Professional Insights

**Recursion depth is bounded.** Compilers cap template instantiation depth (g++ defaults to ~900; older builds 256). Deeply recursive metafunctions can blow this limit — raise it with `-ftemplate-depth=N`, or restructure to logarithmic depth (divide-and-conquer instantiation) for large packs.

**Prefer `constexpr` over struct metafunctions when you can.** A `constexpr` function reads like ordinary code, debugs like ordinary code, and serves both compile-time and runtime callers from one definition. Reserve struct metafunctions for *type-level* computation that `constexpr` cannot express (transforming types, not values).

**TMP is a build-time cost.** Every distinct instantiation is compiled. Heavy trait machinery and large variadic expansions inflate compile times and binary size — a real concern in large systems. Measure; cache common instantiations behind explicit `extern template` declarations (Chapter 17) where appropriate.

**`if constexpr` is the future, but not a total replacement.** C++17's `if constexpr` cleans up *implementation* branching that used to require `enable_if`, but it does **not** replace SFINAE for *overload-set* and *open* extensibility (tag dispatch still wins where third parties must add overloads). In C++11 you have neither `if constexpr` nor fold expressions — recursion, the expander trick, `enable_if`, and tag dispatch are your tools.

**Name your constraints.** A trait named `is_sizeable<T>` or `is_signed<T>` documents intent far better than an inline `decltype(...)` leaking implementation into a signature. Readability of constraints is a maintainability multiplier in template-heavy code.
