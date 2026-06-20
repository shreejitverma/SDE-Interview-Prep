# Chapter 22: New Iterator and Algorithm Facilities

> *C++11 made generic, range-based iteration the default style; C++14 patched the rough edges that style exposed. The non-member `c`/`r` range accessors complete the free-function family, `make_reverse_iterator` removes a type-spelling chore, value-initialized forward iterators gain defined comparison so they can serve as sentinels, and the two-range `<algorithm>` overloads close a long-standing buffer-overrun hole.*

These additions share a theme: they make iterator-and-algorithm code that *looked* correct in C++11 actually safe and uniform. The headline item is the new four-iterator overloads of `equal`, `mismatch`, and `is_permutation`, which finally let the standard algorithms know where the **second** range ends — eliminating a class of out-of-bounds reads that the three-iterator forms invite. The rest are ergonomic completions of the C++11 generic-iteration model.

This chapter is authored to complete the C++14 coverage; it has no predecessor source material.

---

## Table of Contents

- [22.1 Non-Member `cbegin`/`cend`/`rbegin`/`rend`/`crbegin`/`crend`](#221-non-member-cbegincendrbeginrendcrbegincrend)
- [22.2 `std::make_reverse_iterator`](#222-stdmake_reverse_iterator)
- [22.3 Null (Value-Initialized) Forward Iterators](#223-null-value-initialized-forward-iterators)
- [22.4 Two-Range `<algorithm>` Overloads](#224-two-range-algorithm-overloads)
- [22.5 Professional Insights](#225-professional-insights)

---

## 22.1 Non-Member `cbegin`/`cend`/`rbegin`/`rend`/`crbegin`/`crend`

C++11 added the non-member `std::begin(c)` and `std::end(c)` so generic code could iterate any range — including C arrays — through a uniform free-function call. But it stopped there: the `const` and reverse accessors (`cbegin`, `rbegin`, …) existed only as *member* functions, so generic code that wanted a const or reverse iterator had to assume a member interface, which arrays do not have. **C++14 completed the set** with non-member `std::cbegin`, `std::cend`, `std::rbegin`, `std::rend`, `std::crbegin`, and `std::crend`.

```cpp
// Listing 22.1: uniform const/reverse access across containers AND arrays
#include <iterator>
#include <vector>

template <typename Range>
auto sum_const(const Range& r) {
    typename Range::value_type acc{};
    for (auto it = std::cbegin(r); it != std::cend(r); ++it)
        acc += *it;                 // *it is read-only regardless of r's constness
    return acc;
}

std::vector<int> v{1, 2, 3};
int  arr[] = {4, 5, 6};

auto a = std::cbegin(v);            // const_iterator into the vector
auto b = std::cbegin(arr);         // const int* -- works on a raw array too
auto r = std::crbegin(v);          // const reverse iterator, last element first
```

The win is **uniformity**: a template can now obtain a const or reverse iterator from *any* range — STL container, `std::array`, or built-in array — through one spelling, with no `typename Range::const_iterator` and no member-function assumption. `std::cbegin` also guarantees a true `const_iterator` even when called on a non-`const` container, which is the precise tool for "iterate but don't permit mutation."

---

## 22.2 `std::make_reverse_iterator`

Constructing a `std::reverse_iterator` by hand requires naming the underlying iterator type, which in generic code means an ugly `std::reverse_iterator<decltype(it)>(it)`. **C++14's `std::make_reverse_iterator(it)`** is a factory that *deduces* that type — the same convenience `make_pair`/`make_tuple` provide for their types.

```cpp
// Listing 22.2: building reverse iterators without spelling the type
#include <iterator>
#include <vector>

std::vector<int> v{1, 2, 3, 4, 5};

// Verbose, C++11:
auto r1 = std::reverse_iterator<std::vector<int>::iterator>(v.end());

// C++14: the type is deduced from the argument:
auto r2 = std::make_reverse_iterator(v.end());     // points at 5
auto r2end = std::make_reverse_iterator(v.begin());
// [r2, r2end) traverses v back-to-front: 5, 4, 3, 2, 1
```

It is most valuable inside templates, where the iterator type is a dependent name and writing it out is both verbose and fragile:

```cpp
// Listing 22.3: a generic "find from the back" using the factory
template <typename It, typename T>
It find_last(It first, It last, const T& value) {
    auto rfirst = std::make_reverse_iterator(last);
    auto rlast  = std::make_reverse_iterator(first);
    auto found  = std::find(rfirst, rlast, value);
    return found == rlast ? last : std::prev(found.base());
}
```

The factory carries the standard reverse-iterator semantics: a reverse iterator built from position `p` dereferences to `*(p-1)`, so `make_reverse_iterator(container.end())` points at the last element and `make_reverse_iterator(container.begin())` is the reverse end.

---

## 22.3 Null (Value-Initialized) Forward Iterators

C++14 tightened the iterator requirements so that **value-initialized forward iterators may be compared, and all value-initialized iterators of the same type compare equal** — regardless of which container (if any) they came from. Before this, a default-constructed forward iterator was *singular*: comparing it was undefined behavior, so it could not portably be used as a sentinel.

```cpp
// Listing 22.4: a value-initialized iterator is a well-defined "null" sentinel
#include <vector>

std::vector<int>::iterator a{};   // value-initialized -> null forward iterator
std::vector<int>::iterator b{};   // also null

bool same = (a == b);             // C++14: well-defined, and true
```

The guarantee makes a default-constructed iterator usable as a portable **"end of an empty range" / not-found marker** — the iterator analogue of `nullptr`. Generic code can value-initialize an iterator to mean "no position" and compare against it safely:

```cpp
// Listing 22.5: using a null iterator as an explicit "unset" state
template <typename It>
class OptionalCursor {
    It pos_{};                              // null forward iterator = "unset"
public:
    bool engaged() const { return pos_ != It{}; }   // compare against the null value
    void set(It p) { pos_ = p; }
    It   get() const { return pos_; }
};
```

Two value-initialized iterators of the same type form an **empty range** `[It{}, It{})`, which any algorithm processes as zero elements. The contract is narrow but important: it applies to *value-initialized* iterators (default-constructed `It{}`), not to arbitrary singular iterators left over from a destroyed container — those remain invalid.

---

## 22.4 Two-Range `<algorithm>` Overloads

This is the most consequential addition in the chapter. C++11's `std::equal`, `std::mismatch`, and `std::is_permutation` came in a **three-iterator** form: `(first1, last1, first2)`. They receive the end of the *first* range but only the *beginning* of the second, and simply assume the second range is at least as long. If it is shorter, the algorithm reads past its end — **undefined behavior**, and a real source of crashes and silent corruption.

```cpp
// Listing 22.6: the three-iterator form is unsafe when ranges differ in length
#include <algorithm>
#include <vector>

std::vector<int> a{1, 2, 3, 4, 5};
std::vector<int> b{1, 2, 3};

// THREE-iterator form: reads b[3], b[4] -- PAST THE END of b. UB.
bool bad = std::equal(a.begin(), a.end(), b.begin());   // do NOT do this
```

**C++14 added four-iterator overloads** — `(first1, last1, first2, last2)` — that take the end of the second range too. They compare both lengths and never read past either end: `equal` returns `false` immediately if the ranges differ in length, `mismatch` stops at whichever range ends first, and `is_permutation` knows both sizes up front.

```cpp
// Listing 22.7: the C++14 four-iterator form is length-safe
#include <algorithm>
#include <vector>

std::vector<int> a{1, 2, 3, 4, 5};
std::vector<int> b{1, 2, 3};

bool eq = std::equal(a.begin(), a.end(), b.begin(), b.end());  // false: sizes differ
                                                               // and NO overrun

auto m = std::mismatch(a.begin(), a.end(), b.begin(), b.end());
// stops at b.end(); m.first points into a at the first unmatched/leftover position

std::vector<int> p{3, 1, 2};
bool perm = std::is_permutation(b.begin(), b.end(), p.begin(), p.end());  // true
```

Each algorithm also has a four-iterator overload taking a custom binary predicate, mirroring the three-iterator forms:

```cpp
// Listing 22.8: four-iterator form with a predicate
bool ci_equal = std::equal(s1.begin(), s1.end(), s2.begin(), s2.end(),
                           [](char x, char y) {
                               return std::tolower(x) == std::tolower(y);
                           });
```

The three-iterator overloads were not removed — they remain for the case where you have *already* verified the lengths match — but the four-iterator forms should be your default. They turn a latent buffer overrun into a correct `false`.

> **Godhood tip:** treat the three-iterator `equal`/`mismatch`/`is_permutation` as a code smell in review. Unless a comment proves the second range's length is guaranteed, the four-iterator form is the only safe choice — and it is also the one that gives the right answer for unequal lengths instead of UB.

---

## 22.5 Professional Insights

**Use the non-member `c`/`r` accessors in every generic algorithm.** `std::cbegin(r)`/`std::cend(r)` give you a read-only traversal of *any* range — container or raw array — without assuming a member interface or spelling a dependent `const_iterator` type. Defaulting to them makes template code work uniformly across more argument types and signals "I will not mutate this range."

**Prefer `make_reverse_iterator` to a hand-spelled `reverse_iterator<…>`.** In generic code the underlying iterator is a dependent type; the factory deduces it, keeping reverse-traversal helpers short and refactor-stable. It is the same ergonomic win as the other `make_` factories.

**A value-initialized iterator is the portable iterator equivalent of `nullptr`.** When you need an "unset position" or an empty-range sentinel in generic code, value-initialize the iterator type and compare against `It{}` — C++14 makes that comparison defined. Don't, however, treat *any* singular iterator as comparable; the guarantee is specifically for value-initialized ones.

**Make the four-iterator `equal`/`mismatch`/`is_permutation` your default — always.** The three-iterator forms silently read past the end of the second range when lengths differ, which is undefined behavior and a genuine security and stability bug. Passing both ends costs nothing, removes the overrun, and returns the correct result for unequal-length inputs. In latency-critical and security-sensitive code especially, the four-iterator overload is the only defensible choice.
