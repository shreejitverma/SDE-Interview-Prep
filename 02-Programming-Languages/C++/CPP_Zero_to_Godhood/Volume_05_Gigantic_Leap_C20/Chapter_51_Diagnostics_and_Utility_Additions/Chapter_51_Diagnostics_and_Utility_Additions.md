# Chapter 51: Diagnostics and Utility Additions

> *C++20 adds a cluster of small but high-leverage utilities that together close long-standing gaps in diagnostics and everyday code. `std::source_location` captures file/line/function at the call site without macros — the type-safe successor to `__FILE__`/`__LINE__`. `std::ssize` returns a signed size to end the unsigned-comparison warning plague. And `std::is_constant_evaluated` lets one function take different paths at compile time versus run time. This chapter covers these and the related convenience helpers.*

These features individually look minor; collectively they remove some of the most persistent papercuts in C++. Logging and assertion frameworks have leaned on the `__FILE__`/`__LINE__`/`__func__` preprocessor macros for decades, with all the fragility macros bring; `std::source_location` makes the call site a real value you can pass and store. The signed/unsigned mismatch between `container.size()` (unsigned) and loop indices or `std::distance` (signed) generates a steady drip of warnings and subtle bugs that `std::ssize` finally addresses. This chapter is the "quality of life" tour of C++20.

---

## Table of Contents

