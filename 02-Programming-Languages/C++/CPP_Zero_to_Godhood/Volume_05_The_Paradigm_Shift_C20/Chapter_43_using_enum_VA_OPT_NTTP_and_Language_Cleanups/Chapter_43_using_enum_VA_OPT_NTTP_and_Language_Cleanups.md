# Chapter 43: using enum, __VA_OPT__, Class-Type NTTPs, and Language Cleanups

> *This chapter collects the remaining smaller C++20 core-language changes that do not belong to one of the big pillars but each remove a long-standing irritation: `using enum` to pull scoped-enum names into scope, `__VA_OPT__` to write variadic macros that behave correctly with zero arguments, class types as non-type template parameters (which is what lets string literals parameterize templates), the mandate that signed integers are two's complement, the new `char8_t` type for UTF-8, and the deprecation of several `volatile` uses. Individually minor; collectively they sand down the rough edges of daily C++.*

None of these features reshapes how you architect a program, but each fixes something that has annoyed C++ programmers for years. `using enum` ends the verbose `Color::Red` repetition inside switches; `__VA_OPT__` finally makes trailing-comma-free variadic macros possible; class-type NTTPs unlock compile-time string parameters; and the two's-complement mandate removes a whole category of "technically UB" reasoning about signed integers. Knowing them rounds out fluency in modern C++.

---

## Table of Contents

