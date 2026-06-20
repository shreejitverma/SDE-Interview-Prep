# Chapter 41: consteval, constinit, and the constexpr Expansion

> *C++20 turns compile-time computation from a constrained niche into a first-class programming model. It adds two new keywords — `consteval` for functions that must run at compile time and `constinit` for variables that must be constant-initialized — and it massively widens what `constexpr` can do: virtual calls, `try`/`catch`, `dynamic_cast`, `typeid`, and even dynamic allocation are now legal in constant evaluation, which is what makes `constexpr std::vector` and `constexpr std::string` possible. This chapter covers all three and the boundary rules that govern when code runs at compile time versus runtime.*

The pre-C++20 `constexpr` world was a restricted dialect: no allocation, no virtual dispatch, no exceptions. C++20 demolishes most of those walls, so the same ordinary-looking C++ can now execute during compilation. With that power comes a need for precision about *when* evaluation happens — `consteval` forces compile time, `constinit` forces constant initialization without forcing const-ness, and `std::is_constant_evaluated()` lets one function body take different paths in each context. Getting these distinctions right is the difference between code that compiles to a baked-in constant and code that silently runs at startup.

---

## Table of Contents

- [41.1 A Quick Map of the Four constexpr-Family Specifiers](#411-a-quick-map-of-the-four-constexpr-family-specifiers)
- [41.2 consteval: Immediate Functions](#412-consteval-immediate-functions)
- [41.3 constinit: Guaranteed Constant Initialization](#413-constinit-guaranteed-constant-initialization)
- [41.4 The constexpr Expansion: What Became Legal](#414-the-constexpr-expansion-what-became-legal)
- [41.5 constexpr Dynamic Allocation and Transient Allocation](#415-constexpr-dynamic-allocation-and-transient-allocation)
- [41.6 constexpr Containers: vector and string](#416-constexpr-containers-vector-and-string)
- [41.7 constexpr Virtual Functions and Polymorphism](#417-constexpr-virtual-functions-and-polymorphism)
- [41.8 is_constant_evaluated: Branching on Context](#418-is_constant_evaluated-branching-on-context)
- [41.9 Professional Insights](#419-professional-insights)

---

## 41.1 A Quick Map of the Four constexpr-Family Specifiers

Four specifiers govern compile-time behavior, and they answer different questions. Confusing them is the most common source of compile-time bugs.

| Specifier | Question it answers | Forces compile-time? | Forces const? |
|-----------|---------------------|----------------------|---------------|
| `constexpr` (function) | *may* this run at compile time? | no — usable at runtime too | n/a |
| `constexpr` (variable) | is this a compile-time constant? | yes (initializer) | yes |
| `consteval` (function) | *must* this run at compile time? | **yes — always** | n/a |
| `constinit` (variable) | is this constant-*initialized*? | yes (initialization only) | **no — stays mutable** |

The two key contrasts: `consteval` is "`constexpr` that *must* evaluate at compile time" (a call that cannot be a constant expression is an error), and `constinit` is "guaranteed constant *initialization* without const-ness" — the variable is set up at compile time but remains modifiable at runtime.

---

## 41.2 consteval: Immediate Functions

A `consteval` function is an **immediate function**: every call to it must produce a constant expression, evaluated during compilation. Unlike `constexpr`, there is no runtime fallback — if the arguments are not constant expressions, the program is ill-formed.

```cpp
// Listing 41.1: consteval forces compile-time evaluation
consteval int sq(int x) { return x * x; }

constexpr int a = sq(5);     // OK: 25, computed at compile time
int n = 7;
// int b = sq(n);            // ERROR: n is not a constant expression

// constexpr would have allowed both — consteval forbids the runtime call entirely.
constexpr int csq(int x) { return x * x; }
int c = csq(n);              // OK with constexpr: runs at runtime
```

`consteval` is the right tool when a function *only makes sense* at compile time — building a lookup table, validating a string literal's format, computing a type-level constant, or factory functions that must not leak into the runtime binary. Because the call is guaranteed to vanish into a constant, a `consteval` function can do compile-time-only work (like consuming a `std::source_location`) without runtime cost. A useful consequence: an immediate function never appears in the compiled binary as a callable symbol — there is nothing to call.

---

## 41.3 constinit: Guaranteed Constant Initialization

`constinit` asserts that a variable with static or thread storage duration is **initialized with a constant expression**, eliminating the **static initialization order fiasco** for that variable — but it does *not* make the variable `const`. The variable is constant-*initialized* yet remains mutable.

```cpp
// Listing 41.2: constinit guarantees constant init but allows mutation
constinit int counter = 0;       // constant-initialized; still writable
// ... later, at runtime:
void bump() { ++counter; }       // OK: constinit is not const

// The guarantee is about *initialization*, caught at compile time:
int runtime_value();
// constinit int bad = runtime_value();  // ERROR: not constant-initialized

// Contrast:
const int kMax = 100;            // const: read-only, may be runtime-initialized
constexpr int kMin = 0;          // constexpr: const AND a constant expression
```

The problem `constinit` solves is real and subtle: globals across translation units are initialized in an unspecified order, so a global that depends on another may read it before it is set (the static init order fiasco). `constinit` forces the initialization to happen at compile time / load time as a constant, so it is ready before any dynamic initialization runs — without paying the cost of making the variable immutable, which matters for globals that must be written during the program (counters, caches, flags). It is especially valuable for `thread_local` variables, guaranteeing they avoid a runtime initialization guard on every access.

---

## 41.4 The constexpr Expansion: What Became Legal

C++20 enormously widened the set of operations permitted inside `constexpr` evaluation. Constructs that were hard errors before now work at compile time.

| Construct | Pre-C++20 | C++20 |
|-----------|-----------|-------|
| `try`/`catch` in a `constexpr` function | ❌ | ✅ (catch is inert at compile time) |
| `dynamic_cast` | ❌ | ✅ |
| `typeid` | ❌ | ✅ |
| Virtual function calls | ❌ | ✅ |
| `new`/`delete` (transient) | ❌ | ✅ (must free before eval ends) |
| Changing the active member of a union | ❌ | ✅ |
| `std::vector`, `std::string` operations | ❌ | ✅ |

```cpp
// Listing 41.3: try/catch is now allowed in constexpr (catch ignored at compile time)
#include <stdexcept>

constexpr int safe_div(int a, int b) {
    try {
        if (b == 0) throw std::runtime_error("div by zero");
        return a / b;
    } catch (...) {
        return 0;          // unreachable in a *valid* constant expression
    }
}

constexpr int r = safe_div(10, 2);   // 5, at compile time
```

The semantics of `try`/`catch` in constant evaluation: the `try` block is evaluated normally, but **throwing during constant evaluation makes the expression non-constant** — you cannot actually catch at compile time, so a `throw` that fires terminates the constant evaluation with an error. The feature exists mainly so that functions containing `try`/`catch` (perhaps for their runtime path) can *also* be used in constant expressions on the paths that do not throw.

---

## 41.5 constexpr Dynamic Allocation and Transient Allocation

The headline enabler is **constexpr dynamic allocation**: `new` and `delete` are allowed during constant evaluation, subject to one strict rule — **transient allocation**: any memory allocated during constant evaluation must be freed before the evaluation ends. Memory cannot "escape" a constant expression into the runtime.

```cpp
// Listing 41.4: transient allocation — allocate and free within the same evaluation
constexpr int sum_first_n(int n) {
    int* buf = new int[n];          // allocation during constant evaluation
    for (int i = 0; i < n; ++i) buf[i] = i + 1;
    int total = 0;
    for (int i = 0; i < n; ++i) total += buf[i];
    delete[] buf;                   // MUST free before the evaluation ends
    return total;
}

constexpr int s = sum_first_n(100);  // 5050, computed at compile time
```

The transient rule is what keeps the model sound: the compiler runs a little interpreter during constant evaluation, and any heap it hands out must be returned before it finishes, because there is no runtime heap to carry it into. This is precisely why `constexpr std::vector` works *inside* a `constexpr` function but a `constexpr std::vector` **variable** at namespace scope does not compile in C++20 — the variable would need its allocation to persist past evaluation, which the transient rule forbids (lifting that restriction is a later-standard topic).

---

## 41.6 constexpr Containers: vector and string

Because allocation and the necessary member functions are now `constexpr`, **`std::vector` and `std::string` are usable in constant evaluation** in C++20. This makes genuinely dynamic algorithms runnable at compile time.

```cpp
// Listing 41.5: std::vector and std::string at compile time
#include <vector>
#include <string>
#include <algorithm>

constexpr int count_evens_up_to(int n) {
    std::vector<int> v;                  // constexpr vector — fine inside the function
    for (int i = 0; i <= n; ++i) v.push_back(i);
    return static_cast<int>(
        std::count_if(v.begin(), v.end(), [](int x){ return x % 2 == 0; }));
}

constexpr bool starts_with_cpp(std::string s) {
    return s.size() >= 3 && s[0]=='C' && s[1]=='+' && s[2]=='+';
}

constexpr int evens = count_evens_up_to(10);          // 6, at compile time
constexpr bool ok   = starts_with_cpp(std::string("C++20"));  // true, at compile time
```

The critical caveat is the one from Section 41.5: the container must be **transient** — created and destroyed within the constant evaluation. You can compute *with* a `constexpr std::vector` and return a scalar or a fixed-size result, but you cannot declare a `constexpr std::vector` that survives to runtime in C++20. The idiom is "use the container to compute, return a `std::array` or a count."

---

## 41.7 constexpr Virtual Functions and Polymorphism

C++20 permits **virtual functions to be `constexpr`**, enabling runtime-style polymorphism to execute during constant evaluation. Virtual dispatch, `dynamic_cast`, and `typeid` all work in constant expressions.

```cpp
// Listing 41.6: compile-time polymorphism via constexpr virtual functions
struct Shape {
    constexpr virtual double area() const = 0;
    constexpr virtual ~Shape() = default;
};

struct Square : Shape {
    double side;
    constexpr Square(double s) : side(s) {}
    constexpr double area() const override { return side * side; }
};

struct Circle : Shape {
    double r;
    constexpr Circle(double radius) : r(radius) {}
    constexpr double area() const override { return 3.141592653589793 * r * r; }
};

constexpr double total_area() {
    Square sq{2.0};
    Circle ci{1.0};
    const Shape* shapes[] = {&sq, &ci};
    double sum = 0;
    for (const Shape* s : shapes) sum += s->area();   // virtual call at compile time
    return sum;
}

constexpr double t = total_area();   // 4 + pi, computed during compilation
```

This collapses a long-standing dichotomy where "compile-time" meant "no polymorphism." You can now write naturally polymorphic code and have it evaluated at compile time, useful for building constant tables of heterogeneous descriptors or validating polymorphic configurations during the build. As always, the objects involved must be transient within the evaluation.

---

## 41.8 is_constant_evaluated: Branching on Context

`std::is_constant_evaluated()` (from `<type_traits>`) lets a single function body **detect whether it is currently running in a constant-evaluation context** and choose a different implementation — a compile-time-friendly path versus a runtime-optimized one.

```cpp
// Listing 41.7: one function, two implementations by context
#include <type_traits>
#include <cmath>

constexpr double power(double base, int exp) {
    if (std::is_constant_evaluated()) {
        // Compile-time path: a simple loop the constant evaluator can run.
        double r = 1.0;
        for (int i = 0; i < exp; ++i) r *= base;
        return r;
    } else {
        // Runtime path: use the fast, possibly-intrinsic library function.
        return std::pow(base, static_cast<double>(exp));
    }
}

constexpr double ct = power(2.0, 10);   // 1024, compile-time loop
double rt = power(2.0, 10);             // runtime: calls std::pow
```

The classic use is exactly this: `std::pow` is not `constexpr`, so a `constexpr` function that wants compile-time evaluation provides a hand-rolled loop for the constant path while delegating to the optimized library routine at runtime. **Two traps**: (1) it must be called as a function with `()` — `if constexpr (std::is_constant_evaluated())` is a bug, because in a `constexpr if` the trait is *always* true (it is being evaluated in a manifestly-constant context), defeating the purpose; (2) C++23 adds `if consteval` as a clearer, less error-prone spelling, but in C++20 you use the function form inside a plain `if`.

---

## 41.9 Professional Insights

**Use `consteval` to guarantee work disappears from the runtime binary.** When a computation must happen at compile time — a generated lookup table, format-string validation, a factory that should never run at runtime — `consteval` makes the requirement enforced, not hoped-for. A `constexpr` function *might* run at runtime if called with non-constant arguments; `consteval` makes that a compile error, which is exactly what you want for code whose entire value is being precomputed.

**Reach for `constinit` to kill the static initialization order fiasco without giving up mutability.** Globals and `thread_local`s that must be writable but must also be ready before dynamic initialization are the precise use case. `const`/`constexpr` would force immutability; `constinit` guarantees constant initialization while leaving the variable mutable, and for `thread_local`s it removes the per-access initialization guard — a measurable win in hot multithreaded paths.

**Treat `constexpr` containers as compute-time scratch space, not persistent data.** The transient-allocation rule means a `constexpr std::vector` lives only within a constant evaluation in C++20. The productive idiom is "build with a vector/string inside a `constexpr` function, return a `std::array`, a count, or a bool." Trying to declare a `constexpr std::vector` variable that survives to runtime will not compile, and understanding *why* (no escape from the constant evaluator's heap) prevents a frustrating fight with the compiler.

**Call `std::is_constant_evaluated()` inside a plain `if`, never `if constexpr`.** Inside `if constexpr` the trait is always `true`, silently disabling your runtime path. The correct pattern is a regular `if` so both branches are compiled and the right one is selected per call context. If your toolchain supports C++23, prefer `if consteval` for clarity — but in a strict C++20 build, the plain-`if` function-call form is the only correct spelling.

**Lean on the constexpr expansion to validate at build time what you used to assert at startup.** Virtual dispatch, `dynamic_cast`, exceptions-on-the-happy-path, and dynamic containers in constant evaluation mean configuration validation, protocol-table construction, and invariant checks can move from program startup into the compile. A malformed table or invalid config becomes a build failure rather than a runtime crash — the strongest possible place to catch it, and a natural fit for the zero-runtime-overhead discipline of latency-critical systems.
