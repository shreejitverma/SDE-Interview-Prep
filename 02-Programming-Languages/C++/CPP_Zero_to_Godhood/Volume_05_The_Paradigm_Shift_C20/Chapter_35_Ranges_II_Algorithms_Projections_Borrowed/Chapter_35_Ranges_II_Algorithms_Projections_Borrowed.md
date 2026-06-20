# Chapter 35: Ranges II — Range Algorithms, Projections, and Borrowed Ranges

> *Chapter 34 covered views and lazy pipelines. This chapter covers the other half of the ranges library: the `std::ranges::*` algorithms that take a whole range instead of an iterator pair, the projection parameter that lets one algorithm sort or search by a member without a custom comparator, and the borrowed-range model that makes returning an iterator into a temporary a compile error instead of a dangling-pointer bug.*

The range algorithms are not merely convenience wrappers — they are **constrained** (every parameter checked by the concepts of Chapter 33), they accept **projections** (a second function that transforms each element before the algorithm inspects it), and they return **borrow-aware** results (`std::ranges::dangling` when handing back an iterator would dangle). Together these turn the classic `<algorithm>` into a safer, more expressive toolkit that composes directly with the views of Chapter 34.

---

## Table of Contents

- [35.1 Range Algorithms: One Argument Instead of Two](#351-range-algorithms-one-argument-instead-of-two)
- [35.2 The Constrained-Algorithm Guarantee](#352-the-constrained-algorithm-guarantee)
- [35.3 Projections](#353-projections)
- [35.4 Combining Projections, Comparators, and Predicates](#354-combining-projections-comparators-and-predicates)
- [35.5 Borrowed Ranges and std::ranges::dangling](#355-borrowed-ranges-and-stdrangesdangling)
- [35.6 borrowed_range and view Lifetime Safety](#356-borrowed_range-and-view-lifetime-safety)
- [35.7 Algorithms Over Pipelines](#357-algorithms-over-pipelines)
- [35.8 The C++20 Algorithm Catalogue and What Is Missing](#358-the-c20-algorithm-catalogue-and-what-is-missing)
- [35.9 Professional Insights](#359-professional-insights)

---

## 35.1 Range Algorithms: One Argument Instead of Two

For nearly every algorithm in `<algorithm>` and `<numeric>`, C++20 adds a `std::ranges::` counterpart that accepts a single range argument (and still offers the iterator-pair overload when you need a sub-range).

```cpp
// Listing 35.1: ranges algorithms take a whole range
#include <algorithm>
#include <ranges>
#include <vector>

int main() {
    std::vector<int> v{4, 2, 5, 1, 3};

    std::ranges::sort(v);                          // whole-range sort
    bool has3 = std::ranges::binary_search(v, 3);  // whole-range search
    auto it   = std::ranges::find(v, 5);           // whole-range find

    // The iterator-pair form still exists for sub-ranges:
    std::ranges::sort(v.begin(), v.begin() + 3);
}
```

This eliminates the most common iterator bug class — passing `begin()` of one container and `end()` of another — because there is simply one range to name. It also composes with views: any view from Chapter 34 is a range and can be handed to a range algorithm directly.

---

## 35.2 The Constrained-Algorithm Guarantee

Every `std::ranges::` algorithm is **constrained with concepts**. `std::ranges::sort` requires a `random_access_range` whose iterator is `sortable`; `std::ranges::find` requires an `input_range`. The payoff is the Chapter 32 diagnostic: hand `sort` a `std::list` and you get a one-line "constraints not satisfied: list iterator is not random_access" at the call site — not a template explosion inside the sort implementation.

```cpp
// Listing 35.2: the constraint fires at the call site, not deep inside
#include <algorithm>
#include <list>

int main() {
    std::list<int> lst{3, 1, 2};
    std::ranges::sort(lst);
    // error: no matching call; constraint 'sortable' / 'random_access_range'
    //        not satisfied for std::list<int>.  (clear, localized)
}
```

The classic `std::sort` would have failed with an obscure error about `operator-` on list iterators, somewhere inside the introsort. The constraint moves the failure to where you can act on it.

---

## 35.3 Projections

A **projection** is an extra argument — a function, member pointer, or lambda — applied to each element *before* the algorithm examines it. It replaces the most common reason for writing a custom comparator: "compare by this member." The projection slot is the last parameter and defaults to `std::identity` (no-op).

```cpp
// Listing 35.3: sort/find by a member using a projection, no custom comparator
#include <algorithm>
#include <ranges>
#include <vector>
#include <string>

struct User { int id; std::string name; };

int main() {
    std::vector<User> users{{2,"Bob"},{1,"Alice"},{3,"Carol"}};

    std::ranges::sort(users, {}, &User::id);     // sort by id  ({} = default less<>)
    std::ranges::sort(users, {}, &User::name);   // sort by name

    // find by a projected field:
    auto it = std::ranges::find(users, 3, &User::id);   // user whose id == 3

    // max by a projected field:
    auto oldest = std::ranges::max(users, {}, &User::id);
}
```

The signature pattern is `algorithm(range, comparator = {}, projection = identity)`. A member-pointer projection (`&User::id`) is the idiomatic spelling and reads as "by id." This single feature removes a large fraction of the throwaway lambdas that littered pre-C++20 algorithm calls.

---

## 35.4 Combining Projections, Comparators, and Predicates

Projections compose with explicit comparators and predicates. The projection transforms the element; the comparator/predicate then operates on the projected value.

```cpp
// Listing 35.4: projection + comparator, and projection + predicate
#include <algorithm>
#include <ranges>
#include <vector>
#include <string>
#include <cctype>

struct User { int id; std::string name; };

int main() {
    std::vector<User> users{{2,"bob"},{1,"ALICE"},{3,"Carol"}};

    // Sort by name length, descending: projection = name.size(), comparator = greater
    std::ranges::sort(users, std::ranges::greater{},
                      [](const User& u){ return u.name.size(); });

    // count_if with a projection: predicate sees the projected value (the id)
    auto big_ids = std::ranges::count_if(users,
                      [](int id){ return id >= 2; },   // predicate on projected value
                      &User::id);                        // projection
}
```

Note `std::ranges::less`, `std::ranges::greater`, etc. are the **transparent, constrained** comparator objects to prefer here — they are `totally_ordered_with`-constrained and handle heterogeneous comparison correctly.

---

## 35.5 Borrowed Ranges and std::ranges::dangling

A subtle but serious pre-C++20 bug: an algorithm returns an iterator into a range that was a **temporary**, so the iterator dangles the instant the full expression ends.

```cpp
// Listing 35.5: ranges protects against returning an iterator into a temporary
#include <algorithm>
#include <ranges>
#include <vector>

std::vector<int> make_vec() { return {1, 2, 3}; }

int main() {
    // std::max_element(make_vec().begin(), make_vec().end()); // classic UB territory

    auto it = std::ranges::max_element(make_vec());
    // 'it' has type std::ranges::dangling — NOT a real iterator.
    // *it would not compile: the library refuses to hand back a dangling iterator.
}
```

When you pass an **rvalue** range that does not model `borrowed_range`, a range algorithm that would return an iterator instead returns the tag type **`std::ranges::dangling`**. Attempting to dereference or use it as an iterator is a *compile error*, converting a runtime dangling-pointer crash into a build-time diagnostic. The companion alias `std::ranges::borrowed_iterator_t<R>` is what algorithms use to compute "real iterator or `dangling`."

---

## 35.6 borrowed_range and view Lifetime Safety

A range models **`std::ranges::borrowed_range`** when its iterators remain valid even after the range object itself is destroyed — i.e., the range does not own the elements. Lvalue containers, `std::span`, `std::string_view`, and most views are borrowed ranges; an rvalue `std::vector` is not (it owns and will destroy its storage).

```cpp
// Listing 35.6: borrowed_range types return real iterators even as rvalues
#include <algorithm>
#include <ranges>
#include <span>
#include <vector>

int main() {
    std::vector<int> store{5, 2, 8};

    // span is a borrowed_range: it does not own, so an rvalue span is safe.
    auto it = std::ranges::max_element(std::span{store});
    // 'it' is a real iterator into 'store' — safe, *it == 8.

    // To opt a user-defined view into this guarantee, specialize the variable template:
    // template<> inline constexpr bool std::ranges::enable_borrowed_range<MyView> = true;
}
```

You opt a custom view into borrowed-ness with the `std::ranges::enable_borrowed_range` variable-template specialization — asserting that its iterators outlive it. This is the formal contract the dangling protection of Section 35.5 keys off.

---

## 35.7 Algorithms Over Pipelines

Because views are ranges, range algorithms apply directly to pipelines — the two halves of the library compose without glue.

```cpp
// Listing 35.7: a view pipeline fed straight into a range algorithm
#include <algorithm>
#include <ranges>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v{5, 1, 8, 2, 9, 3};
    namespace vws = std::views;

    // Build a lazy view of the evens, then reduce it with a range algorithm:
    auto evens = v | vws::filter([](int x){ return x % 2 == 0; });

    int total       = std::ranges::fold_left(evens, 0, std::plus{}); // C++23: see note
    auto max_even   = std::ranges::max(evens);                        // C++20: fine
    bool any_big    = std::ranges::any_of(evens, [](int x){ return x > 6; });

    std::cout << *max_even << ' ' << any_big << '\n';
}
```

Note `std::ranges::fold_left` is **C++23**; in C++20 use `std::reduce`/`std::accumulate` on the materialized view, or a manual loop. `std::ranges::max`, `any_of`, `for_each`, `count_if`, etc. are all C++20 and accept pipelines directly.

---

## 35.8 The C++20 Algorithm Catalogue and What Is Missing

Practically the entire `<algorithm>` set has a `std::ranges::` form in C++20: `for_each`, `find`/`find_if`/`find_if_not`, `count`/`count_if`, `all_of`/`any_of`/`none_of`, `copy`/`copy_if`/`move`, `transform`, `sort`/`stable_sort`/`partial_sort`, `nth_element`, `lower_bound`/`upper_bound`/`binary_search`/`equal_range`, `min`/`max`/`minmax`/`min_element`/`max_element`/`clamp`, `unique`, `reverse`, `rotate`, `partition`, `merge`, `set_*`, `next_permutation`, and more.

| Want | C++20 status |
|------|--------------|
| `std::ranges::sort`, `find`, `transform`, `max_element` | ✅ available |
| projections on all the above | ✅ available |
| `std::ranges::fold_left` / `fold_right` | ❌ C++23 |
| `std::ranges::to` (materialize) | ❌ C++23 |
| range versions of `<numeric>` (`reduce`, `inclusive_scan`) | ❌ mostly C++23 |

For numeric reductions in C++20, fall back to the non-ranges `<numeric>` algorithms on a container or materialized view.

---

## 35.9 Professional Insights

**Prefer the projection slot to a throwaway comparator lambda.** `std::ranges::sort(users, {}, &User::id)` is clearer, less error-prone, and often better-optimized than `sort(users, [](auto&a,auto&b){return a.id<b.id;})`. Reserve explicit comparators for genuinely custom orderings; use a member-pointer projection for "by this field."

**Let the dangling protection do its job — do not cast it away.** When a range algorithm hands you `std::ranges::dangling`, that is the library catching a lifetime bug at compile time. The fix is to bind the range to a named lvalue first (or use a borrowed range like `std::span`), never to contrive a way to extract an iterator from a temporary. The whole point is that the compile error replaced a crash.

**Opt your custom views into `borrowed_range` only when the guarantee is true.** Specializing `enable_borrowed_range` asserts that your view's iterators outlive the view object. If that is not actually true (the view owns a buffer), enabling it reintroduces exactly the dangling bug the system prevents. Enable it for non-owning adapters; leave it off for anything that holds storage.

**Watch the C++20/23 line in numeric and materialization code.** `fold_left`, `ranges::to`, and most ranges `<numeric>` algorithms are C++23. In C++20, reduce with `std::reduce`/`std::accumulate` and materialize with iterator-pair constructors. Mixing these up is the most common compile failure when porting range-heavy code to a strict C++20 build.

**Reach for constrained range algorithms at API boundaries for the diagnostics alone.** Even when an iterator-pair call would work, the `std::ranges::` form gives callers a localized, concept-named error if they pass an unsuitable range — the same maintainability win as constraining your own templates, inherited for free from the standard library.
