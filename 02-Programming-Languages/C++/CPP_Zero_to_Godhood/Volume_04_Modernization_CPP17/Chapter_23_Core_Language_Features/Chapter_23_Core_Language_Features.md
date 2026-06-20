# Chapter 23: Core Language Features

> *C++17 is the "Modernization" release. Its core-language changes are not a new paradigm but a systematic removal of ceremony: decompose aggregates in one line, branch at compile time without SFINAE, scope variables to the condition that uses them, and stop paying for copies the standard now forbids. This chapter covers every core-language change C++17 made — from the headline ergonomic features down to the literal syntax, evaluation-order guarantees, and the dead syntax the committee finally deleted.*

The through-line of C++17's core language is **make the common case correct and cheap by default**. Structured bindings and init-statements cut boilerplate at every call site; `if constexpr` replaces tag dispatch and `enable_if` for the majority of compile-time branching; guaranteed copy elision turns a long-standing optimization into a language rule you can rely on for non-movable types; and a stricter evaluation order closes a category of subtle bugs. The chapter ends with the cleanups — new literal forms, attribute refinements, and the removal of `auto_ptr`, `register`, trigraphs, and dynamic exception specifications.

---

## Table of Contents

- [23.1 Structured Bindings](#231-structured-bindings)
- [23.2 Compile-Time Branching: `if constexpr`](#232-compile-time-branching-if-constexpr)
- [23.3 Init-Statements in `if` and `switch`](#233-init-statements-in-if-and-switch)
- [23.4 Inline Variables](#234-inline-variables)
- [23.5 Nested Namespace Definitions](#235-nested-namespace-definitions)
- [23.6 Guaranteed Copy Elision](#236-guaranteed-copy-elision)
- [23.7 constexpr Lambdas and Capturing `*this` by Value](#237-constexpr-lambdas-and-capturing-this-by-value)
- [23.8 Aggregate Initialization with Base Classes](#238-aggregate-initialization-with-base-classes)
- [23.9 `__has_include` and Conditional Compilation](#239-__has_include-and-conditional-compilation)
- [23.10 New Literal Forms: Hexadecimal Floats and `u8` Characters](#2310-new-literal-forms-hexadecimal-floats-and-u8-characters)
- [23.11 `noexcept` as Part of the Type System](#2311-noexcept-as-part-of-the-type-system)
- [23.12 Stricter Expression Evaluation Order](#2312-stricter-expression-evaluation-order)
- [23.13 Using-Declaration Pack Expansion and Attribute Namespaces](#2313-using-declaration-pack-expansion-and-attribute-namespaces)
- [23.14 Standard Attributes: `[[nodiscard]]`, `[[maybe_unused]]`, `[[fallthrough]]`](#2314-standard-attributes-nodiscard-maybe_unused-fallthrough)
- [23.15 Language Cleanups: Removed and Deprecated Features](#2315-language-cleanups-removed-and-deprecated-features)
- [23.16 Professional Insights](#2316-professional-insights)

---

## 23.1 Structured Bindings

**Structured bindings** decompose a tuple, pair, struct, or array into named variables in a single declaration. They replace the C++11 `std::tie` dance — which required pre-declaring every target variable and could not bind references cleanly — with `auto [a, b, c] = expr;`.

### 23.1.1 Unpacking Tuples and Pairs

```cpp
// Listing 23.1: structured bindings vs the C++11 std::tie idiom
#include <tuple>
#include <iostream>

std::tuple<int, double, std::string> get_data() {
    return {1, 3.14, "hello"};
}

int main() {
    // Old way (C++11/14): pre-declare, then assign
    // int i; double d; std::string s;
    // std::tie(i, d, s) = get_data();

    // C++17 structured binding: declare and decompose in one statement
    auto [id, val, name] = get_data();
    std::cout << id << ", " << val << ", " << name << "\n";
}
```

### 23.1.2 Unpacking Structs and Arrays

Any class type whose non-static data members are all public (an aggregate-like type) can be decomposed positionally, as can a built-in array:

```cpp
// Listing 23.2: decomposing a struct and an array
struct Point { int x, y; };

Point p = {10, 20};
auto [px, py] = p;          // px = 10, py = 20

int arr[3] = {1, 2, 3};
auto [a, b, c] = arr;       // a=1, b=2, c=3
```

### 23.1.3 Reference and `const` Qualifiers

The `auto` in a structured binding carries the same qualifier rules as ordinary `auto`. Binding by reference lets you mutate the source; `const auto&` gives cheap read-only views:

```cpp
// Listing 23.3: reference and const-reference bindings
std::pair<int, int> p = {1, 2};

auto& [refX, refY] = p;       // references into p.first / p.second
refX = 10;                    // modifies p.first

const auto& [cRefX, cRefY] = p;   // const references — no copy, no mutation
```

The canonical use is iterating an associative container without naming `std::pair`:

```cpp
// Listing 23.4: structured bindings in a range-for over a map
for (const auto& [key, value] : my_map) {
    std::cout << key << " => " << value << "\n";
}
```

> **Note:** a structured binding introduces *names for the members*, not independent objects — the underlying object is a hidden compiler-generated entity. Binding by value copies that entity once; binding by reference copies nothing.

---

## 23.2 Compile-Time Branching: `if constexpr`

**`if constexpr`** evaluates its condition at compile time and **discards the not-taken branch** before instantiation. The discarded branch is not type-checked against the current template arguments, so each branch may contain code that would be ill-formed for the other case. This single feature replaces the bulk of C++11/14 tag dispatch and `std::enable_if` overload sets.

```cpp
// Listing 23.5: one function body, two compile-time-selected behaviors
#include <type_traits>

template <typename T>
auto get_value(T t) {
    if constexpr (std::is_pointer_v<T>) {
        return *t;   // instantiated ONLY when T is a pointer
    } else {
        return t;    // instantiated ONLY when T is not a pointer
    }
}

int main() {
    int  x   = 5;
    int* ptr = &x;
    get_value(x);     // returns int  (5)
    get_value(ptr);   // returns int  (dereferenced)
}
```

The key contrast with a runtime `if`: with a plain `if`, *both* branches must compile for every instantiation, so `*t` would be ill-formed when `T = int`. With `if constexpr`, the false branch is discarded and never type-checked. This makes `if constexpr` the primary tool for writing one generic function that adapts its body to trait queries — recursion termination in variadic templates, dispatching on `std::is_same_v`, or selecting an algorithm by iterator category.

---

## 23.3 Init-Statements in `if` and `switch`

C++17 allows an **initializer statement** before the condition in `if` and `switch`: `if (init; condition)`. The declared variable is scoped to the entire `if`/`else` (or `switch`) construct and destroyed at its end — tightening scope and eliminating an extra enclosing block.

```cpp
// Listing 23.6: init-statement scopes the variable to the branch
// Old way — an extra block to bound the lifetime of 'it':
{
    auto it = map.find(key);
    if (it != map.end()) { /* use it */ }
}   // 'it' leaks into this artificial scope

// C++17 — 'it' lives exactly as long as the if/else:
if (auto it = map.find(key); it != map.end()) {
    std::cout << it->second;
}   // 'it' destroyed here

// switch with an init-statement:
switch (auto status = get_status(); status) {
    case OK:    break;
    case ERROR: break;
}
```

Beyond brevity, this is a correctness and locking idiom: a lock guard or transaction object declared in the init-statement is held for exactly the guarded region. It pairs naturally with structured bindings — `if (auto [it, ok] = m.try_emplace(k, v); ok) { ... }`.

---

## 23.4 Inline Variables

Before C++17, a variable defined in a header and included in multiple translation units violated the One Definition Rule, forcing the `static`-member-plus-out-of-line-definition workaround. **`inline` variables** let the linker merge identical definitions, so a single definition can live in a header.

```cpp
// Listing 23.7: header-only global and in-class static initialization
#pragma once

// C++14: a linker error if this header is included in 2+ .cpp files
// int global_config = 5;

// C++17: safe — the linker merges all definitions into one
inline int global_config = 5;

struct MyClass {
    // A static data member initialized in-class, no .cpp definition needed:
    static inline double tolerance = 0.001;
};
```

This is what makes header-only libraries practical for stateful globals and is the mechanism behind `constexpr static` data members becoming usable without a separate definition. For systems code it removes a class of fragile, easy-to-forget out-of-line definitions.

---

## 23.5 Nested Namespace Definitions

C++17 permits the compact `namespace A::B::C { }` form for opening a deeply nested namespace, replacing the staircase of nested braces.

```cpp
// Listing 23.8: compact nested namespace definition
// Old:
namespace A { namespace B { namespace C {
    // ...
} } }

// C++17:
namespace A::B::C {
    // ...
}
```

Purely syntactic, but it removes indentation noise from the library code that lives several namespaces deep — a real readability gain in large codebases organized by `project::module::detail`.

---

## 23.6 Guaranteed Copy Elision

In C++14, eliding the copy/move when returning or initializing from a **prvalue** (a pure rvalue temporary) was a permitted *optimization* — the copy/move constructor still had to exist and be accessible. **C++17 makes this elision mandatory** by redefining a prvalue as an initializer for an object rather than a temporary that must be materialized: the object is constructed directly in its final location.

```cpp
// Listing 23.9: guaranteed elision lets you return a non-movable type by value
struct NonMovable {
    NonMovable() = default;
    NonMovable(const NonMovable&) = delete;   // no copy
    NonMovable(NonMovable&&)      = delete;    // no move
};

NonMovable make() {
    return NonMovable{};   // C++17: constructed directly in the caller's storage —
                           // NO copy or move is even considered. Ill-formed in C++14.
}

int main() {
    NonMovable nm = make();   // also direct-constructed; no copy/move required
}
```

Two consequences matter in practice. First, factory functions can return types that are deliberately non-copyable and non-movable (locks, `std::atomic` wrappers, scope guards) **by value** — previously impossible. Second, the optimization is now a guarantee you can reason about for performance: prvalue return and prvalue initialization never construct an intermediate object. (Elision of a *named* local return value — NRVO — remains a non-guaranteed optimization, because a named object is an lvalue, not a prvalue.)

---

## 23.7 constexpr Lambdas and Capturing `*this` by Value

C++17 sharpens lambdas in two independent ways.

**constexpr lambdas:** a lambda whose body satisfies the `constexpr` requirements is implicitly `constexpr`, so its `operator()` can be evaluated at compile time and the closure used in constant expressions.

```cpp
// Listing 23.10: a lambda usable in a constant expression
constexpr auto square = [](int n) { return n * n; };
static_assert(square(5) == 25, "evaluated at compile time");

constexpr int table_size = square(4);   // 16, computed at compile time
int buffer[table_size];
```

**Capturing `*this` by value:** C++11/14 let a lambda capture `this` (a pointer), which dangles if the closure outlives the object. C++17 adds `[*this]`, copying the entire object into the closure so member access is safe after the original is gone — essential for lambdas posted to thread pools or stored as continuations.

```cpp
// Listing 23.11: [*this] copies the object into the closure
struct Worker {
    int id = 7;
    auto make_task() {
        // [*this] — the closure owns a COPY of *this; safe even if Worker dies.
        return [*this] { return id * 2; };
    }
};
```

`[this]` (pointer capture) remains correct and cheaper when the object is guaranteed to outlive the closure; `[*this]` is the safe choice for deferred or asynchronous execution.

---

## 23.8 Aggregate Initialization with Base Classes

C++17 extends the definition of an **aggregate** to include classes with public base classes (provided the bases are non-virtual and the derived class adds no user-declared constructors). Such a type can be brace-initialized, with the base subobjects initialized by nested braces in declaration order, before the derived members.

```cpp
// Listing 23.12: brace-initializing an aggregate with a base class
struct Base { int a; int b; };

struct Derived : Base {     // C++17: still an aggregate
    int c;
};

// Initialize the Base subobject, then the Derived member:
Derived d{{1, 2}, 3};       // Base{a=1, b=2}, c=3
// Braces may also be elided:
Derived e{1, 2, 3};         // same result
```

This removes the need to hand-write a forwarding constructor purely to initialize an inherited POD base — common in tag-augmented data structures and CRTP value types in systems code.

---

## 23.9 `__has_include` and Conditional Compilation

`__has_include(<header>)` is a preprocessor expression that evaluates to `1` if the named header can be found and `0` otherwise. It lets code adapt to the availability of a header — a standard one that may not yet be implemented, or an optional dependency — without a build-system probe.

```cpp
// Listing 23.13: feature-detecting a header at preprocessing time
#if __has_include(<optional>)
#  include <optional>
#  define HAS_OPTIONAL 1
#elif __has_include(<experimental/optional>)
#  include <experimental/optional>
#  define HAS_OPTIONAL 1
#else
#  define HAS_OPTIONAL 0
#endif
```

This is the portable mechanism behind graceful fallback across compiler/standard-library versions, and it composes with the `__cpp_*` feature-test macros standardized alongside it in C++17 for detecting individual language and library features.

---

## 23.10 New Literal Forms: Hexadecimal Floats and `u8` Characters

**Hexadecimal floating-point literals** (`0x1.8p3`) specify a floating value by its exact binary representation: a hex mantissa and a binary exponent after `p`. They express the precise bit pattern with no decimal-to-binary rounding — critical for reproducible numeric constants and unit tests of floating-point code.

```cpp
// Listing 23.14: hex-float literals express exact binary values
double a = 0x1.8p3;     // (1 + 8/16) * 2^3 = 1.5 * 8 = 12.0
double b = 0x1p-4;      // 2^-4 = 0.0625, exactly representable
```

**`u8` character literals** (`u8'a'`) denote a single UTF-8 code unit of type `char`, completing the `u8` family that previously existed only for string literals.

```cpp
// Listing 23.15: a u8 character literal
char c = u8'A';   // UTF-8 code unit, type char
```

For HFT and systems code, hex-floats are the reliable way to pin a floating constant to an exact value across platforms and to read a value straight from a hardware/protocol spec given in hex.

---

## 23.11 `noexcept` as Part of the Type System

In C++17, **`noexcept` becomes part of a function's type**, not merely a property the optimizer may consult. A pointer to a `noexcept` function and a pointer to a potentially-throwing function are now distinct, incompatible types, and overload resolution and template deduction can distinguish them.

```cpp
// Listing 23.16: noexcept is now type-significant
void may_throw();
void never_throws() noexcept;

void (*p1)()          = may_throw;     // ok
void (*p2)() noexcept = never_throws;  // ok
// void (*p3)() noexcept = may_throw;  // ERROR in C++17: drops noexcept

template <typename F>
void run(F) noexcept;                  // can detect/forward noexcept-ness
```

The practical effect is stronger guarantees: an API that requires a non-throwing callback (a destructor helper, a swap, a signal-safe handler) can now *enforce* it in the type system, and a `noexcept` function pointer can be safely used where exception freedom is a precondition. A `noexcept(true)` pointer implicitly converts to a potentially-throwing pointer, but not the reverse.

---

## 23.12 Stricter Expression Evaluation Order

C++17 **defines the evaluation order** for several expression forms that were previously unsequenced, eliminating a class of portability bugs and undefined behavior.

The now-guaranteed orderings include:

- In `a.b`, `a->b`, `a[b]`, `a << b`, `a >> b`, and assignment `a = b`, the **right-hand operand is sequenced before the left-hand operand** for the shifts and assignment; for postfix forms the object expression is evaluated first.
- In a function call, each argument's evaluation is **indeterminately sequenced** but no longer interleaved — one argument is fully evaluated before another begins (though their relative order is still unspecified).

```cpp
// Listing 23.17: chained calls and string operations are now well-defined
std::string s = "x";
// C++14: order of the two operator+= side effects was unspecified.
// C++17: left-to-right; the result is well-defined.
s = s + s[0] + s[1];

// Member-call chaining (e.g. fluent builders, std::cout << f() << g())
// now evaluates the stream/object before the inserted operands as specified.
```

This makes idiomatic fluent interfaces, `map[k] = f()`-style updates, and chained stream insertions behave consistently across compilers. It does **not** fully order all function-argument evaluation — writing code that depends on argument order is still unportable — but it removes the most common surprises.

---

## 23.13 Using-Declaration Pack Expansion and Attribute Namespaces

**Using-declaration pack expansion** lets a single `using` introduce names from a parameter pack of base classes — the enabling idiom for the "overloaded lambda" visitor used with `std::variant`.

```cpp
// Listing 23.18: pack expansion in a using-declaration (the overload set idiom)
template <typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;    // C++17: expand each base's operator()
};
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;  // deduction guide

// Used to build an inline visitor:
std::visit(overloaded{
    [](int i)         { /* handle int */ },
    [](const std::string& s) { /* handle string */ }
}, my_variant);
```

C++17 also regularizes **attribute syntax**: attributes may be grouped under a namespace with `using`, and unknown attributes are ignored rather than rejected, improving forward compatibility.

```cpp
// Listing 23.19: attribute-namespace using-directive
[[using gnu: const, always_inline]] int fast();   // == [[gnu::const, gnu::always_inline]]
```

---

## 23.14 Standard Attributes: `[[nodiscard]]`, `[[maybe_unused]]`, `[[fallthrough]]`

C++17 standardizes three attributes that turn previously compiler-specific warnings into portable, intent-expressing annotations.

**`[[nodiscard]]`** — warns if the return value is ignored. Apply it to functions whose result must be checked (error codes, `[[nodiscard]] bool empty()`, allocation results, RAII guards):

```cpp
// Listing 23.20: [[nodiscard]] flags ignored results
[[nodiscard]] int calculate_important_value();
calculate_important_value();   // compiler warning: result discarded
```

**`[[maybe_unused]]`** — suppresses "unused entity" warnings for a variable, parameter, or function used only in some configurations (e.g. only inside `assert`):

```cpp
// Listing 23.21: [[maybe_unused]] for conditionally-used entities
[[maybe_unused]] int debug_id = compute_id();   // used only in asserts/logging
```

**`[[fallthrough]]`** — marks an intentional fall-through between `switch` cases, silencing the implicit-fallthrough warning:

```cpp
// Listing 23.22: [[fallthrough]] documents intentional case fall-through
switch (device_state) {
    case State::INIT:
        initialize_device();
        [[fallthrough]];        // intentional — no break here
    case State::RUNNING:
        run_process();
        break;
}
```

`[[nodiscard]]` in particular is high-value in systems APIs: it converts "caller forgot to check the error" from a runtime incident into a compile-time warning.

---

## 23.15 Language Cleanups: Removed and Deprecated Features

C++17 deletes long-deprecated syntax, reducing the language surface. Code relying on these must be modernized:

| Removed / changed | Replacement |
|-------------------|-------------|
| `std::auto_ptr` (removed) | `std::unique_ptr` |
| `register` keyword (removed as a storage specifier) | nothing — the compiler allocates registers |
| Trigraphs (`??=`, `??/`, …) removed | the literal characters, or `\` line continuation |
| `operator++` on `bool` (removed) | explicit `b = true;` / arithmetic on an integer |
| Dynamic exception specifications `throw(...)` (removed; `throw()` deprecated) | `noexcept` / `noexcept(false)` |

```cpp
// Listing 23.23: the C++17 replacements
std::unique_ptr<Widget> w = std::make_unique<Widget>();   // not auto_ptr
void f() noexcept;                                         // not throw()
```

The removal of dynamic exception specifications is the most consequential: `throw(TypeList)` is gone entirely, and `noexcept` (Section 23.11) is the only supported exception-specification mechanism. `throw()` survives as a deprecated synonym for `noexcept(true)` and should be replaced.

---

## 23.16 Professional Insights

**Make structured bindings and init-statements your default at every container/lookup site.** `if (auto [it, inserted] = m.try_emplace(k, v); inserted)` expresses intent, scopes the result precisely, and avoids both the `std::tie` pre-declaration and the artificial enclosing block. The pattern is so pervasive it should be the reflex spelling.

**Reach for `if constexpr` before SFINAE or tag dispatch.** For the large majority of "do X for pointers, Y otherwise" generic code, a single function with `if constexpr` is clearer, compiles faster, and produces better diagnostics than an overload set guarded by `enable_if`. Reserve the heavier metaprogramming machinery for cases `if constexpr` cannot express (e.g. constraining which overloads are *visible*).

**Rely on guaranteed copy elision in API design.** You can now return non-movable RAII types (locks, scope guards, `atomic` wrappers) by value from factory functions, and you can reason about prvalue return as allocation-free. Design factories around it instead of returning by `unique_ptr` purely to dodge a copy.

**Use `[*this]` for any lambda that outlives its enclosing object.** Pointer capture (`[this]`) dangles when a closure is posted to a thread pool or stored as a continuation; `[*this]` copies the object and is the safe default for deferred execution. Use `[this]` only when lifetime is provably contained.

**Enforce contracts in the type system where C++17 now lets you.** `[[nodiscard]]` on must-check results and `noexcept` in callback signatures move whole classes of caller mistakes from runtime to compile time — cheap, high-leverage hardening for systems and low-latency interfaces. And purge the removed features (`auto_ptr`, `throw(...)`) proactively; they will not survive a C++17 build.
