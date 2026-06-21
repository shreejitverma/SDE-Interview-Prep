# Chapter 74: Advanced Template Metaprogramming

Template metaprogramming (TMP) turns the C++ type system into a compile-time programming language: the compiler becomes an interpreter that runs your meta-program during translation and emits specialised machine code as its output. The central problem this chapter addresses is *abstraction without cost* — how to express generic, policy-driven, statically-dispatched designs that the optimiser can collapse to the same instructions a hand-written specialisation would produce. We cover the techniques, but more importantly the cost model (compile time, binary size, diagnostic quality) and the failure modes (instantiation blowup, ODR hazards, unreadable errors) that decide whether TMP is the right tool.

## Chapter Roadmap

- 74.1 The Evolution of TMP and Why It Exists
- 74.2 SFINAE: Substitution Failure Is Not An Error
- 74.3 Detection Idioms: `void_t`, `declval`, and Trait Construction
- 74.4 The Curiously Recurring Template Pattern (CRTP)
- 74.5 Policy-Based Design
- 74.6 Modern TMP with Concepts (C++20)
- 74.7 Variadic Templates, Fold Expressions, and Type Lists
- 74.8 The Cost Model: Compile Time, Binary Size, and Diagnostics
- 74.9 Correctness Hazards and Anti-Patterns
- 74.10 When *Not* to Use TMP

---

## 74.1 The Evolution of TMP and Why It Exists

**Template metaprogramming** is the art of computing with types and compile-time values so that the compiler generates specialised code on your behalf. The motivation is not cleverness for its own sake: it is the elimination of runtime dispatch. A `virtual` call costs an indirect branch the predictor may miss and a pointer chase that defeats inlining; a template specialisation resolved at compile time costs nothing — the call is inlined and the abstraction disappears.

The technique has improved markedly across standards, and the *right* idiom depends on which you target:

| Standard | Tools added | Practical effect |
|---|---|---|
| C++98 | Recursive `struct` instantiation, `enum` value hacks | Turing-complete but painful; values smuggled through `enum`/`static const` |
| C++11 | `constexpr`, `static_assert`, alias templates (`using`), variadic templates | Real compile-time functions; type lists become ergonomic |
| C++14 | Variable templates, relaxed `constexpr` (loops, locals) | Trait values as `name_v<T>`; readable meta-functions |
| C++17 | `if constexpr`, fold expressions, `std::void_t` | Branch on traits without specialisation; collapse parameter packs |
| C++20 | Concepts (`requires`), `consteval`, constrained `auto` | Constraints become first-class, named, and diagnosable |

> **Why this matters.** Each generation removed an entire category of boilerplate *and* improved diagnostics. Pre-C++11 TMP errors were multi-page template backtraces; C++20 concepts produce a single line naming the unsatisfied constraint. Choosing the newest idiom your toolchain supports is usually the correct engineering decision — not for elegance, but because compile time and diagnosability are real costs paid by every engineer who later touches the code.

---

## 74.2 SFINAE: Substitution Failure Is Not An Error

Before concepts, **SFINAE** was the only mechanism to constrain a template. The rule: when the compiler substitutes template arguments into a candidate's *immediate context* and that substitution produces an ill-formed type or expression, the candidate is silently removed from the overload set rather than causing a hard error. Only if *no* candidate survives do you get an error.

### 74.2.1 `std::enable_if`

`std::enable_if<Cond, T>` has a member `type` only when `Cond` is true. Referencing `::type` when the condition is false is a substitution failure, which prunes the overload.

```cpp
// Minimum standard: C++11. Portable.
#include <type_traits>
#include <iostream>

// Enabled only for integral types.
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
process(T t) {
    std::cout << "Integral: " << t << "\n";
}

// Enabled only for floating point.
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, void>::type
process(T t) {
    std::cout << "Float: " << t << "\n";
}
```
*Listing 74.1 — Overload selection by trait via `enable_if`.*

The two overloads are mutually exclusive: for any `T` exactly one has a valid return type. The other vanishes from the candidate set before ambiguity is even considered.

