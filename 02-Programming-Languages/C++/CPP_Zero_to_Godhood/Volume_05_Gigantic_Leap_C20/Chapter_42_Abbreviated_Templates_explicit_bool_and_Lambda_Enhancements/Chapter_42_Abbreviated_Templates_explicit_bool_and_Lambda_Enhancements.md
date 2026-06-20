# Chapter 42: Abbreviated Templates, explicit(bool), and Lambda Enhancements

> *C++20 polishes the everyday ergonomics of generic code. Abbreviated function templates let `auto` parameters stand in for template parameters so an ordinary-looking function is silently a template; `explicit(bool)` makes a constructor's explicitness a compile-time condition rather than an all-or-nothing decision; and lambdas gain a real template-parameter list, pack capture, default-constructibility, and the ability to appear in unevaluated contexts. This chapter covers all three clusters and the subtle rules that make them safe.*

These features share a theme: removing boilerplate that previously stood between intent and expression. The `template<typename T>` header that added nothing, the pair of constructors that differed only in `explicit`, the verbose `template` parameter list bolted onto a generic lambda — C++20 collapses each into a tighter form. But each also has a sharp edge: an abbreviated template is still a template (with all the overload-resolution consequences), `explicit(bool)` silently changes implicit-conversion behavior, and lambda pack capture interacts with forwarding in ways that reward precision.

---

## Table of Contents

