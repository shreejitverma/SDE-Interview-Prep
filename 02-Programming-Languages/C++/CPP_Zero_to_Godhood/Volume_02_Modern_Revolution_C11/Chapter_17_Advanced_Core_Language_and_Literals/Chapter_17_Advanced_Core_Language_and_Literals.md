# Chapter 17: Advanced Core Language & Literals

> *The headline features get the attention, but C++11 also shipped a dozen smaller core-language refinements — inheriting constructors, member initializers, explicit conversions, user-defined literals, Unicode, alignment control, and `noexcept` — that together close the gaps a systems programmer hits every day.*

This chapter collects the remaining C++11 core-language features into one reference: the class-authoring conveniences (**inheriting constructors**, **NSDMI**, **explicit conversion operators**), the **`noexcept`** contract that the rest of the library depends on, the **literal** machinery (**user-defined literals**, **raw strings**, **Unicode**), low-level control (**`alignas`/`alignof`**, **unrestricted unions**, **fixed-width integers**), and the quiet usability fixes (**right-angle brackets**, **`extern template`**). Each is small on its own; together they remove a surprising amount of boilerplate and undefined behavior from real code.

---

## Table of Contents

- [17.1 Inheriting Constructors](#171-inheriting-constructors)
- [17.2 Non-Static Data Member Initializers (NSDMI)](#172-non-static-data-member-initializers-nsdmi)
- [17.3 Explicit Conversion Operators](#173-explicit-conversion-operators)
- [17.4 `noexcept`: Specifier and Operator](#174-noexcept-specifier-and-operator)
- [17.5 User-Defined Literals](#175-user-defined-literals)
- [17.6 Raw String Literals](#176-raw-string-literals)
- [17.7 Unicode and New Character Types](#177-unicode-and-new-character-types)
- [17.8 `alignas` and `alignof`](#178-alignas-and-alignof)
- [17.9 Unrestricted Unions](#179-unrestricted-unions)
- [17.10 Fixed-Width Integers and `long long`](#1710-fixed-width-integers-and-long-long)
- [17.11 Usability Fixes: Right-Angle Brackets and `extern template`](#1711-usability-fixes-right-angle-brackets-and-extern-template)
- [17.12 Professional Insights](#1712-professional-insights)

---

## 17.1 Inheriting Constructors

Before C++11, a derived class had to re-declare and forward every base constructor by hand — tedious and error-prone for thin wrappers. C++11's **inheriting constructors** import them with a single `using` declaration.

```cpp
// Listing 17.1: using Base::Base imports the base's constructors
struct Base {
    Base(int);
    Base(int, const std::string&);
    Base(double, double);
};

struct Derived : Base {
    using Base::Base;   // inherit ALL of Base's constructors
    // Derived(int), Derived(int, const std::string&), Derived(double,double)
    // are now usable, each forwarding to the matching Base constructor
};

Derived d(42);                 // calls Base(int)
Derived e(1.0, 2.0);           // calls Base(double, double)
```

The compiler synthesizes a derived constructor for each inherited base constructor; it forwards its arguments to the base and **default-initializes the derived class's own members** (combine with NSDMI — §17.2 — to give those members sensible values). Inherited constructors are not imported if the derived class declares its own constructor with the same signature, and the default/copy/move constructors are never inherited.

> **Use it for:** policy wrappers, strong-typedef classes, and exception hierarchies (`struct my_error : std::runtime_error { using std::runtime_error::runtime_error; };`) where the derived class adds behavior but no new construction logic.

---

## 17.2 Non-Static Data Member Initializers (NSDMI)

C++11 lets you give a data member a **default initializer right where it is declared**. Every constructor that does not explicitly initialize that member uses this value, eliminating the classic bug of forgetting to initialize a field in one of several constructors.

```cpp
// Listing 17.2: members initialized at the point of declaration
class Connection {
    int         timeout_ms = 5000;     // NSDMI
    bool        keep_alive = true;      // NSDMI
    std::string host       = "localhost";
public:
    Connection() = default;             // all members get their NSDMI values
    explicit Connection(std::string h)  // member-init-list overrides the NSDMI
        : host(std::move(h)) {}         // timeout_ms, keep_alive still defaulted
};
```

The rule: a member-initializer-list entry **overrides** the NSDMI; otherwise the NSDMI applies. This makes multi-constructor classes dramatically safer — there is exactly one place each member's default lives. NSDMI works with brace or equals syntax (`int x{0};` or `int x = 0;`) and combines naturally with `= default` and inheriting constructors.

---

## 17.3 Explicit Conversion Operators

C++11 allows the **`explicit`** keyword on **conversion operators**, not just constructors. This fixes the infamous "safe bool" problem: a class that should be testable in a boolean context without being silently convertible to `int` or accidentally comparable.

```cpp
// Listing 17.3: explicit operator bool
class UniqueHandle {
    int fd_ = -1;
public:
    explicit operator bool() const { return fd_ != -1; }  // explicit!
};

UniqueHandle h = open_something();
if (h) { /* OK: contextual conversion to bool is allowed */ }
int x = h;            // ERROR: would require an implicit conversion
bool b = h;           // ERROR: copy-initialization is not a contextual conversion
```

An `explicit` conversion operator participates in **contextual conversions to `bool`** (the conditions of `if`, `while`, `for`, `&&`, `||`, `!`, and the ternary) but is excluded from all *implicit* conversions. This is exactly the behavior `std::unique_ptr`, `std::shared_ptr`, and the stream types use for their truthiness tests, and it retired the awkward pre-C++11 "safe bool idiom" entirely.

---

## 17.4 `noexcept`: Specifier and Operator

`noexcept` is both a **specifier** that declares a function will not throw and an **operator** that queries whether an expression is non-throwing. It replaced the deprecated, runtime-checked dynamic exception specifications (`throw(...)`).

```cpp
// Listing 17.4: the noexcept specifier
void cleanup() noexcept;                 // promises not to throw
void maybe() noexcept(false);            // may throw (the default)

template <class T>
void wrap(T x) noexcept(noexcept(x.foo())); // conditional: noexcept iff x.foo() is
```

If a function marked `noexcept` *does* throw, the runtime calls `std::terminate()` immediately — there is no stack unwinding past the boundary. The **operator** form, `noexcept(expr)`, is a compile-time boolean that is `true` when `expr` is guaranteed not to throw; it is the standard tool for propagating the no-throw property through templates.

**Why it is load-bearing.** `noexcept` is not mere documentation — it changes behavior and performance:

- **Move operations must be `noexcept`** for containers to use them. `std::vector` reallocation uses a moved element's move constructor only if it is `noexcept`; otherwise it falls back to copying to preserve the strong exception guarantee (Chapter 12). A missing `noexcept` on a move constructor silently degrades every `vector<T>` growth to copies.
- The compiler can **omit exception-handling machinery** around a `noexcept` call, producing smaller, faster code.

```cpp
// Listing 17.5: the noexcept move constructor that containers require
class Buffer {
    char* data_;
public:
    Buffer(Buffer&& o) noexcept : data_(o.data_) { o.data_ = nullptr; } // critical
    Buffer& operator=(Buffer&&) noexcept;
    ~Buffer();
};
```

> **Rule:** mark every move constructor, move assignment, destructor (implicitly `noexcept` already), and swap as `noexcept`. Mark leaf functions that genuinely cannot throw. Do not mark a function `noexcept` if it may throw — `terminate()` is a brutal failure mode.

---

## 17.5 User-Defined Literals

**User-defined literals (UDLs)** let you attach a custom suffix to numeric and string literals, dispatching to a `operator""` function. They make domain quantities type-safe and readable — distances, durations, units — turning `3.0` plus a comment into a checked `3.0_km`.

```cpp
// Listing 17.6: a units UDL
constexpr long double operator"" _km(long double x) { return x * 1000.0L; }   // -> metres
constexpr long double operator"" _m (long double x) { return x; }

long double distance = 3.0_km + 250.0_m;   // 3250.0 metres, computed at compile time
```

The standard restricts the parameter types a literal operator may take: a single `unsigned long long` or `long double` (the "cooked" forms that receive the parsed value), `const char*` plus `std::size_t` for string literals, single `char`/`char16_t`/etc. for character literals, or a `const char*` (the "raw" form receiving the literal's characters). User suffixes must begin with an underscore; suffixes without a leading underscore are reserved for the standard library (which uses them for `std::chrono` durations like `10ms` and `std::string` literals like `"x"s` — those arrived in C++14).

```cpp
// Listing 17.7: a string UDL
std::string operator"" _upper(const char* s, std::size_t n) {
    std::string r(s, n);
    for (char& c : r) c = std::toupper((unsigned char)c);
    return r;
}
auto shout = "hello"_upper;   // "HELLO"
```

---

## 17.6 Raw String Literals

A **raw string literal**, `R"(...)"`, disables all escape processing: backslashes, quotes, and newlines are taken literally. This is invaluable for regular expressions (Chapter 15), Windows paths, and embedded code or JSON.

```cpp
// Listing 17.8: raw vs ordinary string
std::string path = "C:\\temp\\new\\file";       // ordinary: doubled backslashes
std::string raw  = R"(C:\temp\new\file)";        // raw: exactly as written

std::regex re(R"((\d{4})-(\d{2})-(\d{2}))");      // no backslash doubling
```

If the content itself contains `)"`, use a **custom delimiter** between the quote and parenthesis — `R"delim( ... )delim"` — where `delim` is any sequence of up to 16 characters:

```cpp
// Listing 17.9: custom delimiter when the text contains )"
auto json = R"json({"key": "value with )" inside"})json";
```

Raw strings combine with the encoding prefixes (§17.7): `u8R"(...)"`, `LR"(...)"`, etc.

---

## 17.7 Unicode and New Character Types

C++11 added two **portable, fixed-width character types** and matching literal prefixes so that UTF-16 and UTF-32 data has a well-defined representation (unlike `wchar_t`, whose size varies by platform).

| Type | Encoding | Literal prefix | Example |
| :--- | :--- | :--- | :--- |
| `char` (UTF-8 bytes) | UTF-8 | `u8"..."` | `u8"héllo"` |
| `char16_t` | UTF-16 code unit | `u"..."` / `u'x'` | `u"text"`, `u'A'` |
| `char32_t` | UTF-32 code unit | `U"..."` / `U'x'` | `U"text"`, `U'\U0001F600'` |

```cpp
// Listing 17.10: the new character types and literals
char        utf8_byte = u8'A';        // (C++11: u8 char literals are C++17; string in C++11)
const char*     u8s  = u8"UTF-8 text";   // UTF-8 encoded bytes
const char16_t* u16s = u"UTF-16 text";   // std::u16string for the owning type
const char32_t* u32s = U"UTF-32 text";   // std::u32string
char32_t        grin = U'\U0001F600';    // a single code point
```

Library support comes via `std::u16string` and `std::u32string`. Universal character names (`\uXXXX`, `\U00XXXXXX`) name code points portably inside any of these literals. The intent is determinism: `char16_t` is exactly a UTF-16 code unit on every platform, so wire formats and text processing no longer depend on `wchar_t` being 16 or 32 bits.

---

## 17.8 `alignas` and `alignof`

C++11 standardized **alignment control**, previously the domain of compiler extensions (`__declspec(align)`, `__attribute__((aligned))`). **`alignof(T)`** queries a type's required alignment; **`alignas(N)`** (or `alignas(T)`) imposes one.

```cpp
// Listing 17.11: querying and imposing alignment
std::cout << alignof(double);          // typically 8
std::cout << alignof(std::max_align_t);

struct alignas(64) CacheLineAligned {  // aligned to a 64-byte cache line
    std::atomic<int> counter;
    char padding[60];
};
alignas(16) float simd_vec[4];          // 16-byte aligned for SSE
```

This is a core systems tool. Aligning a per-thread structure to a **cache line (typically 64 bytes)** prevents *false sharing* (Chapter 16) — the situation where two cores fight over a line holding unrelated variables. Alignment also satisfies the requirements of SIMD loads/stores and DMA buffers. `alignas` may only *strengthen* (increase) alignment; it cannot weaken a type's natural alignment.

---

## 17.9 Unrestricted Unions

Pre-C++11 unions could hold only trivial types — no member with a non-trivial constructor, destructor, or assignment. C++11 lifted this: a union may contain **any type**, at the cost of the programmer taking over lifetime management for the non-trivial members.

```cpp
// Listing 17.12: a union with a non-trivial member
union Value {
    int          i;
    double        d;
    std::string   s;     // non-trivial — legal in C++11

    Value() {}           // must be user-provided
    ~Value() {}          // must be user-provided; does NOT auto-destroy s
};

Value v;
new (&v.s) std::string("hello");   // placement-new to construct the active member
// ... use v.s ...
v.s.~basic_string();               // explicit destruction before switching members
```

When a union has a non-trivial member, its special member functions are implicitly **deleted** unless you provide them, and you are responsible for constructing the active member with **placement new** and destroying it explicitly. This feature exists to build *discriminated unions* (tagged variants) — track which member is active with a separate tag and dispatch construction/destruction accordingly. It is the low-level machinery beneath `std::variant` (C++17); in C++11 you write the bookkeeping yourself.

---

## 17.10 Fixed-Width Integers and `long long`

C++11 guarantees **`long long`** (and `unsigned long long`) — an integer type of **at least 64 bits** — promoting the long-standing compiler extension into the standard. More importantly, it adopted C's **`<cstdint>`** fixed-width integer types, giving exact, portable control over integer size and representation.

```cpp
// Listing 17.13: portable, exact-width integers
#include <cstdint>
long long      big  = 9'000'000'000LL;   // at least 64 bits ('LL' suffix)
std::int32_t   i32  = -1;                  // exactly 32 bits, two's complement
std::uint64_t  u64  = 0xFFFFFFFFFFFFFFFF;  // exactly 64 bits, unsigned
std::int_fast16_t fast;                    // fastest type with >= 16 bits
std::int_least8_t least;                   // smallest type with >= 8 bits
std::intptr_t  as_int = reinterpret_cast<std::intptr_t>(&big); // holds a pointer
```

`<cstdint>` provides exact-width (`int8_t` … `int64_t`), fastest-minimum-width (`int_fast*_t`), smallest-minimum-width (`int_least*_t`), and pointer-sized (`intptr_t`/`uintptr_t`) families, plus `intmax_t`. For wire formats, hardware registers, and binary protocols — where a field is *defined* as 32 bits — these types replace the non-portable assumption that `int` is 32 bits or `long` is 64. (Digit separators like `9'000'000'000` shown above are a C++14 readability addition.)

---

## 17.11 Usability Fixes: Right-Angle Brackets and `extern template`

**The right-angle bracket fix.** Before C++11, two closing template brackets in a row were lexed as the `>>` right-shift operator, forcing a space:

```cpp
// Listing 17.14: nested templates no longer need a space
std::vector<std::vector<int>> grid;   // C++11: fine
// std::vector<std::vector<int> >     // pre-C++11: the space was mandatory
```

The parser now correctly closes nested template-argument lists, removing a decade of confusing error messages.

**`extern template`.** A template used in many translation units is implicitly instantiated in each, then the linker discards the duplicates — wasting compile time. An **`extern template` declaration** suppresses implicit instantiation in a translation unit, promising the instantiation exists elsewhere; a matching **explicit instantiation definition** provides it once.

```cpp
// Listing 17.15: control template instantiation across TUs
// in a header:
extern template class std::vector<MyHeavyType>;   // "do NOT instantiate here"

// in exactly one .cpp:
template class std::vector<MyHeavyType>;            // instantiate once, here
```

For heavily-used instantiations (a common container of a complex type, a frequently-used trait), this measurably cuts compile time and object-file bloat in large codebases — directly relevant when build times dominate developer iteration.

---

## 17.12 Professional Insights

**`noexcept` on moves is not optional.** The single most consequential item in this chapter: an un-`noexcept`'d move constructor causes `std::vector` and friends to fall back to copying on reallocation, silently erasing the performance benefit of move semantics. Audit every move constructor and move assignment for `noexcept`; it is correctness-adjacent, not stylistic.

**Use `alignas` to kill false sharing deliberately.** In any multithreaded data structure with per-thread or per-core state, align hot, independently-written fields to a cache line (`alignas(64)`). This is one of the highest-leverage, lowest-effort scaling fixes in concurrent systems code (Chapter 16).

**Reach for `<cstdint>` at every external boundary.** Wire protocols, file formats, mmap'd structures, and hardware interfaces must use exact-width types (`uint32_t`, not `unsigned`). Reserve `int`/`long` for local arithmetic where the platform's natural width is fine; never assume their size in a serialized layout.

**`explicit operator bool` is the right truthiness idiom.** Any resource-handle or optional-like type should expose `explicit operator bool` rather than an implicit conversion — it enables `if (handle)` while blocking the accidental `int n = handle;` that an implicit conversion would allow.

**Inheriting constructors + NSDMI make thin wrappers free.** Together they let strong-typedef, policy, and exception classes inherit construction and default their own members with zero boilerplate — prefer this combination to hand-written forwarding constructors.

**Treat unrestricted unions as a primitive, not a tool.** Manual placement-new/explicit-destruction bookkeeping is error-prone; use it only to *build* tagged-variant abstractions, and prefer `std::variant` (C++17) once available. In C++11, encapsulate the union behind a class that owns the active-member discipline.
