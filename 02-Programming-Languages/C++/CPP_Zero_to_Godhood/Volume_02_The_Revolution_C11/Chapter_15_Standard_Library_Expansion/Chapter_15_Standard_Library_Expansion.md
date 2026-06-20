# Chapter 15: Standard Library Expansion

> *C++11 doubled the standard library's surface area: hashed containers, fixed and singly-linked sequences, type-safe time, a real random-number framework, regular expressions, and the small conveniences — numeric conversions, compile-time ratios — that remove a decade of hand-rolled code.*

The language features of C++11 (Chapters 11–14) are matched by an equally large library expansion. This chapter covers the containers and utilities a systems engineer reaches for daily: **unordered (hashed) containers**, **`std::array`**, **`std::forward_list`**, **`std::regex`**, **`std::chrono`** with **`std::ratio`**, the **`<random>`** framework, and the new **numeric string conversions** (`stoi`/`to_string`). Throughout, the emphasis is on the memory and latency characteristics that decide whether a facility belongs in a hot path.

---

## Table of Contents

- [15.1 Unordered (Hashed) Containers](#151-unordered-hashed-containers)
- [15.2 `std::array`](#152-stdarray)
- [15.3 `std::forward_list`](#153-stdforward_list)
- [15.4 `std::regex`](#154-stdregex)
- [15.5 `std::chrono` and `std::ratio`](#155-stdchrono-and-stdratio)
- [15.6 The `<random>` Framework](#156-the-random-framework)
- [15.7 Numeric String Conversions](#157-numeric-string-conversions)
- [15.8 Professional Insights](#158-professional-insights)

---

## 15.1 Unordered (Hashed) Containers

C++11 added four **hash-based** associative containers, complementing the existing red-black-tree (`std::map`/`std::set`) family:

| Container | Keys | Values | Duplicate keys |
| :--- | :--- | :--- | :--- |
| `std::unordered_set` | yes | — | no |
| `std::unordered_map` | yes | yes | no |
| `std::unordered_multiset` | yes | — | yes |
| `std::unordered_multimap` | yes | yes | yes |

They offer **O(1) average** lookup, insertion, and erasure (versus O(log n) for the ordered trees), at the cost of **no ordering** and worst-case O(n) on pathological collisions. Each requires a **hash function** for the key type — provided automatically for built-in types and `std::string`, and supplied by you (a `std::hash` specialization or a custom functor) for user-defined keys.

```cpp
// Listing 15.1: unordered_map basics
#include <unordered_map>
#include <string>

std::unordered_map<std::string, int> counts;
counts["apple"] = 3;          // insert or assign
++counts["banana"];           // value-initialized to 0, then incremented
auto it = counts.find("apple");
if (it != counts.end()) { /* it->second == 3 */ }
```

The container is an array of **buckets**; each bucket holds a linked list of nodes that hash to it. Two knobs govern performance: **`load_factor()`** (elements ÷ buckets) and **`max_load_factor()`**. When the load factor would exceed the maximum, the table **rehashes** — reallocating buckets and re-distributing nodes, an O(n) operation that invalidates iterators (but never references/pointers to elements).

```cpp
// Listing 15.2: pre-sizing to avoid rehashes in a hot path
std::unordered_map<int, int> m;
m.reserve(1'000'000);         // allocate buckets up front; no mid-run rehash
m.max_load_factor(0.7f);      // trade memory for fewer collisions
```

> **Systems note:** because elements are heap-allocated nodes scattered across buckets, unordered containers are **cache-unfriendly**. For small or hot key sets, a sorted `std::vector` with binary search, or an open-addressing table (e.g. a flat hash map), often beats `std::unordered_map` on real hardware despite the worse asymptotic class.

---

## 15.2 `std::array`

`std::array<T, N>` is a **fixed-size**, **stack-allocated** sequence — a thin, zero-overhead wrapper around a C array that adds the STL container interface. It is an aggregate: no constructors, no heap, no size stored at runtime (N is a compile-time template parameter).

```cpp
// Listing 15.3: std::array vs C array
#include <array>
std::array<int, 5> arr = {1, 2, 3, 4, 5};
arr.size();        // 5 — known at compile time
arr.at(10);        // throws std::out_of_range (bounds-checked)
arr[2];            // unchecked, like a raw array
int* p = arr.data(); // contiguous storage, interoperable with C APIs
// arr does NOT decay to a pointer; it can be returned, copied, compared
```

Compared to `std::vector`, `std::array` has **no dynamic allocation** and **no capacity overhead** — its footprint is exactly `N * sizeof(T)`. It is the correct choice whenever the size is a compile-time constant: lookup tables, fixed protocol frames, SIMD lanes, small math vectors. Unlike a raw array it knows its size, supports `begin()`/`end()`, is copyable and comparable, and can be returned from a function by value.

---

## 15.3 `std::forward_list`

`std::forward_list<T>` is a **singly-linked list** — the most memory-frugal node-based sequence in the library. Each node stores one element plus a single forward pointer (versus two for `std::list`), so it exists specifically for situations where that per-node saving matters and backward traversal is never needed.

```cpp
// Listing 15.4: forward_list and its splice-style insertion
#include <forward_list>
std::forward_list<int> fl = {1, 2, 3};
fl.push_front(0);                 // O(1); there is NO push_back
auto it = fl.before_begin();      // iterator to "slot before the first element"
fl.insert_after(it, 99);          // insertion is "after" a position
fl.erase_after(it);               // erase the element after `it`
```

Because there is no back pointer, the API is built around **`_after`** operations and a special **`before_begin()`** iterator: you cannot insert *at* a position, only *after* one. There is intentionally **no `size()`** member (it would cost a stored counter or an O(n) walk) and **no `push_back`/`back`**.

> **When to use it:** `forward_list` pays off only in large collections of small nodes where the one-pointer saving is significant, or in intrusive/lock-free designs that need a singly-linked structure. For almost everything else, `std::vector` wins on cache locality. Treat `forward_list` as a specialist, not a default.

---

## 15.4 `std::regex`

`<regex>` brings regular expressions into the standard library for searching, matching, and replacing text.

```cpp
// Listing 15.5: search, match, and replace
#include <regex>
#include <string>
#include <iostream>

std::string text = "Contact: user@example.com";
std::regex email(R"((\w+)@(\w+)\.com)");   // raw string avoids backslash doubling

std::smatch m;                              // match results over a std::string
if (std::regex_search(text, m, email)) {    // find first occurrence anywhere
    std::cout << "whole: " << m[0] << "\n"; // user@example.com
    std::cout << "user:  " << m[1] << "\n"; // user  (capture group 1)
}

bool full = std::regex_match(text, email);  // false — must match the ENTIRE string
std::string redacted = std::regex_replace(text, email, "REDACTED");
```

Three primary algorithms: **`regex_search`** (first match anywhere), **`regex_match`** (the whole input must match), and **`regex_replace`** (substitute). Capture groups are accessed through a **`std::smatch`** (for `std::string`) or `std::cmatch` (for `const char*`): `m[0]` is the whole match, `m[1]`, `m[2]`, … are the parenthesized sub-patterns.

**Grammar flavors.** The default syntax is **ECMAScript**; alternatives are selected via `std::regex_constants` — `basic` (POSIX BRE), `extended` (POSIX ERE), and `awk`/`grep`/`egrep`.

> **Performance trap — `std::regex` is heavy.** On several implementations (notably libstdc++), `std::regex` is slow to execute and bloats binaries through deep template instantiation. Two mitigations: (1) **construct the `std::regex` object once** and reuse it — never compile a pattern inside a loop; (2) where regex throughput is critical, prefer **RE2** or **Boost.Regex**. To avoid materializing substrings, read capture groups through `std::ssub_match` rather than copying into new `std::string`s.

---

## 15.5 `std::chrono` and `std::ratio`

`<chrono>` is a **type-safe** time library built on three concepts:

- **Clocks** — sources of "now". `std::chrono::system_clock` (wall-clock, can jump with NTP/leap adjustments), `std::chrono::steady_clock` (monotonic, never goes backward — the right clock for measuring intervals), and `high_resolution_clock` (often an alias for one of the above).
- **Durations** — `std::chrono::duration<Rep, Period>`, a count of ticks whose tick length is a compile-time fraction. Named aliases: `nanoseconds`, `microseconds`, `milliseconds`, `seconds`, `minutes`, `hours`.
- **Time points** — `time_point`, a duration measured from a clock's epoch.

```cpp
// Listing 15.6: measuring an interval with the monotonic clock
#include <chrono>
auto start = std::chrono::steady_clock::now();
// ... work ...
auto end = std::chrono::steady_clock::now();
auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
```

The type system prevents unit-mixing mistakes: adding a `seconds` to a `milliseconds` yields the correct common type; an implicit conversion that would lose precision (e.g. `nanoseconds` → `seconds`) is a **compile error** unless you ask for it explicitly with `duration_cast`.

**`std::ratio`** is the compile-time rational-number machinery underlying durations. `std::ratio<Num, Den>` represents an exact fraction as a *type*, with arithmetic performed by the compiler:

```cpp
// Listing 15.7: std::ratio and its role in durations
#include <ratio>
using half = std::ratio<1, 2>;
static_assert(std::ratio_add<half, half>::type::num == 1, "1/2 + 1/2 == 1");

// A duration IS a ratio-parameterized type:
using milliseconds = std::chrono::duration<long long, std::milli>; // std::milli == ratio<1,1000>
```

The SI prefixes `std::milli`, `std::micro`, `std::nano`, `std::kilo`, etc. are predefined `std::ratio` aliases. Because the tick period lives in the type, all unit conversions are resolved and checked at compile time with **zero runtime cost**.

---

## 15.6 The `<random>` Framework

C++11 replaced the weak, globally-stateful `rand()` with a composable framework that cleanly separates two responsibilities: an **engine** that produces raw pseudo-random bits, and a **distribution** that shapes those bits into the numbers you want.

```cpp
// Listing 15.8: rolling a fair six-sided die
#include <random>
std::random_device rd;                          // non-deterministic seed source
std::mt19937 gen(rd());                          // Mersenne Twister engine, seeded
std::uniform_int_distribution<int> die(1, 6);    // shape: uniform integers in [1,6]
int roll = die(gen);                             // draw
```

- **Engines** — `std::mt19937` (32-bit Mersenne Twister, the usual default), `mt19937_64`, `minstd_rand`, `ranlux`. `std::random_device` provides a (typically) hardware/OS entropy source suitable for **seeding** — it is slow and must not be used as a per-draw generator.
- **Distributions** — `uniform_int_distribution`, `uniform_real_distribution`, `normal_distribution`, `bernoulli_distribution`, `poisson_distribution`, and more.

This separation is the framework's strength: any engine can feed any distribution, distributions are reproducible (seed the engine to replay a sequence — essential for deterministic tests and simulations), and the state is **local**, not a hidden global, so it is thread-safe by construction when each thread owns its engine.

> **Systems note:** seed once, draw many. Constructing `random_device` or re-seeding per call destroys both performance and statistical quality. In multithreaded code give each thread its own `thread_local` engine (Chapter 17) to avoid contention and shared state.

---

## 15.7 Numeric String Conversions

C++11 added simple, locale-independent conversions between numbers and `std::string`, finally retiring `atoi`/`strtol` boilerplate and `stringstream` ceremony for the common case.

```cpp
// Listing 15.9: string -> number
#include <string>
int    i  = std::stoi("42");            // also stol, stoll, stoul, stoull
double d  = std::stod("3.14");          // also stof, stold
std::size_t pos;
int    hex = std::stoi("ff", &pos, 16); // base-16; pos receives chars consumed

// Listing 15.10: number -> string
std::string s1 = std::to_string(42);    // "42"
std::string s2 = std::to_string(3.14);  // "3.140000"
```

The `sto*` family parses leading whitespace and an optional sign, accepts an explicit base, and reports how many characters it consumed through the optional `pos` out-parameter. They **throw** on failure: `std::invalid_argument` when no conversion is possible, `std::out_of_range` when the value overflows the target type.

> **Caveats for systems work:** `std::to_string` for floating point uses `%f` formatting (fixed notation, default 6 digits) and is **locale-dependent** — not what you want for round-tripping or for the shortest-exact representation. The `sto*` functions throw, which is unacceptable on a no-throw hot path. For both round-tripping and zero-allocation/no-throw conversion, the right tool arrived later: C++17's `std::from_chars`/`std::to_chars`. In C++11, prefer these functions for convenience and validated input, but reach for hand-tuned parsing in latency-critical paths.

---

## 15.8 Professional Insights

**Match the container to the access pattern, not the asymptotics.** `std::unordered_map`'s O(1) hides poor cache behavior: node-per-element heap allocation and pointer chasing. For small, hot, or scan-heavy key sets, a sorted `std::vector` or a flat (open-addressing) hash map frequently wins on wall-clock despite identical or worse big-O. Profile on representative data.

**Pre-size hashed containers.** Every rehash is an O(n) reallocation that invalidates iterators. If you know the element count, `reserve()` once and tune `max_load_factor` to trade memory for fewer collisions — critical in any allocation-sensitive or real-time path.

**Default to `std::array` and `std::vector`; treat `list`/`forward_list` as specialists.** Linked lists win only when you need stable element addresses across insertion/erasure, or O(1) splicing, *and* you rarely traverse. The pointer-chasing cost of traversal usually dwarfs the algorithmic benefit on modern memory hierarchies.

**Always measure intervals with `steady_clock`.** `system_clock` can jump backward or forward (NTP, leap seconds, manual changes); using it for timing produces negative or absurd durations. `steady_clock` is monotonic and is the only correct choice for benchmarks, timeouts, and rate limiting.

**Treat `std::regex` and `std::to_string` as convenience, not performance, tools.** Both are fine for config parsing, tooling, and cold paths; both are wrong in a tight loop. Compile regexes once; for hot numeric formatting plan to move to `to_chars`/`from_chars` (C++17) or a bespoke routine.
