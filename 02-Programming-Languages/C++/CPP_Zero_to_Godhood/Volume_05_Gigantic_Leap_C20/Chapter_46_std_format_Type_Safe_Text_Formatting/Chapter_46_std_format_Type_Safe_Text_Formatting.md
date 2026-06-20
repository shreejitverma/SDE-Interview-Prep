# Chapter 46: std::format — Type-Safe Text Formatting

> *`std::format` brings Python-style, type-safe, compile-time-checked string formatting to C++, ending the long-standing choice between `printf` (fast, terse, but type-unsafe and crash-prone) and iostreams (type-safe but verbose, stateful, and slow). A format string with `{}` placeholders is checked against its arguments at compile time, the arguments carry their own types, and user-defined types can opt in by specializing one trait. This chapter covers the syntax, the format specification mini-language, custom formatters, and the performance and version landscape.*

The two pre-C++20 options each failed in a different way. `printf("%d", "oops")` compiles and then corrupts the stack at runtime, because the format specifier and the argument type are checked by nobody. `std::cout << x` is type-safe but drags in manipulator state (`std::setprecision`, `std::hex` that persists), reads poorly for anything with structure, and is hard to localize. `std::format("{}", x)` is type-safe like iostreams, terse like `printf`, **compile-time-validated** unlike either, and extensible to your own types — the format string and arguments are checked together when the program is built.

---

## Table of Contents

