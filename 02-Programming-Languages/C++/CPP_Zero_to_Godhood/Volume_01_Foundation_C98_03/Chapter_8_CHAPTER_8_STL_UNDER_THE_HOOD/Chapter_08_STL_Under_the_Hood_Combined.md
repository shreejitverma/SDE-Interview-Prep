# Chapter 08: STL Under the Hood

> *Knowing what happens inside the containers is the difference between writing correct code and writing fast code.*

Chapter 7 taught you the STL interface. This chapter pulls back the curtain: how `vector` manages its buffer, why `list` is almost always the wrong choice, what makes `map` O(log n), when iterators become invalid, and the essential idioms every C++ programmer must know. It closes with a preview of C++11 lambdas — the feature that completed the STL's design intent.

---

## Table of Contents

- [8.1 `std::vector` Internals](#81-stdvector-internals)
- [8.2 `std::deque` Internals](#82-stddeque-internals)
- [8.3 `std::list` — The Cache-Miss Machine](#83-stdlist--the-cache-miss-machine)
- [8.4 Associative Container Internals (Red-Black Trees)](#84-associative-container-internals-red-black-trees)
- [8.5 Iterator Invalidation: The Complete Reference](#85-iterator-invalidation-the-complete-reference)
- [8.6 Container Adapters Under the Hood](#86-container-adapters-under-the-hood)
- [8.7 Algorithm Internals](#87-algorithm-internals)
- [8.8 The Erase-Remove Idiom](#88-the-erase-remove-idiom)
- [8.9 Essential STL Patterns](#89-essential-stl-patterns)
- [8.10 Algorithm Complexity Reference](#810-algorithm-complexity-reference)
- [8.11 Container and Iterator Comparison Tables](#811-container-and-iterator-comparison-tables)
- [8.12 Forward Reference: C++11 Lambdas and Functional Programming](#812-forward-reference-c11-lambdas-and-functional-programming)
- [8.13 Professional Insights: When Not to Use the STL](#813-professional-insights-when-not-to-use-the-stl)

---

## 8.1 `std::vector` Internals

Every `std::vector` object contains exactly **three pointers**:

```
  vector<int> object:
  ┌──────────────────┐
  │ start            │──► ┌───┬───┬───┬───┬───┐
  │ finish           │──► │ 1 │ 2 │ 3 │   │   │ (contiguous buffer)
  │ end_of_storage   │────────────────────────► end of buffer
  └──────────────────┘
```

- **`start`**: points to the first element.
- **`finish`**: points one-past-the-last active element. `size() = finish - start`.
- **`end_of_storage`**: points one-past-the-last allocated byte. `capacity() = end_of_storage - start`.

### 8.1.1 Growth Strategy

When `size() == capacity()` and you call `push_back()`:

1. Allocate a new buffer — typically **1.5× or 2×** the current capacity.
2. **Copy** every element to the new buffer (in C++98; C++11 would move them).
3. Destroy the old buffer.

This is why `push_back` has **amortised O(1)** cost: after N doublings, the total work done across all operations is proportional to 2N copies, giving O(1) per push on average.

```cpp
// Listing 8.1: Observing reallocation
#include <vector>
#include <iostream>
using namespace std;

int main() {
    vector<int> v;
    size_t last_cap = 0;
    for (int i = 0; i < 20; ++i) {
        v.push_back(i);
        if (v.capacity() != last_cap) {
            cout << "size=" << v.size()
                 << " capacity=" << v.capacity() << "\n";
            last_cap = v.capacity();
        }
    }
    return 0;
}
// Typical output shows capacity jumps: 1, 2, 4, 8, 16, 32...
```

**`reserve(n)` is the fix:** it pre-allocates a buffer for `n` elements so no reallocations occur:

```cpp
// Listing 8.2: Using reserve to avoid reallocations
#include <vector>
using namespace std;

int main() {
    vector<int> v;
    v.reserve(1000);          // One allocation only
    for (int i = 0; i < 1000; ++i)
        v.push_back(i);       // Never reallocates
    return 0;
}
```

### 8.1.2 Shrinking a Vector

After erasing elements, `capacity()` does not decrease automatically. To reclaim the memory, use the **swap trick** (C++98 idiom; `shrink_to_fit()` is C++11):

```cpp
// Listing 8.3: Swap trick to release excess capacity
#include <vector>
using namespace std;

int main() {
    vector<int> v(1000);
    v.erase(v.begin() + 10, v.end()); // size=10, capacity still 1000
    vector<int>(v).swap(v);           // Swap with a fresh tight-fit copy
    // Now v.capacity() == 10
    return 0;
}
```

---

## 8.2 `std::deque` Internals

`std::deque` is **not** a single contiguous array. It is a dynamic array of pointers to **fixed-size chunks**:

```
  deque internal "map" (array of chunk pointers):
  ┌──────────────────────────────────────────────┐
  │ chunk[0] ptr │ chunk[1] ptr │ chunk[2] ptr  │
  └──────┬───────────────┬───────────────┬───────┘
         ▼               ▼               ▼
    ┌──┬──┬──┐      ┌──┬──┬──┐      ┌──┬──┬──┐
    │  │  │  │      │  │  │  │      │  │  │  │  (fixed blocks)
    └──┴──┴──┘      └──┴──┴──┘      └──┴──┴──┘
```

A `deque` iterator is a **smart pointer** that stores:
- A pointer to the current element.
- A pointer to the chunk containing it.
- Pointers to the start and end of that chunk (to know when to jump to the next chunk).

**Implications:**
- `push_front` and `push_back` never reallocate the whole structure — they add a new chunk.
- Random access is O(1) but requires **two pointer dereferences** (slow the chunk pointer, then index into it), making it slower than `vector`.
- Cache locality is between `vector` (bad compared to) and `list` (better than).

---

## 8.3 `std::list` — The Cache-Miss Machine

`std::list` allocates each node individually on the heap:

```cpp
// Listing 8.4: Conceptual list node layout
template<typename T>
struct ListNode {
    T       val;
    ListNode* prev;
    ListNode* next;
};
```

Each node holds **two extra pointers** beyond its payload. On a 64-bit system, every element in a `list<int>` uses 4 bytes of data and 16 bytes of overhead (prev + next) — a **4× memory overhead** just for the links.

**The cache problem:** Nodes are allocated at random heap addresses. Iterating the list means each `++it` follows a pointer to a random location, causing a CPU cache miss. Modern CPUs prefetch contiguous memory; they cannot predict random pointer chains.

**In practice:** iterating a `vector<int>` is often **10–20× faster** than iterating a `list<int>` of the same size because the vector fits in cache lines.

**When `list` is the right choice:**
- You have an iterator to the exact insertion/deletion point and need O(1) operation.
- You need **pointer/reference stability**: after inserting into a `list`, all existing pointers to other elements remain valid forever. `vector` cannot guarantee this after any reallocation.
- You need `splice()` — moving a subrange from one list to another in O(1) without copying elements.

---

## 8.4 Associative Container Internals (Red-Black Trees)

`std::map`, `std::set`, `std::multimap`, and `std::multiset` are all implemented as **Red-Black Trees** (a self-balancing Binary Search Tree variant).

Each tree node stores approximately:

```cpp
// Listing 8.5: Conceptual RB-tree node
template<typename K, typename V>
struct RBNode {
    std::pair<K, V> data;  // The stored key-value pair
    RBNode* left;
    RBNode* right;
    RBNode* parent;
    bool    is_red;        // The "color" bit
};
```

**Properties of the Red-Black Tree:**
- Every path from root to a null leaf has the same number of black nodes (**black-height invariant**).
- No two consecutive red nodes may exist.
- The root is always black.

These invariants guarantee the tree height is bounded by `2 * log2(n+1)`, giving O(log n) worst-case for all operations. Unlike a plain BST, it never degrades to O(n).

**Overhead per element:** 3 pointers + 1 bool + key/value = significant. For small integers, the tree overhead dominates. `map<int,int>` uses roughly **10× more memory** per element than `vector<pair<int,int>>`, which is why you only use `map` when you actually need sorted key-based lookup.

---

## 8.5 Iterator Invalidation: The Complete Reference

This is the most common source of undefined behaviour in STL code. When an iterator is **invalidated**, using it is UB — the program may crash, silently corrupt data, or appear to work.

| Container | Operation | What is invalidated |
| :-------- | :-------- | :------------------ |
| `vector` | Any reallocation (`push_back`, `insert`, `resize`, `reserve` if capacity changes) | **ALL** iterators, pointers, and references |
| `vector` | `insert` / `erase` at position `p` (no reallocation) | All iterators at `p` and after |
| `deque` | `push_front` / `push_back` | All iterators (but not references/pointers to elements!) |
| `deque` | `insert` / `erase` at ends | Iterators only |
| `deque` | `insert` / `erase` in middle | **ALL** iterators, pointers, and references |
| `list` | Any `insert` | No iterators invalidated |
| `list` | `erase` | Only the erased element's iterator |
| `map` / `set` | Any `insert` | No iterators invalidated |
| `map` / `set` | `erase` | Only the erased element's iterator |

**Key pattern:** when erasing during iteration, capture the return value of `erase` (which is the next valid iterator):

```cpp
// Listing 8.6: Safe erase-during-iteration for map
#include <map>
#include <string>

int main() {
    std::map<std::string, int> m;
    m["a"] = 1; m["b"] = 2; m["c"] = 3;

    std::map<std::string, int>::iterator it = m.begin();
    while (it != m.end()) {
        if (it->second == 2)
            it = m.erase(it); // erase returns next valid iterator
        else
            ++it;
    }
    return 0;
}
```

---

## 8.6 Container Adapters Under the Hood

`std::stack`, `std::queue`, and `std::priority_queue` are thin wrappers — they store an instance of an underlying container and expose a restricted interface.

```cpp
// Listing 8.7: How stack is approximately implemented
template<typename T, typename Container = std::deque<T> >
class stack {
    Container c;
public:
    void push(const T& x) { c.push_back(x); }
    void pop()            { c.pop_back(); }
    T&   top()            { return c.back(); }
    bool empty() const    { return c.empty(); }
    size_t size() const   { return c.size(); }
};
```

You can supply `vector` as the underlying container when you need contiguous storage:

```cpp
// Listing 8.8: stack backed by vector (useful for cache performance)
#include <stack>
#include <vector>
using namespace std;

stack<int, vector<int> > fast_stack;
fast_stack.push(1);
fast_stack.push(2);
// Backed by contiguous memory, not deque chunks
```

`priority_queue` uses `std::vector` internally and maintains the heap invariant via `push_heap` / `pop_heap` on every operation.

---

## 8.7 Algorithm Internals

### 8.7.1 `std::sort` — Introsort

`std::sort` is required to be O(N log N) worst-case. Modern implementations use **Introsort**: a hybrid of:
1. **Quicksort** — average O(N log N), in-place, fast in practice.
2. **Heapsort** — guaranteed O(N log N), triggered when recursion depth exceeds 2 × log2(N).
3. **Insertion sort** — O(N²) but very fast for small arrays (N < ~16).

### 8.7.2 `std::stable_sort` — Mergesort

`std::stable_sort` preserves the relative order of equal elements. It uses **mergesort**, which requires O(N) extra memory for the merge buffer. Complexity is O(N log N) when memory is available, O(N log² N) if not.

### 8.7.3 `std::partial_sort` — Heap-Sort Variant

`std::partial_sort(first, middle, last)` guarantees the `[first, middle)` range is sorted with the smallest elements. Internally it uses a heap of size `middle - first` over `[first, last)`. Complexity: O(N log k) where k = `middle - first`.

### 8.7.4 `std::nth_element` — Introselect

`std::nth_element(first, nth, last)` rearranges the range such that `*nth` is what it would be if the range were sorted. Elements before `*nth` are `<=` it; elements after are `>=` it. No guarantee on relative order within the groups. Average O(N) via **introselect** (quickselect + median-of-3 pivoting).

---

## 8.8 The Erase-Remove Idiom

`std::remove` does **not** erase elements from a container — it rearranges them so that the "removed" values are compacted to the end, then returns an iterator to the first "removed" element. You must follow it with `container.erase()`:

```cpp
// Listing 8.9: The erase-remove idiom (standard pattern)
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

int main() {
    vector<int> v;
    v.push_back(1); v.push_back(2); v.push_back(3);
    v.push_back(2); v.push_back(4); v.push_back(2);

    // Step 1: move all 2s to the end, return iterator to the "junk" zone
    vector<int>::iterator new_end = remove(v.begin(), v.end(), 2);
    // v is now {1, 3, 4, ?, ?, ?} where ? are unspecified

    // Step 2: actually erase the junk zone
    v.erase(new_end, v.end());
    // v is now {1, 3, 4}

    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        cout << *it << " ";
    cout << "\n";
    return 0;
}
```

Use `remove_if` with a predicate to erase based on a condition:

```cpp
// Listing 8.10: erase-remove_if
#include <algorithm>
#include <vector>
using namespace std;

bool is_even(int x) { return x % 2 == 0; }

int main() {
    vector<int> v;
    v.push_back(1); v.push_back(2); v.push_back(3);
    v.push_back(4); v.push_back(5);

    v.erase(remove_if(v.begin(), v.end(), is_even), v.end());
    // v is now {1, 3, 5}
    return 0;
}
```

---

## 8.9 Essential STL Patterns

### 8.9.1 Sort and Deduplicate

```cpp
// Listing 8.11: Deduplicate a vector in O(N log N)
#include <algorithm>
#include <vector>
using namespace std;

int main() {
    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
    vector<int> v(arr, arr + 8);

    sort(v.begin(), v.end());                    // Required by unique
    vector<int>::iterator new_end = unique(v.begin(), v.end()); // Move dups to back
    v.erase(new_end, v.end());                   // Erase dups
    // v: {1, 2, 3, 4, 5, 6, 9}
    return 0;
}
```

### 8.9.2 Frequency Counting with `map`

```cpp
// Listing 8.12: Frequency map pattern
#include <map>
#include <string>
#include <iostream>
using namespace std;

int main() {
    const char* words[] = {"apple", "banana", "apple", "cherry", "banana"};
    int n = sizeof(words) / sizeof(words[0]);

    map<string, int> frequency;
    for (int i = 0; i < n; ++i)
        frequency[words[i]]++;

    for (map<string, int>::iterator it = frequency.begin();
         it != frequency.end(); ++it)
        cout << it->first << ": " << it->second << "\n";
    // Output (sorted): apple: 2, banana: 2, cherry: 1
    return 0;
}
```

### 8.9.3 Merge Two Sorted Ranges

```cpp
// Listing 8.13: Merge two sorted vectors
#include <algorithm>
#include <vector>
#include <iterator>
using namespace std;

int main() {
    int a1[] = {1, 3, 5};
    int a2[] = {2, 4, 6};
    vector<int> v1(a1, a1 + 3);
    vector<int> v2(a2, a2 + 3);
    vector<int> merged;

    merge(v1.begin(), v1.end(),
          v2.begin(), v2.end(),
          back_inserter(merged));
    // merged: {1, 2, 3, 4, 5, 6}
    return 0;
}
```

### 8.9.4 Custom Sort (Multi-Criteria)

```cpp
// Listing 8.14: Sort by multiple fields with a predicate
#include <algorithm>
#include <vector>
#include <string>
using namespace std;

struct Person {
    string name;
    int age;
};

bool by_age_then_name(const Person& a, const Person& b) {
    if (a.age != b.age) return a.age < b.age;
    return a.name < b.name;
}

int main() {
    Person people[] = {{"Alice", 30}, {"Bob", 25}, {"Carol", 30}};
    int n = sizeof(people) / sizeof(people[0]);
    vector<Person> v(people, people + n);

    sort(v.begin(), v.end(), by_age_then_name);
    // Sorted: Bob(25), Alice(30), Carol(30)
    return 0;
}
```

### 8.9.5 The `back_inserter` Pattern

`back_inserter` creates an output iterator that calls `push_back` on a container. Use it wherever an algorithm writes to an output range but you want it to grow the destination dynamically:

```cpp
// Listing 8.15: back_inserter with copy and transform
#include <algorithm>
#include <vector>
#include <iterator>
using namespace std;

int square(int x) { return x * x; }

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    vector<int> src(arr, arr + 5);
    vector<int> dst;    // empty — safe because back_inserter grows it

    transform(src.begin(), src.end(), back_inserter(dst), square);
    // dst: {1, 4, 9, 16, 25}
    return 0;
}
```

---

## 8.10 Algorithm Complexity Reference

```
SEARCHING
  find, find_if, find_if_not    O(n)
  count, count_if               O(n)
  search, search_n              O(n*m)
  binary_search                 O(log n)   (sorted range)
  lower_bound, upper_bound      O(log n)   (sorted range)

SORTING
  sort                          O(n log n) average (Introsort)
  stable_sort                   O(n log n)
  partial_sort                  O(n log k) k = output size
  nth_element                   O(n)       average

HEAP OPERATIONS
  make_heap                     O(n)
  push_heap                     O(log n)
  pop_heap                      O(log n)

MODIFYING
  copy, transform, fill         O(n)
  reverse, rotate, unique       O(n)
  remove, partition             O(n)

NUMERIC (<numeric>)
  accumulate                    O(n)
  inner_product                 O(n)
  partial_sum                   O(n)

SET OPERATIONS (sorted ranges)
  set_union, set_intersection   O(n+m)
  set_difference                O(n+m)
```

---

## 8.11 Container and Iterator Comparison Tables

### Container Complexity

```
               Insert  Delete  Search  Random  Memory Layout
vector         O(n)    O(n)    O(n)    O(1)    Contiguous
deque          O(n)    O(n)    O(n)    O(1)    Block chunks
list           O(1)*   O(1)*   O(n)    --      Scattered
map/set        O(lgn)  O(lgn)  O(lgn)  --      Red-Black Tree
queue          O(1)    O(1)    --      --      Adapter (deque)
stack          O(1)    O(1)    --      --      Adapter (deque)
priority_queue O(lgn)  O(lgn)  --      --      Heap (vector)
```
*Given an iterator to the target node.

### Iterator Categories by Container

```
Container           Iterator Category    Bidirectional   Random Access
vector              Random Access        Yes             Yes
deque               Random Access        Yes             Yes
list                Bidirectional        Yes             No
map / set           Bidirectional        Yes             No
multimap / multiset Bidirectional        Yes             No
stack               None (no iterators)
queue               None (no iterators)
priority_queue      None (no iterators)
```

---

## 8.12 Forward Reference: C++11 Lambdas and Functional Programming

In C++98, passing custom logic to algorithms requires a **functor** — a class with `operator()`:

```cpp
// Listing 8.16: C++98 functor — verbose but equivalent
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

struct IsGreaterThan {
    int threshold;
    explicit IsGreaterThan(int t) : threshold(t) {}
    bool operator()(int value) const { return value > threshold; }
};

int main() {
    int arr[] = {10, 20, 30, 40};
    vector<int> v(arr, arr + 4);

    // Count elements greater than 25
    int n = count_if(v.begin(), v.end(), IsGreaterThan(25)); // 2
    cout << n << "\n";
    return 0;
}
```

**C++11 replaces this with lambda expressions** — anonymous functions defined inline:

```cpp
// Listing 8.17: C++11 lambda — same logic, zero boilerplate
// (For reference: not valid C++98 syntax)
// int threshold = 25;
// int n = count_if(v.begin(), v.end(),
//     [threshold](int value) { return value > threshold; }
// );
```

Lambda anatomy: `[captures](parameters) -> return_type { body }`

| Capture form | Meaning |
| :----------- | :------ |
| `[]` | Capture nothing |
| `[x]` | Capture `x` by value (read-only copy) |
| `[&x]` | Capture `x` by reference (can modify original) |
| `[=]` | Capture all locals by value |
| `[&]` | Capture all locals by reference |
| `[this]` | Capture `this` pointer (for member access) |

**`std::function`** (C++11 `<functional>`) wraps any callable (lambda, function pointer, or functor) matching a given signature. It enables storing callables as class members or passing them through non-template APIs. It incurs a small overhead due to type-erasure and possible heap allocation.

In C++98/03: use functors + `operator()` wherever C++11 would use a lambda.

---

## 8.13 Professional Insights: When Not to Use the STL

### 8.13.1 The Rule of Godhood: Reach for an Algorithm First

Before writing a `for` loop, ask: "Is there an STL algorithm for this?" Usually yes. `std::find_if`, `std::count_if`, `std::transform`, `std::accumulate` cover 90% of loop patterns with zero risk of off-by-one errors.

### 8.13.2 The Cache Argument Against `list`

Modern CPUs are **100×–1000× faster** at sequential memory reads (due to hardware prefetchers) than random memory reads (cache misses). A `vector<int>` with 1 million elements will iterate in microseconds; a `list<int>` with the same elements will take milliseconds. Choose `list` only when:
- You genuinely need iterator stability after insertions.
- You need O(1) `splice`.

### 8.13.3 `map` vs. `vector` for Lookup

`map` gives O(log n) lookup. A sorted `vector<pair<K,V>>` searched with `lower_bound` also gives O(log n) lookup, with far better cache performance (contiguous memory, single allocation). For read-heavy workloads where the container is built once and queried many times, the sorted vector often outperforms `map` by 2–4×.

### 8.13.4 The Destination-Size Trap

Algorithms like `std::copy`, `std::transform`, and `std::fill` write into an existing range — they do **not** allocate memory. Writing past the end of the output range is UB. Always either:
- Pre-size the destination: `vector<int> dst(src.size());`
- Or use `back_inserter(dst)` to append via `push_back`.

### 8.13.5 Avoid `operator[]` on `map` When Reading

`map::operator[]` inserts a **default-constructed value** if the key is absent. This can silently bloat your map. Use `find()` for safe lookup:

```cpp
// Listing 8.18: Safe map lookup
#include <map>
#include <string>
#include <iostream>
using namespace std;

int main() {
    map<string, int> m;
    m["key"] = 42;

    // WRONG: inserts "missing" → 0 if absent
    // cout << m["missing"] << "\n";

    // CORRECT: read-only safe lookup
    map<string, int>::const_iterator it = m.find("missing");
    if (it != m.end())
        cout << it->second << "\n";
    else
        cout << "Key not found\n";
    return 0;
}
```