> **Why this matters / cost model.** SFINAE has *zero* runtime cost — it is purely an overload-resolution-time decision. Its cost is paid by the compiler: every candidate is substituted, and dense `enable_if` chains multiply the work the front-end does per call site. The deeper hazard is the phrase *immediate context*: a failure buried inside an instantiated function body is **not** a substitution failure — it is a hard error. Constraints must live in the signature (return type, a defaulted template parameter, or a parameter type), never in the body.

### 74.2.2 The three placements of `enable_if`

```cpp
// Min standard: C++11. Portable.
// (a) Return type — fails if T is not integral.
template <typename T>
std::enable_if_t<std::is_integral_v<T>, T> negate_a(T t) { return -t; }

// (b) Defaulted non-type template parameter — preferred; does not perturb the signature.
template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
T negate_b(T t) { return -t; }

// (c) Defaulted function parameter — rarely used; changes the function's arity ABI.
template <typename T>
T negate_c(T t, std::enable_if_t<std::is_integral_v<T>, int> = 0) { return -t; }
```
*Listing 74.2 — Three SFINAE placements. `enable_if_t`/`is_integral_v` require C++14/C++17 aliases.*

Placement (b) is the production default: it keeps the visible signature clean and avoids the subtle bug where two overloads differing only by their `enable_if` return type are still considered the *same* signature for ODR/redeclaration purposes.

---

## 74.3 Detection Idioms: `void_t`, `declval`, and Trait Construction

A common need is to ask "does type `T` have member `X`?" The **detection idiom** answers this with partial specialisation and `std::void_t`.

```cpp
// Min standard: C++17 (void_t). Portable.
#include <type_traits>
#include <utility>

template <typename T, typename = void>
struct has_print : std::false_type {};

template <typename T>
struct has_print<T, std::void_t<decltype(std::declval<T>().print())>>
    : std::true_type {};

// Usage:
// static_assert(has_print<MyClass>::value, "MyClass must have print()");
```
*Listing 74.3 — The detection idiom for a member function.*

How it works: the primary template defaults the second parameter to `void`. The partial specialisation is preferred *when* `std::void_t<...>` is well-formed — i.e. when `declval<T>().print()` is a valid expression — because `void_t` maps any valid type list to `void`, matching the `= void` default. If the expression is ill-formed, the specialisation is SFINAE'd away and the `false_type` primary remains.

- **`std::declval<T>()`** produces an unevaluated rvalue of `T` without requiring a constructor — essential because `decltype` never actually calls anything.
- **`std::void_t<Ts...>`** is `void` for any well-formed pack; its sole purpose is to convert "this expression compiles" into "this type exists."

> **Why this matters.** Detection lets a generic component *adapt* to a type's capabilities — call `reserve()` if the container has it, fall back otherwise — instead of demanding a fixed interface. The cost is compile-time only. In C++20 the same intent is expressed far more legibly with a `requires` expression (74.6); prefer that when available and reserve `void_t` for code that must build on older toolchains.

The generalised library form is `std::experimental::is_detected` (Library Fundamentals TS); many codebases ship a local `is_detected` to avoid hand-writing one trait per query.

---

## 74.4 The Curiously Recurring Template Pattern (CRTP)

**CRTP** is static polymorphism: a base class is parameterised on its own derived type, so the base can call into the derived class with the concrete type known at compile time.

```cpp
// Min standard: C++11. Portable.
#include <iostream>

template <typename Derived>
class Base {
public:
    void interface() {
        // Compile-time dispatch — resolved and typically inlined.
        static_cast<Derived*>(this)->implementation();
    }
};

class Concrete : public Base<Concrete> {
public:
    void implementation() { std::cout << "Concrete impl\n"; }
};
```
*Listing 74.4 — CRTP for zero-overhead static dispatch.*

> **Why this matters / cost model.** A `virtual` call costs (1) a load of the vptr, (2) a load of the function pointer from the vtable, (3) an indirect branch the predictor may mispredict (~15–20 cycle flush), and (4) a hard inlining barrier — the optimiser cannot see across the call. CRTP collapses all four to nothing: `static_cast` is a no-op, `implementation()` is a direct call, and the inliner sees straight through it, enabling constant propagation across the abstraction boundary. In hot paths this is the difference between a vectorised loop and a scalar one.