- [46.1 The Problem: printf vs iostreams](#461-the-problem-printf-vs-iostreams)
- [46.2 Basic Usage and Compile-Time Checking](#462-basic-usage-and-compile-time-checking)
- [46.3 Argument Indexing](#463-argument-indexing)
- [46.4 The Format Specification Mini-Language](#464-the-format-specification-mini-language)
- [46.5 Formatting Numbers: Width, Precision, Base, Alignment](#465-formatting-numbers-width-precision-base-alignment)
- [46.6 format_to and Output Iterators](#466-format_to-and-output-iterators)
- [46.7 Custom Formatters for User Types](#467-custom-formatters-for-user-types)
- [46.8 Performance and the C++20/23 Landscape](#468-performance-and-the-c2023-landscape)
- [46.9 Professional Insights](#469-professional-insights)

---

## 46.1 The Problem: printf vs iostreams

`std::format` (header `<format>`) provides **type-safe, compile-time-checked, Python-style** string formatting, superseding both `printf` and iostreams for most text production.

```cpp
// Listing 46.1: the three eras, side by side
#include <format>
#include <cstdio>
#include <iostream>
#include <string>

void demo(const std::string& name, int version) {
    // printf: terse but type-UNSAFE — a wrong specifier is UB, no checking.
    std::printf("Hello %s %d\n", name.c_str(), version);   // must remember .c_str()

    // iostreams: type-safe but verbose and stateful.
    std::cout << "Hello " << name << ' ' << version << '\n';

    // std::format: type-safe, terse, compile-time checked.
    std::string s = std::format("Hello {} {}\n", name, version);
    std::cout << s;
}
```

`std::format` returns a `std::string`. It needs no `.c_str()`, no manipulator state, and crucially the placeholders are matched to argument types **when the program compiles**, so a mismatch is a build error rather than a runtime catastrophe.

---

## 46.2 Basic Usage and Compile-Time Checking

Each `{}` is a placeholder filled by the next argument in order. The format string is a `consteval`-checked entity: if it is malformed or the arguments do not match, the program does not compile.

```cpp
// Listing 46.2: placeholders and compile-time validation
#include <format>
#include <string>

std::string a = std::format("{} + {} = {}", 2, 3, 5);     // "2 + 3 = 5"
std::string b = std::format("{}", 3.14159);               // "3.14159"
std::string c = std::format("{}", true);                  // "true"  (not "1")

// Compile-time errors (these do NOT compile in C++20):
// std::format("{}");            // ERROR: missing argument for the placeholder
// std::format("{:d}", "text");  // ERROR: 'd' (integer) spec on a string argument
// std::format("{:#}", x);       // ERROR if the spec is invalid for x's type
```

The compile-time checking is the headline safety property: the format string literal is validated against the argument types during compilation. A stray `{}` with no argument, or an integer specifier applied to a string, is caught at build time. (For format strings only known at runtime, `std::vformat` performs the same work with runtime checking instead.)

---

## 46.3 Argument Indexing

Placeholders may name their argument by **index** (`{0}`, `{1}`), allowing reordering and reuse of arguments — impossible with positional `printf`.

```cpp
// Listing 46.3: explicit argument indices
#include <format>

auto s1 = std::format("{0} {1} {0}", "ab", "cd");   // "ab cd ab"  (reuse arg 0)
auto s2 = std::format("{1}, {0}!", "World", "Hello");// "Hello, World!"  (reorder)

// You cannot mix automatic and manual indexing in one string:
// std::format("{} {1}", a, b);   // ERROR: pick one scheme, not both
```

Indexing shines for localization, where translated strings reorder arguments (`"{1} {0}"` in one language, `"{0} {1}"` in another) against the same argument list. The rule is consistency: a format string uses **either** automatic placeholders (`{}`) **or** explicit indices (`{0}`), never a mix.

---

## 46.4 The Format Specification Mini-Language

Inside the braces, after a colon, comes a **format specification**: `{:spec}`. The grammar mirrors Python's: `{:[[fill]align][sign][#][0][width][.precision][type]}`.

```cpp
// Listing 46.4: the anatomy of a format spec
#include <format>

// {: < ^ >  +   #  0  width . prec  type }
//    fill/align sign alt zero  ...        conversion
std::format("{:<10}", "left");     // "left      "  (left-aligned, width 10)
std::format("{:>10}", "right");    // "     right"  (right-aligned)
std::format("{:^10}", "mid");      // "   mid    "  (centered)
std::format("{:*^10}", "mid");     // "***mid****"  (fill with '*', centered)
std::format("{:+}", 42);           // "+42"         (always show sign)
std::format("{:08.3f}", 3.14159);  // "0003.142"    (zero-pad, width 8, 3 decimals)
```

| Field | Meaning | Example |
|-------|---------|---------|
| fill + align | `<` left, `>` right, `^` center, optional fill char | `{:*^10}` |
| sign | `+` always, `-` only negatives, ` ` space for positive | `{:+}` |
| `#` | alternate form (`0x` for hex, `0b` for binary) | `{:#x}` |
| `0` | zero-pad numbers to width | `{:08}` |
| width | minimum field width | `{:10}` |
| `.precision` | digits after point (float) / max chars (string) | `{:.3f}` |
| type | conversion: `d b o x f e g s` etc. | `{:x}` |

This single mini-language covers alignment, padding, sign control, number base, and precision uniformly across all types — replacing the scattered `printf` flags and the stateful iostream manipulators with one composable spec.

---

## 46.5 Formatting Numbers: Width, Precision, Base, Alignment

Numeric formatting is where the spec language earns its keep — bases, fixed/scientific notation, and precision are all one-liners.

```cpp
// Listing 46.5: numeric formatting
#include <format>

std::format("{:d}",    255);      // "255"      decimal
std::format("{:x}",    255);      // "ff"       hex (lower)
std::format("{:#X}",   255);      // "0XFF"     hex (upper) with 0X prefix
std::format("{:b}",    5);        // "101"      binary
std::format("{:#b}",   5);        // "0b101"    binary with prefix
std::format("{:o}",    8);        // "10"       octal

std::format("{:.2f}",  3.14159);  // "3.14"     fixed, 2 decimals
std::format("{:.3e}",  31415.9);  // "3.142e+04" scientific
std::format("{:g}",    0.0001);   // "0.0001"   general (shortest)
std::format("{:10.2f}", 3.14159); // "      3.14" width 10, right-aligned default
std::format("{:<10.2f}",3.14159); // "3.14      " left-aligned

// Width and precision can themselves be arguments via nested braces:
std::format("{:{}.{}f}", 3.14159, 8, 2);   // width=8, precision=2 -> "    3.14"
```

The nested-brace form `{:{}.{}f}` takes the width and precision as *arguments*, enabling runtime-computed field sizes — something `printf` does only with the awkward `*` specifier. Booleans format as `true`/`false` by default (or `1`/`0` with `{:d}`), and `char`/string types accept alignment and precision (truncation) specs.

---

## 46.6 format_to and Output Iterators

`std::format` allocates a `std::string`. When you want to write into an existing buffer or container — avoiding the allocation, or appending to a log — use **`std::format_to`** (write to an output iterator) and **`std::format_to_n`** (bounded write).

```cpp
// Listing 46.6: formatting without allocating a fresh string
#include <format>
#include <vector>
#include <string>
#include <iterator>

void append_log(std::string& log, int code) {
    // Append directly into 'log' — no temporary string allocated.
    std::format_to(std::back_inserter(log), "[error {}]\n", code);
}

void into_buffer() {
    char buf[64];
    auto result = std::format_to_n(buf, sizeof(buf), "{}-{}", 42, 7);
    // result.out  -> iterator past the last written char
    // result.size -> number of characters that WOULD have been written
    *result.out = '\0';
}

// Compute the size first, to size a buffer exactly:
std::size_t n = std::formatted_size("{}-{}", 42, 7);   // 4  ("42-7")
```

`format_to` with `std::back_inserter` appends to any container; `format_to_n` writes at most `n` characters into a raw buffer and reports both where it stopped and how much was needed (so you can detect truncation). `std::formatted_size` pre-computes the exact length, letting you allocate once. These are the allocation-conscious entry points for hot logging paths.

---

## 46.7 Custom Formatters for User Types

A user-defined type becomes formattable by **specializing `std::formatter<T>`** with `parse` (reads the spec) and `format` (writes the output). Once specialized, the type works everywhere `std::format` does.

```cpp
// Listing 46.7: making a user type formattable
#include <format>
#include <string>

struct Point { int x, y; };

template<>
struct std::formatter<Point> {
    // parse: examine the spec between the braces; return iterator past it.
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();          // this formatter accepts no custom spec
    }
    // format: write the value through the output iterator.
    auto format(const Point& p, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "({}, {})", p.x, p.y);
    }
};

// Now Point formats like any built-in type:
auto s = std::format("origin = {}", Point{0, 0});   // "origin = (0, 0)"
```

The common shortcut for "format my type by delegating to existing specs" is to **inherit from `std::formatter<std::string>`** (or another built-in formatter) and override only `format`, reusing the inherited `parse` so your type automatically supports width/alignment specs. Specializing `std::formatter` is how the standard library itself makes `std::chrono` types, `std::thread::id`, and others formattable — and the same mechanism makes your domain types first-class in every log line and message.

---

## 46.8 Performance and the C++20/23 Landscape

`std::format` is generally **faster than iostreams** and competitive with `printf`, with the decisive advantage of compile-time checking. Two performance and version facts matter:

- **It still allocates a `std::string` by default.** For the hottest paths, `format_to`/`format_to_n` into a reused buffer avoids per-call allocation, and `formatted_size` lets you size once. Naive repeated `std::format(...)` in a tight loop pays an allocation each iteration.
- **`std::print`/`std::println` are C++23, not C++20.** In C++20 you format then send to a stream yourself: `std::cout << std::format(...)`. The convenient `std::print("{}\n", x)` (which formats directly to `stdout` without an intermediate string, and is faster) arrived in C++23, as did `std::println`.

```cpp
// Listing 46.8: C++20 output vs the C++23 conveniences (flagged)
#include <format>
#include <iostream>

int x = 42;
std::cout << std::format("value = {}\n", x);   // C++20: format then stream
// std::print("value = {}\n", x);              // C++23 ONLY — not available in C++20
// std::println("value = {}", x);              // C++23 ONLY
```

Also C++23: `std::format` gaining `constexpr`-friendliness and ranges formatting (`std::format("{}", a_vector)`), which C++20 does not provide — in C++20 you format container elements yourself.

---

## 46.9 Professional Insights

**Default to `std::format` over both `printf` and iostreams for new code.** It is type-safe like iostreams, terse like `printf`, compile-time-checked unlike either, and extensible to your own types. The compile-time validation alone — turning the `printf("%d", str)` class of stack-corrupting bugs into build errors — justifies the switch in any codebase where correctness matters, and the performance is competitive with `printf`.

**Use `format_to`/`format_to_n` with a reused buffer on hot logging paths.** Plain `std::format` allocates a `std::string` per call; in a high-frequency logger or serializer that allocation dominates. Formatting into a `std::back_inserter` over a pre-sized buffer (or a bounded `format_to_n` into a stack array) eliminates the per-call heap traffic, and `formatted_size` lets you allocate exactly once when you do need a string. This is the difference between acceptable and unacceptable in latency-sensitive logging.

**Specialize `std::formatter` for your domain types early.** Once a type has a formatter, it drops into every log message, error string, and diagnostic with `{}` — no per-call-site conversion code. Inherit from `std::formatter<std::string>` and override only `format` to get width/alignment support for free. Treating formattability as part of a type's public interface pays off across the whole codebase the way `operator<<` once did, but type-safely and faster.

**Know the C++20/23 line: no `std::print`, no ranges formatting, in C++20.** Under a strict C++20 build you write `std::cout << std::format(...)`; `std::print`/`std::println` and direct formatting of containers (`std::format("{}", vec)`) are C++23. Code copied from C++23 examples using `std::print` will not compile against C++20 — the most common surprise. Format-then-stream is the portable C++20 idiom.

**Prefer argument indexing for any user-facing or localized text.** `std::format("{1} {0}", a, b)` lets translators reorder arguments without touching code, and lets one argument appear multiple times. For internal diagnostics automatic `{}` is fine, but anything that may be localized should use explicit indices from the start, since retrofitting indices into a positional format string across a translation catalog is tedious and error-prone.
