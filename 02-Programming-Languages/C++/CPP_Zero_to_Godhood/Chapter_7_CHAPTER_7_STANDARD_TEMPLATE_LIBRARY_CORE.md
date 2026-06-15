# CHAPTER 7: STANDARD TEMPLATE LIBRARY CORE


# THE STANDARD TEMPLATE LIBRARY (STL) CORE

<!-- Merged content from Chapter_20_C9803_STANDARD_LIBRARY.md -->

# C++98/03 STANDARD LIBRARY

## Standard Template Library (STL)

---

## Introduction to STL

The Standard Template Library (STL) is a collection of template classes and functions that provide:
- **Containers** - Data structures to hold objects (e.g., vectors, lists, maps).
- **Iterators** - Objects to traverse containers (generalization of pointers).
- **Algorithms** - Functions to manipulate data (e.g., sorting, searching).
- **Function Objects (Functors)** - Objects that act like functions.

### Key Advantages
- **Generic Programming**: Write code that works with any data type.
- **Performance**: Heavily optimized implementations.
- **Reusability**: Standardized components prevent reinventing the wheel.
- **Type Safety**: Templates ensure type correctness at compile time.

---

## STL Components Overview

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

---
### Professional Notes: STL Core Depth

#### 1. Iterators: The Bridge between Algorithms and Containers
Iterators provide a uniform interface for traversing data.
*   **Input/Output**: Single pass, read or write once.
*   **Forward**: Multiple passes, read/write, move forward only (e.g., `std::forward_list`).
*   **Bidirectional**: Move forward and backward (e.g., `std::list`, `std::map`).
*   **Random Access**: Jump to any element in constant time (e.g., `std::vector`, `std::deque`).

**Godhood Tip**: Prefer prefix increment (`++it`) over postfix increment (`it++`) for iterators. Postfix creates a temporary copy of the iterator, which can be costly for complex types.

#### 2. Container Choice and Memory Layout
*   **`std::vector`**: The gold standard. Contiguous memory ensures high **Cache Locality**. Always `reserve()` if you know the final size to avoid reallocations.
*   **`std::deque`**: A "Double-Ended Queue." Implemented as a sequence of fixed-size memory blocks. Offers $O(1)$ at both ends but is slower for random access than vector.
*   **`std::list`**: Doubly linked list. $O(1)$ insertions anywhere if you have the iterator, but terrible cache locality and high memory overhead per element (2 pointers).

#### 3. Associative Containers and Custom Comparators
`std::map` and `std::set` are typically implemented as **Red-Black Trees**.
*   **Complexity**: $O(\log N)$ for all major operations.
*   **Comparators**: You can provide a custom function or functor to define the ordering.
```cpp
struct CaseInsensitiveCompare {
    bool operator()(const std::string& a, const std::string& b) const {
        return strcasecmp(a.c_str(), b.c_str()) < 0;
    }
};
std::set<std::string, CaseInsensitiveCompare> my_set;
```

---
### Professional Notes: Data Structures Internals

#### 1. Binary Search Trees (std::map, std::set)
Typically implemented as **Red-Black Trees**.
*   **Self-Balancing**: Ensures $O(\log N)$ height.
*   **Node Overhead**: Each element is stored in a separate node with pointers to parent and children, plus a color bit.

#### 2. Hash Tables (std::unordered_map)
Typically implemented as an array of buckets (linked lists).
*   **Load Factor**: When the number of elements exceeds `bucket_count * max_load_factor`, the table is rehashed (size doubled).
*   **Hash Collisions**: Handled via chaining (linked lists) or open addressing.

#### 3. Heap (std::priority_queue)
Implemented using `std::make_heap`, `std::push_heap`, and `std::pop_heap` on an underlying `std::vector`.
*   **Invariant**: The parent is always greater than (or equal to) its children.

---

## CONTAINERS - COMPLETE REFERENCE

### Container Characteristics (C++98)

| Container | Type | Insert | Delete | Search | Random Access | Memory |
|-----------|------|--------|--------|--------|---------------|--------|
| `vector` | Sequence | O(n) | O(n) | O(n) | O(1) | Contiguous |
| `list` | Sequence | O(1) | O(1) | O(n) | - | Scattered |
| `deque` | Sequence | O(n) | O(n) | O(n) | O(1) | Chunks |
| `map` | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| `set` | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| `multimap` | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| `multiset` | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| `priority_queue` | Adapter | O(log n) | O(log n) | - | - | Heap |
| `queue` | Adapter | O(1) | O(1) | - | - | - |
| `stack` | Adapter | O(1) | O(1) | - | - | - |

---

## 1.1 VECTOR - Dynamic Array

### What is Vector?
A dynamic array that grows automatically. Use this as your default container.

### Declaration & Initialization

```cpp
#include <vector>
using namespace std;

// Empty vector
vector<int> v1;

// Vector with initial size (10 elements initialized to 0)
vector<int> v2(10);

// Vector with initial size and value (5 elements, all 10)
vector<int> v3(5, 10);

// Copy constructor
vector<int> v4(v3);

// From array (using pointers)
int arr[] = {1, 2, 3, 4, 5};
vector<int> v5(arr, arr + 5);
```

### Accessing Elements

```cpp
vector<int> v;
v.push_back(10);
v.push_back(20);

// 1. Using operator[] (No bounds check)
cout << v[0] << "\n";  // 10

// 2. Using at() (With bounds check, throws std::out_of_range)
cout << v.at(1) << "\n"; // 20

// 3. Front and back
cout << v.front() << "\n"; // 10
cout << v.back() << "\n";  // 20
```

### Modifying Elements

```cpp
vector<int> v;

// Adding elements
v.push_back(10);
v.push_back(20);
v.push_back(30); // {10, 20, 30}

// Inserting elements (Iterators required)
v.insert(v.begin() + 1, 15); // {10, 15, 20, 30}

// Removing elements
v.pop_back();           // Removes 30
v.erase(v.begin() + 1); // Removes 15

// Clearing
v.clear();              // Empty
```

### Size & Capacity

```cpp
vector<int> v(10);

cout << v.size() << "\n";      // 10
cout << v.capacity() << "\n";  // >= 10
cout << v.empty() << "\n";     // 0 (false)

// Reserve space (Optimization to avoid reallocations)
v.reserve(100);

// Resize (Changes size, fills new elements with default or value)
v.resize(20);    // Size becomes 20
v.resize(5);     // Size becomes 5, extra elements destroyed
```

### Iterating Through Vector

```cpp
vector<int> v;
v.push_back(10); v.push_back(20); v.push_back(30);

// 1. Index loop
for (size_t i = 0; i < v.size(); ++i) {
    cout << v[i] << " ";
}

// 2. Iterator loop (Recommended)
for (vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    cout << *it << " ";
}

// 3. Reverse Iterator
for (vector<int>::reverse_iterator rit = v.rbegin(); rit != v.rend(); ++rit) {
    cout << *rit << " ";
}
```

---

## 1.2 DEQUE - Double Ended Queue

### What is Deque?
Fast insertion/deletion at both ends. Unlike vector, storage is not guaranteed to be contiguous.

```cpp
#include <deque>
using namespace std;

deque<int> dq;

dq.push_back(10);
dq.push_front(5);  // {5, 10}

dq.pop_back();     // {5}
dq.pop_front();    // {}

// Supports random access
dq.push_back(100);
cout << dq[0] << "\n";
```

---

## 1.3 LIST - Doubly Linked List

### What is List?
Efficient insertion/deletion anywhere (O(1) if iterator is known). No random access.

```cpp
#include <list>
using namespace std;

list<int> lst;
lst.push_back(10);
lst.push_front(5);

// Insert at position
list<int>::iterator it = lst.begin();
++it; // Move to second position
lst.insert(it, 7); // {5, 7, 10}

// Remove
lst.remove(7); // Remove all elements with value 7

// Unique (Removes consecutive duplicates)
lst.push_back(10);
lst.unique(); // {5, 10} (second 10 removed)

// Sort
lst.sort(); // Internal sort (std::sort doesn't work on lists)
```

---

## 1.4 MAP - Sorted Key-Value Pairs

### What is Map?
Associative container storing key-value pairs sorted by key. Implemented as a Balanced BST (usually Red-Black Tree).

```cpp
#include <map>
#include <string>
using namespace std;

map<string, int> ages;

// Insertion
ages["Alice"] = 30;
ages["Bob"] = 25;
ages.insert(make_pair("Charlie", 28));

// Access / Search
cout << ages["Alice"] << "\n"; // 30

map<string, int>::iterator it = ages.find("Bob");
if (it != ages.end()) {
    cout << "Bob is " << it->second << " years old.\n";
}

// Iteration (Sorted by key)
for (it = ages.begin(); it != ages.end(); ++it) {
    cout << it->first << ": " << it->second << "\n";
}
```

---

## 1.5 SET - Sorted Unique Keys

### What is Set?
Stores unique elements sorted by value.

```cpp
#include <set>
using namespace std;

set<int> s;
s.insert(10);
s.insert(5);
s.insert(10); // Duplicate ignored

// {5, 10}

if (s.find(5) != s.end()) {
    cout << "5 is present.\n";
}

// Range erase
s.erase(s.begin()); // Removes 5
```

---

## 1.6 MULTIMAP & MULTISET

Allow duplicate keys (multimap) or duplicate values (multiset).

```cpp
#include <map>
using namespace std;

multimap<string, int> scores;
scores.insert(make_pair("Alice", 90));
scores.insert(make_pair("Alice", 85)); // Allowed

// Finding all values for a key
pair<multimap<string, int>::iterator, multimap<string, int>::iterator> range;
range = scores.equal_range("Alice");

for (multimap<string, int>::iterator it = range.first; it != range.second; ++it) {
    cout << it->second << " ";
}
// Output: 90 85
```

---

## 1.7 STACK (Adapter)

LIFO (Last In, First Out). Wraps `deque` (default), `vector`, or `list`.

```cpp
#include <stack>
using namespace std;

stack<int> st;
st.push(10);
st.push(20);

cout << st.top() << "\n"; // 20
st.pop();                 // Removes 20
```

---

## 1.8 QUEUE (Adapter)

FIFO (First In, First Out). Wraps `deque` (default) or `list`.

```cpp
#include <queue>
using namespace std;

queue<int> q;
q.push(10);
q.push(20);

cout << q.front() << "\n"; // 10
q.pop();                   // Removes 10
```

---

## 1.9 PRIORITY_QUEUE (Adapter)

Sorted queue (Heap). Max element is always at `top()`.

```cpp
#include <queue>
#include <vector>
#include <functional> // for greater<int>
using namespace std;

// Max Heap (Default)
priority_queue<int> pq;
pq.push(10);
pq.push(30);
pq.push(20);

cout << pq.top() << "\n"; // 30

// Min Heap
priority_queue<int, vector<int>, greater<int> > min_pq;
min_pq.push(10);
min_pq.push(30);

cout << min_pq.top() << "\n"; // 10
```

---

## ITERATORS

Iterators are the glue between Containers and Algorithms.

### Categories
1.  **Input**: Read-only, single pass (e.g., `istream_iterator`).
2.  **Output**: Write-only, single pass (e.g., `ostream_iterator`).
3.  **Forward**: Read/Write, multi-pass (e.g., `slist` - non-std).
4.  **Bidirectional**: Forward + Backward (e.g., `list`, `map`, `set`).
5.  **Random Access**: O(1) jump (e.g., `vector`, `deque`, arrays).

### Operations
- `*it`: Dereference.
- `++it`: Advance.
- `--it`: Retreat (Bidirectional+).
- `it + n`: Jump (Random Access only).
- `it1 - it2`: Distance (Random Access only).

```cpp
vector<int> v(5, 1);
vector<int>::iterator it = v.begin();
advance(it, 2); // Move 2 steps
cout << distance(v.begin(), it); // 2
```

---

## ALGORITHMS

Found in `<algorithm>` and `<numeric>`. They work on iterator ranges `[first, last)`.

### 1. Non-Modifying
- `find(begin, end, val)`
- `count(begin, end, val)`
- `equal(b1, e1, b2)`
- `search(b1, e1, b2, e2)`

---
### Professional Notes: Algorithm Mastery

#### 1. Range Integrity and End Iterators
All STL algorithms operate on half-open ranges `[first, last)`.
*   **The "Last" Iterator**: Points *beyond* the last element. Dereferencing it is **Undefined Behavior**.
*   **Empty Range**: If `first == last`, the range is empty. Algorithms correctly handle this (e.g., `find` returns `last`).

#### 2. Sorting and Stability
*   **`std::sort`**: Usually implemented as **Introsort** (Hybrid of Quicksort, Heapsort, and Insertion Sort). Average complexity $O(N \log N)$.
*   **`std::stable_sort`**: Maintains the relative order of equal elements. Requires extra memory for its work buffer.
*   **`std::partial_sort`**: Find the top $K$ elements without sorting the whole range.

#### 3. The Lambda Evolution (C++11)
Algorithms are most powerful when combined with lambdas:
```cpp
// Find first even number
auto it = std::find_if(vec.begin(), vec.end(), [](int x){ return x % 2 == 0; });
```

#### 4. Binary Search and Sorted Ranges
Functions like `binary_search`, `lower_bound`, and `upper_bound` require the range to be **sorted** or at least partitioned by the search value. Using them on unsorted ranges is UB.

---

### 2. Modifying
- `copy(b1, e1, b2)`
- `transform(b1, e1, out, op)`
- `replace(b1, e1, old, new)`
- `fill(b, e, val)`
- `swap(a, b)`
- `reverse(b, e)`
- `rotate(b, mid, e)`
- `random_shuffle(b, e)`

### 3. Sorting
- `sort(b, e)`: O(N log N).
- `stable_sort(b, e)`: Preserves order of equal elements.
- `partial_sort(b, mid, e)`: Top K elements.

### 4. Binary Search (On Sorted Ranges)
- `binary_search(b, e, val)`: Returns bool.
- `lower_bound(b, e, val)`: First element >= val.
- `upper_bound(b, e, val)`: First element > val.

### 5. Set Operations (On Sorted Ranges)
- `set_union`, `set_intersection`, `set_difference`.

### 6. Numeric (<numeric>)
- `accumulate(b, e, init)`: Sum.
- `inner_product`: Dot product.
- `adjacent_difference`.

### Example: Sort and Find

