# Chapter 32: Concepts I — Constraints, requires-Clauses, and requires-Expressions

> *Concepts are the first of C++20's four pillars and the one that most directly changes day-to-day template code. They turn template parameters from unchecked "duck typing resolved deep inside instantiation" into named, compile-time-verified contracts. This chapter covers the mechanics: how to define a concept, how to attach constraints with a requires-clause, how to express ad-hoc requirements with a requires-expression, and the four ways to spell a constrained template.*

Before C++20, expressing "this template only works for types that support `+`" meant SFINAE, `std::enable_if`, tag dispatch, or a `static_assert` buried in the body — all of which produced diagnostics measured in screenfuls. Concepts replace that machinery with a first-class language feature that names the requirement, checks it at the call site, and emits a one-line error when it fails. This chapter is the foundation; Chapter 33 covers the standard concepts library and the subsumption rules that make constrained overloading work.

---

## Table of Contents

- [32.1 The Problem Concepts Solve](#321-the-problem-concepts-solve)
- [32.2 Defining a Concept](#322-defining-a-concept)
- [32.3 The Four Ways to Constrain a Template](#323-the-four-ways-to-constrain-a-template)
- [32.4 requires-Clauses](#324-requires-clauses)
- [32.5 requires-Expressions](#325-requires-expressions)
  - [32.5.1 Simple Requirements](#3251-simple-requirements)
  - [32.5.2 Type Requirements](#3252-type-requirements)
  - [32.5.3 Compound Requirements](#3253-compound-requirements)
  - [32.5.4 Nested Requirements](#3254-nested-requirements)
- [32.6 Constrained auto](#326-constrained-auto)
- [32.7 Constraint-Based Overloading vs. SFINAE](#327-constraint-based-overloading-vs-sfinae)
- [32.8 Diagnostics: The Real Payoff](#328-diagnostics-the-real-payoff)
- [32.9 Professional Insights](#329-professional-insights)

---

## 32.1 The Problem Concepts Solve

Consider a generic `sort`-like function template. If you hand it a `std::list` (which lacks random-access iterators), pre-C++20 the compiler does not complain at the call site. It complains **deep inside the algorithm**, where `it + n` fails — often dozens of instantiation frames away, in a message hundreds of lines long that names internal helper types you have never heard of.

The standard analogy is the **bouncer**: old C++ lets everyone into the club and only discovers the problem when someone tries to do something they cannot, far inside. Concepts put a bouncer at the door who checks IDs: *"I only admit types that are `std::random_access_iterator`. `std::list::iterator` is not one. Denied — at the call site, in one line."*

The benefit is threefold:

- **Diagnostics** become short and point at the call site.
- **Overload resolution** can select between functions based on type properties, cleanly.
- **Interfaces** become self-documenting — the constraint *is* the contract.

There is **no runtime cost**: concepts are evaluated entirely at compile time and erased.

---

## 32.2 Defining a Concept

A **concept** is a named compile-time predicate over template parameters, introduced with the `concept` keyword. Its body is a constant expression of type `bool` — most usefully, a type trait or a requires-expression.

```cpp
// Listing 32.1: defining concepts from a trait and from a requires-expression
#include <concepts>
#include <type_traits>

// Form 1: built on a type trait
template<typename T>
concept Integral = std::is_integral_v<T>;

// Form 2: built on a requires-expression (tests expression validity)
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;   // a + b must be valid AND convertible to T
};
```

A concept is used wherever a constraint is expected. It can be **composed** with `&&` and `||`, which (unlike a plain `bool` expression) participate in the **subsumption** rules covered in Chapter 33:

```cpp
// Listing 32.2: composing concepts
template<typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template<typename T>
concept SignedNumber = Number<T> && std::is_signed_v<T>;
```

---

## 32.3 The Four Ways to Constrain a Template

C++20 offers four interchangeable syntaxes for applying a concept. Knowing all four is essential because real codebases use all four, and each reads best in different situations.

```cpp
// Listing 32.3: the four constraint syntaxes, all equivalent in effect
#include <concepts>

// (1) Trailing requires-clause
template<typename T>
T inc1(T x) requires std::integral<T> { return x + 1; }

// (2) Leading requires-clause
template<typename T>
    requires std::integral<T>
T inc2(T x) { return x + 1; }

// (3) Constrained template parameter (concept in place of 'typename')
template<std::integral T>
T inc3(T x) { return x + 1; }

// (4) Abbreviated function template (constrained auto parameter)
std::integral auto inc4(std::integral auto x) { return x + 1; }
```

| Syntax | Best when |
|--------|-----------|
| Trailing `requires` | the constraint depends on the parameter types or is complex |
| Leading `requires` | the constraint is a standalone clause you want prominent |
| Constrained parameter `template<C T>` | a single, simple concept on one parameter |
| Abbreviated `C auto` | terse generic functions; no need to name the type |

All four are equivalent for overload resolution; they differ only in readability. Mixing a leading and a trailing requires-clause on the same template is allowed and combines with `&&`.

---

## 32.4 requires-Clauses

A **requires-clause** attaches a boolean constraint expression to a template. Its operand must be a *constraint expression*: a concept, a `bool` constant expression in parentheses, or a conjunction/disjunction of these.

```cpp
// Listing 32.4: requires-clauses with composed and parenthesized constraints
#include <concepts>
#include <type_traits>

template<typename T>
    requires std::integral<T> && (sizeof(T) >= 4)   // concept && parenthesized bool
T checked_double(T x) { return x * 2; }

// A requires-clause can also gate a class template:
template<typename T>
    requires std::copyable<T>
class Box {
    T value;
};
```

A subtlety that trips people: a **raw boolean expression** like `sizeof(T) >= 4` must be **parenthesized** when combined with `&&`/`||` in a requires-clause, because the grammar for constraint expressions only allows primary expressions there. Concepts and parenthesized expressions are primary; bare relational expressions are not.

---

## 32.5 requires-Expressions

A **requires-expression** is a distinct construct (note: *expression*, not *clause*) that yields a `bool` at compile time: `true` if every requirement inside is satisfied, `false` otherwise. It is the workhorse for defining concepts. Its optional parameter list introduces fictional variables used only to test expression validity — they are never evaluated.

```cpp
// Listing 32.5: anatomy of a requires-expression
template<typename T>
concept Stack = requires(T s, typename T::value_type v) {
    s.push(v);          // simple requirement
    s.pop();
    { s.top() } -> std::same_as<typename T::value_type&>;  // compound requirement
    typename T::value_type;                                 // type requirement
    requires std::default_initializable<T>;                 // nested requirement
};
```

There are exactly four kinds of requirement inside the braces.

### 32.5.1 Simple Requirements

A **simple requirement** is just an expression followed by `;`. It is satisfied if the expression is *valid* (compiles) — the expression is never evaluated and its result is discarded.

```cpp
// Listing 32.6: simple requirements
template<typename T>
concept Incrementable = requires(T x) {
    x++;        // postfix increment must be valid
    ++x;        // prefix increment must be valid
    x + x;      // addition must be valid (result type/value irrelevant here)
};
```

### 32.5.2 Type Requirements

A **type requirement** is the keyword `typename` followed by a (possibly qualified) type name. It is satisfied if the named type exists and is valid.

```cpp
// Listing 32.7: type requirements
template<typename T>
concept HasValueType = requires {
    typename T::value_type;        // T must have a nested ::value_type
    typename T::iterator;          // ...and a nested ::iterator
};
```

### 32.5.3 Compound Requirements

A **compound requirement** wraps an expression in braces and optionally asserts properties of its result: that it is `noexcept`, and/or that its type satisfies a *type-constraint* (a concept applied to the result type).

```cpp
// Listing 32.8: compound requirements with return-type constraints and noexcept
#include <concepts>

template<typename T>
concept Hashable = requires(const T& x) {
    { std::hash<T>{}(x) } -> std::convertible_to<std::size_t>;  // result convertible to size_t
};

template<typename T>
concept NothrowSwappable = requires(T& a, T& b) {
    { a.swap(b) } noexcept;        // a.swap(b) must be valid AND noexcept
};
```

The form `{ expr } -> Concept;` means "`expr` is valid, and `Concept<decltype((expr))>` holds." This is the most common and most powerful requirement kind, because it constrains *what an expression produces*, not merely that it compiles.

### 32.5.4 Nested Requirements

A **nested requirement** is the keyword `requires` followed by a constraint expression. Unlike a simple requirement (which only checks validity), a nested requirement checks that the constraint is *satisfied* — its boolean value must be `true`.

```cpp
// Listing 32.9: nested requirement enforces a predicate, not just validity
#include <concepts>

template<typename T>
concept SmallTrivial = requires {
    requires std::is_trivially_copyable_v<T>;   // the predicate must be TRUE
    requires sizeof(T) <= 16;                    // and this one too
};
```

The distinction matters: `sizeof(T) <= 16;` as a *simple* requirement would only check that `sizeof(T) <= 16` is a valid expression (it always is), so it would be useless. As a *nested* requirement, `requires sizeof(T) <= 16;` checks that the value is `true`.

---

## 32.6 Constrained auto

C++20 lets a concept replace `auto` anywhere `auto` is allowed: function parameters (creating an abbreviated function template), return types, variables, and even non-type contexts via `decltype`. The concept constrains the deduced type.

```cpp
// Listing 32.10: constrained auto in parameters, returns, and variables
#include <concepts>
#include <ranges>

void take_integer(std::integral auto x);          // abbreviated function template

std::integral auto make_index() { return 0u; }    // constrained return type

std::ranges::range auto first_n(auto&& r);         // constrained return (must be a range)

std::integral auto n = compute();                  // constrained variable: fails to compile
                                                   // if compute() returns a non-integral
```

Each `Concept auto` in a parameter list introduces an **independent** template parameter. `void f(std::integral auto a, std::integral auto b)` accepts `f(1, 2L)` with two different types; if you need them identical, use a named template parameter `template<std::integral T> void f(T a, T b)`.

---

## 32.7 Constraint-Based Overloading vs. SFINAE

Concepts replace the dominant historical use of SFINAE: selecting among overloads by type properties. The constrained version is dramatically clearer.

```cpp
// Listing 32.11: overloading on concepts (left) replaces enable_if (right, shown in comments)
#include <concepts>
#include <iostream>

void describe(std::integral auto x) {
    std::cout << "integer: " << x << '\n';
}
void describe(std::floating_point auto x) {
    std::cout << "float: " << x << '\n';
}

// Pre-C++20 equivalent required two enable_if-gated templates:
//   template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0> void describe(T);
//   template<class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0> void describe(T);

int main() {
    describe(42);     // integer
    describe(3.14);   // float
}
```

When multiple constrained overloads are viable, the compiler selects the **most constrained** one via subsumption (Chapter 33). The key advantages over SFINAE: the constraints are named and reusable, the overloads are not mutually-exclusion-engineered by hand, and an unsatisfied constraint is a clean "no viable overload" rather than a substitution-failure cascade.

---

## 32.8 Diagnostics: The Real Payoff

The most tangible everyday benefit is the error message. Calling a constrained function with the wrong type produces a diagnostic that names the **unsatisfied concept** and the **specific requirement** that failed:

```cpp
// Listing 32.12: a constraint violation produces a localized, readable error
#include <concepts>
#include <list>
#include <ranges>

void needs_random_access(std::ranges::random_access_range auto&& r);

int main() {
    std::list<int> lst{1, 2, 3};
    needs_random_access(lst);
    // error: constraints not satisfied
    //   note: the concept 'random_access_range<std::list<int>&>' was not satisfied
    //   note: 'std::list<int>::iterator' does not satisfy 'random_access_iterator'
    // ^ one screen, points at main(), names the exact failing requirement.
}
```

This is the difference between a two-minute fix and a two-hour spelunk. In template-heavy low-latency libraries, where a single mis-typed call can otherwise bury you in instantiation noise, constraining your public template interfaces is among the highest-value, lowest-cost things you can do.

---

## 32.9 Professional Insights

**Constrain every public template interface.** The cost is one concept name; the benefit is a call-site diagnostic instead of an instantiation-depth explosion, plus a self-documenting contract. For a library others build against, an unconstrained template parameter is now a code smell — it defers all type-checking to the worst possible place. Start at your API boundaries and work inward.

**Prefer named concepts to inline requires-expressions in interfaces.** An inline `requires(...)` on a function works, but a named concept (`template<class T> concept Sortable = ...;`) is reusable, appears in diagnostics by name, and participates in subsumption so overloads order correctly. Reserve bare requires-expressions for one-off internal constraints.

**Know the simple-vs-nested requirement trap cold.** Writing `sizeof(T) <= 16;` inside a requires-expression checks *validity*, not *truth* — it is almost always a silent bug. The predicate form is `requires sizeof(T) <= 16;`. This single distinction is the most common concepts mistake in production code; internalize that compound/simple requirements test "does it compile" while nested requirements test "is it true."

**Use compound requirements to constrain results, not just calls.** `{ expr } -> std::convertible_to<U>;` is far stronger than the bare `expr;` simple requirement, because it pins down what the expression *yields*. When you care that `container.size()` returns something `size_t`-like, or that `*it` is convertible to your value type, the compound form is what actually enforces it.

**Remember there is no runtime cost and no ABI footprint.** Concepts are pure compile-time predicates — they generate no code, occupy no space, and add no indirection. This is precisely why they belong in hot-path template libraries: they make the code safer and the diagnostics humane while leaving the emitted machine code identical to the unconstrained version.