- [43.1 using enum](#431-using-enum)
- [43.2 __VA_OPT__ for Variadic Macros](#432-va_opt-for-variadic-macros)
- [43.3 Class Types as Non-Type Template Parameters](#433-class-types-as-non-type-template-parameters)
- [43.4 String Literals as Template Parameters](#434-string-literals-as-template-parameters)
- [43.5 Signed Integers Are Two's Complement](#435-signed-integers-are-twos-complement)
- [43.6 char8_t and UTF-8](#436-char8_t-and-utf-8)
- [43.7 Deprecated volatile Uses and Other Cleanups](#437-deprecated-volatile-uses-and-other-cleanups)
- [43.8 Professional Insights](#438-professional-insights)

---

## 43.1 using enum

`using enum E;` injects all of a scoped enum's enumerator names into the current scope, so you can write `Red` instead of `Color::Red`. It is most valuable inside a `switch` over a scoped enum, where the `EnumName::` prefix repeated on every `case` is pure noise.

```cpp
// Listing 43.1: using enum removes the repeated scope qualifier
enum class Color { Red, Green, Blue };

const char* name(Color c) {
    switch (c) {
        using enum Color;          // bring Red/Green/Blue into this scope
        case Red:   return "red";
        case Green: return "green";
        case Blue:  return "blue";
    }
    return "?";
}

// Also usable at block scope:
void demo() {
    using enum Color;
    Color c = Green;               // 'Green' resolves to Color::Green
}
```

`using enum` preserves the type-safety of scoped enums — `Green` is still a `Color`, not a bare `int` — while removing only the syntactic verbosity. You can also bring in a *single* enumerator with `using Color::Red;`. Scope the `using enum` as tightly as possible (inside the `switch` or a small block) so the unqualified names do not leak and collide elsewhere.

---

## 43.2 __VA_OPT__ for Variadic Macros

`__VA_OPT__(content)` expands to `content` only when the variadic argument list is **non-empty**, and to nothing when it is empty. This finally solves the decades-old "trailing comma" problem in variadic macros, where `, ##__VA_ARGS__` was a non-standard GCC extension.

```cpp
// Listing 43.2: __VA_OPT__ handles the zero-argument case correctly
#include <cstdio>

// The comma before __VA_ARGS__ appears ONLY when there are arguments:
#define LOG(fmt, ...) std::printf(fmt __VA_OPT__(,) __VA_ARGS__)

// Both forms now expand correctly:
//   LOG("hello\n");              -> std::printf("hello\n")           (no trailing comma)
//   LOG("%d %d\n", 1, 2);        -> std::printf("%d %d\n", 1, 2)

#define WRAP(...) f(0 __VA_OPT__(,) __VA_ARGS__)
//   WRAP()        -> f(0)
//   WRAP(a, b)    -> f(0, a, b)
```

The mechanism: `__VA_OPT__(,)` emits the comma only if `__VA_ARGS__` is non-empty, so `LOG("hi")` does not produce the malformed `printf("hi",)`. This is the **standard, portable** replacement for the GCC `, ##__VA_ARGS__` hack. It can wrap any tokens, not just a comma — `__VA_OPT__(prefix)` is occasionally used to conditionally emit a keyword or separator.

---

## 43.3 Class Types as Non-Type Template Parameters

C++20 allows **literal class types** to be used as non-type template parameters (NTTPs), where previously NTTPs were limited to integers, enums, pointers, and references. A class type qualifies if it is a *structural type*: a literal type whose members are all public and themselves structural.

```cpp
// Listing 43.3: a structural class type as a template parameter
struct Point {                 // structural: literal type, all-public members
    int x;
    int y;
};

template<Point P>              // a class-type NTTP
struct Tagged {
    static constexpr int sum() { return P.x + P.y; }
};

using T = Tagged<Point{3, 4}>;       // pass a Point value as the parameter
static_assert(T::sum() == 7);
```

The requirements for a **structural type** are strict: all base classes and non-static data members must be public and non-mutable, and all of them must themselves be structural (scalars, or structural class types, or arrays thereof). This rules out types with private members or user-defined copy semantics, because the parameter's identity must be determined by its members' values (the compiler compares NTTP arguments member-wise). The headline application is the next section: compile-time strings.

---

## 43.4 String Literals as Template Parameters

The most important consequence of class-type NTTPs is that you can build a structural wrapper around a character array and thereby **pass string literals as template arguments** — long requested, finally possible.

```cpp
// Listing 43.4: a fixed-string NTTP wrapper enables string template parameters
#include <algorithm>
#include <cstddef>

template<std::size_t N>
struct FixedString {
    char data[N]{};
    constexpr FixedString(const char (&str)[N]) {   // consume a string literal
        std::copy_n(str, N, data);
    }
};

template<FixedString S>          // a string literal as a template parameter!
struct Named {
    static constexpr const char* value() { return S.data; }
};

constexpr auto n = Named<"hello">::value();   // "hello" parameterizes the template
```

`FixedString` is a structural type (public `char[N]` member, literal, `constexpr` constructor), so `Named<"hello">` deduces `N` from the literal and stores its characters in the NTTP. This is the foundation for compile-time string processing: type-safe format strings, compile-time-named dimensions/units, reflection-style tags, and DSLs that take string keys — all checkable at compile time. The pattern underpins many modern C++20 libraries (compile-time JSON pointers, fixed-string event names, etc.).

---

## 43.5 Signed Integers Are Two's Complement

C++20 **mandates two's-complement representation** for all signed integer types. Previously the standard allowed sign-magnitude and one's-complement, leaving certain operations implementation-defined or undefined for portability with exotic hardware that no longer exists.

```cpp
// Listing 43.5: two's complement is now guaranteed
#include <climits>
#include <cstdint>

static_assert(INT_MIN == -INT_MAX - 1);   // guaranteed: asymmetric range, two's comp

// Bit patterns of signed integers are now well-defined:
//   int8_t  n = -1;   has the bit pattern 0xFF, guaranteed.
// Conversion from unsigned to signed out-of-range is still defined by the
// usual modular rule; what's new is that the *representation* is fixed.

std::int32_t mix(std::uint32_t u) {
    return static_cast<std::int32_t>(u);   // well-defined two's-complement reinterpret
}
```

The practical effect: the bit representation of signed integers is now portable and predictable, `INT_MIN` is always `-INT_MAX - 1` (the asymmetric range), and reasoning about sign/unsigned bit tricks is sound on every conforming implementation. **Signed integer overflow is still undefined behavior** — the mandate fixes the *representation*, not the *arithmetic-overflow* rules. This distinction trips people up: `INT_MAX + 1` is still UB; what changed is that `-1` is guaranteed to be all-bits-set.

---

## 43.6 char8_t and UTF-8

C++20 introduces **`char8_t`**, a distinct fundamental type for UTF-8 code units, separate from `char`. UTF-8 string literals `u8"..."` now have type `const char8_t[]` (previously `const char[]`), making UTF-8 data type-distinguishable from arbitrary bytes.

```cpp
// Listing 43.6: char8_t and u8 literals
#include <string>

const char8_t* utf8 = u8"héllo";        // type is const char8_t[], not const char[]
std::u8string s = u8"naïve";            // std::u8string == basic_string<char8_t>

// char8_t is distinct: it does NOT implicitly convert to char*.
// const char* p = u8"x";               // ERROR in C++20 (was OK in C++17)

// To interoperate with char-based APIs, reinterpret explicitly:
auto as_bytes = reinterpret_cast<const char*>(utf8);
```

The motivation is type safety: before C++20, `u8"..."` was `const char[]`, indistinguishable from a Latin-1 or raw-byte string, so the compiler could not help you keep encodings straight. `char8_t` makes "this is UTF-8" part of the type. The cost is a **breaking change**: code that assigned `u8"..."` to `const char*` no longer compiles, and `std::u8string` is a separate type from `std::string`. This is the most common C++17→C++20 source-compatibility break; the fix is an explicit `reinterpret_cast` at the boundary to legacy `char`-based APIs.

---

## 43.7 Deprecated volatile Uses and Other Cleanups

C++20 **deprecates several uses of `volatile`** that had unclear or unimplementable semantics, narrowing it toward its only well-defined purpose (memory-mapped I/O and signal handlers).

```cpp
// Listing 43.7: deprecated volatile patterns in C++20
volatile int v = 0;

// Deprecated: compound assignment / increment on a volatile (read-modify-write
// has ambiguous ordering semantics):
// v += 1;        // deprecated
// ++v;           // deprecated
// v *= 2;        // deprecated

// Still fine: a plain volatile load or store (the actual MMIO use case):
int x = v;        // OK: volatile read
v = 5;            // OK: volatile write

// Deprecated: volatile-qualified function parameters and return types,
// and volatile structured bindings.
```

The reasoning: a compound assignment like `v += 1` on a `volatile` is a read-modify-write whose ordering and atomicity were never well-specified — it looks atomic but is not, a trap for the device-driver programmers `volatile` is meant to serve. C++20 deprecates these compound forms (and `volatile` parameters/returns) to push code toward explicit `load`/`store` patterns or, for concurrency, `std::atomic` — `volatile` is for hardware-visible side effects, never for thread synchronization. Other minor cleanups in the same release include the range-`for` init-statement (Chapter 40) and the deprecation of the comma operator inside subscript expressions (`a[i, j]`), reserved for C++23's multidimensional `operator[]`.

---

## 43.8 Professional Insights

**Scope `using enum` tightly — inside the `switch` or a small block.** Its value is killing the repeated `Color::` prefix in a `switch`, and placing it at the top of the `switch` body confines the unqualified names to exactly where they help. Pulling enumerators into a wide scope (a whole function or, worse, a header namespace) reintroduces the name-collision risk that scoped enums were designed to eliminate. Use it as a local convenience, not a global import.

**Replace GCC's `, ##__VA_ARGS__` with `__VA_OPT__(,)` for portable variadic macros.** The old comma-elision hack is non-standard and silently behaves differently across compilers. `__VA_OPT__` is the standard, portable mechanism and handles the zero-argument case correctly on every C++20 compiler. When modernizing logging and assertion macros, this is a mechanical, safe substitution that removes a long-standing portability wart.

**Use class-type NTTPs and `FixedString` to move string-keyed logic to compile time.** Passing string literals as template parameters enables compile-time-checked format strings, named units, and tag-based dispatch with zero runtime cost. The `FixedString` structural-type wrapper is the canonical idiom; recognize it when reading modern libraries and reach for it when a key or name is known at compile time and you want the type system to enforce it.

**Treat `char8_t` as a deliberate, breaking encoding-safety upgrade.** Migrating to C++20 will break code that fed `u8"..."` into `const char*` APIs — this is intended. The right response is an explicit `reinterpret_cast<const char*>` (or a `std::u8string`-aware boundary) at the point where UTF-8 data meets legacy `char` APIs, keeping the encoding distinction visible in the types rather than silently erased. Audit `u8` literal usage when bumping the standard.

**Rely on the two's-complement mandate for representation, but never for overflow.** You may now portably assume `-1` is all-bits-set and `INT_MIN == -INT_MAX - 1`, which legitimizes a class of well-reasoned bit manipulations. But signed *overflow* remains undefined behavior — the mandate did not change that — so continue to guard arithmetic against overflow (or use unsigned/`std::numeric_limits` checks). Conflating "representation is defined" with "overflow is defined" is a subtle and dangerous mistake.