```cpp
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

bool descending(int a, int b) {
    return a > b;
}

int main() {
    int arr[] = {5, 2, 9, 1, 5, 6};
    vector<int> v(arr, arr + 6);

    // Sort ascending
    sort(v.begin(), v.end());

    // Binary Search
    if (binary_search(v.begin(), v.end(), 9)) {
        cout << "Found 9\n";
    }

    // Sort descending with predicate
    sort(v.begin(), v.end(), descending);

    return 0;
}
```

---

## STRINGS (std::string)

A specialization of `basic_string<char>`. Replaces C-style `char*` strings.

```cpp
#include <string>
#include <iostream>
using namespace std;

string s = "Hello";
s += " World"; // Concatenation

// Substring
string sub = s.substr(0, 5); // "Hello"

// Find
size_t pos = s.find("World");
if (pos != string::npos) {
    cout << "Found at " << pos << "\n";
}

// C-string compatibility
const char* cstr = s.c_str();
```

---

## STREAMS (IOSTREAM)

### File I/O (<fstream>)

```cpp
#include <fstream>
#include <iostream>
using namespace std;

int main() {
    // Write
    ofstream out("test.txt");
    if (out) {
        out << "Line 1" << endl;
        out << 123 << endl;
    }
    out.close();

    // Read
    ifstream in("test.txt");
    string line;
    int num;

    if (in >> line >> num) { // Reads "Line" then fails on "1" vs int? No.
                             // "Line" goes to line. "1" goes to num?
                             // Need careful parsing.
    }
    // Better: getline
    getline(in, line);

    return 0;
}
```

### String Streams (<sstream>)

Useful for parsing strings or formatting.

```cpp
#include <sstream>
using namespace std;

// Int to String
int x = 42;
stringstream ss;
ss << x;
string s = ss.str();

// String to Int
string s2 = "100";
stringstream ss2(s2);
int y;
ss2 >> y;
```

---

## FUNCTORS (Function Objects)

Classes that overload `operator()`. Used by algorithms.

```cpp
struct AddX {
    int x;
    AddX(int val) : x(val) {}

    int operator()(int y) const {
        return x + y;
    }
};

// Usage with transform
vector<int> v(5, 1); // {1, 1, 1, 1, 1}
transform(v.begin(), v.end(), v.begin(), AddX(10));
// v is now {11, 11, 11, 11, 11}
```

This covers the foundational C++98 Standard Library components necessary for mastery.

<!-- Merged content from Chapter_6_ADVANCED_STRINGS.md -->

# ADVANCED STRINGS

## 5.1 String Manipulation

```cpp
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

int main() {
    string s = "Hello World";

    // Length and capacity
    cout << "Length: " << s.length() << endl;
    cout << "Capacity: " << s.capacity() << endl;

    // Access characters
    cout << "First char: " << s[0] << endl;
    cout << "Last char: " << s[s.length() - 1] << endl;

    // Finding substrings
    size_t pos = s.find("World");
    if (pos != string::npos) {
        cout << "Found at position: " << pos << endl;
    }

    // Replace
    s.replace(6, 5, "C++");
    cout << s << endl;  // Hello C++

    // Insert
    s.insert(5, " there");
    cout << s << endl;  // Hello there C++

    // Erase
    s.erase(5, 6);
    cout << s << endl;  // Hello C++

    // Substring
    cout << s.substr(0, 5) << endl;  // Hello

    // Reverse
    reverse(s.begin(), s.end());
    cout << s << endl;  // ++C olleH

    return 0;
}
```

## 5.2 String Conversion

```cpp
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

int main() {
    // String to number (C style)
    string s1 = "42";
    int num = atoi(s1.c_str());
    cout << num << endl;

    string s2 = "3.14";
    double dbl = atof(s2.c_str());
    cout << dbl << endl;

    // Number to string (using stringstream)
    stringstream ss;
    ss << 42 << " " << 3.14 << " " << true;
    string result = ss.str();
    cout << result << endl;  // 42 3.14 1

    // Reverse conversion
    stringstream ss2("100 200 300");
    int a, b, c;
    ss2 >> a >> b >> c;
    cout << a << " " << b << " " << c << endl;  // 100 200 300

    return 0;
}
```

## 5.3 String Tokenization

```cpp
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

int main() {
    string line = "apple,banana,orange,grape";

    // Using stringstream and getline
    stringstream ss(line);
    string token;

    while (getline(ss, token, ',')) {
        cout << token << endl;
    }
    // Output: apple, banana, orange, grape

    // Using strtok (C style)
    char str[] = "hello world how are you";
    char* ptr = strtok(str, " ");

    while (ptr != NULL) {
        cout << ptr << endl;
        ptr = strtok(NULL, " ");
    }
    // Output: hello, world, how, are, you

    return 0;
}
```

---

<!-- Merged content from Chapter_15_FILE_IO_ADVANCED.md -->

# FILE I/O ADVANCED

## 14.1 Binary File I/O

```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Write binary data
    ofstream outfile("data.bin", ios::binary);

    int numbers[] = {10, 20, 30, 40, 50};
    outfile.write((char*)numbers, sizeof(numbers));
    outfile.close();

    // Read binary data
    ifstream infile("data.bin", ios::binary);

    int buffer[5];
    infile.read((char*)buffer, sizeof(buffer));

    for (int i = 0; i < 5; i++) {
        cout << buffer[i] << " ";
    }
    cout << endl;

    infile.close();

    return 0;
}
```

## 14.2 Stream Positioning

```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Write to file
    ofstream outfile("test.txt");
    outfile << "0123456789";
    outfile.close();

    // Read with positioning
    ifstream infile("test.txt");

    // Tell position
    cout << "Current position: " << infile.tellg() << endl;

    // Seek to position
    infile.seekg(5);
    char c;
    infile.get(c);
    cout << "Character at position 5: " << c << endl;  // '5'

    // Seek from end
    infile.seekg(-3, ios::end);
    infile.get(c);
    cout << "Third from end: " << c << endl;  // '7'

    infile.close();

    return 0;
}
```

---

# Professional Notes: Chapter 47: std::string

Section 47.1: Tokenize
Section 47.2: Conversion to (const) char*
Section 47.3: Using the std::string_view class
Section 47.4: Conversion to std::wstring
Section 47.5: Lexicographical comparison
Section 47.6: Trimming characters at start/end
Section 47.7: String replacement
Section 47.8: Converting to std::string
Section 47.9: Splitting
Section 47.10: Accessing a character
Section 47.11: Checking if a string is a prex of another
Section 47.12: Looping through each character
Section 47.13: Conversion to integers/oating point types
Section 47.14: Concatenation
Section 47.15: Converting between character encodings
Section 47.16: Finding character(s) in a string

# Professional Notes: Chapter 49: std::vector

Section 49.1: Accessing Elements
Section 49.2: Initializing a std::vector
Section 49.3: Deleting Elements
Section 49.4: Iterating Over std::vector
Section 49.5: vector<bool>: The Exception To So Many, So Many Rules
Section 49.6: Inserting Elements
Section 49.7: Using std::vector as a C array
Section 49.8: Finding an Element in std::vector
Section 49.9: Concatenating Vectors
Section 49.10: Matrices Using Vectors
Section 49.11: Using a Sorted Vector for Fast Element Lookup
Section 49.12: Reducing the Capacity of a Vector
Section 49.13: Vector size and capacity
Section 49.14: Iterator/Pointer Invalidation
Section 49.15: Find max and min Element and Respective Index in a Vector
Section 49.16: Converting an array to std::vector
Section 49.17: Functions Returning Large Vectors

# Professional Notes: Chapter 50: std::map

Section 50.1: Accessing elements
Section 50.2: Inserting elements
Section 50.3: Searching in std::map or in std::multimap
Section 50.4: Initializing a std::map or std::multimap
Section 50.5: Checking number of elements
Section 50.6: Types of Maps
Section 50.7: Deleting elements
Section 50.8: Iterating over std::map or std::multimap
Section 50.9: Creating std::map with user-dened types as key

# Professional Notes: Chapter 62: Standard Library Algorithms

Section 62.1: std::next_permutation
Section 62.2: std::for_each
Section 62.3: std::accumulate
Section 62.4: std::nd
Section 62.5: std::min_element
Section 62.6: std::nd_if
Section 62.7: Using std::nth_element To Find The Median (Or Other Quantiles)
Section 62.8: std::count
Section 62.9: std::count_if

# Professional Notes: Chapter 67: Sorting

Section 67.1: Sorting and sequence containers
Section 67.2: sorting with std::map (ascending and descending)
Section 67.3: Sorting sequence containers by overloaded less operator
Section 67.4: Sorting sequence containers using compare function
Section 67.5: Sorting sequence containers using lambda expressions (C++11)
Section 67.6: Sorting built-in arrays
Section 67.7: Sorting sequence containers with specifed ordering

# Professional Notes: Chapter 43: C++ Containers

C++ containers store a collection of elements. Containers include vectors, lists, maps, etc. Using Templates, C++
containers contain collections of primitives (e.g. ints) or custom classes (e.g. MyClass).
Section 43.1: C++ Containers Flowchart
Choosing which C++ Container to use can be tricky, so here's a simple owchart to help decide which Container is
right for the job.
This owchart was based on Mikael Persson's post. This little graphic in the owchart is from Megan Hopkins

# Professional Notes: Chapter 47: std::string

