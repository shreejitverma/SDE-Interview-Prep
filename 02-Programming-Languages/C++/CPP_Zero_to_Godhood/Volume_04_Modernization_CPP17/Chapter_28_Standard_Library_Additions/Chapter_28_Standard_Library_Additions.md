# Chapter 28: Standard Library Additions

> *Beyond the headline vocabulary types and parallel algorithms, C++17 scattered a set of smaller but high-value utilities across the standard library: a real byte type, new clamping and number-theory algorithms, an enormous catalogue of special mathematical functions, the blazingly fast `<charconv>` conversions, free-function container accessors, and the subtle correctness tools `launder`, `as_const`, and `uncaught_exceptions`. Individually modest, together they remove a long list of "why isn't this in the standard?" gaps.*

This chapter collects the C++17 additions that don't belong to a single large theme but each pull their weight in real code. `std::byte` finally gives raw-memory code a type the compiler won't let you accidentally do arithmetic on. `std::to_chars`/`from_chars` give you locale-independent, allocation-free, round-trip-exact number conversion — the fastest the standard offers, and the right tool for parsing and formatting in hot paths. The free-function `std::size`/`empty`/`data` complete the generic-accessor family `std::begin`/`end` started. And `std::launder`, `std::as_const`, and `std::uncaught_exceptions()` are the precision instruments you reach for in low-level and exception-aware code.

---

## Table of Contents

