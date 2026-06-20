# Chapter 07: Standard Template Library Core

> *Generic containers, iterators, and algorithms — the vocabulary every C++ program uses every day.*

The **Standard Template Library (STL)** is the most important library in C++. It delivers production-quality implementations of the most common data structures (containers), traversal abstractions (iterators), and algorithms — all expressed as templates, so they work with any element type. This chapter covers the C++98/03 STL in full: the nine containers, the five iterator categories, the algorithm header, `std::string`, streams, and functors.

---

## Table of Contents

- [7.1 STL Architecture](#71-stl-architecture)
- [7.2 Container Characteristics at a Glance](#72-container-characteristics-at-a-glance)
- [7.3 `std::vector` — The Dynamic Array](#73-stdvector--the-dynamic-array)
- [7.4 `std::deque` — Double-Ended Queue](#74-stddeque--double-ended-queue)
- [7.5 `std::list` — Doubly Linked List](#75-stdlist--doubly-linked-list)
- [7.6 `std::map` — Sorted Key-Value Store](#76-stdmap--sorted-key-value-store)
- [7.7 `std::set` — Sorted Unique Keys](#77-stdset--sorted-unique-keys)
- [7.8 `std::multimap` and `std::multiset`](#78-stdmultimap-and-stdmultiset)
- [7.9 `std::stack` — LIFO Adapter](#79-stdstack--lifo-adapter)
- [7.10 `std::queue` — FIFO Adapter](#710-stdqueue--fifo-adapter)
- [7.11 `std::priority_queue` — Heap Adapter](#711-stdpriority_queue--heap-adapter)
- [7.12 Iterators](#712-iterators)
- [7.13 Algorithms](#713-algorithms)
- [7.14 `std::string`](#714-stdstring)
- [7.15 Streams and String Streams](#715-streams-and-string-streams)
- [7.16 Functors (Function Objects)](#716-functors-function-objects)
- [7.17 Advanced String Operations](#717-advanced-string-operations)
- [7.18 Professional Insights: STL Internals and Pitfalls](#718-professional-insights-stl-internals-and-pitfalls)

---

## 7.1 STL Architecture

The STL is organised around four cooperating components:

```
          STL (Standard Template Library)
                       |
   -----------------------------------------
   |              |            |           |
CONTAINERS     ITERATORS    ALGORITHMS   FUNCTORS
   |              |            |           |
Sequence       Input        Searching    Predicates
Associative    Output       Sorting      Comparators
Adapters       Forward      Modifying
               Bidir        Numeric
               Random
```

**Containers** store data. **Iterators** point into containers and define ranges. **Algorithms** operate on iterator ranges, blind to the underlying container. **Functors** parametrise algorithm behaviour without calling convention overhead.

**Key advantages of this design:**
- **Generic programming**: algorithms written once work with any container of any element type.
- **Separation of concerns**: swapping from `vector` to `list` requires changing only the container type, not the algorithm.
- **Performance**: implementations are heavily optimised; the abstractions typically compile away to zero overhead.

---

## 7.2 Container Characteristics at a Glance

| Container | Type | Insert | Delete | Search | Random Access | Memory Layout |
|-----------|------|--------|--------|--------|---------------|---------------|
| `vector` | Sequence | O(n) amort. O(1) back | O(n) amort. O(1) back | O(n) | O(1) | Contiguous |
| `deque` | Sequence | O(1) ends, O(n) mid | O(1) ends, O(n) mid | O(n) | O(1) | Block chunks |
| `list` | Sequence | O(1)† | O(1)† | O(n) | None | Scattered |
| `map` | Associative | O(log n) | O(log n) | O(log n) | None | Red-Black Tree |
| `set` | Associative | O(log n) | O(log n) | O(log n) | None | Red-Black Tree |
| `multimap` | Associative | O(log n) | O(log n) | O(log n) | None | Red-Black Tree |
| `multiset` | Associative | O(log n) | O(log n) | O(log n) | None | Red-Black Tree |
| `stack` | Adapter (deque) | O(1) | O(1) | — | None | Underlying |
| `queue` | Adapter (deque) | O(1) | O(1) | — | None | Underlying |
| `priority_queue` | Adapter (vector) | O(log n) | O(log n) | — | None | Heap |

†Given an iterator to the insertion/deletion point.

---

## 7.3 `std::vector` — The Dynamic Array

**`std::vector`** is the default container. It stores elements contiguously in heap memory, growing automatically by allocating a new buffer when capacity is exceeded. Use it unless you have a specific reason to choose another container.

```cpp
// Listing 7.1: Declaring and initialising vectors
#include <vector>
#include <iostream>
using namespace std;

int main() {
    vector<int> v1;              // Empty
    vector<int> v2(10);          // 10 elements, value-initialised to 0
    vector<int> v3(5, 42);       // 5 elements, all 42
    vector<int> v4(v3);          // Copy

    int arr[] = {1, 2, 3, 4, 5};
    vector<int> v5(arr, arr + 5); // Construct from C array
    return 0;
}
```

### 7.3.1 Accessing Elements

```cpp
// Listing 7.2: Element access
#include <vector>
using namespace std;

int main() {
    vector<int> v;
    v.push_back(10);
    v.push_back(20);

    v[0];          // No bounds check — undefined behaviour if out of range
    v.at(1);       // Throws std::out_of_range if out of range
    v.front();     // First element: 10
    v.back();      // Last element: 20
    return 0;
}
```

### 7.3.2 Modifying a Vector

```cpp
// Listing 7.3: Common mutation operations
#include <vector>
using namespace std;

int main() {
    vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);            // {10, 20, 30}

    v.insert(v.begin() + 1, 15); // {10, 15, 20, 30}
    v.pop_back();                 // {10, 15, 20}
    v.erase(v.begin() + 1);      // {10, 20}
    v.clear();                    // {}
    return 0;
}
```

### 7.3.3 Size and Capacity

```cpp
// Listing 7.4: size vs capacity
#include <vector>
using namespace std;

int main() {
    vector<int> v(10);
    v.size();      // 10 — elements actually stored
    v.capacity();  // >= 10 — allocated slots
    v.empty();     // false

    v.reserve(100); // Allocate space for 100, no new elements
    v.resize(20);   // Add default-initialised elements up to 20
    v.resize(5);    // Truncate to 5 elements
    return 0;
}
```

**Always `reserve()` if you know the final size** — each reallocation copies every element to a new buffer and invalidates all iterators.

### 7.3.4 Iterating

```cpp
// Listing 7.5: Three iteration patterns
#include <vector>
#include <iostream>
using namespace std;

int main() {
    vector<int> v;
    v.push_back(10); v.push_back(20); v.push_back(30);

    // Index loop
    for (size_t i = 0; i < v.size(); ++i)
        cout << v[i] << " ";

    // Iterator loop — preferred
    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        cout << *it << " ";

    // Reverse iterator
    for (vector<int>::reverse_iterator rit = v.rbegin(); rit != v.rend(); ++rit)
        cout << *rit << " ";

    return 0;
}
```

---

## 7.4 `std::deque` — Double-Ended Queue

**`std::deque`** (pronounced "deck") provides O(1) insertion and removal at **both** ends. Unlike `vector`, storage is in fixed-size blocks (not a single contiguous array), so there is no single reallocation. Random access is O(1) but slightly slower than `vector` due to the two-level indirection.

```cpp
// Listing 7.6: deque basics
#include <deque>
#include <iostream>
using namespace std;

int main() {
    deque<int> dq;
    dq.push_back(10);
    dq.push_front(5);   // {5, 10}

    dq.pop_back();      // {5}
    dq.pop_front();     // {}

    dq.push_back(100);
    cout << dq[0] << "\n"; // Random access like vector
    return 0;
}
```

---

## 7.5 `std::list` — Doubly Linked List

**`std::list`** provides O(1) insertion and removal at any position if you have an iterator to the target node. There is no random access. Its non-contiguous layout causes poor cache performance — avoid it in latency-critical code unless the iterator-stability guarantee is essential.

```cpp
// Listing 7.7: list operations
#include <list>
#include <iostream>
using namespace std;

int main() {
    list<int> lst;
    lst.push_back(10);
    lst.push_front(5);   // {5, 10}

    list<int>::iterator it = lst.begin();
    ++it;
    lst.insert(it, 7);   // {5, 7, 10}

    lst.remove(7);       // Remove all nodes with value 7 → {5, 10}
    lst.push_back(10);
    lst.unique();        // Remove consecutive duplicates → {5, 10}
    lst.sort();          // std::sort won't work on list; use member sort()
    return 0;
}
```

---

## 7.6 `std::map` — Sorted Key-Value Store

**`std::map`** is an associative container storing unique key-value pairs sorted by key. Internally implemented as a Red-Black Tree, giving O(log n) for insert, erase, and find.

```cpp
// Listing 7.8: map operations
#include <map>
#include <string>
#include <iostream>
using namespace std;

int main() {
    map<string, int> ages;

    // Insertion
    ages["Alice"] = 30;
    ages["Bob"]   = 25;
    ages.insert(make_pair("Charlie", 28));

    // Access
    cout << ages["Alice"] << "\n"; // 30
    // WARNING: operator[] inserts a default-constructed value if key absent!

    // Safe search
    map<string, int>::iterator it = ages.find("Bob");
    if (it != ages.end())
        cout << "Bob is " << it->second << " years old.\n";

    // Iteration — sorted by key (Alice, Bob, Charlie)
    for (it = ages.begin(); it != ages.end(); ++it)
        cout << it->first << ": " << it->second << "\n";

    return 0;
}
```

**Custom comparator:** provide a comparator struct as the third template argument to change key ordering:

```cpp
// Listing 7.9: map with custom comparator
#include <map>
#include <string>
#include <cstring>

struct CaseInsensitive {
    bool operator()(const std::string& a, const std::string& b) const {
        return strcasecmp(a.c_str(), b.c_str()) < 0;
    }
};

std::map<std::string, int, CaseInsensitive> ci_map;
```

---

## 7.7 `std::set` — Sorted Unique Keys

**`std::set`** stores unique elements in sorted order. Like `map`, backed by a Red-Black Tree.

```cpp
// Listing 7.10: set operations
#include <set>
#include <iostream>
using namespace std;

int main() {
    set<int> s;
    s.insert(10);
    s.insert(5);
    s.insert(10); // Duplicate — ignored. {5, 10}

    if (s.find(5) != s.end())
        cout << "5 is present.\n";

    s.erase(s.begin()); // Erase element 5
    return 0;
}
```

---

## 7.8 `std::multimap` and `std::multiset`

`multimap` allows duplicate keys; `multiset` allows duplicate elements. The same O(log n) complexity as their unique counterparts.

```cpp
// Listing 7.11: multimap with equal_range
#include <map>
#include <string>
#include <iostream>
using namespace std;

int main() {
    multimap<string, int> scores;
    scores.insert(make_pair("Alice", 90));
    scores.insert(make_pair("Alice", 85)); // Allowed

    typedef multimap<string, int>::iterator MMit;
    pair<MMit, MMit> range = scores.equal_range("Alice");
    for (MMit it = range.first; it != range.second; ++it)
        cout << it->second << " "; // 90 85
    return 0;
}
```

---

## 7.9 `std::stack` — LIFO Adapter

**`std::stack`** wraps an underlying container (default `deque`) and exposes a Last-In-First-Out interface. Template signature: `stack<T, Container = deque<T>>`.

```cpp
// Listing 7.12: stack
#include <stack>
#include <iostream>
using namespace std;

int main() {
    stack<int> st;
    st.push(10);
    st.push(20);

    cout << st.top() << "\n"; // 20
    st.pop();                 // Removes 20
    cout << st.top() << "\n"; // 10
    return 0;
}
```

---

## 7.10 `std::queue` — FIFO Adapter

**`std::queue`** wraps `deque` and exposes First-In-First-Out semantics.

```cpp
// Listing 7.13: queue
#include <queue>
#include <iostream>
using namespace std;

int main() {
    queue<int> q;
    q.push(10);
    q.push(20);

    cout << q.front() << "\n"; // 10 (oldest)
    q.pop();                   // Removes 10
    cout << q.back() << "\n";  // 20 (newest)
    return 0;
}
```

---

## 7.11 `std::priority_queue` — Heap Adapter

**`std::priority_queue`** wraps a `vector` and maintains a max-heap. The element with the highest value is always accessible via `top()`. Provide `greater<T>` as the comparator for a min-heap.

```cpp
// Listing 7.14: priority_queue — max and min heap
#include <queue>
#include <vector>
#include <functional>
#include <iostream>
using namespace std;

int main() {
    // Default: max-heap
    priority_queue<int> pq;
    pq.push(10); pq.push(30); pq.push(20);
    cout << pq.top() << "\n"; // 30

    // Min-heap using greater<int>
    priority_queue<int, vector<int>, greater<int> > min_pq;
    min_pq.push(10); min_pq.push(30);
    cout << min_pq.top() << "\n"; // 10
    return 0;
}
```

---

## 7.12 Iterators

An **iterator** is an object that refers to an element in a container and supports operations for advancing through the container. The five categories form a hierarchy:

| Category | Containers | Operations |
|----------|------------|------------|
| **Input** | `istream_iterator` | `*it`, `++it` (single pass, read only) |
| **Output** | `ostream_iterator` | `*it = v`, `++it` (single pass, write only) |
| **Forward** | `slist` (non-standard) | Input + multi-pass |
| **Bidirectional** | `list`, `map`, `set` | Forward + `--it` |
| **Random Access** | `vector`, `deque`, arrays | Bidirectional + `it+n`, `it-it2`, `it[n]` |

Every algorithm specifies the minimum iterator category it requires — you can always substitute a stronger category.

```cpp
// Listing 7.15: Iterator operations and helpers
#include <vector>
#include <iterator>
#include <iostream>
using namespace std;

int main() {
    vector<int> v;
    v.push_back(1); v.push_back(2); v.push_back(3);
    v.push_back(4); v.push_back(5);

    vector<int>::iterator it = v.begin();
    advance(it, 2);                          // Move forward 2: points to 3
    cout << distance(v.begin(), it) << "\n"; // 2
    cout << *it << "\n";                     // 3

    // Prefer ++it over it++ for non-random-access iterators
    ++it; // No temporary copy
    return 0;
}
```

---

## 7.13 Algorithms

Algorithms live in `<algorithm>` and `<numeric>`. They operate on **half-open ranges** `[first, last)` — `last` points past the final element and must never be dereferenced.

### 7.13.1 Non-Modifying Algorithms

```cpp
// Listing 7.16: Non-modifying algorithms
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

int main() {
    int arr[] = {5, 2, 9, 1, 5, 6};
    vector<int> v(arr, arr + 6);

    // find — returns iterator to first match, or end()
    vector<int>::iterator it = find(v.begin(), v.end(), 9);
    if (it != v.end())
        cout << "Found 9 at index " << (it - v.begin()) << "\n";

    // count — how many times does 5 appear?
    cout << count(v.begin(), v.end(), 5) << "\n"; // 2

    return 0;
}
```

### 7.13.2 Modifying Algorithms

```cpp
// Listing 7.17: Modifying algorithms
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    vector<int> v(arr, arr + 5);

    // reverse in-place
    reverse(v.begin(), v.end()); // {5, 4, 3, 2, 1}

    // copy to second vector
    vector<int> v2(5);
    copy(v.begin(), v.end(), v2.begin());

    // replace all 3s with 99
    replace(v.begin(), v.end(), 3, 99);

    // fill with a constant
    fill(v.begin(), v.end(), 0); // {0, 0, 0, 0, 0}

    return 0;
}
```

### 7.13.3 Sorting Algorithms

```cpp
// Listing 7.18: Sorting
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

bool descending(int a, int b) { return a > b; }

int main() {
    int arr[] = {5, 2, 9, 1, 5, 6};
    vector<int> v(arr, arr + 6);

    sort(v.begin(), v.end());              // Ascending: {1, 2, 5, 5, 6, 9}
    sort(v.begin(), v.end(), descending);  // Descending: {9, 6, 5, 5, 2, 1}

    // stable_sort preserves relative order of equal elements
    // partial_sort: only the first k elements need to be sorted
    partial_sort(v.begin(), v.begin() + 3, v.end()); // Smallest 3 in front
    return 0;
}
```

### 7.13.4 Binary Search (Requires Sorted Range)

```cpp
// Listing 7.19: Binary search on sorted ranges
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

int main() {
    int arr[] = {1, 2, 5, 5, 6, 9};
    vector<int> v(arr, arr + 6);

    // binary_search: true/false
    cout << binary_search(v.begin(), v.end(), 5) << "\n"; // true (1)

    // lower_bound: first element >= val
    vector<int>::iterator lo = lower_bound(v.begin(), v.end(), 5);
    // upper_bound: first element > val
    vector<int>::iterator hi = upper_bound(v.begin(), v.end(), 5);
    cout << "Count of 5: " << (hi - lo) << "\n"; // 2

    return 0;
}
```

### 7.13.5 Numeric Algorithms (`<numeric>`)

```cpp
// Listing 7.20: Numeric algorithms
#include <numeric>
#include <vector>
#include <iostream>
using namespace std;

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    vector<int> v(arr, arr + 5);

    // Sum with initial value 0
    int total = accumulate(v.begin(), v.end(), 0); // 15

    // Dot product
    vector<int> v2(arr, arr + 5);
    int dot = inner_product(v.begin(), v.end(), v2.begin(), 0); // 55

    cout << total << "\n" << dot << "\n";
    return 0;
}
```

---

## 7.14 `std::string`

`std::string` is a specialisation of `std::basic_string<char>`. It manages a heap-allocated character buffer with automatic size tracking.

```cpp
// Listing 7.21: std::string fundamentals
#include <string>
#include <iostream>
using namespace std;

int main() {
    string s = "Hello";
    s += " World";               // Concatenation → "Hello World"

    string sub = s.substr(0, 5); // "Hello"

    size_t pos = s.find("World");
    if (pos != string::npos)
        cout << "Found at " << pos << "\n";  // 6

    s.replace(6, 5, "C++");      // "Hello C++"
    s.insert(5, " there");       // "Hello there C++"
    s.erase(5, 6);               // "Hello C++"

    const char* cstr = s.c_str(); // Null-terminated const pointer
    return 0;
}
```

**`string::npos`** is the "not found" sentinel (`static const size_type npos = -1` — the maximum value of `size_t`).

---

## 7.15 Streams and String Streams

### 7.15.1 File I/O (`<fstream>`)

```cpp
// Listing 7.22: Basic file read and write
#include <fstream>
#include <string>
#include <iostream>
using namespace std;

int main() {
    // Write
    ofstream out("test.txt");
    if (out) {
        out << "Hello file\n";
        out << 42 << "\n";
    }
    out.close();

    // Read line by line
    ifstream in("test.txt");
    string line;
    while (getline(in, line))
        cout << line << "\n";
    in.close();
    return 0;
}
```

### 7.15.2 String Streams (`<sstream>`)

`std::stringstream` acts as an in-memory stream — use it for number-to-string and string-to-number conversions in C++98/03 (where `std::to_string` is not yet available):

```cpp
// Listing 7.23: stringstream for type conversion
#include <sstream>
#include <string>
#include <iostream>
using namespace std;

int main() {
    // int → string
    int x = 42;
    stringstream ss;
    ss << x;
    string s = ss.str(); // "42"

    // string → int
    string s2 = "100";
    stringstream ss2(s2);
    int y;
    ss2 >> y; // y == 100

    cout << s << " " << y << "\n";
    return 0;
}
```

---

## 7.16 Functors (Function Objects)

A **functor** is an instance of a class that overloads `operator()`. Algorithms that accept a "callable" can receive a functor instead of a function pointer. The advantage over function pointers: functors carry state and are easily inlined by the compiler.

```cpp
// Listing 7.24: Custom functor with state
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

struct AddX {
    int x;
    explicit AddX(int val) : x(val) {}
    int operator()(int y) const { return x + y; }
};

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    vector<int> v(arr, arr + 5);

    transform(v.begin(), v.end(), v.begin(), AddX(10));
    // v is now {11, 12, 13, 14, 15}

    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        cout << *it << " ";
    cout << "\n";
    return 0;
}
```

Standard functors in `<functional>`: `less<T>`, `greater<T>`, `equal_to<T>`, `plus<T>`, `negate<T>`, etc.

```cpp
// Listing 7.25: Standard functors as algorithm predicates
#include <algorithm>
#include <functional>
#include <vector>
using namespace std;

int main() {
    int arr[] = {5, 1, 3, 9, 2};
    vector<int> v(arr, arr + 5);

    sort(v.begin(), v.end(), greater<int>()); // {9, 5, 3, 2, 1}
    return 0;
}
```

---

## 7.17 Advanced String Operations

### 7.17.1 String Manipulation

```cpp
// Listing 7.26: Comprehensive string manipulation
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

int main() {
    string s = "Hello World";

    cout << "Length:   " << s.length()   << "\n";
    cout << "Capacity: " << s.capacity() << "\n";
    cout << "First:    " << s[0]         << "\n";

    size_t pos = s.find("World");
    if (pos != string::npos)
        cout << "Found 'World' at: " << pos << "\n";

    s.replace(6, 5, "C++");  // "Hello C++"
    s.insert(5, " there");   // "Hello there C++"
    s.erase(5, 6);           // "Hello C++"
    cout << s.substr(0, 5)   << "\n"; // "Hello"

    reverse(s.begin(), s.end()); // "++C olleH"
    cout << s << "\n";
    return 0;
}
```

### 7.17.2 String Conversion

```cpp
// Listing 7.27: C++98-compatible numeric conversion
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

int main() {
    // String → number (C-style; available in C++98)
    string s1 = "42";
    int num = atoi(s1.c_str());

    string s2 = "3.14";
    double dbl = atof(s2.c_str());

    // Number → string (stringstream idiom)
    stringstream ss;
    ss << 42 << " " << 3.14;
    string result = ss.str(); // "42 3.14"

    cout << num << " " << dbl << " " << result << "\n";
    return 0;
}
```

### 7.17.3 String Tokenisation

```cpp
// Listing 7.28: Two tokenisation strategies
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
using namespace std;

int main() {
    // Strategy 1: stringstream + getline (C++, safe, preserves whitespace sensitivity)
    string line = "apple,banana,orange,grape";
    stringstream ss(line);
    string token;
    while (getline(ss, token, ','))
        cout << token << "\n";

    // Strategy 2: strtok (C-style; modifies the original array)
    char str[] = "hello world how are you";
    char* ptr = strtok(str, " ");
    while (ptr != NULL) {
        cout << ptr << "\n";
        ptr = strtok(NULL, " ");
    }
    return 0;
}
```

**Prefer the `stringstream` approach** in C++ code: `strtok` is not thread-safe (in most implementations), cannot handle multiple simultaneous strings, and mutates its input.

---

## 7.18 Professional Insights: STL Internals and Pitfalls

### 7.18.1 Container Selection Guide

Choose your container based on access pattern, not familiarity:

| Need | Best container |
|------|----------------|
| Fast random access, cache-friendly iteration | `vector` |
| Fast push/pop at both ends | `deque` |
| Fast insert/erase anywhere with stable iterators | `list` |
| Key-based lookup, sorted traversal | `map` / `set` |
| Key-based lookup, order irrelevant (C++11) | `unordered_map` / `unordered_set` |
| Priority queue (max/min element fast) | `priority_queue` |
| Bounded LIFO/FIFO | `stack` / `queue` |

### 7.18.2 Iterator Invalidation Rules

Misusing invalidated iterators is undefined behaviour:

| Container | Operation | Invalidated iterators |
|-----------|-----------|----------------------|
| `vector` | `push_back` causes reallocation | All iterators |
| `vector` | `insert` / `erase` in middle | Iterators at or after point |
| `deque` | Any insert/erase | All iterators |
| `list` | Any insert | None; erase only invalidates the erased node |
| `map` / `set` | `insert` | None; `erase` only invalidates the erased node |

### 7.18.3 `vector<bool>` — The Exception

`std::vector<bool>` is **not** a regular `vector`. It packs bits one per bit, so `operator[]` returns a **proxy object**, not a `bool&`. This breaks code that takes the address of an element or uses generic template code. Prefer `vector<char>` or `bitset<N>` when you need bit storage without the proxy surprises.

### 7.18.4 Prefer `++it` Over `it++`

`++it` (prefix increment) advances in place. `it++` (postfix) creates a temporary copy, advances, and returns the old value. For iterators to tree nodes or list links, this temporary copy is non-trivial. Write `++it` everywhere in loops — the compiler can optimise it away for random-access iterators, and it is always correct.

### 7.18.5 `std::sort` and Strict Weak Ordering

Any comparator passed to `sort`, `map`, or `priority_queue` must satisfy **strict weak ordering**:
- Irreflexivity: `comp(x, x)` is false.
- Asymmetry: `comp(x, y)` implies `!comp(y, x)`.
- Transitivity: `comp(x, y) && comp(y, z)` implies `comp(x, z)`.

Violating these rules is undefined behaviour. A common mistake is using `<=` instead of `<`.

### 7.18.6 Algorithms and the `equal_range` Pattern

For sorted ranges, `lower_bound` + `upper_bound` together yield the half-open range of all elements equal to a target value. This is the standard O(log n) multi-match lookup:

```cpp
// Listing 7.29: equal_range on a sorted vector
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

int main() {
    int arr[] = {1, 2, 5, 5, 5, 9};
    vector<int> v(arr, arr + 6);

    pair<vector<int>::iterator, vector<int>::iterator> range
        = equal_range(v.begin(), v.end(), 5);

    cout << "Count of 5: " << (range.second - range.first) << "\n"; // 3
    return 0;
}
```
