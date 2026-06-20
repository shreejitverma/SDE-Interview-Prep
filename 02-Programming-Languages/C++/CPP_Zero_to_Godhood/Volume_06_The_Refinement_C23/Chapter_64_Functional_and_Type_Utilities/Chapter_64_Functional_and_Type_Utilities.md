# Chapter 64: Functional and Type Utilities

> C++23 sprinkles a generous handful of small, sharp library utilities across `<functional>`, `<utility>`, `<bit>`, `<type_traits>`, and `<string>`. None is a headline feature, but collectively they retire a long list of hand-written helpers: a move-aware `std::function` (`move_only_function`), value-category-correct member forwarding (`forward_like`), a clean enum-to-integer cast (`to_underlying`), an optimization-enabling "this is impossible" marker (`unreachable`), endianness swapping (`byteswap`), substring membership on strings (`contains`), in-place buffer filling (`resize_and_overwrite`), and several more. This chapter is the catalog, organized by what each tool replaces.

## Table of Contents

1. [`std::move_only_function` — A Movable `std::function`](#641-stdmove_only_function--a-movable-stdfunction)
2. [`std::forward_like` — Value-Category-Correct Member Forwarding](#642-stdforward_like--value-category-correct-member-forwarding)
3. [`std::bind_back` and `std::invoke_r`](#643-stdbind_back-and-stdinvoke_r)
4. [Enum Utilities: `to_underlying` and `is_scoped_enum`](#644-enum-utilities-to_underlying-and-is_scoped_enum)
5. [`std::unreachable` and `std::byteswap`](#645-stdunreachable-and-stdbyteswap)
6. [String Additions: `contains` and `resize_and_overwrite`](#646-string-additions-contains-and-resize_and_overwrite)
7. [`reference_constructs_from_temporary` and `<stdatomic.h>` Interop](#647-reference_constructs_from_temporary-and-stdatomich-interop)
8. [Professional Insights](#648-professional-insights)

---

## 64.1 `std::move_only_function` — A Movable `std::function`

`std::function` has a quietly painful constraint: it requires its stored callable to be *copy-constructible*. A lambda that captures a `std::unique_ptr`, a `std::promise`, or any other move-only object cannot be stored in a `std::function` at all — the code simply fails to compile. This forced workarounds like `std::shared_ptr`-wrapping the capture, which adds an allocation and reference counting purely to satisfy a copyability requirement you never wanted.

`std::move_only_function<Signature>` (header `<functional>`) is the type-erased callable wrapper for **move-only** callables. It is move-constructible and move-assignable but not copyable, which lets it hold move-only closures directly. It also supports cv- and ref-qualifiers in its signature (`R(Args...) const`, `R(Args...) &&`), giving it more precise const-correctness than `std::function` ever had.

**Listing 64.1: Storing a move-only closure.**

```cpp
#include <functional>
#include <memory>
#include <print>

std::move_only_function<void()> make_task() {
    auto resource = std::make_unique<int>(42);
    // This lambda captures a unique_ptr -> NOT storable in std::function.
    return [r = std::move(resource)] { std::println("value = {}", *r); };
}

int main() {
    auto task = make_task();   // move-only, no allocation for a shared_ptr workaround
    task();
}
```

Reach for `move_only_function` for task queues, one-shot callbacks, continuations, and any callable storage where the closure owns a unique resource — which is most of them once you stop pre-emptively making captures copyable.

---

## 64.2 `std::forward_like` — Value-Category-Correct Member Forwarding

When you have a forwarding reference to an object and want to forward one of its *members* with the same value category, `std::forward` is not quite the right tool — it forwards the member's own type, not the *owner's* value category. `std::forward_like<Owner>(member)` (header `<utility>`) casts `member` to have the value category (and constness) of `Owner`. It is the natural companion to deducing `this` (Chapter 55): inside an explicit-object-parameter function, you forward a member as `std::forward_like<Self>(self.member)`.

**Listing 64.2: Forwarding a member with the object's value category.**

```cpp
#include <utility>
#include <string>

struct Holder {
    std::string data;

    // Forward 'data' with the same value category as the Holder object itself.
    template <typename Self>
    auto&& get_data(this Self&& self) {
        return std::forward_like<Self>(self.data);
        // lvalue Holder -> string&; rvalue Holder -> string&&; const propagates too
    }
};
```

`forward_like` encodes the rule "treat this member as if it had the owner's value category and constness," which is fiddly and error-prone to write by hand with nested `static_cast`s and `conditional_t`. It exists precisely because deducing `this` made member-forwarding a common need.

---

## 64.3 `std::bind_back` and `std::invoke_r`

- **`std::bind_back(f, args...)`** (header `<functional>`) is the mirror of C++20's `std::bind_front`: it binds arguments to the **trailing** parameters of `f`, leaving the leading ones to be supplied at the call. It is the building block for partial application where the *last* arguments are fixed — common when adapting a function to a ranges algorithm that supplies the element as the first argument.
- **`std::invoke_r<R>(f, args...)`** (header `<functional>`) is `std::invoke` with an explicit return type `R`: it invokes `f` and converts the result to `R` (and is valid when `R` is `void`, discarding the result). It removes the boilerplate of wrapping `static_cast<R>(std::invoke(...))` and handles the `void` case cleanly.

**Listing 64.3: Trailing-argument binding and typed invocation.**

```cpp
#include <functional>
#include <print>

int subtract(int a, int b) { return a - b; }

int main() {
    auto minus10 = std::bind_back(subtract, 10);  // fixes b = 10
    std::println("{}", minus10(30));               // subtract(30, 10) = 20

    // invoke_r forces the result type (here narrowing the double to int).
    auto r = std::invoke_r<int>([]{ return 3.9; });
    std::println("{}", r);                         // 3
}
```

---

## 64.4 Enum Utilities: `to_underlying` and `is_scoped_enum`

- **`std::to_underlying(e)`** (header `<utility>`) converts a scoped or unscoped enum to its underlying integer type. It replaces the verbose and easy-to-mistype `static_cast<std::underlying_type_t<E>>(e)`, and because it deduces the underlying type it cannot silently cast to the *wrong* integer width.
- **`std::is_scoped_enum<T>`** / **`std::is_scoped_enum_v<T>`** (header `<type_traits>`) is the trait that distinguishes a scoped enum (`enum class`) from an unscoped `enum` — previously you had to compose `is_enum` with a convertibility check by hand.

**Listing 64.4: Clean enum-to-integer conversion.**

```cpp
#include <utility>
#include <type_traits>
#include <print>

enum class Color : std::uint8_t { Red = 1, Green = 2, Blue = 4 };

int main() {
    Color c = Color::Green;
    auto raw = std::to_underlying(c);          // std::uint8_t{2}, correct width
    std::println("raw = {}", raw);

    static_assert(std::is_scoped_enum_v<Color>);
}
```

`to_underlying` is the kind of utility you will use constantly once you have it — every place that logs, serializes, or indexes by an enum value benefits.

---

## 64.5 `std::unreachable` and `std::byteswap`

- **`std::unreachable()`** (header `<utility>`) marks a point the program can never reach. It is *not* a runtime check — reaching it is undefined behavior — but it gives the optimizer license to assume the impossible cannot happen, eliminating dead branches and bounds checks. The canonical use is the default of a `switch` that provably handles every case.
- **`std::byteswap(x)`** (header `<bit>`) reverses the byte order of an integral value — the standard, single-call endianness swap that replaces compiler intrinsics (`__builtin_bswap32`) and hand-rolled shift-and-mask code, and is `constexpr`.

**Listing 64.5: Optimization hint and endianness conversion.**

```cpp
#include <utility>
#include <bit>
#include <cstdint>
#include <print>

enum class Op { Add, Sub };

int apply(Op op, int a, int b) {
    switch (op) {
        case Op::Add: return a + b;
        case Op::Sub: return a - b;
    }
    std::unreachable();   // every Op is handled; tell the optimizer so
}

int main() {
    std::println("{}", apply(Op::Add, 2, 3));        // 5

    std::uint32_t host = 0x01020304u;
    std::uint32_t swapped = std::byteswap(host);     // 0x04030201
    std::println("{:#010x}", swapped);
}
```

`unreachable` must be used only where unreachability is *guaranteed*; if control can actually reach it, you have introduced UB, not a diagnostic. Pair it with `[[assume]]` (Chapter 66) as the two "promise the compiler something" primitives.

---

## 64.6 String Additions: `contains` and `resize_and_overwrite`

- **`std::string::contains` / `std::string_view::contains`** answers "does this string contain that substring (or character)?" directly, replacing the idiom `s.find(sub) != std::string::npos` — which is correct but verbose and a perennial source of "I forgot the `!= npos`" bugs. (This is the string analogue of `std::ranges::contains` from Chapter 60, and parallels the `starts_with`/`ends_with` members added in C++20.)
- **`std::string::resize_and_overwrite(n, op)`** resizes the string to (up to) `n` characters and invokes `op(ptr, n)` to fill the buffer *in place*, then sets the final size to whatever `op` returns. It eliminates the redundant zero-initialization that `resize(n)` performs before you overwrite the bytes anyway — the standard, safe way to write directly into a string's storage when interfacing with a C API or a fast formatter.

**Listing 64.6: Substring test and zero-copy buffer fill.**

```cpp
#include <string>
#include <string_view>
#include <print>
#include <cstdio>

int main() {
    std::string_view url = "https://example.com/path";
    std::println("{}", url.contains("example"));     // true (no find()/npos dance)

    // Fill a string directly, skipping resize()'s zero-init of the buffer.
    std::string buf;
    buf.resize_and_overwrite(32, [](char* p, std::size_t cap) {
        int written = std::snprintf(p, cap, "pid=%d", 1234);
        return static_cast<std::size_t>(written);    // final size
    });
    std::println("{}", buf);                          // pid=1234
}
```

`resize_and_overwrite` is a genuine performance tool: for large strings filled from a C API it removes an O(n) memset that `resize` would otherwise perform.

---

## 64.7 `reference_constructs_from_temporary` and `<stdatomic.h>` Interop

Two more specialized additions round out the chapter:

- **`std::reference_constructs_from_temporary<T, U>`** / **`reference_converts_from_temporary`** (header `<type_traits>`) are traits that detect, at compile time, whether binding a reference of type `T` from an expression of type `U` would bind to a *temporary* — i.e. create a dangling reference. They are the building blocks the standard library itself uses (for example in `std::tuple` and `std::pair` constructors) to *reject* lifetime-bug-prone conversions, and you can use them in your own templates to refuse APIs that would silently dangle.
- **`<stdatomic.h>` interop.** C++23 makes the C `<stdatomic.h>` header usable from C++ so that `_Atomic`-qualified types and the C atomics map onto `std::atomic`. This matters for code that shares atomic data structures across a C/C++ boundary (drivers, shared headers), letting the two languages agree on atomic layout and semantics rather than relying on ad-hoc compatibility.

```cpp
#include <type_traits>

// Reject construction that would bind a reference to a temporary (dangle).
template <typename T, typename U>
void bind_checked(const T&) requires (!std::reference_constructs_from_temporary_v<const T&, U>);
```

> **Version-trap flag:** every facility in this chapter — `move_only_function`, `forward_like`, `bind_back`, `invoke_r`, `to_underlying`, `is_scoped_enum`, `unreachable`, `byteswap`, string/`string_view` `contains`, `resize_and_overwrite`, `reference_constructs_from_temporary`, and `<stdatomic.h>` C++ interop — is **C++23**. C++20 had `bind_front` but not `bind_back`; `starts_with`/`ends_with` (C++20) but not `contains` (C++23).

---

## 64.8 Professional Insights

**Default to `std::move_only_function` for callable storage; reserve `std::function` for when you genuinely need copies.** The copyability requirement of `std::function` is a constraint most code does not actually want, and satisfying it for a move-only closure means an avoidable `shared_ptr` allocation. Task queues, callbacks, and continuations almost always store a callable that is invoked once or moved around but never copied — `move_only_function` fits that shape exactly, holds move-only captures directly, and gives you precise const/ref-qualified signatures as a bonus.

**Adopt the small enum and string utilities wholesale — they remove entire bug categories.** `to_underlying` cannot cast to the wrong width the way a hand-written `static_cast` can; `string::contains` cannot forget the `!= npos`; `byteswap` cannot get the shift-and-mask wrong. Each is trivial in isolation, but standardizing on them across a codebase eliminates a recurring class of silent mistakes, and they read more clearly than the idioms they replace. There is no downside to using them everywhere the standard is available.

**Use `unreachable` (and `[[assume]]`) only where the impossibility is *proven*, and treat them as UB contracts, not assertions.** `std::unreachable()` is a promise to the optimizer, not a check; if control can actually reach it, you have written undefined behavior, not added a guard. It earns real speedups — eliminated branches, dropped bounds checks — in exhaustively-handled `switch`es and post-validation hot paths, but it must be backed by an argument that the point is genuinely unreachable. When in doubt, use a real assertion or `std::abort` instead.

**Lean on `forward_like` and the temporary-binding traits to keep generic code lifetime-correct.** Member forwarding under deducing `this` is exactly the situation where hand-written casts get value category or constness subtly wrong; `forward_like<Self>` encodes the correct rule in one call. And `reference_constructs_from_temporary` lets your own templates refuse, at compile time, the conversions that would dangle — the same defensive technique the standard library now applies to `tuple` and `pair`. In template-heavy, performance-sensitive code these are the tools that make aggressive forwarding safe rather than a source of latent dangling references.
