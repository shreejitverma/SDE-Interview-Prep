# Chapter 56: `std::expected` and Monadic Error Handling

> C++ has long forced a false choice between exceptions — powerful but with control flow many domains forbid — and sentinel returns like `std::optional`, which tell you that something failed but throw away *why*. `std::expected<T, E>` is the resolution: a value-based result type that holds either a success value of type `T` **or** an error of type `E`, with the error reason preserved and zero hidden allocation. Paired with the monadic operations added to both `expected` and `optional`, it lets you compose fallible operations into flat pipelines instead of nested `if` checks. For the exception-averse worlds of trading and kernel code, this is the most consequential library addition in C++23.

## Table of Contents

1. [The Error-Handling Trilemma](#561-the-error-handling-trilemma)
2. [Anatomy of `std::expected<T, E>`](#562-anatomy-of-stdexpectedt-e)
3. [Constructing Success and Failure](#563-constructing-success-and-failure)
4. [Inspecting and Extracting the Result](#564-inspecting-and-extracting-the-result)
5. [Monadic Composition: `and_then`, `transform`, `or_else`, `transform_error`](#565-monadic-composition-and_then-transform-or_else-transform_error)
6. [Monadic `std::optional`](#566-monadic-stdoptional)
7. [Performance: Why `expected` Belongs in Hot Paths](#567-performance-why-expected-belongs-in-hot-paths)
8. [Professional Insights](#568-professional-insights)

---

## 56.1 The Error-Handling Trilemma

Before C++23, returning failure from a function meant choosing one of three imperfect tools:

1. **Exceptions.** Expressive and composable, but they impose unpredictable control flow and a cost on the throw path that is unacceptable in latency-bounded code. Many large C++ shops compile with `-fno-exceptions` outright, and most kernels forbid them entirely.
2. **`std::optional<T>`.** Cheap and exception-free, but it collapses every failure mode into a single bit: you learn *that* the operation produced nothing, never *why*. A parser returning `nullopt` cannot distinguish "end of input" from "malformed token."
3. **Error codes / out-parameters.** Carries the reason, but pollutes the signature, is trivially ignored, and decouples the error from the value so the two can drift out of sync.

`std::expected<T, E>` keeps the virtues of each while shedding their costs: it is a single return value (no out-parameters), it carries the full error reason (`E`, not a bit), and it is exception-free with no heap allocation. It makes "this function can fail, and here is how" a visible part of the type.

---

## 56.2 Anatomy of `std::expected<T, E>`

`std::expected<T, E>`, declared in `<expected>`, is a discriminated union: at any moment it holds *either* a `T` (the **expected** value) or an `E` (the **unexpected** error), never both and never neither. It is conceptually `variant<T, E>` with an asymmetry baked in — `T` is the "normal" outcome and the API is built around the happy path being the default.

The error side is wrapped in the helper type `std::unexpected<E>` (and constructed via `std::unexpected(e)`) so that `T` and `E` can be the same type without ambiguity. The companion `std::bad_expected_access<E>` exception is thrown if you call `.value()` on an object that actually holds an error — the only place `expected` interacts with exceptions, and only if you opt in.

```cpp
#include <expected>
#include <string>

enum class Error { NotFound, PermissionDenied };

std::expected<std::string, Error> read_file(int id) {
    if (id < 0) return std::unexpected(Error::NotFound);
    return "File Content";   // implicit construction of the success value
}
```

Note the asymmetry in the return statements: a bare `return "File Content";` constructs the success side implicitly, while the error side must be spelled `std::unexpected(...)`. This keeps the happy path syntactically lightweight.

---

## 56.3 Constructing Success and Failure

There are several ways to populate an `expected`, each with a purpose:

- **Implicit success:** `return value;` — the most common case; the value converts into the `T` slot.
- **Explicit error:** `return std::unexpected(err);` — wraps `err` so it lands in the `E` slot.
- **In-place success:** `std::expected<T, E>{std::in_place, args...}` constructs `T` directly from `args`, avoiding a move.
- **In-place error:** `std::expected<T, E>{std::unexpect, args...}` constructs `E` in place.
- **`void` success type:** `std::expected<void, E>` models "an operation that either succeeds with no value or fails with reason `E`" — the natural return type for a command that has side effects but no result.

**Listing 56.1: A validation function returning `expected<void, E>`.**

```cpp
#include <expected>
#include <string>
#include <print>

enum class FormError { TooShort, MissingAt };

std::expected<void, FormError> validate_email(std::string_view s) {
    if (s.size() < 3)                       return std::unexpected(FormError::TooShort);
    if (s.find('@') == std::string_view::npos) return std::unexpected(FormError::MissingAt);
    return {};   // success carries no value
}

int main() {
    if (auto r = validate_email("a@b"); r)
        std::println("valid");
    else
        std::println("invalid: code {}", std::to_underlying(r.error()));
}
```

---

## 56.4 Inspecting and Extracting the Result

The query and access surface mirrors `optional` but adds an error accessor:

| Member | Meaning |
|---|---|
| `e.has_value()` / `explicit operator bool` | `true` if holding a value |
| `*e`, `e->m` | unchecked access to the value (UB if it holds an error) |
| `e.value()` | checked access; throws `bad_expected_access<E>` if it holds an error |
| `e.error()` | access to the error (UB if it holds a value) |
| `e.value_or(fallback)` | the value, or `fallback` if it holds an error |

The idiomatic pattern is to test with `if (e)` and then use `*e`, reserving `.value()` for code paths where you have already proven success or genuinely want the throw. `.error()` is symmetric and only valid in the failure branch.

---

## 56.5 Monadic Composition: `and_then`, `transform`, `or_else`, `transform_error`

The headline ergonomic feature is the **monadic interface**, which lets you chain fallible operations without manually unpacking and re-checking at each step — eliminating the nested-`if` "pyramid of doom." Four operations cover the cases:

- **`and_then(f)`** — if holding a value, call `f(value)`; `f` must itself return an `expected` (it can fail). If holding an error, the error passes through untouched. This is monadic *bind*: it chains operations that can each fail.
- **`transform(f)`** — if holding a value, call `f(value)` and wrap the **raw** result back into an `expected`; `f` returns a plain value, not an `expected`. The error passes through. This is *map*: it applies an infallible transformation to the success value.
- **`or_else(f)`** — if holding an *error*, call `f(error)`, which returns an `expected` (a recovery attempt). If holding a value, the value passes through.
- **`transform_error(f)`** — if holding an error, replace it with `f(error)` (a new error type/value). Used to translate error types as a result crosses a layer boundary.

The rule of thumb: **`and_then` when the next step can fail, `transform` when it cannot; `or_else` to recover, `transform_error` to translate.**

**Listing 56.2: A flat, fail-fast pipeline over `expected`.**

```cpp
#include <expected>
#include <string>
#include <print>

enum class Error { NotFound, Parse, OutOfRange };

std::expected<std::string, Error> fetch(int id);          // may fail
std::expected<int, Error>         parse(const std::string&); // may fail
int                               scale(int x) { return x * 10; } // cannot fail

std::expected<int, Error> pipeline(int id) {
    return fetch(id)
        .and_then(parse)          // string -> int, may fail
        .transform(scale)         // int -> int, cannot fail
        .transform_error([](Error e) {           // remap on the way out
            return e == Error::NotFound ? Error::OutOfRange : e;
        });
}

int main() {
    auto r = pipeline(7);
    std::println("{}", r ? std::to_string(*r) : "failed");
}
```

If `fetch` fails, `parse`, `scale`, and the success path are all skipped and the error flows straight to the bottom — a short-circuit with no branches written by hand.

---

## 56.6 Monadic `std::optional`

C++23 retrofits the same `and_then` / `transform` / `or_else` operations onto `std::optional` (the long-existing C++17 type). This lets you compose optional-returning operations with the same flat style, the difference being that `optional` carries no error payload — `or_else` takes a nullary callable since there is no error to inspect.

**Listing 56.3: Flattening nested `optional` lookups.**

```cpp
#include <optional>
#include <string>

struct User { std::string email; };
std::optional<User>        get_user();
std::optional<std::string> get_email(const User&);

std::optional<std::string> verified_email() {
    return get_user()
        .and_then(get_email)                               // User -> optional<string>
        .transform([](auto e){ return e + " verified"; })  // string -> string
        .or_else([]{ return std::optional<std::string>{"Unknown"}; });
}
```

The pre-C++23 version of this code was a staircase of `if (auto u = get_user())` blocks; the monadic form is a single expression with the same short-circuiting semantics.

---

## 56.7 Performance: Why `expected` Belongs in Hot Paths

For latency-sensitive code, `std::expected` is close to ideal:

- **No heap allocation.** The value and error live inline in the object, in storage sized to `max(sizeof(T), sizeof(E))` plus a discriminant. There is no allocation on either the success or failure path.
- **No throw on the failure path.** Returning `std::unexpected(...)` is an ordinary value return — no stack unwinding, no exception tables, no unpredictable cost. This is what makes it usable under `-fno-exceptions`.
- **Branch-predictable.** Failure is a normal `return`, so the CPU's branch predictor handles the happy path the same way it would handle any other early return. Contrast with exceptions, whose cold-path machinery the optimizer must keep reachable.
- **Monadic chains optimize well.** `and_then`/`transform` are small inline templates; with the lambdas visible, the optimizer collapses the chain into the same code a hand-written staircase would produce — you pay nothing for the abstraction.

The one cost to watch is **object size**: `expected<T, E>` is as large as its bigger alternative plus the discriminant, so returning `expected<BigStruct, BigError>` by value moves more bytes than a bare `optional<BigStruct>`. Keep `E` small (an `enum` or an error-code struct, not a `std::string` message you rarely read) when the type appears in hot return paths.

> **Version-trap flag:** `std::expected`, its monadic members, and the monadic members of `std::optional` are all C++23. The C++20 `std::optional` had `value_or` but none of `and_then`/`transform`/`or_else`. Do not assume monadic `optional` compiles under `-std=c++20`.

---

## 56.8 Professional Insights

**Make fallibility part of the type, and `expected` will improve your API design, not just your error handling.** The deepest benefit is that a function returning `std::expected<T, E>` advertises both that it can fail and the vocabulary of its failures, at the type level, where callers cannot ignore it. Retrofitting `expected` into an interface forces you to enumerate `E` — and that enumeration is usually clarifying, surfacing failure modes that error codes had quietly merged.

**Keep `E` small and stable; treat it as part of your ABI.** Because `expected` is sized to the larger of `T` and `E` and is moved by value, a fat error type taxes every call on the happy path. Prefer a compact `enum class` or a small error struct, and translate to a richer human-readable form only at the boundary where you actually render the error — `transform_error` is the tool for that translation.

**Use `and_then`/`transform` to flatten, but do not over-monadize.** The monadic interface shines when several fallible steps chain naturally; it earns its keep by removing the staircase. But a single fallible call followed by ordinary logic is clearer with an `if (!r) return r.error();` guard than with a contrived `and_then`. Reach for the monadic style when it removes nesting, not as a reflex.

**Standardize on `expected` even in exception-using codebases.** Reserve exceptions for truly exceptional, unrecoverable conditions and use `expected` for *expected* failures — file-not-found, parse errors, validation. This split gives you cheap, branch-predictable handling for the common failures while keeping exceptions for the rare ones, and it documents the distinction in the type system. In `-fno-exceptions` environments, `expected` simply becomes your only principled error channel.