- [51.1 std::source_location: Macro-Free Call-Site Info](#511-stdsource_location-macro-free-call-site-info)
- [51.2 Building a Logger with source_location](#512-building-a-logger-with-source_location)
- [51.3 std::ssize: Signed Size](#513-stdssize-signed-size)
- [51.4 std::is_constant_evaluated: One Function, Two Paths](#514-stdis_constant_evaluated-one-function-two-paths)
- [51.5 std::to_array: Deducing Array Types](#515-stdto_array-deducing-array-types)
- [51.6 std::bind_front: Cleaner Partial Application](#516-stdbind_front-cleaner-partial-application)
- [51.7 Professional Insights](#517-professional-insights)

---

## 51.1 std::source_location: Macro-Free Call-Site Info

`std::source_location` (header `<source_location>`) captures the **file, line, column, and function name** of a call site as a first-class value. The magic is `source_location::current()` used as a **default argument**: it is evaluated at the *caller's* location, not the callee's.

```cpp
// Listing 51.1: capturing the call site without macros
#include <source_location>
#include <iostream>

void trace(std::source_location loc = std::source_location::current()) {
    std::cout << loc.file_name()     << ':'    // e.g. "main.cpp"
              << loc.line()          << ':'    // e.g. 42
              << loc.column()        << " in "
              << loc.function_name() << '\n';  // e.g. "int main()"
}

int main() {
    trace();   // prints THIS line's location — current() bound at the call site
}
```

Because `current()` appears as a default argument, the compiler substitutes it at each call site, so `loc` describes where `trace` was *called* — exactly the behavior `__FILE__`/`__LINE__` macros provided, but as a typed object that can be stored, copied, and forwarded. It exposes `file_name()`, `function_name()`, `line()`, and `column()`, and `current()` is `consteval`, so the capture costs nothing at run time.

---

## 51.2 Building a Logger with source_location

The canonical use is threading call-site information through a logging or assertion API without forcing every caller to pass macros. The trick is putting the defaulted `source_location` **after** the message parameters.

```cpp
// Listing 51.2: a log function that knows where it was called
#include <source_location>
#include <string_view>
#include <iostream>
#include <format>

void log_error(std::string_view msg,
              std::source_location loc = std::source_location::current()) {
    std::cerr << std::format("[ERROR] {}:{} ({}): {}\n",
                             loc.file_name(), loc.line(),
                             loc.function_name(), msg);
}

void risky() {
    log_error("disk full");   // location points HERE, inside risky()
}
```

The defaulted-argument-last idiom means callers write `log_error("...")` and automatically get accurate location data, with no `#define LOG(m) log_error(m, __FILE__, __LINE__)` macro and none of the macro hazards (no token-pasting surprises, works with namespaces, debuggable). To capture the caller's location through a *template* forwarding wrapper, take the `source_location` as a defaulted parameter on the wrapper too — it propagates correctly because each default binds at its own call site.

---

## 51.3 std::ssize: Signed Size

`std::ssize(c)` (header `<iterator>`, also works on arrays) returns the size of a container as a **signed** integer (`std::ptrdiff_t`), unlike `c.size()` which is unsigned (`std::size_t`). This eliminates the signed/unsigned comparison warnings and the wraparound bugs they portend.

```cpp
// Listing 51.3: signed size for clean comparisons
#include <iterator>
#include <vector>

void demo(const std::vector<int>& v, int offset) {
    // Unsigned size() makes mixed comparisons warn — and underflow when empty:
    // for (std::size_t i = 0; i < v.size() - 1; ++i)   // v.size()-1 wraps if empty!

    // Signed ssize() compares cleanly with signed indices and never wraps:
    for (int i = 0; i < std::ssize(v) - 1; ++i) { /* ... */ }   // safe when empty

    if (offset < std::ssize(v)) { /* ... */ }   // no signed/unsigned warning
}
```

The classic trap is `v.size() - 1` when `v` is empty: unsigned arithmetic wraps to a huge positive number, and the loop runs catastrophically. `std::ssize(v) - 1` is `-1`, so the loop body is correctly skipped. `ssize` also lets signed loop indices and `std::distance` results (already signed) compare against container sizes without the pervasive `-Wsign-compare` noise — a small change that removes a whole category of warnings and the bugs hiding among them.

---

## 51.4 std::is_constant_evaluated: One Function, Two Paths

`std::is_constant_evaluated()` (header `<type_traits>`) returns whether the current evaluation is happening **at compile time**, letting a single `constexpr` function choose a compile-time-friendly algorithm or a faster runtime one (e.g. a hardware intrinsic).

```cpp
// Listing 51.4: branching on compile-time vs run-time evaluation
#include <type_traits>

constexpr double power(double base, int exp) {
    if (std::is_constant_evaluated()) {
        // Compile-time path: simple loop the constant evaluator can run.
        double r = 1.0;
        for (int i = 0; i < exp; ++i) r *= base;
        return r;
    } else {
        // Run-time path: may call a non-constexpr intrinsic / std::pow, etc.
        return __builtin_powi(base, exp);   // illustrative fast runtime path
    }
}

constexpr double a = power(2.0, 10);   // uses the loop (compile time)
double b = power(2.0, 10);             // uses the fast path (run time)
```

The function must be used in **both** contexts to matter: when invoked in a constant expression the trait is `true` and the constexpr-safe branch runs; at run time it is `false` and the optimized branch runs. A critical gotcha: you must call it as `std::is_constant_evaluated()` inside a regular `if`, **never** `if constexpr` — `if constexpr (std::is_constant_evaluated())` is *always* true (the condition itself is a constant expression), silently disabling the runtime path. **Version note:** C++23 adds the dedicated `if consteval` statement that expresses this intent more safely; in C++20 the plain-`if` form is the only option.

---

## 51.5 std::to_array: Deducing Array Types

`std::to_array` (header `<array>`) converts a built-in array (or braced list) into a `std::array` with the element type and size **deduced**, avoiding the need to spell both out — and enabling array construction from contexts where CTAD alone falls short.

```cpp
// Listing 51.5: deducing std::array element type and size
#include <array>

// Without to_array you must repeat the type and count:
std::array<int, 3> a1{1, 2, 3};

// With to_array, both are deduced from the initializer:
auto a2 = std::to_array({1, 2, 3});            // std::array<int, 3>
auto a3 = std::to_array<long>({1, 2, 3});      // std::array<long, 3> (explicit elem)
auto a4 = std::to_array("hello");              // std::array<char, 6> (incl. '\0')

// Converts a C array (e.g. from a macro or C API) into a std::array:
int c_arr[] = {10, 20, 30};
auto a5 = std::to_array(c_arr);                // std::array<int, 3>
```

`to_array` deduces the size from the number of initializers and copies the elements, giving you a fully-typed `std::array` without restating `<int, 3>`. It is particularly useful for string literals (`to_array("hello")` yields `array<char, 6>` including the terminator) and for converting C arrays returned by legacy APIs into the safer `std::array` interface.

---

## 51.6 std::bind_front: Cleaner Partial Application

`std::bind_front` (header `<functional>`) binds the **leading** arguments of a callable, returning a callable that takes the rest. It is a focused, safer replacement for `std::bind` for the common "fix the first N arguments" case — no placeholders, and it forwards remaining arguments perfectly.

```cpp
// Listing 51.6: partial application without placeholders
#include <functional>

int add(int a, int b, int c) { return a + b + c; }

auto add10 = std::bind_front(add, 10);     // fixes a=10; takes (b, c)
int r = add10(20, 30);                     // 60

// Binding a member function + object is a frequent use:
struct Net { void send(int channel, std::string_view msg); };
Net net;
auto send_ch0 = std::bind_front(&Net::send, &net, 0);  // fix object + channel
send_ch0("hello");                          // calls net.send(0, "hello")
```

Unlike `std::bind`, there are no `_1`/`_2` placeholders to manage and no surprising decay/copy semantics — `bind_front` simply stores the leading arguments and appends the call-time arguments after them. It is the idiomatic C++20 way to create a partially-applied callback, especially for binding a member function to an object (`bind_front(&T::method, obj)`). **Version note:** C++23 adds the symmetric `std::bind_back` (fixing trailing arguments); C++20 has only `bind_front`.

---

## 51.7 Professional Insights

**Replace every `__FILE__`/`__LINE__`/`__func__` logging macro with `std::source_location`.** A defaulted `source_location::current()` parameter gives accurate call-site information as a typed, storable value with no preprocessor involvement — no token-pasting fragility, no macro hygiene problems, and it composes with namespaces and templates. Put the defaulted `source_location` last in the parameter list so callers pass only their real arguments. This is one of the clearest wins in C++20 for any logging, tracing, or assertion library.

**Adopt `std::ssize` to kill signed/unsigned comparison bugs at the source.** The `container.size() - 1` underflow on an empty container is a perennial production bug, and `-Wsign-compare` noise trains developers to ignore warnings. `std::ssize` returns a signed `ptrdiff_t` that compares cleanly with loop indices and `std::distance` results and never wraps below zero. Prefer it wherever a size meets signed arithmetic; the consistency is worth more than the keystrokes.

**Use `std::is_constant_evaluated` only inside a plain `if`, never `if constexpr`.** Wrapping it in `if constexpr` makes the condition unconditionally true and silently deletes your runtime path — a bug that compiles cleanly and only shows up as missing optimization or, worse, a runtime call in a constexpr-only branch. Remember it requires the function to actually be evaluated in both contexts to be meaningful, and that C++23's `if consteval` is the safer successor when you can target it; under C++20 the plain-`if` discipline is mandatory.

**Prefer `std::bind_front` over `std::bind` for partial application, and `std::to_array` over restating array types.** `bind_front` eliminates the placeholder gymnastics and copy-semantics surprises of `std::bind` for the overwhelmingly common "fix the leading arguments / bind a member to an object" case, and it forwards the rest perfectly. `to_array` deduces element type and size so you never repeat `<T, N>`, and it cleanly upgrades C arrays and string literals into the safer `std::array` interface. Both are small, focused tools that make everyday code more readable and less error-prone — and remember their C++23 companions (`bind_back`) are not available under C++20.
