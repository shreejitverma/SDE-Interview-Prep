# Chapter 55: Deducing `this` — The Explicit Object Parameter

> For its entire history, C++ passed the object a member function operates on through a hidden, untyped `this` pointer. You could not name it, deduce its value category, or write one function that adapts to being called on an lvalue versus an rvalue versus a `const` object — so the language forced you into overload sets, CRTP scaffolding, and `std::function`-wrapped recursion. C++23's *explicit object parameter*, universally called **deducing `this`**, makes the object a normal, named, deducible template parameter. This single change collapses four idioms into one and is the most far-reaching core-language feature in the release.

## Table of Contents

1. [The Hidden Parameter and Why It Hurt](#551-the-hidden-parameter-and-why-it-hurt)
2. [Syntax and Semantics of the Explicit Object Parameter](#552-syntax-and-semantics-of-the-explicit-object-parameter)
3. [Pattern A: Collapsing Ref-Qualified Overloads](#553-pattern-a-collapsing-ref-qualified-overloads)
4. [Pattern B: Replacing CRTP for Static Polymorphism](#554-pattern-b-replacing-crtp-for-static-polymorphism)
5. [Pattern C: Recursive Lambdas](#555-pattern-c-recursive-lambdas)
6. [The Rules: What You Can and Cannot Do](#556-the-rules-what-you-can-and-cannot-do)
7. [Performance and Code-Size Implications](#557-performance-and-code-size-implications)
8. [Professional Insights](#558-professional-insights)

---

## 55.1 The Hidden Parameter and Why It Hurt

Every non-static member function has an implicit first parameter: the object it is called on, reachable through `this`. That pointer has always been a second-class citizen. You cannot name it as a parameter, you cannot deduce its type, and — critically — its **value category** (is the object an lvalue, an rvalue, `const`, `volatile`?) is expressed only through *trailing qualifiers* on the function, not through the type system in a way templates can grab.

The consequence is a family of well-known boilerplate idioms, each a workaround for the same missing capability:

- **The four-overload accessor explosion.** A getter that wants to forward the object's value category correctly must be written four times — `&`, `const&`, `&&`, `const&&`.
- **CRTP** (the Curiously Recurring Template Pattern), where a base class is parameterized on its own derived type purely so it can `static_cast<Derived*>(this)`.
- **Recursive lambdas**, which had no name to call themselves with and so required a `std::function` wrapper or a Y-combinator trick.

Deducing `this` addresses all three with one mechanism: let the member function declare the object as an explicit, named, deducible parameter.

---

## 55.2 Syntax and Semantics of the Explicit Object Parameter

A member function may declare its first parameter with the keyword `this`:

```cpp
struct Widget {
    template <typename Self>
    void process(this Self&& self) {
        // 'self' IS the object. 'this' is not available here.
    }
};
```

The parameter `this Self&& self` is the **explicit object parameter**. The keyword `this` appears only on the *first* parameter and only in its declaration; everywhere else `self` is an ordinary parameter name.

The defining behaviors:

1. **`Self` is deduced like any forwarding reference.** Call `process` on a `Widget&` and `Self` deduces to `Widget&`; call it on a `const Widget&` and `Self` is `const Widget&`; call it on a temporary and `Self` is `Widget`. The value category of the *caller's object* is captured in the type.
2. **Inside the function, `this` does not exist.** You use the named parameter (`self`) instead. Member access is `self.member`, not `this->member` or bare `member`.
3. **No trailing ref-qualifiers.** A function with an explicit object parameter may not also be `const`, `&`, `&&`, `volatile`, `static`, or `virtual` — that information now lives in the parameter type.
4. **It is still a member function** for overload resolution and name lookup; the explicit object parameter is purely a different *spelling* of the implicit one.

The object parameter need not be a template. You can pin it to an exact type when that is what you want:

```cpp
struct Logger {
    void log(this const Logger& self, std::string_view msg);  // explicit, non-deduced
};
```

But the deduced, forwarding-reference form is where the power lies, because one function body then serves every value category.

---

## 55.3 Pattern A: Collapsing Ref-Qualified Overloads

The motivating problem. To forward a data member with the correct value category, pre-C++23 code needed the full overload set:

```cpp
// Before C++23: four near-identical overloads.
struct Widget {
    void process() &;        // lvalue
    void process() const&;   // const lvalue
    void process() &&;       // rvalue
    void process() const&&;  // const rvalue
};
```

Each overload typically differs only in how it forwards. With deducing `this`, a single function template handles all four, forwarding `self` so that the value category propagates correctly:

```cpp
// After C++23: one function.
struct Widget {
    template <typename Self>
    void process(this Self&& self) {
        handle(std::forward<Self>(self));   // value category preserved
    }
};
```

The canonical example is a wrapper's accessor. Returning `auto&&` and forwarding `self.payload` means an lvalue wrapper yields an lvalue reference, a `const` wrapper yields a `const` reference, and an rvalue wrapper yields an rvalue reference — automatically, from one line.

**Listing 55.1: One accessor that perfectly forwards value category and constness.**

```cpp
#include <utility>
#include <print>

template <typename T>
class OptionalWrapper {
    T payload{};
public:
    // Replaces the four overloads &, const&, &&, const&&.
    template <typename Self>
    auto&& value(this Self&& self) {
        return std::forward<Self>(self).payload;
    }
};

int main() {
    OptionalWrapper<std::string> w;
    auto& lref = w.value();                       // T&        (lvalue)
    const auto& cref = std::as_const(w).value();  // const T&  (const lvalue)
    auto moved = OptionalWrapper<std::string>{}.value();  // T&& -> moved-from temporary
    std::println("ok: {} {}", lref.size(), moved.size());
}
```

This is not just less code; it is *more correct* code, because the four hand-written overloads were a frequent source of subtle const- or move-correctness bugs.

---

## 55.4 Pattern B: Replacing CRTP for Static Polymorphism

The Curiously Recurring Template Pattern existed so a base class could call into its derived type without the cost of a virtual dispatch. The base had to be a template parameterized on the derived class, and every call site inside the base performed a `static_cast` to recover the derived type.

**Before C++23 — the CRTP way:**

```cpp
template <typename Derived>
struct Base {
    void interface() {
        // Cast 'this' to the Derived type to call its implementation.
        static_cast<Derived*>(this)->implementation();
    }
};

struct Derived : Base<Derived> {
    void implementation() { std::println("Derived implementation"); }
};
```

The angle brackets in `Base<Derived>` are the tell-tale CRTP boilerplate, and they leak into every layer of the hierarchy. With deducing `this`, the base function simply deduces the *actual* most-derived type from the object it was invoked on — no template parameter on the base, no `static_cast`.

**Listing 55.2: Static polymorphism without CRTP.**

```cpp
#include <print>
#include <utility>

struct Base {
    // 'Self' deduces to the exact dynamic-free type that invoked interface().
    template <typename Self>
    void interface(this Self&& self) {
        std::forward<Self>(self).implementation();
    }
};

// No angle brackets in the inheritance list.
struct Derived : Base {
    void implementation() { std::println("Derived implementation"); }
};

int main() {
    Derived d;
    d.interface();   // resolves to Derived::implementation(), statically
}
```

`Self` deduces to `Derived` (or `Derived&`, etc.) because that is the type of the object the call was made on, so `self.implementation()` binds to the derived member with no runtime dispatch. The base class no longer needs to know its derived types at definition time, which makes mixin composition dramatically cleaner.

> **Caveat (mixin layering):** when several deducing-`this` mixins each provide an `interface()`-style entry point, name lookup still follows ordinary base-class rules. Deducing `this` removes the CRTP *parameterization*, not the need to disambiguate genuinely conflicting member names across bases.

---

## 55.5 Pattern C: Recursive Lambdas

A lambda is an unnamed closure object with an `operator()`. Because it has no name in its own scope, it historically could not call itself: there was nothing to name. Programmers reached for `std::function` (which adds type erasure and an indirect call), or passed the lambda to itself manually.

The explicit object parameter solves this directly: the lambda's own closure object can be the deduced `self`.

**Listing 55.3: A self-recursive lambda with no `std::function` overhead.**

```cpp
#include <print>

int main() {
    // 'self' is the closure object itself; it can be invoked recursively.
    auto fib = [](this auto&& self, int n) -> int {
        if (n <= 1) return n;
        return self(n - 1) + self(n - 2);
    };

    std::println("Fibonacci(10) = {}", fib(10));   // 55
}
```

Because `self` is the concrete closure type (deduced, not type-erased), the recursive calls are ordinary, inlinable function calls — there is no allocation, no indirect call through a `std::function` vtable, and the optimizer can see straight through them. This makes deducing `this` the idiomatic way to write recursive lambdas in performance-sensitive code, replacing both the `std::function` workaround and the older Y-combinator helper.

---

## 55.6 The Rules: What You Can and Cannot Do

The explicit object parameter is governed by a precise set of constraints. The ones you will hit in practice:

| Rule | Detail |
|---|---|
| **Position** | The explicit object parameter must be the **first** parameter, and only the first may carry the `this` keyword. |
| **No trailing qualifiers** | A function with an explicit object parameter cannot be `const`, `&`, `&&`, `volatile`, `static`, or `virtual` — those would conflict with the parameter's type. |
| **`this` is gone in the body** | Inside such a function you must use the named parameter; the keyword `this` (and implicit member access) is not available. |
| **No mixing in one declaration** | A single function is either an explicit-object-parameter function or an implicit-object one — not both. |
| **Overloading is allowed** | You may overload an explicit-object-parameter function against implicit-object overloads, subject to the usual ambiguity rules. |
| **Constructors** | An explicit object parameter is **not** permitted on constructors or destructors. |
| **Taking its address** | `&Widget::process` yields a pointer-to-member-function whose signature *includes* the object parameter type, which can surprise generic code. |

A subtle pitfall worth its own callout: because `Self` is a forwarding reference, **a derived class invoking an inherited deducing-`this` member will deduce `Self` to the derived type.** That is exactly what makes Pattern B work — but it means a base-class member can end up instantiated once per derived type, which has code-size consequences (Section 55.7) and can expose derived members the base did not expect. Constrain `Self` with a concept when you need to restrict it.

> **Version-trap flag:** Deducing `this` is C++23. It is frequently demonstrated alongside `std::println` (also C++23) and `std::forward_like` (C++23, Chapter 64) — none of these compile under `-std=c++20`. Do not confuse the explicit object parameter with the unrelated C++26 reflection features.

---

## 55.7 Performance and Code-Size Implications

Deducing `this` is, at the call level, a **zero-overhead** abstraction: the explicit object parameter compiles to exactly the same calling convention as the implicit one, and the forwarding `self.member` access is identical to `this->member`. There is no extra indirection.

The two performance angles a senior engineer must weigh:

1. **It removes overhead that workarounds imposed.** Replacing a `std::function`-wrapped recursive lambda with a deducing-`this` lambda eliminates a heap allocation and an indirect call. Replacing CRTP changes nothing at runtime (CRTP was already static) but simplifies the code the optimizer must chew through.
2. **Templated object parameters multiply instantiations.** A `template <typename Self>` member is instantiated for every distinct value category and every derived type it is called on — potentially `Widget&`, `const Widget&`, `Widget&&`, and one per subclass. For a large hierarchy or a heavily-used accessor this can grow code size and compile time. When the body does not actually depend on the deduced type (only on its value category), prefer the non-template forms or factor the body into a non-template helper that the thin templated wrapper forwards to.

In a low-latency context the guidance is: use the deduced form where value-category forwarding is the point (accessors, recursive lambdas, static dispatch), and pin the object parameter to a concrete type where you only wanted to name `self`, to keep the instantiation count down.

---

## 55.8 Professional Insights

**Reach for deducing `this` to delete overload sets, not to show off.** The clearest wins are the four-overload accessor collapse and CRTP elimination — both replace error-prone boilerplate with a single, provably-correct function. If a member function does not need to adapt to the object's value category or derived type, a plain member function is still the right tool; the explicit object parameter is a precision instrument, not a default.

**Constrain `Self` when the base is shared.** An unconstrained `template <typename Self>` base member will happily instantiate for any caller, including derived types you never intended and value categories you never tested. Add a `requires` clause or a concept on `Self` so the compiler enforces the contract and so accidental instantiations become hard errors rather than silent code-size bloat.

**Watch the instantiation count in hot, header-heavy code.** Because each value category and each derived type can spawn a separate instantiation, a deducing-`this` accessor used pervasively across a large codebase can measurably inflate binary size and compile time. When the body is value-category-agnostic, forward from a thin templated shell to a single concrete implementation — you keep the ergonomic call site and pay for only one instantiation of the real work.

**Prefer it for recursive lambdas in performance code.** The `[](this auto&& self, …)` form gives you recursion with no type erasure, no allocation, and full inlinability — strictly better than the `std::function` idiom it replaces. This alone is reason enough to enable C++23 in a numerics or trading codebase that leans on lambda-heavy algorithm composition.