- [42.1 Abbreviated Function Templates](#421-abbreviated-function-templates)
- [42.2 The Equivalence: auto Parameters Are Template Parameters](#422-the-equivalence-auto-parameters-are-template-parameters)
- [42.3 Constrained auto Parameters](#423-constrained-auto-parameters)
- [42.4 explicit(bool): Conditional Explicitness](#424-explicitbool-conditional-explicitness)
- [42.5 Template Parameter Lists for Lambdas](#425-template-parameter-lists-for-lambdas)
- [42.6 Pack Expansion in Lambda Init-Capture](#426-pack-expansion-in-lambda-init-capture)
- [42.7 Default-Constructible and Assignable Stateless Lambdas](#427-default-constructible-and-assignable-stateless-lambdas)
- [42.8 Lambdas in Unevaluated Contexts and [=, this]](#428-lambdas-in-unevaluated-contexts-and--this)
- [42.9 Professional Insights](#429-professional-insights)

---

## 42.1 Abbreviated Function Templates

An **abbreviated function template** is a function with one or more `auto` parameters. Each `auto` parameter introduces an invented template type parameter, so the function is a template without the `template<...>` header.

```cpp
// Listing 42.1: an auto parameter makes the function a template
auto add(auto a, auto b) {        // two invented type parameters
    return a + b;
}

add(1, 2);        // instantiates add<int, int>
add(1.0, 2.0);    // instantiates add<double, double>
add(1, 2.0);      // instantiates add<int, double>
```

Each `auto` in the parameter list is independent — `add(auto, auto)` is a two-parameter template, equivalent to `template<class T, class U> auto add(T, U)`. This is the function-parameter analogue of the generic lambda's `auto` parameters, generalized to ordinary functions. The syntax is at its best for short, obviously-generic utilities where the explicit template header would be pure noise.

---

## 42.2 The Equivalence: auto Parameters Are Template Parameters

The mental model that prevents surprises: an abbreviated template is **exactly** a normal template with invented parameters. It participates in overload resolution, can be specialized via the long form, and obeys all the usual template rules.

```cpp
// Listing 42.2: the abbreviated form and its desugared equivalent
auto twice(auto x) { return x + x; }
// is IDENTICAL to:
template<class T> auto twice(T x) { return x + x; }

// You can even mix invented and explicit parameters:
template<class T>
auto scale(T factor, auto value) { return factor * value; }
//  ^ explicit T                ^ invented second parameter
```

Two consequences engineers must internalize. First, because it is a template, it is only fully type-checked on instantiation — a typo in the body compiles until someone calls it (the classic template-lateness problem). Second, you cannot take a plain function pointer to an abbreviated template without specifying the types, exactly as with any template. Treat `auto` parameters as a spelling shortcut for a template, not as a distinct "polymorphic function" feature.

---

## 42.3 Constrained auto Parameters

An `auto` parameter can be **constrained** by prefixing it with a concept (Chapters 32–33), which is the idiomatic C++20 way to bound an abbreviated template. `Concept auto x` means "an invented parameter whose type must satisfy `Concept`."

```cpp
// Listing 42.3: constraining abbreviated-template parameters with concepts
#include <concepts>

auto add(std::integral auto a, std::integral auto b) {   // both must be integral
    return a + b;
}

add(1, 2);        // OK
// add(1.0, 2.0); // ERROR: double does not satisfy std::integral

// Mixed: one constrained, one unconstrained
void log_value(std::convertible_to<std::string_view> auto msg, auto level);
```

This is the single most important habit for abbreviated templates: an *unconstrained* `auto` parameter accepts anything and defers all errors to deep inside the body, while a *constrained* one (`std::integral auto`, `std::ranges::range auto`, a custom concept) produces a clear, localized diagnostic at the call site. The constrained-`auto` form is what makes abbreviated templates production-grade rather than a footgun — it pairs the brevity of `auto` with the diagnostics of concepts.

---

## 42.4 explicit(bool): Conditional Explicitness

`explicit(bool-expr)` makes a constructor (or conversion operator) explicit **only when the compile-time condition is true**. This replaces the pre-C++20 pattern of writing two near-identical constructors selected by SFINAE.

```cpp
// Listing 42.4: explicit(bool) — explicitness depends on the type
#include <type_traits>

template<class T>
struct Wrapper {
    // Implicit when T converts to int; explicit otherwise.
    explicit(!std::is_convertible_v<T, int>) Wrapper(T);
};

Wrapper<int>    a = 5;     // OK: implicit (int converts to int)
Wrapper<std::string> b{std::string{}};   // OK: explicit form required
// Wrapper<std::string> c = std::string{}; // ERROR: copy-init blocked (explicit)
```

`explicit(true)` is the same as plain `explicit`; `explicit(false)` is the same as non-explicit. The value comes from making explicitness *depend on a trait*, which is exactly what wrapper, optional-like, and tuple types need: `std::tuple`, for instance, uses `explicit(bool)` so its constructor is implicit when every element converts implicitly and explicit otherwise. Before C++20 this required two overloaded constructors guarded by `enable_if`; `explicit(bool)` expresses the same intent in one line.

---

## 42.5 Template Parameter Lists for Lambdas

Generic lambdas got `auto` parameters in C++14, but `auto` alone cannot name the deduced type or constrain relationships between parameters. C++20 adds an explicit **template parameter list** to lambdas, written `[]<typename T>(...)`.

```cpp
// Listing 42.5: an explicit template parameter list gives the type a name
#include <vector>

// Pre-C++20: 'auto v' works but you cannot name the element type cleanly.
auto sum_old = [](auto v) { /* element type is decltype(v)::value_type, awkward */ };

// C++20: name T, use it in the body and to relate parameters.
auto first = []<typename T>(const std::vector<T>& v) -> T {
    return v.front();                  // T is directly usable
};

// Enforce that two parameters share a type:
auto same = []<typename T>(T a, T b) { return a == b; };
// same(1, 2.0);   // ERROR: T cannot be both int and double — exactly the intent
```

The explicit list solves real problems `auto` cannot: naming the element type of a container parameter, requiring two arguments to be the *same* type (rather than independently deduced), perfect-forwarding with a named type, and applying `requires` clauses to the lambda. It is the lambda equivalent of writing a full template header and is the right tool whenever a generic lambda needs to *refer to* its deduced types.

---

## 42.6 Pack Expansion in Lambda Init-Capture

C++20 allows **parameter packs to be captured by init-capture**, including with `std::forward`, solving the long-standing problem of perfectly capturing a variadic pack into a lambda.

```cpp
// Listing 42.6: capturing a forwarded parameter pack into a lambda
#include <utility>
#include <tuple>

template<class F, class... Args>
auto defer(F f, Args&&... args) {
    // Capture the pack by move/forward into the lambda's storage:
    return [f = std::move(f), ...args = std::forward<Args>(args)]() mutable {
        return f(std::move(args)...);   // use the captured pack
    };
}
```

Before C++20, capturing a pack required wrapping it in a `std::tuple` and unpacking with `std::apply` — verbose and error-prone. The `[...args = std::forward<Args>(args)]` syntax captures each pack element directly into the closure, with the value category chosen by `std::forward`. This is the enabling feature for writing correct deferred-call, continuation, and task-wrapping utilities that must own their arguments, exactly the building blocks the coroutine and concurrency chapters rely on.

---

## 42.7 Default-Constructible and Assignable Stateless Lambdas

In C++20 a **stateless** (capture-less) lambda's closure type is **default-constructible and assignable**. This lets a lambda type be used where a default-constructed instance is needed — most usefully as a comparator or hasher template argument for associative containers.

```cpp
// Listing 42.7: a stateless lambda as a container comparator, via its type
#include <map>
#include <string>
#include <set>

// The lambda's TYPE is used as the comparator; the container default-constructs it.
std::map<std::string, int,
         decltype([](const std::string& a, const std::string& b){ return a < b; })> m;

std::set<int, decltype([](int a, int b){ return a > b; })> descending;  // reverse order

// Default-construct and assign a stateless lambda directly:
auto cmp = [](int a, int b){ return a < b; };
decltype(cmp) cmp2;        // C++20: default-constructible
cmp2 = cmp;                // C++20: assignable
```

Pre-C++20, a lambda closure had a deleted default constructor, so you could not name its type as a container's comparator and let the container construct it — you had to pass an instance to the constructor or use a named function object. C++20's default-constructibility removes that friction: `decltype([...])` as a template argument "just works," making inline lambdas first-class comparators and hashers. The lambda must be **capture-less** for this — a stateful lambda has nothing sensible to default-construct.

---

## 42.8 Lambdas in Unevaluated Contexts and [=, this]

Two smaller C++20 lambda refinements close remaining gaps:

- **Lambdas in unevaluated contexts.** A lambda expression may now appear inside `decltype`, `sizeof`, `noexcept`, and other unevaluated operands. Combined with default-constructibility (Section 42.7), this is what makes `decltype([]{...})` usable as a type.
- **`[=, this]` capture.** C++20 deprecates implicit `this` capture under `[=]` (which silently captured the object by *pointer*, a dangling-by-surprise hazard) and lets you spell the intent explicitly: `[=, this]` captures the enclosing object by reference (pointer) alongside by-value captures, while `[=, *this]` captures a *copy* of the object.

```cpp
// Listing 42.8: explicit this-capture and a lambda in an unevaluated context
#include <type_traits>

struct Widget {
    int value = 0;

    auto make_reader() {
        // Explicit: capture 'this' (by pointer) plus everything else by value.
        return [=, this]() { return value; };   // C++20: [=] alone is deprecated here
    }
    auto make_snapshot() {
        // Capture a COPY of *this — safe to outlive the Widget.
        return [*this]() { return value; };
    }
};

// Lambda in an unevaluated context: querying the closure type.
using Closure = decltype([](int x){ return x * 2; });
static_assert(std::is_default_constructible_v<Closure>);   // true in C++20
```

The `[=, this]` change matters for correctness: the old implicit-`this`-under-`[=]` capture looked like a value capture but was a pointer capture, so the lambda dangled if it outlived the object. C++20 forces you to choose — `this` (pointer, must not outlive the object) or `*this` (copy, may outlive it) — making the lifetime decision explicit at the capture site.

---

## 42.9 Professional Insights

**Always constrain abbreviated-template `auto` parameters.** An unconstrained `auto` parameter is a template that accepts anything and reports errors deep in the body at instantiation. Writing `std::integral auto`, `std::ranges::range auto`, or a domain concept turns those into localized call-site diagnostics and documents the function's contract. Reserve bare `auto` parameters for genuinely any-type utilities; everywhere else, the constrained form is the professional default.

**Remember an abbreviated template is a real template, with template lateness.** Body errors surface only on instantiation, you cannot take an unspecialized function pointer to it, and it participates fully in overload resolution. The terse syntax can lull you into thinking it is an ordinary function; it is not. Keep abbreviated templates small and well-constrained so the template-lateness surface stays manageable.

**Use `explicit(bool)` to collapse twin SFINAE constructors into one.** Wrapper, optional, variant, and tuple-like types historically carried duplicate constructors differing only in `explicit`, selected by `enable_if`. `explicit(trait)` expresses the same conditional explicitness in a single declaration — less code, clearer intent, and no overload-set bloat. Mirror the standard library's own use of it in `std::tuple`/`std::optional` for your generic wrappers.

**Reach for the lambda template-parameter list when a generic lambda must name or relate its types.** `[]<typename T>(T a, T b)` enforces that two arguments share a type — something `(auto a, auto b)` cannot express, since each `auto` deduces independently. Use it to constrain relationships, name a container's element type, and attach `requires` clauses; fall back to plain `auto` parameters only when the types are truly independent and unnamed.

**Make the `this`-capture lifetime decision explicit, and prefer `[*this]` for escaping lambdas.** Any lambda that may outlive its enclosing object must capture `*this` (a copy), not `this` (a pointer). C++20 deprecated the silent `[=]`-captures-`this`-by-pointer behavior precisely because it produced dangling captures that looked safe. Spell `[=, this]` when the lambda stays within the object's lifetime and `[*this]` when it escapes — and treat any stored or asynchronous lambda as escaping by default.