- [28.1 `std::byte`](#281-stdbyte)
- [28.2 New Algorithms: `clamp`, `gcd`, `lcm`, `sample`](#282-new-algorithms-clamp-gcd-lcm-sample)
- [28.3 Mathematical Special Functions](#283-mathematical-special-functions)
- [28.4 Elementary String Conversions (`<charconv>`)](#284-elementary-string-conversions-charconv)
- [28.5 Non-Member `size`, `empty`, and `data`](#285-non-member-size-empty-and-data)
- [28.6 `std::as_const`](#286-stdas_const)
- [28.7 `std::launder`](#287-stdlaunder)
- [28.8 `std::uncaught_exceptions`](#288-stduncaught_exceptions)
- [28.9 Searchers: `default_searcher`, `boyer_moore_searcher`, `boyer_moore_horspool_searcher`](#289-searchers-default_searcher-boyer_moore_searcher-boyer_moore_horspool_searcher)
- [28.10 Professional Insights](#2810-professional-insights)

---

## 28.1 `std::byte`

`std::byte` (header `<cstddef>`) is a distinct type for **byte-oriented memory access**. Unlike `char` or `unsigned char`, it is **not an arithmetic type**: you cannot accidentally add, multiply, or stream it as a number. It supports only the operations that make sense on raw bytes — the bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`) — which is exactly what you want when manipulating a buffer's bits rather than its characters.

```cpp
// Listing 28.1: std::byte models raw memory, not a number
#include <cstddef>

std::byte b = std::byte{0xAB};

// b += 1;                       // ERROR: std::byte is not arithmetic
b = b | std::byte{0x01};         // OK: bitwise ops are allowed
b <<= 1;                         // OK

// Explicit conversion to/from an integer is required:
int i = std::to_integer<int>(b); // 0xAB as int
```

The win is **intent and safety**. A `char*` buffer invites accidental arithmetic and sign-extension bugs; a `std::byte*` buffer expresses "these are bytes, not text or numbers," and the type system enforces that distinction. Conversion to a numeric type is deliberately explicit via `std::to_integer<T>` and construction from one via `std::byte{value}`, so every crossing between the byte world and the number world is visible in the code.

---

## 28.2 New Algorithms: `clamp`, `gcd`, `lcm`, `sample`

C++17 adds four broadly useful algorithms.

**`std::clamp(v, lo, hi)`** (header `<algorithm>`) constrains a value to a range, returning `lo` if `v < lo`, `hi` if `v > hi`, and `v` otherwise — replacing the error-prone `std::max(lo, std::min(v, hi))` idiom:

```cpp
// Listing 28.2: clamp constrains a value to [lo, hi]
#include <algorithm>

int x = std::clamp(10, 0, 5);     // 5  (10 > hi)
int y = std::clamp(-3, 0, 5);     // 0  (-3 < lo)
int z = std::clamp(3, 0, 5);      // 3  (in range)
```

**`std::gcd(a, b)`** and **`std::lcm(a, b)`** (header `<numeric>`) compute the greatest common divisor and least common multiple of two integers — previously hand-rolled in every codebase:

```cpp
// Listing 28.3: gcd and lcm
#include <numeric>

int g = std::gcd(12, 18);         // 6
int l = std::lcm(4, 6);           // 12
```

**`std::sample`** (header `<algorithm>`) selects `n` elements at random, without replacement, from a range, writing them to an output iterator using a supplied random engine — the standard way to take a uniform random subset:

```cpp
// Listing 28.4: sample draws a random subset
#include <algorithm>
#include <random>
#include <vector>
#include <iterator>

std::vector<int> population(100);
std::vector<int> chosen;
std::sample(population.begin(), population.end(),
            std::back_inserter(chosen),
            5, std::mt19937{std::random_device{}()});   // 5 random elements
```

---

## 28.3 Mathematical Special Functions

C++17 folds a large catalogue of **mathematical special functions** into `<cmath>` — the functions of mathematical physics and engineering that previously required Boost or a numeric library. These include the Bessel functions (`std::cyl_bessel_j`, `std::cyl_bessel_k`, `std::sph_bessel`), Legendre and associated Legendre polynomials (`std::legendre`, `std::assoc_legendre`), Laguerre polynomials (`std::laguerre`, `std::assoc_laguerre`), Hermite polynomials (`std::hermite`), the elliptic integrals (`std::ellint_1`, `std::ellint_2`, `std::ellint_3`), the error/gamma family extensions (`std::beta`, `std::riemann_zeta`, `std::expint`), and more.

```cpp
// Listing 28.5: special functions now live in <cmath>
#include <cmath>

double z  = std::riemann_zeta(2.0);     // pi^2 / 6
double j0 = std::cyl_bessel_j(0, 1.0);  // Bessel function J_0(1)
double p  = std::legendre(3, 0.5);      // Legendre polynomial P_3(0.5)
double b  = std::beta(2.0, 3.0);        // Beta function B(2, 3)
```

For scientific, signal-processing, and quantitative-finance code, having these standardized — with defined accuracy requirements — removes a dependency and makes numerically heavy code portable.

---

## 28.4 Elementary String Conversions (`<charconv>`)

`std::to_chars` and `std::from_chars` (header `<charconv>`) are **low-level, allocation-free, locale-independent** conversions between numbers and their text representations. They are the fastest conversions the standard provides, and unlike `sprintf`/`stringstream`/`stoi` they perform no allocation, consult no locale, and (for the round trip) are exact.

```cpp
// Listing 28.6: to_chars and from_chars over a caller-provided buffer
#include <charconv>

char buffer[16];
int value = 42;

// number -> text: writes into [buffer, buffer+16), no allocation
auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);
if (ec == std::errc{}) {
    // [buffer, ptr) now holds "42"
}

// text -> number: parses from the buffer, no locale, no allocation
int parsed;
auto [p2, ec2] = std::from_chars(buffer, buffer + sizeof(buffer), parsed);
if (ec2 == std::errc{}) {
    // parsed == 42
}
```

Both return a small struct: `to_chars_result` carries a pointer to one-past-the-last-written character and an `errc` (set to `errc::value_too_large` if the buffer was too small); `from_chars_result` carries a pointer to one-past-the-last-consumed character and an `errc` (`errc::invalid_argument` or `errc::result_out_of_range` on failure). No exceptions are thrown — failure is reported in the result.

Because they ignore the locale and never allocate, `to_chars`/`from_chars` are the correct primitives for **serialization, parsing protocols, and any latency-sensitive number formatting**, where `std::stringstream`'s locale lookups and allocations are unacceptable overhead and `std::stoi`'s exceptions are unwelcome. The floating-point overloads additionally provide the shortest round-trippable representation.

---

## 28.5 Non-Member `size`, `empty`, and `data`

C++17 completes the free-function container-accessor family begun by C++11's `std::begin`/`std::end`. **`std::size(c)`**, **`std::empty(c)`**, and **`std::data(c)`** (header `<iterator>`) work uniformly on standard containers, `std::initializer_list`, and built-in arrays — so generic code need not assume member functions that raw arrays lack.

```cpp
// Listing 28.7: free-function accessors work on containers AND raw arrays
#include <iterator>
#include <vector>

std::vector<int> v{1, 2, 3};
int arr[5] = {};

std::size_t n1 = std::size(v);      // 3
std::size_t n2 = std::size(arr);    // 5 — and it's a compile-time constant
bool e        = std::empty(v);      // false
int* p        = std::data(arr);     // pointer to the first element
```

`std::size` on a built-in array yields a `constexpr` value, making it a safer replacement for the classic `sizeof(arr)/sizeof(arr[0])` trick (which silently misbehaves on a decayed pointer). In templates, preferring `std::size(r)`/`std::data(r)` over `r.size()`/`r.data()` lets the same code accept arrays and containers alike — the same uniformity argument as the non-member iterators.

---

## 28.6 `std::as_const`

`std::as_const(x)` (header `<utility>`) returns a `const` reference to its argument — a tiny utility whose value is making "treat this as const **here**" explicit and effortless. Its main use is forcing selection of a `const` overload (for example, to get a `const_iterator`, or to avoid a copy-on-write detach) without writing a `const_cast` or naming the type.

```cpp
// Listing 28.8: as_const forces the const overload
#include <utility>
#include <vector>

std::vector<int> v{1, 2, 3};

auto it  = v.begin();                 // iterator (mutable)
auto cit = std::as_const(v).begin();  // const_iterator — selects begin() const

for (const auto& x : std::as_const(v)) { /* guaranteed read-only view */ }
```

`std::as_const` is deliberately deleted for rvalues, preventing you from forming a `const` reference to a temporary that is about to die. It pairs well with range-`for` when you want to *prove* the loop does not mutate the container.

---

## 28.7 `std::launder`

`std::launder` (header `<new>`) is a low-level **pointer-laundering** barrier for advanced memory-reuse code. When you construct a new object in storage that previously held a *different* object — especially one with `const` or reference members — a pointer obtained before the re-construction may not legally refer to the new object, because the compiler is allowed to assume `const`/reference subobjects don't change. `std::launder(p)` returns a pointer to the object actually now living at that address, defeating that assumption.

```cpp
// Listing 28.9: launder after constructing a new object over an old one
#include <new>

struct Widget { const int id; };   // const member is the trap

alignas(Widget) unsigned char buf[sizeof(Widget)];
Widget* p1 = new (buf) Widget{1};

p1->~Widget();
Widget* p2 = new (buf) Widget{2};  // different object, same storage

// Using p1 here is UB (it has a const member). Launder to get a valid pointer:
Widget* valid = std::launder(reinterpret_cast<Widget*>(buf));
int id = valid->id;                // 2 — correct and well-defined
```

This is a tool for implementers of `optional`, `variant`, small-buffer-optimized containers, and arena allocators — code that reuses raw storage for objects with `const`/reference members. In ordinary application code you should never need it; its presence signals deliberate, careful storage reuse.

---

## 28.8 `std::uncaught_exceptions`

`std::uncaught_exceptions()` (header `<exception>`) returns the **number** of exceptions currently in flight (being propagated and not yet caught). It replaces the C++98 `std::uncaught_exception()` (singular, returning `bool`), whose boolean answer could not distinguish "an exception is propagating" from "*another* exception started while I was already unwinding."

The canonical use is a **scope-guard destructor** that needs to know whether it is running during normal scope exit or during stack unwinding — so it can commit on success and roll back on an exception:

```cpp
// Listing 28.10: a transaction guard that detects unwinding correctly
#include <exception>

class TransactionGuard {
    int entry_count = std::uncaught_exceptions();
public:
    ~TransactionGuard() {
        if (std::uncaught_exceptions() > entry_count) {
            rollback();   // MORE exceptions in flight than at construction -> we are unwinding
        } else {
            commit();     // normal exit
        }
    }
};
```

Comparing the count at construction with the count at destruction is robust even when the guard itself lives inside another exception's unwinding — a case the old boolean `uncaught_exception()` got wrong. This is the correct foundation for `scope_success`/`scope_fail`-style guards.

---

## 28.9 Searchers: `default_searcher`, `boyer_moore_searcher`, `boyer_moore_horspool_searcher`

C++17 generalizes `std::search` to accept a **searcher** object (header `<functional>`), letting you choose the substring-search *algorithm* while keeping one call site. Three searchers ship:

- **`std::default_searcher`** — the naive O(n·m) scan, equivalent to plain `std::search`.
- **`std::boyer_moore_searcher`** — the Boyer-Moore algorithm: builds skip tables from the pattern so it can jump ahead on mismatches, giving sublinear average-case time on long texts.
- **`std::boyer_moore_horspool_searcher`** — the Horspool simplification of Boyer-Moore: a single skip table, lower preprocessing cost and memory, slightly less skipping.

```cpp
// Listing 28.11: choosing a substring-search algorithm via a searcher
#include <functional>
#include <algorithm>
#include <string>

std::string text    = "...a long body of text to scan...";
std::string pattern = "needle";

// Boyer-Moore: preprocess the pattern once, then search (sublinear on average):
auto bm = std::boyer_moore_searcher(pattern.begin(), pattern.end());
auto it = std::search(text.begin(), text.end(), bm);
if (it != text.end()) {
    // found at position std::distance(text.begin(), it)
}

// The Horspool variant — cheaper preprocessing, less memory:
auto hs = std::boyer_moore_horspool_searcher(pattern.begin(), pattern.end());
auto it2 = std::search(text.begin(), text.end(), hs);
```

The key efficiency point is **reuse**: a searcher precomputes its skip tables from the pattern at construction, so if you search for the *same pattern* in *many texts*, you build the searcher once and amortize that preprocessing across every search. For a one-off search of a short pattern, `default_searcher` (or plain `std::search`) wins because the preprocessing isn't repaid; Boyer-Moore pays off for long patterns, long texts, or repeated searches.

---

## 28.10 Professional Insights

**Use `std::byte` for raw-memory buffers and `to_chars`/`from_chars` for number conversion in hot paths.** `std::byte` makes "this is memory, not a number" a type-system fact, killing accidental-arithmetic and sign-extension bugs in serialization and protocol code. `to_chars`/`from_chars` are the only standard conversions that neither allocate nor touch the locale and report failure without exceptions — making them the correct primitives for parsers, serializers, and any latency-sensitive formatting where `stringstream` and `stoi` are too slow or too surprising.

**Prefer `std::clamp` and the free-function accessors as defaults.** `std::clamp(v, lo, hi)` says what it means, unlike a nested `max(min(...))`. `std::size`/`std::empty`/`std::data` make generic code accept raw arrays as well as containers and give a `constexpr` array size that the `sizeof` trick cannot safely guarantee — adopt them in templates as a matter of style.

**Build a searcher once and reuse it across many searches.** The Boyer-Moore and Horspool searchers front-load preprocessing into the searcher object; their advantage materializes only when you amortize that cost over repeated searches of the same pattern, or over a long text. For a single search of a short pattern, the default searcher is faster. Match the searcher to the access pattern, and don't reach for Boyer-Moore reflexively.

**Reserve `std::launder` for deliberate storage reuse, and `uncaught_exceptions()` for unwinding-aware guards.** `launder` is a precision tool for implementers of `variant`/`optional`-like containers and arena allocators that reconstruct objects with `const`/reference members in raw storage; its appearance should signal exactly that intent. `std::uncaught_exceptions()` (plural) is the correct way for a scope-guard destructor to tell normal exit from stack unwinding — the basis of reliable `scope_fail`/`scope_success` semantics, and a correctness fix over the old boolean form.

**Let `std::as_const` express read-only intent precisely.** When you want a `const_iterator` or to force the `const` overload without a `const_cast`, `std::as_const(x)` says so in one token and refuses to bind to a dying temporary. It is the cleanest way to prove a loop or call does not mutate its subject.