**The trade-offs CRTP buys you:**

| Aspect | `virtual` (dynamic) | CRTP (static) |
|---|---|---|
| Dispatch cost | Indirect branch + vtable load | Inlined direct call |
| Heterogeneous containers | `vector<Base*>` works | Not possible — each `Base<D>` is a distinct type |
| Binary size | One function body | One body *per derived type* (code bloat) |
| Runtime extensibility | Plugins/late binding | Closed set, fixed at compile time |

**Canonical uses:** mixins that inject operators (`operator!=` from `operator==`), the *enable_shared_from_this* pattern, expression templates, and any interface where the concrete set of types is known at compile time and dispatch is hot.

**Hazard:** `static_cast<Derived*>(this)` is undefined behaviour if the object is not actually a `Derived` — i.e. if someone writes `class Wrong : public Base<Concrete>`. CRTP relies on the discipline that the template argument *is* the inheriting class.

---

## 74.5 Policy-Based Design

**Policy-based design** (Alexandrescu) composes a class from orthogonal *policy* classes supplied as template parameters, each defining one axis of behaviour. The host inherits from its policies so their members fold into one interface.

```cpp
// Min standard: C++11. Portable.
#include <iostream>
#include <string>

struct ConsoleOutput {
    void print(const std::string& s) { std::cout << s << "\n"; }
};

struct EnglishLanguage {
    std::string message() { return "Hello, World!"; }
};

template <typename OutputPolicy, typename LanguagePolicy>
class HelloWorld : private OutputPolicy, private LanguagePolicy {
public:
    void run() {
        this->print(this->message()); // OutputPolicy::print, LanguagePolicy::message
    }
};

// HelloWorld<ConsoleOutput, EnglishLanguage> app; app.run();
```
*Listing 74.5 — Behaviour composed from orthogonal policies.*

> **Why this matters / trade-offs.** Policies turn an explosion of behavioural combinations (output × language × error-handling × threading) into independent, separately-testable units, with each combination resolved at compile time so dispatch is free. The cost is the same as all template composition: **code bloat** (one full instantiation per policy combination), **error-message complexity**, and a hard boundary against runtime configuration — a policy chosen by a config file at startup cannot be a template argument. Inheriting policies *privately* (as above) avoids leaking their interface and enables the empty base optimisation, so stateless policies add zero bytes to the object. Prefer policies over a forest of `if`/virtuals when the axes are genuinely orthogonal and fixed at compile time; otherwise use strategy objects.

---

## 74.6 Modern TMP with Concepts (C++20)

**Concepts** make constraints first-class: named predicates over types that participate in overload resolution and produce a single-line diagnostic when unsatisfied.

```cpp
// Min standard: C++20. Portable.
#include <concepts>

template <typename T>
concept Printable = requires(T t) {
    { t.print() } -> std::same_as<void>;
};

void process(Printable auto& obj) {   // constrained terse syntax
    obj.print();
}
```
*Listing 74.6 — A constraint that replaces both SFINAE and a detection trait.*

Concepts subsume the prior three sections:

- They replace `enable_if` for constraining overloads, and they **partially order** overloads by constraint strength (the more-constrained candidate wins, eliminating tie-break boilerplate).
- A `requires`-expression replaces the `void_t` detection idiom with readable syntax (`{ expr } -> Concept`).
- They constrain CRTP bases and policy parameters so misuse is reported at the *definition*, not deep inside an instantiation.

> **Why this matters.** The dominant cost of pre-C++20 TMP was *diagnosability*: a missing member surfaced as a 40-line backtrace pointing inside `std::sort`. A concept reports `constraint 'Printable<Widget>' not satisfied` at the call site. There is also a subtle correctness gain: concepts check the constraint *before* instantiation, so an unsatisfied requirement is a clean rejection rather than a hard error escaping the immediate context. Use concepts wherever your toolchain allows; fall back to SFINAE only for portability to pre-C++20 compilers.

---

