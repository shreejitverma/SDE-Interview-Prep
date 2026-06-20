# Chapter 52: Container and Algorithm Additions

> *C++20 sands down the rough edges of the standard containers and algorithms with a set of additions that replace verbose, error-prone idioms with single, intention-revealing calls. `.contains()` ends the `find() != end()` dance; `starts_with`/`ends_with` give strings the prefix/suffix tests they always lacked; the uniform `std::erase`/`std::erase_if` free functions fix the notorious erase-remove idiom; and `make_shared` finally supports arrays. This chapter catalogues these container and algorithm improvements and the idioms they retire.*

None of these features are large, but each replaces a pattern that was either verbose, subtly wrong, or both. `m.find(k) != m.end()` is the canonical "is this key present?" incantation that everyone writes and no one likes. The erase-remove idiom (`v.erase(std::remove(v.begin(), v.end(), x), v.end())`) is a genuine C++ shibboleth — concise once learned, baffling and bug-prone until then. Substring prefix checks meant `s.compare(0, p.size(), p) == 0` or `s.rfind(p, 0) == 0`. C++20 gives all of these a direct, readable spelling.

---

## Table of Contents

- [52.1 Associative contains()](#521-associative-contains)
- [52.2 String starts_with and ends_with](#522-string-starts_with-and-ends_with)
- [52.3 Uniform std::erase and std::erase_if](#523-uniform-stderase-and-stderase_if)
- [52.4 make_shared and make_unique for Arrays](#524-make_shared-and-make_unique-for-arrays)
- [52.5 Heterogeneous Lookup for Unordered Containers](#525-heterogeneous-lookup-for-unordered-containers)
- [52.6 shift_left and shift_right](#526-shift_left-and-shift_right)
- [52.7 Professional Insights](#527-professional-insights)

---

## 52.1 Associative contains()

Every associative and unordered container (`map`, `set`, `multimap`, `unordered_map`, …) gains a `.contains(key)` member returning `bool`. It replaces the `find() != end()` idiom with a direct, readable test.

```cpp
// Listing 52.1: membership tests, before and after
#include <map>
#include <set>
#include <unordered_map>
#include <string>

std::map<std::string, int> ages{{"alice", 30}, {"bob", 25}};

// Before C++20: the find/end dance.
if (ages.find("alice") != ages.end()) { /* present */ }

// C++20: says exactly what it means.
if (ages.contains("alice")) { /* present */ }

std::set<int> s{1, 2, 3};
bool has = s.contains(2);                 // true

std::unordered_map<int, int> u{{1, 10}};
bool present = u.contains(1);              // true
```

`contains` returns only presence — when you need the *value*, `find` (or `at`/`operator[]`) is still the tool, since `contains` followed by `find` would hash/compare the key twice. But for the pervasive "is this key here?" question, `contains` is clearer and signals intent without exposing iterators. For `multimap`/`multiset` it reports whether *any* element matches.

---

## 52.2 String starts_with and ends_with

`std::string` and `std::string_view` gain `starts_with` and `ends_with`, accepting a string, `string_view`, or single character. These replace the awkward `compare`/`rfind`/`substr` workarounds for prefix and suffix testing.

```cpp
// Listing 52.2: prefix and suffix tests
#include <string>
#include <string_view>

std::string url = "https://example.com";

// Before: opaque and easy to get wrong.
// bool secure = url.compare(0, 8, "https://") == 0;

// C++20: direct.
bool secure = url.starts_with("https://");     // true
bool com    = url.ends_with(".com");           // true
bool slash  = url.starts_with('h');            // true — single char overload too

std::string_view sv = "filename.txt";
bool is_txt = sv.ends_with(".txt");            // true
```

Both take a `string_view`-convertible argument (or a `char`, or a `const char*`) and return `bool` without allocating. They are `constexpr` and work identically on `std::string` and `std::string_view`, so prefix/suffix logic can move into constant expressions and into views over non-owning data. **Version note:** the related `contains` member on strings (substring search returning `bool`) is **C++23**, not C++20 — in C++20 use `s.find(sub) != std::string::npos`.

---

## 52.3 Uniform std::erase and std::erase_if

C++20 adds free functions `std::erase(container, value)` and `std::erase_if(container, predicate)` that erase matching elements **and** actually shrink the container — finally retiring the error-prone erase-remove idiom and unifying erasure across all standard containers.

```cpp
// Listing 52.3: erasing elements the right way
#include <vector>
#include <list>
#include <string>
#include <algorithm>

std::vector<int> v{1, 2, 3, 2, 4, 2, 5};

// Before C++20: the erase-remove idiom — correct but cryptic, and a classic bug
// if you forget the second .end() argument (erasing only one element).
v.erase(std::remove(v.begin(), v.end(), 2), v.end());

// C++20: one call, says what it does, works on every container.
std::vector<int> w{1, 2, 3, 2, 4, 2, 5};
std::erase(w, 2);                                   // removes all 2s
std::erase_if(w, [](int x){ return x % 2 == 0; }); // removes all evens

std::string s = "a1b2c3";
std::erase_if(s, [](char c){ return std::isdigit(c); });  // "abc"
```

The free functions return the **number of elements removed** (useful for "did anything change?" logic) and are specialized per container so they do the efficient thing everywhere: on `vector`/`string`/`deque` they perform the remove-and-shrink; on node-based containers (`list`, `map`, `set`) they splice out nodes directly. This eliminates the single most common erase bug — calling `remove` without the trailing `erase`, which reorders but does not shrink — and gives one uniform spelling for all containers.

---

## 52.4 make_shared and make_unique for Arrays

C++20 extends `std::make_shared` to support array types, matching the array support `std::make_unique` already had since C++14. You can now allocate a shared array in a single call with value-initialized (or default-initialized) elements.

```cpp
// Listing 52.4: making shared and unique arrays
#include <memory>

// make_unique for arrays (since C++14):
auto u = std::make_unique<int[]>(100);          // 100 value-initialized ints

// make_shared for arrays (NEW in C++20):
auto s1 = std::make_shared<int[]>(100);         // shared array of 100 ints (zeroed)
auto s2 = std::make_shared<int[]>(100, 7);      // 100 ints, each initialized to 7
auto s3 = std::make_shared<int[10]>();          // fixed-size shared array of 10

int  first = s1[0];                             // operator[] on the shared array
```

Before C++20, a shared dynamic array required either a `std::vector` (the usual right answer) or constructing `shared_ptr` with a manual `delete[]` deleter. `make_shared<T[]>(n)` allocates the control block and the array in one allocation, value-initializes the elements (or initializes all to a provided value with the two-argument form), and provides `operator[]`. As with the non-array `make_shared`, the single combined allocation is more cache-friendly and exception-safe than separate `new[]` plus `shared_ptr` construction.

---

## 52.5 Heterogeneous Lookup for Unordered Containers

C++20 extends **heterogeneous lookup** — searching a container with a key of a *different but comparable* type, avoiding a temporary key conversion — to the unordered (hashed) containers. Ordered containers had it since C++14 via transparent comparators; now `unordered_map`/`unordered_set` get it via transparent hashers.

```cpp
// Listing 52.5: looking up by string_view in a string-keyed hash map
#include <unordered_map>
#include <string>
#include <string_view>

// Opt in by supplying a transparent hash AND a transparent equality, both of
// which must define the is_transparent tag.
struct StringHash {
    using is_transparent = void;                 // <-- enables heterogeneous lookup
    std::size_t operator()(std::string_view sv) const {
        return std::hash<std::string_view>{}(sv);
    }
    std::size_t operator()(const std::string& s) const {
        return std::hash<std::string_view>{}(s);
    }
};

std::unordered_map<std::string, int, StringHash, std::equal_to<>> m{
    {"hello", 1}, {"world", 2}
};

std::string_view key = "hello";
auto it = m.find(key);     // NO temporary std::string constructed for the lookup
```

Without heterogeneous lookup, `m.find(string_view)` on a `std::string`-keyed map constructs a temporary `std::string` (an allocation) just to perform the search. Opting in requires two things on the container's type: a transparent hasher (one whose member type `is_transparent` exists and which can hash all the lookup types) **and** `std::equal_to<>` as the transparent comparator. With both, `find`, `count`, `contains`, and `equal_range` accept any type the hasher and comparator understand, eliminating the temporary — a meaningful win in hot lookup paths over string-keyed maps.

---

## 52.6 shift_left and shift_right

`std::shift_left` and `std::shift_right` (header `<algorithm>`) move the elements of a range toward the beginning or end by *n* positions, in place. They fill the conceptual gap left by `rotate` (which wraps) — a shift discards elements that fall off the end rather than wrapping them around.

```cpp
// Listing 52.6: in-place element shifting
#include <algorithm>
#include <vector>

std::vector<int> v{1, 2, 3, 4, 5};

std::shift_left(v.begin(), v.end(), 2);    // {3, 4, 5, ?, ?}
// First 3 elements are now {3,4,5}; the last 2 are moved-from (unspecified).

std::vector<int> w{1, 2, 3, 4, 5};
std::shift_right(w.begin(), w.end(), 2);   // {?, ?, 1, 2, 3}
// Last 3 elements are now {1,2,3}; the first 2 are moved-from.
```

Both return an iterator delimiting the range of valid (shifted-into) elements, and the vacated positions hold moved-from values you should overwrite or ignore. Unlike `std::rotate`, shifted-out elements are **not** preserved — this is the right primitive for sliding-window buffers, ring-buffer compaction, and time-series shifting where old data should be dropped, not rotated back in. They move (not copy) elements, so they work efficiently on move-only types.

---

## 52.7 Professional Insights

**Use `.contains()` for membership and reserve `find` for value retrieval.** `m.contains(k)` states intent directly and avoids exposing iterators for the pervasive "is this key present?" check; the old `find() != end()` idiom should disappear from new code. The one caveat: do not write `if (m.contains(k)) use(m.find(k))` — that hashes or compares the key twice. When you need both presence and value, call `find` once and test the iterator, or use `try_emplace`/`insert_or_assign` for insert-or-update semantics.

**Replace the erase-remove idiom everywhere with `std::erase`/`std::erase_if`.** The classic `v.erase(std::remove(...), v.end())` is the single most common source of "it compiled but didn't remove anything" bugs — forget the trailing `.end()` argument and you erase one element instead of all matches. The free functions are correct by construction, return the count removed, read as plain English, and dispatch to the efficient strategy for each container type (compact-and-shrink for contiguous, node-splice for linked). There is no reason to write the manual idiom in C++20.

**Add `starts_with`/`ends_with` to your string-handling vocabulary, and remember string `contains` is C++23.** Prefix and suffix tests are now direct, `constexpr`, allocation-free, and identical on `string` and `string_view` — retiring the `compare`/`rfind(p, 0)` tricks that were easy to get subtly wrong. But the substring `contains` member on strings did not arrive until C++23; under C++20 keep using `s.find(sub) != npos` for "does it contain this substring."

**Opt into heterogeneous lookup on hot string-keyed hash maps.** A `std::string`-keyed `unordered_map` searched with a `string_view` or `const char*` allocates a temporary `std::string` on every lookup unless you supply a transparent hasher (with `is_transparent`) and `std::equal_to<>`. In lookup-heavy code over string keys this is a real, measurable cost; the opt-in is a small amount of boilerplate that eliminates an allocation per query. Make it the default for any performance-sensitive string-keyed hash container.

**Prefer `make_shared<T[]>` and the right shifting primitive over hand-rolled alternatives.** `make_shared<T[]>(n)` gives a shared array in one cache-friendly, exception-safe allocation — though for a resizable owning buffer `std::vector` is still usually the better answer; reach for the shared array only when shared ownership of a fixed-size block is genuinely needed. And distinguish `shift_left`/`shift_right` (discard the elements that fall off — correct for sliding windows) from `std::rotate` (wrap them around): choosing the wrong one silently produces stale or duplicated data in ring buffers and time series.
