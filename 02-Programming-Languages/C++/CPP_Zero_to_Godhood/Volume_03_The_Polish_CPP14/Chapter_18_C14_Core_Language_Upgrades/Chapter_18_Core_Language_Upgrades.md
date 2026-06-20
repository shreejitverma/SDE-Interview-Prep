# Chapter 18: Core Language Upgrades

> *C++11 was the revolution; C++14 is the refinement. This chapter covers the core-language polish — a `constexpr` that finally behaves like a programming language, readable numeric literals, aggregate initialization that respects default members, faster deallocation, and a standard way to retire old APIs.*

C++14 added no grand new paradigm. Instead it removed the friction that made C++11 feel unfinished: `constexpr` gained loops and mutation, literals gained binary and digit-separator syntax, aggregates regained brace initialization when they carry default member initializers, the standard finally let user-defined literals live in the library, deallocation got a size hint, and `[[deprecated]]` gave a portable way to mark code for removal. Each is small; together they make compile-time and systems code markedly cleaner.

---

## Table of Contents

- [18.1 Relaxed `constexpr`](#181-relaxed-constexpr)
- [18.2 Binary Literals and Digit Separators](#182-binary-literals-and-digit-separators)
- [18.3 Standard-Library User-Defined Literals](#183-standard-library-user-defined-literals)
- [18.4 Aggregate Member Initialization](#184-aggregate-member-initialization)
- [18.5 Sized Deallocation](#185-sized-deallocation)
- [18.6 The `[[deprecated]]` Attribute](#186-the-deprecated-attribute)
- [18.7 Professional Insights](#187-professional-insights)

---

## 18.1 Relaxed `constexpr`

In C++11 a `constexpr` function was effectively a single `return` expression — no local variables, no loops, no branches beyond the ternary operator. Every non-trivial compile-time computation had to be expressed recursively. **C++14 relaxed these rules**, allowing imperative control flow inside `constexpr` functions and turning compile-time programming from a puzzle into ordinary code.

### 18.1.1 The C++11 vs. C++14 Paradigm Shift

The same algorithm, recursive in C++11 and iterative in C++14:

```cpp
// Listing 18.1: recursive (C++11) vs iterative (C++14) constexpr
// C++11: functional/recursive — harder to read, deep compile-time recursion
constexpr int fib11(int n) {
    return (n <= 1) ? n : fib11(n - 1) + fib11(n - 2);
}

// C++14: imperative/iterative — readable, efficient, familiar
constexpr int fib14(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}
```

The iterative version is not just more readable — it avoids the exponential instantiation blow-up of the naive recursive form and compiles faster.

### 18.1.2 What Is Now Permitted

Inside a C++14 `constexpr` function you may use:

- **Local variable declarations** (but not `static` or `thread_local`, which would imply runtime storage).
- **Branching:** `if`, `switch`.
- **Loops:** `for`, `while`, `do-while`.
- **Mutation** of local variables.
- **Multiple `return` statements** (all must yield the same deduced type).

### 18.1.3 What Remains Forbidden

Even in C++14, a `constexpr` function still cannot:

1. Call a non-`constexpr` function.
2. Allocate dynamic memory (lifted in C++20).
3. Throw — though a `throw` may appear in a branch never taken during a constant evaluation.
4. Use `goto`, `asm`, or a `try` block.

> **Godhood tip:** make any computation that *can* be done at compile time `constexpr`. It removes work from the runtime and lets the compiler fold the result into an immediate operand — lookup tables, bit masks, protocol constants, and hashing of fixed strings all belong at compile time in latency-critical code.

---

## 18.2 Binary Literals and Digit Separators

C++14 added two literal conveniences that matter most to systems and embedded engineers.

**Binary literals** use the `0b` (or `0B`) prefix, letting a bitmask map directly to the hardware register it represents instead of forcing a mental hex translation.

**Digit separators** use a single quote (`'`) that may appear between any two digits of any numeric literal — integer, floating-point, hex, or binary — and is purely visual; the compiler ignores it.

```cpp
// Listing 18.2: binary literals and digit separators
#include <cstdint>

// Hex required a mental hex->binary mapping:
std::uint8_t mask_old = 0x2A;

// Binary literal maps 1:1 onto the hardware register layout:
std::uint8_t mask_new = 0b0010'1010;       // separator groups nibbles

// Separators work in any base and in floating point:
constexpr long double PLANCK   = 6.626'070'15e-34;
constexpr std::uint64_t MAX_U64 = 0xFF'FF'FF'FF'FF'FF'FF'FF;
constexpr int          MILLION  = 1'000'000;
```

The separator placement is unconstrained — `0b1'0'1` is legal — but the idiom is to group by the natural unit of the base (nibbles for hex/binary, thousands for decimal).

---

## 18.3 Standard-Library User-Defined Literals

C++11 introduced *user-defined* literals (the `operator""` machinery, Volume 2); C++14 is the first standard to *use* them, shipping suffix literals for durations, strings, and complex numbers. Suffixes without a leading underscore are reserved for the standard library — these are them. Each lives in an inline namespace you opt into with `using namespace`.

| Literal | Suffix(es) | Namespace | Result type |
|---------|-----------|-----------|-------------|
| Chrono durations | `h` `min` `s` `ms` `us` `ns` | `std::chrono_literals` | `std::chrono::duration` |
| `std::string` | `s` | `std::string_literals` | `std::string` |
| `std::complex` | `i` `if` `il` | `std::complex_literals` | `std::complex<double/float/long double>` |

```cpp
// Listing 18.3: chrono and string literals
#include <chrono>
#include <string>
using namespace std::chrono_literals;   // brings in h, min, s, ms, us, ns
using namespace std::string_literals;   // brings in the 's' string suffix

auto timeout  = 250ms;                   // std::chrono::milliseconds, no verbosity
auto interval = 2h + 30min;              // duration arithmetic, type-checked
std::this_thread::sleep_for(50us);

auto greeting = "hello"s;                // std::string, NOT const char* —
auto combined = "a"s + "b";              // operator+ works because LHS is std::string
```

```cpp
// Listing 18.4: complex literals
#include <complex>
using namespace std::complex_literals;

auto z  = 2.0 + 1i;     // std::complex<double>{2.0, 1.0}
auto zf = 3.0f + 2if;   // std::complex<float>
```

The `s` suffix is overloaded by context: on a string literal it yields `std::string`; on an integer or floating literal (with `chrono_literals` in scope) it yields `std::chrono::seconds`. Because the suffix is selected by the literal's type, the two never collide.

> **Note:** the `s` string suffix matters for correctness, not just brevity — `"a"s + "b"` compiles (string concatenation) whereas `"a" + "b"` is an illegal attempt to add two pointers. Prefer the literal in any expression doing string arithmetic.

---

## 18.4 Aggregate Member Initialization

In C++11, giving a class a **default member initializer** (an NSDMI, e.g. `int x = 0;`) disqualified it from being an *aggregate* — so you lost brace (aggregate) initialization. This forced an unpleasant choice between default members and aggregate syntax. **C++14 removed the restriction:** an aggregate may now have default member initializers and still be brace-initialized.

```cpp
// Listing 18.5: aggregates with default member initializers (C++14)
struct Config {
    int  timeout_ms = 5000;   // default member initializers...
    int  retries    = 3;
    bool verbose    = false;
};
// In C++11 the NSDMIs above made Config a non-aggregate. In C++14 it stays one:

Config a{};            // all defaults -> {5000, 3, false}
Config b{1000};        // timeout_ms=1000, retries=3, verbose=false (defaults fill in)
Config c{1000, 5};     // timeout_ms=1000, retries=5, verbose=false
Config d{1000, 5, true};
```

The rule is intuitive: members listed in the braces are initialized positionally; members beyond the list fall back to their default member initializer (or are value-initialized if they have none). This makes plain-data configuration and descriptor structs — common in systems code for protocol headers and option blocks — both safe-by-default and brace-initializable, with no constructor boilerplate.

---

## 18.5 Sized Deallocation

C++14 added a **global sized deallocation** function: an `operator delete` overload that receives the size of the object being freed.

```cpp
// Listing 18.6: the sized-deallocation signatures C++14 made callable
void operator delete(void* p, std::size_t size) noexcept;
void operator delete[](void* p, std::size_t size) noexcept;
```

Before C++14, the global `operator delete` received only the pointer; an allocator that needed the block size had to store it alongside every allocation or look it up in metadata. With sized deallocation, the compiler — which knows the static type and therefore the size at the `delete` site — passes the size directly, letting a size-class allocator (the tcmalloc/jemalloc family) route the free to the correct bucket **without a size lookup**.

```cpp
// Listing 18.7: a custom allocator exploiting the size hint
#include <cstddef>
#include <cstdlib>

void operator delete(void* p, std::size_t size) noexcept {
    // 'size' lets us pick the right free-list bucket directly,
    // avoiding the header read a pointer-only delete would require.
    size_class_free(p, size);
}
```

The unsized `operator delete(void*)` still exists and is still called when the size is genuinely unknown (e.g. deleting through a base pointer with a virtual destructor where the dynamic type's size is recovered differently). For allocation-heavy, low-latency systems, providing the sized overload removes a per-deallocation memory read — a small but real win on hot paths.

---

## 18.6 The `[[deprecated]]` Attribute

C++14 standardized **`[[deprecated]]`**, a portable attribute that marks an entity as obsolete and makes the compiler emit a warning at each use — replacing the non-portable `#pragma` and compiler-specific `__attribute__((deprecated))`/`__declspec(deprecated)`.

```cpp
// Listing 18.8: deprecating functions, types, and namespaces
namespace [[deprecated("use APIv2")]] LegacyAPI {
    struct [[deprecated]] OldData { int x; };
}

class Database {
public:
    [[deprecated("use execute(Query&&) for better performance")]]
    void runRawSQL(const char* sql);

    void execute(Query&& q);
};
```

The attribute applies to functions, classes, type aliases, variables, non-static data members, enumerators, and namespaces. The optional string argument is shown in the diagnostic — use it to point callers at the replacement. Because it is part of the language, the warning fires during semantic analysis exactly when the deprecated entity is named, across every conforming compiler — making it a reliable tool for staged API migrations in a large codebase.

---

## 18.7 Professional Insights

**Push logic to compile time aggressively.** Relaxed `constexpr` removes the last excuse for runtime computation of constants. Bit masks, CRC tables, fixed-string hashes, and unit conversions written as iterative `constexpr` functions evaluate during compilation and fold into immediates — zero runtime cost and better downstream optimization. Verify the evaluation actually happened at compile time by assigning the result to a `constexpr` variable.

**Binary literals are a correctness tool, not just cosmetics.** When a value *is* a hardware register layout or a bit-flag set, write it in binary with nibble-grouped separators. The literal then visually matches the datasheet, and review catches a wrong bit at a glance — far harder with `0x2A`.

**Provide sized deallocation in custom allocators.** If you override global `operator delete` for a size-class or arena allocator, supply the sized overload. It eliminates a per-free metadata read that the pointer-only form requires — measurable in allocation-bound, latency-sensitive services.

**Prefer aggregates with default members for plain data.** Configuration blocks, descriptors, and protocol option structs are clearer as C++14 aggregates with NSDMIs than as classes with hand-written constructors: brace-initializable, safe by default, and free of boilerplate.

**Treat `[[deprecated]]` as the first step of every removal.** Mark, ship a release with the warning, then delete. The string argument naming the replacement turns a build warning into actionable migration guidance and is the cheapest way to drive a deprecation across many call sites.