## 74.7 Variadic Templates, Fold Expressions, and Type Lists

Variadic templates (C++11) accept arbitrary parameter packs; fold expressions (C++17) collapse a pack with a binary operator without recursion.

```cpp
// Min standard: C++17. Portable.
#include <iostream>

template <typename... Args>
auto sum(Args... args) {
    return (args + ...);          // unary right fold: a0 + (a1 + (a2 + ...))
}

template <typename... Args>
void print_all(const Args&... args) {
    ((std::cout << args << ' '), ...);   // fold over the comma operator
    std::cout << '\n';
}
```
*Listing 74.7 — Fold expressions replace recursive pack expansion.*

> **Why this matters / cost model.** Pre-C++17, processing a pack required a recursive template with a base case, generating one instantiation per pack length *per call arity* — a real compile-time and binary-size cost. Folds emit a single non-recursive expansion, dramatically cutting instantiation count. For genuine *type-level* computation (a `type_list`, `tuple` element access, metafunction maps), recursion is still common; minimise its depth, because instantiation depth is bounded (`-ftemplate-depth`) and quadratic memoisation pressure in the front-end is a frequent cause of slow builds.

---

## 74.8 The Cost Model: Compile Time, Binary Size, and Diagnostics

TMP moves work from runtime to compile time, but compile time is not free — it is paid by every engineer on every build.

| Cost axis | What drives it | Mitigation |
|---|---|---|
| Compile time | Number of distinct instantiations; template recursion depth; SFINAE candidate count | Reduce instantiation arity; prefer folds over recursion; `extern template` to suppress duplicate instantiation |
| Binary size | One code body per instantiation (CRTP, policies, variadics) | Type-erase cold paths; share non-dependent code in a non-template base |
| Diagnostics | SFINAE depth; unconstrained templates | Concepts / `static_assert` with messages at the boundary |
| Link time | Duplicate instantiations across TUs merged by the linker (COMDAT folding) | `extern template` declarations in headers, explicit instantiation in one TU |

> **Why this matters.** A header-only library that instantiates `std::variant`, deep `tuple`s, and recursive metafunctions per TU can dominate a project's build. The fix is to measure (`-ftime-trace` on Clang produces a flame graph of instantiation cost) and then cut the worst offenders — usually by reducing the number of *distinct* instantiations, since the compiler memoises identical ones but pays full price for each new combination.

---

## 74.9 Correctness Hazards and Anti-Patterns

- **Constraints outside the immediate context.** A trait check inside a function body is a hard error, not a SFINAE rejection. Keep constraints in signatures (or use concepts).
- **ODR violations from differing instantiations.** If two TUs see different definitions of a template (e.g. behind different macros), the linker silently picks one — undefined behaviour. Keep template definitions identical across all TUs.
- **`static_cast` in CRTP on the wrong type.** Undefined behaviour; rely on the discipline that the template argument is the deriving class.
- **Accidental recursion explosion.** Unbounded or wide recursive metafunctions blow `-ftemplate-depth` and build time; prefer folds and shallow recursion.
- **Over-constraining vs under-constraining.** An unconstrained template accepts anything and fails deep inside; over-eager `enable_if` can silently drop the overload you wanted, yielding "no matching function" with no hint why. Concepts with named constraints are the cure for both.

---

## 74.10 When *Not* to Use TMP

Reach for runtime polymorphism or plain functions instead of TMP when:

- The set of types or behaviours is **open** or chosen at runtime (plugins, config-driven dispatch) — templates are closed at compile time.
- The dispatch is **cold** — outside hot loops the indirect-branch cost of `virtual` is irrelevant, and dynamic dispatch keeps binary size and compile time down.
- The abstraction would multiply **binary size** across many instantiations without a measured runtime win.
- The team cannot absorb the **diagnostic and build-time** cost — TMP is a force multiplier in expert hands and a maintenance tax in others.

The mastery position is not "use TMP everywhere" but "use the compiler to erase cost on the hot path, and pay for flexibility with runtime dispatch everywhere else." The following chapters on the memory model, lock-free structures, and allocators repeatedly apply exactly this discipline.
