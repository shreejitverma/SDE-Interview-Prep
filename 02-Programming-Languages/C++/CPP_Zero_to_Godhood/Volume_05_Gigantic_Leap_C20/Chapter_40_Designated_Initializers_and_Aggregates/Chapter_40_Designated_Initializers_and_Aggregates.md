# Chapter 40: Designated Initializers and Aggregate Refinements

> *C++20 finally brings designated initializers — the `{.x = 1, .y = 2}` syntax C programmers have had since C99 — to C++, along with quieter but consequential refinements to what counts as an aggregate and how aggregates may be initialized. This chapter covers the designated-initializer syntax and its deliberately strict rules, the new ability to range-`for` with an init-statement, array-size deduction in `new`-expressions, and the aggregate model that underpins all of it.*

Designated initializers make struct construction self-documenting: the field names appear at the call site, so `Config{.timeout = 30, .retries = 3}` cannot silently transpose two `int` arguments the way `Config{30, 3}` can. But C++'s version is intentionally more restrictive than C's — designators must appear in declaration order, cannot be nested in the same brace pair, and cannot be mixed with positional initializers. Understanding those rules, and the aggregate concept they rest on, is what makes this feature safe to rely on.

---

## Table of Contents

- [40.1 Aggregates: The Foundation](#401-aggregates-the-foundation)
- [40.2 Designated Initializers](#402-designated-initializers)
- [40.3 The Rules: Order, No Mixing, No Gaps Reordered](#403-the-rules-order-no-mixing-no-gaps-reordered)
- [40.4 Designated Initializers vs C99](#404-designated-initializers-vs-c99)
- [40.5 Init-Statements in Range-Based for](#405-init-statements-in-range-based-for)
- [40.6 Array Size Deduction in new-Expressions](#406-array-size-deduction-in-new-expressions)
- [40.7 Aggregate Refinements in C++20](#407-aggregate-refinements-in-c20)
- [40.8 Professional Insights](#408-professional-insights)

---

## 40.1 Aggregates: The Foundation

Designated initializers apply only to **aggregates**, so the feature begins with that definition. In C++20 a class type is an aggregate when it has: no user-*declared* or inherited constructors, no private or protected non-static data members, no virtual functions, and no virtual/private/protected base classes. Aggregates can be initialized member-by-member with braces, bypassing constructors entirely.

```cpp
// Listing 40.1: aggregates — what qualifies
struct Point   { int x; int y; };              // aggregate
struct Config  { int timeout; int retries; bool verbose; }; // aggregate

struct NotAgg  {
    NotAgg(int);                                // user-declared ctor -> NOT aggregate
    int v;
};

Point  p{1, 2};                                // positional aggregate init (always legal)
Config c{30, 3, true};
```

The aggregate rules matter because designated initialization is *only* available for these types — adding a single user-declared constructor disqualifies a struct and makes `{.field = ...}` a compile error. This is why plain data-holder structs ("POD-like" config and message types) are the natural home for the feature.

---

## 40.2 Designated Initializers

A **designated initializer** names the member being initialized with a leading `.member =`, making brace initialization explicit and order-documenting.

```cpp
// Listing 40.2: designated initializers name each field at the call site
struct Point { int x; int y; };

Point p{.x = 1, .y = 2};          // explicit, self-documenting

struct Config { int timeout; int retries; bool verbose; };

Config c{.timeout = 30, .retries = 3, .verbose = true};

// Omitted members are value-initialized (zeroed for scalars):
Config d{.timeout = 30};          // retries = 0, verbose = false
```

The win is readability and safety at the call site. `Config{30, 3, true}` requires the reader to know the field order; `Config{.timeout = 30, .retries = 3, .verbose = true}` states it. Any member you omit is **value-initialized** — scalars become zero, class members are default-constructed — so you initialize only the fields you care about and the rest take well-defined defaults.

---

## 40.3 The Rules: Order, No Mixing, No Gaps Reordered

C++'s designated initializers are deliberately constrained. Three rules catch most mistakes:

```cpp
// Listing 40.3: what is and isn't allowed
struct S { int a; int b; int c; };

S ok    {.a = 1, .b = 2, .c = 3};   // OK: declaration order
S skip  {.a = 1, .c = 3};           // OK: skip b (b is value-initialized to 0)

// S bad1  {.b = 2, .a = 1};        // ERROR: out of declaration order
// S bad2  {.a = 1, 2};             // ERROR: cannot mix designated + positional
// S bad3  {.a = 1, .a = 2};        // ERROR: cannot designate the same member twice
```

1. **Declaration order is mandatory.** Designators must appear in the same order as the members are declared. You may *skip* members, but you may not *reorder* them — unlike C, where any order is legal.
2. **No mixing with positional initializers.** A brace list is either all-designated or all-positional, never a blend.
3. **Each member at most once.** No repeated designators.

These restrictions exist so that the order of side effects in the initializer matches the order of member construction — C++ guarantees members are initialized in declaration order, and allowing out-of-order designators would let the written order diverge from the evaluation order. The strictness is a feature, not an oversight.

---

## 40.4 Designated Initializers vs C99

Because the syntax is borrowed from C99, the differences are a common source of porting surprises for engineers moving C code into C++.

| Capability | C99 | C++20 |
|------------|-----|-------|
| `.field = value` syntax | ✅ | ✅ |
| Out-of-order designators | ✅ allowed | ❌ must be declaration order |
| Mixed designated + positional | ✅ allowed | ❌ forbidden |
| Array designators `[3] = x` | ✅ allowed | ❌ not supported |
| Nested designators `.a.b = x` | ✅ allowed | ❌ one level (use nested braces) |

```cpp
// Listing 40.4: nested aggregates use nested braces, not chained designators
struct Inner { int u; int v; };
struct Outer { Inner in; int w; };

Outer o{.in = {.u = 1, .v = 2}, .w = 3};   // C++20: nest the braces
// Outer o{.in.u = 1, .in.v = 2, .w = 3};  // ERROR in C++20 (legal in C99)
```

The practical rule when porting: C++ accepts the *common* subset (in-order, non-array, non-chained designators), so disciplined C code usually compiles, but C code exploiting out-of-order or array designators must be rewritten.

---

## 40.5 Init-Statements in Range-Based for

C++20 extends the range-based `for` with an **init-statement**, mirroring the init-statements already allowed in `if` and `switch` (C++17). It lets you declare and scope a variable alongside the range without leaking it into the surrounding scope.

```cpp
// Listing 40.5: init-statement in a range-based for
#include <vector>

std::vector<int>& getData();   // returns a reference into some store

void process() {
    // Bind the range to a named lvalue AND keep it alive for the loop's duration,
    // all scoped to the loop:
    for (auto& data = getData(); auto& x : data) {
        // use x; 'data' is visible here but not after the loop
    }
}
```

The canonical use is the **dangling-range fix**: `for (auto& x : getObj().items())` can dangle if `getObj()` returns a temporary, because the temporary's lifetime does not extend across the loop. The init-statement form `for (auto obj = getObj(); auto& x : obj.items())` binds the temporary to a named variable whose lifetime spans the loop. It also neatly scopes an index or counter to the loop: `for (int i = 0; auto& x : v)`.

---

## 40.6 Array Size Deduction in new-Expressions

C++20 allows the array bound to be **deduced** from the initializer in a `new`-expression, removing a redundant count that could drift out of sync with the braced list.

```cpp
// Listing 40.6: array size deduced in a new-expression
int*  p = new int[]{1, 2, 3};          // size 3 deduced from the initializer
char* s = new char[]{'a', 'b', '\0'};  // size 3 deduced

// Previously required the explicit (and error-prone) count:
// int* q = new int[3]{1, 2, 3};
delete[] p;
delete[] s;
```

This parallels the long-standing rule for non-`new` array declarations (`int a[] = {1,2,3};`). The benefit is the same: the count is computed from the data, so adding or removing an initializer element cannot leave a stale, mismatched size — a small but real safety improvement for dynamically allocated arrays.

---

## 40.7 Aggregate Refinements in C++20

Beyond designated initializers, C++20 refines the aggregate model itself in two further ways worth knowing:

- **Aggregates may have user-declared but not user-provided special members in some cases relaxed** — more precisely, C++20 tightened the rule so that a class with a *user-declared* constructor (even `= default`/`= delete`) is **no longer** an aggregate. This closed a C++17 loophole where `struct X { X() = default; int a; };` was an aggregate and could be brace-initialized around its own defaulted constructor, which was surprising. In C++20, declaring any constructor — defaulted or deleted — makes the type a non-aggregate.
- **Parenthesized aggregate initialization** (a related C++20 feature) lets aggregates be initialized with parentheses, `T(a, b)`, not just braces, which makes `std::make_unique<Aggregate>(1, 2)` and `emplace`-style construction work for aggregates that previously rejected parens.

```cpp
// Listing 40.7: the C++20 aggregate tightening and parenthesized init
struct A { A() = default; int x; };   // C++20: NOT an aggregate (declared ctor)
// A a{42};                           // ERROR in C++20 (was OK in C++17)

struct Point { int x; int y; };
Point p1{1, 2};                       // brace init
Point p2(1, 2);                       // C++20: parenthesized aggregate init
auto up = std::make_unique<Point>(1, 2);  // now works for aggregates
```

The net effect is that aggregates behave more predictably: declaring a constructor opts you fully out of aggregate-ness (no more half-aggregate surprises), and the brace/paren asymmetry that blocked `make_unique` for plain structs is gone.

---

## 40.8 Professional Insights

**Use designated initializers for config and message structs — they prevent argument transposition.** The highest-value use is wide structs of same-typed fields (timeouts, flags, counts) where positional initialization silently accepts a wrong order. `Order{.price = p, .qty = q}` makes a transposition a visible diff; `Order{q, p}` compiles and trades wrong. In domains like HFT where such a bug is catastrophic, prefer designated initializers at every aggregate construction site.

**Remember the feature requires an aggregate — adding a constructor silently disables it.** A struct that gains a single user-declared constructor (even `= default`) stops being an aggregate in C++20, and every `{.field = ...}` site for it becomes a compile error. Keep data-holder structs constructor-free if you rely on designated initialization, and be aware that "just adding one convenience constructor" is a breaking change for all designated call sites.

**Lean on the init-statement range-for to kill dangling-range bugs.** `for (auto&& obj = make_temp(); auto& x : obj.items())` is the idiomatic fix for iterating over members of a temporary. It binds the temporary to a named variable whose lifetime covers the loop, eliminating one of the most common and hard-to-spot lifetime bugs in range-based iteration.

**Treat C99 designated-init habits as non-portable into C++.** Out-of-order designators, array designators (`[2] = x`), and chained designators (`.a.b = x`) are all legal C99 and all rejected by C++20. When porting C, expect to rewrite those into in-order, nested-brace form. Conversely, write C++ designated initializers in declaration order from the start so the code stays valid if shared logic moves between the languages.

**Prefer deduced array bounds in `new[]` to keep counts honest.** `new int[]{...}` computes the size from the initializer, so the count cannot drift out of sync with the data when elements are added or removed. Reserve explicit `new T[n]` for the case where the size genuinely is a runtime value independent of any initializer list.
