# Chapter 61: Modern Output — `std::print`, `std::println`, and Range Formatting

> C++20 gave us `std::format`, a fast, type-safe formatting engine — and then made us write `std::cout << std::format(...)` to actually get the text onto the screen, defeating much of the point. C++23 finishes the job with **`std::print`** and **`std::println`**, which format *and* write in one call, directly to the output's underlying file descriptor, with no intermediate `std::string` allocation. They are type-safe like `printf` never was, fast like `iostream` never was, and Unicode-correct on the console. This chapter also covers the C++23 addition that makes them shine on containers: native **range and tuple formatting**.

## Table of Contents

1. [The Output Half That C++20 Left Unfinished](#611-the-output-half-that-c20-left-unfinished)
2. [`std::print` and `std::println`](#612-stdprint-and-stdprintln)
3. [Writing to Streams, Files, and `stderr`](#613-writing-to-streams-files-and-stderr)
4. [The Format Spec Is `std::format`'s Spec](#614-the-format-spec-is-stdformats-spec)
5. [Range and Container Formatting](#615-range-and-container-formatting)
6. [`std::vprint` and Building Print-Like APIs](#616-stdvprint-and-building-print-like-apis)
7. [Performance and Unicode Correctness](#617-performance-and-unicode-correctness)
8. [Professional Insights](#618-professional-insights)

---

## 61.1 The Output Half That C++20 Left Unfinished

`std::format` (Volume 5) solved formatting: it is type-safe, compile-time-checked, and fast. But it returns a `std::string`. To display that string you reached for `std::cout`, which means you paid for (a) a heap-allocated temporary `string` to hold the formatted result, and (b) the `iostream` machinery — locale imbuing, sentry construction, `sync_with_stdio` overhead — that makes `cout` notoriously slow. The idiom `std::cout << std::format("{}\n", x);` works, but it is verbose and combines the costs of both subsystems.

The two pre-C++23 output tools were each broken in a different way:

- **`printf`** is fast and concise but *not type-safe*: a format-string/argument mismatch is undefined behavior the compiler historically could not catch, and it knows nothing of user-defined types.
- **`std::cout`** is type-safe but slow, verbose (`<< x << " " << y`), and entangled with locale and stream state.

`std::print` resolves the dilemma: the conciseness and speed of `printf` with the type safety of `format`, and no temporary string.

---

## 61.2 `std::print` and `std::println`

Declared in `<print>`, the two core functions are:

- **`std::print(fmt, args...)`** — formats `args` according to `fmt` and writes the result to `stdout`, with no trailing newline.
- **`std::println(fmt, args...)`** — the same, plus a trailing `'\n'`. (There is also a no-argument `std::println()` that just emits a newline.)

```cpp
#include <print>

int main() {
    int id = 42;
    std::string name = "Alice";

    std::println("User {} has ID {}", name, id);   // User Alice has ID 42
}
```

The format string is checked **at compile time**, exactly as in `std::format`: a mismatch between the placeholders and the argument types is a compilation error, not a runtime surprise or silent garbage. This alone retires `printf` for new code.

**Listing 61.1: `print` versus `println` and the no-arg newline.**

```cpp
#include <print>

int main() {
    std::print("no newline here -> ");
    std::print("still same line\n");
    std::println("this line ends itself");
    std::println();                       // just a blank line
    std::println("user {} scored {}", "bob", 99);
}
```

---

## 61.3 Writing to Streams, Files, and `stderr`

The first argument may be a destination. Both functions accept either a C `FILE*` or a `std::ostream&` as an optional leading argument:

- **`std::println(stderr, "Error: {}", msg)`** — write to standard error.
- **`std::print(some_file_ptr, ...)`** — write to an open `std::FILE*`.
- **`std::println(ofstream_obj, "...")`** — write to a `std::ostream` (e.g. an `std::ofstream`), bridging the new API to existing stream-based code.

**Listing 61.2: Directing output to `stderr` and to a file stream.**

```cpp
#include <print>
#include <fstream>

int main() {
    std::println(stderr, "Error: connection failed");   // to standard error

    std::ofstream log("log.txt");
    std::println(log, "Error code: {}", 500);           // to an ostream
}
```

The `ostream` overload means you do not have to rewrite a codebase's stream plumbing to adopt `print`; you can format with the new engine while still targeting an `std::ofstream`, `std::ostringstream`, or any other stream you already have.

---

## 61.4 The Format Spec Is `std::format`'s Spec

`print`/`println` use the **identical** format mini-language as `std::format` (covered in depth in Volume 5), so everything you know transfers directly: positional/automatic argument indexing, fill and alignment, sign, `#` alternate form, width and precision, and type presentation specifiers.

```cpp
#include <print>

int main() {
    std::println("Pi: {:.2f}", 3.14159);      // Pi: 3.14
    std::println("Hex: {:#x}", 255);          // Hex: 0xff
    std::println("{1} before {0}", "a", "b"); // b before a
    std::println("{:>8}", "right");           //    right (width 8, right-aligned)
}
```

Because the spec is shared, a custom `std::formatter<T>` specialization you wrote for `std::format` works unchanged with `std::print` — the same type extensibility, the same syntax, now with direct output.

---

## 61.5 Range and Container Formatting

A major C++23 ergonomic addition — and the reason `print` feels modern — is that the formatting library now understands **ranges, containers, tuples, and pairs** natively. You no longer write a loop to print a vector; you format it directly.

```cpp
#include <print>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3};
    std::println("{}", v);            // [1, 2, 3]
}
```

The defaults are sensible and composable:

- **Sequences** (`vector`, `array`, any range) format as `[a, b, c]`.
- **Associative containers** (`map`, `set`) format as `{...}`; a `map` shows `{key: value, ...}`.
- **Tuples and pairs** format as `(a, b)`.
- **Nesting works**: a `vector<pair<int,string>>` formats each element recursively.

C++23 also adds **range format specifiers** to control this output. The `n` specifier drops the brackets, and `:` introduces a per-element format applied to every element of the range.

**Listing 61.3: Default and customized range formatting.**

```cpp
#include <print>
#include <vector>
#include <map>

int main() {
    std::vector<int> v{1, 2, 3};
    std::println("default : {}",    v);        // [1, 2, 3]
    std::println("no-bracket: {:n}", v);       // 1, 2, 3
    std::println("hex elems: {::#x}", v);      // [0x1, 0x2, 0x3]  (spec after the 2nd colon)

    std::map<std::string, int> m{{"a", 1}, {"b", 2}};
    std::println("{}", m);                     // {"a": 1, "b": 2}
}
```

In the `{::#x}` form, the part after the *second* colon (`#x`) is the format spec applied to each element — here, hexadecimal — while the outer range formatter supplies the brackets and separators. This makes one-line debugging output of complex containers genuinely pleasant, replacing the hand-written print loops that cluttered C++20 diagnostics.

---

## 61.6 `std::vprint` and Building Print-Like APIs

When you write your *own* logging or output function that forwards a runtime format string and a variable set of arguments, you cannot use the compile-time-checked `std::print` directly (the format string is not a constant expression at your wrapper's call site). The type-erased primitives `std::vprint_unicode` and `std::vprint_nonunicode`, together with `std::make_format_args`, are the building blocks — the same relationship `std::vformat` has to `std::format`.

**Listing 61.4: A thin logging wrapper built on `vprint`.**

```cpp
#include <print>
#include <format>
#include <string_view>

template <typename... Args>
void log_line(std::format_string<Args...> fmt, Args&&... args) {
    std::print("[log] ");
    std::println(fmt, std::forward<Args>(args)...);   // still compile-time checked
}

// When the format string is only known at runtime, drop to the vprint layer:
void log_runtime(std::string_view fmt, std::format_args args) {
    std::vprint_unicode(stdout, fmt, args);
}

int main() {
    log_line("user {} connected from {}", "ann", "10.0.0.1");
    log_runtime("raw {} value\n", std::make_format_args(123));
}
```

Prefer the `std::format_string<Args...>` wrapper (as in `log_line`) so that your API keeps compile-time checking; reach for `vprint` only when the format string is genuinely dynamic.

> **Version-trap flag:** `std::print`, `std::println`, `std::vprint_unicode`/`std::vprint_nonunicode`, and the range/tuple formatting specializations are all **C++23**, in `<print>` and `<format>`. C++20 had `std::format`/`std::vformat` but no `std::print` and *no* built-in formatting for ranges or tuples — printing a `vector` in C++20 required a manual loop or a custom formatter.

---

## 61.7 Performance and Unicode Correctness

`std::print`'s speed comes from what it *avoids*:

- **No intermediate `std::string`.** `print` formats into a buffer and writes that buffer to the destination's underlying file handle directly. The `std::cout << std::format(...)` idiom, by contrast, allocates a `string`, copies the formatted bytes into it, then streams it out.
- **No `iostream` tax.** Writing to `stdout` via `print` bypasses the locale-imbuing, sentry, and `sync_with_stdio` overhead that makes `std::cout` slow; it is frequently faster than `printf` as well, because the formatting is resolved without `printf`'s runtime format-string parsing of every call.
- **Unicode-aware output.** On a Unicode-capable destination (notably the Windows console), `std::print` uses `vprint_unicode` to transcode correctly to the terminal's encoding, so non-ASCII text renders properly — something neither `printf` nor naive `cout` byte-writing guarantees. The `nonunicode` variant exists for destinations where you explicitly do not want transcoding.

For a logging-heavy service or a tool that emits large volumes of formatted text, switching the hot output paths from `cout`/`printf` to `print` is a measurable, low-risk win — fewer allocations, less per-call overhead, and the compile-time safety as a bonus.

---

## 61.8 Professional Insights

**Make `std::print`/`std::println` the default for all new output, and retire both `printf` and `cout`.** They are simultaneously safer than `printf` (compile-time-checked, type-aware, extensible to your own types) and faster than `cout` (no temporary string, no stream tax, Unicode-correct). There is essentially no category of console or stream output where one of the older tools is the better choice in C++23 code, which is rare clarity in an API migration decision.

**Lean on range and tuple formatting to make diagnostics one-liners.** The single biggest day-to-day quality-of-life change is `std::println("{}", container)`. The hand-written `for` loops that printed vectors and maps in C++20 — each a small opportunity for an off-by-one or a missing separator — collapse to one expression, and the per-element spec (`{::...}`) lets you control precision or base without unrolling the loop. This dramatically lowers the cost of adding debug output, which tends to improve how much of it people actually write.

**Keep compile-time checking by passing format strings as `std::format_string`, and reserve `vprint` for genuinely dynamic formats.** When you build a logging facade, forward the format string through `std::format_string<Args...>` so the checking propagates to your callers; dropping to `vprint_unicode` with `make_format_args` discards that safety and should be confined to the rare case where the format string is loaded at runtime (from config, a translation table, etc.). Getting this boundary right is what separates a safe logging wrapper from one that reintroduces `printf`-style format bugs.

**Prefer the `FILE*`/`ostream` overloads to integrate incrementally.** You do not need to rip out a codebase's existing stream plumbing to benefit from `print`. Because `println(ostream, ...)` and `println(FILE*, ...)` target the destinations you already have, you can adopt the faster, safer formatting engine file-by-file while leaving the surrounding I/O architecture untouched — the low-friction path to migrating a large codebase off `cout`.
