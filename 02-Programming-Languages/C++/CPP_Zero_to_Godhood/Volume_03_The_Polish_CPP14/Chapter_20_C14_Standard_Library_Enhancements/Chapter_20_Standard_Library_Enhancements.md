# Chapter 20: Standard Library Enhancements

> *The C++11 library shipped `make_shared` but not `make_unique`, a `shared_mutex` proposal that didn't make it, and associative containers that forced a temporary on every heterogeneous lookup. C++14 closed these gaps: `make_unique` completes the factory pair, `shared_timed_mutex` brings reader-writer locking, transparent functors enable allocation-free lookup, and a wave of small utilities (`exchange`, `quoted`, `get<T>`) remove daily friction.*

This chapter collects the library additions of C++14. None is large, but each removes a recurring papercut: an exception-safe `unique_ptr` factory, a one-line move-and-replace idiom, a reader-writer mutex, quoted-string I/O, tuple access by type, heterogeneous container lookup without temporaries, and the start of the standard library's migration to `constexpr`.

---

## Table of Contents

- [20.1 `std::make_unique`](#201-stdmake_unique)
- [20.2 `std::exchange`](#202-stdexchange)
- [20.3 `std::shared_timed_mutex` and `std::shared_lock`](#203-stdshared_timed_mutex-and-stdshared_lock)
- [20.4 `std::quoted`](#204-stdquoted)
- [20.5 Tuple Addressing by Type: `std::get<T>`](#205-tuple-addressing-by-type-stdgett)
- [20.6 Transparent Operators and Heterogeneous Lookup](#206-transparent-operators-and-heterogeneous-lookup)
- [20.7 The `constexpr`-ification of the Standard Library](#207-the-constexpr-ification-of-the-standard-library)
- [20.8 Professional Insights](#208-professional-insights)

---

## 20.1 `std::make_unique`

C++11 shipped `std::make_shared` but, by an oversight, no `std::make_unique`. C++14 fixed the omission. `std::make_unique<T>(args...)` constructs a `T` (forwarding `args` to its constructor) and returns a `std::unique_ptr<T>` owning it — with no visible `new`.

```cpp
// Listing 20.1: make_unique vs raw new
#include <memory>

// Old, error-prone:
std::unique_ptr<Widget> w1(new Widget(42));

// C++14: no raw new, deduced type, exception-safe:
auto w2 = std::make_unique<Widget>(42);

// Arrays are supported via the T[] overload (value-initialized elements):
auto buf = std::make_unique<int[]>(1000);    // 1000 ints, zero-initialized
```

Beyond brevity, `make_unique` eliminates a real exception-safety hole. In a call like `f(std::unique_ptr<A>(new A), may_throw())`, the compiler may evaluate `new A`, then `may_throw()`, then the `unique_ptr` constructor; if `may_throw()` throws, the raw `A` leaks. Wrapping each argument in `make_unique` makes the allocation and the ownership transfer a single, indivisible call — nothing can interleave between `new` and the smart-pointer capture.

> **Rule:** prefer `make_unique`/`make_shared` to a raw `new` everywhere. The only routine exception is when you need a custom deleter (the make-functions don't accept one) or must pass an already-constructed pointer.

---

## 20.2 `std::exchange`

`std::exchange(obj, new_value)` assigns `new_value` to `obj` and returns the **old** value of `obj`. It is the missing primitive for the move-and-reset pattern that pervades move constructors and move-assignment operators.

```cpp
// Listing 20.2: std::exchange in a move constructor
#include <utility>

class Buffer {
    char*       data_;
    std::size_t size_;
public:
    // Move-construct: take other's pointer, leave other null -- in one expression each.
    Buffer(Buffer&& other) noexcept
        : data_(std::exchange(other.data_, nullptr)),
          size_(std::exchange(other.size_, 0)) {}

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = std::exchange(other.data_, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }
};
```

The semantics are exactly "set, return the previous." It generalizes the classic atomic-style swap-out beyond move semantics — resetting a flag while reading its prior state, or rotating a value out of a slot — in a single, self-documenting expression.

---

## 20.3 `std::shared_timed_mutex` and `std::shared_lock`

C++14 introduced the first standard **reader-writer lock**: `std::shared_timed_mutex`. It supports two ownership modes:

- **Shared (read) ownership** — many threads may hold it simultaneously, acquired via `std::shared_lock`.
- **Exclusive (write) ownership** — only one thread, no readers concurrent with it, acquired via `std::unique_lock` (or `std::lock_guard`).

This is the right tool when reads vastly outnumber writes, since readers no longer serialize against one another.

```cpp
// Listing 20.3: a read-mostly map guarded by a shared_timed_mutex
#include <shared_mutex>
#include <mutex>
#include <map>
#include <string>

class ThreadSafeMap {
    mutable std::shared_timed_mutex mtx_;
    std::map<std::string, int>      data_;
public:
    // Reads: shared ownership -- concurrent readers do not block each other.
    int get(const std::string& key) const {
        std::shared_lock<std::shared_timed_mutex> lock(mtx_);
        auto it = data_.find(key);
        return it != data_.end() ? it->second : -1;
    }

    // Writes: exclusive ownership -- blocks all readers and other writers.
    void set(const std::string& key, int value) {
        std::unique_lock<std::shared_timed_mutex> lock(mtx_);
        data_[key] = value;
    }
};
```

Two points of precision matter in C++14:

1. **Spell the lock's template argument.** Class template argument deduction (`std::shared_lock lock(mtx_)`) is a C++17 feature; in C++14 you must write `std::shared_lock<std::shared_timed_mutex> lock(mtx_)`.
2. **It is the *timed* mutex.** C++14 provides only `std::shared_timed_mutex` (which also offers `try_lock_for`/`try_lock_until`). The lighter, non-timed `std::shared_mutex` was added later, in **C++17** — flagged here because it is a common version trap.

> **Performance note:** a reader-writer lock is not free — its bookkeeping is heavier than a plain mutex, and under heavy *write* contention it can be slower. Use it only when the read-to-write ratio is genuinely high; for short critical sections a plain `std::mutex` often wins.

---

## 20.4 `std::quoted`

`std::quoted` (in `<iomanip>`) is an I/O manipulator that handles the quoting and escaping of strings containing spaces, quotes, and delimiters — symmetrically on output and input. It removes the hand-rolled escaping that string serialization (CSV, logs, config) otherwise requires.

```cpp
// Listing 20.4: round-tripping a string with embedded quotes and spaces
#include <iomanip>
#include <sstream>
#include <string>

std::string original = R"(He said "hello world")";

std::stringstream ss;
ss << std::quoted(original);      // writes:  "He said \"hello world\""

std::string restored;
ss >> std::quoted(restored);      // parses the quoting/escaping back out
// restored == original, exactly -- spaces and embedded quotes preserved
```

On output `std::quoted` wraps the string in delimiters and escapes any embedded delimiter; on input it reads a delimited, escaped sequence and reconstructs the original. Because the two are inverses, it is the simplest correct way to serialize a string field that may contain whitespace or the delimiter itself — exactly the case where naive `<<`/`>>` would split or corrupt the value.

---

## 20.5 Tuple Addressing by Type: `std::get<T>`

C++11 let you index a `std::tuple` only positionally — `std::get<0>(t)`. C++14 added `std::get<T>(t)`, which retrieves the element **of type `T`**. It is well-formed only when exactly one element has that type; otherwise the program is ill-formed (ambiguous or absent).

```cpp
// Listing 20.5: addressing tuple elements by type
#include <tuple>
#include <string>

std::tuple<int, std::string, double> record{42, "alpha", 3.14};

auto n    = std::get<int>(record);          // 42      -- by type, not index
auto name = std::get<std::string>(record);  // "alpha"
auto x    = std::get<double>(record);       // 3.14
```

Addressing by type is more robust than by index: it does not silently break when the tuple's element order changes, and it reads as intent (`get<Timestamp>`) rather than as a magic position (`get<2>`). The constraint — type must be unique within the tuple — is the price of that clarity; for tuples with repeated types, fall back to positional `get<N>`.

---

## 20.6 Transparent Operators and Heterogeneous Lookup

The ordered associative containers (`std::map`, `std::set`, and their `multi` variants) use a comparator — by default `std::less<Key>`. In C++11, looking up a `std::map<std::string, V>` with a `const char*` or a `string_view`-like type **constructed a temporary `std::string`** for every probe, because the comparator's operands were both fixed to `Key`.

C++14 added **transparent comparators**: the `void` specializations `std::less<>` (i.e. `std::less<void>`), `std::greater<>`, etc., whose `operator()` is a *template* accepting heterogeneous operand types. When a container's comparator declares the nested type `is_transparent`, its `find`/`count`/`lower_bound`/`equal_range` gain overloads that accept **any** key-like argument comparable to `Key` — with no temporary.

```cpp
// Listing 20.6: heterogeneous lookup avoids constructing a temporary string
#include <map>
#include <string>

// std::less<> is transparent: it defines is_transparent and a templated operator().
std::map<std::string, int, std::less<>> scores{
    {"alice", 1}, {"bob", 2}
};

const char* key = "alice";
auto it = scores.find(key);   // C++14: NO temporary std::string is built --
                              // const char* is compared directly against the keys.
```

```cpp
// Listing 20.7: the transparent functor itself
struct CaseInsensitiveLess {
    using is_transparent = void;     // <-- opt in to heterogeneous lookup
    template <typename A, typename B>
    bool operator()(const A& a, const B& b) const {
        return ci_compare(a, b) < 0; // compares any two key-like types
    }
};
std::set<std::string, CaseInsensitiveLess> names;
```

The `is_transparent` member is a deliberate opt-in: it tells the container "my comparator can compare across types, so enable the templated lookup overloads." Without it, the container keeps the homogeneous (temporary-constructing) interface for backward compatibility. For hot lookup paths into string-keyed maps, declaring the comparator `std::less<>` is a free allocation eliminated per query.

> **Godhood tip:** default every `std::map`/`std::set` whose key is `std::string` to `std::less<>`. Combined with `string_view`-style probe types, it removes a heap allocation from every lookup — one of the cheapest latency wins in string-heavy services. (Heterogeneous lookup for the *unordered* containers arrived later, in C++20.)

---

## 20.7 The `constexpr`-ification of the Standard Library

Relaxed `constexpr` (Chapter 18) was not only a language change — C++14 also went through the library and marked many functions `constexpr` so they can participate in compile-time evaluation. Most of `<array>`'s accessors and capacity functions, much of `std::tuple`'s and `std::pair`'s interface (constructors, `get`, `make_pair`/`make_tuple`), `std::min`/`std::max`/`std::minmax` and their `initializer_list` forms, the `std::initializer_list` member functions, and several others became usable inside constant expressions.

```cpp
// Listing 20.8: standard-library calls evaluated at compile time (C++14)
#include <array>
#include <algorithm>

constexpr std::array<int, 4> a{4, 1, 3, 2};
constexpr int first = std::get<0>(a);      // get<> is constexpr
constexpr int n     = a.size();            // size() is constexpr
constexpr int hi    = std::max(10, 20);    // std::max is constexpr in C++14

static_assert(first == 4, "");
static_assert(hi == 20, "");
```

The practical effect: small lookup tables, fixed-size buffers, and bounds derived from `std::array`/`std::min`/`std::max` can be computed during compilation and embedded as immediates. This is the library half of the same shift Chapter 18 describes for the language — together they make non-trivial compile-time programming ordinary in C++14.

---

## 20.8 Professional Insights

**Make `make_unique` the default and `new` the exception.** It is shorter, deduces the type, and — critically — closes the multi-argument exception-safety hole that bare `new` leaves open. Reserve raw `new`/`unique_ptr(ptr)` for custom deleters or adopting a pointer you didn't allocate.

**`std::exchange` is the move-assignment primitive — use it to make moves obviously correct.** `data_ = std::exchange(other.data_, nullptr)` says "take theirs, null theirs" in one expression with no temporary and no ordering mistake. It makes `noexcept` move operations both shorter and easier to audit.

**Choose the reader-writer lock on evidence, not instinct.** `shared_timed_mutex` shines only when reads dominate writes; its overhead exceeds a plain `std::mutex` for short or write-heavy sections. Measure the ratio before adopting it — and remember the non-timed `shared_mutex` is C++17, so guard your `-std` assumptions.

**Default string-keyed ordered containers to `std::less<>`.** Transparent comparators turn every `find` with a `const char*` or view from a heap-allocating temporary into a direct comparison. In string-heavy, latency-sensitive code this is a zero-risk allocation removed from the hot path.

**Prefer `get<T>` and `constexpr` library calls where they fit.** Address tuples by type when types are unique — it survives reordering and documents intent. And lean on the now-`constexpr` `<array>`/`<algorithm>`/`<tuple>` interface to push fixed-size table and bound computation to compile time, eliminating it from the runtime entirely.
