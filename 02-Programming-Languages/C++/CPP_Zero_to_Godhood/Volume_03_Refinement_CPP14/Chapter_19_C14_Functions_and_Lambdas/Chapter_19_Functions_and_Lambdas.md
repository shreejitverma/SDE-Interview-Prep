# Chapter 19: Functions and Lambdas

> *C++14's lambda and function-deduction changes are where the "Refinement" release pays off daily. Generic lambdas turn a closure into a template; init-capture lets a closure own a moved-in resource; return-type deduction removes the trailing-return-type ceremony; and `decltype(auto)` finally lets a forwarding wrapper preserve references exactly.*

In C++11 a lambda's parameters had to be concretely typed, a closure could only capture by copy or reference, and any function whose return type you wanted deduced needed an explicit `auto ... -> decltype(...)` tail. C++14 lifts all three limits and adds `decltype(auto)`, a deduction rule that — unlike `auto` — keeps references and `const` intact. Together they make generic, move-aware, perfectly-forwarding code dramatically shorter.

---

## Table of Contents

- [19.1 Generic Lambdas](#191-generic-lambdas)
- [19.2 Lambda Init-Capture (Generalized Capture)](#192-lambda-init-capture-generalized-capture)
- [19.3 Function Return Type Deduction](#193-function-return-type-deduction)
- [19.4 `decltype(auto)`](#194-decltypeauto)
- [19.5 Professional Insights](#195-professional-insights)

---

## 19.1 Generic Lambdas

In C++11 every lambda parameter needed a concrete type, so a closure was monomorphic — one that summed `int`s could not sum `double`s. **C++14 allows `auto` in a lambda's parameter list**, making the lambda *generic*: the compiler synthesizes a templated `operator()` on the closure type, and the actual types are deduced per call, exactly as for a function template.

```cpp
// Listing 19.1: a generic lambda is a closure with a templated operator()
auto sum = [](auto a, auto b) { return a + b; };

sum(3, 4);              // a, b deduced as int    -> 7
sum(1.5, 2.5);          // a, b deduced as double -> 4.0
sum(std::string("x"), std::string("y"));  // -> "xy"
```

Conceptually, `sum` is an object whose class has:

```cpp
// Listing 19.2: the closure type the compiler generates (schematic)
struct __sum_closure {
    template <typename T, typename U>
    auto operator()(T a, U b) const { return a + b; }
};
```

Each parameter spelled `auto` becomes an independent template type parameter, so `sum(1, 2.0)` deduces `T=int`, `U=double`. This makes generic lambdas the natural callable to pass to standard algorithms when the element type is awkward to spell:

```cpp
// Listing 19.3: a generic comparator without naming the iterator's value type
std::sort(v.begin(), v.end(),
          [](const auto& lhs, const auto& rhs) { return lhs.key < rhs.key; });
```

Take parameters by `const auto&` (or forwarding `auto&&`) to avoid copies — the same guidance as for ordinary template parameters.

---

## 19.2 Lambda Init-Capture (Generalized Capture)

C++11 captures were limited to *copy* (`[x]`) and *reference* (`[&x]`) of an existing variable. There was no way to **move** a variable into a closure, which made it impossible to capture a `unique_ptr` or other move-only resource. **C++14 init-capture** (also called *generalized capture*) lets you introduce a new closure member and initialize it with an arbitrary expression — including a `std::move`.

```cpp
// Listing 19.4: moving a move-only resource into a closure
auto data = std::make_unique<int[]>(1000);

// [p = std::move(data)] creates a closure member 'p' initialized by moving 'data'
auto task = [p = std::move(data)]() {
    return p[0];          // the closure now owns the buffer; 'data' is null
};
```

The general form is `[name = expression]`: `name` is a fresh data member of the closure, and `expression` is evaluated in the enclosing scope to initialize it. This subsumes ordinary capture and adds two new capabilities:

```cpp
// Listing 19.5: renaming/transforming a capture, and capturing by computed value
int x = 10;
auto f = [val = x + 5]() { return val; };   // val == 15, independent of x

// Capture a const copy, or a reference under a new name:
auto g = [&r = x]() { r += 1; };             // r is a reference to x
```

Init-capture is the idiomatic C++14 way to give a closure exclusive ownership of a resource — essential when handing a task with a `unique_ptr` payload to a thread pool or `std::async`, where the closure must outlive the enclosing scope.

> **Godhood tip:** prefer init-capture by move over capturing a raw pointer by copy. The closure then owns the resource with correct lifetime semantics; capturing a raw pointer risks the pointee being destroyed before the closure runs.

---

## 19.3 Function Return Type Deduction

C++11 allowed `auto` as a function's return type only with a trailing `-> decltype(...)`. **C++14 lets an ordinary function return `auto` with no trailing type**, deducing the return type from the `return` statements using the same rules as `auto` variable initialization.

```cpp
// Listing 19.6: return type deduction
// C++11 required the trailing return type:
auto add11(int a, int b) -> int { return a + b; }

// C++14: deduced automatically
auto add14(int a, int b) { return a + b; }   // returns int
```

Two rules govern it:

1. **All `return` statements must deduce the same type**, or the program is ill-formed:
   ```cpp
   auto bad(bool c) {
       if (c) return 1;       // int
       else   return 2.0;     // double -> ERROR: inconsistent deduction
   }
   ```
2. **A recursive function must have at least one `return` the compiler sees before the recursive call**, so the type is known when it is needed:
   ```cpp
   auto factorial(int n) {
       if (n <= 1) return 1;           // establishes return type = int
       return n * factorial(n - 1);    // OK: type already deduced
   }
   ```

Because `auto` return deduction applies the `auto` decay rules, the returned type is always a *value* (references and top-level `const` are stripped). When you need to preserve a reference — as in a forwarding accessor — use `decltype(auto)`, next.

---

## 19.4 `decltype(auto)`

`auto` deduction **decays**: it strips references and top-level `const`, always yielding a value type. That is wrong for a function that should forward whatever its expression yields — returning `auto` from an accessor silently copies. **`decltype(auto)`** deduces using `decltype` rules instead, which *preserve* references and `const` exactly.

```cpp
// Listing 19.7: auto decays; decltype(auto) preserves
int  global = 100;
int& get_ref() { return global; }

auto           proxy1() { return get_ref(); }  // deduces int   (a COPY)
decltype(auto) proxy2() { return get_ref(); }  // deduces int&  (the reference)

proxy1() = 200;   // ERROR: assigning to an int prvalue
proxy2() = 200;   // OK: writes through the reference -> global == 200
```

The two forms differ precisely because the deduction rule differs:

| Form | Deduction rule | On an lvalue `int&` expression |
|------|----------------|-------------------------------|
| `auto` | template-argument deduction (decays) | `int` (copy) |
| `decltype(auto)` | `decltype` of the expression (exact) | `int&` (reference) |

The canonical use is a **perfectly forwarding wrapper** whose return type must match the wrapped call's value category:

```cpp
// Listing 19.8: a forwarding wrapper that preserves the callee's return category
template <typename F, typename... Args>
decltype(auto) invoke_logged(F&& f, Args&&... args) {
    log_call();
    return std::forward<F>(f)(std::forward<Args>(args)...);
    // returns exactly what f returns: T, T&, or T&& -- no accidental copy,
    // no dangling from over-eager decay
}
```

> **Caution:** `decltype(auto)` returning a reference to a local, or to a temporary's subobject, dangles just as a hand-written `T&` return would. It preserves whatever category the expression has — including a dangling one. Use it for *forwarding* an existing object's category, not for returning locals.

---

## 19.5 Professional Insights

**Reach for generic lambdas to cut comparator and projection boilerplate.** A `[](const auto& a, const auto& b){ ... }` comparator passed to `std::sort` or `std::max_element` avoids spelling the iterator's value type and adapts automatically if that type changes. Keep parameters `const auto&` or `auto&&` so you don't silently copy elements.

**Init-capture is how you make closures own resources.** Any time a closure outlives its enclosing scope — a task queued to a thread pool, a continuation handed to `std::async`, a callback stored in a handler table — capture move-only payloads with `[p = std::move(ptr)]`. It gives the closure correct ownership and lifetime instead of a borrowed pointer that may dangle.

**Default to deduced returns for templates and small functions, but pin the type at API boundaries.** Return-type deduction is excellent for internal helpers and template glue. For public headers, prefer an explicit return type: it documents the contract, keeps the type stable across refactors, and avoids forcing every caller to recompile when an internal `return` expression changes.

**Use `decltype(auto)` exactly when category preservation matters — and nowhere else.** Forwarding wrappers, transparent accessors, and `operator[]` proxies need it to avoid an accidental copy or to return a true reference. Everywhere else, plain `auto` is clearer and its decay-to-value is the safer default. Never use `decltype(auto)` to return something that lives only inside the function.