Strings are objects that represent sequences of characters. The standard string class provides a simple, safe and
versatile alternative to using explicit arrays of chars when dealing with text and other sequences of characters. The
C++ string class is part of the std namespace and was standardized in 1998.
Section 47.1: Tokenize
Listed from least expensive to most expensive at run-time:
1.
std::strtok is the cheapest standard provided tokenization method, it also allows the delimiter to be
modied between tokens, but it incurs 3 diculties with modern C++:
std::strtok cannot be used on multiple strings at the same time (though some implementations do
extend to support this, such as: strtok_s)
For the same reason std::strtok cannot be used on multiple threads simultaneously (this may
however be implementation dened, for example: Visual Studio's implementation is thread safe)
Calling std::strtok modies the std::string it is operating on, so it cannot be used on const
strings, const char*s, or literal strings, to tokenize any of these with std::strtok or to operate on a
std::string who's contents need to be preserved, the input would have to be copied, then the copy
could be operated on
Generally any of these options cost will be hidden in the allocation cost of the tokens, but if the cheapest
algorithm is required and std::strtok's diculties are not overcomable consider a hand-spun solution.
// String to tokenize
std::string str{ "The quick brown fox" };
// Vector to store tokens
vector<std::string> tokens;
for (auto i = strtok(&str[0], " "); i != NULL; i = strtok(NULL, " "))
    tokens.push_back(i);
Live Example
2.
The std::istream_iterator uses the stream's extraction operator iteratively. If the input std::string is
white-space delimited this is able to expand on the std::strtok option by eliminating its diculties, allowing
inline tokenization thereby supporting the generation of a const vector<string>, and by adding support for
multiple delimiting white-space character:
// String to tokenize
const std::string str("The  quick \tbrown \nfox");
std::istringstream is(str);
// Vector to store tokens
const std::vector<std::string> tokens = std::vector<std::string>(
                                        std::istream_iterator<std::string>(is),
                                        std::istream_iterator<std::string>());
Live Example
3.
The std::regex_token_iterator uses a std::regex to iteratively tokenize. It provides for a more exible
delimiter denition. For example, non-delimited commas and white-space:
Version  C++11
// String to tokenize
const std::string str{ "The ,qu\\,ick ,\tbrown, fox" };
const std::regex re{ "\\s*((?:[^\\\\,]|\\\\.)*?)\\s*(?:,|$)" };
// Vector to store tokens
const std::vector<std::string> tokens{
    std::sregex_token_iterator(str.begin(), str.end(), re, 1),
    std::sregex_token_iterator()
};
Live Example
See the regex_token_iterator Example for more details.
Section 47.2: Conversion to (const) char*
In order to get const char* access to the data of a std::string you can use the string's c_str() member function.
Keep in mind that the pointer is only valid as long as the std::string object is within scope and remains
unchanged, that means that only const methods may be called on the object.
Version  C++17
The data() member function can be used to obtain a modiable char*, which can be used to manipulate the
std::string object's data.
Version  C++11
A modiable char* can also be obtained by taking the address of the rst character: &s[0]. Within C++11, this is
guaranteed to yield a well-formed, null-terminated string. Note that &s[0] is well-formed even if s is empty,
whereas &s.front() is undened if s is empty.
Version  C++11
std::string str("This is a string.");
const char* cstr = str.c_str(); // cstr points to: "This is a string.\0"
const char* data = str.data();  // data points to: "This is a string.\0"
std::string str("This is a string.");
// Copy the contents of str to untie lifetime from the std::string object
std::unique_ptr<char []> cstr = std::make_unique<char[]>(str.size() + 1);
// Alternative to the line above (no exception safety):
// char* cstr_unsafe = new char[str.size() + 1];
std::copy(str.data(), str.data() + str.size(), cstr);
cstr[str.size()] = '\0'; // A null-terminator needs to be added
// delete[] cstr_unsafe;
std::cout << cstr.get();
Section 47.3: Using the std::string_view class
Version  C++17
C++17 introduces std::string_view, which is simply a non-owning range of const chars, implementable as either
a pair of pointers or a pointer and a length. It is a superior parameter type for functions that requires non-
modiable string data. Before C++17, there were three options for this:
void foo(std::string const& s);      // pre-C++17, single argument, could incur
                                     // allocation if caller's data was not in a string
                                     // (e.g. string literal or vector<char> )
void foo(const char* s, size_t len); // pre-C++17, two arguments, have to pass them
                                     // both everywhere
void foo(const char* s);             // pre-C++17, single argument, but need to call
                                     // strlen()
template <class StringT>
void foo(StringT const& s);          // pre-C++17, caller can pass arbitrary char data
                                     // provider, but now foo() has to live in a header
All of these can be replaced with:
void foo(std::string_view s);        // post-C++17, single argument, tighter coupling
                                     // zero copies regardless of how caller is storing
                                     // the data
Note that std::string_view cannot modify its underlying data.
string_view is useful when you want to avoid unnecessary copies.
It oers a useful subset of the functionality that std::string does, although some of the functions behave
dierently:
std::string str = "lllloooonnnngggg sssstttrrriiinnnggg"; //A really long string
//Bad way - 'string::substr' returns a new string (expensive if the string is long)
std::cout << str.substr(15, 10) << '\n';
//Good way - No copies are created!
std::string_view view = str;
// string_view::substr returns a new string_view
std::cout << view.substr(15, 10) << '\n';
Section 47.4: Conversion to std::wstring
In C++, sequences of characters are represented by specializing the std::basic_string class with a native
character type. The two major collections dened by the standard library are std::string and std::wstring:
std::string is built with elements of type char
std::wstring is built with elements of type wchar_t
To convert between the two types, use wstring_convert:
#include <string>
#include <codecvt>
#include <locale>
std::string input_str = "this is a -string-, which is a sequence based on the -char- type.";
std::wstring input_wstr = L"this is a -wide- string, which is based on the -wchar_t- type.";
// conversion
std::wstring str_turned_to_wstr =
std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(input_str);
std::string wstr_turned_to_str =
std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(input_wstr);
In order to improve usability and/or readability, you can dene functions to perform the conversion:
#include <string>
#include <codecvt>
#include <locale>
using convert_t = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_t, wchar_t> strconverter;
std::string to_string(std::wstring wstr)
{
    return strconverter.to_bytes(wstr);
}
std::wstring to_wstring(std::string str)
{
    return strconverter.from_bytes(str);
}
Sample usage:
std::wstring a_wide_string = to_wstring("Hello World!");
That's certainly more readable than std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes("Hello
World!").
Please note that char and wchar_t do not imply encoding, and gives no indication of size in bytes. For instance,
wchar_t is commonly implemented as a 2-bytes data type and typically contains UTF-16 encoded data under
Windows (or UCS-2 in versions prior to Windows 2000) and as a 4-bytes data type encoded using UTF-32 under
Linux. This is in contrast with the newer types char16_t and char32_t, which were introduced in C++11 and are
guaranteed to be large enough to hold any UTF16 or UTF32 "character" (or more precisely, code point) respectively.
Section 47.5: Lexicographical comparison
Two std::strings can be compared lexicographically using the operators ==, !=, <, <=, >, and >=:
std::string str1 = "Foo";
std::string str2 = "Bar";
assert(!(str1 < str2));
assert(str > str2);
assert(!(str1 <= str2));
assert(str1 >= str2);
assert(!(str1 == str2));
assert(str1 != str2);
All these functions use the underlying std::string::compare() method to perform the comparison, and return for
convenience boolean values. The operation of these functions may be interpreted as follows, regardless of the
actual implementation:
operator==:
If str1.length() == str2.length() and each character pair matches, then returns true, otherwise returns
false.
operator!=:
If str1.length() != str2.length() or one character pair doesn't match, returns true, otherwise it returns
false.
operator< or operator>:
Finds the rst dierent character pair, compares them then returns the boolean result.
operator<= or operator>=:
Finds the rst dierent character pair, compares them then returns the boolean result.
Note: The term character pair means the corresponding characters in both strings of the same positions. For
better understanding, if two example strings are str1 and str2, and their lengths are n and m respectively, then
character pairs of both strings means each str1[i] and str2[i] pairs where i = 0, 1, 2, ..., max(n,m). If for any i
where the corresponding character does not exist, that is, when i is greater than or equal to n or m, it would be
considered as the lowest value.
Here is an example of using <:
std::string str1 = "Barr";
std::string str2 = "Bar";
assert(str2 < str1);
The steps are as follows:
1.
2.
3.
4.
Compare the rst characters, 'B' == 'B' - move on.
Compare the second characters, 'a' == 'a' - move on.
Compare the third characters, 'r' == 'r' - move on.
The str2 range is now exhausted, while the str1 range still has characters. Thus, str2 < str1.
Section 47.6: Trimming characters at start/end
This example requires the headers <algorithm>, <locale>, and <utility>.
Version  C++11
To trim a sequence or string means to remove all leading and trailing elements (or characters) matching a certain
predicate. We rst trim the trailing elements, because it doesn't involve moving any elements, and then trim the
leading elements. Note that the generalizations below work for all types of std::basic_string (e.g. std::string
and std::wstring), and accidentally also for sequence containers (e.g. std::vector and std::list).
template <typename Sequence, // any basic_string, vector, list etc.
          typename Pred>     // a predicate on the element (character) type
Sequence& trim(Sequence& seq, Pred pred) {
    return trim_start(trim_end(seq, pred), pred);
}
Trimming the trailing elements involves nding the last element not matching the predicate, and erasing from there
on:
template <typename Sequence, typename Pred>
Sequence& trim_end(Sequence& seq, Pred pred) {
    auto last = std::find_if_not(seq.rbegin(),
                                 seq.rend(),
                                 pred);
    seq.erase(last.base(), seq.end());
    return seq;
}
Trimming the leading elements involves nding the rst element not matching the predicate and erasing up to
there:
template <typename Sequence, typename Pred>
Sequence& trim_start(Sequence& seq, Pred pred) {
    auto first = std::find_if_not(seq.begin(),
                                  seq.end(),
                                  pred);
    seq.erase(seq.begin(), first);
    return seq;
}
To specialize the above for trimming whitespace in a std::string we can use the std::isspace() function as a
predicate:
std::string& trim(std::string& str, const std::locale& loc = std::locale()) {
    return trim(str, [&loc](const char c){ return std::isspace(c, loc); });
}
std::string& trim_start(std::string& str, const std::locale& loc = std::locale()) {
    return trim_start(str, [&loc](const char c){ return std::isspace(c, loc); });
}
std::string& trim_end(std::string& str, const std::locale& loc = std::locale()) {
    return trim_end(str, [&loc](const char c){ return std::isspace(c, loc); });
}
Similarly, we can use the std::iswspace() function for std::wstring etc.
If you wish to create a new sequence that is a trimmed copy, then you can use a separate function:
template <typename Sequence, typename Pred>
Sequence trim_copy(Sequence seq, Pred pred) { // NOTE: passing seq by value
    trim(seq, pred);
    return seq;
}
Section 47.7: String replacement
Replace by position
To replace a portion of a std::string you can use the method replace from std::string.
replace has a lot of useful overloads:
//Define string
std::string str = "Hello foo, bar and world!";
std::string alternate = "Hello foobar";
//1)
str.replace(6, 3, "bar"); //"Hello bar, bar and world!"
//2)
str.replace(str.begin() + 6, str.end(), "nobody!"); //"Hello nobody!"
//3)
str.replace(19, 5, alternate, 6, 6); //"Hello foo, bar and foobar!"
Version  C++14
//4)
str.replace(19, 5, alternate, 6); //"Hello foo, bar and foobar!"
//5)
str.replace(str.begin(), str.begin() + 5, str.begin() + 6, str.begin() + 9);
//"foo foo, bar and world!"
//6)
str.replace(0, 5, 3, 'z'); //"zzz foo, bar and world!"
//7)
str.replace(str.begin() + 6, str.begin() + 9, 3, 'x'); //"Hello xxx, bar and world!"
Version  C++11
//8)
str.replace(str.begin(), str.begin() + 5, { 'x', 'y', 'z' }); //"xyz foo, bar and world!"
Replace occurrences of a string with another string
Replace only the rst occurrence of replace with with in str:
std::string replaceString(std::string str,
                          const std::string& replace,
                          const std::string& with){
    std::size_t pos = str.find(replace);
    if (pos != std::string::npos)
        str.replace(pos, replace.length(), with);
    return str;
}
Replace all occurrence of replace with with in str:
std::string replaceStringAll(std::string str,
                             const std::string& replace,
                             const std::string& with) {
    if(!replace.empty()) {
        std::size_t pos = 0;
        while ((pos = str.find(replace, pos)) != std::string::npos) {
            str.replace(pos, replace.length(), with);
            pos += with.length();
        }
    }
    return str;
}
Section 47.8: Converting to std::string
std::ostringstream can be used to convert any streamable type to a string representation, by inserting the object
into a std::ostringstream object (with the stream insertion operator <<) and then converting the whole
std::ostringstream to a std::string.
For int for instance:
#include <sstream>
int main()
{
    int val = 4;
    std::ostringstream str;
    str << val;
    std::string converted = str.str();
    return 0;
}
Writing your own conversion function, the simple:
template<class T>
std::string toString(const T& x)
{
  std::ostringstream ss;
  ss << x;
  return ss.str();
}
works but isn't suitable for performance critical code.
User-dened classes may implement the stream insertion operator if desired:
std::ostream operator<<( std::ostream& out, const A& a )
{
    // write a string representation of a to out
    return out;
}
Version  C++11
Aside from streams, since C++11 you can also use the std::to_string (and std::to_wstring) function which is
overloaded for all fundamental types and returns the string representation of its parameter.
std::string s = to_string(0x12f3);  // after this the string s contains "4851"
Section 47.9: Splitting
Use std::string::substr to split a string. There are two variants of this member function.
The rst takes a starting position from which the returned substring should begin. The starting position must be
valid in the range (0, str.length()]:
std::string str = "Hello foo, bar and world!";
std::string newstr = str.substr(11); // "bar and world!"
The second takes a starting position and a total length of the new substring. Regardless of the length, the substring
will never go past the end of the source string:
std::string str = "Hello foo, bar and world!";
std::string newstr = str.substr(15, 3); // "and"
Note that you can also call substr with no arguments, in this case an exact copy of the string is returned
std::string str = "Hello foo, bar and world!";
std::string newstr = str.substr(); // "Hello foo, bar and world!"
Section 47.10: Accessing a character
There are several ways to extract characters from a std::string and each is subtly dierent.
std::string str("Hello world!");
operator[](n)
Returns a reference to the character at index n.
std::string::operator[] is not bounds-checked and does not throw an exception. The caller is responsible for
asserting that the index is within the range of the string:
char c = str[6]; // 'w'
at(n)
Returns a reference to the character at index n.
std::string::at is bounds checked, and will throw std::out_of_range if the index is not within the range of the
string:
char c = str.at(7); // 'o'
Version  C++11
Note: Both of these examples will result in undened behavior if the string is empty.
front()
Returns a reference to the rst character:
char c = str.front(); // 'H'
back()
Returns a reference to the last character:
char c = str.back(); // '!'
Section 47.11: Checking if a string is a prex of another
Version  C++14
In C++14, this is easily done by std::mismatch which returns the rst mismatching pair from two ranges:
std::string prefix = "foo";
std::string string = "foobar";
bool isPrefix = std::mismatch(prefix.begin(), prefix.end(),
    string.begin(), string.end()).first == prefix.end();
Note that a range-and-a-half version of mismatch() existed prior to C++14, but this is unsafe in the case that the
second string is the shorter of the two.
Version < C++14
We can still use the range-and-a-half version of std::mismatch(), but we need to rst check that the rst string is at
most as big as the second:
bool isPrefix = prefix.size() <= string.size() &&
    std::mismatch(prefix.begin(), prefix.end(),
        string.begin(), string.end()).first == prefix.end();
Version  C++17
With std::string_view, we can write the direct comparison we want without having to worry about allocation
overhead or making copies:
bool isPrefix(std::string_view prefix, std::string_view full)
{
    return prefix == full.substr(0, prefix.size());
}
Section 47.12: Looping through each character
Version  C++11
std::string supports iterators, and so you can use a ranged based loop to iterate through each character:
std::string str = "Hello World!";
for (auto c : str)
    std::cout << c;
You can use a "traditional" for loop to loop through every character:
std::string str = "Hello World!";
for (std::size_t i = 0; i < str.length(); ++i)
    std::cout << str[i];
Section 47.13: Conversion to integers/oating point types
A std::string containing a number can be converted into an integer type, or a oating point type, using
conversion functions.
Note that all of these functions stop parsing the input string as soon as they encounter a non-numeric character, so
"123abc" will be converted into 123.
The std::ato* family of functions converts C-style strings (character arrays) to integer or oating-point types:
std::string ten = "10";
double num1 = std::atof(ten.c_str());
int num2 = std::atoi(ten.c_str());
long num3 = std::atol(ten.c_str());
Version  C++11
long long num4 = std::atoll(ten.c_str());
However, use of these functions is discouraged because they return 0 if they fail to parse the string. This is bad
because 0 could also be a valid result, if for example the input string was "0", so it is impossible to determine if the
conversion actually failed.
The newer std::sto* family of functions convert std::strings to integer or oating-point types, and throw
exceptions if they could not parse their input. You should use these functions if possible:
Version  C++11
std::string ten = "10";
int num1 = std::stoi(ten);
long num2 = std::stol(ten);
long long num3 = std::stoll(ten);
float num4 = std::stof(ten);
double num5 = std::stod(ten);
long double num6 = std::stold(ten);
Furthermore, these functions also handle octal and hex strings unlike the std::ato* family. The second parameter
is a pointer to the rst unconverted character in the input string (not illustrated here), and the third parameter is
the base to use. 0 is automatic detection of octal (starting with 0) and hex (starting with 0x or 0X), and any other
value is the base to use
std::string ten = "10";
std::string ten_octal = "12";
std::string ten_hex = "0xA";
int num1 = std::stoi(ten, 0, 2); // Returns 2
int num2 = std::stoi(ten_octal, 0, 8); // Returns 10
long num3 = std::stol(ten_hex, 0, 16);  // Returns 10
long num4 = std::stol(ten_hex);  // Returns 0
long num5 = std::stol(ten_hex, 0, 0); // Returns 10 as it detects the leading 0x
Section 47.14: Concatenation
You can concatenate std::strings using the overloaded + and += operators. Using the + operator:
std::string hello = "Hello";
std::string world = "world";
std::string helloworld = hello + world; // "Helloworld"
Using the += operator:
std::string hello = "Hello";
std::string world = "world";
hello += world; // "Helloworld"
You can also append C strings, including string literals:
std::string hello = "Hello";
std::string world = "world";
const char *comma = ", ";
std::string newhelloworld = hello + comma + world + "!"; // "Hello, world!"
You can also use push_back() to push back individual chars:
std::string s = "a, b, ";
s.push_back('c'); // "a, b, c"
There is also append(), which is pretty much like +=:
std::string app = "test and ";
app.append("test"); // "test and test"
Section 47.15: Converting between character encodings
Converting between encodings is easy with C++11 and most compilers are able to deal with it in a cross-platform
manner through <codecvt> and <locale> headers.
#include <iostream>
#include <codecvt>
#include <locale>
#include <string>
using namespace std;
int main() {
    // converts between wstring and utf8 string
    wstring_convert<codecvt_utf8_utf16<wchar_t>> wchar_to_utf8;
    // converts between u16string and utf8 string
    wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> utf16_to_utf8;
    wstring wstr = L"foobar";
    string utf8str = wchar_to_utf8.to_bytes(wstr);
    wstring wstr2 = wchar_to_utf8.from_bytes(utf8str);
    wcout << wstr << endl;
    cout << utf8str << endl;
    wcout << wstr2 << endl;
    u16string u16str = u"foobar";
    string utf8str2 = utf16_to_utf8.to_bytes(u16str);
    u16string u16str2 = utf16_to_utf8.from_bytes(utf8str2);
    return 0;
}
Mind that Visual Studio 2015 provides supports for these conversion but a bug in their library implementation
requires to use a dierent template for wstring_convert when dealing with char16_t:
using utf16_char = unsigned short;
wstring_convert<codecvt_utf8_utf16<utf16_char>, utf16_char> conv_utf8_utf16;
void strings::utf16_to_utf8(const std::u16string& utf16, std::string& utf8)
{
  std::basic_string<utf16_char> tmp;
  tmp.resize(utf16.length());
  std::copy(utf16.begin(), utf16.end(), tmp.begin());
  utf8 = conv_utf8_utf16.to_bytes(tmp);
}
void strings::utf8_to_utf16(const std::string& utf8, std::u16string& utf16)
{
  std::basic_string<utf16_char> tmp = conv_utf8_utf16.from_bytes(utf8);
  utf16.clear();
  utf16.resize(tmp.length());
  std::copy(tmp.begin(), tmp.end(), utf16.begin());
}
Section 47.16: Finding character(s) in a string
To nd a character or another string, you can use std::string::find. It returns the position of the rst character
of the rst match. If no matches were found, the function returns std::string::npos
std::string str = "Curiosity killed the cat";
auto it = str.find("cat");
if (it != std::string::npos)
    std::cout << "Found at position: " << it << '\n';
else
    std::cout << "Not found!\n";
Found at position: 21
The search opportunities are further expanded by the following functions:
find_first_of     // Find first occurrence of characters
find_first_not_of // Find first absence of characters
find_last_of      // Find last occurrence of characters
find_last_not_of  // Find last absence of characters
These functions can allow you to search for characters from the end of the string, as well as nd the negative case
(ie. characters that are not in the string). Here is an example:
std::string str = "dog dog cat cat";
std::cout << "Found at position: " << str.find_last_of("gzx") << '\n';
Found at position: 6
Note: Be aware that the above functions do not search for substrings, but rather for characters contained in the
search string. In this case, the last occurrence of 'g' was found at position 6 (the other characters weren't found).

# Professional Notes: Chapter 49: std::vector

A vector is a dynamic array with automatically handled storage. The elements in a vector can be accessed just as
eciently as those in an array with the advantage being that vectors can dynamically change in size.
In terms of storage the vector data is (usually) placed in dynamically allocated memory thus requiring some minor
overhead; conversely C-arrays and std::array use automatic storage relative to the declared location and thus do
not have any overhead.
Section 49.1: Accessing Elements
There are two primary ways of accessing elements in a std::vector
index-based access
iterators
Index-based access:
This can be done either with the subscript operator [], or the member function at().
Both return a reference to the element at the respective position in the std::vector (unless it's a vector<bool>), so
that it can be read as well as modied (if the vector is not const).
[] and at() dier in that [] is not guaranteed to perform any bounds checking, while at() does. Accessing
elements where index < 0 or index >= size is undened behavior for [], while at() throws a std::out_of_range
exception.
Note: The examples below use C++11-style initialization for clarity, but the operators can be used with all versions
(unless marked C++11).
Version  C++11
std::vector<int> v{ 1, 2, 3 };
// using []
int a = v[1];    // a is 2
v[1] = 4;        // v now contains { 1, 4, 3 }
// using at()
int b = v.at(2); // b is 3
v.at(2) = 5;     // v now contains { 1, 4, 5 }
int c = v.at(3); // throws std::out_of_range exception
Because the at() method performs bounds checking and can throw exceptions, it is slower than []. This makes []
preferred code where the semantics of the operation guarantee that the index is in bounds. In any case, accesses
to elements of vectors are done in constant time. That means accessing to the rst element of the vector has the
same cost (in time) of accessing the second element, the third element and so on.
For example, consider this loop
for (std::size_t i = 0; i < v.size(); ++i) {
    v[i] = 1;
}
Here we know that the index variable i is always in bounds, so it would be a waste of CPU cycles to check that i is
in bounds for every call to operator[].
The front() and back() member functions allow easy reference access to the rst and last element of the vector,
respectively. These positions are frequently used, and the special accessors can be more readable than their
alternatives using []:
std::vector<int> v{ 4, 5, 6 }; // In pre-C++11 this is more verbose
int a = v.front();   // a is 4, v.front() is equivalent to v[0]
v.front() = 3;       // v now contains {3, 5, 6}
int b = v.back();    // b is 6, v.back() is equivalent to v[v.size() - 1]
v.back() = 7;        // v now contains {3, 5, 7}
Note: It is undened behavior to invoke front() or back() on an empty vector. You need to check that the
container is not empty using the empty() member function (which checks if the container is empty) before calling
front() or back(). A simple example of the use of 'empty()' to test for an empty vector follows:
int main ()
{
  std::vector<int> v;
  int sum (0);
  for (int i=1;i<=10;i++) v.push_back(i);//create and initialize the vector
  while (!v.empty())//loop through until the vector tests to be empty
  {
     sum += v.back();//keep a running total
     v.pop_back();//pop out the element which removes it from the vector
  }
  std::cout << "total: " << sum << '\n';//output the total to the user
  return 0;
}
The example above creates a vector with a sequence of numbers from 1 to 10. Then it pops the elements of the
vector out until the vector is empty (using 'empty()') to prevent undened behavior. Then the sum of the numbers
in the vector is calculated and displayed to the user.
Version  C++11
The data() method returns a pointer to the raw memory used by the std::vector to internally store its elements.
This is most often used when passing the vector data to legacy code that expects a C-style array.
std::vector<int> v{ 1, 2, 3, 4 }; // v contains {1, 2, 3, 4}
int* p = v.data(); // p points to 1
*p = 4;            // v now contains {4, 2, 3, 4}
++p;               // p points to 2
*p = 3;            // v now contains {4, 3, 3, 4}
p[1] = 2;          // v now contains {4, 3, 2, 4}
*(p + 2) = 1;      // v now contains {4, 3, 2, 1}
Version < C++11
Before C++11, the data() method can be simulated by calling front() and taking the address of the returned
value:
std::vector<int> v(4);
int* ptr = &(v.front()); // or &v[0]
This works because vectors are always guaranteed to store their elements in contiguous memory locations,
assuming the contents of the vector doesn't override unary operator&. If it does, you'll have to re-implement
std::addressof in pre-C++11. It also assumes that the vector isn't empty.
Iterators:
Iterators are explained in more detail in the example "Iterating over std::vector" and the article Iterators. In short,
they act similarly to pointers to the elements of the vector:
Version  C++11
std::vector<int> v{ 4, 5, 6 };
auto it = v.begin();
int i = *it;        // i is 4
++it;
i = *it;            // i is 5
*it = 6;            // v contains { 4, 6, 6 }
auto e = v.end();   // e points to the element after the end of v. It can be
                    // used to check whether an iterator reached the end of the vector:
++it;
it == v.end();      // false, it points to the element at position 2 (with value 6)
++it;
it == v.end();      // true
It is consistent with the standard that a std::vector<T>'s iterators actually be T*s, but most standard libraries do
not do this. Not doing this both improves error messages, catches non-portable code, and can be used to
instrument the iterators with debugging checks in non-release builds. Then, in release builds, the class wrapping
around the underlying pointer is optimized away.
You can persist a reference or a pointer to an element of a vector for indirect access. These references or pointers
to elements in the vector remain stable and access remains dened unless you add/remove elements at or before
the element in the vector, or you cause the vector capacity to change. This is the same as the rule for invalidating
iterators.
Version  C++11
std::vector<int> v{ 1, 2, 3 };
int* p = v.data() + 1;     // p points to 2
v.insert(v.begin(), 0);    // p is now invalid, accessing *p is a undefined behavior.
p = v.data() + 1;          // p points to 1
v.reserve(10);             // p is now invalid, accessing *p is a undefined behavior.
p = v.data() + 1;          // p points to 1
v.erase(v.begin());        // p is now invalid, accessing *p is a undefined behavior.
Section 49.2: Initializing a std::vector
A std::vector can be initialized in several ways while declaring it:
Version  C++11
std::vector<int> v{ 1, 2, 3 };  // v becomes {1, 2, 3}
// Different from std::vector<int> v(3, 6)
std::vector<int> v{ 3, 6 };     // v becomes {3, 6}
// Different from std::vector<int> v{3, 6} in C++11
std::vector<int> v(3, 6);  // v becomes {6, 6, 6}
std::vector<int> v(4);     // v becomes {0, 0, 0, 0}
A vector can be initialized from another container in several ways:
Copy construction (from another vector only), which copies data from v2:
std::vector<int> v(v2);
std::vector<int> v = v2;
Version  C++11
Move construction (from another vector only), which moves data from v2:
std::vector<int> v(std::move(v2));
std::vector<int> v = std::move(v2);
Iterator (range) copy-construction, which copies elements into v:
// from another vector
std::vector<int> v(v2.begin(), v2.begin() + 3); // v becomes {v2[0], v2[1], v2[2]}
// from an array
int z[] = { 1, 2, 3, 4 };
std::vector<int> v(z, z + 3);                   // v becomes {1, 2, 3}
// from a list
std::list<int> list1{ 1, 2, 3 };
std::vector<int> v(list1.begin(), list1.end()); // v becomes {1, 2, 3}
Version  C++11
Iterator move-construction, using std::make_move_iterator, which moves elements into v:
// from another vector
std::vector<int> v(std::make_move_iterator(v2.begin()),
                   std::make_move_iterator(v2.end());
// from a list
std::list<int> list1{ 1, 2, 3 };
std::vector<int> v(std::make_move_iterator(list1.begin()),
                   std::make_move_iterator(list1.end()));
With the help of the assign() member function, a std::vector can be reinitialized after its construction:
v.assign(4, 100);                      // v becomes {100, 100, 100, 100}
v.assign(v2.begin(), v2.begin() + 3);  // v becomes {v2[0], v2[1], v2[2]}
int z[] = { 1, 2, 3, 4 };
v.assign(z + 1, z + 4);                // v becomes {2, 3, 4}
Section 49.3: Deleting Elements
Deleting the last element:
std::vector<int> v{ 1, 2, 3 };
v.pop_back();                           // v becomes {1, 2}
Deleting all elements:
std::vector<int> v{ 1, 2, 3 };
v.clear();                              // v becomes an empty vector
Deleting element by index:
std::vector<int> v{ 1, 2, 3, 4, 5, 6 };
v.erase(v.begin() + 3);                 // v becomes {1, 2, 3, 5, 6}
Note: For a vector deleting an element which is not the last element, all elements beyond the deleted element have
to be copied or moved to ll the gap, see the note below and std::list.
Deleting all elements in a range:
std::vector<int> v{ 1, 2, 3, 4, 5, 6 };
v.erase(v.begin() + 1, v.begin() + 5);  // v becomes {1, 6}
Note: The above methods do not change the capacity of the vector, only the size. See Vector Size and Capacity.
The erase method, which removes a range of elements, is often used as a part of the erase-remove idiom. That is,
rst std::remove moves some elements to the end of the vector, and then erase chops them o. This is a relatively
inecient operation for any indices less than the last index of the vector because all elements after the erased
segments must be relocated to new positions. For speed critical applications that require ecient removal of
arbitrary elements in a container, see std::list.
Deleting elements by value:
std::vector<int> v{ 1, 1, 2, 2, 3, 3 };
int value_to_remove = 2;
v.erase(std::remove(v.begin(), v.end(), value_to_remove), v.end()); // v becomes {1, 1, 3, 3}
Deleting elements by condition:
// std::remove_if needs a function, that takes a vector element as argument and returns true,
// if the element shall be removed
bool _predicate(const int& element) {
    return (element > 3); // This will cause all elements to be deleted that are larger than 3
}
std::vector<int> v{ 1, 2, 3, 4, 5, 6 };
v.erase(std::remove_if(v.begin(), v.end(), _predicate), v.end()); // v becomes {1, 2, 3}
Deleting elements by lambda, without creating additional predicate function
Version  C++11
std::vector<int> v{ 1, 2, 3, 4, 5, 6 };
v.erase(std::remove_if(v.begin(), v.end(),
     [](auto& element){return element > 3;} ), v.end()
);
Deleting elements by condition from a loop:
std::vector<int> v{ 1, 2, 3, 4, 5, 6 };
std::vector<int>::iterator it = v.begin();
while (it != v.end()) {
    if (condition)
        it = v.erase(it); // after erasing, 'it' will be set to the next element in v
    else
        ++it;             // manually set 'it' to the next element in v
}
While it is important not to increment it in case of a deletion, you should consider using a dierent method when
then erasing repeatedly in a loop. Consider remove_if for a more ecient way.
Deleting elements by condition from a reverse loop:
std::vector<int> v{ -1, 0, 1, 2, 3, 4, 5, 6 };
typedef std::vector<int>::reverse_iterator rev_itr;
rev_itr it = v.rbegin();
while (it != v.rend()) { // after the loop only '0' will be in v
    int value = *it;
    if (value) {
        ++it;
        // See explanation below for the following line.
        it = rev_itr(v.erase(it.base()));
    } else
        ++it;
}
Note some points for the preceding loop:
Given a reverse iterator it pointing to some element, the method base gives the regular (non-reverse)
iterator pointing to the same element.
vector::erase(iterator) erases the element pointed to by an iterator, and returns an iterator to the
element that followed the given element.
reverse_iterator::reverse_iterator(iterator) constructs a reverse iterator from an iterator.
Put altogether, the line it = rev_itr(v.erase(it.base())) says: take the reverse iterator it, have v erase the
element pointed by its regular iterator; take the resulting iterator, construct a reverse iterator from it, and assign it
to the reverse iterator it.
Deleting all elements using v.clear() does not free up memory (capacity() of the vector remains unchanged). To
reclaim space, use:
std::vector<int>().swap(v);
Version  C++11
shrink_to_fit() frees up unused vector capacity:
v.shrink_to_fit();
The shrink_to_fit does not guarantee to really reclaim space, but most current implementations do.
Section 49.4: Iterating Over std::vector
You can iterate over a std::vector in several ways. For each of the following sections, v is dened as follows:
std::vector<int> v;
Iterating in the Forward Direction
Version  C++11
// Range based for
for(const auto& value: v) {
    std::cout << value << "\n";
}
// Using a for loop with iterator
for(auto it = std::begin(v); it != std::end(v); ++it) {
    std::cout << *it << "\n";
}
// Using for_each algorithm, using a function or functor:
void fun(int const& value) {
    std::cout << value << "\n";
}
std::for_each(std::begin(v), std::end(v), fun);
// Using for_each algorithm. Using a lambda:
std::for_each(std::begin(v), std::end(v), [](int const& value) {
    std::cout << value << "\n";
});
Version < C++11
// Using a for loop with iterator
for(std::vector<int>::iterator it = std::begin(v); it != std::end(v); ++it) {
    std::cout << *it << "\n";
}
// Using a for loop with index
for(std::size_t i = 0; i < v.size(); ++i) {
    std::cout << v[i] << "\n";
}
Iterating in the Reverse Direction
Version  C++14
// There is no standard way to use range based for for this.
// See below for alternatives.
// Using for_each algorithm
// Note: Using a lambda for clarity. But a function or functor will work
std::for_each(std::rbegin(v), std::rend(v), [](auto const& value) {
    std::cout << value << "\n";
});
// Using a for loop with iterator
for(auto rit = std::rbegin(v); rit != std::rend(v); ++rit) {
    std::cout << *rit << "\n";
}
// Using a for loop with index
for(std::size_t i = 0; i < v.size(); ++i) {
    std::cout << v[v.size() - 1 - i] << "\n";
}
Though there is no built-in way to use the range based for to reverse iterate; it is relatively simple to x this. The
range based for uses begin() and end() to get iterators and thus simulating this with a wrapper object can achieve
the results we require.
Version  C++14
template<class C>
struct ReverseRange {
  C c; // could be a reference or a copy, if the original was a temporary
  ReverseRange(C&& cin): c(std::forward<C>(cin)) {}
  ReverseRange(ReverseRange&&)=default;
  ReverseRange& operator=(ReverseRange&&)=delete;
  auto begin() const {return std::rbegin(c);}
  auto end()   const {return std::rend(c);}
};
// C is meant to be deduced, and perfect forwarded into
template<class C>
ReverseRange<C> make_ReverseRange(C&& c) {return {std::forward<C>(c)};}
int main() {
    std::vector<int> v { 1,2,3,4};
    for(auto const& value: make_ReverseRange(v)) {
        std::cout << value << "\n";
    }
}
Enforcing const elements
Since C++11 the cbegin() and cend() methods allow you to obtain a constant iterator for a vector, even if the vector
is non-const. A constant iterator allows you to read but not modify the contents of the vector which is useful to
enforce const correctness:
Version  C++11
// forward iteration
for (auto pos = v.cbegin(); pos != v.cend(); ++pos) {
   // type of pos is vector<T>::const_iterator
   // *pos = 5; // Compile error - can't write via const iterator
}
// reverse iteration
for (auto pos = v.crbegin(); pos != v.crend(); ++pos) {
   // type of pos is vector<T>::const_iterator
   // *pos = 5; // Compile error - can't write via const iterator
}
// expects Functor::operand()(T&)
for_each(v.begin(), v.end(), Functor());
// expects Functor::operand()(const T&)
for_each(v.cbegin(), v.cend(), Functor())
Version  C++17
as_const extends this to range iteration:
for (auto const& e : std::as_const(v)) {
  std::cout << e << '\n';
}
This is easy to implement in earlier versions of C++:
Version  C++14
template <class T>
constexpr std::add_const_t<T>& as_const(T& t) noexcept {
  return t;
}
A Note on Eciency
Since the class std::vector is basically a class that manages a dynamically allocated contiguous array, the same
principle explained here applies to C++ vectors. Accessing the vector's content by index is much more ecient
when following the row-major order principle. Of course, each access to the vector also puts its management
content into the cache as well, but as has been debated many times (notably here and here), the dierence in
performance for iterating over a std::vector compared to a raw array is negligible. So the same principle of
eciency for raw arrays in C also applies for C++'s std::vector.
Section 49.5: vector<bool>: The Exception To So Many, So
Many Rules
The standard (section 23.3.7) species that a specialization of vector<bool> is provided, which optimizes space by
packing the bool values, so that each takes up only one bit. Since bits aren't addressable in C++, this means that
several requirements on vector are not placed on vector<bool>:
The data stored is not required to be contiguous, so a vector<bool> can't be passed to a C API which expects
a bool array.
at(), operator [], and dereferencing of iterators do not return a reference to bool. Rather they return a
proxy object that (imperfectly) simulates a reference to a bool by overloading its assignment operators. As an
example, the following code may not be valid for std::vector<bool>, because dereferencing an iterator
does not return a reference:
Version  C++11
std::vector<bool> v = {true, false};
for (auto &b: v) { } // error
Similarly, functions expecting a bool& argument cannot be used with the result of operator [] or at() applied to
vector<bool>, or with the result of dereferencing its iterator:
  void f(bool& b);
  f(v[0]);             // error
  f(*v.begin());       // error
The implementation of std::vector<bool> is dependent on both the compiler and architecture. The specialisation
is implemented by packing n Booleans into the lowest addressable section of memory. Here, n is the size in bits of
the lowest addressable memory. In most modern systems this is 1 byte or 8 bits. This means that one byte can
store 8 Boolean values. This is an improvement over the traditional implementation where 1 Boolean value is
stored in 1 byte of memory.
Note: The below example shows possible bitwise values of individual bytes in a traditional vs. optimized
vector<bool>. This will not always hold true in all architectures. It is, however, a good way of visualising the
optimization. In the below examples a byte is represented as [x, x, x, x, x, x, x, x].
Traditional std::vector<char> storing 8 Boolean values:
Version  C++11
std::vector<char> trad_vect = {true, false, false, false, true, false, true, true};
Bitwise representation:
[0,0,0,0,0,0,0,1], [0,0,0,0,0,0,0,0], [0,0,0,0,0,0,0,0], [0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,1], [0,0,0,0,0,0,0,0], [0,0,0,0,0,0,0,1], [0,0,0,0,0,0,0,1]
Specialized std::vector<bool> storing 8 Boolean values:
Version  C++11
std::vector<bool> optimized_vect = {true, false, false, false, true, false, true, true};
Bitwise representation:
[1,0,0,0,1,0,1,1]
Notice in the above example, that in the traditional version of std::vector<bool>, 8 Boolean values take up 8 bytes
of memory, whereas in the optimized version of std::vector<bool>, they only use 1 byte of memory. This is a
signicant improvement on memory usage. If you need to pass a vector<bool> to an C-style API, you may need to
copy the values to an array, or nd a better way to use the API, if memory and performance are at risk.
Section 49.6: Inserting Elements
Appending an element at the end of a vector (by copying/moving):
struct Point {
  double x, y;
  Point(double x, double y) : x(x), y(y) {}
};
std::vector<Point> v;
Point p(10.0, 2.0);
v.push_back(p);  // p is copied into the vector.
Version  C++11
Appending an element at the end of a vector by constructing the element in place:
std::vector<Point> v;
v.emplace_back(10.0, 2.0); // The arguments are passed to the constructor of the
                           // given type (here Point). The object is constructed
                           // in the vector, avoiding a copy.
Note that std::vector does not have a push_front() member function due to performance reasons. Adding an
element at the beginning causes all existing elements in the vector to be moved. If you want to frequently insert
elements at the beginning of your container, then you might want to use std::list or std::deque instead.
Inserting an element at any position of a vector:
std::vector<int> v{ 1, 2, 3 };
v.insert(v.begin(), 9);          // v now contains {9, 1, 2, 3}
Version  C++11
Inserting an element at any position of a vector by constructing the element in place:
std::vector<int> v{ 1, 2, 3 };
v.emplace(v.begin()+1, 9);     // v now contains {1, 9, 2, 3}
Inserting another vector at any position of the vector:
std::vector<int> v(4);      // contains: 0, 0, 0, 0
std::vector<int> v2(2, 10); // contains: 10, 10
v.insert(v.begin()+2, v2.begin(), v2.end()); // contains: 0, 0, 10, 10, 0, 0
Inserting an array at any position of a vector:
std::vector<int> v(4); // contains: 0, 0, 0, 0
int a [] = {1, 2, 3}; // contains: 1, 2, 3
v.insert(v.begin()+1, a, a+sizeof(a)/sizeof(a[0])); // contains: 0, 1, 2, 3, 0, 0, 0
Use reserve() before inserting multiple elements if resulting vector size is known beforehand to avoid multiple
reallocations (see vector size and capacity):
std::vector<int> v;
v.reserve(100);
for(int i = 0; i < 100; ++i)
    v.emplace_back(i);
Be sure to not make the mistake of calling resize() in this case, or you will inadvertently create a vector with 200
elements where only the latter one hundred will have the value you intended.
Section 49.7: Using std::vector as a C array
There are several ways to use a std::vector as a C array (for example, for compatibility with C libraries). This is
possible because the elements in a vector are stored contiguously.
Version  C++11
std::vector<int> v{ 1, 2, 3 };
int* p = v.data();
In contrast to solutions based on previous C++ standards (see below), the member function .data() may also be
applied to empty vectors, because it doesn't cause undened behavior in this case.
Before C++11, you would take the address of the vector's rst element to get an equivalent pointer, if the vector
isn't empty, these both methods are interchangeable:
int* p = &v[0];      // combine subscript operator and 0 literal
int* p = &v.front(); // explicitly reference the first element
Note: If the vector is empty, v[0] and v.front() are undened and cannot be used.
When storing the base address of the vector's data, note that many operations (such as push_back, resize, etc.) can
change the data memory location of the vector, thus invalidating previous data pointers. For example:
std::vector<int> v;
int* p = v.data();
v.resize(42);      // internal memory location changed; value of p is now invalid
Section 49.8: Finding an Element in std::vector
The function std::find, dened in the <algorithm> header, can be used to nd an element in a std::vector.
std::find uses the operator== to compare elements for equality. It returns an iterator to the rst element in the
range that compares equal to the value.
If the element in question is not found, std::find returns std::vector::end (or std::vector::cend if the vector is
const).
Version < C++11
static const int arr[] = {5, 4, 3, 2, 1};
std::vector<int> v (arr, arr + sizeof(arr) / sizeof(arr[0]) );
std::vector<int>::iterator it = std::find(v.begin(), v.end(), 4);
std::vector<int>::difference_type index = std::distance(v.begin(), it);
// `it` points to the second element of the vector, `index` is 1
std::vector<int>::iterator missing = std::find(v.begin(), v.end(), 10);
std::vector<int>::difference_type index_missing = std::distance(v.begin(), missing);
// `missing` is v.end(), `index_missing` is 5 (ie. size of the vector)
Version  C++11
std::vector<int> v { 5, 4, 3, 2, 1 };
auto it = std::find(v.begin(), v.end(), 4);
auto index = std::distance(v.begin(), it);
// `it` points to the second element of the vector, `index` is 1
auto missing = std::find(v.begin(), v.end(), 10);
auto index_missing = std::distance(v.begin(), missing);
// `missing` is v.end(), `index_missing` is 5 (ie. size of the vector)
If you need to perform many searches in a large vector, then you may want to consider sorting the vector rst,
before using the binary_search algorithm.
To nd the rst element in a vector that satises a condition, std::find_if can be used. In addition to the two
parameters given to std::find, std::find_if accepts a third argument which is a function object or function
pointer to a predicate function. The predicate should accept an element from the container as an argument and
return a value convertible to bool, without modifying the container:
Version < C++11
bool isEven(int val) {
    return (val % 2 == 0);
}
struct moreThan {
    moreThan(int limit) : _limit(limit) {}
    bool operator()(int val) {
        return val > _limit;
    }
    int _limit;
};
static const int arr[] = {1, 3, 7, 8};
std::vector<int> v (arr, arr + sizeof(arr) / sizeof(arr[0]) );
std::vector<int>::iterator it = std::find_if(v.begin(), v.end(), isEven);
// `it` points to 8, the first even element
std::vector<int>::iterator missing = std::find_if(v.begin(), v.end(), moreThan(10));
// `missing` is v.end(), as no element is greater than 10
Version  C++11
// find the first value that is even
std::vector<int> v = {1, 3, 7, 8};
auto it = std::find_if(v.begin(), v.end(), [](int val){return val % 2 == 0;});
// `it` points to 8, the first even element
auto missing = std::find_if(v.begin(), v.end(), [](int val){return val > 10;});
// `missing` is v.end(), as no element is greater than 10
Section 49.9: Concatenating Vectors
One std::vector can be append to another by using the member function insert():
std::vector<int> a = {0, 1, 2, 3, 4};
std::vector<int> b = {5, 6, 7, 8, 9};
a.insert(a.end(), b.begin(), b.end());
However, this solution fails if you try to append a vector to itself, because the standard species that iterators given
to insert() must not be from the same range as the receiver object's elements.
Version  c++11
Instead of using the vector's member functions, the functions std::begin() and std::end() can be used:
a.insert(std::end(a), std::begin(b), std::end(b));
This is a more general solution, for example, because b can also be an array. However, also this solution doesn't
allow you to append a vector to itself.
If the order of the elements in the receiving vector doesn't matter, considering the number of elements in each
vector can avoid unnecessary copy operations:
if (b.size() < a.size())
  a.insert(a.end(), b.begin(), b.end());
else
  b.insert(b.end(), a.begin(), a.end());
Section 49.10: Matrices Using Vectors
Vectors can be used as a 2D matrix by dening them as a vector of vectors.
A matrix with 3 rows and 4 columns with each cell initialised as 0 can be dened as:
std::vector<std::vector<int> > matrix(3, std::vector<int>(4));
Version  C++11
The syntax for initializing them using initialiser lists or otherwise are similar to that of a normal vector.
  std::vector<std::vector<int>> matrix = { {0,1,2,3},
                                           {4,5,6,7},
                                           {8,9,10,11}
                                         };
Values in such a vector can be accessed similar to a 2D array
int var = matrix[0][2];
Iterating over the entire matrix is similar to that of a normal vector but with an extra dimension.
for(int i = 0; i < 3; ++i)
{
    for(int j = 0; j < 4; ++j)
    {
        std::cout << matrix[i][j] << std::endl;
    }
}
Version  C++11
for(auto& row: matrix)
{
    for(auto& col : row)
    {
        std::cout << col << std::endl;
    }
}
A vector of vectors is a convenient way to represent a matrix but it's not the most ecient: individual vectors are
scattered around memory and the data structure isn't cache friendly.
Also, in a proper matrix, the length of every row must be the same (this isn't the case for a vector of vectors). The
additional exibility can be a source of errors.
Section 49.11: Using a Sorted Vector for Fast Element Lookup
The <algorithm> header provides a number of useful functions for working with sorted vectors.
An important prerequisite for working with sorted vectors is that the stored values are comparable with <.
An unsorted vector can be sorted by using the function std::sort():
std::vector<int> v;
// add some code here to fill v with some elements
std::sort(v.begin(), v.end());
Sorted vectors allow ecient element lookup using the function std::lower_bound(). Unlike std::find(), this
performs an ecient binary search on the vector. The downside is that it only gives valid results for sorted input
ranges:
// search the vector for the first element with value 42
std::vector<int>::iterator it = std::lower_bound(v.begin(), v.end(), 42);
if (it != v.end() && *it == 42) {
    // we found the element!
}
Note: If the requested value is not part of the vector, std::lower_bound() will return an iterator to the rst element
that is greater than the requested value. This behavior allows us to insert a new element at its right place in an
already sorted vector:
int const new_element = 33;
v.insert(std::lower_bound(v.begin(), v.end(), new_element), new_element);
If you need to insert a lot of elements at once, it might be more ecient to call push_back() for all them rst and
then call std::sort() once all elements have been inserted. In this case, the increased cost of the sorting can pay
o against the reduced cost of inserting new elements at the end of the vector and not in the middle.
If your vector contains multiple elements of the same value, std::lower_bound() will try to return an iterator to the
rst element of the searched value. However, if you need to insert a new element after the last element of the
searched value, you should use the function std::upper_bound() as this will cause less shifting around of
elements:
v.insert(std::upper_bound(v.begin(), v.end(), new_element), new_element);
If you need both the upper bound and the lower bound iterators, you can use the function std::equal_range() to
retrieve both of them eciently with one call:
std::pair<std::vector<int>::iterator,
          std::vector<int>::iterator> rg = std::equal_range(v.begin(), v.end(), 42);
std::vector<int>::iterator lower_bound = rg.first;
std::vector<int>::iterator upper_bound = rg.second;
In order to test whether an element exists in a sorted vector (although not specic to vectors), you can use the
function std::binary_search():
bool exists = std::binary_search(v.begin(), v.end(), value_to_find);
Section 49.12: Reducing the Capacity of a Vector
A std::vector automatically increases its capacity upon insertion as needed, but it never reduces its capacity after
element removal.
// Initialize a vector with 100 elements
std::vector<int> v(100);
// The vector's capacity is always at least as large as its size
auto const old_capacity = v.capacity();
// old_capacity >= 100
// Remove half of the elements
v.erase(v.begin() + 50, v.end());  // Reduces the size from 100 to 50 (v.size() == 50),
                                   // but not the capacity (v.capacity() == old_capacity)
To reduce its capacity, we can copy the contents of a vector to a new temporary vector. The new vector will have the
minimum capacity that is needed to store all elements of the original vector. If the size reduction of the original
vector was signicant, then the capacity reduction for the new vector is likely to be signicant. We can then swap
the original vector with the temporary one to retain its minimized capacity:
std::vector<int>(v).swap(v);
Version  C++11
In C++11 we can use the shrink_to_fit() member function for a similar eect:
v.shrink_to_fit();
Note: The shrink_to_fit() member function is a request and doesn't guarantee to reduce capacity.
Section 49.13: Vector size and capacity
Vector size is simply the number of elements in the vector:
1.
Current vector size is queried by size() member function. Convenience empty() function returns true if size
is 0:
vector<int> v = { 1, 2, 3 }; // size is 3
const vector<int>::size_type size = v.size();
cout << size << endl; // prints 3
cout << boolalpha << v.empty() << endl; // prints false
2.
Default constructed vector starts with a size of 0:
vector<int> v; // size is 0
cout << v.size() << endl; // prints 0
3.
Adding N elements to vector increases size by N (e.g. by push_back(), insert() or resize() functions).
4.
Removing N elements from vector decreases size by N (e.g. by pop_back(), erase() or clear() functions).
5.
Vector has an implementation-specic upper limit on its size, but you are likely to run out of RAM before
reaching it:
vector<int> v;
const vector<int>::size_type max_size = v.max_size();
cout << max_size << endl; // prints some large number
v.resize( max_size ); // probably won't work
v.push_back( 1 ); // definitely won't work
Common mistake: size is not necessarily (or even usually) int:
// !!!bad!!!evil!!!
vector<int> v_bad( N, 1 ); // constructs large N size vector
for( int i = 0; i < v_bad.size(); ++i ) { // size is not supposed to be int!
    do_something( v_bad[i] );
}
Vector capacity diers from size. While size is simply how many elements the vector currently has, capacity is for
how many elements it allocated/reserved memory for. That is useful, because too frequent (re)allocations of too
large sizes can be expensive.
1.
Current vector capacity is queried by capacity() member function. Capacity is always greater or equal to
size:
vector<int> v = { 1, 2, 3 }; // size is 3, capacity is >= 3
const vector<int>::size_type capacity = v.capacity();
cout << capacity << endl; // prints number >= 3
2.
You can manually reserve capacity by reserve( N ) function (it changes vector capacity to N):
// !!!bad!!!evil!!!
vector<int> v_bad;
for( int i = 0; i < 10000; ++i ) {
    v_bad.push_back( i ); // possibly lot of reallocations
}
// good
vector<int> v_good;
v_good.reserve( 10000 ); // good! only one allocation
for( int i = 0; i < 10000; ++i ) {
    v_good.push_back( i ); // no allocations needed anymore
}
3.
You can request for the excess capacity to be released by shrink_to_fit() (but the implementation doesn't
have to obey you). This is useful to conserve used memory:
vector<int> v = { 1, 2, 3, 4, 5 }; // size is 5, assume capacity is 6
v.shrink_to_fit(); // capacity is 5 (or possibly still 6)
cout << boolalpha << v.capacity() == v.size() << endl; // prints likely true (but possibly
false)
Vector partly manages capacity automatically, when you add elements it may decide to grow. Implementers like to
use 2 or 1.5 for the grow factor (golden ratio would be the ideal value - but is impractical due to being rational
number). On the other hand vector usually do not automatically shrink. For example:
vector<int> v; // capacity is possibly (but not guaranteed) to be 0
v.push_back( 1 ); // capacity is some starter value, likely 1
v.clear(); // size is 0 but capacity is still same as before!

# Professional Notes: Chapter 50: std::map

To use any of std::map or std::multimap the header le <map> should be included.
std::map and std::multimap both keep their elements sorted according to the ascending order of keys. In
case of std::multimap, no sorting occurs for the values of the same key.
The basic dierence between std::map and std::multimap is that the std::map one does not allow duplicate
values for the same key where std::multimap does.
Maps are implemented as binary search trees. So search(), insert(), erase() takes (log n) time in
average. For constant time operation use std::unordered_map.
size() and empty() functions have (1) time complexity, number of nodes is cached to avoid walking
through tree each time these functions are called.
Section 50.1: Accessing elements
An std::map takes (key, value) pairs as input.
Consider the following example of std::map initialization:
std::map < std::string, int > ranking { std::make_pair("stackoverflow", 2),
                                        std::make_pair("docs-beta", 1) };
In an std::map , elements can be inserted as follows:
ranking["stackoverflow"]=2;
ranking["docs-beta"]=1;
In the above example, if the key stackoverflow is already present, its value will be updated to 2. If it isn't already
present, a new entry will be created.
In an std::map, elements can be accessed directly by giving the key as an index:
std::cout << ranking[ "stackoverflow" ] << std::endl;
Note that using the operator[] on the map will actually insert a new value with the queried key into the map. This
means that you cannot use it on a const std::map, even if the key is already stored in the map. To prevent this
insertion, check if the element exists (for example by using find()) or use at() as described below.
Version  C++11
Elements of a std::map can be accessed with at():
std::cout << ranking.at("stackoverflow") << std::endl;
Note that at() will throw an std::out_of_range exception if the container does not contain the requested
element.
In both containers std::map and std::multimap, elements can be accessed using iterators:
Version  C++11
// Example using begin()
std::multimap < int, std::string > mmp { std::make_pair(2, "stackoverflow"),
                                         std::make_pair(1, "docs-beta"),
                                         std::make_pair(2, "stackexchange")  };
auto it = mmp.begin();
std::cout << it->first << " : " << it->second << std::endl; // Output: "1 : docs-beta"
it++;
std::cout << it->first << " : " << it->second << std::endl; // Output: "2 : stackoverflow"
it++;
std::cout << it->first << " : " << it->second << std::endl; // Output: "2 : stackexchange"
// Example using rbegin()
std::map < int, std::string > mp {  std::make_pair(2, "stackoverflow"),
                                    std::make_pair(1, "docs-beta"),
                                    std::make_pair(2, "stackexchange")  };
auto it2 = mp.rbegin();
std::cout << it2->first << " : " << it2->second << std::endl; // Output: "2 : stackoverflow"
it2++;
std::cout << it2->first << " : " << it2->second << std::endl; // Output: "1 : docs-beta"
Section 50.2: Inserting elements
An element can be inserted into a std::map only if its key is not already present in the map. Given for example:
std::map< std::string, size_t > fruits_count;
A key-value pair is inserted into a std::map through the insert() member function. It requires a pair as an
argument:
fruits_count.insert({"grapes", 20});
fruits_count.insert(make_pair("orange", 30));
fruits_count.insert(pair<std::string, size_t>("banana", 40));
fruits_count.insert(map<std::string, size_t>::value_type("cherry", 50));
The insert() function returns a pair consisting of an iterator and a bool value:
If the insertion was successful, the iterator points to the newly inserted element, and the bool value is
true.
If there was already an element with the same key, the insertion fails. When that happens, the iterator
points to the element causing the conict, and the bool is value is false.
The following method can be used to combine insertion and searching operation:
auto success = fruits_count.insert({"grapes", 20});
if (!success.second) {           // we already have 'grapes' in the map
    success.first->second += 20; // access the iterator to update the value
}
For convenience, the std::map container provides the subscript operator to access elements in the map and
to insert new ones if they don't exist:
fruits_count["apple"] = 10;
While simpler, it prevents the user from checking if the element already exists. If an element is missing,
std::map::operator[] implicitly creates it, initializing it with the default constructor before overwriting it
with the supplied value.
insert() can be used to add several elements at once using a braced list of pairs. This version of insert()
returns void:
fruits_count.insert({{"apricot", 1}, {"jackfruit", 1}, {"lime", 1}, {"mango", 7}});
insert() can also be used to add elements by using iterators denoting the begin and end of value_type
values:
std::map< std::string, size_t > fruit_list{ {"lemon", 0}, {"olive", 0}, {"plum", 0}};
fruits_count.insert(fruit_list.begin(), fruit_list.end());
Example:
std::map<std::string, size_t> fruits_count;
std::string fruit;
while(std::cin >> fruit){
    // insert an element with 'fruit' as key and '1' as value
    // (if the key is already stored in fruits_count, insert does nothing)
    auto ret = fruits_count.insert({fruit, 1});
    if(!ret.second){            // 'fruit' is already in the map
        ++ret.first->second;    // increment the counter
    }
}
Time complexity for an insertion operation is O(log n) because std::map are implemented as trees.
Version  C++11
A pair can be constructed explicitly using make_pair() and emplace():
std::map< std::string , int > runs;
runs.emplace("Babe Ruth", 714);
runs.insert(make_pair("Barry Bonds", 762));
If we know where the new element will be inserted, then we can use emplace_hint() to specify an iterator hint. If
the new element can be inserted just before hint, then the insertion can be done in constant time. Otherwise it
behaves in the same way as emplace():
std::map< std::string , int > runs;
auto it = runs.emplace("Barry Bonds", 762); // get iterator to the inserted element
// the next element will be before "Barry Bonds", so it is inserted before 'it'
runs.emplace_hint(it, "Babe Ruth", 714);
Section 50.3: Searching in std::map or in std::multimap
There are several ways to search a key in std::map or in std::multimap.
To get the iterator of the rst occurrence of a key, the find() function can be used. It returns end() if the key
does not exist.
  std::multimap< int , int > mmp{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {3, 4}, {6, 7} };
  auto it = mmp.find(6);
  if(it!=mmp.end())
      std::cout << it->first << ", " << it->second << std::endl; //prints: 6, 5
  else
      std::cout << "Value does not exist!" << std::endl;
  it = mmp.find(66);
  if(it!=mmp.end())
      std::cout << it->first << ", " << it->second << std::endl;
  else
      std::cout << "Value does not exist!" << std::endl; // This line would be executed.
Another way to nd whether an entry exists in std::map or in std::multimap is using the count() function,
which counts how many values are associated with a given key. Since std::map associates only one value
with each key, its count() function can only return 0 (if the key is not present) or 1 (if it is). For
std::multimap, count() can return values greater than 1 since there can be several values associated with
the same key.
 std::map< int , int > mp{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {3, 4}, {6, 7} };
 if(mp.count(3) > 0) // 3 exists as a key in map
     std::cout << "The key exists!" << std::endl; // This line would be executed.
 else
     std::cout << "The key does not exist!" << std::endl;
If you only care whether some element exists, find is strictly better: it documents your intent and, for
multimaps, it can stop once the rst matching element has been found.
In the case of std::multimap, there could be several elements having the same key. To get this range, the
equal_range() function is used which returns std::pair having iterator lower bound (inclusive) and upper
bound (exclusive) respectively. If the key does not exist, both iterators would point to end().
  auto eqr = mmp.equal_range(6);
  auto st = eqr.first, en = eqr.second;
  for(auto it = st; it != en; ++it){
      std::cout << it->first << ", " << it->second << std::endl;
  }
      // prints: 6, 5
      //         6, 7
Section 50.4: Initializing a std::map or std::multimap
std::map and std::multimap both can be initialized by providing key-value pairs separated by comma. Key-value
pairs could be provided by either {key, value} or can be explicitly created by std::make_pair(key, value). As
std::map does not allow duplicate keys and comma operator performs right to left, the pair on right would be
overwritten with the pair with same key on the left.
std::multimap < int, std::string > mmp { std::make_pair(2, "stackoverflow"),
                                     std::make_pair(1, "docs-beta"),
                                     std::make_pair(2, "stackexchange")  };
// 1 docs-beta
// 2 stackoverflow
// 2 stackexchange
std::map < int, std::string > mp {  std::make_pair(2, "stackoverflow"),
                                std::make_pair(1, "docs-beta"),
                                std::make_pair(2, "stackexchange")  };
// 1 docs-beta
// 2 stackoverflow
Both could be initialized with iterator.
// From std::map or std::multimap iterator
std::multimap< int , int > mmp{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {6, 8}, {3, 4},
                               {6, 7} };
                       // {1, 2}, {3, 4}, {3, 4}, {6, 5}, {6, 8}, {6, 7}, {8, 9}
auto it = mmp.begin();
std::advance(it,3); //moved cursor on first {6, 5}
std::map< int, int > mp(it, mmp.end()); // {6, 5}, {8, 9}
//From std::pair array
std::pair< int, int > arr[10];
arr[0] = {1, 3};
arr[1] = {1, 5};
arr[2] = {2, 5};
arr[3] = {0, 1};
std::map< int, int > mp(arr,arr+4); //{0 , 1}, {1, 3}, {2, 5}
//From std::vector of std::pair
std::vector< std::pair<int, int> > v{ {1, 5}, {5, 1}, {3, 6}, {3, 2} };
std::multimap< int, int > mp(v.begin(), v.end());
                        // {1, 5}, {3, 6}, {3, 2}, {5, 1}
Section 50.5: Checking number of elements
The container std::map has a member function empty(), which returns true or false, depending on whether the
map is empty or not. The member function size() returns the number of element stored in a std::map container:
std::map<std::string , int> rank {{"facebook.com", 1} ,{"google.com", 2}, {"youtube.com", 3}};
if(!rank.empty()){
    std::cout << "Number of elements in the rank map: " << rank.size() << std::endl;
}
else{
    std::cout << "The rank map is empty" << std::endl;
}
Section 50.6: Types of Maps
Regular Map
A map is an associative container, containing key-value pairs.
#include <string>
#include <map>
std::map<std::string, size_t> fruits_count;
In the above example, std::string is the key type, and size_t is a value.
The key acts as an index in the map. Each key must be unique, and must be ordered.
If you need mutliple elements with the same key, consider using multimap (explained below)
If your value type does not specify any ordering, or you want to override the default ordering, you may
provide one:
#include <string>
#include <map>
#include <cstring>
struct StrLess {
    bool operator()(const std::string& a, const std::string& b) {
        return strncmp(a.c_str(), b.c_str(), 8)<0;
               //compare only up to 8 first characters
    }
}
std::map<std::string, size_t, StrLess> fruits_count2;
If StrLess comparator returns false for two keys, they are considered the same even if their actual contents
dier.
Multi-Map
Multimap allows multiple key-value pairs with the same key to be stored in the map. Otherwise, its interface and
creation is very similar to the regular map.
 #include <string>
 #include <map>
 std::multimap<std::string, size_t> fruits_count;
 std::multimap<std::string, size_t, StrLess> fruits_count2;
Hash-Map (Unordered Map)
A hash map stores key-value pairs similar to a regular map. It does not order the elements with respect to the key
though. Instead, a hash value for the key is used to quickly access the needed key-value pairs.
#include <string>
#include <unordered_map>
std::unordered_map<std::string, size_t> fruits_count;
Unordered maps are usually faster, but the elements are not stored in any predictable order. For example, iterating
over all elements in an unordered_map gives the elements in a seemingly random order.
Section 50.7: Deleting elements
Removing all elements:
std::multimap< int , int > mmp{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {3, 4}, {6, 7} };
mmp.clear(); //empty multimap
Removing element from somewhere with the help of iterator:
std::multimap< int , int > mmp{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {3, 4}, {6, 7} };
                            // {1, 2}, {3, 4}, {3, 4}, {6, 5}, {6, 7}, {8, 9}
auto it = mmp.begin();
std::advance(it,3); // moved cursor on first {6, 5}
mmp.erase(it); // {1, 2}, {3, 4}, {3, 4}, {6, 7}, {8, 9}
Removing all elements in a range:
std::multimap< int , int > mmp{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {3, 4}, {6, 7} };
                            // {1, 2}, {3, 4}, {3, 4}, {6, 5}, {6, 7}, {8, 9}
auto it = mmp.begin();
auto it2 = it;
it++; //moved first cursor on first {3, 4}
std::advance(it2,3);  //moved second cursor on first {6, 5}
mmp.erase(it,it2); // {1, 2}, {6, 5}, {6, 7}, {8, 9}
Removing all elements having a provided value as key:
std::multimap< int , int > mmp{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {3, 4}, {6, 7} };
                            // {1, 2}, {3, 4}, {3, 4}, {6, 5}, {6, 7}, {8, 9}
mmp.erase(6); // {1, 2}, {3, 4}, {3, 4}, {8, 9}
Removing elements that satisfy a predicate pred:
std::map<int,int> m;
auto it = m.begin();
while (it != m.end())
{
   if (pred(*it))
       it = m.erase(it);
   else
       ++it;
}
Section 50.8: Iterating over std::map or std::multimap
std::map or std::multimap could be traversed by the following ways:
std::multimap< int , int > mmp{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {3, 4}, {6, 7} };
//Range based loop - since C++11
for(const auto &x: mmp)
    std::cout<< x.first <<":"<< x.second << std::endl;
//Forward iterator for loop: it would loop through first element to last element
//it will be a std::map< int, int >::iterator
for (auto it = mmp.begin(); it != mmp.end(); ++it)
std::cout<< it->first <<":"<< it->second << std::endl; //Do something with iterator
//Backward iterator for loop: it would loop through last element to first element
//it will be a std::map< int, int >::reverse_iterator
for (auto it = mmp.rbegin(); it != mmp.rend(); ++it)
std::cout<< it->first <<" "<< it->second << std::endl; //Do something with iterator
While iterating over a std::map or a std::multimap, the use of auto is preferred to avoid useless implicit
conversions (see this SO answer for more details).
Section 50.9: Creating std::map with user-dened types as
key
In order to be able to use a class as the key in a map, all that is required of the key is that it be copiable and
assignable. The ordering within the map is dened by the third argument to the template (and the argument to
the constructor, if used). This defaults to std::less<KeyType>, which defaults to the < operator, but there's no
requirement to use the defaults. Just write a comparison operator (preferably as a functional object):
struct CmpMyType
{
    bool operator()( MyType const& lhs, MyType const& rhs ) const
    {
        //  ...
    }
};
In C++, the "compare" predicate must be a strict weak ordering. In particular, compare(X,X) must return false for
any X. i.e. if CmpMyType()(a, b) returns true, then CmpMyType()(b, a) must return false, and if both return false,
the elements are considered equal (members of the same equivalence class).
Strict Weak Ordering
This is a mathematical term to dene a relationship between two objects.
Its denition is:
Two objects x and y are equivalent if both f(x, y) and f(y, x) are false. Note that an object is always (by the
irreexivity invariant) equivalent to itself.
In terms of C++ this means if you have two objects of a given type, you should return the following values when
compared with the operator <.
X    a;
X    b;
Condition:                  Test:     Result
a is equivalent to b:       a < b     false
a is equivalent to b        b < a     false
a is less than b            a < b     true
a is less than b            b < a     false
b is less than a            a < b     false
b is less than a            b < a     true
How you dene equivalent/less is totally dependent on the type of your object.

# Professional Notes: Chapter 62: Standard Library Algorithms

Section 62.1: std::next_permutation
template< class Iterator >
bool next_permutation( Iterator first, Iterator last );
template< class Iterator, class Compare >
bool next_permutation( Iterator first, Iterator last, Compare cmpFun );
Eects:
Sift the data sequence of the range [rst, last) into the next lexicographically higher permutation. If cmpFun is
provided, the permutation rule is customized.
Parameters:
first- the beginning of the range to be permutated, inclusive
last - the end of the range to be permutated, exclusive
Return Value:
Returns true if such permutation exists.
Otherwise the range is swaped to the lexicographically smallest permutation and return false.
Complexity:
O(n), n is the distance from first to last.
Example:
std::vector< int > v { 1, 2, 3 };
do
{
   for( int i = 0; i < v.size(); i += 1 )
   {
       std::cout << v[i];
   }
   std::cout << std::endl;
}while( std::next_permutation( v.begin(), v.end() ) );
print all the permutation cases of 1,2,3 in lexicographically-increasing order.
output:
Section 62.2: std::for_each
template<class InputIterator, class Function>
    Function for_each(InputIterator first, InputIterator last, Function f);
Eects:
Applies f to the result of dereferencing every iterator in the range [first, last) starting from first and
proceeding to last - 1.
Parameters:
first, last - the range to apply f to.
f - callable object which is applied to the result of dereferencing every iterator in the range [first, last).
Return value:
f (until C++11) and std::move(f) (since C++11).
Complexity:
Applies f exactly last - first times.
Example:
Version  c++11
std::vector<int> v { 1, 2, 4, 8, 16 };
std::for_each(v.begin(), v.end(), [](int elem) { std::cout << elem << " "; });
Applies the given function for every element of the vector v printing this element to stdout.
Section 62.3: std::accumulate
Dened in header <numeric>
template<class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init); // (1)
template<class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation f); // (2)
Eects:
std::accumulate performs fold operation using f function on range [first, last) starting with init as
accumulator value.
Eectively it's equivalent of:
T acc = init;
for (auto it = first; first != last; ++it)
    acc = f(acc, *it);
return acc;
In version (1) operator+ is used in place of f, so accumulate over container is equivalent of sum of container
elements.
Parameters:
first, last - the range to apply f to.
init - initial value of accumulator.
f - binary folding function.
Return value:
Accumulated value of f applications.
Complexity:
O(nk), where n is the distance from first to last, O(k) is complexity of f function.
Example:
Simple sum example:
std::vector<int> v { 2, 3, 4 };
auto sum = std::accumulate(v.begin(), v.end(), 1);
std::cout << sum << std::endl;
Output:
Convert digits to number:
Version < c++11
class Converter {
public:
    int operator()(int a, int d) const { return a * 10 + d; }
};
and later
const int ds[3] = {1, 2, 3};
int n = std::accumulate(ds, ds + 3, 0, Converter());
std::cout << n << std::endl;
Version  c++11
const std::vector<int> ds = {1, 2, 3};
int n = std::accumulate(ds.begin(), ds.end(),
                        0,
                        [](int a, int d) { return a * 10 + d; });
std::cout << n << std::endl;
Output:
Section 62.4: std::nd
template <class InputIterator, class T>
InputIterator find (InputIterator first, InputIterator last, const T& val);
Eects
Finds the rst occurrence of val within the range [rst, last)
Parameters
first => iterator pointing to the beginning of the range last => iterator pointing to the end of the range val => The
value to nd within the range
Return
An iterator that points to the rst element within the range that is equal(==) to val, the iterator points to last if val is
not found.
Example
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
int main(int argc, const char * argv[]) {
  //create a vector
  vector<int> intVec {4, 6, 8, 9, 10, 30, 55,100, 45, 2, 4, 7, 9, 43, 48};
  //define iterators
  vector<int>::iterator  itr_9;
  vector<int>::iterator  itr_43;
  vector<int>::iterator  itr_50;
  //calling find
  itr_9 = find(intVec.begin(), intVec.end(), 9); //occurs twice
  itr_43 = find(intVec.begin(), intVec.end(), 43); //occurs once
  //a value not in the vector
  itr_50 = find(intVec.begin(), intVec.end(), 50); //does not occur
  cout << "first occurrence of: " << *itr_9 << endl;
  cout << "only occurrence of: " << *itr_43 << Lendl;
  /*
    let's prove that itr_9 is pointing to the first occurrence
    of 9 by looking at the element after 9, which should be 10
    not 43
  */
  cout << "element after first 9: " << *(itr_9 + 1) << ends;
  /*
    to avoid dereferencing intVec.end(), lets look at the
    element right before the end
  */
  cout << "last element: " << *(itr_50 - 1) << endl;
  return 0;
}
Output
first occurrence of: 9
only occurrence of: 43
element after first 9: 10
last element: 48
Section 62.5: std::min_element
template <class ForwardIterator>
ForwardIterator min_element (ForwardIterator first, ForwardIterator last);
template <class ForwardIterator, class Compare>
ForwardIterator min_element (ForwardIterator first, ForwardIterator last,Compare comp);
Eects
Finds the minimum element in a range
Parameters
first - iterator pointing to the beginning of the range
last - iterator pointing to the end of the range comp - a function pointer or function object that takes two
arguments and returns true or false indicating whether argument is less than argument 2. This function should not
modify inputs
Return
Iterator to the minimum element in the range
Complexity
Linear in one less than the number of elements compared.
Example
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>  //to use make_pair
using namespace std;
//function compare two pairs
bool pairLessThanFunction(const pair<string, int> &p1, const pair<string, int> &p2)
{
  return p1.second < p2.second;
}
int main(int argc, const char * argv[]) {
  vector<int> intVec {30,200,167,56,75,94,10,73,52,6,39,43};
  vector<pair<string, int>> pairVector = {make_pair("y", 25), make_pair("b", 2), make_pair("z",
26), make_pair("e", 5) };
  // default using < operator
  auto minInt = min_element(intVec.begin(), intVec.end());
  //Using pairLessThanFunction
  auto minPairFunction = min_element(pairVector.begin(), pairVector.end(), pairLessThanFunction);
  //print minimum of intVector
  cout << "min int from default: " << *minInt << endl;
  //print minimum of pairVector
  cout << "min pair from PairLessThanFunction: " << (*minPairFunction).second << endl;
  return 0;
}
Output
min int from default: 6
min pair from PairLessThanFunction: 2
Section 62.6: std::nd_if
template <class InputIterator, class UnaryPredicate>
InputIterator find_if (InputIterator first, InputIterator last, UnaryPredicate pred);
Eects
Finds the rst element in a range for which the predicate function pred returns true.
Parameters
first => iterator pointing to the beginning of the range last => iterator pointing to the end of the range pred =>
predicate function(returns true or false)
Return
An iterator that points to the rst element within the range the predicate function pred returns true for. The
iterator points to last if val is not found
Example
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
/*
    define some functions to use as predicates
*/
//Returns true if x is multiple of 10
bool multOf10(int x) {
  return x % 10 == 0;
}
//returns true if item greater than passed in parameter
class Greater {
  int _than;
public:
  Greater(int th):_than(th){
  }
  bool operator()(int data) const
  {
    return data > _than;
  }
};
int main()
{
  vector<int> myvec {2, 5, 6, 10, 56, 7, 48, 89, 850, 7, 456};
  //with a lambda function
  vector<int>::iterator gt10 = find_if(myvec.begin(), myvec.end(), [](int x){return x>10;}); // >=
C++11
  //with a function pointer
  vector<int>::iterator pow10 = find_if(myvec.begin(), myvec.end(), multOf10);
  //with functor
  vector<int>::iterator gt5 = find_if(myvec.begin(), myvec.end(), Greater(5));
  //not Found
  vector<int>::iterator nf = find_if(myvec.begin(), myvec.end(), Greater(1000)); // nf points to
myvec.end()
  //check if pointer points to myvec.end()
  if(nf != myvec.end()) {
    cout << "nf points to: " << *nf << endl;
  }
  else {
    cout << "item not found" << endl;
  }
  cout << "First item >   10: " << *gt10  << endl;
  cout << "First Item n * 10: " << *pow10 << endl;
  cout << "First Item >    5: " << *gt5   << endl;
  return 0;
}
Output
item not found
First item >   10: 56
First Item n * 10: 10
First Item >    5: 6
Section 62.7: Using std::nth_element To Find The Median (Or
Other Quantiles)
The std::nth_element algorithm takes three iterators: an iterator to the beginning, nth position, and end. Once the
function returns, the nth element (by order) will be the nth smallest element. (The function has more elaborate
overloads, e.g., some taking comparison functors; see the above link for all the variations.)
Note This function is very ecient - it has linear complexity.
For the sake of this example, let's dene the median of a sequence of length n as the element that would be in
position n / 2. For example, the median of a sequence of length 5 is the 3rd smallest element, and so is the
median of a sequence of length 6.
To use this function to nd the median, we can use the following. Say we start with
std::vector<int> v{5, 1, 2, 3, 4};
std::vector<int>::iterator b = v.begin();
std::vector<int>::iterator e = v.end();
std::vector<int>::iterator med = b;
std::advance(med, v.size() / 2);
// This makes the 2nd position hold the median.
std::nth_element(b, med, e);
// The median is now at v[2].
To nd the pth quantile, we would change some of the lines above:
const std::size_t pos = p * std::distance(b, e);
std::advance(nth, pos);
and look for the quantile at position pos.
Section 62.8: std::count
template <class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type
count (InputIterator first, InputIterator last, const T& val);
Eects
Counts the number of elements that are equal to val
Parameters
first => iterator pointing to the beginning of the range
last => iterator pointing to the end of the range
val => The occurrence of this value in the range will be counted
Return
The number of elements in the range that are equal(==) to val.
Example
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
int main(int argc, const char * argv[]) {
  //create vector
  vector<int> intVec{4,6,8,9,10,30,55,100,45,2,4,7,9,43,48};
  //count occurrences of 9, 55, and 101
  size_t count_9 = count(intVec.begin(), intVec.end(), 9); //occurs twice
  size_t count_55 = count(intVec.begin(), intVec.end(), 55); //occurs once
  size_t count_101 = count(intVec.begin(), intVec.end(), 101); //occurs once
  //print result
  cout << "There are " << count_9  << " 9s"<< endl;
  cout << "There is " << count_55  << " 55"<< endl;
  cout << "There is " << count_101  << " 101"<< ends;
  //find the first element == 4 in the vector
  vector<int>::iterator itr_4 = find(intVec.begin(), intVec.end(), 4);
  //count its occurrences in the vector starting from the first one
  size_t count_4 = count(itr_4, intVec.end(), *itr_4); // should be 2
  cout << "There are " << count_4  << " " << *itr_4 << endl;
  return 0;
}
Output
There are 2 9s
There is 1 55
There is 0 101
There are 2 4
Section 62.9: std::count_if
template <class InputIterator, class UnaryPredicate>
typename iterator_traits<InputIterator>::difference_type
count_if (InputIterator first, InputIterator last, UnaryPredicate red);
Eects
Counts the number of elements in a range for which a specied predicate function is true
Parameters
first => iterator pointing to the beginning of the range last => iterator pointing to the end of the range red =>
predicate function(returns true or false)
Return
The number of elements within the specied range for which the predicate function returned true.
Example
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
/*
    Define a few functions to use as predicates
*/
//return true if number is odd
bool isOdd(int i){
  return i%2 == 1;
}
//functor that returns true if number is greater than the value of the constructor parameter
provided
class Greater {
  int _than;
public:
  Greater(int th): _than(th){}
  bool operator()(int i){
    return i > _than;
  }
};
int main(int argc, const char * argv[]) {
  //create a vector
  vector<int> myvec = {1,5,8,0,7,6,4,5,2,1,5,0,6,9,7};
  //using a lambda function to count even numbers
  size_t evenCount = count_if(myvec.begin(), myvec.end(), [](int i){return i % 2 == 0;}); // >=
C++11
  //using function pointer to count odd number in the first half of the vector
  size_t oddCount = count_if(myvec.begin(), myvec.end()- myvec.size()/2, isOdd);
  //using a functor to count numbers greater than 5
  size_t greaterCount = count_if(myvec.begin(), myvec.end(), Greater(5));
  cout << "vector size: " << myvec.size() << endl;
  cout << "even numbers: " << evenCount << " found" << endl;
  cout << "odd numbers: " << oddCount << " found" << endl;
  cout << "numbers > 5: " << greaterCount << " found"<< endl;
  return 0;
}
Output
vector size: 15
even numbers: 7 found
odd numbers: 4 found
numbers > 5: 6 found

# Professional Notes: Chapter 67: Sorting

Section 67.1: Sorting and sequence containers
std::sort, found in the standard library header algorithm, is a standard library algorithm for sorting a range of
values, dened by a pair of iterators. std::sort takes as the last parameter a functor used to compare two values;
this is how it determines the order. Note that std::sort is not stable.
The comparison function must impose a Strict, Weak Ordering on the elements. A simple less-than (or greater-than)
comparison will suce.
A container with random-access iterators can be sorted using the std::sort algorithm:
Version  C++11
#include <vector>
#include <algorithm>
std::vector<int> MyVector = {3, 1, 2}
//Default comparison of <
std::sort(MyVector.begin(), MyVector.end());
std::sort requires that its iterators are random access iterators. The sequence containers std::list and
std::forward_list (requiring C++11) do not provide random access iterators, so they cannot be used with
std::sort. However, they do have sort member functions which implement a sorting algorithm that works with
their own iterator types.
Version  C++11
#include <list>
#include <algorithm>
std::list<int> MyList = {3, 1, 2}
//Default comparison of <
//Whole list only.
MyList.sort();
Their member sort functions always sort the entire list, so they cannot sort a sub-range of elements. However,
since list and forward_list have fast splicing operations, you could extract the elements to be sorted from the
list, sort them, then stu them back where they were quite eciently like this:
void sort_sublist(std::list<int>& mylist, std::list<int>::const_iterator start,
std::list<int>::const_iterator end) {
    //extract and sort half-open sub range denoted by start and end iterator
    std::list<int> tmp;
    tmp.splice(tmp.begin(), list, start, end);
    tmp.sort();
    //re-insert range at the point we extracted it from
    list.splice(end, tmp);
}
Section 67.2: sorting with std::map (ascending and
descending)
This example sorts elements in ascending order of a key using a map. You can use any type, including class,
instead of std::string, in the example below.
#include <iostream>
#include <utility>
#include <map>
int main()
{
    std::map<double, std::string> sorted_map;
    // Sort the names of the planets according to their size
    sorted_map.insert(std::make_pair(0.3829, "Mercury"));
    sorted_map.insert(std::make_pair(0.9499, "Venus"));
    sorted_map.insert(std::make_pair(1,      "Earth"));
    sorted_map.insert(std::make_pair(0.532,  "Mars"));
    sorted_map.insert(std::make_pair(10.97,  "Jupiter"));
    sorted_map.insert(std::make_pair(9.14,   "Saturn"));
    sorted_map.insert(std::make_pair(3.981,  "Uranus"));
    sorted_map.insert(std::make_pair(3.865,  "Neptune"));
    for (auto const& entry: sorted_map)
    {
        std::cout << entry.second << " (" << entry.first << " of Earth's radius)" << '\n';
    }
}
Output:
Mercury (0.3829 of Earth's radius)
Mars (0.532 of Earth's radius)
Venus (0.9499 of Earth's radius)
Earth (1 of Earth's radius)
Neptune (3.865 of Earth's radius)
Uranus (3.981 of Earth's radius)
Saturn (9.14 of Earth's radius)
Jupiter (10.97 of Earth's radius)
If entries with equal keys are possible, use multimap instead of map (like in the following example).
To sort elements in descending manner, declare the map with a proper comparison functor (std::greater<>):
#include <iostream>
#include <utility>
#include <map>
int main()
{
    std::multimap<int, std::string, std::greater<int>> sorted_map;
    // Sort the names of animals in descending order of the number of legs
    sorted_map.insert(std::make_pair(6,   "bug"));
    sorted_map.insert(std::make_pair(4,   "cat"));
    sorted_map.insert(std::make_pair(100, "centipede"));
    sorted_map.insert(std::make_pair(2,   "chicken"));
    sorted_map.insert(std::make_pair(0,   "fish"));
    sorted_map.insert(std::make_pair(4,   "horse"));
    sorted_map.insert(std::make_pair(8,   "spider"));
    for (auto const& entry: sorted_map)
    {
        std::cout << entry.second << " (has " << entry.first << " legs)" << '\n';
    }
}
Output
centipede (has 100 legs)
spider (has 8 legs)
bug (has 6 legs)
cat (has 4 legs)
horse (has 4 legs)
chicken (has 2 legs)
fish (has 0 legs)
Section 67.3: Sorting sequence containers by overloaded less
operator
If no ordering function is passed, std::sort will order the elements by calling operator< on pairs of elements,
which must return a type contextually convertible to bool (or just bool). Basic types (integers, oats, pointers etc)
have already build in comparison operators.
We can overload this operator to make the default sort call work on user-dened types.
// Include sequence containers
#include <vector>
#include <deque>
#include <list>
// Insert sorting algorithm
#include <algorithm>
class Base {
 public:
    // Constructor that set variable to the value of v
    Base(int v): variable(v) {
    }
    // Use variable to provide total order operator less
    //`this` always represents the left-hand side of the compare.
    bool operator<(const Base &b) const {
        return this->variable < b.variable;
    }
    int variable;
};
int main() {
    std::vector <Base> vector;
    std::deque <Base> deque;
    std::list <Base> list;
    // Create 2 elements to sort
    Base a(10);
    Base b(5);
    // Insert them into backs of containers
    vector.push_back(a);
    vector.push_back(b);
    deque.push_back(a);
    deque.push_back(b);
    list.push_back(a);
    list.push_back(b);
    // Now sort data using operator<(const Base &b) function
    std::sort(vector.begin(), vector.end());
    std::sort(deque.begin(), deque.end());
    // List must be sorted differently due to its design
    list.sort();
    return 0;
}
Section 67.4: Sorting sequence containers using compare
function
// Include sequence containers
#include <vector>
#include <deque>
#include <list>
// Insert sorting algorithm
#include <algorithm>
class Base {
 public:
    // Constructor that set variable to the value of v
    Base(int v): variable(v) {
    }
    int variable;
};
bool compare(const Base &a, const Base &b) {
    return a.variable < b.variable;
}
int main() {
    std::vector <Base> vector;
    std::deque <Base> deque;
    std::list <Base> list;
    // Create 2 elements to sort
    Base a(10);
    Base b(5);
    // Insert them into backs of containers
    vector.push_back(a);
    vector.push_back(b);
    deque.push_back(a);
    deque.push_back(b);
    list.push_back(a);
    list.push_back(b);
    // Now sort data using comparing function
    std::sort(vector.begin(), vector.end(), compare);
    std::sort(deque.begin(), deque.end(), compare);
    list.sort(compare);
    return 0;
}
Section 67.5: Sorting sequence containers using lambda
expressions (C++11)
Version  C++11
// Include sequence containers
#include <vector>
#include <deque>
#include <list>
#include <array>
#include <forward_list>
// Include sorting algorithm
#include <algorithm>
class Base {
 public:
    // Constructor that set variable to the value of v
    Base(int v): variable(v) {
    }
    int variable;
};
int main() {
    // Create 2 elements to sort
    Base a(10);
    Base b(5);
    // We're using C++11, so let's use initializer lists to insert items.
    std::vector <Base> vector = {a, b};
    std::deque <Base> deque = {a, b};
    std::list <Base> list = {a, b};
    std::array <Base, 2> array = {a, b};
    std::forward_list<Base> flist = {a, b};
    // We can sort data using an inline lambda expression
    std::sort(std::begin(vector), std::end(vector),
      [](const Base &a, const Base &b) { return a.variable < b.variable;});
    // We can also pass a lambda object as the comparator
    // and reuse the lambda multiple times
    auto compare = [](const Base &a, const Base &b) {
                     return a.variable < b.variable;};
    std::sort(std::begin(deque), std::end(deque), compare);
    std::sort(std::begin(array), std::end(array), compare);
    list.sort(compare);
    flist.sort(compare);
    return 0;
}
Section 67.6: Sorting built-in arrays
The sort algorithm sorts a sequence dened by two iterators. This is enough to sort a built-in (also known as c-
style) array.
Version  C++11
int arr1[] = {36, 24, 42, 60, 59};
// sort numbers in ascending order
sort(std::begin(arr1), std::end(arr1));
// sort numbers in descending order
sort(std::begin(arr1), std::end(arr1), std::greater<int>());
Prior to C++11, end of array had to be "calculated" using the size of the array:
Version < C++11
// Use a hard-coded number for array size
sort(arr1, arr1 + 5);
// Alternatively, use an expression
const size_t arr1_size = sizeof(arr1) / sizeof(*arr1);
sort(arr1, arr1 + arr1_size);
Section 67.7: Sorting sequence containers with specifed
ordering
If the values in a container have certain operators already overloaded, std::sort can be used with specialized
functors to sort in either ascending or descending order:
Version  C++11
#include <vector>
#include <algorithm>
#include <functional>
std::vector<int> v = {5,1,2,4,3};
//sort in ascending order (1,2,3,4,5)
std::sort(v.begin(), v.end(), std::less<int>());
// Or just:
std::sort(v.begin(), v.end());
//sort in descending order (5,4,3,2,1)
std::sort(v.begin(), v.end(), std::greater<int>());
//Or just:
std::sort(v.rbegin(), v.rend());
Version  C++14
In C++14, we don't need to provide the template argument for the comparison function objects and instead let the
object deduce based on what it gets passed in:
std::sort(v.begin(), v.end(), std::less<>());     // ascending order
std::sort(v.begin(), v.end(), std::greater<>());  // descending order
