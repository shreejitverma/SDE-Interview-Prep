# Chapter 08: Standard Template Library Core

# THE STANDARD TEMPLATE LIBRARY (STL) CORE

<!-- Merged content from Chapter_20_C9803_STANDARD_LIBRARY.md -->

# C++98/03 STANDARD LIBRARY

## Standard Template Library (STL)

***

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

***

## STL Components Overview

```
          STL (Standard Template Library)

                       |
   ***************************************--

   |              |            |           |
CONTAINERS     ITERATORS    ALGORITHMS   FUNCTORS

   |              |            |           |
Sequence       Input        Searching    Predicates
Associative    Output       Sorting      Comparators
Adapters       Forward      Modifying
               Bidir        Numeric
               Random
```

***
### Professional Insights: STL Core Depth

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

***
### Professional Insights: Data Structures Internals

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

***

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

***

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

***

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

***

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

***

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

***

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

***

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

***

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

***

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

***

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

***

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

***

## ALGORITHMS

Found in `<algorithm>` and `<numeric>`. They work on iterator ranges `[first, last)`.

### 1. Non-Modifying

- `find(begin, end, val)`
- `count(begin, end, val)`
- `equal(b1, e1, b2)`
- `search(b1, e1, b2, e2)`

***
### Professional Insights: Algorithm Mastery

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

***

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

***

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

***

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

***

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

***

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

***

## Professional Insights: C++ Containers

C++ containers store a collection of elements. Containers include vectors, lists, maps, etc. Using Templates, C++
containers contain collections of primitives (e.g. ints) or custom classes (e.g. MyClass).
Section 43.1: C++ Containers Flowchart
Choosing which C++ Container to use can be tricky, so here's a simple ﬂowchart to help decide which Container is
right for the job.
This ﬂowchart was based on Mikael Persson's post. This little graphic in the ﬂowchart is from Megan Hopkins

## Professional Insights: std::string

Strings are objects that represent sequences of characters. The standard string class provides a simple, safe and
versatile alternative to using explicit arrays of chars when dealing with text and other sequences of characters. The
C++ string class is part of the std namespace and was standardized in 1998.
Section 47.1: Tokenize
Listed from least expensive to most expensive at run-time:
1.
std::strtok is the cheapest standard provided tokenization method, it also allows the delimiter to be
modiﬁed between tokens, but it incurs 3 diﬃculties with modern C++:
std::strtok cannot be used on multiple strings at the same time (though some implementations do
extend to support this, such as: strtok_s)
For the same reason std::strtok cannot be used on multiple threads simultaneously (this may
however be implementation deﬁned, for example: Visual Studio's implementation is thread safe)
Calling std::strtok modiﬁes the std::string it is operating on, so it cannot be used on const
strings, const char*s, or literal strings, to tokenize any of these with std::strtok or to operate on a
std::string who's contents need to be preserved, the input would have to be copied, then the copy
could be operated on
Generally any of these options cost will be hidden in the allocation cost of the tokens, but if the cheapest
algorithm is required and std::strtok's diﬃculties are not overcomable consider a hand-spun solution.
// String to tokenize
```cpp
std::string str{ "The quick brown fox" };
// Vector to store tokens
vector<std::string> tokens;
for (auto i = strtok(&str[0], " "); i != NULL; i = strtok(NULL, " "))
    tokens.push_back(i);
Live Example
2.
The std::istream_iterator uses the stream's extraction operator iteratively. If the input std::string is
white-space delimited this is able to expand on the std::strtok option by eliminating its diﬃculties, allowing
inline tokenization thereby supporting the generation of a const vector<string>, and by adding support for
multiple delimiting white-space character:
// String to tokenize
const std::string str("The  quick \\tbrown \\nfox");
std::istringstream is(str);
// Vector to store tokens
const std::vector<std::string> tokens = std::vector<std::string>(
                                        std::istream_iterator<std::string>(is),
                                        std::istream_iterator<std::string>());
Live Example
3.
The std::regex_token_iterator uses a std::regex to iteratively tokenize. It provides for a more ﬂexible
delimiter deﬁnition. For example, non-delimited commas and white-space:

Version ≥ C++11
// String to tokenize
const std::string str{ "The ,qu\\\\,ick ,\\tbrown, fox" };
const std::regex re{ "\\\\s*((?:[^\\\\\\\\,]|\\\\\\\\.)*?)\\\\s*(?:,|$)" };
// Vector to store tokens
const std::vector<std::string> tokens{
    std::sregex_token_iterator(str.begin(), str.end(), re, 1),
    std::sregex_token_iterator()
};
Live Example
```

See the regex_token_iterator Example for more details.
Section 47.2: Conversion to (const) char*
In order to get const char* access to the data of a std::string you can use the string's c_str() member function.
Keep in mind that the pointer is only valid as long as the std::string object is within scope and remains
unchanged, that means that only const methods may be called on the object.
Version ≥ C++17
The data() member function can be used to obtain a modiﬁable char*, which can be used to manipulate the
std::string object's data.
Version ≥ C++11
A modiﬁable char* can also be obtained by taking the address of the ﬁrst character: &s[0]. Within C++11, this is
guaranteed to yield a well-formed, null-terminated string. Note that &s[0] is well-formed even if s is empty,
whereas &s.front() is undeﬁned if s is empty.
Version ≥ C++11
```cpp
std::string str("This is a string.");
const char* cstr = str.c_str(); // cstr points to: "This is a string.\\0"
const char* data = str.data();  // data points to: "This is a string.\\0"
std::string str("This is a string.");
// Copy the contents of str to untie lifetime from the std::string object
std::unique_ptr<char []> cstr = std::make_unique<char[]>(str.size() + 1);
// Alternative to the line above (no exception safety):
// char* cstr_unsafe = new char[str.size() + 1];
std::copy(str.data(), str.data() + str.size(), cstr);
cstr[str.size()] = '\\0'; // A null-terminator needs to be added
// delete[] cstr_unsafe;
std::cout << cstr.get();
Section 47.3: Using the std::string_view class
Version ≥ C++17
C++17 introduces std::string_view, which is simply a non-owning range of const chars, implementable as either
a pair of pointers or a pointer and a length. It is a superior parameter type for functions that requires non-
modiﬁable string data. Before C++17, there were three options for this:
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
It oﬀers a useful subset of the functionality that std::string does, although some of the functions behave
diﬀerently:
std::string str = "lllloooonnnngggg sssstttrrriiinnnggg"; //A really long string
//Bad way - 'string::substr' returns a new string (expensive if the string is long)
std::cout << str.substr(15, 10) << '\\n';
//Good way - No copies are created!
std::string_view view = str;
// string_view::substr returns a new string_view
std::cout << view.substr(15, 10) << '\\n';
Section 47.4: Conversion to std::wstring
In C++, sequences of characters are represented by specializing the std::basic_string class with a native
character type. The two major collections deﬁned by the standard library are std::string and std::wstring:
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
In order to improve usability and/or readability, you can deﬁne functions to perform the conversion:
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
```

World!").
Please note that char and wchar_t do not imply encoding, and gives no indication of size in bytes. For instance,
wchar_t is commonly implemented as a 2-bytes data type and typically contains UTF-16 encoded data under
Windows (or UCS-2 in versions prior to Windows 2000) and as a 4-bytes data type encoded using UTF-32 under
Linux. This is in contrast with the newer types char16_t and char32_t, which were introduced in C++11 and are
guaranteed to be large enough to hold any UTF16 or UTF32 "character" (or more precisely, code point) respectively.
Section 47.5: Lexicographical comparison
Two std::strings can be compared lexicographically using the operators ==, !=, <, <=, >, and >=:
```cpp
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
```

Finds the ﬁrst diﬀerent character pair, compares them then returns the boolean result.
operator<= or operator>=:
Finds the ﬁrst diﬀerent character pair, compares them then returns the boolean result.
Note: The term character pair means the corresponding characters in both strings of the same positions. For
better understanding, if two example strings are str1 and str2, and their lengths are n and m respectively, then
character pairs of both strings means each str1[i] and str2[i] pairs where i = 0, 1, 2, ..., max(n,m). If for any i
where the corresponding character does not exist, that is, when i is greater than or equal to n or m, it would be
considered as the lowest value.
Here is an example of using <:
```cpp
std::string str1 = "Barr";
std::string str2 = "Bar";
assert(str2 < str1);
The steps are as follows:
1.
2.
3.
4.
```

Compare the ﬁrst characters, 'B' == 'B' - move on.
Compare the second characters, 'a' == 'a' - move on.
Compare the third characters, 'r' == 'r' - move on.
The str2 range is now exhausted, while the str1 range still has characters. Thus, str2 < str1.
Section 47.6: Trimming characters at start/end
This example requires the headers <algorithm>, <locale>, and <utility>.
Version ≥ C++11
To trim a sequence or string means to remove all leading and trailing elements (or characters) matching a certain
predicate. We ﬁrst trim the trailing elements, because it doesn't involve moving any elements, and then trim the
leading elements. Note that the generalizations below work for all types of std::basic_string (e.g. std::string
and std::wstring), and accidentally also for sequence containers (e.g. std::vector and std::list).
```cpp
template <typename Sequence, // any basic_string, vector, list etc.
          typename Pred>     // a predicate on the element (character) type
Sequence& trim(Sequence& seq, Pred pred) {
    return trim_start(trim_end(seq, pred), pred);
}

Trimming the trailing elements involves ﬁnding the last element not matching the predicate, and erasing from there
on:
template <typename Sequence, typename Pred>
Sequence& trim_end(Sequence& seq, Pred pred) {
    auto last = std::find_if_not(seq.rbegin(),
                                 seq.rend(),
                                 pred);
    seq.erase(last.base(), seq.end());
    return seq;
}
Trimming the leading elements involves ﬁnding the ﬁrst element not matching the predicate and erasing up to
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
Version ≥ C++14
//4)
str.replace(19, 5, alternate, 6); //"Hello foo, bar and foobar!"
//5)
str.replace(str.begin(), str.begin() + 5, str.begin() + 6, str.begin() + 9);
//"foo foo, bar and world!"
//6)
str.replace(0, 5, 3, 'z'); //"zzz foo, bar and world!"
//7)
str.replace(str.begin() + 6, str.begin() + 9, 3, 'x'); //"Hello xxx, bar and world!"
Version ≥ C++11
//8)
str.replace(str.begin(), str.begin() + 5, { 'x', 'y', 'z' }); //"xyz foo, bar and world!"
Replace occurrences of a string with another string
Replace only the ﬁrst occurrence of replace with with in str:
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
User-deﬁned classes may implement the stream insertion operator if desired:
std::ostream operator<<( std::ostream& out, const A& a )
{
    // write a string representation of a to out
    return out;
}
Version ≥ C++11
Aside from streams, since C++11 you can also use the std::to_string (and std::to_wstring) function which is
overloaded for all fundamental types and returns the string representation of its parameter.
std::string s = to_string(0x12f3);  // after this the string s contains "4851"
Section 47.9: Splitting
Use std::string::substr to split a string. There are two variants of this member function.
The ﬁrst takes a starting position from which the returned substring should begin. The starting position must be
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
There are several ways to extract characters from a std::string and each is subtly diﬀerent.
std::string str("Hello world!");
operator[](n)
```

Returns a reference to the character at index n.
```cpp
std::string::operator[] is not bounds-checked and does not throw an exception. The caller is responsible for
asserting that the index is within the range of the string:
char c = str[6]; // 'w'
at(n)
```

Returns a reference to the character at index n.
```cpp
std::string::at is bounds checked, and will throw std::out_of_range if the index is not within the range of the
string:
char c = str.at(7); // 'o'
Version ≥ C++11
```

Note: Both of these examples will result in undeﬁned behavior if the string is empty.
front()
Returns a reference to the ﬁrst character:
char c = str.front(); // 'H'
back()
Returns a reference to the last character:
char c = str.back(); // '!'
Section 47.11: Checking if a string is a preﬁx of another
Version ≥ C++14
In C++14, this is easily done by std::mismatch which returns the ﬁrst mismatching pair from two ranges:
```cpp
std::string prefix = "foo";

std::string string = "foobar";
bool isPrefix = std::mismatch(prefix.begin(), prefix.end(),
    string.begin(), string.end()).first == prefix.end();
Note that a range-and-a-half version of mismatch() existed prior to C++14, but this is unsafe in the case that the
second string is the shorter of the two.
Version < C++14
We can still use the range-and-a-half version of std::mismatch(), but we need to ﬁrst check that the ﬁrst string is at
most as big as the second:
bool isPrefix = prefix.size() <= string.size() &&
    std::mismatch(prefix.begin(), prefix.end(),
        string.begin(), string.end()).first == prefix.end();
Version ≥ C++17
With std::string_view, we can write the direct comparison we want without having to worry about allocation
overhead or making copies:
bool isPrefix(std::string_view prefix, std::string_view full)
{
    return prefix == full.substr(0, prefix.size());
}
Section 47.12: Looping through each character
Version ≥ C++11
std::string supports iterators, and so you can use a ranged based loop to iterate through each character:
std::string str = "Hello World!";
for (auto c : str)
    std::cout << c;
You can use a "traditional" for loop to loop through every character:
std::string str = "Hello World!";
for (std::size_t i = 0; i < str.length(); ++i)
    std::cout << str[i];
Section 47.13: Conversion to integers/ﬂoating point types
A std::string containing a number can be converted into an integer type, or a ﬂoating point type, using
conversion functions.
Note that all of these functions stop parsing the input string as soon as they encounter a non-numeric character, so
"123abc" will be converted into 123.
The std::ato* family of functions converts C-style strings (character arrays) to integer or ﬂoating-point types:
std::string ten = "10";
double num1 = std::atof(ten.c_str());

int num2 = std::atoi(ten.c_str());
long num3 = std::atol(ten.c_str());
Version ≥ C++11
long long num4 = std::atoll(ten.c_str());
However, use of these functions is discouraged because they return 0 if they fail to parse the string. This is bad
because 0 could also be a valid result, if for example the input string was "0", so it is impossible to determine if the
conversion actually failed.
The newer std::sto* family of functions convert std::strings to integer or ﬂoating-point types, and throw
exceptions if they could not parse their input. You should use these functions if possible:
Version ≥ C++11
std::string ten = "10";
int num1 = std::stoi(ten);
long num2 = std::stol(ten);
long long num3 = std::stoll(ten);
float num4 = std::stof(ten);
double num5 = std::stod(ten);
long double num6 = std::stold(ten);
Furthermore, these functions also handle octal and hex strings unlike the std::ato* family. The second parameter
is a pointer to the ﬁrst unconverted character in the input string (not illustrated here), and the third parameter is
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
requires to use a diﬀerent template for wstring_convert when dealing with char16_t:
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
To ﬁnd a character or another string, you can use std::string::find. It returns the position of the ﬁrst character
of the ﬁrst match. If no matches were found, the function returns std::string::npos
std::string str = "Curiosity killed the cat";
auto it = str.find("cat");
if (it != std::string::npos)
    std::cout << "Found at position: " << it << '\\n';
else
    std::cout << "Not found!\\n";
Found at position: 21
The search opportunities are further expanded by the following functions:
find_first_of     // Find first occurrence of characters
find_first_not_of // Find first absence of characters
find_last_of      // Find last occurrence of characters
find_last_not_of  // Find last absence of characters
These functions can allow you to search for characters from the end of the string, as well as ﬁnd the negative case
(ie. characters that are not in the string). Here is an example:
std::string str = "dog dog cat cat";
std::cout << "Found at position: " << str.find_last_of("gzx") << '\\n';
Found at position: 6
Note: Be aware that the above functions do not search for substrings, but rather for characters contained in the
search string. In this case, the last occurrence of 'g' was found at position 6 (the other characters weren't found).
```


## Professional Insights: std::vector

A vector is a dynamic array with automatically handled storage. The elements in a vector can be accessed just as
eﬃciently as those in an array with the advantage being that vectors can dynamically change in size.
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
that it can be read as well as modiﬁed (if the vector is not const).
[] and at() diﬀer in that [] is not guaranteed to perform any bounds checking, while at() does. Accessing
elements where index < 0 or index >= size is undeﬁned behavior for [], while at() throws a std::out_of_range
exception.
Note: The examples below use C++11-style initialization for clarity, but the operators can be used with all versions
(unless marked C++11).
Version ≥ C++11
```cpp
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
to elements of vectors are done in constant time. That means accessing to the ﬁrst element of the vector has the
same cost (in time) of accessing the second element, the third element and so on.
For example, consider this loop
for (std::size_t i = 0; i < v.size(); ++i) {
    v[i] = 1;
}
Here we know that the index variable i is always in bounds, so it would be a waste of CPU cycles to check that i is
in bounds for every call to operator[].

The front() and back() member functions allow easy reference access to the ﬁrst and last element of the vector,
respectively. These positions are frequently used, and the special accessors can be more readable than their
alternatives using []:
std::vector<int> v{ 4, 5, 6 }; // In pre-C++11 this is more verbose
int a = v.front();   // a is 4, v.front() is equivalent to v[0]
v.front() = 3;       // v now contains {3, 5, 6}
int b = v.back();    // b is 6, v.back() is equivalent to v[v.size() - 1]
v.back() = 7;        // v now contains {3, 5, 7}
Note: It is undeﬁned behavior to invoke front() or back() on an empty vector. You need to check that the
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
  std::cout << "total: " << sum << '\\n';//output the total to the user
  return 0;
}
The example above creates a vector with a sequence of numbers from 1 to 10. Then it pops the elements of the
vector out until the vector is empty (using 'empty()') to prevent undeﬁned behavior. Then the sum of the numbers
in the vector is calculated and displayed to the user.
Version ≥ C++11
The data() method returns a pointer to the raw memory used by the std::vector to internally store its elements.
```

This is most often used when passing the vector data to legacy code that expects a C-style array.
```cpp
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
Version ≥ C++11
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
to elements in the vector remain stable and access remains deﬁned unless you add/remove elements at or before
the element in the vector, or you cause the vector capacity to change. This is the same as the rule for invalidating
iterators.
Version ≥ C++11
std::vector<int> v{ 1, 2, 3 };
int* p = v.data() + 1;     // p points to 2
v.insert(v.begin(), 0);    // p is now invalid, accessing *p is a undefined behavior.
p = v.data() + 1;          // p points to 1
v.reserve(10);             // p is now invalid, accessing *p is a undefined behavior.
p = v.data() + 1;          // p points to 1
v.erase(v.begin());        // p is now invalid, accessing *p is a undefined behavior.
Section 49.2: Initializing a std::vector
A std::vector can be initialized in several ways while declaring it:
Version ≥ C++11
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
Version ≥ C++11
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
Version ≥ C++11
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
to be copied or moved to ﬁll the gap, see the note below and std::list.
Deleting all elements in a range:
std::vector<int> v{ 1, 2, 3, 4, 5, 6 };
v.erase(v.begin() + 1, v.begin() + 5);  // v becomes {1, 6}
```

Note: The above methods do not change the capacity of the vector, only the size. See Vector Size and Capacity.
The erase method, which removes a range of elements, is often used as a part of the erase-remove idiom. That is,
ﬁrst std::remove moves some elements to the end of the vector, and then erase chops them oﬀ. This is a relatively
ineﬃcient operation for any indices less than the last index of the vector because all elements after the erased
segments must be relocated to new positions. For speed critical applications that require eﬃcient removal of
arbitrary elements in a container, see std::list.
Deleting elements by value:
```cpp
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
Version ≥ C++11
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
While it is important not to increment it in case of a deletion, you should consider using a diﬀerent method when
then erasing repeatedly in a loop. Consider remove_if for a more eﬃcient way.
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
Version ≥ C++11
shrink_to_fit() frees up unused vector capacity:
v.shrink_to_fit();
```

The shrink_to_fit does not guarantee to really reclaim space, but most current implementations do.
Section 49.4: Iterating Over std::vector
You can iterate over a std::vector in several ways. For each of the following sections, v is deﬁned as follows:
```cpp
std::vector<int> v;
Iterating in the Forward Direction
Version ≥ C++11
// Range based for
for(const auto& value: v) {
    std::cout << value << "\\n";
}
// Using a for loop with iterator
for(auto it = std::begin(v); it != std::end(v); ++it) {
    std::cout << *it << "\\n";
}
// Using for_each algorithm, using a function or functor:
void fun(int const& value) {
    std::cout << value << "\\n";
}
std::for_each(std::begin(v), std::end(v), fun);

// Using for_each algorithm. Using a lambda:
std::for_each(std::begin(v), std::end(v), [](int const& value) {
    std::cout << value << "\\n";
});
Version < C++11
// Using a for loop with iterator
for(std::vector<int>::iterator it = std::begin(v); it != std::end(v); ++it) {
    std::cout << *it << "\\n";
}
// Using a for loop with index
for(std::size_t i = 0; i < v.size(); ++i) {
    std::cout << v[i] << "\\n";
}
Iterating in the Reverse Direction
Version ≥ C++14
// There is no standard way to use range based for for this.
// See below for alternatives.
// Using for_each algorithm
// Note: Using a lambda for clarity. But a function or functor will work
std::for_each(std::rbegin(v), std::rend(v), [](auto const& value) {
    std::cout << value << "\\n";
});
// Using a for loop with iterator
for(auto rit = std::rbegin(v); rit != std::rend(v); ++rit) {
    std::cout << *rit << "\\n";
}
// Using a for loop with index
for(std::size_t i = 0; i < v.size(); ++i) {
    std::cout << v[v.size() - 1 - i] << "\\n";
}
Though there is no built-in way to use the range based for to reverse iterate; it is relatively simple to ﬁx this. The
range based for uses begin() and end() to get iterators and thus simulating this with a wrapper object can achieve
the results we require.
Version ≥ C++14
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
        std::cout << value << "\\n";
    }
}
Enforcing const elements

Since C++11 the cbegin() and cend() methods allow you to obtain a constant iterator for a vector, even if the vector
is non-const. A constant iterator allows you to read but not modify the contents of the vector which is useful to
enforce const correctness:
Version ≥ C++11
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
Version ≥ C++17
as_const extends this to range iteration:
for (auto const& e : std::as_const(v)) {
  std::cout << e << '\\n';
}
This is easy to implement in earlier versions of C++:
Version ≥ C++14
template <class T>
constexpr std::add_const_t<T>& as_const(T& t) noexcept {
  return t;
}
A Note on Eﬃciency
Since the class std::vector is basically a class that manages a dynamically allocated contiguous array, the same
principle explained here applies to C++ vectors. Accessing the vector's content by index is much more eﬃcient
when following the row-major order principle. Of course, each access to the vector also puts its management
content into the cache as well, but as has been debated many times (notably here and here), the diﬀerence in
performance for iterating over a std::vector compared to a raw array is negligible. So the same principle of
eﬃciency for raw arrays in C also applies for C++'s std::vector.
Section 49.5: vector<bool>: The Exception To So Many, So
Many Rules
The standard (section 23.3.7) speciﬁes that a specialization of vector<bool> is provided, which optimizes space by
packing the bool values, so that each takes up only one bit. Since bits aren't addressable in C++, this means that
several requirements on vector are not placed on vector<bool>:
The data stored is not required to be contiguous, so a vector<bool> can't be passed to a C API which expects
a bool array.
at(), operator [], and dereferencing of iterators do not return a reference to bool. Rather they return a

proxy object that (imperfectly) simulates a reference to a bool by overloading its assignment operators. As an
example, the following code may not be valid for std::vector<bool>, because dereferencing an iterator
does not return a reference:
Version ≥ C++11
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
Version ≥ C++11
std::vector<char> trad_vect = {true, false, false, false, true, false, true, true};
Bitwise representation:
[0,0,0,0,0,0,0,1], [0,0,0,0,0,0,0,0], [0,0,0,0,0,0,0,0], [0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,1], [0,0,0,0,0,0,0,0], [0,0,0,0,0,0,0,1], [0,0,0,0,0,0,0,1]
Specialized std::vector<bool> storing 8 Boolean values:
Version ≥ C++11
std::vector<bool> optimized_vect = {true, false, false, false, true, false, true, true};
Bitwise representation:
[1,0,0,0,1,0,1,1]
Notice in the above example, that in the traditional version of std::vector<bool>, 8 Boolean values take up 8 bytes
of memory, whereas in the optimized version of std::vector<bool>, they only use 1 byte of memory. This is a
signiﬁcant improvement on memory usage. If you need to pass a vector<bool> to an C-style API, you may need to
copy the values to an array, or ﬁnd a better way to use the API, if memory and performance are at risk.
Section 49.6: Inserting Elements
Appending an element at the end of a vector (by copying/moving):
struct Point {
  double x, y;

  Point(double x, double y) : x(x), y(y) {}
};
std::vector<Point> v;
Point p(10.0, 2.0);
v.push_back(p);  // p is copied into the vector.
Version ≥ C++11
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
Version ≥ C++11
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

Version ≥ C++11
std::vector<int> v{ 1, 2, 3 };
int* p = v.data();
In contrast to solutions based on previous C++ standards (see below), the member function .data() may also be
applied to empty vectors, because it doesn't cause undeﬁned behavior in this case.
Before C++11, you would take the address of the vector's ﬁrst element to get an equivalent pointer, if the vector
isn't empty, these both methods are interchangeable:
int* p = &v[0];      // combine subscript operator and 0 literal
int* p = &v.front(); // explicitly reference the first element
```

Note: If the vector is empty, v[0] and v.front() are undeﬁned and cannot be used.
When storing the base address of the vector's data, note that many operations (such as push_back, resize, etc.) can
change the data memory location of the vector, thus invalidating previous data pointers. For example:
```cpp
std::vector<int> v;
int* p = v.data();
v.resize(42);      // internal memory location changed; value of p is now invalid
Section 49.8: Finding an Element in std::vector
The function std::find, deﬁned in the <algorithm> header, can be used to ﬁnd an element in a std::vector.
std::find uses the operator== to compare elements for equality. It returns an iterator to the ﬁrst element in the
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
Version ≥ C++11
std::vector<int> v { 5, 4, 3, 2, 1 };
auto it = std::find(v.begin(), v.end(), 4);
auto index = std::distance(v.begin(), it);
// `it` points to the second element of the vector, `index` is 1
auto missing = std::find(v.begin(), v.end(), 10);
auto index_missing = std::distance(v.begin(), missing);
// `missing` is v.end(), `index_missing` is 5 (ie. size of the vector)
If you need to perform many searches in a large vector, then you may want to consider sorting the vector ﬁrst,

before using the binary_search algorithm.
To ﬁnd the ﬁrst element in a vector that satisﬁes a condition, std::find_if can be used. In addition to the two
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
Version ≥ C++11
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
However, this solution fails if you try to append a vector to itself, because the standard speciﬁes that iterators given
to insert() must not be from the same range as the receiver object's elements.
Version ≥ c++11
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
```

Vectors can be used as a 2D matrix by deﬁning them as a vector of vectors.
A matrix with 3 rows and 4 columns with each cell initialised as 0 can be deﬁned as:
```cpp
std::vector<std::vector<int> > matrix(3, std::vector<int>(4));
Version ≥ C++11
```

The syntax for initializing them using initialiser lists or otherwise are similar to that of a normal vector.
```cpp
  std::vector<std::vector<int>> matrix = { {0,1,2,3},
                                           {4,5,6,7},
                                           {8,9,10,11}
                                         };
Values in such a vector can be accessed similar to a 2D array
int var = matrix[0][2];
```

Iterating over the entire matrix is similar to that of a normal vector but with an extra dimension.
for(int i = 0; i < 3; ++i)
{
    for(int j = 0; j < 4; ++j)
    {
```cpp
        std::cout << matrix[i][j] << std::endl;
    }
}
Version ≥ C++11
for(auto& row: matrix)
{
    for(auto& col : row)
    {
        std::cout << col << std::endl;
    }
}
A vector of vectors is a convenient way to represent a matrix but it's not the most eﬃcient: individual vectors are
scattered around memory and the data structure isn't cache friendly.
Also, in a proper matrix, the length of every row must be the same (this isn't the case for a vector of vectors). The
additional ﬂexibility can be a source of errors.

Section 49.11: Using a Sorted Vector for Fast Element Lookup
```

The <algorithm> header provides a number of useful functions for working with sorted vectors.
An important prerequisite for working with sorted vectors is that the stored values are comparable with <.
An unsorted vector can be sorted by using the function std::sort():
```cpp
std::vector<int> v;
// add some code here to fill v with some elements
std::sort(v.begin(), v.end());
Sorted vectors allow eﬃcient element lookup using the function std::lower_bound(). Unlike std::find(), this
performs an eﬃcient binary search on the vector. The downside is that it only gives valid results for sorted input
ranges:
// search the vector for the first element with value 42
std::vector<int>::iterator it = std::lower_bound(v.begin(), v.end(), 42);
if (it != v.end() && *it == 42) {
    // we found the element!
}
Note: If the requested value is not part of the vector, std::lower_bound() will return an iterator to the ﬁrst element
that is greater than the requested value. This behavior allows us to insert a new element at its right place in an
already sorted vector:
int const new_element = 33;
v.insert(std::lower_bound(v.begin(), v.end(), new_element), new_element);
If you need to insert a lot of elements at once, it might be more eﬃcient to call push_back() for all them ﬁrst and
then call std::sort() once all elements have been inserted. In this case, the increased cost of the sorting can pay
oﬀ against the reduced cost of inserting new elements at the end of the vector and not in the middle.
If your vector contains multiple elements of the same value, std::lower_bound() will try to return an iterator to the
ﬁrst element of the searched value. However, if you need to insert a new element after the last element of the
searched value, you should use the function std::upper_bound() as this will cause less shifting around of
elements:
v.insert(std::upper_bound(v.begin(), v.end(), new_element), new_element);
If you need both the upper bound and the lower bound iterators, you can use the function std::equal_range() to
retrieve both of them eﬃciently with one call:
std::pair<std::vector<int>::iterator,
          std::vector<int>::iterator> rg = std::equal_range(v.begin(), v.end(), 42);
std::vector<int>::iterator lower_bound = rg.first;
std::vector<int>::iterator upper_bound = rg.second;
In order to test whether an element exists in a sorted vector (although not speciﬁc to vectors), you can use the
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
vector was signiﬁcant, then the capacity reduction for the new vector is likely to be signiﬁcant. We can then swap
the original vector with the temporary one to retain its minimized capacity:
std::vector<int>(v).swap(v);
Version ≥ C++11
In C++11 we can use the shrink_to_fit() member function for a similar eﬀect:
v.shrink_to_fit();
```

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
Vector has an implementation-speciﬁc upper limit on its size, but you are likely to run out of RAM before
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
Vector capacity diﬀers from size. While size is simply how many elements the vector currently has, capacity is for
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

## Professional Insights: std::map

To use any of std::map or std::multimap the header ﬁle <map> should be included.
std::map and std::multimap both keep their elements sorted according to the ascending order of keys. In
case of std::multimap, no sorting occurs for the values of the same key.
The basic diﬀerence between std::map and std::multimap is that the std::map one does not allow duplicate
values for the same key where std::multimap does.
Maps are implemented as binary search trees. So search(), insert(), erase() takes Θ(log n) time in
average. For constant time operation use std::unordered_map.
size() and empty() functions have Θ(1) time complexity, number of nodes is cached to avoid walking
through tree each time these functions are called.
Section 50.1: Accessing elements
An std::map takes (key, value) pairs as input.
Consider the following example of std::map initialization:
```cpp
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
Version ≥ C++11
Elements of a std::map can be accessed with at():
std::cout << ranking.at("stackoverflow") << std::endl;
Note that at() will throw an std::out_of_range exception if the container does not contain the requested
element.
In both containers std::map and std::multimap, elements can be accessed using iterators:
Version ≥ C++11
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
points to the element causing the conﬂict, and the bool is value is false.
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
Version ≥ C++11
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
To get the iterator of the ﬁrst occurrence of a key, the find() function can be used. It returns end() if the key
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
Another way to ﬁnd whether an entry exists in std::map or in std::multimap is using the count() function,
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
multimaps, it can stop once the ﬁrst matching element has been found.
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
```


Both could be initialized with iterator.
// From std::map or std::multimap iterator
std::multimap< int , int > mmp{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {6, 8}, {3, 4},
                               {6, 7} };
                       // {1, 2}, {3, 4}, {3, 4}, {6, 5}, {6, 8}, {6, 7}, {8, 9}
```cpp
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
```

A map is an associative container, containing key-value pairs.
```cpp
#include <string>
#include <map>

std::map<std::string, size_t> fruits_count;
In the above example, std::string is the key type, and size_t is a value.
```

The key acts as an index in the map. Each key must be unique, and must be ordered.
If you need mutliple elements with the same key, consider using multimap (explained below)
If your value type does not specify any ordering, or you want to override the default ordering, you may
provide one:
```cpp
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
diﬀer.
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
Section 50.9: Creating std::map with user-deﬁned types as
key
In order to be able to use a class as the key in a map, all that is required of the key is that it be copiable and
assignable. The ordering within the map is deﬁned by the third argument to the template (and the argument to
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
```

This is a mathematical term to deﬁne a relationship between two objects.
Its deﬁnition is:
Two objects x and y are equivalent if both f(x, y) and f(y, x) are false. Note that an object is always (by the
irreﬂexivity invariant) equivalent to itself.
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
How you deﬁne equivalent/less is totally dependent on the type of your object.

## Professional Insights: Standard Library Algorithms

Section 62.1: std::next_permutation
```cpp
template< class Iterator >
bool next_permutation( Iterator first, Iterator last );
template< class Iterator, class Compare >
bool next_permutation( Iterator first, Iterator last, Compare cmpFun );
Eﬀects:
Sift the data sequence of the range [ﬁrst, last) into the next lexicographically higher permutation. If cmpFun is
provided, the permutation rule is customized.
Parameters:
first- the beginning of the range to be permutated, inclusive
last - the end of the range to be permutated, exclusive
Return Value:
```

Returns true if such permutation exists.
Otherwise the range is swaped to the lexicographically smallest permutation and return false.
Complexity:
O(n), n is the distance from first to last.
Example:
```cpp
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
Eﬀects:
Applies f to the result of dereferencing every iterator in the range [first, last) starting from first and

proceeding to last - 1.
Parameters:
first, last - the range to apply f to.
f - callable object which is applied to the result of dereferencing every iterator in the range [first, last).
Return value:
f (until C++11) and std::move(f) (since C++11).
Complexity:
```

Applies f exactly last - first times.
Example:
Version ≥ c++11
```cpp
std::vector<int> v { 1, 2, 4, 8, 16 };
std::for_each(v.begin(), v.end(), [](int elem) { std::cout << elem << " "; });
```

Applies the given function for every element of the vector v printing this element to stdout.
Section 62.3: std::accumulate
Deﬁned in header <numeric>
```cpp
template<class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init); // (1)
template<class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation f); // (2)
Eﬀects:
std::accumulate performs fold operation using f function on range [first, last) starting with init as
accumulator value.
Eﬀectively it's equivalent of:
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
```


Accumulated value of f applications.
Complexity:
O(n×k), where n is the distance from first to last, O(k) is complexity of f function.
Example:
Simple sum example:
```cpp
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
Version ≥ c++11
const std::vector<int> ds = {1, 2, 3};
int n = std::accumulate(ds.begin(), ds.end(),
                        0,
                        [](int a, int d) { return a * 10 + d; });
std::cout << n << std::endl;
Output:
Section 62.4: std::ﬁnd
template <class InputIterator, class T>
InputIterator find (InputIterator first, InputIterator last, const T& val);
Eﬀects
Finds the ﬁrst occurrence of val within the range [ﬁrst, last)
Parameters
first => iterator pointing to the beginning of the range last => iterator pointing to the end of the range val => The
value to ﬁnd within the range

Return
An iterator that points to the ﬁrst element within the range that is equal(==) to val, the iterator points to last if val is
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
Eﬀects
Finds the minimum element in a range
Parameters
first - iterator pointing to the beginning of the range
last - iterator pointing to the end of the range comp - a function pointer or function object that takes two
arguments and returns true or false indicating whether argument is less than argument 2. This function should not
modify inputs
Return
Iterator to the minimum element in the range
Complexity
```

Linear in one less than the number of elements compared.
Example
```cpp
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
Section 62.6: std::ﬁnd_if
template <class InputIterator, class UnaryPredicate>
InputIterator find_if (InputIterator first, InputIterator last, UnaryPredicate pred);
Eﬀects
```

Finds the ﬁrst element in a range for which the predicate function pred returns true.
Parameters
first => iterator pointing to the beginning of the range last => iterator pointing to the end of the range pred =>
predicate function(returns true or false)
Return
An iterator that points to the ﬁrst element within the range the predicate function pred returns true for. The
iterator points to last if val is not found
Example
```cpp
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
```

Note This function is very eﬃcient - it has linear complexity.

For the sake of this example, let's deﬁne the median of a sequence of length n as the element that would be in
position ⌈n / 2⌉. For example, the median of a sequence of length 5 is the 3rd smallest element, and so is the
median of a sequence of length 6.
To use this function to ﬁnd the median, we can use the following. Say we start with
```cpp
std::vector<int> v{5, 1, 2, 3, 4};
std::vector<int>::iterator b = v.begin();
std::vector<int>::iterator e = v.end();
std::vector<int>::iterator med = b;
std::advance(med, v.size() / 2);
// This makes the 2nd position hold the median.
std::nth_element(b, med, e);
// The median is now at v[2].
To ﬁnd the pth quantile, we would change some of the lines above:
const std::size_t pos = p * std::distance(b, e);
std::advance(nth, pos);
and look for the quantile at position pos.
Section 62.8: std::count
template <class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type
count (InputIterator first, InputIterator last, const T& val);
Eﬀects
Counts the number of elements that are equal to val
Parameters
first => iterator pointing to the beginning of the range
last => iterator pointing to the end of the range
val => The occurrence of this value in the range will be counted
Return
```

The number of elements in the range that are equal(==) to val.
Example
```cpp
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
Eﬀects
Counts the number of elements in a range for which a speciﬁed predicate function is true
Parameters
first => iterator pointing to the beginning of the range last => iterator pointing to the end of the range red =>
predicate function(returns true or false)
Return
```

The number of elements within the speciﬁed range for which the predicate function returned true.
Example
```cpp
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
```


## Professional Insights: Sorting

Section 67.1: Sorting and sequence containers
std::sort, found in the standard library header algorithm, is a standard library algorithm for sorting a range of
values, deﬁned by a pair of iterators. std::sort takes as the last parameter a functor used to compare two values;
this is how it determines the order. Note that std::sort is not stable.
The comparison function must impose a Strict, Weak Ordering on the elements. A simple less-than (or greater-than)
comparison will suﬃce.
A container with random-access iterators can be sorted using the std::sort algorithm:
Version ≥ C++11
```cpp
#include <vector>
#include <algorithm>

std::vector<int> MyVector = {3, 1, 2}
//Default comparison of <
std::sort(MyVector.begin(), MyVector.end());
std::sort requires that its iterators are random access iterators. The sequence containers std::list and
std::forward_list (requiring C++11) do not provide random access iterators, so they cannot be used with
std::sort. However, they do have sort member functions which implement a sorting algorithm that works with
their own iterator types.
Version ≥ C++11
#include <list>
#include <algorithm>

std::list<int> MyList = {3, 1, 2}
//Default comparison of <
//Whole list only.
MyList.sort();
Their member sort functions always sort the entire list, so they cannot sort a sub-range of elements. However,
since list and forward_list have fast splicing operations, you could extract the elements to be sorted from the
list, sort them, then stuﬀ them back where they were quite eﬃciently like this:
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
        std::cout << entry.second << " (" << entry.first << " of Earth's radius)" << '\\n';
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
```

If entries with equal keys are possible, use multimap instead of map (like in the following example).
To sort elements in descending manner, declare the map with a proper comparison functor (std::greater<>):
```cpp
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
        std::cout << entry.second << " (has " << entry.first << " legs)" << '\\n';

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
which must return a type contextually convertible to bool (or just bool). Basic types (integers, ﬂoats, pointers etc)
have already build in comparison operators.
```

We can overload this operator to make the default sort call work on user-deﬁned types.
// Include sequence containers
```cpp
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
Version ≥ C++11
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
The sort algorithm sorts a sequence deﬁned by two iterators. This is enough to sort a built-in (also known as c-
style) array.
Version ≥ C++11
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
Version ≥ C++11
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
Version ≥ C++14
In C++14, we don't need to provide the template argument for the comparison function objects and instead let the


```

#### Comparison & Assignment

```cpp
vector<int> v1 = {1, 2, 3};
vector<int> v2 = {1, 2, 3};
vector<int> v3 = {1, 2, 4};

cout << (v1 == v2) << "\n";    // 1 (true)
cout << (v1 != v3) << "\n";    // 1 (true)
cout << (v1 < v3) << "\n";     // 1 (true) - lexicographic

// Assignment
v1 = v3;                       // Copy v3 to v1
v1.swap(v2);                   // Swap v1 and v2

// Swap single elements
swap(v1[0], v1[1]);
```



#### Deque vs Vector

- **Deque**: Better for push_front/pop_front operations
- **Vector**: Better for push_back/pop_back and random access

***



#### List-Specific Operations

```cpp
list<int> lst1 = {1, 2, 3};
list<int> lst2 = {4, 5, 6};

// Splice - move elements from one list to another
lst1.splice(lst1.end(), lst2);  // Append lst2 to lst1
// lst1: {1, 2, 3, 4, 5, 6}
// lst2: {} (empty)

// Merge - combine two sorted lists
list<int> a = {1, 3, 5};
list<int> b = {2, 4, 6};
a.merge(b);                      // a: {1, 2, 3, 4, 5, 6}, b: {}

// Remove if
list<int> nums = {1, 2, 3, 4, 5};
nums.remove_if([](int x) { return x % 2 == 0; }); // Remove even numbers
// nums: {1, 3, 5}
```

***



#### Map Operations

```cpp
map<int, string> dict;

// Insert
dict[1] = "one";
dict[2] = "two";
dict[3] = "three";

// Erase
dict.erase(2);                 // Erase by key
dict.erase(dict.begin());      // Erase by iterator

// Find
auto it = dict.find(1);
if (it != dict.end()) {
    cout << it->second << "\n";
}

// Lower and upper bound
map<int, string> scores = {{1, "A"}, {2, "B"}, {3, "C"}};

// find first >= key
auto it1 = scores.lower_bound(2);  // Points to {2, "B"}

// Find first > key
auto it2 = scores.upper_bound(2);  // Points to {3, "C"}

// Range [lower_bound, upper_bound)
auto range = scores.equal_range(2);  // Pair of iterators
for (auto it = range.first; it != range.second; ++it) {
    cout << it->second << "\n";
}

// Clear
dict.clear();
```






#### Map with Custom Comparator

```cpp
// Descending order
map<int, string, greater<int>> descending;
descending[3] = "three";
descending[1] = "one";
descending[2] = "two";
// Iteration order: 3, 2, 1

// Custom comparator
struct Compare {
    bool operator()(const string& a, const string& b) const {
        return a.length() < b.length();  // Sort by string length
    }
};

map<string, int, Compare> byLength;
byLength["a"] = 1;
byLength["abc"] = 3;
byLength["ab"] = 2;
```


***




### 1.6 MULTIMAP - Key-Value Pairs with Duplicate Keys

```cpp
#include <map>

multimap<string, int> students;

// Multiple values for same key
students.insert({"Math", 95});
students.insert({"Math", 87});
students.insert({"English", 92});
students.insert({"English", 88});

// Find all with key "Math"
auto range = students.equal_range("Math");
for (auto it = range.first; it != range.second; ++it) {
    cout << it->first << ": " << it->second << "\n";
}
// Output: Math: 95, Math: 87

// Count how many with key "Math"
cout << students.count("Math") << "\n";  // 2

// Iterate all
for (const auto& [subject, score] : students) {
    cout << subject << ": " << score << "\n";
}
```


***



#### What is Unordered Map?

Like map but no sorting, O(1) average operations.

```cpp
#include <unordered_map>

// Declaration
unordered_map<string, int> ages;

// Insertion - same as map
ages["Alice"] = 30;
ages["Bob"] = 25;

// Accessing - same as map
cout << ages["Alice"] << "\n";

// Find
if (ages.find("Bob") != ages.end()) {
    cout << "Found Bob\n";
}

// Erase
ages.erase("Alice");

// Iteration - ORDER IS ARBITRARY
for (const auto& [name, age] : ages) {
    cout << name << ": " << age << "\n";
}

// Size
cout << ages.size() << "\n";

// Bucket information
cout << ages.bucket_count() << "\n";      // Number of buckets
cout << ages.load_factor() << "\n";       // Load factor
cout << ages.max_load_factor() << "\n";   // Max load factor

// Rehash
ages.rehash(100);                         // Rehash with hint 100
ages.reserve(50);                         // Reserve space for 50 elements

// Clear
ages.clear();
```




#### What is Queue?

Adapter container. Elements added at back, removed from front.

```cpp
#include <queue>

queue<int> q;

// Adding (enqueue)
q.push(10);
q.push(20);
q.push(30);

// Size and check empty
cout << q.size() << "\n";
cout << q.empty() << "\n";

// Access front and back
cout << q.front() << "\n";  // 10 (first element)
cout << q.back() << "\n";   // 30 (last element)

// Removing (dequeue)
q.pop();  // Removes 10

// Typical queue pattern
while (!q.empty()) {
    cout << q.front() << " ";
    q.pop();
}
```




#### Queue Example - Task Processing

```cpp
queue<string> taskQueue;

taskQueue.push("Task 1");
taskQueue.push("Task 2");
taskQueue.push("Task 3");

while (!taskQueue.empty()) {
    cout << "Processing: " << taskQueue.front() << "\n";
    taskQueue.pop();
}
// Output: Task 1, Task 2, Task 3
```

***



#### What is Stack?

Adapter container. Elements added and removed from top.

```cpp
#include <stack>

stack<int> st;

// Adding (push)
st.push(10);
st.push(20);
st.push(30);

// Size and check empty
cout << st.size() << "\n";
cout << st.empty() << "\n";

// Access top
cout << st.top() << "\n";  // 30 (last added)

// Removing (pop)
st.pop();  // Removes 30

// Typical stack pattern
while (!st.empty()) {
    cout << st.top() << " ";
    st.pop();
}
// Output: 20 10
```




#### What is Priority Queue?

Elements removed in order of priority (max/min).

```cpp
#include <queue>

// Max heap (largest element has highest priority)
priority_queue<int> pq;

pq.push(10);
pq.push(30);
pq.push(20);

while (!pq.empty()) {
    cout << pq.top() << " ";  // Largest first
    pq.pop();
}
// Output: 30 20 10

// Min heap (smallest element has highest priority)
priority_queue<int, vector<int>, greater<int>> minPQ;

minPQ.push(10);
minPQ.push(30);
minPQ.push(20);

while (!minPQ.empty()) {
    cout << minPQ.top() << " ";  // Smallest first
    minPQ.pop();
}
// Output: 10 20 30
```




#### Priority Queue with Custom Comparator

```cpp
struct Task {
    string name;
    int priority;
    
    // For priority_queue to work
    bool operator<(const Task& other) const {
        return priority < other.priority;  // Max heap on priority
    }
};

priority_queue<Task> tasks;

tasks.push({"Task A", 5});
tasks.push({"Task B", 10});
tasks.push({"Task C", 3});

while (!tasks.empty()) {
    cout << tasks.top().name << " (P: " << tasks.top().priority << ")\n";
    tasks.pop();
}
// Output: Task B (P: 10), Task A (P: 5), Task C (P: 3)
```


***



### Iterator Categories

```
                    ┌─────────────────────┐
                    │  Iterator           │
                    ├─────────────────────┤
                    │ • Single pass       │
                    │ • Basic operations  │
                    └──────────┬──────────┘
                               │
        ┌──────────────────────┼──────────────────────┐
        │                      │                      │
    ┌───▼────┐          ┌─────▼──────┐        ┌──────▼───┐
    │ Input  │          │   Output   │        │ Forward  │
    ├────────┤          ├────────────┤        ├──────────┤
    │ Read   │          │ Write      │        │ Read+Write
    │ ++, *  │          │ ++, =      │        │ All ops  │
    └────────┘          └────────────┘        └──────┬───┘
                                                     │
                        ┌────────────────────────────┘
                        │
                    ┌───▼─────────────┐
                    │  Bidirectional  │
                    ├─────────────────┤
                    │ ++, --, *, =    │
                    └────────┬────────┘
                             │
                    ┌────────▼────────┐
                    │ Random Access   │
                    ├─────────────────┤
                    │ All ops + []    │
                    └─────────────────┘
```



#### Iterator Operations

```cpp
vector<int> v = {10, 20, 30, 40, 50};

auto it = v.begin();

// Navigation
++it;                    // Move forward
--it;                    // Move backward (if bidirectional)
it++;                    // Post-increment
it--;                    // Post-decrement

// Dereference
cout << *it << "\n";     // Get value

// Arithmetic (random access only)
it = it + 3;             // Move 3 positions forward
it = it - 2;             // Move 2 positions backward
it += 5;
it -= 3;

// Comparison
if (it == v.end()) {}    // Check equality
if (it != v.end()) {}    // Check inequality
if (it < v.end()) {}     // Less than (random access)

// Distance
int dist = distance(v.begin(), it);

// Advance
advance(it, 5);          // Move 5 positions forward
```






#### Const Iterators

```cpp
const vector<int> v = {10, 20, 30};

// Const iterator
auto it = v.begin();     // const_iterator
cout << *it << "\n";     // OK - read
// *it = 100;            // Error - can't modify

// Explicit const iterator
const_iterator cit = v.cbegin();

// Reverse const iterator
auto rit = v.crbegin();

// Using const_iterator with non-const vector
vector<int> v2 = {1, 2, 3};
const_iterator it2 = v2.cbegin();
```


const_iterator it2 = v2.cbegin();

#### Advanced Iterator Concepts

##### 1. Iterator Traits (std::iterator_traits)
Algorithms use `iterator_traits` to know what an iterator can do.

```cpp
#include <iterator>

template<typename Iter>
void my_advance(Iter& it, int n) {
    using category = typename std::iterator_traits<Iter>::iterator_category;
    
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, category>) {
        it += n; // O(1)
    } else {
        while (n--) ++it; // O(N)
    }
}
```

##### 2. Writing a Custom Iterator
To make a class compatible with STL algorithms (like `std::find`), you need a conformant iterator.

```cpp
class Integers {
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = int;
        using pointer           = int*;
        using reference         = int&;

        int value;
        Iterator(int v) : value(v) {}

        reference operator*() { return value; }
        pointer operator->() { return &value; }
        
        Iterator& operator++() { value++; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
        
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.value == b.value; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.value != b.value; };
    };

public:
    Iterator begin() { return Iterator(0); }
    Iterator end()   { return Iterator(10); } // Range [0, 10)
};
```

##### 3. Stream Iterators
Treat IO streams as containers.

```cpp
#include <iterator>
#include <algorithm>

// Read ints from cin until EOF or invalid input
std::istream_iterator<int> input_it(std::cin);
std::istream_iterator<int> eos;

// Write ints to cout with ", " delimiter
std::ostream_iterator<int> output_it(std::cout, ", ");

std::copy(input_it, eos, output_it);
```

##### 4. Insert Iterators
Special output iterators that grow the container.

*   `std::back_inserter(c)`: Calls `c.push_back(val)`. (Vector, List, Deque)
*   `std::front_inserter(c)`: Calls `c.push_front(val)`. (List, Deque)
*   `std::inserter(c, it)`: Calls `c.insert(it, val)`. (Map, Set, List, Vector)

```cpp
std::vector<int> v;
std::fill_n(std::back_inserter(v), 5, 42); // v becomes {42, 42, 42, 42, 42}
```

***

## ALGORITHMS - COMPLETE REFERENCE
## C++ STL Advanced - Extended Reference & Algorithms Library

### COMPREHENSIVE STL ALGORITHMS REFERENCE

#### All Algorithms by Category (60+ Algorithms)

***

### NON-MODIFYING SEQUENCE ALGORITHMS

#### 1. find Family
```cpp
#include <algorithm>

auto it = find(first, last, value);              // Find element
auto it = find_if(first, last, predicate);       // Find matching condition
auto it = find_if_not(first, last, predicate);   // Find non-matching (C++11)
```

#### 2. count Family
```cpp
int n = count(first, last, value);               // Count occurrences
int n = count_if(first, last, predicate);        // Count matching
```

#### 3. mismatch
```cpp
auto [it1, it2] = mismatch(first1, last1, first2);  // Find first difference
auto [it1, it2] = mismatch(first1, last1, first2, comp); // With comparator
```

#### 4. equal
```cpp
bool eq = equal(first1, last1, first2);          // Compare ranges
bool eq = equal(first1, last1, first2, comp);    // With comparator
```

#### 5. search & adjacent_find
```cpp
auto it = search(first, last, s_first, s_last);  // Find subsequence
auto it = search_n(first, last, count, value);   // Find N equal elements
auto it = adjacent_find(first, last);            // Find adjacent equal elements
auto it = adjacent_find(first, last, comp);      // With comparator
```

#### 6. Logical Operations
```cpp
bool b = all_of(first, last, predicate);         // All match
bool b = any_of(first, last, predicate);         // Any matches
bool b = none_of(first, last, predicate);        // None match
```

#### 7. Min/Max
```cpp
auto it = min_element(first, last);              // Find minimum
auto it = max_element(first, last);              // Find maximum
auto [minIt, maxIt] = minmax_element(first, last);  // Both (C++11)

auto it = min_element(first, last, comp);        // With comparator
auto it = max_element(first, last, comp);
auto [minIt, maxIt] = minmax_element(first, last, comp);
```

***

### MODIFYING SEQUENCE ALGORITHMS

#### 1. copy Family
```cpp
copy(first, last, d_first);                      // Copy range
copy_n(first, count, d_first);                   // Copy N elements
copy_if(first, last, d_first, predicate);        // Conditional copy
copy_backward(first, last, d_last);              // Copy backwards
```

#### 2. move (C++11)
```cpp
move(first, last, d_first);                      // Move range
move_backward(first, last, d_last);              // Move backwards
```

#### 3. transform
```cpp
transform(first, last, d_first, op);             // Apply function
transform(first1, last1, first2, d_first, op);   // Apply to two ranges
```

#### 4. fill & generate
```cpp
fill(first, last, value);                        // Fill with value
fill_n(first, count, value);                     // Fill N elements
generate(first, last, gen);                      // Generate values
generate_n(first, count, gen);                   // Generate N values
```

#### 5. replace
```cpp
replace(first, last, old_value, new_value);      // Replace values
replace_if(first, last, predicate, new_value);   // Conditional replace
replace_copy(first, last, d_first, old, new);    // Copy with replace
replace_copy_if(first, last, d_first, pred, new);// Conditional copy-replace
```

#### 6. swap & reverse
```cpp
swap(a, b);                                       // Swap two values
iter_swap(it1, it2);                             // Swap via iterators
reverse(first, last);                            // Reverse range
reverse_copy(first, last, d_first);              // Copy reversed
```

#### 7. rotate
```cpp
rotate(first, middle, last);                     // Rotate range
rotate_copy(first, middle, last, d_first);       // Copy rotated
```

#### 8. unique
```cpp
auto it = unique(first, last);                   // Remove consecutive duplicates
auto it = unique(first, last, comp);             // With comparator
auto it = unique_copy(first, last, d_first);     // Copy unique
auto it = unique_copy(first, last, d_first, comp);
```

#### 9. shuffle & random
```cpp
shuffle(first, last, rng);                       // Random shuffle
random_shuffle(first, last);                     // Legacy shuffle
random_shuffle(first, last, randFunc);           // With random function
```

#### 10. remove
```cpp
auto it = remove(first, last, value);            // Remove all matching
auto it = remove_if(first, last, predicate);     // Conditional remove
auto it = remove_copy(first, last, d_first, val);// Copy without matching
auto it = remove_copy_if(first, last, d_first, pred);
```

***

### SORTING & PARTITIONING ALGORITHMS

#### Sorting
```cpp
sort(first, last);                               // Sort (introsort)
sort(first, last, comp);                         // With comparator
stable_sort(first, last);                        // Stable sort
stable_sort(first, last, comp);

partial_sort(first, middle, last);               // Sort first part
partial_sort(first, middle, last, comp);
partial_sort_copy(first, last, d_first, d_last);// Copy partially sorted

nth_element(first, nth, last);                   // Sort around nth
nth_element(first, nth, last, comp);
```

#### Partitioning
```cpp
auto it = partition(first, last, predicate);     // Partition
auto it = stable_partition(first, last, pred);   // Stable partition
auto it = partition_copy(first, last, d_true, d_false, pred);  // Copy partitions

bool b = is_partitioned(first, last, predicate); // Check if partitioned
auto it = partition_point(first, last, predicate); // Find partition point
```

#### Binary Search (requires sorted range)
```cpp
auto it = lower_bound(first, last, value);       // First >= value
auto it = upper_bound(first, last, value);       // First > value
auto [lo, hi] = equal_range(first, last, value);  // Range of value
bool b = binary_search(first, last, value);      // Check existence

auto it = lower_bound(first, last, value, comp);
auto it = upper_bound(first, last, value, comp);
auto [lo, hi] = equal_range(first, last, value, comp);
bool b = binary_search(first, last, value, comp);
```

***

### NUMERIC ALGORITHMS

```cpp
#include <numeric>

// Accumulation
int sum = accumulate(first, last, init);          // Sum
auto prod = accumulate(first, last, init, op);    // Custom operation

// Inner product (dot product)
int dot = inner_product(first1, last1, first2, init);
auto result = inner_product(first1, last1, first2, init, op1, op2);

// Partial sums
partial_sum(first, last, d_first);                // Cumulative sum
partial_sum(first, last, d_first, op);            // Custom operation

// Adjacent differences
adjacent_difference(first, last, d_first);        // Differences
adjacent_difference(first, last, d_first, op);    // Custom operation
```

***

### SET OPERATIONS (require sorted ranges)

```cpp
// Union - all unique elements
auto it = set_union(first1, last1, first2, last2, d_first);
auto it = set_union(first1, last1, first2, last2, d_first, comp);

// Intersection - common elements
auto it = set_intersection(first1, last1, first2, last2, d_first);
auto it = set_intersection(first1, last1, first2, last2, d_first, comp);

// Difference - in first but not in second
auto it = set_difference(first1, last1, first2, last2, d_first);
auto it = set_difference(first1, last1, first2, last2, d_first, comp);

// Symmetric difference - in one but not both
auto it = set_symmetric_difference(first1, last1, first2, last2, d_first);
auto it = set_symmetric_difference(first1, last1, first2, last2, d_first, comp);

// Check relationship
bool b = includes(first1, last1, first2, last2);   // first1 includes first2
bool b = includes(first1, last1, first2, last2, comp);
```

***

### HEAP OPERATIONS

```cpp
#include <algorithm>

make_heap(first, last);                          // Create heap
make_heap(first, last, comp);                    // With comparator

push_heap(first, last);                          // Add element to heap
pop_heap(first, last);                           // Extract max from heap
sort_heap(first, last);                          // Sort heap

bool b = is_heap(first, last);                   // Check if valid heap
bool b = is_heap(first, last, comp);
auto it = is_heap_until(first, last);            // Find where heap property breaks
```

***

### PERMUTATION ALGORITHMS

```cpp
bool b = next_permutation(first, last);          // Next lexicographic permutation
bool b = next_permutation(first, last, comp);
bool b = prev_permutation(first, last);          // Previous permutation
bool b = prev_permutation(first, last, comp);

bool b = is_permutation(first1, last1, first2);  // Check if permutation
bool b = is_permutation(first1, last1, first2, comp);
```

***

### COMPLETE STL ALGORITHMS QUICK REFERENCE

| Algorithm | Purpose | Returns |
|-----------|---------|---------|
| find | Find element | Iterator |
| find_if | Find matching | Iterator |
| count | Count occurrences | Count |
| equal | Compare ranges | Bool |
| search | Find subsequence | Iterator |
| sort | Sort range | Void |
| binary_search | Check existence (sorted) | Bool |
| lower_bound | First >= value (sorted) | Iterator |
| partition | Divide by condition | Iterator |
| copy | Copy range | Iterator |
| transform | Apply function | Iterator |
| remove | Remove matching | Iterator |
| unique | Remove duplicates | Iterator |
| reverse | Reverse range | Void |
| rotate | Rotate range | Void |
| min_element | Find minimum | Iterator |
| max_element | Find maximum | Iterator |
| accumulate | Sum/aggregate | Value |
| inner_product | Dot product | Value |
| set_union | Union of sets | Iterator |
| set_intersection | Intersection | Iterator |

***

### CONTAINERS DETAILED OPERATIONS

#### Vector Operations
```cpp
v.push_back(val);               // Add to end
v.pop_back();                   // Remove from end
v.insert(pos, val);             // Insert at position
v.erase(pos);                   // Erase at position
v.clear();                      // Remove all
v.resize(n);                    // Change size
v.reserve(n);                   // Pre-allocate
v.shrink_to_fit();              // Release excess memory
v.swap(other);                  // Swap two vectors

// Access
v[i];                           // O(1) random access
v.at(i);                        // O(1) with bounds check
v.front();                      // First element
v.back();                       // Last element
v.data();                       // Raw pointer (C++11)

// Iteration
begin(v), end(v);               // Iterators
rbegin(v), rend(v);             // Reverse iterators
cbegin(v), cend(v);             // Const iterators (C++11)

// Properties
v.size();                       // Number of elements
v.capacity();                   // Allocated space
v.empty();                      // Check if empty
v.max_size();                   // Maximum possible size
```

#### Map Operations
```cpp
m[key] = value;                 // Insert/update
m.insert({key, value});         // Insert
m.erase(key);                   // Erase by key
m.clear();                      // Remove all
m.swap(other);                  // Swap two maps

// Access
m[key];                         // Access (creates if missing)
m.at(key);                      // Access (throws if missing)
m.find(key);                    // Find key
m.count(key);                   // Check existence

// Range operations
m.lower_bound(key);             // First >= key
m.upper_bound(key);             // First > key
m.equal_range(key);             // All equal keys

// Iteration
m.begin(), m.end();             // Forward
m.rbegin(), m.rend();           // Reverse

// Properties
m.size();                       // Number of elements
m.empty();                      // Check if empty
```

#### Set Operations
```cpp
s.insert(val);                  // Insert element
s.erase(val);                   // Erase by value
s.clear();                      // Remove all
s.swap(other);                  // Swap

// Access
s.find(val);                    // Find element
s.count(val);                   // Check existence (1 or 0)
s.lower_bound(val);             // First >= value
s.upper_bound(val);             // First > value
s.equal_range(val);             // All equal values

// Iteration
s.begin(), s.end();             // Forward
s.rbegin(), s.rend();           // Reverse

// Properties
s.size();
s.empty();
```

***

### ALGORITHM COMPLEXITY CHEAT SHEET

```
find, find_if, find_if_not:        O(n)
count, count_if:                   O(n)
search, search_n:                  O(n*m)
all_of, any_of, none_of:           O(n)

sort:                              O(n log n) avg
stable_sort:                       O(n log n)
partial_sort:                      O(n log k) k=distance(first,last)
nth_element:                       O(n) avg
make_heap:                         O(n)
push_heap:                         O(log n)
pop_heap:                          O(log n)

copy:                              O(n)
transform:                         O(n)
fill:                              O(n)
reverse:                           O(n)
rotate:                            O(n)
unique:                            O(n)
remove:                            O(n)
partition:                         O(n)
binary_search:                     O(log n)
lower_bound:                       O(log n)
upper_bound:                       O(log n)

accumulate:                        O(n)
inner_product:                     O(n)
partial_sum:                       O(n)

set_union:                         O(n+m)
set_intersection:                  O(n+m)
set_difference:                    O(n+m)
```

***

### CONTAINER COMPLEXITY COMPARISON

```
                  Insert  Delete  Search  Random Access  Memory
vector            O(n)    O(n)    O(n)    O(1)           Contiguous
deque             O(n)    O(n)    O(n)    O(1)           Chunks
list              O(1)    O(1)    O(n)    O(n)           Scattered
map               O(log n) O(log n) O(log n) -           Tree
set               O(log n) O(log n) O(log n) -           Tree
multimap          O(log n) O(log n) O(log n) -           Tree
multiset          O(log n) O(log n) O(log n) -           Tree
unordered_map     O(1)    O(1)    O(1)    -              Hash
unordered_set     O(1)    O(1)    O(1)    -              Hash
queue             O(1)    O(1)    O(n)    -              - (adapter)
stack             O(1)    O(1)    O(n)    -              - (adapter)
priority_queue    O(log n) O(log n) O(n)    -              Heap
```

***

### ITERATOR COMPARISON TABLE

```
Container        Iterator Type          Bidirectional  Random Access
vector           random access           Yes            Yes
deque            random access           Yes            Yes
list             bidirectional           Yes            No
map              bidirectional           Yes            No
set              bidirectional           Yes            No
multimap         bidirectional           Yes            No
multiset         bidirectional           Yes            No
unordered_map    forward                 No             No
unordered_set    forward                 No             No
```

***

### PRACTICAL STL PATTERNS

#### Pattern 1: Find & Remove
```cpp
vector<int> v = {1, 2, 3, 2, 4, 2};
auto it = find(v.begin(), v.end(), 2);
if (it != v.end()) {
    v.erase(it);  // Remove first occurrence
}
// v: {1, 3, 2, 4, 2}

// Remove all
v.erase(remove(v.begin(), v.end(), 2), v.end());
// v: {1, 3, 4}
```

#### Pattern 2: Filter & Copy
```cpp
vector<int> v = {1, 2, 3, 4, 5};
vector<int> even;

copy_if(v.begin(), v.end(), back_inserter(even),
    [](int x) { return x % 2 == 0; });
// even: {2, 4}
```

#### Pattern 3: Transform & Collect
```cpp
vector<int> v = {1, 2, 3};
vector<int> squared;

transform(v.begin(), v.end(), back_inserter(squared),
    [](int x) { return x * x; });
// squared: {1, 4, 9}
```

#### Pattern 4: Partition & Process
```cpp
vector<int> v = {1, 2, 3, 4, 5, 6};

auto it = partition(v.begin(), v.end(),
    [](int x) { return x % 2 == 0; });

// Process even numbers
for (auto i = v.begin(); i != it; ++i) {
    cout << *i << " ";  // 2 4 6
}
```

#### Pattern 5: Sort & Deduplicate
```cpp
vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

sort(v.begin(), v.end());
auto it = unique(v.begin(), v.end());
v.erase(it, v.end());
// v: {1, 2, 3, 4, 5, 6, 9}
```

#### Pattern 6: Group & Count
```cpp
map<string, int> frequency;

vector<string> words = {"apple", "banana", "apple", "cherry", "banana"};
for (const string& word : words) {
    frequency[word]++;
}

for (const auto& [word, count] : frequency) {
    cout << word << ": " << count << "\n";
}
// Output: apple: 2, banana: 2, cherry: 1
```

#### Pattern 7: Custom Sorting
```cpp
struct Person {
    string name;
    int age;
};

vector<Person> people = {
    {"Alice", 30}, {"Bob", 25}, {"Carol", 30}
};

// Sort by age, then by name
sort(people.begin(), people.end(),
    [](const Person& a, const Person& b) {
        if (a.age != b.age) return a.age < b.age;
        return a.name < b.name;
    });
```

#### Pattern 8: Merge Ranges
```cpp
vector<int> v1 = {1, 3, 5};
vector<int> v2 = {2, 4, 6};
vector<int> merged;

merge(v1.begin(), v1.end(), v2.begin(), v2.end(),
    back_inserter(merged));
// merged: {1, 2, 3, 4, 5, 6}
```

***

### STL WITH LAMBDAS (C++11 and later)

```cpp
#include <algorithm>
#include <vector>

vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Simple lambda
auto isEven = [](int x) { return x % 2 == 0; };

// With capture by value
int threshold = 5;
auto gt = [threshold](int x) { return x > threshold; };

// With capture by reference
int sum = 0;
for_each(v.begin(), v.end(),
    [&sum](int x) { sum += x; });

// Mutable lambda
auto counter = [count = 0]() mutable { return ++count; };

// Generic lambda (C++14)
auto print = [](auto x) { cout << x << " "; };
for_each(v.begin(), v.end(), print);

// Find even numbers
auto it = find_if(v.begin(), v.end(),
    [](int x) { return x % 2 == 0; });

// Count odd numbers
int oddCount = count_if(v.begin(), v.end(),
    [](int x) { return x % 2 != 0; });

// Transform to squares
vector<int> squared;
transform(v.begin(), v.end(), back_inserter(squared),
    [](int x) { return x * x; });

// Filter and collect
vector<int> filtered;
copy_if(v.begin(), v.end(), back_inserter(filtered),
    [](int x) { return x % 2 == 0; });
```

***

## STRINGS - MASTER GUIDE

### 4.1 String Basics

```cpp
#include <string>
using namespace std;

// Declaration
string s1;                     // Empty string
string s2 = "Hello";           // Initialize with C-string
string s3("World");            // Constructor
string s4(5, 'a');             // 5 'a' characters: "aaaaa"
string s5 = s2;                // Copy
string s6(s2, 1, 3);           // Substring of s2 from pos 1, length 3: "ell"
string s7(s2.begin(), s2.end()); // From iterators

// C-string
const char* cstr = s2.c_str(); // Convert to C-string
const char* data = s2.data();  // Get data pointer
```

#### Size and Capacity

```cpp
string s = "Hello";

cout << s.length() << "\n";   // 5
cout << s.size() << "\n";     // 5 (same as length)
cout << s.capacity() << "\n"; // >= 5

cout << s.empty() << "\n";    // 0 (false)
cout << s.max_size() << "\n"; // Maximum possible size

// Resize
s.resize(10, '*');            // Resize to 10, fill with '*'
// s: "Hello*****"

// Reserve
s.reserve(100);               // Reserve space for 100 characters

// Clear
s.clear();                    // Empty the string
```

***

### 4.2 Accessing Characters

```cpp
string s = "Hello";

// Using operator[]
cout << s[0] << "\n";        // 'H' - No bounds checking
cout << s.at(0) << "\n";     // 'H' - With bounds checking

// Front and back
cout << s.front() << "\n";   // 'H'
cout << s.back() << "\n";    // 'o'

// Modifying characters
s[0] = 'J';                  // "Jello"
s.at(1) = 'A';               // "JAllo"
s.front() = 'B';             // "BAllo"
s.back() = 'z';              // "BAllz"
```

***

### 4.3 String Concatenation

```cpp
string s1 = "Hello";
string s2 = "World";

// Using + operator
string s3 = s1 + " " + s2;   // "Hello World"

// Using += operator
s1 += " ";
s1 += s2;                    // s1: "Hello World"

// Using append()
s1.append(" C++");           // "Hello World C++"
s1.append(3, '!');           // "Hello World C+++!!"

// Using push_back()
s1.push_back('*');           // "Hello World C+++!!*"

// Using insert()
s1.insert(5, "_");           // Insert "_" at position 5
// s1: "Hello_ World C+++!!*"
```

***

### 4.4 String Searching

```cpp
string s = "Hello World Hello";

// find() - find substring
size_t pos = s.find("World");
if (pos != string::npos) {
    cout << "Found at position: " << pos << "\n";  // 6
}

// find() with starting position
pos = s.find("Hello", 2);    // Find "Hello" starting from position 2
// Returns 12 (second "Hello")

// rfind() - find from right
pos = s.rfind("Hello");      // Position of last "Hello"
// Returns 12

// find_first_of() - find first occurrence of any character in string
pos = s.find_first_of("aeiou");
// Returns position of first vowel: 1 ('e')

// find_last_of() - find last occurrence of any character
pos = s.find_last_of("aeiou");
// Returns position of last vowel: 14 ('o')

// find_first_not_of() - find first character NOT in string
pos = s.find_first_not_of("He");
// Returns 2 ('l')

// find_last_not_of() - find last character NOT in string
pos = s.find_last_not_of("o");
// Returns 15

// Check if string starts with (C++20)
// bool starts = s.starts_with("Hello");
// bool ends = s.ends_with("ello");
```

***

### 4.5 String Manipulation

```cpp
string s = "Hello World";

// Substring
string sub = s.substr(0, 5);  // "Hello"
string sub2 = s.substr(6);    // "World"

// Replace
s.replace(0, 5, "Hi");        // Replace first 5 chars with "Hi"
// s: "Hi World"

// Erase
s.erase(2, 1);                // Erase 1 char starting at position 2
// s: "HiWorld"

// remove() + erase() idiom (for removing specific characters)
string s2 = "H-e-l-l-o";
s2.erase(remove(s2.begin(), s2.end(), '-'), s2.end());
// s2: "Hello"

// Compare
string a = "apple";
string b = "apple";
cout << (a == b) << "\n";     // 1 (true)
cout << a.compare(b) << "\n"; // 0 (equal)

// Case conversion (manual)
for (char& c : s) {
    c = toupper(c);  // Convert to uppercase
}
// s: "HI WORLD"

// Transform with algorithm
transform(s.begin(), s.end(), s.begin(), ::toupper);
```

***

### 4.6 String Iteration

```cpp
string s = "Hello";

// Iterator
for (auto it = s.begin(); it != s.end(); ++it) {
    cout << *it << " ";
}

// Range-based for (C++11)
for (char c : s) {
    cout << c << " ";
}

// Index-based
for (int i = 0; i < s.length(); i++) {
    cout << s[i] << " ";
}

// Reverse iteration
for (auto it = s.rbegin(); it != s.rend(); ++it) {
    cout << *it << " ";
}
```

***

### 4.7 String Conversion

```cpp
#include <string>

// String to numbers
string num = "123";
int intVal = stoi(num);           // 123
long longVal = stol(num);         // 123L
float floatVal = stof("3.14");    // 3.14
double doubleVal = stod("3.14159"); // 3.14159

// Number to string
int x = 42;
string s1 = to_string(x);         // "42"
string s2 = to_string(3.14);      // "3.140000"
string s3 = to_string(true);      // "1"

// With radix (base)
string hex = to_string(255);      // "255" (decimal)
// For hex, use stringstream or manual conversion
```

***

### 4.8 String Comparison

```cpp
string s1 = "apple";
string s2 = "apple";
string s3 = "banana";

// Equality
cout << (s1 == s2) << "\n";      // 1 (true)
cout << (s1 != s3) << "\n";      // 1 (true)

// Ordering (lexicographic)
cout << (s1 < s3) << "\n";       // 1 (true) - "apple" < "banana"
cout << (s1 > s3) << "\n";       // 0 (false)

// compare() method
cout << s1.compare(s2) << "\n";  // 0 (equal)
cout << s1.compare(s3) << "\n";  // -1 (s1 < s3)
cout << s3.compare(s1) << "\n";  // 1 (s3 > s1)

// Compare substring
cout << s1.compare(0, 3, "app") << "\n";  // 0 (equal)

// Case-insensitive comparison (manual)
bool caseInsensitive = true;
for (int i = 0; i < s1.length() && i < s3.length(); i++) {
    if (tolower(s1[i]) != tolower(s3[i])) {
        caseInsensitive = false;
        break;
    }
}
```

***

### 4.9 String from Stringstream

```cpp
#include <sstream>

// Building string
ostringstream oss;
oss << "Value: " << 42 << ", Name: " << "Alice";
string result = oss.str();  // "Value: 42, Name: Alice"

// Parsing string
istringstream iss("10 20 30");
int a, b, c;
iss >> a >> b >> c;  // a=10, b=20, c=30

// Convert various types
int num = 42;
double pi = 3.14159;
string name = "Alice";

ostringstream convert;
convert << num << " " << pi << " " << name;
string combined = convert.str();

// Parse line with specific delimiter
istringstream lineStream("apple,banana,mango");
string fruit;
while (getline(lineStream, fruit, ',')) {
    cout << fruit << "\n";
}
```

***

## FILE I/O - COMPLETE COVERAGE

### 5.1 File I/O Basics

```cpp
#include <fstream>
#include <iostream>
using namespace std;

// Output file stream (write)
ofstream outFile;
outFile.open("output.txt");

if (outFile.is_open()) {
    outFile << "Hello, File!\n";
    outFile << "This is a test.\n";
    outFile.close();
} else {
    cout << "Error opening file\n";
}

// Input file stream (read)
ifstream inFile;
inFile.open("output.txt");

if (inFile.is_open()) {
    string line;
    while (getline(inFile, line)) {
        cout << line << "\n";
    }
    inFile.close();
} else {
    cout << "Error opening file\n";
}
```

### 5.2 File Operations

#### Open Modes

```cpp
#include <fstream>

// Write (truncate if exists)
ofstream file1("data.txt");  // Default
ofstream file2("data.txt", ios::out);  // Explicit

// Append
ofstream file3("data.txt", ios::app);

// Read
ifstream file4("data.txt");
ifstream file5("data.txt", ios::in);

// Read and Write
fstream file6("data.txt", ios::in | ios::out);

// Binary mode
ofstream binFile("data.bin", ios::binary);
ifstream binRead("data.bin", ios::binary);

// Truncate (discards existing content)
ofstream file7("data.txt", ios::trunc);

// Seek position
fstream file8("data.txt", ios::in | ios::out);
file8.seekg(10);  // Seek to position 10 for reading
file8.seekp(10);  // Seek to position 10 for writing
```

***

### 5.3 Writing to Files

```cpp
ofstream outFile("output.txt");

if (outFile) {
    // Write strings
    outFile << "Hello, World!\n";
    outFile << "Line 2\n";
    
    // Write numbers
    outFile << 42 << " " << 3.14 << "\n";
    
    // Write characters
    outFile << 'A' << 'B' << 'C' << "\n";
    
    // Write using put() for single character
    outFile.put('X');
    
    // Write raw data
    string data = "Raw data";
    outFile.write(data.c_str(), data.length());
    
    outFile.close();
}
```

***

### 5.4 Reading from Files

#### Line by Line

```cpp
#include <fstream>
#include <string>

ifstream inFile("input.txt");

if (inFile) {
    string line;
    while (getline(inFile, line)) {
        cout << line << "\n";
    }
    inFile.close();
}
```

#### Word by Word

```cpp
ifstream inFile("input.txt");

if (inFile) {
    string word;
    while (inFile >> word) {
        cout << word << "\n";
    }
    inFile.close();
}
```

#### Character by Character

```cpp
ifstream inFile("input.txt");

if (inFile) {
    char ch;
    while (inFile.get(ch)) {
        cout << ch;
    }
    inFile.close();
}
```

#### Specific Format

```cpp
ifstream inFile("data.txt");

if (inFile) {
    int id;
    string name;
    double salary;
    
    while (inFile >> id >> name >> salary) {
        cout << "ID: " << id << ", Name: " << name 
             << ", Salary: " << salary << "\n";
    }
    inFile.close();
}
```

***

### 5.5 Binary File I/O

```cpp
#include <fstream>

// Writing binary
struct Person {
    int age;
    double height;
    char initial;
};

ofstream binOut("people.bin", ios::binary);
Person p = {30, 5.9, 'A'};
binOut.write(reinterpret_cast<char*>(&p), sizeof(Person));
binOut.close();

// Reading binary
ifstream binIn("people.bin", ios::binary);
Person p2;
binIn.read(reinterpret_cast<char*>(&p2), sizeof(Person));
cout << "Age: " << p2.age << ", Height: " << p2.height << "\n";
binIn.close();

// Reading multiple binary objects
vector<Person> people;
Person p3;
while (binIn.read(reinterpret_cast<char*>(&p3), sizeof(Person))) {
    people.push_back(p3);
}
```

***

### 5.6 File Position

```cpp
fstream file("data.txt", ios::in | ios::out);

// Get position
streampos pos = file.tellg();  // Get read position
pos = file.tellp();            // Get write position

// Set position
file.seekg(0, ios::beg);       // Seek to beginning
file.seekg(0, ios::end);       // Seek to end
file.seekg(-10, ios::end);     // Seek 10 bytes from end
file.seekp(5);                 // Seek write position to 5

// File size
file.seekg(0, ios::end);
int fileSize = file.tellg();
cout << "File size: " << fileSize << " bytes\n";

file.close();
```

***

### 5.7 Error Handling

```cpp
ifstream file("input.txt");

// Check if file opened
if (!file) {
    cout << "Failed to open file\n";
    return;
}

// Check read state
if (file.fail()) {
    cout << "Read operation failed\n";
}

if (file.bad()) {
    cout << "Severe error\n";
}

if (file.eof()) {
    cout << "End of file reached\n";
}

// Clear error flags
file.clear();

// Check if good
if (file.good()) {
    cout << "File is in good state\n";
}

file.close();
```

***

### 5.8 Complete File I/O Example

```cpp
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

struct Student {
    int id;
    string name;
    double gpa;
};

// Write students to file
void writeStudents(const string& filename, const vector<Student>& students) {
    ofstream file(filename);
    
    for (const auto& student : students) {
        file << student.id << " " << student.name << " " << student.gpa << "\n";
    }
    
    file.close();
}

// Read students from file
vector<Student> readStudents(const string& filename) {
    vector<Student> students;
    ifstream file(filename);
    
    int id;
    string name;
    double gpa;
    
    while (file >> id >> name >> gpa) {
        students.push_back({id, name, gpa});
    }
    
    file.close();
    return students;
}

// Main
int main() {
    vector<Student> students = {
        {101, "Alice", 3.8},
        {102, "Bob", 3.5},
        {103, "Carol", 3.9}
    };
    
    // Write
    writeStudents("students.txt", students);
    
    // Read
    auto readData = readStudents("students.txt");
    
    for (const auto& s : readData) {
        cout << s.id << " " << s.name << " " << s.gpa << "\n";
    }
    
    return 0;
}
```

***

## FUNCTION OBJECTS & COMPARATORS

### 6.1 Function Objects (Functors)

```cpp
// Function object for greater than comparison
struct GreaterThan {
    int value;
    
    GreaterThan(int v) : value(v) {}
    
    bool operator()(int x) const {
        return x > value;
    }
};

vector<int> v = {10, 20, 30, 40, 50};

// Use with algorithm
auto it = find_if(v.begin(), v.end(), GreaterThan(25));
if (it != v.end()) {
    cout << "Found: " << *it << "\n";  // 30
}

// Count elements greater than 25
int count = count_if(v.begin(), v.end(), GreaterThan(25));
cout << "Count: " << count << "\n";  // 3
```

### 6.2 Standard Comparators

```cpp
#include <functional>

// less - ascending order
sort(v.begin(), v.end(), less<int>());

// greater - descending order
sort(v.begin(), v.end(), greater<int>());

// equal_to, not_equal_to
count_if(v.begin(), v.end(), bind(equal_to<int>(), placeholders::_1, 20));

// Map with custom comparator
map<string, int, less<string>> ascending;      // A-Z
map<string, int, greater<string>> descending;  // Z-A
```

***

## STL BEST PRACTICES

### 7.1 Container Selection

```
Use VECTOR when:
  - Need random access
  - Need cache locality
  - Mostly append operations

Use LIST when:
  - Need frequent insertion/deletion in middle
  - Don't need random access

Use DEQUE when:
  - Need efficient push_front/pop_front
  - Need random access

Use MAP/SET when:
  - Need sorted, unique elements
  - Need O(log n) lookup

Use UNORDERED_MAP/SET when:
  - Need O(1) average lookup
  - Don't care about order

Use QUEUE when:
  - Need FIFO behavior

Use STACK when:
  - Need LIFO behavior

Use PRIORITY_QUEUE when:
  - Need elements processed by priority
```

### 7.2 Algorithm Selection

```cpp
// For small data: simple loop
for (int i = 0; i < v.size(); i++) {
    // Process v[i]
}

// For searching: find_if
auto it = find_if(v.begin(), v.end(), predicate);

// For filtering: copy_if
copy_if(v.begin(), v.end(), back_inserter(result), predicate);

// For transforming: transform
transform(v.begin(), v.end(), result.begin(), function);

// For aggregating: accumulate
int sum = accumulate(v.begin(), v.end(), 0);

// For sorting: sort
sort(v.begin(), v.end());
```

### 7.3 Memory Management

```cpp
// Reserve space when size is known
vector<int> v;
v.reserve(1000);  // Avoid reallocations

// Clear and shrink
v.clear();
v.shrink_to_fit();  // Release memory

// Use move semantics
vector<int> getVector() {
    vector<int> v;
    // ... fill v
    return v;  // Move, not copy (C++11)
}
```

### 7.4 Performance Tips

```cpp
// Prefer iterators over indices in generic code
for (auto it = v.begin(); it != v.end(); ++it) {
    // Optimized for all container types
}

// Use const references to avoid copying
void process(const vector<int>& v);

// Pre-allocate space
map<string, int> m;
m.reserve(1000);

// Use stable algorithms when order matters
stable_sort(v.begin(), v.end());

// Avoid repeated function calls in loops
int size = v.size();
for (int i = 0; i < size; i++) {
    // Use cached size
}
```

***

#### Containers Intro (C++98)

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>

int main() {
    // Vector (dynamic array)
    std::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    
    for (int i = 0; i < v.size(); i++) {
        std::cout << v[i] << " ";
    }
    std::cout << "\n";
    
    // List (linked list)
    std::list<int> l;
    l.push_back(10);
    l.push_front(5);
    
    for (std::list<int>::iterator it = l.begin(); it != l.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    
    return 0;
}
```

***



***
#### Professional Insights: Standard Library & I/O
##### Arrays
Arrays are elements of the same type placed in adjoining memory locations. The elements can be individually
referenced by a unique identiﬁer with an added index.
This allows you to declare multiple variable values of a speciﬁc type and access them individually without needing
to declare a variable for each value.
Section 8.1: Array initialization
An array is just a block of sequential memory locations for a speciﬁc type of variable. Arrays are allocated the same
way as normal variables, but with square brackets appended to its name [] that contain the number of elements
that ﬁt into the array memory.
The following example of an array uses the typ int, the variable name arrayOfInts, and the number of elements
[5] that the array has space for:
int arrayOfInts[5];
An array can be declared and initialized at the same time like this
int arrayOfInts[5] = {10, 20, 30, 40, 50};
When initializing an array by listing all of its members, it is not necessary to include the number of elements inside
the square brackets. It will be automatically calculated by the compiler. In the following example, it's 5:
int arrayOfInts[] = {10, 20, 30, 40, 50};
It is also possible to initialize only the ﬁrst elements while allocating more space. In this case, deﬁning the length in
brackets is mandatory. The following will allocate an array of length 5 with partial initialization, the compiler
initializes all remaining elements with the standard value of the element type, in this case zero.
int arrayOfInts[5] = {10,20}; // means 10, 20, 0, 0, 0
Arrays of other basic data types may be initialized in the same way.
char arrayOfChars[5]; // declare the array and allocate the memory, don't initialize
char arrayOfChars[5] = { 'a', 'b', 'c', 'd', 'e' } ; //declare and initialize
double arrayOfDoubles[5] = {1.14159, 2.14159, 3.14159, 4.14159, 5.14159};
string arrayOfStrings[5] = { "C++", "is", "super", "duper", "great!"};
It is also important to take note that when accessing array elements, the array's element index(or position) starts
from 0.
int array[5] = { 10/*Element no.0*/, 20/*Element no.1*/, 30, 40, 50/*Element no.4*/};
```cpp
std::cout << array[4]; //outputs 50
std::cout << array[0]; //outputs 10
Section 8.2: A ﬁxed size raw array matrix (that is, a 2D raw
array)
// A fixed size raw array matrix (that is, a 2D raw array).
#include <iostream>
#include <iomanip>
using namespace std;
auto main() -> int
{
    int const   n_rows  = 3;
    int const   n_cols  = 7;
    int const   m[n_rows][n_cols] =             // A raw array matrix.
    {
        {  1,  2,  3,  4,  5,  6,  7 },
        {  8,  9, 10, 11, 12, 13, 14 },
        { 15, 16, 17, 18, 19, 20, 21 }
    };
    for( int y = 0; y < n_rows; ++y )
    {
        for( int x = 0; x < n_cols; ++x )
        {
            cout << setw( 4 ) << m[y][x];       // Note: do NOT use m[y,x]!
        }
        cout << '\n';
    }
}
Output:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
C++ doesn't support special syntax for indexing a multi-dimensional array. Instead such an array is viewed as an
array of arrays (possibly of arrays, and so on), and the ordinary single index notation [i] is used for each level. In
the example above m[y] refers to row y of m, where y is a zero-based index. Then this row can be indexed in turn,
e.g. m[y][x], which refers to the xth item – or column – of row y.
I.e. the last index varies fastest, and in the declaration the range of this index, which here is the number of columns
per row, is the last and “innermost” size speciﬁed.
Since C++ doesn't provide built-in support for dynamic size arrays, other than dynamic allocation, a dynamic size
matrix is often implemented as a class. Then the raw array matrix indexing notation m[y][x] has some cost, either
by exposing the implementation (so that e.g. a view of a transposed matrix becomes practically impossible) or by
adding some overhead and slight inconvenience when it's done by returning a proxy object from operator[]. And
so the indexing notation for such an abstraction can and will usually be diﬀerent, both in look-and-feel and in the
order of indices, e.g. m(x,y) or m.at(x,y) or m.item(x,y).
Section 8.3: Dynamically sized raw array
// Example of raw dynamic size array. It's generally better to use std::vector.
#include <algorithm>            // std::sort
#include <iostream>
using namespace std;
auto int_from( istream& in ) -> int { int x; in >> x; return x; }
auto main()
    -> int
{
    cout << "Sorting n integers provided by you.\\n";
    cout << "n? ";
    int const   n   = int_from( cin );
    int*        a   = new int[n];       // ← Allocation of array of n items.
    for( int i = 1; i <= n; ++i )
    {
        cout << "The #" << i << " number, please: ";
        a[i-1] = int_from( cin );
    }
    sort( a, a + n );
    for( int i = 0; i < n; ++i ) { cout << a[i] << ' '; }
    cout << '\\n';
    delete[] a;
}
A program that declares an array T a[n]; where n is determined a run-time, can compile with certain compilers
that support C99 variadic length arrays (VLAs) as a language extension. But VLAs are not supported by standard C++.
This example shows how to manually allocate a dynamic size array via a new[]-expression,
int*        a   = new int[n];       // ← Allocation of array of n items.
… then use it, and ﬁnally deallocate it via a delete[]-expression:
delete[] a;
The array allocated here has indeterminate values, but it can be zero-initialized by just adding an empty
parenthesis (), like this: new int[n](). More generally, for arbitrary item type, this performs a value-initialization.
As part of a function down in a call hierarchy this code would not be exception safe, since an exception before the
delete[] expression (and after the new[]) would cause a memory leak. One way to address that issue is to
automate the cleanup via e.g. a std::unique_ptr smart pointer. But a generally better way to address it is to just
use a std::vector: that's what std::vector is there for.
Section 8.4: Array size: type safe at compile time
#include      // size_t, ptrdiff_t
//----------------------------------- Machinery:
using Size = ptrdiff_t;
template< class Item, size_t n >
constexpr auto n_items( Item (&)[n] ) noexcept
-> Size
{ return n; }
//----------------------------------- Usage:
#include
using namespace std;
auto main()
-> int
{
int const   a[]     = {3, 1, 4, 1, 5, 9, 2, 6, 5, 4};
Size const  n       = n_items( a );
int         b[n]    = {};       // An array of the same size as a.
(void) b;
cout <}
The C idiom for array size, sizeof(a)/sizeof(a[0]), will accept a pointer as argument and will then generally yield
an incorrect result.
For C++11
using C++11 you can do:
std::extent<decltype(MyArray)>::value;
Example:
char MyArray[] = { 'X','o','c','e' };
const auto n = std::extent<decltype(MyArray)>::value;
std::cout << n << "\n"; // Prints 4
Up till C++17 (forthcoming as of this writing) C++ had no built-in core language or standard library utility to obtain
the size of an array, but this can be implemented by passing the array by reference to a function template, as shown
above. Fine but important point: the template size parameter is a size_t, somewhat inconsistent with the signed
Size function result type, in order to accommodate the g++ compiler which sometimes insists on size_t for
template matching.
With C++17 and later one may instead use std::size, which is specialized for arrays.
Section 8.5: Expanding dynamic size array by using
std::vector
// Example of std::vector as an expanding dynamic size array.
#include <algorithm>            // std::sort
#include <iostream>
#include <vector>               // std::vector
using namespace std;
int int_from( std::istream& in ) { int x = 0; in >> x; return x; }
int main()
{
    cout << "Sorting integers provided by you.\n";
    cout << "You can indicate EOF via F6 in Windows or Ctrl+D in Unix-land.\n";
    vector<int> a;      // ← Zero size by default.
    while( cin )
    {
        cout << "One number, please, or indicate EOF: ";
        int const x = int_from( cin );
        if( !cin.fail() ) { a.push_back( x ); }  // Expands as necessary.
    }
    sort( a.begin(), a.end() );
    int const n = a.size();
    for( int i = 0; i < n; ++i ) { cout << a[i] << ' '; }
    cout << '\n';
}
std::vector is a standard library class template that provides the notion of a variable size array. It takes care of all
the memory management, and the buﬀer is contiguous so a pointer to the buﬀer (e.g. &v[0] or v.data()) can be
passed to API functions requiring a raw array. A vector can even be expanded at run time, via e.g. the push_back
member function that appends an item.
The complexity of the sequence of n push_back operations, including the copying or moving involved in the vector
expansions, is amortized O(n). “Amortized”: on average.
```

Internally this is usually achieved by the vector doubling its buﬀer size, its capacity, when a larger buﬀer is needed.
E.g. for a buﬀer starting out as size 1, and being repeatedly doubled as needed for n=17 push_back calls, this
involves 1 + 2 + 4 + 8 + 16 = 31 copy operations, which is less than 2×n = 34. And more generally the sum of this
sequence can't exceed 2×n.
Compared to the dynamic size raw array example, this vector-based code does not require the user to supply (and
know) the number of items up front. Instead the vector is just expanded as necessary, for each new item value
speciﬁed by the user.
Section 8.6: A dynamic size matrix using std::vector for
storage
Unfortunately as of C++14 there's no dynamic size matrix class in the C++ standard library. Matrix classes that
support dynamic size are however available from a number of 3rd party libraries, including the Boost Matrix library
(a sub-library within the Boost library).
If you don't want a dependency on Boost or some other library, then one poor man's dynamic size matrix in C++ is
just like
vector<vector<int>> m( 3, vector<int>( 7 ) );
… where vector is std::vector. The matrix is here created by copying a row vector n times where n is the number
of rows, here 3. It has the advantage of providing the same m[y][x] indexing notation as for a ﬁxed size raw array
matrix, but it's a bit ineﬃcient because it involves a dynamic allocation for each row, and it's a bit unsafe because
it's possible to inadvertently resize a row.
A more safe and eﬃcient approach is to use a single vector as storage for the matrix, and map the client code's (x, y)
to a corresponding index in that vector:
// A dynamic size matrix using std::vector for storage.
//--------------------------------------------- Machinery:
#include         // std::copy
#include          // assert
#include  // std::initializer_list
#include            // std::vector
#include          // ptrdiff_t
namespace my {
using Size = ptrdiff_t;
```cpp
using std::initializer_list;
using std::vector;
template< class Item >
class Matrix
{
private:
vector    items_;
Size            n_cols_;
auto index_for( Size const x, Size const y ) const
-> Size
{ return y*n_cols_ + x; }
public:
auto n_rows() const -> Size { return items_.size()/n_cols_; }
auto n_cols() const -> Size { return n_cols_; }
auto item( Size const x, Size const y )
-> Item&
{ return items_[index_for(x, y)]; }
auto item( Size const x, Size const y ) const
-> Item const&
{ return items_[index_for(x, y)]; }
Matrix(): n_cols_( 0 ) {}
Matrix( Size const n_cols, Size const n_rows )
: items_( n_cols*n_rows )
, n_cols_( n_cols )
{}
Matrix( initializer_list< initializer_list > const& values )
: items_()
, n_cols_( values.size() == 0? 0 : values.begin()->size() )
{
for( auto const& row : values )
{
assert( Size( row.size() ) == n_cols_ );
items_.insert( items_.end(), row.begin(), row.end() );
}
}
};
}  // namespace my
//--------------------------------------------- Usage:
using my::Matrix;
auto some_matrix()
-> Matrix
{
return
{
{  1,  2,  3,  4,  5,  6,  7 },
{  8,  9, 10, 11, 12, 13, 14 },
{ 15, 16, 17, 18, 19, 20, 21 }
};
}
#include
#include
using namespace std;
auto main() -> int
{
Matrix const m = some_matrix();
assert( m.n_cols() == 7 );
assert( m.n_rows() == 3 );
for( int y = 0, y_end = m.n_rows(); y < y_end; ++y )
{
for( int x = 0, x_end = m.n_cols(); x < x_end; ++x )
{
cout <← Note: not `m[y][x]`!
}
cout <}
}
Output:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
The above code is not industrial grade: it's designed to show the basic principles, and serve the needs of students
learning C++.
```

For example, one may deﬁne operator() overloads to simplify the indexing notation.
##### Iterators
Section 9.1: Overview
Iterators are Positions
Iterators are a means of navigating and operating on a sequence of elements and are a generalized extension of
pointers. Conceptually it is important to remember that iterators are positions, not elements. For example, take the
following sequence:
A B C
The sequence contains three elements and four positions
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
Elements are things within a sequence. Positions are places where meaningful operations can happen to the
sequence. For example, one inserts into a position, before or after element A, not into an element. Even deletion of
an element (erase(A)) is done by ﬁrst ﬁnding its position, then deleting it.
From Iterators to Values
To convert from a position to a value, an iterator is dereferenced:
auto my_iterator = my_vector.begin(); // position
auto my_value = *my_iterator; // value
One can think of an iterator as dereferencing to the value it refers to in the sequence. This is especially useful in
understanding why you should never dereference the end() iterator in a sequence:
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
↑           ↑
|           +-- An iterator here has no value. Do not dereference it!
+-------------- An iterator here dereferences to the value A.
In all the sequences and containers found in the C++ standard library, begin() will return an iterator to the ﬁrst
position, and end() will return an iterator to one past the last position (not the last position!). Consequently, the
names of these iterators in algorithms are oftentimes labelled first and last:
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
↑           ↑
|           |
+- first    +- last
It is also possible to obtain an iterator to any sequence, because even an empty sequence contains at least one
position:
+---+
|   |
+---+
In an empty sequence, begin() and end() will be the same position, and neither can be dereferenced:
+---+
|   |
+---+
  ↑
  |
  +- empty_sequence.begin()
  |
  +- empty_sequence.end()
The alternative visualization of iterators is that they mark the positions between elements:
+---+---+---+
| A | B | C |
+---+---+---+
↑   ^   ^   ↑
|           |
+- first    +- last
and dereferencing an iterator returns a reference to the element coming after the iterator. Some situations where
this view is particularly useful are:
insert operations will insert elements into the position indicated by the iterator,
erase operations will return an iterator corresponding to the same position as the one passed in,
an iterator and its corresponding reverse iterator are located in the same .position between elements
Invalid Iterators
An iterator becomes invalidated if (say, in the course of an operation) its position is no longer a part of a sequence.
An invalidated iterator cannot be dereferenced until it has been reassigned to a valid position. For example:
```cpp
std::vector<int>::iterator first;
{
    std::vector<int> foo;
    first = foo.begin(); // first is now valid
} // foo falls out of scope and is destroyed
// At this point first is now invalid
The many algorithms and sequence member functions in the C++ standard library have rules governing when
iterators are invalidated. Each algorithm is diﬀerent in the way they treat (and invalidate) iterators.
Navigating with Iterators
As we know, iterators are for navigating sequences. In order to do that an iterator must migrate its position
throughout the sequence. Iterators can advance forward in the sequence and some can advance backwards:
auto first = my_vector.begin();
++first;                                             // advance the iterator 1 position
std::advance(first, 1);                              // advance the iterator 1 position
first = std::next(first);                            // returns iterator to the next element
std::advance(first, -1);                             // advance the iterator 1 position backwards
first = std::next(first, 20);                        // returns iterator to the element 20 position
forward
first = std::prev(first, 5);                         // returns iterator to the element 5 position
backward
auto dist = std::distance(my_vector.begin(), first); // returns distance between two iterators.
Note, second argument of std::distance should be reachable from the ﬁrst one(or, in other words first should be
less or equal than second).
Even though you can perform arithmetic operators with iterators, not all operations are deﬁned for all types of
iterators. a = b + 3; would work for Random Access Iterators, but wouldn't work for Forward or Bidirectional
Iterators, which still can be advanced by 3 position with something like b = a; ++b; ++b; ++b;. So it is
recommended to use special functions in case you are not sure what is iterator type (for example, in a template
function accepting iterator).
Iterator Concepts
The C++ standard describes several diﬀerent iterator concepts. These are grouped according to how they behave in
the sequences they refer to. If you know the concept an iterator models (behaves like), you can be assured of the
behavior of that iterator regardless of the sequence to which it belongs. They are often described in order from the
most to least restrictive (because the next iterator concept is a step better than its predecessor):
Input Iterators : Can be dereferenced only once per position. Can only advance, and only one position at a
time.
```

Forward Iterators : An input iterator that can be dereferenced any number of times.
Bidirectional Iterators : A forward iterator that can also advance backwards one position at a time.
Random Access Iterators : A bidirectional iterator that can advance forwards or backwards any number of
positions at a time.
Contiguous Iterators (since C++17) : A random access iterator that guaranties that underlying data is
contiguous in memory.
Algorithms can vary depending on the concept modeled by the iterators they are given. For example, although
random_shuffle can be implemented for forward iterators, a more eﬃcient variant that requires random access
iterators could be provided.
Iterator traits
Iterator traits provide uniform interface to the properties of iterators. They allow you to retrieve value, diﬀerence,
pointer, reference types and also category of iterator:
```cpp
template<class Iter>
Iter find(Iter first, Iter last, typename std::iterator_traits<Iter>::value_type val)  {
    while (first != last) {
        if (*first == val)
            return first;
        ++first;
    }
    return last;
}
Category of iterator can be used to specialize algorithms:
template<class BidirIt>
void test(BidirIt a, std::bidirectional_iterator_tag)  {
    std::cout << "Bidirectional iterator is used" << std::endl;
}
template<class ForwIt>
void test(ForwIt a, std::forward_iterator_tag)  {
    std::cout << "Forward iterator is used" << std::endl;
}
template<class Iter>
void test(Iter a)  {
    test(a, typename std::iterator_traits<Iter>::iterator_category());
}
Categories of iterators are basically iterators concepts, except Contiguous Iterators don't have their own tag, since it
was found to break code.
Section 9.2: Vector Iterator
begin returns an iterator to the ﬁrst element in the sequence container.
end returns an iterator to the ﬁrst element past the end.
If the vector object is const, both begin and end return a const_iterator. If you want a const_iterator to be
returned even if your vector is not const, you can use cbegin and cend.
Example:
#include <vector>
#include <iostream>
int main() {
    std::vector<int> v = { 1, 2, 3, 4, 5 };  //intialize vector using an initializer_list
    for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    return 0;
}
Output:
1 2 3 4 5
Section 9.3: Map Iterator
```

An iterator to the ﬁrst element in the container.
If a map object is const-qualiﬁed, the function returns a const_iterator. Otherwise, it returns an iterator.
// Create a map and insert some values
```cpp
std::map<char,int> mymap;
mymap['b'] = 100;
mymap['a'] = 200;
mymap['c'] = 300;
// Iterate over all tuples
for (std::map<char,int>::iterator it = mymap.begin(); it != mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';
Output:
a => 200
b => 100
c => 300
Section 9.4: Reverse Iterators
If we want to iterate backwards through a list or vector we can use a reverse_iterator. A reverse iterator is made
from a bidirectional, or random access iterator which it keeps as a member which can be accessed through base().
To iterate backwards use rbegin() and rend() as the iterators for the end of the collection, and the start of the
collection respectively.
For instance, to iterate backwards use:
std::vector<int> v{1, 2, 3, 4, 5};
for (std::vector<int>::reverse_iterator it = v.rbegin(); it != v.rend(); ++it)
{
    cout << *it;
} // prints 54321
A reverse iterator can be converted to a forward iterator via the base() member function. The relationship is that
the reverse iterator references one element past the base() iterator:
std::vector<int>::reverse_iterator r = v.rbegin();
std::vector<int>::iterator i = r.base();
assert(&*r == &*(i-1)); // always true if r, (i-1) are dereferenceable
                        // and are not proxy iterators
 +---+---+---+---+---+---+---+
 |   | 1 | 2 | 3 | 4 | 5 |   |
 +---+---+---+---+---+---+---+
   ↑   ↑               ↑   ↑
   |   |               |   |
rend() |         rbegin()  end()
       |                   rbegin().base()
     begin()
     rend().base()
In the visualization where iterators mark positions between elements, the relationship is simpler:
  +---+---+---+---+---+
| 1 | 2 | 3 | 4 | 5 |
+---+---+---+---+---+
↑                   ↑
|                   |
|                 end()
|                 rbegin()
begin()             rbegin().base()
rend()
rend().base()
Section 9.5: Stream Iterators
Stream iterators are useful when we need to read a sequence or print formatted data from a container:
// Data stream. Any number of various whitespace characters will be OK.
std::istringstream istr("1\t 2     3 4");
std::vector<int> v;
// Constructing stream iterators and copying data from stream into vector.
std::copy(
    // Iterator which will read stream data as integers.
    std::istream_iterator<int>(istr),
    // Default constructor produces end-of-stream iterator.
    std::istream_iterator<int>(),
    std::back_inserter(v));
// Print vector contents.
std::copy(v.begin(), v.end(),
    //Will print values to standard output as integers delimeted by " -- ".
    std::ostream_iterator<int>(std::cout, " -- "));
The example program will print 1 -- 2 -- 3 -- 4 -- to standard output.
Section 9.6: C Iterators (Pointers)
// This creates an array with 5 values.
const int array[] = { 1, 2, 3, 4, 5 };
#ifdef BEFORE_CPP11
// You can use `sizeof` to determine how many elements are in an array.
const int* first = array;
const int* afterLast = first + sizeof(array) / sizeof(array[0]);
// Then you can iterate over the array by incrementing a pointer until
// it reaches past the end of our array.
for (const int* i = first; i < afterLast; ++i) {
    std::cout << *i << std::endl;
}
#else
// With C++11, you can let the STL compute the start and end iterators:
for (auto i = std::begin(array); i != std::end(array); ++i) {
    std::cout << *i << std::endl;
}
#endif
This code would output the numbers 1 through 5, one on each line like this:
Breaking It Down
const int array[] = { 1, 2, 3, 4, 5 };
This line creates a new integer array with 5 values. C arrays are just pointers to memory where each value is stored
together in a contiguous block.
const int* first = array;
const int* afterLast = first + sizeof(array) / sizeof(array[0]);
These lines create two pointers. The ﬁrst pointer is given the value of the array pointer, which is the address of the
ﬁrst element in the array. The sizeof operator when used on a C array returns the size of the array in bytes.
Divided by the size of an element this gives the number of elements in the array. We can use this to ﬁnd the
address of the block after the array.
for (const int* i = first; i < afterLast; ++i) {
Here we create a pointer which we will use as an iterator. It is initialized with the address of the ﬁrst element we
want to iterate over, and we'll continue to iterate as long as i is less than afterLast, which means as long as i is
pointing to an address within array.
    std::cout << *i << std::endl;
Finally, within the loop we can access the value our iterator i is pointing to by dereferencing it. Here the
dereference operator * returns the value at the address in i.
Section 9.7: Write your own generator-backed iterator
A common pattern in other languages is having a function that produces a "stream" of objects, and being able to
use loop-code to loop over it.
We can model this in C++ as
template<class T>
struct generator_iterator {
  using difference_type=std::ptrdiff_t;
  using value_type=T;
  using pointer=T*;
  using reference=T;
  using iterator_category=std::input_iterator_tag;
  std::optional<T> state;
  std::function< std::optional<T>() > operation;
  // we store the current element in "state" if we have one:
  T operator*() const {
    return *state;
  }
  // to advance, we invoke our operation.  If it returns a nullopt
  // we have reached the end:
  generator_iterator& operator++() {
    state = operation();
    return *this;        
  }
  generator_iterator operator++(int) {
    auto r = *this;
    ++(*this);
    return r;
  }
  // generator iterators are only equal if they are both in the "end" state:
  friend bool operator==( generator_iterator const& lhs, generator_iterator const& rhs ) {
    if (!lhs.state && !rhs.state) return true;
    return false;
  }
  friend bool operator!=( generator_iterator const& lhs, generator_iterator const& rhs ) {
    return !(lhs==rhs);
  }
  // We implicitly construct from a std::function with the right signature:
  generator_iterator( std::function< std::optional<T>() > f ):operation(std::move(f))
  {
    if (operation)
      state = operation();
  }
  // default all special member functions:
  generator_iterator( generator_iterator && ) =default;
  generator_iterator( generator_iterator const& ) =default;
  generator_iterator& operator=( generator_iterator && ) =default;
  generator_iterator& operator=( generator_iterator const& ) =default;
  generator_iterator() =default;
};
live example.
```

We store the generated element early so we can more easily detect if we are already at the end.
As the function of an end generator iterator is never used, we can create a range of generator iterators by only
copying the std::function once. A default constructed generator iterator compares equal to itself, and to all other
end-generator-iterators.
##### File I O
C++ ﬁle I/O is done via streams. The key abstractions are:
std::istream for reading text.
std::ostream for writing text.
std::streambuf for reading or writing characters.
Formatted input uses operator>>.
Formatted output uses operator<<.
Streams use std::locale, e.g., for details of the formatting and for translation between external encodings and the
internal encoding.
More on streams: <iostream> Library
Section 12.1: Writing to a ﬁle
There are several ways to write to a ﬁle. The easiest way is to use an output ﬁle stream (ofstream) together with the
stream insertion operator (<<):
std::ofstream os("foo.txt");
if(os.is_open()){
    os << "Hello World!";
}
Instead of <<, you can also use the output ﬁle stream's member function write():
std::ofstream os("foo.txt");
if(os.is_open()){
    char data[] = "Foo";
    // Writes 3 characters from data -> "Foo".
    os.write(data, 3);
}
After writing to a stream, you should always check if error state ﬂag badbit has been set, as it indicates whether the
operation failed or not. This can be done by calling the output ﬁle stream's member function bad():
os << "Hello Badbit!"; // This operation might fail for any reason.
if (os.bad())
    // Failed to write!
Section 12.2: Opening a ﬁle
Opening a ﬁle is done in the same way for all 3 ﬁle streams (ifstream, ofstream, and fstream).
You can open the ﬁle directly in the constructor:
std::ifstream ifs("foo.txt");  // ifstream: Opens file "foo.txt" for reading only.
std::ofstream ofs("foo.txt");  // ofstream: Opens file "foo.txt" for writing only.
std::fstream iofs("foo.txt");  // fstream:  Opens file "foo.txt" for reading and writing.
Alternatively, you can use the ﬁle stream's member function open():
std::ifstream ifs;
ifs.open("bar.txt");           // ifstream: Opens file "bar.txt" for reading only.
std::ofstream ofs;
ofs.open("bar.txt");           // ofstream: Opens file "bar.txt" for writing only.
std::fstream iofs;
iofs.open("bar.txt");          // fstream:  Opens file "bar.txt" for reading and writing.
You should always check if a ﬁle has been opened successfully (even when writing). Failures can include: the ﬁle
doesn't exist, ﬁle hasn't the right access rights, ﬁle is already in use, disk errors occurred, drive disconnected ...
Checking can be done as follows:
// Try to read the file 'foo.txt'.
std::ifstream ifs("fooo.txt");  // Note the typo; the file can't be opened.
// Check if the file has been opened successfully.
if (!ifs.is_open()) {
    // The file hasn't been opened; take appropriate actions here.
```cpp
    throw CustomException(ifs, "File could not be opened");
}
When ﬁle path contains backslashes (for example, on Windows system) you should properly escape them:
// Open the file 'c:\\folder\\foo.txt' on Windows.
std::ifstream ifs("c:\\\\folder\\\\foo.txt"); // using escaped backslashes
Version ≥ C++11
or use raw literal:
// Open the file 'c:\\folder\\foo.txt' on Windows.
std::ifstream ifs(R"(c:\\folder\\foo.txt)"); // using raw literal
or use forward slashes instead:
// Open the file 'c:\\folder\\foo.txt' on Windows.
std::ifstream ifs("c:/folder/foo.txt");
Version ≥ C++11
If you want to open ﬁle with non-ASCII characters in path on Windows currently you can use non-standard wide
character path argument:
// Open the file 'пример\\foo.txt' on Windows.
std::ifstream ifs(LR"(пример\\foo.txt)"); // using wide characters with raw literal
Section 12.3: Reading from a ﬁle
```

There are several ways to read data from a ﬁle.
If you know how the data is formatted, you can use the stream extraction operator (>>). Let's assume you have a ﬁle
named foo.txt which contains the following data:
John Doe 25 4 6 1987
Jane Doe 15 5 24 1976
Then you can use the following code to read that data from the ﬁle:
// Define variables.
std::ifstream is("foo.txt");
```cpp
std::string firstname, lastname;
int age, bmonth, bday, byear;
// Extract firstname, lastname, age, bday month, bday day, and bday year in that order.
// Note: '>>' returns false if it reached EOF (end of file) or if the input data doesn't
// correspond to the type of the input variable (for example, the string "foo" can't be
// extracted into an 'int' variable).
while (is >> firstname >> lastname >> age >> bmonth >> bday >> byear)
    // Process the data that has been read.
The stream extraction operator >> extracts every character and stops if it ﬁnds a character that can't be stored or if
it is a special character:
```

For string types, the operator stops at a whitespace () or at a newline (\n).
For numbers, the operator stops at a non-number character.
This means that the following version of the ﬁle foo.txt will also be successfully read by the previous code:
John
Doe 25
4 6 1987
Jane
Doe
15 5
The stream extraction operator >> always returns the stream given to it. Therefore, multiple operators can be
chained together in order to read data consecutively. However, a stream can also be used as a Boolean expression
(as shown in the while loop in the previous code). This is because the stream classes have a conversion operator
for the type bool. This bool() operator will return true as long as the stream has no errors. If a stream goes into an
error state (for example, because no more data can be extracted), then the bool() operator will return false.
Therefore, the while loop in the previous code will be exited after the input ﬁle has been read to its end.
If you wish to read an entire ﬁle as a string, you may use the following code:
// Opens 'foo.txt'.
std::ifstream is("foo.txt");
```cpp
std::string whole_file;
// Sets position to the end of the file.
is.seekg(0, std::ios::end);
// Reserves memory for the file.
whole_file.reserve(is.tellg());
// Sets position to the start of the file.
is.seekg(0, std::ios::beg);
// Sets contents of 'whole_file' to all characters in the file.
whole_file.assign(std::istreambuf_iterator<char>(is),
  std::istreambuf_iterator<char>());
```

This code reserves space for the string in order to cut down on unneeded memory allocations.
If you want to read a ﬁle line by line, you can use the function getline():
std::ifstream is("foo.txt");  
// The function getline returns false if there are no more lines.
for (std::string str; std::getline(is, str);) {
    // Process the line that has been read.
}
If you want to read a ﬁxed number of characters, you can use the stream's member function read():
std::ifstream is("foo.txt");
char str[4];
// Read 4 characters from the file.
is.read(str, 4);
After executing a read command, you should always check if the error state ﬂag failbit has been set, as it
indicates whether the operation failed or not. This can be done by calling the ﬁle stream's member function fail():
is.read(str, 4); // This operation might fail for any reason.
if (is.fail())
    // Failed to read!
Section 12.4: Opening modes
When creating a ﬁle stream, you can specify an opening mode. An opening mode is basically a setting to control
how the stream opens the ﬁle.
(All modes can be found in the std::ios namespace.)
An opening mode can be provided as second parameter to the constructor of a ﬁle stream or to its open() member
function:
std::ofstream os("foo.txt", std::ios::out | std::ios::trunc);
std::ifstream is;
is.open("foo.txt", std::ios::in | std::ios::binary);
It is to be noted that you have to set ios::in or ios::out if you want to set other ﬂags as they are not implicitly set
by the iostream members although they have a correct default value.
If you don't specify an opening mode, then the following default modes are used:
ifstream - in
ofstream - out
fstream - in and out
The ﬁle opening modes that you may specify by design are:
Mode Meaning
app
append Output
For
Description
Appends data at the end of the ﬁle.
binary binary
Input/Output Input and output is done in binary.
in
out
input
Input
Opens the ﬁle for reading.
output Output
Opens the ﬁle for writing.
trunc truncate Input/Output Removes contents of the ﬁle when opening.
ate
at end
Input
Goes to the end of the ﬁle when opening.
Note: Setting the binary mode lets the data be read/written exactly as-is; not setting it enables the translation of
the newline '\n' character to/from a platform speciﬁc end of line sequence.
For example on Windows the end of line sequence is CRLF ("\r\n").
Write: "\n" => "\r\n"
Read: "\r\n" => "\n"
Section 12.5: Reading an ASCII ﬁle into a std::string
std::ifstream f("file.txt");
if (f)
{
```cpp
  std::stringstream buffer;
  buffer << f.rdbuf();
  f.close();
  // The content of "file.txt" is available in the string `buffer.str()`
}
The rdbuf() method returns a pointer to a streambuf that can be pushed into buffer via the
stringstream::operator<< member function.
Another possibility (popularized in Eﬀective STL by Scott Meyers) is:
std::ifstream f("file.txt");
if (f)
{
  std::string str((std::istreambuf_iterator<char>(f)),
                  std::istreambuf_iterator<char>());
  // Operations on `str`...
}
This is nice because requires little code (and allows reading a ﬁle directly into any STL container, not only strings)
but can be slow for big ﬁles.
NOTE: the extra parentheses around the ﬁrst argument to the string constructor are essential to prevent the most
vexing parse problem.
Last but not least:
std::ifstream f("file.txt");
if (f)
{
  f.seekg(0, std::ios::end);
  const auto size = f.tellg();
  std::string str(size, ' ');
  f.seekg(0);
  f.read(&str[0], size);
  f.close();
  // Operations on `str`...
}
which is probably the fastest option (among the three proposed).
Section 12.6: Writing ﬁles with non-standard locale settings
If you need to write a ﬁle using diﬀerent locale settings to the default, you can use std::locale and
std::basic_ios::imbue() to do that for a speciﬁc ﬁle stream:
Guidance for use:
```

You should always apply a local to a stream before opening the ﬁle.
Once the stream has been imbued you should not change the locale.
Reasons for Restrictions: Imbuing a ﬁle stream with a locale has undeﬁned behavior if the current locale is not
state independent or not pointing at the beginning of the ﬁle.
UTF-8 streams (and others) are not state independent. Also a ﬁle stream with a UTF-8 locale may try and read the
BOM marker from the ﬁle when it is opened; so just opening the ﬁle may read characters from the ﬁle and it will
not be at the beginning.
```cpp
#include <iostream>
#include <fstream>
#include <locale>
int main()
{
  std::cout << "User-preferred locale setting is "
            << std::locale("").name().c_str() << std::endl;
  // Write a floating-point value using the user's preferred locale.
  std::ofstream ofs1;
  ofs1.imbue(std::locale(""));
  ofs1.open("file1.txt");
  ofs1 << 78123.456 << std::endl;
  // Use a specific locale (names are system-dependent)
  std::ofstream ofs2;
  ofs2.imbue(std::locale("en_US.UTF-8"));
  ofs2.open("file2.txt");
  ofs2 << 78123.456 << std::endl;
  // Switch to the classic "C" locale
  std::ofstream ofs3;
  ofs3.imbue(std::locale::classic());
  ofs3.open("file3.txt");
  ofs3 << 78123.456 << std::endl;
}
Explicitly switching to the classic "C" locale is useful if your program uses a diﬀerent default locale and you want to
ensure a ﬁxed standard for reading and writing ﬁles. With a "C" preferred locale, the example writes
78,123.456
78,123.456
78123.456
If, for example, the preferred locale is German and hence uses a diﬀerent number format, the example writes
78 123,456
78,123.456
78123.456
(note the decimal comma in the ﬁrst line).
Section 12.7: Checking end of ﬁle inside a loop condition, bad
practice?
eof returns true only after reading the end of ﬁle. It does NOT indicate that the next read will be the end of
stream.
while (!f.eof())
{
  // Everything is OK
  f >> buffer;
  // What if *only* now the eof / fail bit is set?
  /* Use `buffer` */
}
You could correctly write:
while (!f.eof())
{  
  f >> buffer >> std::ws;
  if (f.fail())
    break;
  /* Use `buffer` */
}
but
while (f >> buffer)
{
  /* Use `buffer` */
}
is simpler and less error prone.
Further references:
std::ws: discards leading whitespace from an input stream
std::basic_ios::fail: returns true if an error has occurred on the associated stream
Section 12.8: Flushing a stream
File streams are buﬀered by default, as are many other types of streams. This means that writes to the stream may
not cause the underlying ﬁle to change immediately. In oder to force all buﬀered writes to take place immediately,
you can ﬂush the stream. You can do this either directly by invoking the flush() method or through the std::flush
stream manipulator:
std::ofstream os("foo.txt");
os << "Hello World!" << std::flush;
char data[3] = "Foo";
os.write(data, 3);
os.flush();
There is a stream manipulator std::endl that combines writing a newline with ﬂushing the stream:
// Both following lines do the same thing
os << "Hello World!\n" << std::flush;
os << "Hello world!" << std::endl;
Buﬀering can improve the performance of writing to a stream. Therefore, applications that do a lot of writing
should avoid ﬂushing unnecessarily. Contrary, if I/O is done infrequently, applications should consider ﬂushing
frequently in order to avoid data getting stuck in the stream object.
Section 12.9: Reading a ﬁle into a container
In the example below we use std::string and operator>> to read items from the ﬁle.
    std::ifstream file("file3.txt");
    std::vector<std::string>  v;
    std::string s;
    while(file >> s) // keep reading until we run out
    {
        v.push_back(s);
    }
In the above example we are simply iterating through the ﬁle reading one "item" at a time using operator>>. This
same aﬀect can be achieved using the std::istream_iterator which is an input iterator that reads one "item" at a
time from the stream. Also most containers can be constructed using two iterators so we can simplify the above
code to:
    std::ifstream file("file3.txt");
    std::vector<std::string>  v(std::istream_iterator<std::string>{file},
                                std::istream_iterator<std::string>{});
We can extend this to read any object types we like by simply specifying the object we want to read as the template
parameter to the std::istream_iterator. Thus we can simply extend the above to read lines (rather than words)
like this:
// Unfortunately there is  no built in type that reads line using >>
// So here we build a simple helper class to do it. That will convert
// back to a string when used in string context.
struct Line
{
    // Store data here
    std::string data;
    // Convert object to string
    operator std::string const&() const {return data;}
    // Read a line from a stream.
    friend std::istream& operator>>(std::istream& stream, Line& line)
    {
        return std::getline(stream, line.data);
    }
};
    std::ifstream file("file3.txt");
    // Read the lines of a file into a container.
    std::vector<std::string>  v(std::istream_iterator<Line>{file},
                                std::istream_iterator<Line>{});
Section 12.10: Copying a ﬁle
std::ifstream  src("source_filename", std::ios::binary);
std::ofstream  dst("dest_filename",   std::ios::binary);
dst << src.rdbuf();
Version ≥ C++17
With C++17 the standard way to copy a ﬁle is including the <filesystem> header and using copy_file:
std::fileystem::copy_file("source_filename", "dest_filename");
The ﬁlesystem library was originally developed as boost.filesystem and ﬁnally merged to ISO C++ as of C++17.
Section 12.11: Closing a ﬁle
Explicitly closing a ﬁle is rarely necessary in C++, as a ﬁle stream will automatically close its associated ﬁle in its
destructor. However, you should try to limit the lifetime of a ﬁle stream object, so that it does not keep the ﬁle
handle open longer than necessary. For example, this can be done by putting all ﬁle operations into an own scope
({}):
std::string const prepared_data = prepare_data();
{
    // Open a file for writing.
    std::ofstream output("foo.txt");
    // Write data.
    output << prepared_data;
}  // The ofstream will go out of scope here.
   // Its destructor will take care of closing the file properly.
Calling close() explicitly is only necessary if you want to reuse the same fstream object later, but don't want to
keep the ﬁle open in between:
// Open the file "foo.txt" for the first time.
std::ofstream output("foo.txt");
// Get some data to write from somewhere.
std::string const prepared_data = prepare_data();
// Write data to the file "foo.txt".
output << prepared_data;
// Close the file "foo.txt".
output.close();
// Preparing data might take a long time. Therefore, we don't open the output file stream
// before we actually can write some data to it.
std::string const more_prepared_data = prepare_complex_data();
// Open the file "foo.txt" for the second time once we are ready for writing.
output.open("foo.txt");
// Write the data to the file "foo.txt".
output << more_prepared_data;
// Close the file "foo.txt" once again.
output.close();
Section 12.12: Reading a `struct` from a formatted text ﬁle
Version ≥ C++11
struct info_type
{
    std::string name;
    int age;
    float height;
    // we define an overload of operator>> as a friend function which
    // gives in privileged access to private data members
    friend std::istream& operator>>(std::istream& is, info_type& info)
    {
        // skip whitespace
        is >> std::ws;
        std::getline(is, info.name);
        is >> info.age;
        is >> info.height;
        return is;
    }
};
void func4()
{
    auto file = std::ifstream("file4.txt");
    std::vector<info_type> v;
    for(info_type info; file >> info;) // keep reading until we run out
    {
        // we only get here if the read succeeded
        v.push_back(info);
    }
    for(auto const& info: v)
    {
        std::cout << "  name: " << info.name << '\n';
        std::cout << "   age: " << info.age << " years" << '\n';
        std::cout << "height: " << info.height << "lbs" << '\n';
        std::cout << '\n';
    }
}
ﬁle4.txt
Wogger Wabbit
6.2
Bilbo Baggins
81.3
Mary Poppins
154.8
Output:
name: Wogger Wabbit
 age: 2 years
height: 6.2lbs
name: Bilbo Baggins
 age: 111 years
height: 81.3lbs
name: Mary Poppins
 age: 29 years
height: 154.8lbs
```

##### C:  Streams
Section 13.1: String streams
std::ostringstream is a class whose objects look like an output stream (that is, you can write to them via
operator<<), but actually store the writing results, and provide them in the form of a stream.
Consider the following short code:
```cpp
#include <sstream>
#include <string>                                                                                  
using namespace std;
int main()
{
    ostringstream ss;
    ss << "the answer to everything is " << 42;
    const string result = ss.str();
}  
The line
ostringstream ss;
creates such an object. This object is ﬁrst manipulated like a regular stream:
ss << "the answer to everything is " << 42;
Following that, though, the resulting stream can be obtained like this:
const string result = ss.str();
(the string result will be equal to "the answer to everything is 42").
This is mainly useful when we have a class for which stream serialization has been deﬁned, and for which we want a
string form. For example, suppose we have some class
class foo
{  
    // All sort of stuff here.
};  
ostream &operator<<(ostream &os, const foo &f);
To get the string representation of a foo object,
foo f;
we could use
ostringstream ss;
ss << f;
const string result = ss.str();        
```

Then result contains the string representation of the foo object.
Section 13.2: Printing collections with iostream
Basic printing
std::ostream_iterator allows to print contents of an STL container to any output stream without explicit loops.
The second argument of std::ostream_iterator constructor sets the delimiter. For example, the following code:
```cpp
std::vector<int> v = {1,2,3,4};
std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " ! "));
will print
1 ! 2 ! 3 ! 4 !
Implicit type cast
std::ostream_iterator allows to cast container's content type implicitly. For example, let's tune std::cout to print
ﬂoating-point values with 3 digits after decimal point:
std::cout << std::setprecision(3);
std::fixed(std::cout);
and instantiate std::ostream_iterator with float, while the contained values remain int:
std::vector<int> v = {1,2,3,4};
std::copy(v.begin(), v.end(), std::ostream_iterator<float>(std::cout, " ! "));
so the code above yields
1.000 ! 2.000 ! 3.000 ! 4.000 !
despite std::vector holds ints.
Generation and transformation
std::generate, std::generate_n and std::transform functions provide a very powerful tool for on-the-ﬂy data
manipulation. For example, having a vector:
std::vector<int> v = {1,2,3,4,8,16};
we can easily print boolean value of "x is even" statement for each element:
std::boolalpha(std::cout); // print booleans alphabetically
std::transform(v.begin(), v.end(), std::ostream_iterator<bool>(std::cout, " "),
[](int val) {
    return (val % 2) == 0;
});
or print the squared element:
std::transform(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "),
[](int val) {
    return val * val;
});
Printing N space-delimited random numbers:
const int N = 10;
std::generate_n(std::ostream_iterator<int>(std::cout, " "), N, std::rand);
Arrays
As in the section about reading text ﬁles, almost all these considerations may be applied to native arrays. For
example, let's print squared values from a native array:
int v[] = {1,2,3,4,8,16};
std::transform(v, std::end(v), std::ostream_iterator<int>(std::cout, " "),
[](int val) {
    return val * val;
});


***
```

#### Professional Insights: Iterators Deep Dive

##### Iterators

Section 9.1: Overview
Iterators are Positions
Iterators are a means of navigating and operating on a sequence of elements and are a generalized extension of
pointers. Conceptually it is important to remember that iterators are positions, not elements. For example, take the
following sequence:
A B C
The sequence contains three elements and four positions
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
Elements are things within a sequence. Positions are places where meaningful operations can happen to the
sequence. For example, one inserts into a position, before or after element A, not into an element. Even deletion of
an element (erase(A)) is done by ﬁrst ﬁnding its position, then deleting it.
From Iterators to Values
To convert from a position to a value, an iterator is dereferenced:
auto my_iterator = my_vector.begin(); // position
auto my_value = *my_iterator; // value
One can think of an iterator as dereferencing to the value it refers to in the sequence. This is especially useful in
understanding why you should never dereference the end() iterator in a sequence:
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
↑           ↑
|           +-- An iterator here has no value. Do not dereference it!
+-------------- An iterator here dereferences to the value A.
In all the sequences and containers found in the C++ standard library, begin() will return an iterator to the ﬁrst
position, and end() will return an iterator to one past the last position (not the last position!). Consequently, the
names of these iterators in algorithms are oftentimes labelled first and last:
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
↑           ↑
|           |
+- first    +- last
It is also possible to obtain an iterator to any sequence, because even an empty sequence contains at least one
position:
+---+
|   |
+---+
In an empty sequence, begin() and end() will be the same position, and neither can be dereferenced:
+---+
|   |
+---+
  ↑
  |
  +- empty_sequence.begin()
  |
  +- empty_sequence.end()
The alternative visualization of iterators is that they mark the positions between elements:
+---+---+---+
| A | B | C |
+---+---+---+
↑   ^   ^   ↑
|           |
+- first    +- last
and dereferencing an iterator returns a reference to the element coming after the iterator. Some situations where
this view is particularly useful are:
insert operations will insert elements into the position indicated by the iterator,
erase operations will return an iterator corresponding to the same position as the one passed in,
an iterator and its corresponding reverse iterator are located in the same .position between elements
Invalid Iterators
An iterator becomes invalidated if (say, in the course of an operation) its position is no longer a part of a sequence.
An invalidated iterator cannot be dereferenced until it has been reassigned to a valid position. For example:
```cpp
std::vector<int>::iterator first;
{
    std::vector<int> foo;
    first = foo.begin(); // first is now valid
} // foo falls out of scope and is destroyed
// At this point first is now invalid
The many algorithms and sequence member functions in the C++ standard library have rules governing when
iterators are invalidated. Each algorithm is diﬀerent in the way they treat (and invalidate) iterators.
Navigating with Iterators
As we know, iterators are for navigating sequences. In order to do that an iterator must migrate its position
throughout the sequence. Iterators can advance forward in the sequence and some can advance backwards:
auto first = my_vector.begin();
++first;                                             // advance the iterator 1 position
std::advance(first, 1);                              // advance the iterator 1 position
first = std::next(first);                            // returns iterator to the next element
std::advance(first, -1);                             // advance the iterator 1 position backwards
first = std::next(first, 20);                        // returns iterator to the element 20 position
forward
first = std::prev(first, 5);                         // returns iterator to the element 5 position
backward
auto dist = std::distance(my_vector.begin(), first); // returns distance between two iterators.
Note, second argument of std::distance should be reachable from the ﬁrst one(or, in other words first should be
less or equal than second).
Even though you can perform arithmetic operators with iterators, not all operations are deﬁned for all types of
iterators. a = b + 3; would work for Random Access Iterators, but wouldn't work for Forward or Bidirectional
Iterators, which still can be advanced by 3 position with something like b = a; ++b; ++b; ++b;. So it is
recommended to use special functions in case you are not sure what is iterator type (for example, in a template
function accepting iterator).
Iterator Concepts
The C++ standard describes several diﬀerent iterator concepts. These are grouped according to how they behave in
the sequences they refer to. If you know the concept an iterator models (behaves like), you can be assured of the
behavior of that iterator regardless of the sequence to which it belongs. They are often described in order from the
most to least restrictive (because the next iterator concept is a step better than its predecessor):
Input Iterators : Can be dereferenced only once per position. Can only advance, and only one position at a
time.
```

Forward Iterators : An input iterator that can be dereferenced any number of times.
Bidirectional Iterators : A forward iterator that can also advance backwards one position at a time.
Random Access Iterators : A bidirectional iterator that can advance forwards or backwards any number of
positions at a time.
Contiguous Iterators (since C++17) : A random access iterator that guaranties that underlying data is
contiguous in memory.
Algorithms can vary depending on the concept modeled by the iterators they are given. For example, although
random_shuffle can be implemented for forward iterators, a more eﬃcient variant that requires random access
iterators could be provided.
Iterator traits
Iterator traits provide uniform interface to the properties of iterators. They allow you to retrieve value, diﬀerence,
pointer, reference types and also category of iterator:
```cpp
template<class Iter>
Iter find(Iter first, Iter last, typename std::iterator_traits<Iter>::value_type val)  {
    while (first != last) {
        if (*first == val)
            return first;
        ++first;
    }
    return last;
}
Category of iterator can be used to specialize algorithms:
template<class BidirIt>
void test(BidirIt a, std::bidirectional_iterator_tag)  {
    std::cout << "Bidirectional iterator is used" << std::endl;
}
template<class ForwIt>
void test(ForwIt a, std::forward_iterator_tag)  {
    std::cout << "Forward iterator is used" << std::endl;
}
template<class Iter>
void test(Iter a)  {
    test(a, typename std::iterator_traits<Iter>::iterator_category());
}
Categories of iterators are basically iterators concepts, except Contiguous Iterators don't have their own tag, since it
was found to break code.
Section 9.2: Vector Iterator
begin returns an iterator to the ﬁrst element in the sequence container.
end returns an iterator to the ﬁrst element past the end.
If the vector object is const, both begin and end return a const_iterator. If you want a const_iterator to be
returned even if your vector is not const, you can use cbegin and cend.
Example:
#include <vector>
#include <iostream>
int main() {
    std::vector<int> v = { 1, 2, 3, 4, 5 };  //intialize vector using an initializer_list
    for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    return 0;
}
Output:
1 2 3 4 5
Section 9.3: Map Iterator
```

An iterator to the ﬁrst element in the container.
If a map object is const-qualiﬁed, the function returns a const_iterator. Otherwise, it returns an iterator.
// Create a map and insert some values
```cpp
std::map<char,int> mymap;
mymap['b'] = 100;
mymap['a'] = 200;
mymap['c'] = 300;
// Iterate over all tuples
for (std::map<char,int>::iterator it = mymap.begin(); it != mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';
Output:
a => 200
b => 100
c => 300
Section 9.4: Reverse Iterators
If we want to iterate backwards through a list or vector we can use a reverse_iterator. A reverse iterator is made
from a bidirectional, or random access iterator which it keeps as a member which can be accessed through base().
To iterate backwards use rbegin() and rend() as the iterators for the end of the collection, and the start of the
collection respectively.
For instance, to iterate backwards use:
std::vector<int> v{1, 2, 3, 4, 5};
for (std::vector<int>::reverse_iterator it = v.rbegin(); it != v.rend(); ++it)
{
    cout << *it;
} // prints 54321
A reverse iterator can be converted to a forward iterator via the base() member function. The relationship is that
the reverse iterator references one element past the base() iterator:
std::vector<int>::reverse_iterator r = v.rbegin();
std::vector<int>::iterator i = r.base();
assert(&*r == &*(i-1)); // always true if r, (i-1) are dereferenceable
                        // and are not proxy iterators
 +---+---+---+---+---+---+---+
 |   | 1 | 2 | 3 | 4 | 5 |   |
 +---+---+---+---+---+---+---+
   ↑   ↑               ↑   ↑
   |   |               |   |
rend() |         rbegin()  end()
       |                   rbegin().base()
     begin()
     rend().base()
In the visualization where iterators mark positions between elements, the relationship is simpler:
  +---+---+---+---+---+
| 1 | 2 | 3 | 4 | 5 |
+---+---+---+---+---+
↑                   ↑
|                   |
|                 end()
|                 rbegin()
begin()             rbegin().base()
rend()
rend().base()
Section 9.5: Stream Iterators
Stream iterators are useful when we need to read a sequence or print formatted data from a container:
// Data stream. Any number of various whitespace characters will be OK.
std::istringstream istr("1\t 2     3 4");
std::vector<int> v;
// Constructing stream iterators and copying data from stream into vector.
std::copy(
    // Iterator which will read stream data as integers.
    std::istream_iterator<int>(istr),
    // Default constructor produces end-of-stream iterator.
    std::istream_iterator<int>(),
    std::back_inserter(v));
// Print vector contents.
std::copy(v.begin(), v.end(),
    //Will print values to standard output as integers delimeted by " -- ".
    std::ostream_iterator<int>(std::cout, " -- "));
The example program will print 1 -- 2 -- 3 -- 4 -- to standard output.
Section 9.6: C Iterators (Pointers)
// This creates an array with 5 values.
const int array[] = { 1, 2, 3, 4, 5 };
#ifdef BEFORE_CPP11
// You can use `sizeof` to determine how many elements are in an array.
const int* first = array;
const int* afterLast = first + sizeof(array) / sizeof(array[0]);
// Then you can iterate over the array by incrementing a pointer until
// it reaches past the end of our array.
for (const int* i = first; i < afterLast; ++i) {
    std::cout << *i << std::endl;
}
#else
// With C++11, you can let the STL compute the start and end iterators:
for (auto i = std::begin(array); i != std::end(array); ++i) {
    std::cout << *i << std::endl;
}
#endif
This code would output the numbers 1 through 5, one on each line like this:
Breaking It Down
const int array[] = { 1, 2, 3, 4, 5 };
This line creates a new integer array with 5 values. C arrays are just pointers to memory where each value is stored
together in a contiguous block.
const int* first = array;
const int* afterLast = first + sizeof(array) / sizeof(array[0]);
These lines create two pointers. The ﬁrst pointer is given the value of the array pointer, which is the address of the
ﬁrst element in the array. The sizeof operator when used on a C array returns the size of the array in bytes.
Divided by the size of an element this gives the number of elements in the array. We can use this to ﬁnd the
address of the block after the array.
for (const int* i = first; i < afterLast; ++i) {
Here we create a pointer which we will use as an iterator. It is initialized with the address of the ﬁrst element we
want to iterate over, and we'll continue to iterate as long as i is less than afterLast, which means as long as i is
pointing to an address within array.
    std::cout << *i << std::endl;
Finally, within the loop we can access the value our iterator i is pointing to by dereferencing it. Here the
dereference operator * returns the value at the address in i.
Section 9.7: Write your own generator-backed iterator
A common pattern in other languages is having a function that produces a "stream" of objects, and being able to
use loop-code to loop over it.
We can model this in C++ as
template<class T>
struct generator_iterator {
  using difference_type=std::ptrdiff_t;
  using value_type=T;
  using pointer=T*;
  using reference=T;
  using iterator_category=std::input_iterator_tag;
  std::optional<T> state;
  std::function< std::optional<T>() > operation;
  // we store the current element in "state" if we have one:
  T operator*() const {
    return *state;
  }
  // to advance, we invoke our operation.  If it returns a nullopt
  // we have reached the end:
  generator_iterator& operator++() {
    state = operation();
    return *this;        
  }
  generator_iterator operator++(int) {
    auto r = *this;
    ++(*this);
    return r;
  }
  // generator iterators are only equal if they are both in the "end" state:
  friend bool operator==( generator_iterator const& lhs, generator_iterator const& rhs ) {
    if (!lhs.state && !rhs.state) return true;
    return false;
  }
  friend bool operator!=( generator_iterator const& lhs, generator_iterator const& rhs ) {
    return !(lhs==rhs);
  }
  // We implicitly construct from a std::function with the right signature:
  generator_iterator( std::function< std::optional<T>() > f ):operation(std::move(f))
  {
    if (operation)
      state = operation();
  }
  // default all special member functions:
  generator_iterator( generator_iterator && ) =default;
  generator_iterator( generator_iterator const& ) =default;
  generator_iterator& operator=( generator_iterator && ) =default;
  generator_iterator& operator=( generator_iterator const& ) =default;
  generator_iterator() =default;
};
live example.
```

We store the generated element early so we can more easily detect if we are already at the end.
As the function of an end generator iterator is never used, we can create a range of generator iterators by only
copying the std::function once. A default constructed generator iterator compares equal to itself, and to all other
end-generator-iterators.

### <a name="chapter-6-stlinternalsdeepdive"></a>CHAPTER 6: STL INTERNALS DEEP DIVE


***
#### Professional Insights: Maps, Sets & Algorithms

##### std::map

To use any of std::map or std::multimap the header ﬁle <map> should be included.
```cpp
std::map and std::multimap both keep their elements sorted according to the ascending order of keys. In
case of std::multimap, no sorting occurs for the values of the same key.
The basic diﬀerence between std::map and std::multimap is that the std::map one does not allow duplicate
values for the same key where std::multimap does.
Maps are implemented as binary search trees. So search(), insert(), erase() takes Θ(log n) time in
average. For constant time operation use std::unordered_map.
size() and empty() functions have Θ(1) time complexity, number of nodes is cached to avoid walking
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
Version ≥ C++11
Elements of a std::map can be accessed with at():
std::cout << ranking.at("stackoverflow") << std::endl;
Note that at() will throw an std::out_of_range exception if the container does not contain the requested
element.
In both containers std::map and std::multimap, elements can be accessed using iterators:
Version ≥ C++11
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
points to the element causing the conﬂict, and the bool is value is false.
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
Version ≥ C++11
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
To get the iterator of the ﬁrst occurrence of a key, the find() function can be used. It returns end() if the key
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
Another way to ﬁnd whether an entry exists in std::map or in std::multimap is using the count() function,
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
multimaps, it can stop once the ﬁrst matching element has been found.
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
```

Both could be initialized with iterator.
// From std::map or std::multimap iterator
std::multimap< int , int > mmp{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {6, 8}, {3, 4},
                               {6, 7} };
                       // {1, 2}, {3, 4}, {3, 4}, {6, 5}, {6, 8}, {6, 7}, {8, 9}
```cpp
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
```

A map is an associative container, containing key-value pairs.
```cpp
#include <string>
#include <map>
std::map<std::string, size_t> fruits_count;
In the above example, std::string is the key type, and size_t is a value.
```

The key acts as an index in the map. Each key must be unique, and must be ordered.
If you need mutliple elements with the same key, consider using multimap (explained below)
If your value type does not specify any ordering, or you want to override the default ordering, you may
provide one:
```cpp
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
diﬀer.
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
Section 50.9: Creating std::map with user-deﬁned types as
key
In order to be able to use a class as the key in a map, all that is required of the key is that it be copiable and
assignable. The ordering within the map is deﬁned by the third argument to the template (and the argument to
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
```

This is a mathematical term to deﬁne a relationship between two objects.
Its deﬁnition is:
Two objects x and y are equivalent if both f(x, y) and f(y, x) are false. Note that an object is always (by the
irreﬂexivity invariant) equivalent to itself.
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
How you deﬁne equivalent/less is totally dependent on the type of your object.

##### std::set and std::multiset

set is a type of container whose elements are sorted and unique.  multiset is similar, but, in the case of multiset,
multiple elements can have the same value.
Section 59.1: Changing the default sort of a set
set and multiset have default compare methods, but in some cases you may need to overload them.
Let's imagine we are storing string values in a set, but we know those strings contain only numeric values. By
default the sort will be a lexicographical string comparison, so the order won't match the numerical sort. If you
want to apply a sort equivalent to what you would have with int values, you need a functor to overload the
compare method:
```cpp
#include <iostream>
#include <set>
#include <stdlib.h>
struct custom_compare final
{
    bool operator() (const std::string& left, const std::string& right) const
    {
        int nLeft = atoi(left.c_str());
        int nRight = atoi(right.c_str());
        return nLeft < nRight;
    }
};
int main ()
{
    std::set<std::string> sut({"1", "2", "5", "23", "6", "290"});
    std::cout << "### Default sort on std::set<std::string> :" << std::endl;
    for (auto &&data: sut)
        std::cout << data << std::endl;
    std::set<std::string, custom_compare> sut_custom({"1", "2", "5", "23", "6", "290"},
                                                     custom_compare{}); //< Compare object optional
as its default constructible.
    std::cout << std::endl << "### Custom sort on set :" << std::endl;
    for (auto &&data : sut_custom)
        std::cout << data << std::endl;
    auto compare_via_lambda = [](auto &&lhs, auto &&rhs){ return lhs > rhs; };
    using set_via_lambda = std::set<std::string, decltype(compare_via_lambda)>;
    set_via_lambda sut_reverse_via_lambda({"1", "2", "5", "23", "6", "290"},
                                          compare_via_lambda);
    std::cout << std::endl << "### Lambda sort on set :" << std::endl;
    for (auto &&data : sut_reverse_via_lambda)
        std::cout << data << std::endl;
    return 0;
}
Output will be:
```

### Default sort on std::set<std::string> :
#### Custom sort on set :
#### Lambda sort on set :
In the example above, one can ﬁnd 3 diﬀerent ways of adding compare operations to the std::set, each of them is
useful in its own context.
Default sort
This will use the compare operator of the key (ﬁrst template argument). Often, the key will already provide a good
default for the std::less<T> function. Unless this function is specialized, it uses the operator< of the object. This is
especially useful when other code also tries to use some ordering, as this allows consistency over the whole code
base.
Writing the code this way, will reduce the eﬀort to update your code when the key changes is API, like: a class
containing 2 members which changes to a class containing 3 members. By updating the operator< in the class, all
occurrences will get updated.
As you might expect, using the default sort is a reasonable default.
Custom sort
Adding a custom sort via an object with a compare operator is often used when the default comparison doesn't
comply. In the example above this is because the strings are referring to integers. In other cases, it's often used
when you want to compare (smart) pointers based upon the object they refer to or because you need diﬀerent
constraints for comparing (example: comparing std::pair by the value of first).
When creating a compare operator, this should be a stable sorting. If the result of the compare operator changes
after insert, you will have undeﬁned behavior. As a good practice, your compare operator should only use the
constant data (const members, const functions ...).
As in the example above, you will often encounter classes without members as compare operators. This results in
default constructors and copy constructors. The default constructor allows you to omit the instance at construction
time and the copy constructor is required as the set takes a copy of the compare operator.
Lambda sort
Lambdas are a shorter way to write function objects. This allows writing the compare operator on less lines, making
the overall code more readable.
The disadvantage of the use of lambdas is that each lambda gets a speciﬁc type at compile time, so
decltype(lambda) will be diﬀerent for each compilation of the same compilation unit (cpp ﬁle) as over multiple
compilation units (when included via header ﬁle). For this reason, its recommended to use function objects as
compare operator when used within header ﬁles.
This construction is often encountered when a std::set is used within the local scope of a function instead, while
the function object is preferred when used as function arguments or class members.
Other sort options
As the compare operator of std::set is a template argument, all callable objects can be used as compare operator
and the examples above are only speciﬁc cases. The only restrictions these callable objects have are:
They must be copy constructable
They must be callable with 2 arguments of the type of the key. (implicit conversions are allowed, though not
recommended as it can hurt performance)
Section 59.2: Deleting values from a set
The most obvious method, if you just want to reset your set/multiset to an empty one, is to use clear:
```cpp
  std::set<int> sut;
  sut.insert(10);
  sut.insert(15);
  sut.insert(22);
  sut.insert(3);
  sut.clear(); //size of sut is 0
Then the erase method can be used.  It oﬀers some possibilities looking somewhat equivalent to the insertion:
std::set<int> sut;
std::set<int>::iterator it;
sut.insert(10);
sut.insert(15);
sut.insert(22);
sut.insert(3);
sut.insert(30);
sut.insert(33);
sut.insert(45);
// Basic deletion
sut.erase(3);
// Using iterator
it = sut.find(22);
sut.erase(it);
// Deleting a range of values
it = sut.find(33);
sut.erase(it, sut.end());
std::cout << std::endl << "Set under test contains:" << std::endl;
for (it = sut.begin(); it != sut.end(); ++it)
{
  std::cout << *it << std::endl;
}
Output will be:
Set under test contains:                                                                          
All those methods also apply to multiset. Please note that if you ask to delete an element from a multiset, and it
is present multiple times, all the equivalent values will be deleted.
Section 59.3: Inserting values in a set
```

Three diﬀerent methods of insertion can used with sets.
First, a simple insert of the value. This method returns a pair allowing the caller to check whether the insert
really occurred.
Second, an insert by giving a hint of where the value will be inserted. The objective is to optimize the
insertion time in such a case, but knowing where a value should be inserted is not the common case. Be
careful in that case; the way to give a hint diﬀers with compiler versions.
Finally you can insert a range of values by giving a starting and an ending pointer. The starting one will be
included in the insertion, the ending one is excluded.
```cpp
#include <iostream>
#include <set>
int main ()
{
  std::set<int> sut;
  std::set<int>::iterator it;
  std::pair<std::set<int>::iterator,bool> ret;
  // Basic insert
  sut.insert(7);
  sut.insert(5);
  sut.insert(12);
  ret = sut.insert(23);
  if (ret.second==true)
    std::cout << "# 23 has been inserted!" << std::endl;
  ret = sut.insert(23); // since it's a set and 23 is already present in it, this insert should
fail
  if (ret.second==false)
    std::cout << "# 23 already present in set!" << std::endl;
  // Insert with hint for optimization
  it = sut.end();
  // This case is optimized for C++11 and above
  // For earlier version, point to the element preceding your insertion
  sut.insert(it, 30);
  // inserting a range of values
  std::set<int> sut2;
  sut2.insert(20);
  sut2.insert(30);
  sut2.insert(45);
  std::set<int>::iterator itStart = sut2.begin();
  std::set<int>::iterator itEnd = sut2.end();
  sut.insert (itStart, itEnd); // second iterator is excluded from insertion
  std::cout << std::endl << "Set under test contains:" << std::endl;
  for (it = sut.begin(); it != sut.end(); ++it)
  {
    std::cout << *it << std::endl;
  }
  return 0;
}
Output will be:
```

## 23 has been inserted!                                                                            
## 23 already present in set!                                                                      
Set under test contains:                                                                          
Section 59.4: Inserting values in a multiset
All the insertion methods from sets also apply to multisets. Nevertheless, another possibility exists, which is
providing an initializer_list:
```cpp
auto il = { 7, 5, 12 };
std::multiset<int> msut;
msut.insert(il);
Section 59.5: Searching values in set and multiset
There are several ways to search a given value in std::set or in std::multiset:
To get the iterator of the ﬁrst occurrence of a key, the find() function can be used. It returns end() if the key does
not exist.
  std::set<int> sut;
  sut.insert(10);
  sut.insert(15);
  sut.insert(22);
  sut.insert(3); // contains 3, 10, 15, 22    
  auto itS = sut.find(10); // the value is found, so *itS == 10
  itS = sut.find(555); // the value is not found, so itS == sut.end()  
  std::multiset<int> msut;
  sut.insert(10);
  sut.insert(15);
  sut.insert(22);
  sut.insert(15);
  sut.insert(3); // contains 3, 10, 15, 15, 22  
  auto itMS = msut.find(10);
Another way is using the count() function, which counts how many corresponding values have been found in the
set/multiset (in case of a set, the return value can be only 0 or 1). Using the same values as above, we will have:
int result = sut.count(10); // result == 1
result = sut.count(555); // result == 0
result = msut.count(10); // result == 1
result = msut.count(15); // result == 2
In the case of std::multiset, there could be several elements having the same value. To get this range, the
equal_range() function can be used. It returns std::pair having iterator lower bound (inclusive) and upper bound
(exclusive) respectively. If the key does not exist, both iterators would point to the nearest superior value (based on
compare method used to sort the given multiset).
auto eqr = msut.equal_range(15);
auto st = eqr.first; // point to first element '15'
auto en = eqr.second; // point to element '22'
eqr = msut.equal_range(9); // both eqr.first and eqr.second point to element '10'
```


##### Standard Library Algorithms

Section 62.1: std::next_permutation
```cpp
template< class Iterator >
bool next_permutation( Iterator first, Iterator last );
template< class Iterator, class Compare >
bool next_permutation( Iterator first, Iterator last, Compare cmpFun );
Eﬀects:
Sift the data sequence of the range [ﬁrst, last) into the next lexicographically higher permutation. If cmpFun is
provided, the permutation rule is customized.
Parameters:
first- the beginning of the range to be permutated, inclusive
last - the end of the range to be permutated, exclusive
Return Value:
```

Returns true if such permutation exists.
Otherwise the range is swaped to the lexicographically smallest permutation and return false.
Complexity:
O(n), n is the distance from first to last.
Example:
```cpp
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
Eﬀects:
Applies f to the result of dereferencing every iterator in the range [first, last) starting from first and
proceeding to last - 1.
Parameters:
first, last - the range to apply f to.
f - callable object which is applied to the result of dereferencing every iterator in the range [first, last).
Return value:
f (until C++11) and std::move(f) (since C++11).
Complexity:
```

Applies f exactly last - first times.
Example:
Version ≥ c++11
```cpp
std::vector<int> v { 1, 2, 4, 8, 16 };
std::for_each(v.begin(), v.end(), [](int elem) { std::cout << elem << " "; });
```

Applies the given function for every element of the vector v printing this element to stdout.
Section 62.3: std::accumulate
Deﬁned in header <numeric>
```cpp
template<class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init); // (1)
template<class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation f); // (2)
Eﬀects:
std::accumulate performs fold operation using f function on range [first, last) starting with init as
accumulator value.
Eﬀectively it's equivalent of:
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
```

Accumulated value of f applications.
Complexity:
O(n×k), where n is the distance from first to last, O(k) is complexity of f function.
Example:
Simple sum example:
```cpp
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
Version ≥ c++11
const std::vector<int> ds = {1, 2, 3};
int n = std::accumulate(ds.begin(), ds.end(),
                        0,
                        [](int a, int d) { return a * 10 + d; });
std::cout << n << std::endl;
Output:
Section 62.4: std::ﬁnd
template <class InputIterator, class T>
InputIterator find (InputIterator first, InputIterator last, const T& val);
Eﬀects
Finds the ﬁrst occurrence of val within the range [ﬁrst, last)
Parameters
first => iterator pointing to the beginning of the range last => iterator pointing to the end of the range val => The
value to ﬁnd within the range
Return
An iterator that points to the ﬁrst element within the range that is equal(==) to val, the iterator points to last if val is
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
Eﬀects
Finds the minimum element in a range
Parameters
first - iterator pointing to the beginning of the range
last - iterator pointing to the end of the range comp - a function pointer or function object that takes two
arguments and returns true or false indicating whether argument is less than argument 2. This function should not
modify inputs
Return
Iterator to the minimum element in the range
Complexity
```

Linear in one less than the number of elements compared.
Example
```cpp
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
Section 62.6: std::ﬁnd_if
template <class InputIterator, class UnaryPredicate>
InputIterator find_if (InputIterator first, InputIterator last, UnaryPredicate pred);
Eﬀects
Finds the ﬁrst element in a range for which the predicate function pred returns true.
Parameters
first => iterator pointing to the beginning of the range last => iterator pointing to the end of the range pred =>
predicate function(returns true or false)
Return
An iterator that points to the ﬁrst element within the range the predicate function pred returns true for. The
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
```

Note This function is very eﬃcient - it has linear complexity.
For the sake of this example, let's deﬁne the median of a sequence of length n as the element that would be in
position ⌈n / 2⌉. For example, the median of a sequence of length 5 is the 3rd smallest element, and so is the
median of a sequence of length 6.
To use this function to ﬁnd the median, we can use the following. Say we start with
```cpp
std::vector<int> v{5, 1, 2, 3, 4};    
std::vector<int>::iterator b = v.begin();
std::vector<int>::iterator e = v.end();
std::vector<int>::iterator med = b;
std::advance(med, v.size() / 2);
// This makes the 2nd position hold the median.
std::nth_element(b, med, e);    
// The median is now at v[2].
To ﬁnd the pth quantile, we would change some of the lines above:
const std::size_t pos = p * std::distance(b, e);
std::advance(nth, pos);
and look for the quantile at position pos.
Section 62.8: std::count
template <class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type
count (InputIterator first, InputIterator last, const T& val);
Eﬀects
Counts the number of elements that are equal to val
Parameters
first => iterator pointing to the beginning of the range
last => iterator pointing to the end of the range
val => The occurrence of this value in the range will be counted
Return
```

The number of elements in the range that are equal(==) to val.
Example
```cpp
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
Eﬀects
Counts the number of elements in a range for which a speciﬁed predicate function is true
Parameters
first => iterator pointing to the beginning of the range last => iterator pointing to the end of the range red =>
predicate function(returns true or false)
Return
The number of elements within the speciﬁed range for which the predicate function returned true.
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
```


##### Sorting

Section 67.1: Sorting and sequence containers
std::sort, found in the standard library header algorithm, is a standard library algorithm for sorting a range of
values, deﬁned by a pair of iterators. std::sort takes as the last parameter a functor used to compare two values;
this is how it determines the order. Note that std::sort is not stable.
The comparison function must impose a Strict, Weak Ordering on the elements. A simple less-than (or greater-than)
comparison will suﬃce.
A container with random-access iterators can be sorted using the std::sort algorithm:
Version ≥ C++11
```cpp
#include <vector>
#include <algorithm>
std::vector<int> MyVector = {3, 1, 2}
//Default comparison of <
std::sort(MyVector.begin(), MyVector.end());
std::sort requires that its iterators are random access iterators. The sequence containers std::list and
std::forward_list (requiring C++11) do not provide random access iterators, so they cannot be used with
std::sort. However, they do have sort member functions which implement a sorting algorithm that works with
their own iterator types.
Version ≥ C++11
#include <list>
#include <algorithm>
std::list<int> MyList = {3, 1, 2}
//Default comparison of <
//Whole list only.
MyList.sort();
Their member sort functions always sort the entire list, so they cannot sort a sub-range of elements. However,
since list and forward_list have fast splicing operations, you could extract the elements to be sorted from the
list, sort them, then stuﬀ them back where they were quite eﬃciently like this:
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
```

If entries with equal keys are possible, use multimap instead of map (like in the following example).
To sort elements in descending manner, declare the map with a proper comparison functor (std::greater<>):
```cpp
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
which must return a type contextually convertible to bool (or just bool). Basic types (integers, ﬂoats, pointers etc)
have already build in comparison operators.
```

We can overload this operator to make the default sort call work on user-deﬁned types.
// Include sequence containers
```cpp
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
Version ≥ C++11
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
The sort algorithm sorts a sequence deﬁned by two iterators. This is enough to sort a built-in (also known as c-
style) array.
Version ≥ C++11
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
Version ≥ C++11
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
Version ≥ C++14
In C++14, we don't need to provide the template argument for the comparison function objects and instead let the
object deduce based on what it gets passed in:
std::sort(v.begin(), v.end(), std::less<>());     // ascending order
std::sort(v.begin(), v.end(), std::greater<>());  // descending order



***
```

#### Professional Insights: String & Vector Deep Dive

##### std::string

Strings are objects that represent sequences of characters. The standard string class provides a simple, safe and
versatile alternative to using explicit arrays of chars when dealing with text and other sequences of characters. The
C++ string class is part of the std namespace and was standardized in 1998.
Section 47.1: Tokenize
Listed from least expensive to most expensive at run-time:
1.
std::strtok is the cheapest standard provided tokenization method, it also allows the delimiter to be
modiﬁed between tokens, but it incurs 3 diﬃculties with modern C++:
std::strtok cannot be used on multiple strings at the same time (though some implementations do
extend to support this, such as: strtok_s)
For the same reason std::strtok cannot be used on multiple threads simultaneously (this may
however be implementation deﬁned, for example: Visual Studio's implementation is thread safe)
Calling std::strtok modiﬁes the std::string it is operating on, so it cannot be used on const
strings, const char*s, or literal strings, to tokenize any of these with std::strtok or to operate on a
```cpp
std::string who's contents need to be preserved, the input would have to be copied, then the copy
could be operated on
Generally any of these options cost will be hidden in the allocation cost of the tokens, but if the cheapest
algorithm is required and std::strtok's diﬃculties are not overcomable consider a hand-spun solution.
// String to tokenize
std::string str{ "The quick brown fox" };
// Vector to store tokens
vector<std::string> tokens;
for (auto i = strtok(&str[0], " "); i != NULL; i = strtok(NULL, " "))
    tokens.push_back(i);
Live Example
2.
The std::istream_iterator uses the stream's extraction operator iteratively. If the input std::string is
white-space delimited this is able to expand on the std::strtok option by eliminating its diﬃculties, allowing
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
The std::regex_token_iterator uses a std::regex to iteratively tokenize. It provides for a more ﬂexible
delimiter deﬁnition. For example, non-delimited commas and white-space:
Version ≥ C++11
// String to tokenize
const std::string str{ "The ,qu\\,ick ,\tbrown, fox" };
const std::regex re{ "\\s*((?:[^\\\\,]|\\\\.)*?)\\s*(?:,|$)" };
// Vector to store tokens
const std::vector<std::string> tokens{
    std::sregex_token_iterator(str.begin(), str.end(), re, 1),
    std::sregex_token_iterator()
};
Live Example
```

See the regex_token_iterator Example for more details.
Section 47.2: Conversion to (const) char*
In order to get const char* access to the data of a std::string you can use the string's c_str() member function.
Keep in mind that the pointer is only valid as long as the std::string object is within scope and remains
unchanged, that means that only const methods may be called on the object.
Version ≥ C++17
The data() member function can be used to obtain a modiﬁable char*, which can be used to manipulate the
```cpp
std::string object's data.
Version ≥ C++11
A modiﬁable char* can also be obtained by taking the address of the ﬁrst character: &s[0]. Within C++11, this is
guaranteed to yield a well-formed, null-terminated string. Note that &s[0] is well-formed even if s is empty,
whereas &s.front() is undeﬁned if s is empty.
Version ≥ C++11
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
Version ≥ C++17
C++17 introduces std::string_view, which is simply a non-owning range of const chars, implementable as either
a pair of pointers or a pointer and a length. It is a superior parameter type for functions that requires non-
modiﬁable string data. Before C++17, there were three options for this:
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
It oﬀers a useful subset of the functionality that std::string does, although some of the functions behave
diﬀerently:
std::string str = "lllloooonnnngggg sssstttrrriiinnnggg"; //A really long string
//Bad way - 'string::substr' returns a new string (expensive if the string is long)
std::cout << str.substr(15, 10) << '\n';
//Good way - No copies are created!
std::string_view view = str;
// string_view::substr returns a new string_view
std::cout << view.substr(15, 10) << '\n';
Section 47.4: Conversion to std::wstring
In C++, sequences of characters are represented by specializing the std::basic_string class with a native
character type. The two major collections deﬁned by the standard library are std::string and std::wstring:
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
In order to improve usability and/or readability, you can deﬁne functions to perform the conversion:
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
```

World!").
Please note that char and wchar_t do not imply encoding, and gives no indication of size in bytes. For instance,
wchar_t is commonly implemented as a 2-bytes data type and typically contains UTF-16 encoded data under
Windows (or UCS-2 in versions prior to Windows 2000) and as a 4-bytes data type encoded using UTF-32 under
Linux. This is in contrast with the newer types char16_t and char32_t, which were introduced in C++11 and are
guaranteed to be large enough to hold any UTF16 or UTF32 "character" (or more precisely, code point) respectively.
Section 47.5: Lexicographical comparison
Two std::strings can be compared lexicographically using the operators ==, !=, <, <=, >, and >=:
```cpp
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
Finds the ﬁrst diﬀerent character pair, compares them then returns the boolean result.
operator<= or operator>=:
Finds the ﬁrst diﬀerent character pair, compares them then returns the boolean result.
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
```

Compare the ﬁrst characters, 'B' == 'B' - move on.
Compare the second characters, 'a' == 'a' - move on.
Compare the third characters, 'r' == 'r' - move on.
The str2 range is now exhausted, while the str1 range still has characters. Thus, str2 < str1.
Section 47.6: Trimming characters at start/end
This example requires the headers <algorithm>, <locale>, and <utility>.
Version ≥ C++11
To trim a sequence or string means to remove all leading and trailing elements (or characters) matching a certain
predicate. We ﬁrst trim the trailing elements, because it doesn't involve moving any elements, and then trim the
leading elements. Note that the generalizations below work for all types of std::basic_string (e.g. std::string
and std::wstring), and accidentally also for sequence containers (e.g. std::vector and std::list).
```cpp
template <typename Sequence, // any basic_string, vector, list etc.
          typename Pred>     // a predicate on the element (character) type
Sequence& trim(Sequence& seq, Pred pred) {
    return trim_start(trim_end(seq, pred), pred);
}
Trimming the trailing elements involves ﬁnding the last element not matching the predicate, and erasing from there
on:
template <typename Sequence, typename Pred>
Sequence& trim_end(Sequence& seq, Pred pred) {
    auto last = std::find_if_not(seq.rbegin(),
                                 seq.rend(),
                                 pred);
    seq.erase(last.base(), seq.end());
    return seq;
}
Trimming the leading elements involves ﬁnding the ﬁrst element not matching the predicate and erasing up to
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
Version ≥ C++14
//4)
str.replace(19, 5, alternate, 6); //"Hello foo, bar and foobar!"
//5)
str.replace(str.begin(), str.begin() + 5, str.begin() + 6, str.begin() + 9);
//"foo foo, bar and world!"
//6)
str.replace(0, 5, 3, 'z'); //"zzz foo, bar and world!"
//7)
str.replace(str.begin() + 6, str.begin() + 9, 3, 'x'); //"Hello xxx, bar and world!"
Version ≥ C++11
//8)
str.replace(str.begin(), str.begin() + 5, { 'x', 'y', 'z' }); //"xyz foo, bar and world!"
Replace occurrences of a string with another string
Replace only the ﬁrst occurrence of replace with with in str:
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
User-deﬁned classes may implement the stream insertion operator if desired:
std::ostream operator<<( std::ostream& out, const A& a )
{
    // write a string representation of a to out
    return out;
}
Version ≥ C++11
Aside from streams, since C++11 you can also use the std::to_string (and std::to_wstring) function which is
overloaded for all fundamental types and returns the string representation of its parameter.
std::string s = to_string(0x12f3);  // after this the string s contains "4851"
Section 47.9: Splitting
Use std::string::substr to split a string. There are two variants of this member function.
The ﬁrst takes a starting position from which the returned substring should begin. The starting position must be
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
There are several ways to extract characters from a std::string and each is subtly diﬀerent.
std::string str("Hello world!");
operator[](n)
```

Returns a reference to the character at index n.
```cpp
std::string::operator[] is not bounds-checked and does not throw an exception. The caller is responsible for
asserting that the index is within the range of the string:
char c = str[6]; // 'w'
at(n)
```

Returns a reference to the character at index n.
```cpp
std::string::at is bounds checked, and will throw std::out_of_range if the index is not within the range of the
string:
char c = str.at(7); // 'o'
Version ≥ C++11
```

Note: Both of these examples will result in undeﬁned behavior if the string is empty.
front()
Returns a reference to the ﬁrst character:
char c = str.front(); // 'H'
back()
Returns a reference to the last character:
char c = str.back(); // '!'
Section 47.11: Checking if a string is a preﬁx of another
Version ≥ C++14
In C++14, this is easily done by std::mismatch which returns the ﬁrst mismatching pair from two ranges:
```cpp
std::string prefix = "foo";
std::string string = "foobar";
bool isPrefix = std::mismatch(prefix.begin(), prefix.end(),
    string.begin(), string.end()).first == prefix.end();
Note that a range-and-a-half version of mismatch() existed prior to C++14, but this is unsafe in the case that the
second string is the shorter of the two.
Version < C++14
We can still use the range-and-a-half version of std::mismatch(), but we need to ﬁrst check that the ﬁrst string is at
most as big as the second:
bool isPrefix = prefix.size() <= string.size() &&
    std::mismatch(prefix.begin(), prefix.end(),
        string.begin(), string.end()).first == prefix.end();
Version ≥ C++17
With std::string_view, we can write the direct comparison we want without having to worry about allocation
overhead or making copies:
bool isPrefix(std::string_view prefix, std::string_view full)
{
    return prefix == full.substr(0, prefix.size());
}
Section 47.12: Looping through each character
Version ≥ C++11
std::string supports iterators, and so you can use a ranged based loop to iterate through each character:
std::string str = "Hello World!";
for (auto c : str)
    std::cout << c;
You can use a "traditional" for loop to loop through every character:
std::string str = "Hello World!";
for (std::size_t i = 0; i < str.length(); ++i)
    std::cout << str[i];
Section 47.13: Conversion to integers/ﬂoating point types
A std::string containing a number can be converted into an integer type, or a ﬂoating point type, using
conversion functions.
Note that all of these functions stop parsing the input string as soon as they encounter a non-numeric character, so
"123abc" will be converted into 123.
The std::ato* family of functions converts C-style strings (character arrays) to integer or ﬂoating-point types:
std::string ten = "10";
double num1 = std::atof(ten.c_str());
int num2 = std::atoi(ten.c_str());
long num3 = std::atol(ten.c_str());
Version ≥ C++11
long long num4 = std::atoll(ten.c_str());
However, use of these functions is discouraged because they return 0 if they fail to parse the string. This is bad
because 0 could also be a valid result, if for example the input string was "0", so it is impossible to determine if the
conversion actually failed.
The newer std::sto* family of functions convert std::strings to integer or ﬂoating-point types, and throw
exceptions if they could not parse their input. You should use these functions if possible:
Version ≥ C++11
std::string ten = "10";
int num1 = std::stoi(ten);
long num2 = std::stol(ten);
long long num3 = std::stoll(ten);
float num4 = std::stof(ten);
double num5 = std::stod(ten);
long double num6 = std::stold(ten);
Furthermore, these functions also handle octal and hex strings unlike the std::ato* family. The second parameter
is a pointer to the ﬁrst unconverted character in the input string (not illustrated here), and the third parameter is
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
requires to use a diﬀerent template for wstring_convert when dealing with char16_t:
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
To ﬁnd a character or another string, you can use std::string::find. It returns the position of the ﬁrst character
of the ﬁrst match. If no matches were found, the function returns std::string::npos
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
These functions can allow you to search for characters from the end of the string, as well as ﬁnd the negative case
(ie. characters that are not in the string). Here is an example:
std::string str = "dog dog cat cat";
std::cout << "Found at position: " << str.find_last_of("gzx") << '\n';
Found at position: 6
Note: Be aware that the above functions do not search for substrings, but rather for characters contained in the
search string. In this case, the last occurrence of 'g' was found at position 6 (the other characters weren't found).
```


##### std::vector

A vector is a dynamic array with automatically handled storage. The elements in a vector can be accessed just as
eﬃciently as those in an array with the advantage being that vectors can dynamically change in size.
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
that it can be read as well as modiﬁed (if the vector is not const).
[] and at() diﬀer in that [] is not guaranteed to perform any bounds checking, while at() does. Accessing
elements where index < 0 or index >= size is undeﬁned behavior for [], while at() throws a std::out_of_range
exception.
Note: The examples below use C++11-style initialization for clarity, but the operators can be used with all versions
(unless marked C++11).
Version ≥ C++11
```cpp
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
to elements of vectors are done in constant time. That means accessing to the ﬁrst element of the vector has the
same cost (in time) of accessing the second element, the third element and so on.
For example, consider this loop
for (std::size_t i = 0; i < v.size(); ++i) {
    v[i] = 1;
}
Here we know that the index variable i is always in bounds, so it would be a waste of CPU cycles to check that i is
in bounds for every call to operator[].
The front() and back() member functions allow easy reference access to the ﬁrst and last element of the vector,
respectively. These positions are frequently used, and the special accessors can be more readable than their
alternatives using []:
std::vector<int> v{ 4, 5, 6 }; // In pre-C++11 this is more verbose
int a = v.front();   // a is 4, v.front() is equivalent to v[0]
v.front() = 3;       // v now contains {3, 5, 6}
int b = v.back();    // b is 6, v.back() is equivalent to v[v.size() - 1]
v.back() = 7;        // v now contains {3, 5, 7}
Note: It is undeﬁned behavior to invoke front() or back() on an empty vector. You need to check that the
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
vector out until the vector is empty (using 'empty()') to prevent undeﬁned behavior. Then the sum of the numbers
in the vector is calculated and displayed to the user.
Version ≥ C++11
The data() method returns a pointer to the raw memory used by the std::vector to internally store its elements.
```

This is most often used when passing the vector data to legacy code that expects a C-style array.
```cpp
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
Version ≥ C++11
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
to elements in the vector remain stable and access remains deﬁned unless you add/remove elements at or before
the element in the vector, or you cause the vector capacity to change. This is the same as the rule for invalidating
iterators.
Version ≥ C++11
std::vector<int> v{ 1, 2, 3 };
int* p = v.data() + 1;     // p points to 2
v.insert(v.begin(), 0);    // p is now invalid, accessing *p is a undefined behavior.
p = v.data() + 1;          // p points to 1
v.reserve(10);             // p is now invalid, accessing *p is a undefined behavior.
p = v.data() + 1;          // p points to 1
v.erase(v.begin());        // p is now invalid, accessing *p is a undefined behavior.
Section 49.2: Initializing a std::vector
A std::vector can be initialized in several ways while declaring it:
Version ≥ C++11
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
Version ≥ C++11
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
Version ≥ C++11
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
to be copied or moved to ﬁll the gap, see the note below and std::list.
Deleting all elements in a range:
std::vector<int> v{ 1, 2, 3, 4, 5, 6 };
v.erase(v.begin() + 1, v.begin() + 5);  // v becomes {1, 6}
```

Note: The above methods do not change the capacity of the vector, only the size. See Vector Size and Capacity.
The erase method, which removes a range of elements, is often used as a part of the erase-remove idiom. That is,
ﬁrst std::remove moves some elements to the end of the vector, and then erase chops them oﬀ. This is a relatively
ineﬃcient operation for any indices less than the last index of the vector because all elements after the erased
segments must be relocated to new positions. For speed critical applications that require eﬃcient removal of
arbitrary elements in a container, see std::list.
Deleting elements by value:
```cpp
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
Version ≥ C++11
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
While it is important not to increment it in case of a deletion, you should consider using a diﬀerent method when
then erasing repeatedly in a loop. Consider remove_if for a more eﬃcient way.
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
Version ≥ C++11
shrink_to_fit() frees up unused vector capacity:
v.shrink_to_fit();
```

The shrink_to_fit does not guarantee to really reclaim space, but most current implementations do.
Section 49.4: Iterating Over std::vector
You can iterate over a std::vector in several ways. For each of the following sections, v is deﬁned as follows:
```cpp
std::vector<int> v;
Iterating in the Forward Direction
Version ≥ C++11
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
Version ≥ C++14
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
Though there is no built-in way to use the range based for to reverse iterate; it is relatively simple to ﬁx this. The
range based for uses begin() and end() to get iterators and thus simulating this with a wrapper object can achieve
the results we require.
Version ≥ C++14
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
Version ≥ C++11
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
Version ≥ C++17
as_const extends this to range iteration:
for (auto const& e : std::as_const(v)) {
  std::cout << e << '\n';
}
This is easy to implement in earlier versions of C++:
Version ≥ C++14
template <class T>
constexpr std::add_const_t<T>& as_const(T& t) noexcept {
  return t;
}
A Note on Eﬃciency
Since the class std::vector is basically a class that manages a dynamically allocated contiguous array, the same
principle explained here applies to C++ vectors. Accessing the vector's content by index is much more eﬃcient
when following the row-major order principle. Of course, each access to the vector also puts its management
content into the cache as well, but as has been debated many times (notably here and here), the diﬀerence in
performance for iterating over a std::vector compared to a raw array is negligible. So the same principle of
eﬃciency for raw arrays in C also applies for C++'s std::vector.
Section 49.5: vector<bool>: The Exception To So Many, So
Many Rules
The standard (section 23.3.7) speciﬁes that a specialization of vector<bool> is provided, which optimizes space by
packing the bool values, so that each takes up only one bit. Since bits aren't addressable in C++, this means that
several requirements on vector are not placed on vector<bool>:
The data stored is not required to be contiguous, so a vector<bool> can't be passed to a C API which expects
a bool array.
at(), operator [], and dereferencing of iterators do not return a reference to bool. Rather they return a
proxy object that (imperfectly) simulates a reference to a bool by overloading its assignment operators. As an
example, the following code may not be valid for std::vector<bool>, because dereferencing an iterator
does not return a reference:
Version ≥ C++11
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
Version ≥ C++11
std::vector<char> trad_vect = {true, false, false, false, true, false, true, true};
Bitwise representation:
[0,0,0,0,0,0,0,1], [0,0,0,0,0,0,0,0], [0,0,0,0,0,0,0,0], [0,0,0,0,0,0,0,0],
[0,0,0,0,0,0,0,1], [0,0,0,0,0,0,0,0], [0,0,0,0,0,0,0,1], [0,0,0,0,0,0,0,1]
Specialized std::vector<bool> storing 8 Boolean values:
Version ≥ C++11
std::vector<bool> optimized_vect = {true, false, false, false, true, false, true, true};
Bitwise representation:
[1,0,0,0,1,0,1,1]
Notice in the above example, that in the traditional version of std::vector<bool>, 8 Boolean values take up 8 bytes
of memory, whereas in the optimized version of std::vector<bool>, they only use 1 byte of memory. This is a
signiﬁcant improvement on memory usage. If you need to pass a vector<bool> to an C-style API, you may need to
copy the values to an array, or ﬁnd a better way to use the API, if memory and performance are at risk.
Section 49.6: Inserting Elements
Appending an element at the end of a vector (by copying/moving):
struct Point {
  double x, y;
  Point(double x, double y) : x(x), y(y) {}
};
std::vector<Point> v;
Point p(10.0, 2.0);
v.push_back(p);  // p is copied into the vector.
Version ≥ C++11
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
Version ≥ C++11
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
Version ≥ C++11
std::vector<int> v{ 1, 2, 3 };
int* p = v.data();
In contrast to solutions based on previous C++ standards (see below), the member function .data() may also be
applied to empty vectors, because it doesn't cause undeﬁned behavior in this case.
Before C++11, you would take the address of the vector's ﬁrst element to get an equivalent pointer, if the vector
isn't empty, these both methods are interchangeable:
int* p = &v[0];      // combine subscript operator and 0 literal
int* p = &v.front(); // explicitly reference the first element
```

Note: If the vector is empty, v[0] and v.front() are undeﬁned and cannot be used.
When storing the base address of the vector's data, note that many operations (such as push_back, resize, etc.) can
change the data memory location of the vector, thus invalidating previous data pointers. For example:
```cpp
std::vector<int> v;
int* p = v.data();
v.resize(42);      // internal memory location changed; value of p is now invalid
Section 49.8: Finding an Element in std::vector
The function std::find, deﬁned in the <algorithm> header, can be used to ﬁnd an element in a std::vector.
std::find uses the operator== to compare elements for equality. It returns an iterator to the ﬁrst element in the
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
Version ≥ C++11
std::vector<int> v { 5, 4, 3, 2, 1 };
auto it = std::find(v.begin(), v.end(), 4);
auto index = std::distance(v.begin(), it);
// `it` points to the second element of the vector, `index` is 1
auto missing = std::find(v.begin(), v.end(), 10);
auto index_missing = std::distance(v.begin(), missing);
// `missing` is v.end(), `index_missing` is 5 (ie. size of the vector)
If you need to perform many searches in a large vector, then you may want to consider sorting the vector ﬁrst,
before using the binary_search algorithm.
To ﬁnd the ﬁrst element in a vector that satisﬁes a condition, std::find_if can be used. In addition to the two
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
Version ≥ C++11
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
However, this solution fails if you try to append a vector to itself, because the standard speciﬁes that iterators given
to insert() must not be from the same range as the receiver object's elements.
Version ≥ c++11
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
```

Vectors can be used as a 2D matrix by deﬁning them as a vector of vectors.
A matrix with 3 rows and 4 columns with each cell initialised as 0 can be deﬁned as:
```cpp
std::vector<std::vector<int> > matrix(3, std::vector<int>(4));
Version ≥ C++11
```

The syntax for initializing them using initialiser lists or otherwise are similar to that of a normal vector.
```cpp
  std::vector<std::vector<int>> matrix = { {0,1,2,3},
                                           {4,5,6,7},
                                           {8,9,10,11}
                                         };
Values in such a vector can be accessed similar to a 2D array
int var = matrix[0][2];
```

Iterating over the entire matrix is similar to that of a normal vector but with an extra dimension.
for(int i = 0; i < 3; ++i)
{
    for(int j = 0; j < 4; ++j)
    {
```cpp
        std::cout << matrix[i][j] << std::endl;
    }
}
Version ≥ C++11
for(auto& row: matrix)
{
    for(auto& col : row)
    {
        std::cout << col << std::endl;
    }
}
A vector of vectors is a convenient way to represent a matrix but it's not the most eﬃcient: individual vectors are
scattered around memory and the data structure isn't cache friendly.
Also, in a proper matrix, the length of every row must be the same (this isn't the case for a vector of vectors). The
additional ﬂexibility can be a source of errors.
Section 49.11: Using a Sorted Vector for Fast Element Lookup
```

The <algorithm> header provides a number of useful functions for working with sorted vectors.
An important prerequisite for working with sorted vectors is that the stored values are comparable with <.
An unsorted vector can be sorted by using the function std::sort():
```cpp
std::vector<int> v;
// add some code here to fill v with some elements
std::sort(v.begin(), v.end());
Sorted vectors allow eﬃcient element lookup using the function std::lower_bound(). Unlike std::find(), this
performs an eﬃcient binary search on the vector. The downside is that it only gives valid results for sorted input
ranges:
// search the vector for the first element with value 42
std::vector<int>::iterator it = std::lower_bound(v.begin(), v.end(), 42);
if (it != v.end() && *it == 42) {
    // we found the element!
}
Note: If the requested value is not part of the vector, std::lower_bound() will return an iterator to the ﬁrst element
that is greater than the requested value. This behavior allows us to insert a new element at its right place in an
already sorted vector:
int const new_element = 33;
v.insert(std::lower_bound(v.begin(), v.end(), new_element), new_element);
If you need to insert a lot of elements at once, it might be more eﬃcient to call push_back() for all them ﬁrst and
then call std::sort() once all elements have been inserted. In this case, the increased cost of the sorting can pay
oﬀ against the reduced cost of inserting new elements at the end of the vector and not in the middle.
If your vector contains multiple elements of the same value, std::lower_bound() will try to return an iterator to the
ﬁrst element of the searched value. However, if you need to insert a new element after the last element of the
searched value, you should use the function std::upper_bound() as this will cause less shifting around of
elements:
v.insert(std::upper_bound(v.begin(), v.end(), new_element), new_element);
If you need both the upper bound and the lower bound iterators, you can use the function std::equal_range() to
retrieve both of them eﬃciently with one call:
std::pair<std::vector<int>::iterator,
          std::vector<int>::iterator> rg = std::equal_range(v.begin(), v.end(), 42);
std::vector<int>::iterator lower_bound = rg.first;
std::vector<int>::iterator upper_bound = rg.second;
In order to test whether an element exists in a sorted vector (although not speciﬁc to vectors), you can use the
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
vector was signiﬁcant, then the capacity reduction for the new vector is likely to be signiﬁcant. We can then swap
the original vector with the temporary one to retain its minimized capacity:
std::vector<int>(v).swap(v);
Version ≥ C++11
In C++11 we can use the shrink_to_fit() member function for a similar eﬀect:
v.shrink_to_fit();
```

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
Vector has an implementation-speciﬁc upper limit on its size, but you are likely to run out of RAM before
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
Vector capacity diﬀers from size. While size is simply how many elements the vector currently has, capacity is for
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



***
#### Professional Insights: Algorithms & Sorting

##### Standard Library Algorithms

Section 62.1: std::next_permutation 
Section 62.2: std::for_each 
Section 62.3: std::accumulate 
Section 62.4: std::ﬁnd 
Section 62.5: std::min_element 
Section 62.6: std::ﬁnd_if 
Section 62.7: Using std::nth_element To Find The Median (Or Other Quantiles) 
Section 62.8: std::count 
Section 62.9: std::count_if 

##### Sorting

Section 67.1: Sorting and sequence containers 
Section 67.2: sorting with std::map (ascending and descending) 
Section 67.3: Sorting sequence containers by overloaded less operator 
Section 67.4: Sorting sequence containers using compare function 
Section 67.5: Sorting sequence containers using lambda expressions (C++11) 
Section 67.6: Sorting built-in arrays 
Section 67.7: Sorting sequence containers with specifed ordering 


To master the STL, you must understand what happens under the hood.

#### 3.5.1 The Truth About std::vector
`std::vector` is a dynamic array. It guarantees contiguous memory.

*   **Layout**: Three pointers: `start`, `finish`, `end_of_storage`.
    *   `start`: Points to first element.
    *   `finish`: Points to one-past-the-last active element (size).
    *   `end_of_storage`: Points to end of allocated buffer (capacity).

*   **Growth Strategy**: Geometric growth.
    *   When `size() == capacity()`, a new buffer is allocated (usually 2x or 1.5x larger).
    *   **Elements are MOVED** (or copied) to the new buffer.
    *   Old buffer is deleted.
    *   *Cost*: Amortized O(1) push_back, but worst-case O(N).

*   **Iterator Invalidation**:
    *   **Reallocation**: Invalidates ALL iterators, pointers, and references.
    *   **Insertion/Erasure**: Invalidates iterators at and after the point of operation.

#### 3.5.2 The std::deque Implementation
`std::deque` (Double-Ended Queue) is NOT a contiguous array.

*   **Layout**: A "Map" (dynamic array) of pointers to fixed-size "Chunks" (blocks).
    *   Iterators are smart pointers that know how to jump between chunks.
*   **Performance**:
    *   O(1) random access (double dereference).
    *   O(1) push/pop at BOTH ends (no full reallocation needed, just add a new chunk).
*   **Cache Locality**: Worse than vector, better than list.

#### 3.5.3 Why std::list is (Almost) Always Wrong
`std::list` is a Doubly Linked List.

*   **Layout**: Nodes allocated individually on the heap.
    *   `struct Node { T val; Node* prev; Node* next; }`
*   **The Cache Problem**: Nodes are scattered in memory. Traversing a list causes constant **Cache Misses**.
*   **Benchmark**: Iterating a `vector` is orders of magnitude faster than a `list`, even for large types, due to prefetching.
*   **Use Case**: Only when you need **Reference Stability** (insertions never invalidate references to other elements).

#### 3.5.4 Associative Containers (Map/Set)
`std::map`, `std::set`, `std::multimap`, `std::multiset`.

*   **Implementation**: Balanced Binary Search Tree (usually **Red-Black Tree**).
*   **Node Layout**: `struct Node { T val; Node* left; Node* right; Node* parent; Color color; }`
*   **Complexity**: O(log N) for insert, lookup, delete.
*   **Overhead**: 3 pointers + enum per element (heavy memory overhead).

#### 3.5.5 Unordered Containers (Hash Maps)
`std::unordered_map`, `std::unordered_set`.

*   **Implementation**: Array of "Buckets" (Linked Lists).
    *   Hash function maps Key -> Bucket Index.
    *   Collisions handled by Chaining (linked list in bucket).
*   **Complexity**:
    *   Average: O(1).
    *   Worst Case: O(N) (if all keys hash to same bucket).
*   **Rehashing**: When `load_factor > max_load_factor`, bucket array grows, all elements rehashed.

#### 3.5.6 Iterator Invalidation Cheat Sheet

| Container | Operation | Invalidates |
| :--- | :--- | :--- |
| **Vector** | Capacity Change | **ALL** |
| **Vector** | Insert/Erase | Current & After |
| **Deque** | Insert/Erase (ends) | Iterators only (Refs valid!) |
| **Deque** | Insert/Erase (middle) | **ALL** |
| **List** | Insert/Erase | Only deleted element |
| **Map/Set** | Insert/Erase | Only deleted element |
| **Unordered** | Rehash | **ALL** |
| **Unordered** | Insert (no rehash) | None |

***

## Volume II: The Modern Renaissance
### <a name="chapter-7-c11"></a>CHAPTER 7: C++11 CORE LANGUAGE FEATURES

The C++11 standard (originally known as C++0x) was a revolutionary update that transformed C++ into a modern language. It addressed verbosity, safety, and performance.

***

### 1. AUTO & TYPE DEDUCTION

#### 1.1 The `auto` Keyword

In C++98, you had to explicitly declare types, which could be verbose, especially with iterators. C++11 introduced `auto` to ask the compiler to deduce the type from the initializer.

```cpp
// C++98
int x = 5;
std::vector<int> v;
std::vector<int>::iterator it = v.begin();

// C++11
auto x = 5;       // deduced as int
auto it = v.begin(); // deduced as std::vector<int>::iterator
```

**Key Rules:**
1.  **Must be initialized**: `auto x;` is invalid.
2.  **References**: `auto` strips references. Use `auto&` to keep them.
3.  **Const**: `auto` strips top-level const. Use `const auto` or `const auto&`.

```cpp
int a = 10;
int& ref = a;

auto x = ref;  // x is int (copy), not int&
auto& y = ref; // y is int& (reference)
```

#### 1.2 `decltype`

`decltype` inspects the declared type of an entity or expression. Unlike `auto`, it does not strip references or const.

```cpp
int x = 5;
decltype(x) y = 10; // y is int

const int& z = x;
decltype(z) w = x; // w is const int&
```

***

### 2. RANGE-BASED FOR LOOPS

C++11 introduced a syntactic sugar for iterating over containers, arrays, and initializer lists.

```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};

// C++98
for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it << " ";
}

// C++11 Range-based for
for (int i : vec) {
    std::cout << i << " ";
}

// With auto (Common pattern)
for (auto i : vec) {
    std::cout << i << " ";
}

// By Reference (to modify elements)
for (auto& i : vec) {
    i *= 2;
}

// By Const Reference (read-only, avoids copy)
for (const auto& i : vec) {
    std::cout << i << " ";
}
```

***

### 3. UNIFORM INITIALIZATION

C++11 introduced a consistent syntax for initializing everything using curly braces `{}`.

#### 3.1 Brace Initialization

```cpp
// C++98 inconsistency
int a = 5;
int arr[] = {1, 2};
std::vector<int> v; v.push_back(1); // Tedious

// C++11 Uniformity
int a{5};
int arr[]{1, 2};
std::vector<int> v{1, 2, 3}; // Initializer list!
std::string s{"Hello"};
```

#### 3.2 Preventing Narrowing

Brace initialization forbids "narrowing conversions" where data loss might occur.

```cpp
int x = 3.14;  // C++98: Compiles (x becomes 3), implicit cast
// int y{3.14}; // C++11: Error! Narrowing conversion
```

#### 3.3 `std::initializer_list`

Classes can now take a list of elements in their constructor.

```cpp
#include <initializer_list>

class MyClass {
public:
    MyClass(std::initializer_list<int> list) {
        for (auto elem : list) {
            // process elem
        }
    }
};

MyClass obj = {1, 2, 3, 4, 5};
```

***

### 4. NULLPTR

C++98 used `0` or `NULL` (macro for 0) for null pointers. This caused ambiguity with function overloading.

```cpp
void func(int x) { std::cout << "Integer"; }
void func(char* p) { std::cout << "Pointer"; }

// C++98
func(NULL); // Calls func(int)! Because NULL is 0.

// C++11
func(nullptr); // Calls func(char*).
```

`nullptr` is a keyword of type `std::nullptr_t`. It implicitly converts to any pointer type but *not* to integers (except bool `false`).

***

### 5. STRONGLY TYPED ENUMS

C++98 enums leaked their names into the surrounding scope and implicitly converted to integers. C++11 `enum class` fixes this.

```cpp
// C++98
enum Color { RED, GREEN, BLUE };
int r = RED; // Implicit conversion allowed

// C++11
enum class TrafficLight { RED, YELLOW, GREEN };

// TrafficLight t = RED; // Error: RED not in scope
TrafficLight t = TrafficLight::RED;
// int x = TrafficLight::RED; // Error: No implicit conversion

// Explicit underlying type
enum class Byte : unsigned char { A, B, C };
```

***

### 6. OTHER CORE FEATURES

-   **`constexpr`**: Allows functions and variables to be evaluated at compile-time (limited in C++11, expanded later).
-   **`static_assert`**: Compile-time assertion checking.
```cpp
    static_assert(sizeof(int) == 4, "Int must be 4 bytes");
```
-   **Delegating Constructors**: One constructor can call another.
-   **`default` and `delete` functions**:
```cpp
    class NonCopyable {
        NonCopyable(const NonCopyable&) = delete; // Ban copying
        NonCopyable() = default; // Explicitly request default ctor
    };
```
-   **`override` and `final`**: Virtual function controls (See Chapter 19).
### <a name="chapter-8-c11"></a>CHAPTER 8: C++11 SMART POINTERS & MEMORY MANAGEMENT


***
#### Professional Insights: Smart Pointers Mastery

##### Smart Pointers

Section 33.1: Unique ownership (std::unique_ptr)
Version ≥ C++11
A std::unique_ptr is a class template that manages the lifetime of a dynamically stored object. Unlike for
```cpp
std::shared_ptr, the dynamic object is owned by only one instance of a std::unique_ptr at any time,
// Creates a dynamic int with value of 20 owned by a unique pointer
std::unique_ptr<int> ptr = std::make_unique<int>(20);
(Note: std::unique_ptr is available since C++11 and std::make_unique since C++14.)
Only the variable ptr holds a pointer to a dynamically allocated int. When a unique pointer that owns an object
goes out of scope, the owned object is deleted, i.e. its destructor is called if the object is of class type, and the
memory for that object is released.
To use std::unique_ptr and std::make_unique with array-types, use their array specializations:
// Creates a unique_ptr to an int with value 59
std::unique_ptr<int> ptr = std::make_unique<int>(59);
// Creates a unique_ptr to an array of 15 ints
std::unique_ptr<int[]> ptr = std::make_unique<int[]>(15);
You can access the std::unique_ptr just like a raw pointer, because it overloads those operators.
You can transfer ownership of the contents of a smart pointer to another pointer by using std::move, which will
cause the original smart pointer to point to nullptr.
// 1. std::unique_ptr
std::unique_ptr<int> ptr = std::make_unique<int>();
// Change value to 1
*ptr = 1;
// 2. std::unique_ptr (by moving 'ptr' to 'ptr2', 'ptr' doesn't own the object anymore)
std::unique_ptr<int> ptr2 = std::move(ptr);
int a = *ptr2; // 'a' is 1
int b = *ptr;  // undefined behavior! 'ptr' is 'nullptr'
               // (because of the move command above)
Passing unique_ptr to functions as parameter:
void foo(std::unique_ptr<int> ptr)
{
    // Your code goes here
}
std::unique_ptr<int> ptr = std::make_unique<int>(59);
foo(std::move(ptr))
Returning unique_ptr from functions. This is the preferred C++11 way of writing factory functions, as it clearly
conveys the ownership semantics of the return: the caller owns the resulting unique_ptr and is responsible for it.
std::unique_ptr<int> foo()
{
    std::unique_ptr<int> ptr = std::make_unique<int>(59);
    return ptr;
}
std::unique_ptr<int> ptr = foo();
Compare this to:
int* foo_cpp03();
int* p = foo_cpp03(); // do I own p? do I have to delete it at some point?
                      // it's not readily apparent what the answer is.
Version < C++14
The class template make_unique is provided since C++14. It's easy to add it manually to C++11 code:
template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(Args&&... args)
{ return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }
// Use make_unique for arrays
template<typename T>
typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(size_t n)
{ return std::unique_ptr<T>(new typename std::remove_extent<T>::type[n]()); }
Version ≥ C++11
Unlike the dumb smart pointer (std::auto_ptr), unique_ptr can also be instantiated with vector allocation (not
std::vector). Earlier examples were for scalar allocations. For example to have a dynamically allocated integer
array for 10 elements, you would specify int[] as the template type (and not just int):
std::unique_ptr<int[]> arr_ptr = std::make_unique<int[]>(10);
Which can be simpliﬁed with:
auto arr_ptr = std::make_unique<int[]>(10);
Now, you use arr_ptr as if it is an array:
arr_ptr[2] =  10; // Modify third element
You need not to worry about de-allocation. This template specialized version calls constructors and destructors
appropriately. Using vectored version of unique_ptr or a vector itself - is a personal choice.
In versions prior to C++11, std::auto_ptr was available. Unlike unique_ptr it is allowed to copy auto_ptrs, upon
which the source ptr will lose the ownership of the contained pointer and the target receives it.
Section 33.2: Sharing ownership (std::shared_ptr)
The class template std::shared_ptr deﬁnes a shared pointer that is able to share ownership of an object with
other shared pointers. This contrasts to std::unique_ptr which represents exclusive ownership.
The sharing behavior is implemented through a technique known as reference counting, where the number of
shared pointers that point to the object is stored alongside it. When this count reaches zero, either through the
destruction or reassignment of the last std::shared_ptr instance, the object is automatically destroyed.
// Creation: 'firstShared' is a shared pointer for a new instance of 'Foo'
std::shared_ptr<Foo> firstShared = std::make_shared<Foo>(/*args*/);
To create multiple smart pointers that share the same object, we need to create another shared_ptr that aliases
the ﬁrst shared pointer. Here are 2 ways of doing it:
std::shared_ptr<Foo> secondShared(firstShared);  // 1st way: Copy constructing
std::shared_ptr<Foo> secondShared;
secondShared = firstShared;                      // 2nd way: Assigning
Either of the above ways makes secondShared a shared pointer that shares ownership of our instance of Foo with
firstShared.
The smart pointer works just like a raw pointer. This means, you can use * to dereference them. The regular ->
operator works as well:
secondShared->test(); // Calls Foo::test()
```

Finally, when the last aliased shared_ptr goes out of scope, the destructor of our Foo instance is called.
Warning: Constructing a shared_ptr might throw a bad_alloc exception when extra data for shared ownership
semantics needs to be allocated. If the constructor is passed a regular pointer it assumes to own the object pointed
to and calls the deleter if an exception is thrown. This means shared_ptr<T>(new T(args)) will not leak a T object if
allocation of shared_ptr<T> fails. However, it is advisable to use make_shared<T>(args) or
allocate_shared<T>(alloc, args), which enable the implementation to optimize the memory allocation.
Allocating Arrays([]) using shared_ptr
Version ≥ C++11 Version < C++17
Unfortunately, there is no direct way to allocate Arrays using make_shared<>.
It is possible to create arrays for shared_ptr<> using new and std::default_delete.
For example, to allocate an array of 10 integers, we can write the code as
shared_ptr<int> sh(new int[10], std::default_delete<int[]>());
Specifying std::default_delete is mandatory here to make sure that the allocated memory is correctly cleaned up
using delete[].
If we know the size at compile time, we can do it this way:
```cpp
template<class Arr>
struct shared_array_maker {};
template<class T, std::size_t N>
struct shared_array_maker<T[N]> {
  std::shared_ptr<T> operator()const{
    auto r = std::make_shared<std::array<T,N>>();
    if (!r) return {};
    return {r.data(), r};
  }
};
template<class Arr>
auto make_shared_array()
-> decltype( shared_array_maker<Arr>{}() )
{ return shared_array_maker<Arr>{}(); }
then make_shared_array<int[10]> returns a shared_ptr<int> pointing to 10 ints all default constructed.
Version ≥ C++17
With C++17, shared_ptr gained special support for array types. It is no longer necessary to specify the array-deleter
explicitly, and the shared pointer can be dereferenced using the [] array index operator:
std::shared_ptr<int[]> sh(new int[10]);
sh[0] = 42;
Shared pointers can point to a sub-object of the object it owns:
struct Foo { int x; };
std::shared_ptr<Foo> p1 = std::make_shared<Foo>();
std::shared_ptr<int> p2(p1, &p1->x);
Both p2 and p1 own the object of type Foo, but p2 points to its int member x. This means that if p1 goes out of
scope or is reassigned, the underlying Foo object will still be alive, ensuring that p2 does not dangle.
Important: A shared_ptr only knows about itself and all other shared_ptr that were created with the alias
constructor. It does not know about any other pointers, including all other shared_ptrs created with a reference to
the same Foo instance:
Foo *foo = new Foo;
std::shared_ptr<Foo> shared1(foo);
std::shared_ptr<Foo> shared2(foo); // don't do this
shared1.reset(); // this will delete foo, since shared1
                 // was the only shared_ptr that owned it
shared2->test(); // UNDEFINED BEHAVIOR: shared2's foo has been
                 // deleted already!!
Ownership Transfer of shared_ptr
By default, shared_ptr increments the reference count and doesn't transfer the ownership. However, it can be
made to transfer the ownership using std::move:
shared_ptr<int> up = make_shared<int>();
// Transferring the ownership
shared_ptr<int> up2 = move(up);
// At this point, the reference count of up = 0 and the
// ownership of the pointer is solely with up2 with reference count = 1
Section 33.3: Sharing with temporary ownership
(std::weak_ptr)
Instances of std::weak_ptr can point to objects owned by instances of std::shared_ptr while only becoming
temporary owners themselves. This means that weak pointers do not alter the object's reference count and
therefore do not prevent an object's deletion if all of the object's shared pointers are reassigned or destroyed.
In the following example instances of std::weak_ptr are used so that the destruction of a tree object is not
inhibited:
#include <memory>
#include <vector>
struct TreeNode {
    std::weak_ptr<TreeNode> parent;
    std::vector< std::shared_ptr<TreeNode> > children;
};
int main() {
    // Create a TreeNode to serve as the root/parent.
    std::shared_ptr<TreeNode> root(new TreeNode);
    // Give the parent 100 child nodes.
    for (size_t i = 0; i < 100; ++i) {
        std::shared_ptr<TreeNode> child(new TreeNode);
        root->children.push_back(child);
        child->parent = root;
    }
    // Reset the root shared pointer, destroying the root object, and
    // subsequently its child nodes.
    root.reset();
}
As child nodes are added to the root node's children, their std::weak_ptr member parent is set to the root node.
The member parent is declared as a weak pointer as opposed to a shared pointer such that the root node's
reference count is not incremented. When the root node is reset at the end of main(), the root is destroyed. Since
the only remaining std::shared_ptr references to the child nodes were contained in the root's collection children,
all child nodes are subsequently destroyed as well.
Due to control block implementation details, shared_ptr allocated memory may not be released until shared_ptr
reference counter and weak_ptr reference counter both reach zero.
#include <memory>
int main()
{
    {
         std::weak_ptr<int> wk;
         {
             // std::make_shared is optimized by allocating only once
             // while std::shared_ptr<int>(new int(42)) allocates twice.
             // Drawback of std::make_shared is that control block is tied to our integer
             std::shared_ptr<int> sh = std::make_shared<int>(42);
             wk = sh;
             // sh memory should be released at this point...
         }
         // ... but wk is still alive and needs access to control block
     }
     // now memory is released (sh and wk)
}
Since std::weak_ptr does not keep its referenced object alive, direct data access through a std::weak_ptr is not
possible. Instead it provides a lock() member function that attempts to retrieve a std::shared_ptr to the
referenced object:
#include <cassert>
#include <memory>
int main()
{
    {
         std::weak_ptr<int> wk;
         std::shared_ptr<int> sp;
         {
             std::shared_ptr<int> sh = std::make_shared<int>(42);
             wk = sh;
             // calling lock will create a shared_ptr to the object referenced by wk
             sp = wk.lock();
             // sh will be destroyed after this point, but sp is still alive
         }
         // sp still keeps the data alive.
         // At this point we could even call lock() again
         // to retrieve another shared_ptr to the same data from wk
         assert(*sp == 42);
         assert(!wk.expired());
         // resetting sp will delete the data,
         // as it is currently the last shared_ptr with ownership
         sp.reset();
         // attempting to lock wk now will return an empty shared_ptr,
         // as the data has already been deleted
         sp = wk.lock();
         assert(!sp);
         assert(wk.expired());
     }
}
Section 33.4: Using custom deleters to create a wrapper to a
C interface
Many C interfaces such as SDL2 have their own deletion functions. This means that you cannot use smart pointers
directly:
std::unique_ptr<SDL_Surface> a; // won't work, UNSAFE!
Instead, you need to deﬁne your own deleter. The examples here use the SDL_Surface structure which should be
freed using the SDL_FreeSurface() function, but they should be adaptable to many other C interfaces.
The deleter must be callable with a pointer argument, and therefore can be e.g. a simple function pointer:
std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> a(pointer, SDL_FreeSurface);
Any other callable object will work, too, for example a class with an operator():
struct SurfaceDeleter {
    void operator()(SDL_Surface* surf) {
        SDL_FreeSurface(surf);
    }
};
std::unique_ptr<SDL_Surface, SurfaceDeleter> a(pointer, SurfaceDeleter{}); // safe
std::unique_ptr<SDL_Surface, SurfaceDeleter> b(pointer); // equivalent to the above
                                                         // as the deleter is value-initialized
This not only provides you with safe, zero overhead (if you use unique_ptr) automatic memory management, you
also get exception safety.
Note that the deleter is part of the type for unique_ptr, and the implementation can use the empty base
optimization to avoid any change in size for empty custom deleters. So while std::unique_ptr<SDL_Surface,
SurfaceDeleter> and std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> solve the same problem in a
similar way, the former type is still only the size of a pointer while the latter type has to hold two pointers: both the
SDL_Surface* and the function pointer! When having free function custom deleters, it is preferable to wrap the
function in an empty type.
In cases where reference counting is important, one could use a shared_ptr instead of an unique_ptr. The
shared_ptr always stores a deleter, this erases the type of the deleter, which might be useful in APIs. The
disadvantages of using shared_ptr over unique_ptr include a higher memory cost for storing the deleter and a
performance cost for maintaining the reference count.
// deleter required at construction time and is part of the type
std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> a(pointer, SDL_FreeSurface);
// deleter is only required at construction time, not part of the type
std::shared_ptr<SDL_Surface> b(pointer, SDL_FreeSurface);
Version ≥ C++17
With template auto, we can make it even easier to wrap our custom deleters:
template <auto DeleteFn>
struct FunctionDeleter {
    template <class T>
    void operator()(T* ptr) {
        DeleteFn(ptr);
    }
};
template <class T, auto DeleteFn>
using unique_ptr_deleter = std::unique_ptr<T, FunctionDeleter<DeleteFn>>;
With which the above example is simply:
unique_ptr_deleter<SDL_Surface, SDL_FreeSurface> c(pointer);
Here, the purpose of auto is to handle all free functions, whether they return void (e.g. SDL_FreeSurface) or not
(e.g. fclose).
Section 33.5: Unique ownership without move semantics
(auto_ptr)
Version < C++11
NOTE: std::auto_ptr has been deprecated in C++11 and will be removed in C++17. You should only use this if you
are forced to use C++03 or earlier and are willing to be careful. It is recommended to move to unique_ptr in
combination with std::move to replace std::auto_ptr behavior.
Before we had std::unique_ptr, before we had move semantics, we had std::auto_ptr. std::auto_ptr provides
unique ownership but transfers ownership upon copy.
As with all smart pointers, std::auto_ptr automatically cleans up resources (see RAII):
{
    std::auto_ptr<int> p(new int(42));
    std::cout << *p;
} // p is deleted here, no memory leaked
but allows only one owner:
std::auto_ptr<X> px = ...;
std::auto_ptr<X> py = px;
  // px is now empty
This allows to use std::auto_ptr to keep ownership explicit and unique at the danger of losing ownership
unintended:
void f(std::auto_ptr<X> ) {
    // assumes ownership of X
    // deletes it at end of scope
};
std::auto_ptr<X> px = ...;
f(px); // f acquires ownership of underlying X
       // px is now empty
px->foo(); // NPE!
// px.~auto_ptr() does NOT delete
The transfer of ownership happened in the "copy" constructor. auto_ptr's copy constructor and copy assignment
operator take their operands by non-const reference so that they could be modiﬁed. An example implementation
might be:
template <typename T>
class auto_ptr {
    T* ptr;
public:
    auto_ptr(auto_ptr& rhs)
    : ptr(rhs.release())
    { }
    auto_ptr& operator=(auto_ptr& rhs) {
        reset(rhs.release());
        return *this;
    }
    T* release() {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }
    void reset(T* tmp = nullptr) {
        if (ptr != tmp) {
            delete ptr;
            ptr = tmp;
        }
    }
    /* other functions ... */
};
This breaks copy semantics, which require that copying an object leaves you with two equivalent versions of it. For
any copyable type, T, I should be able to write:
T a = ...;
T b(a);
assert(b == a);
```

But for auto_ptr, this is not the case. As a result, it is not safe to put auto_ptrs in containers.
Section 33.6: Casting std::shared_ptr pointers
It is not possible to directly use static_cast, const_cast, dynamic_cast and reinterpret_cast on
```cpp
std::shared_ptr to retrieve a pointer sharing ownership with the pointer being passed as argument. Instead, the
functions std::static_pointer_cast, std::const_pointer_cast, std::dynamic_pointer_cast and
std::reinterpret_pointer_cast should be used:
struct Base { virtual ~Base() noexcept {}; };
struct Derived: Base {};
auto derivedPtr(std::make_shared<Derived>());
auto basePtr(std::static_pointer_cast<Base>(derivedPtr));
auto constBasePtr(std::const_pointer_cast<Base const>(basePtr));
auto constDerivedPtr(std::dynamic_pointer_cast<Derived const>(constBasePtr));
Note that std::reinterpret_pointer_cast is not available in C++11 and C++14, as it was only proposed by N3920
and adopted into Library Fundamentals TS in February 2014. However, it can be implemented as follows:
template <typename To, typename From>
inline std::shared_ptr<To> reinterpret_pointer_cast(
    std::shared_ptr<From> const & ptr) noexcept
{ return std::shared_ptr<To>(ptr, reinterpret_cast<To *>(ptr.get())); }
Section 33.7: Writing a smart pointer: value_ptr
A value_ptr is a smart pointer that behaves like a value. When copied, it copies its contents. When created, it
creates its contents.
// Like std::default_delete:
template<class T>
struct default_copier {
  // a copier must handle a null T const* in and return null:
  T* operator()(T const* tin)const {
    if (!tin) return nullptr;
    return new T(*tin);
  }
  void operator()(void* dest, T const* tin)const {
    if (!tin) return;
    return new(dest) T(*tin);
  }
};
// tag class to handle empty case:
struct empty_ptr_t {};
constexpr empty_ptr_t empty_ptr{};
// the value pointer type itself:
template<class T, class Copier=default_copier<T>, class Deleter=std::default_delete<T>,
  class Base=std::unique_ptr<T, Deleter>
>
struct value_ptr:Base, private Copier {
  using copier_type=Copier;
  // also typedefs from unique_ptr
  using Base::Base;
  value_ptr( T const& t ):
    Base( std::make_unique<T>(t) ),
    Copier()
  {}
  value_ptr( T && t ):
    Base( std::make_unique<T>(std::move(t)) ),
    Copier()
  {}
  // almost-never-empty:
      value_ptr():
    Base( std::make_unique<T>() ),
    Copier()
  {}
  value_ptr( empty_ptr_t ) {}
  value_ptr( Base b, Copier c={} ):
    Base(std::move(b)),
    Copier(std::move(c))
  {}
  Copier const& get_copier() const {
    return *this;
  }
  value_ptr clone() const {
    return {
      Base(
        get_copier()(this->get()),
        this->get_deleter()
      ),
      get_copier()
    };
  }
  value_ptr(value_ptr&&)=default;
  value_ptr& operator=(value_ptr&&)=default;
  value_ptr(value_ptr const& o):value_ptr(o.clone()) {}
  value_ptr& operator=(value_ptr const&o) {
    if (o && *this) {
      // if we are both non-null, assign contents:
      **this = *o;
    } else {
      // otherwise, assign a clone (which could itself be null):
      *this = o.clone();
    }
    return *this;
  }
  value_ptr& operator=( T const& t ) {
    if (*this) {
      **this = t;
    } else {
      *this = value_ptr(t);
    }
    return *this;
  }
  value_ptr& operator=( T && t ) {
    if (*this) {
      **this = std::move(t);
    } else {
      *this = value_ptr(std::move(t));
    }
    return *this;
  }
  T& get() { return **this; }
  T const& get() const { return **this; }
  T* get_pointer() {
    if (!*this) return nullptr;
    return std::addressof(get());
  }
  T const* get_pointer() const {
    if (!*this) return nullptr;
    return std::addressof(get());
  }
  // operator-> from unique_ptr
};
template<class T, class...Args>
value_ptr<T> make_value_ptr( Args&&... args ) {
  return {std::make_unique<T>(std::forward<Args>(args)...)};
}
This particular value_ptr is only empty if you construct it with empty_ptr_t or if you move from it. It exposes the fact
it is a unique_ptr, so explicit operator bool() const works on it. .get() has been changed to return a
reference (as it is almost never empty), and .get_pointer() returns a pointer instead.
This smart pointer can be useful for pImpl cases, where we want value-semantics but we also don't want to expose
the contents of the pImpl outside of the implementation ﬁle.
With a non-default Copier, it can even handle virtual base classes that know how to produce instances of their
derived and turn them into value-types.
Section 33.8: Getting a shared_ptr referring to this
enable_shared_from_this enables you to get a valid shared_ptr instance to this.
By deriving your class from the class template enable_shared_from_this, you inherit a method shared_from_this
that returns a shared_ptr instance to this.
Note that the object must be created as a shared_ptr in ﬁrst place:
#include <memory>
class A: public enable_shared_from_this<A> {
};
A* ap1 =new A();
shared_ptr<A> ap2(ap1); // First prepare a shared pointer to the object and hold it!
// Then get a shared pointer to the object from the object itself
shared_ptr<A> ap3 = ap1->shared_from_this();
int c3 =ap3.use_count(); // =2: pointing to the same object
```

Note(2) you cannot call enable_shared_from_this inside the constructor.
```cpp
#include <memory> // enable_shared_from_this
class Widget : public std::enable_shared_from_this< Widget >
{
public:
    void DoSomething()
    {
        std::shared_ptr< Widget > self = shared_from_this();
        someEvent -> Register( self );
    }
private:
};
int main()
{
    auto w = std::make_shared< Widget >();
    w -> DoSomething();
}
If you use shared_from_this() on an object not owned by a shared_ptr, such as a local automatic object or a
global object, then the behavior is undeﬁned. Since C++17 it throws std::bad_alloc instead.
Using shared_from_this() from a constructor is equivalent to using it on an object not owned by a shared_ptr,
because the objects is possessed by the shared_ptr after the constructor returns.


C++11 revolutionized memory management by introducing smart pointers, which strictly define ownership semantics and automate memory reclamation, effectively making `new` and `delete` unnecessary in user code.

***
```


### 1. THE PROBLEM WITH RAW POINTERS

In C++98, dynamic memory required manual management:
1.  **Memory Leaks**: Forgetting `delete`.
2.  **Dangling Pointers**: Accessing deleted memory.
3.  **Double Free**: Deleting the same memory twice.
4.  **Exception Safety**: If an exception throws before `delete`, memory leaks.

Smart pointers solve these by using **RAII (Resource Acquisition Is Initialization)**.

***

### 2. UNIQUE_PTR (Exclusive Ownership)

`std::unique_ptr` represents exclusive ownership. An object can have only one `unique_ptr` pointing to it. When the `unique_ptr` is destroyed, the object is deleted.

#### 2.1 Basic Usage

```cpp
#include <memory>

void func() {
    std::unique_ptr<int> ptr(new int(10));
    // or better: auto ptr = std::make_unique<int>(10); (C++14)
    
    *ptr = 20;
    // No delete needed. Memory freed when ptr goes out of scope.
}
```

#### 2.2 Move Only

You cannot copy a `unique_ptr`. You must **move** it. This ensures uniqueness.

```cpp
std::unique_ptr<int> p1(new int(5));
// std::unique_ptr<int> p2 = p1; // Error! Copy deleted.

std::unique_ptr<int> p2 = std::move(p1); // OK. p1 is now empty/null.
```

#### 2.3 Custom Deleters

Useful for managing C-style resources (files, sockets).

```cpp
auto deleter = [](FILE* f) { fclose(f); };
std::unique_ptr<FILE, decltype(deleter)> file(fopen("test.txt", "r"), deleter);
```

***

### 3. SHARED_PTR (Shared Ownership)

`std::shared_ptr` allows multiple pointers to own the same resource. The resource is deleted only when the *last* `shared_ptr` is destroyed.

#### 3.1 Reference Counting

It maintains a "control block" with a reference count.

```cpp
auto p1 = std::make_shared<int>(100); // Ref count = 1
{
    auto p2 = p1; // Copy allowed. Ref count = 2
} // p2 destroyed. Ref count = 1

// p1 destroyed. Ref count = 0. Memory freed.
```

#### 3.2 Performance Cost

`shared_ptr` is heavier than `unique_ptr` (2x size usually, plus atomic ref-count increment/decrement overhead). Use only when ownership is truly shared.

***

### 4. WEAK_PTR (Non-Owning Reference)

`std::weak_ptr` observes a `shared_ptr` without keeping it alive. It breaks **circular references**.

#### 4.1 Circular Reference Problem

If A has a `shared_ptr` to B, and B has a `shared_ptr` to A, the reference count never drops to zero.

#### 4.2 Using weak_ptr

```cpp
struct B;
struct A {
    std::shared_ptr<B> b_ptr;
};
struct B {
    std::weak_ptr<A> a_ptr; // Use weak_ptr back to A
};
```

To use a `weak_ptr`, you must convert it to `shared_ptr` via `.lock()`.

```cpp
if (auto shared = weak.lock()) {
    // safe to use shared
} else {
    // object died
}
```

***

### 5. BEST PRACTICES

1.  **Prefer `unique_ptr`** by default. It has zero overhead.
2.  **Use `make_unique`** (C++14) and **`make_shared`**. They are cleaner and exception-safe. `make_shared` is also more efficient (allocates object and control block in one chunk).
3.  **Avoid `new` and `delete`**.
### <a name="chapter-9-c11"></a>CHAPTER 9: C++11 MOVE SEMANTICS


***
#### Professional Insights: Move Semantics & Value Categories

##### Value Categories

Section 74.1: Value Category Meanings
Expressions in C++ are assigned a particular value category, based on the result of those expressions. Value
categories for expressions can aﬀect C++ function overload resolution.
Value categories determines two important-but-separate properties about an expression. One property is whether
the expression has identity. An expression has identity if it refers to an object that has a variable name. The variable
name may not be involved in the expression, but the object can still have one.
The other property is whether it is legal to implicitly move from the expression's value. Or more speciﬁcally,
whether the expression, when used as a function parameter, will bind to r-value parameter types or not.
C++ deﬁnes 3 value categories which represent the useful combination of these properties: lvalue (expressions with
identity but not movable from), xvalue (expressions with identity that are moveable from), and prvalue (expressions
without identity that are moveable from). C++ does not have expressions which have no identity and cannot be
moved from.
C++ deﬁnes two other value categories, each based solely on one of these properties: glvalue (expressions with
identity) and rvalue (expressions that can be moved from). These act as useful groupings of the prior categories.
This graph serves as an illustration:
Section 74.2: rvalue
An rvalue expression is any expression which can be implicitly moved from, regardless of whether it has identity.
More precisely, rvalue expressions may be used as the argument to a function that takes a parameter of type T &&
(where T is the type of expr). Only rvalue expressions may be given as arguments to such function parameters; if a
non-rvalue expression is used, then overload resolution will pick any function that does not use an rvalue reference
parameter. And if none exist, then you get an error.
The category of rvalue expressions includes all xvalue and prvalue expressions, and only those expressions.
The standard library function std::move exists to explicitly transform a non-rvalue expression into an rvalue. More
speciﬁcally, it turns the expression into an xvalue, since even if it was an identity-less prvalue expression before, by
passing it as a parameter to std::move, it gains identity (the function's parameter name) and becomes an xvalue.
Consider the following:
```cpp
std::string str("init");                       //1
std::string test1(str);                        //2
std::string test2(std::move(str));             //3
str = std::string("new value");                //4
std::string &&str_ref = std::move(str);        //5
std::string test3(str_ref);                    //6
std::string has a constructor which takes a single parameter of type std::string&&, commonly called a "move
constructor". However, the value category of the expression str is not an rvalue (speciﬁcally it is an lvalue), so it
cannot call that constructor overload. Instead, it calls the const std::string& overload, the copy constructor.
Line 3 changes things. The return value of std::move is a T&&, where T is the base type of the parameter passed in.
So std::move(str) returns std::string&&. A function call who's return value is an rvalue reference is an rvalue
expression (speciﬁcally an xvalue), so it may call the move constructor of std::string. After line 3, str has been
moved from (who's contents are now undeﬁned).
Line 4 passes a temporary to the assignment operator of std::string. This has an overload which takes a
std::string&&. The expression std::string("new value") is an rvalue expression (speciﬁcally a prvalue), so it
may call that overload. Thus, the temporary is moved into str, replacing the undeﬁned contents with speciﬁc
contents.
Line 5 creates a named rvalue reference called str_ref that refers to str. This is where value categories get
confusing.
See, while str_ref is an rvalue reference to std::string, the value category of the expression str_ref is not an
rvalue. It is an lvalue expression. Yes, really. Because of this, one cannot call the move constructor of std::string
with the expression str_ref. Line 6 therefore copies the value of str into test3.
To move it, we would have to employ std::move again.
Section 74.3: xvalue
An xvalue (eXpiring value) expression is an expression which has identity and represents an object which can be
implicitly moved from. The general idea with xvalue expressions is that the object they represent is going to be
destroyed soon (hence the "eXpiring" part), and therefore implicitly moving from them is ﬁne.
Given:
struct X { int n; };
extern X x;
4;                   // prvalue: does not have an identity
x;                   // lvalue
x.n;                 // lvalue
std::move(x);        // xvalue
std::forward<X&>(x); // lvalue
X{4};                // prvalue: does not have an identity
X{4}.n;              // xvalue: does have an identity and denotes resources
                     // that can be reused
Section 74.4: prvalue
A prvalue (pure-rvalue) expression is an expression which lacks identity, whose evaluation is typically used to
initialize an object, and which can be implicitly moved from. These include, but are not limited to:
Expressions that represent temporary objects, such as std::string("123").
A function call expression that does not return a reference
A literal (except a string literal - those are lvalues), such has 1, true, 0.5f, or 'a'
A lambda expression
```

The built-in addressof operator (&) cannot be applied on these expressions.
Section 74.5: lvalue
An lvalue expression is an expression which has identity, but cannot be implicitly moved from. Among these are
expressions that consist of a variable name, function name, expressions that are built-in dereference operator uses
and expressions that refer to lvalue references.
The typical lvalue is simply a name, but lvalues can come in other ﬂavors as well:
struct X { ... };
X x;         // x is an lvalue
X* px = &x;  // px is an lvalue
*px = X{};   // *px is also an lvalue, X{} is a prvalue
X* foo_ptr();  // foo_ptr() is a prvalue
X& foo_ref();  // foo_ref() is an lvalue
Additionally, while most literals (e.g. 4, 'x', etc.) are prvalues, string literals are lvalues.
Section 74.6: glvalue
A glvalue (a "generalized lvalue") expression is any expression which has identity, regardless of whether it can be
moved from or not. This category includes lvalues (expressions that have identity but can't be moved from) and
xvalues (expressions that have identity, and can be moved from), but excludes prvalues (expressions without
identity).
If an expression has a name, it's a glvalue:
struct X { int n; };
X foo();
X x;
x; // has a name, so it's a glvalue
std::move(x); // has a name (we're moving from "x"), so it's a glvalue
              // can be moved from, so it's an xvalue not an lvalue
foo(); // has no name, so is a prvalue, not a glvalue
X{};   // temporary has no name, so is a prvalue, not a glvalue
X{}.n; // HAS a name, so is a glvalue. can be moved from, so it's an xvalue

##### Move Semantics

Section 106.1: Move semantics
Move semantics are a way of moving one object to another in C++. For this, we empty the old object and place
everything it had in the new object.
For this, we must understand what an rvalue reference is. An rvalue reference (T&& where T is the object type) is not
much diﬀerent than a normal reference (T&, now called lvalue references). But they act as 2 diﬀerent types, and so,
we can make constructors or functions that take one type or the other, which will be necessary when dealing with
move semantics.
The reason why we need two diﬀerent types is to specify two diﬀerent behaviors. Lvalue reference constructors are
related to copying, while rvalue reference constructors are related to moving.
To move an object, we will use std::move(obj). This function returns an rvalue reference to the object, so that we
can steal the data from that object into a new one. There are several ways of doing this which are discussed below.
Important to note is that the use of std::move creates just an rvalue reference. In other words the statement
std::move(obj) does not change the content of obj, while auto obj2 = std::move(obj) (possibly) does.
Section 106.2: Using std::move to reduce complexity from
O(n²) to O(n)
C++11 introduced core language and standard library support for moving an object. The idea is that when an
object o is a temporary and one wants a logical copy, then its safe to just pilfer o's resources, such as a dynamically
allocated buﬀer, leaving o logically empty but still destructible and copyable.
The core language support is mainly
the rvalue reference type builder &&, e.g., std::string&& is an rvalue reference to a std::string, indicating
that that referred to object is a temporary whose resources can just be pilfered (i.e. moved)
special support for a move constructor T( T&& ), which is supposed to eﬃciently move resources from the
speciﬁed other object, instead of actually copying those resources, and
special support for a move assignment operator auto operator=(T&&) -> T&, which also is supposed to
move from the source.
The standard library support is mainly the std::move function template from the <utility> header. This function
produces an rvalue reference to the speciﬁed object, indicating that it can be moved from, just as if it were a
temporary.
For a container actual copying is typically of O(n) complexity, where n is the number of items in the container, while
moving is O(1), constant time. And for an algorithm that logically copies that container n times, this can reduce the
complexity from the usually impractical O(n²) to just linear O(n).
In his article “Containers That Never Change” in Dr. Dobbs Journal in September 19 2013, Andrew Koenig presented
an interesting example of algorithmic ineﬃciency when using a style of programming where variables are
immutable after initialization. With this style loops are generally expressed using recursion. And for some
algorithms such as generating a Collatz sequence, the recursion requires logically copying a container:
// Based on an example by Andrew Koenig in his Dr. Dobbs Journal article
// “Containers That Never Change” September 19, 2013, available at
// <url: http://www.drdobbs.com/cpp/containters-that-never-change/240161543>
// Includes here, e.g. <vector>
namespace my {
```cpp
    template< class Item >
    using Vector_ = /* E.g. std::vector<Item> */;
    auto concat( Vector_<int> const& v, int const x )
        -> Vector_<int>
    {
        auto result{ v };
        result.push_back( x );
        return result;
    }
    auto collatz_aux( int const n, Vector_<int> const& result )
        -> Vector_<int>
    {
        if( n == 1 )
        {
            return result;
        }
        auto const new_result = concat( result, n );
        if( n % 2 == 0 )
        {
            return collatz_aux( n/2, new_result );
        }
        else
        {
            return collatz_aux( 3*n + 1, new_result );
        }
    }
    auto collatz( int const n )
        -> Vector_<int>
    {
        assert( n != 0 );
        return collatz_aux( n, Vector_<int>() );
    }
}  // namespace my
#include <iostream>
using namespace std;
auto main() -> int
{
    for( int const x : my::collatz( 42 ) )
    {
        cout << x << ' ';
    }
    cout << '\n';
}
Output:
42 21 64 32 16 8 4 2
The number of item copy operations due to copying of the vectors is here roughly O(n²), since it's the sum 1 + 2 + 3
+ ... n.
In concrete numbers, with g++ and Visual C++ compilers the above invocation of collatz(42) resulted in a Collatz
sequence of 8 items and 36 item copy operations (8*7/2 = 28, plus some) in vector copy constructor calls.
```

All of these item copy operations can be removed by simply moving vectors whose values are not needed anymore.
To do this it's necessary to remove const and reference for the vector type arguments, passing the vectors by value.
The function returns are already automatically optimized. For the calls where vectors are passed, and not used
again further on in the function, just apply std::move to move those buﬀers rather than actually copying them:
```cpp
using std::move;
auto concat( Vector_<int> v, int const x )
    -> Vector_<int>
{
    v.push_back( x );
    // warning: moving a local object in a return statement prevents copy elision [-Wpessimizing-
move]
    // See https://stackoverflow.com/documentation/c%2b%2b/2489/copy-elision
    // return move( v );
    return v;
}
auto collatz_aux( int const n, Vector_<int> result )
    -> Vector_<int>
{
    if( n == 1 )
    {
        return result;
    }
    auto new_result = concat( move( result ), n );
    struct result;      // Make absolutely sure no use of `result` after this.
    if( n % 2 == 0 )
    {
        return collatz_aux( n/2, move( new_result ) );
    }
    else
    {
        return collatz_aux( 3*n + 1, move( new_result ) );
    }
}
auto collatz( int const n )
    -> Vector_<int>
{
    assert( n != 0 );
    return collatz_aux( n, Vector_<int>() );
}
Here, with g++ and Visual C++ compilers, the number of item copy operations due to vector copy constructor
invocations, was exactly 0.
The algorithm is necessarily still O(n) in the length of the Collatz sequence produced, but this is a quite dramatic
improvement: O(n²) → O(n).
With some language support one could perhaps use moving and still express and enforce the immutability of a
variable between its initialization and ﬁnal move, after which any use of that variable should be an error. Alas, as of
C++14 C++ does not support that. For loop-free code the no use after move can be enforced via a re-declaration of
the relevant name as an incomplete struct, as with struct result; above, but this is ugly and not likely to be
understood by other programmers; also the diagnostics can be quite misleading.
Summing up, the C++ language and library support for moving allows drastic improvements in algorithm
complexity, but due the support's incompleteness, at the cost of forsaking the code correctness guarantees and
code clarity that const can provide.
For completeness, the instrumented vector class used to measure the number of item copy operations due to copy
constructor invocations:
template< class Item >
class Copy_tracking_vector
{
private:
    static auto n_copy_ops()
        -> int&
    {
        static int value;
        return value;
    }
    vector<Item>    items_;
public:
    static auto n() -> int { return n_copy_ops(); }
    void push_back( Item const& o ) { items_.push_back( o ); }
    auto begin() const { return items_.begin(); }
    auto end() const { return items_.end(); }
    Copy_tracking_vector(){}
    Copy_tracking_vector( Copy_tracking_vector const& other )
        : items_( other.items_ )
    { n_copy_ops() += items_.size(); }
    Copy_tracking_vector( Copy_tracking_vector&& other )
        : items_( move( other.items_ ) )
    {}
};
Section 106.3: Move constructor
```

Say we have this code snippet.
```cpp
class A {
public:
    int a;
    int b;
    A(const A &other) {
        this->a = other.a;
        this->b = other.b;
    }
};
To create a copy constructor, that is, to make a function that copies an object and creates a new one, we normally
would choose the syntax shown above, we would have a constructor for A that takes an reference to another object
of type A, and we would copy the object manually inside the method.
Alternatively, we could have written A(const A &) = default; which automatically copies over all members,
making use of its copy constructor.
To create a move constructor, however, we will be taking an rvalue reference instead of an lvalue reference, like
here.
class Wallet {
public:
    int nrOfDollars;
    Wallet() = default; //default ctor
    Wallet(Wallet &&other) {
        this->nrOfDollars = other.nrOfDollars;
        other.nrOfDollars = 0;
    }
};
Please notice that we set the old values to zero. The default move constructor (Wallet(Wallet&&) = default;)
copies the value of nrOfDollars, as it is a POD.
As move semantics are designed to allow 'stealing' state from the original instance, it is important to consider how
the original instance should look like after this stealing. In this case, if we would not change the value to zero we
would have doubled the amount of dollars into play.
Wallet a;
a.nrOfDollars = 1;
Wallet b (std::move(a)); //calling B(B&& other);
std::cout << a.nrOfDollars << std::endl; //0
std::cout << b.nrOfDollars << std::endl; //1
```

Thus we have move constructed an object from an old one.
While the above is a simple example, it shows what the move constructor is intended to do. It becomes more useful
in more complex cases, such as when resource management is involved.
    // Manages operations involving a specified type.
    // Owns a helper on the heap, and one in its memory (presumably on the stack).
    // Both helpers are DefaultConstructible, CopyConstructible, and MoveConstructible.
```cpp
    template<typename T,
             template<typename> typename HeapHelper,
             template<typename> typename StackHelper>
    class OperationsManager {
        using MyType = OperationsManager<T, HeapHelper, StackHelper>;
        HeapHelper<T>* h_helper;
        StackHelper<T> s_helper;
        // ...
      public:
        // Default constructor & Rule of Five.
        OperationsManager() : h_helper(new HeapHelper<T>) {}
        OperationsManager(const MyType& other)
          : h_helper(new HeapHelper<T>(*other.h_helper)), s_helper(other.s_helper) {}
        MyType& operator=(MyType copy) {
            swap(*this, copy);
            return *this;
        }
        ~OperationsManager() {
            if (h_helper) { delete h_helper; }
        }
        // Move constructor (without swap()).
        // Takes other's HeapHelper<T>*.
        // Takes other's StackHelper<T>, by forcing the use of StackHelper<T>'s move constructor.
        // Replaces other's HeapHelper<T>* with nullptr, to keep other from deleting our shiny
        //  new helper when it's destroyed.
        OperationsManager(MyType&& other) noexcept
          : h_helper(other.h_helper),
            s_helper(std::move(other.s_helper)) {
            other.h_helper = nullptr;
        }
        // Move constructor (with swap()).
        // Places our members in the condition we want other's to be in, then switches members
        //  with other.
        // OperationsManager(MyType&& other) noexcept : h_helper(nullptr) {
        //     swap(*this, other);
        // }
        // Copy/move helper.
        friend void swap(MyType& left, MyType& right) noexcept {
            std::swap(left.h_helper, right.h_helper);
            std::swap(left.s_helper, right.s_helper);
        }
    };
Section 106.4: Re-use a moved object
You can re-use a moved object:
void consumingFunction(std::vector<int> vec) {
    // Some operations
}
int main() {
    // initialize vec with 1, 2, 3, 4
    std::vector<int> vec{1, 2, 3, 4};
    // Send the vector by move
    consumingFunction(std::move(vec));
    // Here the vec object is in an indeterminate state.
    // Since the object is not destroyed, we can assign it a new content.
    // We will, in this case, assign an empty value to the vector,
    // making it effectively empty
    vec = {};
    // Since the vector as gained a determinate value, we can use it normally.
    vec.push_back(42);
    // Send the vector by move again.
    consumingFunction(std::move(vec));
}
Section 106.5: Move assignment
Similarly to how we can assign a value to an object with an lvalue reference, copying it, we can also move the values
from an object to another without constructing a new one. We call this move assignment. We move the values from
one object to another existing object.
For this, we will have to overload operator =, not so that it takes an lvalue reference, like in copy assignment, but
so that it takes an rvalue reference.
class A {
    int a;
    A& operator= (A&& other) {
        this->a = other.a;
        other.a = 0;
        return *this;
    }
};
This is the typical syntax to deﬁne move assignment. We overload operator = so that we can feed it an rvalue
reference and it can assign it to another object.
A a;
a.a = 1;
A b;
b = std::move(a); //calling A& operator= (A&& other)
std::cout << a.a << std::endl; //0
std::cout << b.a << std::endl; //1
```

Thus, we can move assign an object to another one.
Section 106.6: Using move semantics on containers
You can move a container instead of copying it:
```cpp
void print(const std::vector<int>& vec) {
    for (auto&& val : vec) {
        std::cout << val << ", ";
    }
    std::cout << std::endl;
}
int main() {
    // initialize vec1 with 1, 2, 3, 4 and vec2 as an empty vector
    std::vector<int> vec1{1, 2, 3, 4};
    std::vector<int> vec2;
    // The following line will print 1, 2, 3, 4
    print(vec1);
    // The following line will print a new line
    print(vec2);
    // The vector vec2 is assigned with move assingment.
    // This will "steal" the value of vec1 without copying it.
    vec2 = std::move(vec1);
    // Here the vec1 object is in an indeterminate state, but still valid.
    // The object vec1 is not destroyed,
    // but there's is no guarantees about what it contains.
    // The following line will print 1, 2, 3, 4
    print(vec2);
}


Move semantics is arguably the most significant performance feature in C++11. It allows resources to be "transferred" (moved) from temporary objects rather than copied.

***
```


### 1. LVALUES AND RVALUES

#### 1.1 Lvalues
An **lvalue** (locator value) represents an object that occupies an identifiable location in memory (has an address).
- Example: `int x = 5;` (`x` is lvalue).
- You can take its address: `&x`.

#### 1.2 Rvalues
An **rvalue** is everything else: temporary values, literals, or results of expressions.
- Example: `5`, `x + 2`, `funcReturningVal()`.
- You cannot take its address.

***

### 2. RVALUE REFERENCES

C++11 introduced the rvalue reference: `T&&`. It binds *only* to rvalues.

```cpp
int x = 10;
int& lref = x;      // Lvalue ref binds to lvalue
// int&& rref = x;  // Error: cannot bind rvalue ref to lvalue

int&& rref2 = 20;   // OK: 20 is rvalue
```

***

### 3. MOVE CONSTRUCTOR & ASSIGNMENT

This allows a class to steal resources from a temporary object instead of making a deep copy.

#### 3.1 Deep Copy (The Old Way)

```cpp
class Vector {
    int* data;
    size_t size;
public:
    // Copy Constructor
    Vector(const Vector& other) : size(other.size) {
        data = new int[size];
        std::copy(other.data, other.data + size, data);
    }
};
```

#### 3.2 Move Constructor (The C++11 Way)

```cpp
    // Move Constructor
    Vector(Vector&& other) noexcept : data(other.data), size(other.size) {
        // Steal the pointer
        other.data = nullptr; // Null out source
        other.size = 0;
    }
```

If `other` is a temporary, the compiler selects the Move Constructor. This is O(1) instead of O(N).

***

### 4. STD::MOVE

`std::move(x)` does exactly one thing: it casts `x` to an rvalue reference (`T&&`). It essentially says, "I am done with this object, you can steal from it."

```cpp
Vector v1(100);
Vector v2 = std::move(v1); // Calls Move Constructor
// v1 is now empty (if implemented correctly)
```

***

### 5. PERFECT FORWARDING

Used in templates to preserve the value category (lvalue vs rvalue) of arguments.

#### 5.1 Universal References (Forwarding References)

If `T` is a template parameter, `T&&` is a **universal reference**, not just an rvalue reference. It can bind to anything.

#### 5.2 std::forward

```cpp
template<typename T>
void wrapper(T&& arg) {
    func(std::forward<T>(arg));
}
```

- If `wrapper` is called with lvalue, `arg` is lvalue, `forward` keeps it lvalue.
- If `wrapper` is called with rvalue, `arg` is lvalue (as a named variable), but `forward` casts it back to rvalue.

This enables `emplace_back` to work efficiently.


### <a name="chapter-10-c11"></a>CHAPTER 10: C++11 FUNCTIONAL PROGRAMMING


***
#### Professional Insights: Functional Programming Deep Dive

##### std::function: To wrap any

element that is callable
Section 52.1: Simple usage
```cpp
#include <iostream>
#include <functional>
std::function<void(int , const std::string&)> myFuncObj;
void theFunc(int i, const std::string& s)
{
    std::cout << s << ": " << i << std::endl;
}
int main(int argc, char *argv[])
{
    myFuncObj = theFunc;
    myFuncObj(10, "hello world");
}
Section 52.2: std::function used with std::bind
Think about a situation where we need to callback a function with arguments. std::function used with std::bind
gives a very powerful design construct as shown below.
class A
{
public:
    std::function<void(int, const std::string&)> m_CbFunc = nullptr;
    void foo()
    {
        if (m_CbFunc)
        {
            m_CbFunc(100, "event fired");
        }
    }
};
class B
{
public:
    B()
    {
        auto aFunc = std::bind(&B::eventHandler, this, std::placeholders::_1,
std::placeholders::_2);
        anObjA.m_CbFunc = aFunc;
    }
    void eventHandler(int i, const std::string& s)
    {
        std::cout << s << ": " << i << std::endl;
    }
    void DoSomethingOnA()
    {
        anObjA.foo();
    }
    A anObjA;
};
int main(int argc, char *argv[])
{
     B anObjB;
     anObjB.DoSomethingOnA();
}
Section 52.3: Binding std::function to a dierent callable
types
/*
 * This example show some ways of using std::function to call
 *  a) C-like function
 *  b) class-member function
 *  c) operator()
 *  d) lambda function
 *
 * Function call can be made:
 *  a) with right arguments
 *  b) argumens with different order, types and count
 */
#include <iostream>
#include <functional>
#include <iostream>
#include <vector>
using std::cout;
using std::endl;
using namespace std::placeholders;
// simple function to be called
double foo_fn(int x, float y, double z)
{
  double res = x + y + z;
  std::cout << "foo_fn called with arguments: "
            << x << ", " << y << ", " << z
            << " result is : " << res
            << std::endl;
  return res;
}
// structure with member function to call
struct foo_struct
{
    // member function to call
    double foo_fn(int x, float y, double z)
    {
        double res = x + y + z;
        std::cout << "foo_struct::foo_fn called with arguments: "
                << x << ", " << y << ", " << z
                << " result is : " << res
                << std::endl;
        return res;
    }
    // this member function has different signature - but it can be used too
    // please not that argument order is changed too
    double foo_fn_4(int x, double z, float y, long xx)
    {
        double res = x + y + z + xx;
        std::cout << "foo_struct::foo_fn_4 called with arguments: "
                << x << ", " << z << ", " << y << ", " << xx
                << " result is : " << res
                << std::endl;
        return res;
    }
    // overloaded operator() makes whole object to be callable
    double operator()(int x, float y, double z)
    {
        double res = x + y + z;
        std::cout << "foo_struct::operator() called with arguments: "
                << x << ", " << y << ", " << z
                << " result is : " << res
                << std::endl;
        return res;
    }
};
int main(void)
{
  // typedefs
  using function_type = std::function<double(int, float, double)>;
  // foo_struct instance
  foo_struct fs;
  // here we will store all binded functions
  std::vector<function_type> bindings;
  // var #1 - you can use simple function
  function_type var1 = foo_fn;
  bindings.push_back(var1);
  // var #2 - you can use member function
  function_type var2 = std::bind(&foo_struct::foo_fn, fs, _1, _2, _3);
  bindings.push_back(var2);
  // var #3 - you can use member function with different signature
  // foo_fn_4 has different count of arguments and types
  function_type var3 = std::bind(&foo_struct::foo_fn_4, fs, _1, _3, _2, 0l);
  bindings.push_back(var3);
  // var #4 - you can use object with overloaded operator()
  function_type var4 = fs;
  bindings.push_back(var4);
  // var #5 - you can use lambda function
  function_type var5 = [](int x, float y, double z)
    {
        double res = x + y + z;
        std::cout << "lambda  called with arguments: "
                << x << ", " << y << ", " << z
                << " result is : " << res
                << std::endl;
        return res;
    };
  bindings.push_back(var5);
  std::cout << "Test stored functions with arguments: x = 1, y = 2, z = 3"
            << std::endl;
  for (auto f : bindings)
      f(1, 2, 3);
}
Live
Output:
Test stored functions with arguments: x = 1, y = 2, z = 3
foo_fn called with arguments: 1, 2, 3 result is : 6
foo_struct::foo_fn called with arguments: 1, 2, 3 result is : 6
foo_struct::foo_fn_4 called with arguments: 1, 3, 2, 0 result is : 6
foo_struct::operator() called with arguments: 1, 2, 3 result is : 6
lambda  called with arguments: 1, 2, 3 result is : 6
Section 52.4: Storing function arguments in std::tuple
```

Some programs need so store arguments for future calling of some function.
This example shows how to call any function with arguments stored in std::tuple
```cpp
#include <iostream>
#include <functional>
#include <tuple>
#include <iostream>
// simple function to be called
double foo_fn(int x, float y, double z)
{
   double res =  x + y + z;
   std::cout << "foo_fn called. x = " << x << " y = " << y << " z = " << z
             << " res=" << res;
   return res;
}
// helpers for tuple unrolling
template<int ...> struct seq {};
template<int N, int ...S> struct gens : gens<N-1, N-1, S...> {};
template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };
// invocation helper
template<typename FN, typename P, int ...S>
double call_fn_internal(const FN& fn, const P& params, const seq<S...>)
{
   return fn(std::get<S>(params) ...);
}
// call function with arguments stored in std::tuple
template<typename Ret, typename ...Args>
Ret call_fn(const std::function<Ret(Args...)>& fn,
            const std::tuple<Args...>& params)
{
    return call_fn_internal(fn, params, typename gens<sizeof...(Args)>::type());
}
int main(void)
{
  // arguments
  std::tuple<int, float, double> t = std::make_tuple(1, 5, 10);
  // function to call
  std::function<double(int, float, double)> fn = foo_fn;
  // invoke a function with stored arguments
  call_fn(fn, t);
}
Live
Output:
foo_fn called. x = 1 y = 5 z = 10 res=16
Section 52.5: std::function with lambda and std::bind
#include <iostream>
#include <functional>
using std::placeholders::_1; // to be used in std::bind example
int stdf_foobar (int x, std::function<int(int)> moo)
{
    return x + moo(x); // std::function moo called
}
int foo (int x) { return 2+x; }
int foo_2 (int x, int y) { return 9*x + y; }
int main()
{
    int a = 2;
    /* Function pointers */
    std::cout << stdf_foobar(a, &foo) << std::endl; // 6 ( 2 + (2+2) )
    // can also be: stdf_foobar(2, foo)
    /* Lambda expressions */
    /* An unnamed closure from a lambda expression can be
     * stored in a std::function object:
     */
    int capture_value = 3;
    std::cout << stdf_foobar(a,
                             [capture_value](int param) -> int { return 7 + capture_value * param;
})
              << std::endl;
    // result: 15 ==  value + (7 * capture_value * value) == 2 + (7 + 3 * 2)
    /* std::bind expressions */
    /* The result of a std::bind expression can be passed.
     * For example by binding parameters to a function pointer call:
     */    
    int b = stdf_foobar(a, std::bind(foo_2, _1, 3));
    std::cout << b << std::endl;
    // b == 23 == 2 + ( 9*2 + 3 )
    int c = stdf_foobar(a, std::bind(foo_2, 5, _1));
    std::cout << c << std::endl;
    // c == 49 == 2 + ( 9*5 + 2 )
    return 0;
}
Section 52.6: `function` overhead
std::function can cause signiﬁcant overhead. Because std::function has [value semantics][1], it must copy or
move the given callable into itself. But since it can take callables of an arbitrary type, it will frequently have to
allocate memory dynamically to do this.
Some function implementations have so-called "small object optimization", where small types (like function
pointers, member pointers, or functors with very little state) will be stored directly in the function object. But even
this only works if the type is noexcept move constructible. Furthermore, the C++ standard does not require that all
implementations provide one.
Consider the following:
//Header file
using MyPredicate = std::function<bool(const MyValue &, const MyValue &)>;
void SortMyContainer(MyContainer &C, const MyPredicate &pred);
//Source file
void SortMyContainer(MyContainer &C, const MyPredicate &pred)
{
    std::sort(C.begin(), C.end(), pred);
}
A template parameter would be the preferred solution for SortMyContainer, but let us assume that this is not
possible or desirable for whatever reason. SortMyContainer does not need to store pred beyond its own call. And
yet, pred may well allocate memory if the functor given to it is of some non-trivial size.
function allocates memory because it needs something to copy/move into; function takes ownership of the
callable it is given. But SortMyContainer does not need to own the callable; it's just referencing it. So using function
here is overkill; it may be eﬃcient, but it may not.
There is no standard library function type that merely references a callable. So an alternate solution will have to be
found, or you can choose to live with the overhead.
Also, function has no eﬀective means to control where the memory allocations for the object come from. Yes, it
has constructors that take an allocator, but [many implementations do not implement them correctly... or even at
all][2].
Version ≥ C++17
The function constructors that take an allocator no longer are part of the type. Therefore, there is no way to
manage the allocation.
Calling a function is also slower than calling the contents directly. Since any function instance could hold any
callable, the call through a function must be indirect. The overhead of calling function is on the order of a virtual
function call.
```


##### Lambdas

Parameter
default-capture
Details
Speciﬁes how all non-listed variables are captured. Can be = (capture by value) or & (capture by
reference). If omitted, non-listed variables are inaccessible within the lambda-body. The default-
capture must precede the capture-list.
capture-list
Speciﬁes how local variables are made accessible within the lambda-body. Variables without
preﬁx are captured by value. Variables preﬁxed with & are captured by reference. Within a class
method, this can be used to make all its members accessible by reference. Non-listed variables
are inaccessible, unless the list is preceded by a default-capture.
argument-list
Speciﬁes the arguments of the lambda function.
mutable
(optional) Normally variables captured by value are const. Specifying mutable makes them non-
const. Changes to those variables are retained between calls.
throw-speciﬁcation
(optional) Speciﬁes the exception throwing behavior of the lambda function. For example:
noexcept or throw(std::exception).
attributes
(optional) Any attributes for the lambda function. For example, if the lambda-body always throws
an exception then [[noreturn]] can be used.
-> return-type
(optional) Speciﬁes the return type of the lambda function. Required when the return type
cannot be determined by the compiler.
lambda-body
A code block containing the implementation of the lambda function.
Section 73.1: What is a lambda expression?
A lambda expression provides a concise way to create simple function objects. A lambda expression is a prvalue
whose result object is called closure object, which behaves like a function object.
The name 'lambda expression' originates from lambda calculus, which is a mathematical formalism invented in the
1930s by Alonzo Church to investigate questions about logic and computability. Lambda calculus formed the basis
of LISP, a functional programming language. Compared to lambda calculus and LISP, C++ lambda expressions share
the properties of being unnamed, and to capture variables from the surrounding context, but they lack the ability to
operate on and return functions.
A lambda expression is often used as an argument to functions that take a callable object. That can be simpler than
creating a named function, which would be only used when passed as the argument. In such cases, lambda
expressions are generally preferred because they allow deﬁning the function objects inline.
A lambda consists typically of three parts: a capture list [], an optional parameter list () and a body {}, all of which
can be empty:
[](){}                // An empty lambda, which does and returns nothing
Capture list
[] is the capture list. By default, variables of the enclosing scope cannot be accessed by a lambda. Capturing a
variable makes it accessible inside the lambda, either as a copy or as a reference. Captured variables become a part
of the lambda; in contrast to function arguments, they do not have to be passed when calling the lambda.
int a = 0;                       // Define an integer variable
auto f = []()   { return a*9; }; // Error: 'a' cannot be accessed
auto f = [a]()  { return a*9; }; // OK, 'a' is "captured" by value
auto f = [&a]() { return a++; }; // OK, 'a' is "captured" by reference
                                 //      Note: It is the responsibility of the programmer
                                 //      to ensure that a is not destroyed before the


C++11 brought functional programming paradigms to the language, centered around Lambdas.

***

### 1. LAMBDA EXPRESSIONS

Lambdas are anonymous function objects.

#### 1.1 Syntax

`[ captures ] ( params ) -> ret { body }`

```cpp
auto add = [](int a, int b) { return a + b; };
int sum = add(1, 2);
```

#### 1.2 Captures

- `[]`: No capture.
- `[=]`: Capture everything by value (copy).
- `[&]`: Capture everything by reference.
- `[x]`: Capture x by value.
- `[&x]`: Capture x by reference.

```cpp
int factor = 10;
auto multiply = [factor](int n) { return n * factor; }; // factor captured by value
```

#### 1.3 Mutable Lambdas

By default, value captures are `const`. Use `mutable` to modify them.

```cpp
int x = 0;
auto increment = [x]() mutable { return ++x; }; // x is internal state
```

***

### 2. STD::FUNCTION

`std::function` is a polymorphic wrapper for *any* callable (function pointer, lambda, functor, bind result).

```cpp
#include <functional>

void print(int i) { std::cout << i; }

std::function<void(int)> func;
func = print;
func = [](int i) { std::cout << i * 2; };
```

It has runtime overhead (virtual function call, possible allocation). Prefer templates or auto if possible.

***

### 3. STD::BIND

`std::bind` performs partial application of functions.

```cpp
#include <functional>
using namespace std::placeholders;

int sub(int a, int b) { return a - b; }

// Bind second argument to 5
auto sub5 = std::bind(sub, _1, 5); 
// sub5(10) calls sub(10, 5) -> 5
```

**Note:** Lambdas mostly replaced `std::bind` in modern C++ because they are clearer and faster (compiler optimization).
### <a name="chapter-11-c11"></a>CHAPTER 11: C++11 CONCURRENCY


***
#### Professional Insights: Concurrency Deep Dive

##### Threading

Parameter
other
Details
Takes ownership of other, other doesn't own the thread anymore
func
args
Function to call in a separate thread
Arguments for func
Section 80.1: Creating a std::thread
In C++, threads are created using the std::thread class. A thread is a separate ﬂow of execution; it is analogous to
having a helper perform one task while you simultaneously perform another. When all the code in the thread is
executed, it terminates. When creating a thread, you need to pass something to be executed on it. A few things that
you can pass to a thread:
Free functions
Member functions
Functor objects
Lambda expressions
Free function example - executes a function on a separate thread (Live Example):
```cpp
#include <iostream>
#include <thread>
void foo(int a)
{
    std::cout << a << '\n';
}
int main()
{
    // Create and execute the thread
    std::thread thread(foo, 10); // foo is the function to execute, 10 is the
                                 // argument to pass to it
    // Keep going; the thread is executed separately
    // Wait for the thread to finish; we stay here until it is done
    thread.join();
    return 0;
}
Member function example - executes a member function on a separate thread (Live Example):
#include <iostream>
#include <thread>
class Bar
{
public:
    void foo(int a)
    {
        std::cout << a << '\n';
    }
};
int main()
{
    Bar bar;
    // Create and execute the thread
    std::thread thread(&Bar::foo, &bar, 10); // Pass 10 to member function
    // The member function will be executed in a separate thread
    // Wait for the thread to finish, this is a blocking operation
    thread.join();
    return 0;
}
Functor object example (Live Example):
#include <iostream>
#include <thread>
class Bar
{
public:
    void operator()(int a)
    {
        std::cout << a << '\n';
    }
};
int main()
{
    Bar bar;
    // Create and execute the thread
    std::thread thread(bar, 10); // Pass 10 to functor object
    // The functor object will be executed in a separate thread
    // Wait for the thread to finish, this is a blocking operation
    thread.join();
    return 0;
}
Lambda expression example (Live Example):
#include <iostream>
#include <thread>
int main()
{
    auto lambda = [](int a) { std::cout << a << '\n'; };
    // Create and execute the thread
    std::thread thread(lambda, 10); // Pass 10 to the lambda expression
    // The lambda expression will be executed in a separate thread
    // Wait for the thread to finish, this is a blocking operation
    thread.join();
    return 0;
}
Section 80.2: Passing a reference to a thread
You cannot pass a reference (or const reference) directly to a thread because std::thread will copy/move them.
Instead, use std::reference_wrapper:
void foo(int& b)
{
    b = 10;
}
int a = 1;
std::thread thread{ foo, std::ref(a) }; //'a' is now really passed as reference
thread.join();
std::cout << a << '\n'; //Outputs 10
void bar(const ComplexObject& co)
{
    co.doCalculations();
}
ComplexObject object;
std::thread thread{ bar, std::cref(object) }; //'object' is passed as const&
thread.join();
std::cout << object.getResult() << '\n'; //Outputs the result
Section 80.3: Using std::async instead of std::thread
std::async is also able to make threads. Compared to std::thread it is considered less powerful but easier to use
when you just want to run a function asynchronously.
Asynchronously calling a function
#include <future>
#include <iostream>
unsigned int square(unsigned int i){
    return i*i;
}
int main() {
    auto f = std::async(std::launch::async, square, 8);
    std::cout << "square currently running\n"; //do something while square is running
    std::cout << "result is " << f.get() << '\n'; //getting the result from square
}
Common Pitfalls
std::async returns a std::future that holds the return value that will be calculated by the function. When
that future gets destroyed it waits until the thread completes, making your code eﬀectively single threaded.
This is easily overlooked when you don't need the return value:
std::async(std::launch::async, square, 5);
//thread already completed at this point, because the returning future got destroyed
std::async works without a launch policy, so std::async(square, 5); compiles. When you do that the
system gets to decide if it wants to create a thread or not. The idea was that the system chooses to make a
thread unless it is already running more threads than it can run eﬃciently. Unfortunately implementations
commonly just choose not to create a thread in that situation, ever, so you need to override that behavior
with std::launch::async which forces the system to create a thread.
```

Beware of race conditions.
More on async on Futures and Promises
Section 80.4: Basic Synchronization
Thread synchronization can be accomplished using mutexes, among other synchronization primitives. There are
several mutex types provided by the standard library, but the simplest is std::mutex. To lock a mutex, you
construct a lock on it. The simplest lock type is std::lock_guard:
```cpp
std::mutex m;
void worker() {
    std::lock_guard<std::mutex> guard(m); // Acquires a lock on the mutex
    // Synchronized code here
} // the mutex is automatically released when guard goes out of scope
With std::lock_guard the mutex is locked for the whole lifetime of the lock object. In cases where you need to
manually control the regions for locking, use std::unique_lock instead:
std::mutex m;
void worker() {
    // by default, constructing a unique_lock from a mutex will lock the mutex
    // by passing the std::defer_lock as a second argument, we
    // can construct the guard in an unlocked state instead and
    // manually lock later.
    std::unique_lock<std::mutex> guard(m, std::defer_lock);
    // the mutex is not locked yet!
    guard.lock();
    // critical section
    guard.unlock();
    // mutex is again released
}
More Thread synchronization structures
Section 80.5: Create a simple thread pool
C++11 threading primitives are still relatively low level. They can be used to write a higher level construct, like a
thread pool:
Version ≥ C++14
struct tasks {
  // the mutex, condition variable and deque form a single
  // thread-safe triggered queue of tasks:
  std::mutex m;
  std::condition_variable v;
  // note that a packaged_task<void> can store a packaged_task<R>:
  std::deque<std::packaged_task<void()>> work;
  // this holds futures representing the worker threads being done:
  std::vector<std::future<void>> finished;
  // queue( lambda ) will enqueue the lambda into the tasks for the threads
  // to use.  A future of the type the lambda returns is given to let you get
  // the result out.
  template<class F, class R=std::result_of_t<F&()>>
  std::future<R> queue(F&& f) {
    // wrap the function object into a packaged task, splitting
    // execution from the return value:
    std::packaged_task<R()> p(std::forward<F>(f));
    auto r=p.get_future(); // get the return value before we hand off the task
    {
      std::unique_lock<std::mutex> l(m);
      work.emplace_back(std::move(p)); // store the task<R()> as a task<void()>
    }
    v.notify_one(); // wake a thread to work on the task
    return r; // return the future result of the task
  }
  // start N threads in the thread pool.
  void start(std::size_t N=1){
    for (std::size_t i = 0; i < N; ++i)
    {
      // each thread is a std::async running this->thread_task():
      finished.push_back(
        std::async(
          std::launch::async,
          [this]{ thread_task(); }
        )
      );
    }
  }
  // abort() cancels all non-started tasks, and tells every working thread
  // stop running, and waits for them to finish up.
  void abort() {
    cancel_pending();
    finish();
  }
  // cancel_pending() merely cancels all non-started tasks:
  void cancel_pending() {
    std::unique_lock<std::mutex> l(m);
    work.clear();
  }
  // finish enques a "stop the thread" message for every thread, then waits for them:
  void finish() {
    {
      std::unique_lock<std::mutex> l(m);
      for(auto&&unused:finished){
        work.push_back({});
      }
    }
    v.notify_all();
    finished.clear();
  }
  ~tasks() {
    finish();
  }
private:
  // the work that a worker thread does:
  void thread_task() {
    while(true){
      // pop a task off the queue:
      std::packaged_task<void()> f;
      {
        // usual thread-safe queue code:
        std::unique_lock<std::mutex> l(m);
        if (work.empty()){
          v.wait(l,[&]{return !work.empty();});
        }
        f = std::move(work.front());
        work.pop_front();
      }
      // if the task is invalid, it means we are asked to abort:
      if (!f.valid()) return;
      // otherwise, run the task:
      f();
    }
  }
};
tasks.queue( []{ return "hello world"s; } ) returns a std::future<std::string>, which when the tasks
object gets around to running it is populated with hello world.
```

You create threads by running tasks.start(10) (which starts 10 threads).
The use of packaged_task<void()> is merely because there is no type-erased std::function equivalent that stores
move-only types. Writing a custom one of those would probably be faster than using packaged_task<void()>.
Live example.
Version = C++11
In C++11, replace result_of_t<blah> with typename result_of<blah>::type.
More on Mutexes.
Section 80.6: Ensuring a thread is always joined
When the destructor for std::thread is invoked, a call to either join() or detach() must have been made. If a
thread has not been joined or detached, then by default std::terminate will be called. Using RAII, this is generally
simple enough to accomplish:
```cpp
class thread_joiner
{
public:
    thread_joiner(std::thread t)
        : t_(std::move(t))
    { }
    ~thread_joiner()
    {
        if(t_.joinable()) {
            t_.join();
        }
    }
private:
    std::thread t_;
}
This is then used like so:
 void perform_work()
 {
     // Perform some work
 }
 void t()
 {
     thread_joiner j{std::thread(perform_work)};
     // Do some other calculations while thread is running
 } // Thread is automatically joined here
This also provides exception safety; if we had created our thread normally and the work done in t() performing
other calculations had thrown an exception, join() would never have been called on our thread and our process
would have been terminated.
Section 80.7: Operations on the current thread
std::this_thread is a namespace which has functions to do interesting things on the current thread from function
it is called from.
Function
Description
get_id
Returns the id of the thread
sleep_for
Sleeps for a speciﬁed amount of time
sleep_until Sleeps until a speciﬁc time
yield
Reschedule running threads, giving other threads priority
Getting the current threads id using std::this_thread::get_id:
void foo()
{
    //Print this threads id
    std::cout << std::this_thread::get_id() << '\n';
}
std::thread thread{ foo };
thread.join(); //'threads' id has now been printed, should be something like 12556
foo(); //The id of the main thread is printed, should be something like 2420
Sleeping for 3 seconds using std::this_thread::sleep_for:
void foo()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
}
std::thread thread{ foo };
foo.join();
std::cout << "Waited for 3 seconds!\n";
Sleeping until 3 hours in the future using std::this_thread::sleep_until:
void foo()
{
    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::hours(3));
}
std::thread thread{ foo };
thread.join();
std::cout << "We are now located 3 hours after the thread has been called\n";
Letting other threads take priority using std::this_thread::yield:
void foo(int a)
{
    for (int i = 0; i < al ++i)
        std::this_thread::yield(); //Now other threads take priority, because this thread
                                   //isn't doing anything important
    std::cout << "Hello World!\n";
}
std::thread thread{ foo, 10 };
thread.join();
Section 80.8: Using Condition Variables
A condition variable is a primitive used in conjunction with a mutex to orchestrate communication between
threads. While it is neither the exclusive or most eﬃcient way to accomplish this, it can be among the simplest to
those familiar with the pattern.
One waits on a std::condition_variable with a std::unique_lock<std::mutex>. This allows the code to safely
examine shared state before deciding whether or not to proceed with acquisition.
Below is a producer-consumer sketch that uses std::thread, std::condition_variable, std::mutex, and a few
others to make things interesting.
#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
int main()
{
    std::condition_variable cond;
    std::mutex mtx;
    std::queue<int> intq;
    bool stopped = false;
    std::thread producer{[&]()
    {
        // Prepare a random number generator.
        // Our producer will simply push random numbers to intq.
        //
        std::default_random_engine gen{};
        std::uniform_int_distribution<int> dist{};
        std::size_t count = 4006;    
        while(count--)
        {    
            // Always lock before changing
            // state guarded by a mutex and
            // condition_variable (a.k.a. "condvar").
            std::lock_guard<std::mutex> L{mtx};
            // Push a random int into the queue
            intq.push(dist(gen));
            // Tell the consumer it has an int
            cond.notify_one();
        }
        // All done.
        // Acquire the lock, set the stopped flag,
        // then inform the consumer.
        std::lock_guard<std::mutex> L{mtx};
        std::cout << "Producer is done!" << std::endl;
        stopped = true;
        cond.notify_one();
    }};
    std::thread consumer{[&]()
    {
        do{
            std::unique_lock<std::mutex> L{mtx};
            cond.wait(L,[&]()
            {
                // Acquire the lock only if
                // we've stopped or the queue
                // isn't empty
                return stopped || ! intq.empty();
            });
            // We own the mutex here; pop the queue
            // until it empties out.
            while( ! intq.empty())
            {
                const auto val = intq.front();
                intq.pop();
                std::cout << "Consumer popped: " << val << std::endl;
            }
            if(stopped){
                // producer has signaled a stop
                std::cout << "Consumer is done!" << std::endl;
                break;
            }
        }while(true);
    }};
    consumer.join();
    producer.join();
    std::cout << "Example Completed!" << std::endl;
    return 0;
}
Section 80.9: Thread operations
```

When you start a thread, it will execute until it is ﬁnished.
Often, at some point, you need to (possibly - the thread may already be done) wait for the thread to ﬁnish, because
you want to use the result for example.
```cpp
int n;
std::thread thread{ calculateSomething, std::ref(n) };
//Doing some other stuff
//We need 'n' now!
//Wait for the thread to finish - if it is not already done
thread.join();
//Now 'n' has the result of the calculation done in the separate thread
std::cout << n << '\n';
You can also detach the thread, letting it execute freely:
std::thread thread{ doSomething };
//Detaching the thread, we don't need it anymore (for whatever reason)
thread.detach();
//The thread will terminate when it is done, or when the main thread returns
Section 80.10: Thread-local storage
Thread-local storage can be created using the thread_local keyword. A variable declared with the thread_local
speciﬁer is said to have thread storage duration.
```

Each thread in a program has its own copy of each thread-local variable.
A thread-local variable with function (local) scope will be initialized the ﬁrst time control passes through its
deﬁnition. Such a variable is implicitly static, unless declared extern.
A thread-local variable with namespace or class (non-local) scope will be initialized as part of thread startup.
Thread-local variables are destroyed upon thread termination.
A member of a class can only be thread-local if it is static. There will therefore be one copy of that variable
per thread, rather than one copy per (thread, instance) pair.
Example:
```cpp
void debug_counter() {
    thread_local int count = 0;
    Logger::log("This function has been called %d times by this thread", ++count);
}
Section 80.11: Reassigning thread objects
```

We can create empty thread objects and assign work to them later.
If we assign a thread object to another active, joinable thread, std::terminate will automatically be called before
the thread is replaced.
```cpp
#include <thread>
void foo()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
}
//create 100 thread objects that do nothing
std::thread executors[100];
// Some code
// I want to create some threads now
for (int i = 0;i < 100;i++)
{
    // If this object doesn't have a thread assigned
    if (!executors[i].joinable())
         executors[i] = std::thread(foo);
}
```


##### Thread synchronization

structures
Working with threads might need some synchronization techniques if the threads interact. In this topic, you can
ﬁnd the diﬀerent structures which are provided by the standard library to solve these issues.
Section 81.1: std::condition_variable_any, std::cv_status
A generalization of std::condition_variable, std::condition_variable_any works with any type of
BasicLockable structure.
std::cv_status as a return status for a condition variable has two possible return codes:
std::cv_status::no_timeout: There was no timeout, condition variable was notiﬁed
std::cv_status::no_timeout: Condition variable timed out
Section 81.2: std::shared_lock
A shared_lock can be used in conjunction with a unique lock to allow multiple readers and exclusive writers.
```cpp
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <string>
#include <iostream>
class PhoneBook {
    public:
        string getPhoneNo( const std::string & name )
        {
            shared_lock<shared_timed_mutex> r(_protect);
            auto it =  _phonebook.find( name );
            if ( it == _phonebook.end() )
                return (*it).second;
            return "";
        }
        void addPhoneNo ( const std::string & name, const std::string & phone )
        {
            unique_lock<shared_timed_mutex> w(_protect);
            _phonebook[name] = phone;
        }
        shared_timed_mutex _protect;
        unordered_map<string,string>  _phonebook;
    };
Section 81.3: std::call_once, std::once_ﬂag
std::call_once ensures execution of a function exactly once by competing threads. It throws std::system_error
in case it cannot complete its task.
Used in conjunction with std::once_flag.
#include <mutex>
#include <iostream>
std::once_flag flag;
void do_something(){
      std::call_once(flag, [](){std::cout << "Happens once" << std::endl;});
      std::cout << "Happens every time" << std::endl;
}
Section 81.4: Object locking for ecient access
Often you want to lock the entire object while you perform multiple operations on it. For example, if you need to
examine or modify the object using iterators. Whenever you need to call multiple member functions it is generally
more eﬃcient to lock the whole object rather than individual member functions.
For example:
class text_buffer
{
    // for readability/maintainability
    using mutex_type = std::shared_timed_mutex;
    using reading_lock = std::shared_lock<mutex_type>;
    using updates_lock = std::unique_lock<mutex_type>;
public:
    // This returns a scoped lock that can be shared by multiple
    // readers at the same time while excluding any writers
    [[nodiscard]]
    reading_lock lock_for_reading() const { return reading_lock(mtx); }
    // This returns a scoped lock that is exclusing to one
    // writer preventing any readers
    [[nodiscard]]
    updates_lock lock_for_updates() { return updates_lock(mtx); }
    char* data() { return buf; }
    char const* data() const { return buf; }
    char* begin() { return buf; }
    char const* begin() const { return buf; }
    char* end() { return buf + sizeof(buf); }
    char const* end() const { return buf + sizeof(buf); }
    std::size_t size() const { return sizeof(buf); }
private:
    char buf[1024];
    mutable mutex_type mtx; // mutable allows const objects to be locked
};
When calculating a checksum the object is locked for reading, allowing other threads that want to read from the
object at the same time to do so.
std::size_t checksum(text_buffer const& buf)
{
    std::size_t sum = 0xA44944A4;
    // lock the object for reading
    auto lock = buf.lock_for_reading();
    for(auto c: buf)
        sum = (sum << 8) | (((unsigned char) ((sum & 0xFF000000) >> 24)) ^ c);
    return sum;
}
```

Clearing the object updates its internal data so it must be done using an exclusing lock.
```cpp
void clear(text_buffer& buf)
{
    auto lock = buf.lock_for_updates(); // exclusive lock
    std::fill(std::begin(buf), std::end(buf), '\0');
}
When obtaining more than one lock care should be taken to always acquire the locks in the same order for all
threads.
void transfer(text_buffer const& input, text_buffer& output)
{
    auto lock1 = input.lock_for_reading();
    auto lock2 = output.lock_for_updates();
    std::copy(std::begin(input), std::end(input), std::begin(output));
}
note: This is best done using std::deferred::lock and calling std::lock
```


##### Mutexes

Section 85.1: Mutex Types
C++1x oﬀers a selection of mutex classes:
```cpp
std::mutex - oﬀers simple locking functionality.
std::timed_mutex - oﬀers try_to_lock functionality
std::recursive_mutex - allows recursive locking by the same thread.
std::shared_mutex, std::shared_timed_mutex - oﬀers shared and unique lock functionality.
Section 85.2: std::lock
std::lock uses deadlock avoidance algorithms to lock one or more mutexes. If an exception is thrown during a call
to lock multiple objects, std::lock unlocks the successfully locked objects before re-throwing the exception.
std::lock(_mutex1, _mutex2);
Section 85.3: std::unique_lock, std::shared_lock,
std::lock_guard
Used for the RAII style acquiring of try locks, timed try locks and recursive locks.
std::unique_lock allows for exclusive ownership of mutexes.
std::shared_lock allows for shared ownership of mutexes. Several threads can hold std::shared_locks on a
std::shared_mutex. Available from C++ 14.
std::lock_guard is a lightweight alternative to std::unique_lock and std::shared_lock.
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <string>
#include <iostream>
class PhoneBook {
public:
    std::string getPhoneNo( const std::string & name )
    {
        std::shared_lock<std::shared_timed_mutex> l(_protect);
        auto it =  _phonebook.find( name );
        if ( it != _phonebook.end() )
            return (*it).second;
        return "";
    }
    void addPhoneNo ( const std::string & name, const std::string & phone )
    {
        std::unique_lock<std::shared_timed_mutex> l(_protect);
        _phonebook[name] = phone;
    }
    std::shared_timed_mutex _protect;
    std::unordered_map<std::string,std::string>  _phonebook;
};
Section 85.4: Strategies for lock classes: std::try_to_lock,
std::adopt_lock, std::defer_lock
When creating a std::unique_lock, there are three diﬀerent locking strategies to choose from: std::try_to_lock,
std::defer_lock and std::adopt_lock
1.
std::try_to_lock allows for trying a lock without blocking:
{
    std::atomic_int temp {0};
    std::mutex _mutex;
    std::thread t( [&](){
        while( temp!= -1){
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::unique_lock<std::mutex> lock( _mutex, std::try_to_lock);
            if(lock.owns_lock()){
                //do something
                temp=0;
            }
        }
    });
    while ( true )
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::unique_lock<std::mutex> lock( _mutex, std::try_to_lock);
        if(lock.owns_lock()){
            if (temp < INT_MAX){
                ++temp;
            }
            std::cout << temp << std::endl;
        }
    }
}
2.
std::defer_lock allows for creating a lock structure without acquiring the lock. When locking more than one
mutex, there is a window of opportunity for a deadlock if two function callers try to acquire the locks at the
same time:
{
    std::unique_lock<std::mutex> lock1(_mutex1, std::defer_lock);
    std::unique_lock<std::mutex> lock2(_mutex2, std::defer_lock);
    lock1.lock()
    lock2.lock(); // deadlock here
    std::cout << "Locked! << std::endl;
    //...
}
With the following code, whatever happens in the function, the locks are acquired and released in appropriate
order:
   {
       std::unique_lock<std::mutex> lock1(_mutex1, std::defer_lock);
       std::unique_lock<std::mutex> lock2(_mutex2, std::defer_lock);
       std::lock(lock1,lock2); // no deadlock possible
       std::cout << "Locked! << std::endl;
       //...
   }
3.
std::adopt_lock does not attempt to lock a second time if the calling thread currently owns the lock.
{
    std::unique_lock<std::mutex> lock1(_mutex1, std::adopt_lock);
    std::unique_lock<std::mutex> lock2(_mutex2, std::adopt_lock);
    std::cout << "Locked! << std::endl;
    //...
}
Something to keep in mind is that std::adopt_lock is not a substitute for recursive mutex usage. When the lock goes
out of scope the mutex is released.
Section 85.5: std::mutex
std::mutex is a simple, non-recursive synchronization structure that is used to protect data which is accessed by
multiple threads.
    std::atomic_int temp{0};
    std::mutex _mutex;
    std::thread t( [&](){
                      while( temp!= -1){
                          std::this_thread::sleep_for(std::chrono::seconds(5));
                          std::unique_lock<std::mutex> lock( _mutex);
                              temp=0;
                      }
                  });
    while ( true )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::unique_lock<std::mutex> lock( _mutex, std::try_to_lock);
        if ( temp < INT_MAX )
            temp++;
        cout << temp << endl;
    }
Section 85.6: std::scoped_lock (C++ 17)
std::scoped_lock provides RAII style semantics for owning one more mutexes, combined with the lock avoidance
algorithms used by std::lock. When std::scoped_lock is destroyed, mutexes are released in the reverse order
from which they where acquired.
{
    std::scoped_lock lock{_mutex1,_mutex2};
    //do something
}
```


##### Futures and Promises

Promises and Futures are used to ferry a single object from one thread to another.
A std::promise object is set by the thread which generates the result.
A std::future object can be used to retrieve a value, to test to see if a value is available, or to halt execution until
the value is available.
Section 88.1: Async operation classes
std::async: performs an asynchronous operation.
std::future: provides access to the result of an asynchronous operation.
std::promise: packages the result of an asynchronous operation.
std::packaged_task: bundles a function and the associated promise for its return type.
Section 88.2: std::future and std::promise
The following example sets a promise to be consumed by another thread:
    {
```cpp
        auto promise = std::promise<std::string>();
        auto producer = std::thread([&]
        {
            promise.set_value("Hello World");
        });
        auto future = promise.get_future();
        auto consumer = std::thread([&]
        {
            std::cout << future.get();
        });
        producer.join();
        consumer.join();
}
Section 88.3: Deferred async example
This code implements a version of std::async, but it behaves as if async were always called with the deferred
launch policy. This function also does not have async's special future behavior; the returned future can be
destroyed without ever acquiring its value.
template<typename F>
auto async_deferred(F&& func) -> std::future<decltype(func())>
{
    using result_type = decltype(func());
    auto promise = std::promise<result_type>();
    auto future  = promise.get_future();
    std::thread(std::bind([=](std::promise<result_type>& promise)
    {
        try
        {
            promise.set_value(func());
            // Note: Will not work with std::promise<void>. Needs some meta-template programming
which is out of scope for this example.
        }
        catch(...)
        {
            promise.set_exception(std::current_exception());
        }
    }, std::move(promise))).detach();
    return future;
}
Section 88.4: std::packaged_task and std::future
std::packaged_task bundles a function and the associated promise for its return type:
template<typename F>
auto async_deferred(F&& func) -> std::future<decltype(func())>
{
    auto task   = std::packaged_task<decltype(func())()>(std::forward<F>(func));
    auto future = task.get_future();
    std::thread(std::move(task)).detach();
    return std::move(future);
}
The thread starts running immediately. We can either detach it, or have join it at the end of the scope. When the
function call to std::thread ﬁnishes, the result is ready.
Note that this is slightly diﬀerent from std::async where the returned std::future when destructed will actually
block until the thread is ﬁnished.
Section 88.5: std::future_error and std::future_errc
If constraints for std::promise and std::future are not met an exception of type std::future_error is thrown.
The error code member in the exception is of type std::future_errc and values are as below, along with some test
cases:
enum class future_errc {
    broken_promise             = /* the task is no longer shared */,
    future_already_retrieved   = /* the answer was already retrieved */,
    promise_already_satisfied  = /* the answer was stored already */,
    no_state                   = /* access to a promise in non-shared state */
};
Inactive promise:
int test()
{
    std::promise<int> pr;
    return 0; // returns ok
}
Active promise, unused:
  int test()
    {
        std::promise<int> pr;
        auto fut = pr.get_future(); //blocks indefinitely!
        return 0;
    }
Double retrieval:
int test()
{
    std::promise<int> pr;
    auto fut1 = pr.get_future();
    try{
        auto fut2 = pr.get_future();    //   second attempt to get future
        return 0;
    }
    catch(const std::future_error& e)
    {
        cout << e.what() << endl;       //   Error: "The future has already been retrieved from the
promise or packaged_task."
        return -1;
    }
    return fut2.get();
}
Setting std::promise value twice:
int test()
{
    std::promise<int> pr;
    auto fut = pr.get_future();
    try{
        std::promise<int> pr2(std::move(pr));
        pr2.set_value(10);
        pr2.set_value(10);  // second attempt to set promise throws exception
    }
    catch(const std::future_error& e)
    {
        cout << e.what() << endl;       //   Error: "The state of the promise has already been
set."
        return -1;
    }
    return fut.get();
}
Section 88.6: std::future and std::async
In the following naive parallel merge sort example, std::async is used to launch multiple parallel merge_sort tasks.
std::future is used to wait for the results and synchronize them:
#include <iostream>
using namespace std;
void merge(int low,int mid,int high, vector<int>&num)
{
    vector<int> copy(num.size());
    int h,i,j,k;
    h=low;
    i=low;
    j=mid+1;
    while((h<=mid)&&(j<=high))
    {
        if(num[h]<=num[j])
        {
            copy[i]=num[h];
            h++;
        }
        else
        {
            copy[i]=num[j];
            j++;
        }
        i++;
    }
    if(h>mid)
    {
        for(k=j;k<=high;k++)
        {
            copy[i]=num[k];
            i++;
        }
    }
    else
    {
        for(k=h;k<=mid;k++)
        {
            copy[i]=num[k];
            i++;
        }
    }
    for(k=low;k<=high;k++)
        swap(num[k],copy[k]);
}
void merge_sort(int low,int high,vector<int>& num)
{
    int mid;
    if(low<high)
    {
        mid = low + (high-low)/2;
        auto future1    =  std::async(std::launch::deferred,[&]()
                                      {
                                        merge_sort(low,mid,num);
                                      });
        auto future2    =  std::async(std::launch::deferred, [&]()
                                       {
                                          merge_sort(mid+1,high,num) ;
                                       });
        future1.get();
        future2.get();
        merge(low,mid,high,num);
    }
}
Note: In the example std::async is launched with policy std::launch_deferred. This is to avoid a new thread
being created in every call. In the case of our example, the calls to std::async are made out of order, the they
synchronize at the calls for std::future::get().
std::launch_async forces a new thread to be created in every call.
The default policy is std::launch::deferred| std::launch::async, meaning the implementation determines the
policy for creating new threads.



***
```

#### Professional Insights: Atomics & Memory Model

##### std::atomics

Section 55.1: atomic types
Each instantiation and full specialization of the std::atomic template deﬁnes an atomic type. If one thread writes
to an atomic object while another thread reads from it, the behavior is well-deﬁned (see memory model for details
on data races)
In addition, accesses to atomic objects may establish inter-thread synchronization and order non-atomic memory
accesses as speciﬁed by std::memory_order.
std::atomic may be instantiated with any TriviallyCopyable type T. std::atomic is neither copyable nor
movable.
The standard library provides specializations of the std::atomic template for the following types:
1.
One full specialization for the type bool and its typedef name is deﬁned that is treated as a non-specialized
std::atomic<T> except that it has standard layout, trivial default constructor, trivial destructors, and
supports aggregate initialization syntax:
Typedef name
Full specialization
std::atomic_bool std::atomic<bool>
2)Full specializations and typedefs for integral types, as follows:
Typedef name
Full specialization
std::atomic_char
std::atomic<char>
std::atomic_char
std::atomic<char>
std::atomic_schar
std::atomic<signed char>
std::atomic_uchar
std::atomic<unsigned char>
std::atomic_short
std::atomic<short>
std::atomic_ushort
std::atomic<unsigned short>
std::atomic_int
std::atomic<int>
std::atomic_uint
std::atomic<unsigned int>
std::atomic_long
std::atomic<long>
std::atomic_ulong
std::atomic<unsigned long>
std::atomic_llong
std::atomic<long long>
std::atomic_ullong
std::atomic<unsigned long long>
std::atomic_char16_t
std::atomic<char16_t>
std::atomic_char32_t
std::atomic<char32_t>
std::atomic_wchar_t
std::atomic<wchar_t>
std::atomic_int8_t
std::atomic<std::int8_t>
std::atomic_uint8_t
std::atomic<std::uint8_t>
std::atomic_int16_t
std::atomic<std::int16_t>
std::atomic_uint16_t
std::atomic<std::uint16_t>
std::atomic_int32_t
std::atomic<std::int32_t>
std::atomic_uint32_t
std::atomic<std::uint32_t>
std::atomic_int64_t
std::atomic<std::int64_t>
std::atomic_uint64_t
std::atomic<std::uint64_t>
std::atomic_int_least8_t
std::atomic<std::int_least8_t>
std::atomic_uint_least8_t std::atomic<std::uint_least8_t>
std::atomic_int_least16_t std::atomic<std::int_least16_t>
std::atomic_uint_least16_t std::atomic<std::uint_least16_t>
std::atomic_int_least32_t std::atomic<std::int_least32_t>
std::atomic_uint_least32_t std::atomic<std::uint_least32_t>
std::atomic_int_least64_t std::atomic<std::int_least64_t>
std::atomic_uint_least64_t std::atomic<std::uint_least64_t>
std::atomic_int_fast8_t
std::atomic<std::int_fast8_t>
std::atomic_uint_fast8_t
std::atomic<std::uint_fast8_t>
std::atomic_int_fast16_t
std::atomic<std::int_fast16_t>
std::atomic_uint_fast16_t std::atomic<std::uint_fast16_t>
std::atomic_int_fast32_t
std::atomic<std::int_fast32_t>
std::atomic_uint_fast32_t std::atomic<std::uint_fast32_t>
std::atomic_int_fast64_t
std::atomic<std::int_fast64_t>
std::atomic_uint_fast64_t std::atomic<std::uint_fast64_t>
std::atomic_intptr_t
std::atomic<std::intptr_t>
std::atomic_uintptr_t
std::atomic<std::uintptr_t>
std::atomic_size_t
std::atomic<std::size_t>
std::atomic_ptrdiff_t
std::atomic<std::ptrdiff_t>
std::atomic_intmax_t
std::atomic<std::intmax_t>
std::atomic_uintmax_t
std::atomic<std::uintmax_t>
Simple example of using std::atomic_int
```cpp
#include <iostream>       // std::cout
#include <atomic>         // std::atomic, std::memory_order_relaxed
#include <thread>         // std::thread
std::atomic_int foo (0);
void set_foo(int x) {
  foo.store(x,std::memory_order_relaxed);     // set value atomically
}
void print_foo() {
  int x;
  do {
    x = foo.load(std::memory_order_relaxed);  // get value atomically
  } while (x==0);
  std::cout << "foo: " << x << '\n';
}
int main ()
{
  std::thread first (print_foo);
  std::thread second (set_foo,10);
  first.join();
  //second.join();
  return 0;
}
//output: foo: 10


Before C++11, multithreading was platform-specific (pthreads, Windows threads). C++11 added a standard memory model and threading library.

***
```


### 1. THREADS

`std::thread` represents a single thread of execution.

```cpp
#include <thread>
#include <iostream>

void task(int id) {
    std::cout << "Thread " << id << " running\n";
}

int main() {
    std::thread t1(task, 1);
    std::thread t2(task, 2);

    // Must join or detach before destructor
    t1.join(); // Wait for finish
    t2.join();
    return 0;
}
```

***

### 2. MUTEX AND LOCKS

Protect shared data with `std::mutex`.

```cpp
#include <mutex>

std::mutex mtx;
int count = 0;

void safe_increment() {
    // RAII Lock: locks on construction, unlocks on destruction
    std::lock_guard<std::mutex> lock(mtx);
    count++;
}
```

***

### 3. ATOMICS

`std::atomic<T>` provides lock-free thread safety for simple types.

```cpp
#include <atomic>

std::atomic<int> counter(0);

void fast_increment() {
    counter++; // Atomic increment (hardware supported)
}
```

This avoids the overhead of mutexes for simple counters and flags.

***

### 4. ASYNC AND FUTURE

`std::async` runs a function asynchronously and returns a `std::future` that holds the result.

```cpp
#include <future>

int calculate() { return 42; }

int main() {
    // Launch async task
    std::future<int> result = std::async(std::launch::async, calculate);
    
    // Do other work...    
    // Get result (blocks if not ready)
    std::cout << result.get(); 
}
```

***

### 5. CONDITION VARIABLES

Used for thread synchronization (waiting for an event).

```cpp
std::condition_variable cv;
std::mutex cv_m;
bool ready = false;

void worker() {
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk, []{ return ready; }); // Wait until ready is true
    // process...
}

void signal() {
    {
        std::lock_guard<std::mutex> lk(cv_m);
        ready = true;
    }
    cv.notify_one();
}
```
### <a name="chapter-12-c11"></a>CHAPTER 12: C++11 STANDARD LIBRARY ADDITIONS

C++11 massively expanded the STL.

***

### 1. UNORDERED CONTAINERS (HASH MAPS)

C++98 `std::map` is a tree (O(log n)). C++11 added hash tables (O(1) average).

- `std::unordered_map`
- `std::unordered_set`
- `std::unordered_multimap`
- `std::unordered_multiset`

```cpp
#include <unordered_map>
std::unordered_map<std::string, int> scores;
scores["Alice"] = 100; // O(1)
```

They require a Hash function for the key type.

***

### 2. STD::ARRAY

`std::array` is a fixed-size wrapper around C-style arrays. It doesn't decay to a pointer automatically and knows its size.

```cpp
#include <array>
std::array<int, 5> arr = {1, 2, 3, 4, 5};
// arr.size() is 5
```

Prefer over C-arrays (`int arr[5]`).

***

### 3. STD::TUPLE

Generalization of `std::pair` for N elements.

```cpp
#include <tuple>
auto t = std::make_tuple(10, "Hello", 3.14);
int i = std::get<0>(t);
```

***

### 4. REGULAR EXPRESSIONS

`std::regex` provides regex matching and replacement.

```cpp
#include <regex>
std::regex pattern(R"(\d+)"); // Matches digits
bool match = std::regex_match("123", pattern);
```

***

### 5. CHRONO (TIME)

Type-safe time library.

```cpp
#include <chrono>
auto start = std::chrono::high_resolution_clock::now();
// ... work ...
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
```

***

### 6. RANDOM

Better random number generation than `rand()`.

```cpp
#include <random>
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, 6); // Dice roll
int roll = dis(gen);
```


### <a name="chapter-13-c11"></a>CHAPTER 13: C++11 METAPROGRAMMING


***
#### Professional Insights: Advanced Templates

##### Type Erasure

Type erasure is a set of techniques for creating a type that can provide a uniform interface to various underlying
types, while hiding the underlying type information from the client. std::function<R(A...)>, which has the ability
to hold callable objects of various types, is perhaps the best known example of type erasure in C++.
Section 90.1: A move-only `std::function`
std::function type erases down to a few operations. One of the things it requires is that the stored value be
copyable.
This causes problems in a few contexts, like lambdas storing unique ptrs. If you are using the std::function in a
context where copying doesn't matter, like a thread pool where you dispatch tasks to threads, this requirement can
add overhead.
In particular, std::packaged_task<Sig> is a callable object that is move-only. You can store a
std::packaged_task<R(Args...)> in a std::packaged_task<void(Args...)>, but that is a pretty heavy-weight and
obscure way to create a move-only callable type-erasure class.
Thus the task. This demonstrates how you could write a simple std::function type. I omitted the copy constructor
(which would involve adding a clone method to details::task_pimpl<...> as well).
```cpp
template<class Sig>
struct task;
// putting it in a namespace allows us to specialize it nicely for void return value:
namespace details {
  template<class R, class...Args>
  struct task_pimpl {
    virtual R invoke(Args&&...args) const = 0;
    virtual ~task_pimpl() {};
    virtual const std::type_info& target_type() const = 0;
  };
  // store an F.  invoke(Args&&...) calls the f
  template<class F, class R, class...Args>
  struct task_pimpl_impl:task_pimpl<R,Args...> {
    F f;
    template<class Fin>
    task_pimpl_impl( Fin&& fin ):f(std::forward<Fin>(fin)) {}
    virtual R invoke(Args&&...args) const final override {
      return f(std::forward<Args>(args)...);
    }
    virtual const std::type_info& target_type() const final override {
      return typeid(F);
    }
  };
  // the void version discards the return value of f:
  template<class F, class...Args>
  struct task_pimpl_impl<F,void,Args...>:task_pimpl<void,Args...> {
    F f;
    template<class Fin>
    task_pimpl_impl( Fin&& fin ):f(std::forward<Fin>(fin)) {}
    virtual void invoke(Args&&...args) const final override {
      f(std::forward<Args>(args)...);
    }
    virtual const std::type_info& target_type() const final override {
      return typeid(F);
    }
  };
};
template<class R, class...Args>
struct task<R(Args...)> {
  // semi-regular:
  task()=default;
  task(task&&)=default;
  // no copy
private:
  // aliases to make some SFINAE code below less ugly:
  template<class F>
  using call_r = std::result_of_t<F const&(Args...)>;
  template<class F>
  using is_task = std::is_same<std::decay_t<F>, task>;
public:
  // can be constructed from a callable F
  template<class F,
    // that can be invoked with Args... and converted-to-R:
    class= decltype( (R)(std::declval<call_r<F>>()) ),
    // and is not this same type:
    std::enable_if_t<!is_task<F>{}, int>* = nullptr
  >
  task(F&& f):
    m_pImpl( make_pimpl(std::forward<F>(f)) )
  {}
  // the meat: the call operator        
  R operator()(Args... args)const {
        return m_pImpl->invoke( std::forward<Args>(args)... );
  }
  explicit operator bool() const {
    return (bool)m_pImpl;
  }
  void swap( task& o ) {
    std::swap( m_pImpl, o.m_pImpl );
  }
  template<class F>
  void assign( F&& f ) {
    m_pImpl = make_pimpl(std::forward<F>(f));    
  }
  // Part of the std::function interface:
  const std::type_info& target_type() const {
    if (!*this) return typeid(void);
    return m_pImpl->target_type();
  }
  template< class T >
  T* target() {
    return target_impl<T>();
  }
  template< class T >
  const T* target() const {
    return target_impl<T>();
  }
  // compare with nullptr    :    
  friend bool operator==( std::nullptr_t, task const& self ) { return !self; }
  friend bool operator==( task const& self, std::nullptr_t ) { return !self; }
  friend bool operator!=( std::nullptr_t, task const& self ) { return !!self; }
  friend bool operator!=( task const& self, std::nullptr_t ) { return !!self; }
private:
  template<class T>
  using pimpl_t = details::task_pimpl_impl<T, R, Args...>;
  template<class F>
  static auto make_pimpl( F&& f ) {
    using dF=std::decay_t<F>;
    using pImpl_t = pimpl_t<dF>;
    return std::make_unique<pImpl_t>(std::forward<F>(f));
  }
  std::unique_ptr<details::task_pimpl<R,Args...>> m_pImpl;
  template< class T >
  T* target_impl() const {
    return dynamic_cast<pimpl_t<T>*>(m_pImpl.get());
  }
};
To make this library-worthy, you'd want to add in a small buﬀer optimization, so it does not store every callable on
the heap.
Adding SBO would require a non-default task(task&&), some std::aligned_storage_t within the class, a m_pImpl
unique_ptr with a deleter that can be set to destroy-only (and not return the memory to the heap), and a
emplace_move_to( void* ) = 0 in the task_pimpl.
live example of the above code (with no SBO).
Section 90.2: Erasing down to a Regular type with manual
vtable
C++ thrives on what is known as a Regular type (or at least Pseudo-Regular).
A Regular type is a type that can be constructed and assigned-to and assigned-from via copy or move, can be
destroyed, and can be compared equal-to. It can also be constructed from no arguments. Finally, it also has
support for a few other operations that are highly useful in various std algorithms and containers.
This is the root paper, but in C++11 would want to add std::hash support.
```

I will use the manual vtable approach to type erasure here.
using dtor_unique_ptr = std::unique_ptr<void, void(*)(void*)>;
```cpp
template<class T, class...Args>
dtor_unique_ptr make_dtor_unique_ptr( Args&&... args ) {
  return {new T(std::forward<Args>(args)...), [](void* self){ delete static_cast<T*>(self); }};
}
struct regular_vtable {
  void(*copy_assign)(void* dest, void const* src); // T&=(T const&)
  void(*move_assign)(void* dest, void* src); // T&=(T&&)
  bool(*equals)(void const* lhs, void const* rhs); // T const&==T const&
  bool(*order)(void const* lhs, void const* rhs); // std::less<T>{}(T const&, T const&)
  std::size_t(*hash)(void const* self); // std::hash<T>{}(T const&)
  std::type_info const&(*type)(); // typeid(T)
  dtor_unique_ptr(*clone)(void const* self); // T(T const&)
};
template<class T>
regular_vtable make_regular_vtable() noexcept {
  return {
    [](void* dest, void const* src){ *static_cast<T*>(dest) = *static_cast<T const*>(src); },
    [](void* dest, void* src){ *static_cast<T*>(dest) = std::move(*static_cast<T*>(src)); },
    [](void const* lhs, void const* rhs){ return *static_cast<T const*>(lhs) == *static_cast<T
const*>(rhs); },
    [](void const* lhs, void const* rhs) { return std::less<T>{}(*static_cast<T
const*>(lhs),*static_cast<T const*>(rhs)); },
    [](void const* self){ return std::hash<T>{}(*static_cast<T const*>(self)); },
    []()->decltype(auto){ return typeid(T); },
    [](void const* self){ return make_dtor_unique_ptr<T>(*static_cast<T const*>(self)); }
  };
}
template<class T>
regular_vtable const* get_regular_vtable() noexcept {
  static const regular_vtable vtable=make_regular_vtable<T>();
  return &vtable;
}
struct regular_type {
  using self=regular_type;
  regular_vtable const* vtable = 0;
  dtor_unique_ptr ptr{nullptr, [](void*){}};
  bool empty() const { return !vtable; }
  template<class T, class...Args>
  void emplace( Args&&... args ) {
    ptr = make_dtor_unique_ptr<T>(std::forward<Args>(args)...);
    if (ptr)
      vtable = get_regular_vtable<T>();
    else
      vtable = nullptr;
  }
  friend bool operator==(regular_type const& lhs, regular_type const& rhs) {
    if (lhs.vtable != rhs.vtable) return false;
    return lhs.vtable->equals( lhs.ptr.get(), rhs.ptr.get() );
  }
  bool before(regular_type const& rhs) const {
    auto const& lhs = *this;
    if (!lhs.vtable || !rhs.vtable)
      return std::less<regular_vtable const*>{}(lhs.vtable,rhs.vtable);
    if (lhs.vtable != rhs.vtable)
      return lhs.vtable->type().before(rhs.vtable->type());
    return lhs.vtable->order( lhs.ptr.get(), rhs.ptr.get() );
  }
  // technically friend bool operator< that calls before is also required
  std::type_info const* type() const {
    if (!vtable) return nullptr;
    return &vtable->type();
  }
  regular_type(regular_type&& o):
    vtable(o.vtable),
    ptr(std::move(o.ptr))
  {
    o.vtable = nullptr;
  }
  friend void swap(regular_type& lhs, regular_type& rhs){
    std::swap(lhs.ptr, rhs.ptr);
    std::swap(lhs.vtable, rhs.vtable);
  }
  regular_type& operator=(regular_type&& o) {
    if (o.vtable == vtable) {
      vtable->move_assign(ptr.get(), o.ptr.get());
      return *this;
    }
    auto tmp = std::move(o);
    swap(*this, tmp);
    return *this;
  }
  regular_type(regular_type const& o):
    vtable(o.vtable),
    ptr(o.vtable?o.vtable->clone(o.ptr.get()):dtor_unique_ptr{nullptr, [](void*){}})
  {
    if (!ptr && vtable) vtable = nullptr;
  }
  regular_type& operator=(regular_type const& o) {
    if (o.vtable == vtable) {
      vtable->copy_assign(ptr.get(), o.ptr.get());
      return *this;
    }
    auto tmp = o;
    swap(*this, tmp);
    return *this;
  }
  std::size_t hash() const {
    if (!vtable) return 0;
    return vtable->hash(ptr.get());
  }
  template<class T,
    std::enable_if_t< !std::is_same<std::decay_t<T>, regular_type>{}, int>* =nullptr
  >
  regular_type(T&& t) {
    emplace<std::decay_t<T>>(std::forward<T>(t));
  }
};
namespace std {
  template<>
  struct hash<regular_type> {
    std::size_t operator()( regular_type const& r )const {
      return r.hash();
    }
  };
  template<>
  struct less<regular_type> {
    bool operator()( regular_type const& lhs, regular_type const& rhs ) const {
      return lhs.before(rhs);
    }
  };
}    
live example.
Such a regular type can be used as a key for a std::map or a std::unordered_map that accepts anything regular for a
key, like:
std::map<regular_type, std::any>
would be basically a map from anothing regular, to anything copyable.
```

Unlike any, my regular_type does no small object optimization nor does it support getting the original data back.
Getting the original type back isn't hard.
Small object optimization requires that we store an aligned storage buﬀer within the regular_type, and carefully
tweak the deleter of the ptr to only destroy the object and not delete it.
I would start at make_dtor_unique_ptr and teach it how to sometimes store the data in a buﬀer, and then in the
heap if no room in the buﬀer. That may be suﬃcient.
Section 90.3: Basic mechanism
Type erasure is a way to hide the type of an object from code using it, even though it is not derived from a common
base class. In doing so, it provides a bridge between the worlds of static polymorphism (templates; at the place of
use, the exact type must be known at compile time, but it need not be declared to conform to an interface at
deﬁnition) and dynamic polymorphism (inheritance and virtual functions; at the place of use, the exact type need
not be known at compile time, but must be declared to conform to an interface at deﬁnition).
The following code shows the basic mechanism of type erasure.
```cpp
#include <ostream>
class Printable
{
public:
  template <typename T>
  Printable(T value) : pValue(new Value<T>(value)) {}
  ~Printable() { delete pValue; }
  void print(std::ostream &os) const { pValue->print(os); }
private:
  Printable(Printable const &)        /* in C++1x: =delete */; // not implemented
  void operator = (Printable const &) /* in C++1x: =delete */; // not implemented
  struct ValueBase
  {
      virtual ~ValueBase() = default;
      virtual void print(std::ostream &) const = 0;
  };
  template <typename T>
  struct Value : ValueBase
  {
      Value(T const &t) : v(t) {}
      virtual void print(std::ostream &os) const { os << v; }
      T v;
  };
  ValueBase *pValue;
};
At the use site, only the above deﬁnition need to be visible, just as with base classes with virtual functions. For
example:
#include <iostream>
void print_value(Printable const &p)
{
    p.print(std::cout);
}
Note that this is not a template, but a normal function that only needs to be declared in a header ﬁle, and can be
deﬁned in an implementation ﬁle (unlike templates, whose deﬁnition must be visible at the place of use).
At the deﬁnition of the concrete type, nothing needs to be known about Printable, it just needs to conform to an
interface, as with templates:
struct MyType { int i; };
ostream& operator << (ostream &os, MyType const &mc)
{
  return os << "MyType {" << mc.i << "}";
}
We can now pass an object of this class to the function deﬁned above:
MyType foo = { 42 };
print_value(foo);
Section 90.4: Erasing down to a contiguous buer of T
```

Not all type erasure involves virtual inheritance, allocations, placement new, or even function pointers.
What makes type erasure type erasure is that it describes a (set of) behavior(s), and takes any type that supports
that behavior and wraps it up. All information that isn't in that set of behaviors is "forgotten" or "erased".
An array_view takes its incoming range or container type and erases everything except the fact it is a contiguous
buﬀer of T.
// helper traits for SFINAE:
```cpp
template<class T>
using data_t = decltype( std::declval<T>().data() );
template<class Src, class T>
using compatible_data = std::integral_constant<bool, std::is_same< data_t<Src>, T* >{} ||
std::is_same< data_t<Src>, std::remove_const_t<T>* >{}>;
template<class T>
struct array_view {
  // the core of the class:
  T* b=nullptr;
  T* e=nullptr;
  T* begin() const { return b; }
  T* end() const { return e; }
  // provide the expected methods of a good contiguous range:
  T* data() const { return begin(); }
  bool empty() const { return begin()==end(); }
  std::size_t size() const { return end()-begin(); }
  T& operator[](std::size_t i)const{ return begin()[i]; }
  T& front()const{ return *begin(); }
  T& back()const{ return *(end()-1); }
  // useful helpers that let you generate other ranges from this one
  // quickly and safely:
  array_view without_front( std::size_t i=1 ) const {
    i = (std::min)(i, size());
    return {begin()+i, end()};
  }
  array_view without_back( std::size_t i=1 ) const {
    i = (std::min)(i, size());
    return {begin(), end()-i};
  }
  // array_view is plain old data, so default copy:
  array_view(array_view const&)=default;
  // generates a null, empty range:
  array_view()=default;
  // final constructor:
  array_view(T* s, T* f):b(s),e(f) {}
  // start and length is useful in my experience:
  array_view(T* s, std::size_t length):array_view(s, s+length) {}
  // SFINAE constructor that takes any .data() supporting container
  // or other range in one fell swoop:
  template<class Src,
    std::enable_if_t< compatible_data<std::remove_reference_t<Src>&, T >{}, int>* =nullptr,
    std::enable_if_t< !std::is_same<std::decay_t<Src>, array_view >{}, int>* =nullptr
  >
  array_view( Src&& src ):
    array_view( src.data(), src.size() )
  {}
  // array constructor:
  template<std::size_t N>
  array_view( T(&arr)[N] ):array_view(arr, N) {}
  // initializer list, allowing {} based:
  template<class U,
    std::enable_if_t< std::is_same<const U, T>{}, int>* =nullptr
  >
  array_view( std::initializer_list<U> il ):array_view(il.begin(), il.end()) {}
};
an array_view takes any container that supports .data() returning a pointer to T and a .size() method, or an
array, and erases it down to being a random-access range over contiguous Ts.
It can take a std::vector<T>, a std::string<T> a std::array<T, N> a T[37], an initializer list (including {} based
ones), or something else you make up that supports it (via T* x.data() and size_t x.size()).
In this case, the data we can extract from the thing we are erasing, together with our "view" non-owning state,
means we don't have to allocate memory or write custom type-dependent functions.
```

Live example.
An improvement would be to use a non-member data and a non-member size in an ADL-enabled context.
Section 90.5: Type erasing type erasure with std::any
This example uses C++14 and boost::any. In C++17 you can swap in std::any instead.
The syntax we end up with is:
const auto print =
  make_any_method<void(std::ostream&)>([](auto&& p, std::ostream& t){ t << p << "\n"; });
super_any<decltype(print)> a = 7;
(a->*print)(std::cout);
which is almost optimal.
This example is based oﬀ of work by @dyp and @cpplearner as well as my own.
First we use a tag to pass around types:
```cpp
template<class T>struct tag_t{constexpr tag_t(){};};
template<class T>constexpr tag_t<T> tag{};
This trait class gets the signature stored with an any_method:
This creates a function pointer type, and a factory for said function pointers, given an any_method:
template<class any_method>
using any_sig_from_method = typename any_method::signature;
template<class any_method, class Sig=any_sig_from_method<any_method>>
struct any_method_function;
template<class any_method, class R, class...Args>
struct any_method_function<any_method, R(Args...)>
{
  template<class T>
  using decorate = std::conditional_t< any_method::is_const, T const, T >;
  using any = decorate<boost::any>;
  using type = R(*)(any&, any_method const*, Args&&...);
  template<class T>
  type operator()( tag_t<T> )const{
    return +[](any& self, any_method const* method, Args&&...args) {
      return (*method)( boost::any_cast<decorate<T>&>(self), decltype(args)(args)... );
    };
  }
};
any_method_function::type is the type of a function pointer we will store alongside the instance.
any_method_function::operator() takes a tag_t<T> and writes a custom instance of the
any_method_function::type that assumes the any& is going to be a T.
We want to be able to type-erase more than one method at a time. So we bundle them up in a tuple, and write a
helper wrapper to stick the tuple into static storage on a per-type basis and maintain a pointer to them.
template<class...any_methods>
using any_method_tuple = std::tuple< typename any_method_function<any_methods>::type... >;
template<class...any_methods, class T>
any_method_tuple<any_methods...> make_vtable( tag_t<T> ) {
  return std::make_tuple(
    any_method_function<any_methods>{}(tag<T>)...
  );
}
template<class...methods>
struct any_methods {
private:
  any_method_tuple<methods...> const* vtable = 0;
  template<class T>
  static any_method_tuple<methods...> const* get_vtable( tag_t<T> ) {
    static const auto table = make_vtable<methods...>(tag<T>);
    return &table;
  }
public:
  any_methods() = default;
  template<class T>
  any_methods( tag_t<T> ): vtable(get_vtable(tag<T>)) {}
  any_methods& operator=(any_methods const&)=default;
  template<class T>
  void change_type( tag_t<T> ={} ) { vtable = get_vtable(tag<T>); }
  template<class any_method>
  auto get_invoker( tag_t<any_method> ={} ) const {
    return std::get<typename any_method_function<any_method>::type>( *vtable );
  }
};
We could specialize this for a cases where the vtable is small (for example, 1 item), and use direct pointers stored
in-class in those cases for eﬃciency.
```

Now we start the super_any. I use super_any_t to make the declaration of super_any a bit easier.
```cpp
template<class...methods>
struct super_any_t;
This searches the methods that the super any supports for SFINAE and better error messages:
template<class super_any, class method>
struct super_method_applies_helper : std::false_type {};
template<class M0, class...Methods, class method>
struct super_method_applies_helper<super_any_t<M0, Methods...>, method> :
    std::integral_constant<bool, std::is_same<M0, method>{}  ||
super_method_applies_helper<super_any_t<Methods...>, method>{}>
{};
template<class...methods, class method>
auto super_method_test( super_any_t<methods...> const&, tag_t<method> )
{
  return std::integral_constant<bool, super_method_applies_helper< super_any_t<methods...>, method
>{} && method::is_const >{};
}
template<class...methods, class method>
auto super_method_test( super_any_t<methods...>&, tag_t<method> )
{
  return std::integral_constant<bool, super_method_applies_helper< super_any_t<methods...>, method
>{} >{};
}
template<class super_any, class method>
struct super_method_applies:
    decltype( super_method_test( std::declval<super_any>(), tag<method> ) )
{};
Next we create the any_method type. An any_method is a pseudo-method-pointer. We create it globally and constly
using syntax like:
const auto print=make_any_method( [](auto&&self, auto&&os){ os << self; } );
or in C++17:
const any_method print=[](auto&&self, auto&&os){ os << self; };
Note that using a non-lambda can make things hairy, as we use the type for a lookup step. This can be ﬁxed, but
would make this example longer than it already is. So always initialize an any method from a lambda, or from a
type parametarized on a lambda.
template<class Sig, bool const_method, class F>
struct any_method {
  using signature=Sig;
  enum{is_const=const_method};
private:
  F f;
public:
  template<class Any,
    // SFINAE testing that one of the Anys's matches this type:
    std::enable_if_t< super_method_applies< Any&&, any_method >{}, int>* =nullptr
  >
  friend auto operator->*( Any&& self, any_method const& m ) {
    // we don't use the value of the any_method, because each any_method has
    // a unique type (!) and we check that one of the auto*'s in the super_any
    // already has a pointer to us.  We then dispatch to the corresponding
    // any_method_data...
    return [&self, invoke = self.get_invoker(tag<any_method>), m](auto&&...args)->decltype(auto)
    {
      return invoke( decltype(self)(self), &m, decltype(args)(args)... );
    };
  }
  any_method( F fin ):f(std::move(fin)) {}
  template<class...Args>
  decltype(auto) operator()(Args&&...args)const {
    return f(std::forward<Args>(args)...);
  }
};
A factory method, not needed in C++17 I believe:
template<class Sig, bool is_const=false, class F>
any_method<Sig, is_const, std::decay_t<F>>
make_any_method( F&& f ) {
  return {std::forward<F>(f)};
}
This is the augmented any. It is both an any, and it carries around a bundle of type-erasure function pointers that
change whenever the contained any does:
template<class... methods>
struct super_any_t:boost::any, any_methods<methods...> {
  using vtable=any_methods<methods...>;
public:
  template<class T,
    std::enable_if_t< !std::is_base_of<super_any_t, std::decay_t<T>>{}, int> =0
  >
  super_any_t( T&& t ):
    boost::any( std::forward<T>(t) )
  {
    using dT=std::decay_t<T>;
    this->change_type( tag<dT> );
  }
  boost::any& as_any()&{return *this;}
  boost::any&& as_any()&&{return std::move(*this);}
  boost::any const& as_any()const&{return *this;}
  super_any_t()=default;
  super_any_t(super_any_t&& o):
    boost::any( std::move( o.as_any() ) ),
    vtable(o)
  {}
  super_any_t(super_any_t const& o):
    boost::any( o.as_any() ),
    vtable(o)
  {}
  template<class S,
    std::enable_if_t< std::is_same<std::decay_t<S>, super_any_t>{}, int> =0
  >
  super_any_t( S&& o ):
    boost::any( std::forward<S>(o).as_any() ),
    vtable(o)
  {}
  super_any_t& operator=(super_any_t&&)=default;
  super_any_t& operator=(super_any_t const&)=default;
  template<class T,
    std::enable_if_t< !std::is_same<std::decay_t<T>, super_any_t>{}, int>* =nullptr
  >
  super_any_t& operator=( T&& t ) {
    ((boost::any&)*this) = std::forward<T>(t);
    using dT=std::decay_t<T>;
    this->change_type( tag<dT> );
    return *this;
  }  
};
Because we store the any_methods as const objects, this makes making a super_any a bit easier:
template<class...Ts>
using super_any = super_any_t< std::remove_cv_t<Ts>... >;
Test code:
const auto print = make_any_method<void(std::ostream&)>([](auto&& p, std::ostream& t){ t << p <<
"\n"; });
const auto wprint = make_any_method<void(std::wostream&)>([](auto&& p, std::wostream& os ){ os << p
<< L"\n"; });
int main()
{
  super_any<decltype(print), decltype(wprint)> a = 7;
  super_any<decltype(print), decltype(wprint)> a2 = 7;
  (a->*print)(std::cout);
  (a->*wprint)(std::wcout);
}
live example.
```

Originally posted here in a SO self question & answer (and people noted above helped with the implementation).

##### Perfect Forwarding

Section 101.1: Factory functions
Suppose we want to write a factory function that accepts an arbitrary list of arguments and passes those
arguments unmodiﬁed to another function. An example of such a function is make_unique, which is used to safely
construct a new instance of T and return a unique_ptr<T> that owns the instance.
The language rules regarding variadic templates and rvalue references allows us to write such a function.
```cpp
template<class T, class... A>
unique_ptr<T> make_unique(A&&... args)
{
    return unique_ptr<T>(new T(std::forward<A>(args)...));
}
The use of ellipses ... indicate a parameter pack, which represents an arbitrary number of types. The compiler will
expand this parameter pack to the correct number of arguments at the call site. These arguments are then passed
to T's constructor using std::forward. This function is required to preserve the ref-qualiﬁers of the arguments.
struct foo
{
    foo() {}
    foo(const foo&) {}                    // copy constructor
    foo(foo&&) {}                         // copy constructor
    foo(int, int, int) {}
};
foo f;
auto p1 = make_unique<foo>(f);            // calls foo::foo(const foo&)
auto p2 = make_unique<foo>(std::move(f)); // calls foo::foo(foo&&)
auto p3 = make_unique<foo>(1, 2, 3);
```


##### SFINAE (Substitution Failure Is

Not An Error)
Section 103.1: What is SFINAE
SFINAE stands for Substitution Failure Is Not An Error. Ill-formed code that results from substituting types (or
values) to instantiate a function template or a class template is not a hard compile error, it is only treated as a
deduction failure.
Deduction failures on instantiating function templates or class template specializations remove that candidate from
the set of consideration - as if that failed candidate did not exist to begin with.
```cpp
template <class T>
auto begin(T& c) -> decltype(c.begin()) { return c.begin(); }
template <class T, size_t N>
T* begin(T (&arr)[N]) { return arr; }
int vals[10];
begin(vals); // OK. The first function template substitution fails because
             // vals.begin() is ill-formed. This is not an error! That function
             // is just removed from consideration as a viable overload candidate,
             // leaving us with the array overload.
Only substitution failures in the immediate context are considered deduction failures, all others are considered
hard errors.
template <class T>
void add_one(T& val) { val += 1; }
int i = 4;
add_one(i); // ok
std::string msg = "Hello";
add_one(msg); // error. msg += 1 is ill-formed for std::string, but this
              // failure is NOT in the immediate context of substituting T
Section 103.2: void_t
Version ≥ C++11
void_t is a meta-function that maps any (number of) types to type void. The primary purpose of void_t is to
facilitate writing of type traits.
std::void_t will be part of C++17, but until then, it is extremely straightforward to implement:
template <class...> using void_t = void;
Some compilers require a slightly diﬀerent implementation:
template <class...>
struct make_void { using type = void; };
template <typename... T>
using void_t = typename make_void<T...>::type;
The primary application of void_t is writing type traits that check validity of a statement. For example, let's check if
a type has a member function foo() that takes no arguments:
template <class T, class=void>
struct has_foo : std::false_type {};
template <class T>
struct has_foo<T, void_t<decltype(std::declval<T&>().foo())>> : std::true_type {};
How does this work? When I try to instantiate has_foo<T>::value, that will cause the compiler to try to look for the
best specialization for has_foo<T, void>. We have two options: the primary, and this secondary one which involves
having to instantiate that underlying expression:
If T does have a member function foo(), then whatever type that returns gets converted to void, and the
specialization is preferred to the primary based on the template partial ordering rules. So
has_foo<T>::value will be true
If T doesn't have such a member function (or it requires more than one argument), then substitution fails for
the specialization and we only have the primary template to fallback on. Hence, has_foo<T>::value is false.
A simpler case:
template<class T, class=void>
struct can_reference : std::false_type {};
template<class T>
struct can_reference<T, std::void_t<T&>> : std::true_type {};
this doesn't use std::declval or decltype.
You may notice a common pattern of a void argument. We can factor this out:
struct details {
  template<template<class...>class Z, class=void, class...Ts>
  struct can_apply:
    std::false_type
  {};
  template<template<class...>class Z, class...Ts>
  struct can_apply<Z, std::void_t<Z<Ts...>>, Ts...>:
    std::true_type
  {};
};
template<template<class...>class Z, class...Ts>
using can_apply = details::can_apply<Z, void, Ts...>;
which hides the use of std::void_t and makes can_apply act like an indicator whether the type supplied as the
ﬁrst template argument is well-formed after substituting the other types into it. The previous examples may now be
rewritten using can_apply as:
template<class T>
using ref_t = T&;
template<class T>
using can_reference = can_apply<ref_t, T>;    // Is T& well formed for T?
and:
template<class T>
using dot_foo_r = decltype(std::declval<T&>().foo());
template<class T>
using can_dot_foo = can_apply< dot_foo_r, T >;    // Is T.foo() well formed for T?
which seems simpler than the original versions.
There are post-C++17 proposals for std traits similar to can_apply.
```

The utility of void_t was discovered by Walter Brown. He gave a wonderful presentation on it at CppCon 2016.
Section 103.3: enable_if
std::enable_if is a convenient utility to use boolean conditions to trigger SFINAE. It is deﬁned as:
```cpp
template <bool Cond, typename Result=void>
struct enable_if { };
template <typename Result>
struct enable_if<true, Result> {
    using type = Result;
};
That is, enable_if<true, R>::type is an alias for R, whereas enable_if<false, T>::type is ill-formed as that
specialization of enable_if does not have a type member type.
std::enable_if can be used to constrain templates:
int negate(int i) { return -i; }
template <class F>
auto negate(F f) { return -f(); }
Here, a call to negate(1) would fail due to ambiguity. But the second overload is not intended to be used for
integral types, so we can add:
int negate(int i) { return -i; }
template <class F, class = typename std::enable_if<!std::is_arithmetic<F>::value>::type>
auto negate(F f) { return -f(); }
Now, instantiating negate<int> would result in a substitution failure since !std::is_arithmetic<int>::value is
false. Due to SFINAE, this is not a hard error, this candidate is simply removed from the overload set. As a result,
negate(1) only has one single viable candidate - which is then called.
When to use it
It's worth keeping in mind that std::enable_if is a helper on top of SFINAE, but it's not what makes SFINAE work in
the ﬁrst place. Let's consider these two alternatives for implementing functionality similar to std::size, i.e. an
overload set size(arg) that produces the size of a container or array:
// for containers
template<typename Cont>
auto size1(Cont const& cont) -> decltype( cont.size() );
// for arrays
template<typename Elt, std::size_t Size>
std::size_t size1(Elt const(&arr)[Size]);
// implementation omitted
template<typename Cont>
struct is_sizeable;
// for containers
template<typename Cont, std::enable_if_t<std::is_sizeable<Cont>::value, int> = 0>
auto size2(Cont const& cont);
// for arrays
template<typename Elt, std::size_t Size>
std::size_t size2(Elt const(&arr)[Size]);
Assuming that is_sizeable is written appropriately, these two declarations should be exactly equivalent with
respect to SFINAE. Which is the easiest to write, and which is the easiest to review and understand at a glance?
Now let's consider how we might want to implement arithmetic helpers that avoid signed integer overﬂow in favour
of wrap around or modular behaviour. Which is to say that e.g. incr(i, 3) would be the same as i += 3 save for
the fact that the result would always be deﬁned even if i is an int with value INT_MAX. These are two possible
alternatives:
// handle signed types
template<typename Int>
auto incr1(Int& target, Int amount)
-> std::void_t<int[static_cast<Int>(-1) < static_cast<Int>(0)]>;
// handle unsigned types by just doing target += amount
// since unsigned arithmetic already behaves as intended
template<typename Int>
auto incr1(Int& target, Int amount)
-> std::void_t<int[static_cast<Int>(0) < static_cast<Int>(-1)]>;
template<typename Int, std::enable_if_t<std::is_signed<Int>::value, int> = 0>
void incr2(Int& target, Int amount);
template<typename Int, std::enable_if_t<std::is_unsigned<Int>::value, int> = 0>
void incr2(Int& target, Int amount);
Once again which is the easiest to write, and which is the easiest to review and understand at a glance?
A strength of std::enable_if is how it plays with refactoring and API design. If is_sizeable<Cont>::value is
meant to reﬂect whether cont.size() is valid then just using the expression as it appears for size1 can be more
concise, although that could depend on whether is_sizeable would be used in several places or not. Contrast that
with std::is_signed which reﬂects its intention much more clearly than when its implementation leaks into the
declaration of incr1.
Section 103.4: is_detected
To generalize type_trait creation:based on SFINAE there are experimental traits detected_or, detected_t,
is_detected.
With template parameters typename Default, template <typename...> Op and typename ... Args:
is_detected: alias of std::true_type or std::false_type depending of the validity of Op<Args...>
detected_t: alias of Op<Args...> or nonesuch depending of validity of Op<Args...>.
detected_or: alias of a struct with value_t which is is_detected, and type which is Op<Args...> or Default
depending of validity of Op<Args...>
which can be implemented using std::void_t for SFINAE as following:
Version ≥ C++17
namespace detail {
    template <class Default, class AlwaysVoid,
              template<class...> class Op, class... Args>
    struct detector
    {
        using value_t = std::false_type;
        using type = Default;
    };
    template <class Default, template<class...> class Op, class... Args>
    struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
    {
        using value_t = std::true_type;
        using type = Op<Args...>;
    };
} // namespace detail
// special type to indicate detection failure
struct nonesuch {
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
};
template <template<class...> class Op, class... Args>
using is_detected =
    typename detail::detector<nonesuch, void, Op, Args...>::value_t;
template <template<class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;
template <class Default, template<class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;
Traits to detect presence of method can then be simply implemented:
typename <typename T, typename ...Ts>
using foo_type = decltype(std::declval<T>().foo(std::declval<Ts>()...));
struct C1 {};
struct C2 {
    int foo(char) const;
};
template <typename T>
using has_foo_char = is_detected<foo_type, T, char>;
static_assert(!has_foo_char<C1>::value, "Unexpected");
static_assert(has_foo_char<C2>::value, "Unexpected");
static_assert(std::is_same<int, detected_t<foo_type, C2, char>>::value,
              "Unexpected");
static_assert(std::is_same<void, // Default
                           detected_or<void, foo_type, C1, char>>::value,
              "Unexpected");
static_assert(std::is_same<int, detected_or<void, foo_type, C2, char>>::value,
              "Unexpected");
Section 103.5: Overload resolution with a large number of
options
If you need to select between several options, enabling just one via enable_if<> can be quite cumbersome, since
several conditions needs to be negated too.
```

The ordering between overloads can instead be selected using inheritance, i.e. tag dispatch.
Instead of testing for the thing that needs to be well-formed, and also testing the negation of all the other versions
conditions, we instead test just for what we need, preferably in a decltype in a trailing return.
This might leave several option well formed, we diﬀerentiate between those using 'tags', similar to iterator-trait tags
(random_access_tag et al). This works because a direct match is better that a base class, which is better that a base
class of a base class, etc.
```cpp
#include <algorithm>
#include <iterator>
namespace detail
{
    // this gives us infinite types, that inherit from each other
    template<std::size_t N>
    struct pick : pick<N-1> {};
    template<>
    struct pick<0> {};
    // the overload we want to be preferred have a higher N in pick<N>
    // this is the first helper template function
    template<typename T>
    auto stable_sort(T& t, pick<2>)
        -> decltype( t.stable_sort(), void() )
    {
        // if the container have a member stable_sort, use that
        t.stable_sort();
    }
    // this helper will be second best match
    template<typename T>
    auto stable_sort(T& t, pick<1>)
        -> decltype( t.sort(), void() )
    {
        // if the container have a member sort, but no member stable_sort
        // it's customary that the sort member is stable
        t.sort();
    }
    // this helper will be picked last
    template<typename T>
    auto stable_sort(T& t, pick<0>)
        -> decltype( std::stable_sort(std::begin(t), std::end(t)), void() )
    {
        // the container have neither a member sort, nor member stable_sort
        std::stable_sort(std::begin(t), std::end(t));
    }
}
// this is the function the user calls. it will dispatch the call
// to the correct implementation with the help of 'tags'.
template<typename T>
void stable_sort(T& t)
{
    // use an N that is higher that any used above.
    // this will pick the highest overload that is well formed.
    detail::stable_sort(t, detail::pick<10>{});
}
There are other methods commonly used to diﬀerentiate between overloads, such as exact match being better
than conversion, being better than ellipsis.
```

However, tag-dispatch can extend to any number of choices, and is a bit more clear in intent.
Section 103.6: trailing decltype in function templates
Version ≥ C++11
One of constraining function is to use trailing decltype to specify the return type:
namespace details {
```cpp
   using std::to_string;
   // this one is constrained on being able to call to_string(T)
   template <class T>
   auto convert_to_string(T const& val, int )
       -> decltype(to_string(val))
   {
       return to_string(val);
   }
   // this one is unconstrained, but less preferred due to the ellipsis argument
   template <class T>
   std::string convert_to_string(T const& val, ... )
   {
       std::ostringstream oss;
       oss << val;
       return oss.str();
   }
}
template <class T>
std::string convert_to_string(T const& val)
{
    return details::convert_to_string(val, 0);
}
If I call convert_to_string() with an argument with which I can invoke to_string(), then I have two viable
functions for details::convert_to_string(). The ﬁrst is preferred since the conversion from 0 to int is a better
implicit conversion sequence than the conversion from 0 to ...
If I call convert_to_string() with an argument from which I cannot invoke to_string(), then the ﬁrst function
template instantiation leads to substitution failure (there is no decltype(to_string(val))). As a result, that
candidate is removed from the overload set. The second function template is unconstrained, so it is selected and
we instead go through operator<<(std::ostream&, T). If that one is undeﬁned, then we have a hard compile error
with a template stack on the line oss << val.
Section 103.7: enable_if_all / enable_if_any
Version ≥ C++11
Motivational example
When you have a variadic template pack in the template parameters list, like in the following code snippet:
template<typename ...Args> void func(Args &&...args) { //... };
The standard library (prior to C++17) oﬀers no direct way to write enable_if to impose SFINAE constraints on all of
the parameters in Args or any of the parameters in Args. C++17 oﬀers std::conjunction and std::disjunction
which solve this problem. For example:
/// C++17: SFINAE constraints on all of the parameters in Args.
template<typename ...Args,
         std::enable_if_t<std::conjunction_v<custom_conditions_v<Args>...>>* = nullptr>
void func(Args &&...args) { //... };
/// C++17: SFINAE constraints on any of the parameters in Args.
template<typename ...Args,
         std::enable_if_t<std::disjunction_v<custom_conditions_v<Args>...>>* = nullptr>
void func(Args &&...args) { //... };
If you do not have C++17 available, there are several solutions to achieve these. One of them is to use a base-case
class and partial specializations, as demonstrated in answers of this question.
Alternatively, one may also implement by hand the behavior of std::conjunction and std::disjunction in a
rather straight-forward way. In the following example I'll demonstrate the implementations and combine them with
std::enable_if to produce two alias: enable_if_all and enable_if_any, which do exactly what they are supposed
to semantically. This may provide a more scalable solution.
Implementation of enable_if_all and enable_if_any
First let's emulate std::conjunction and std::disjunction using customized seq_and and seq_or respectively:
/// Helper for prior to C++14.
template<bool B, class T, class F >
using conditional_t = typename std::conditional<B,T,F>::type;
/// Emulate C++17 std::conjunction.
template<bool...> struct seq_or: std::false_type {};
template<bool...> struct seq_and: std::true_type {};
template<bool B1, bool... Bs>
struct seq_or<B1,Bs...>:
  conditional_t<B1,std::true_type,seq_or<Bs...>> {};
template<bool B1, bool... Bs>
struct seq_and<B1,Bs...>:
  conditional_t<B1,seq_and<Bs...>,std::false_type> {};  
Then the implementation is quite straight-forward:
template<bool... Bs>
using enable_if_any = std::enable_if<seq_or<Bs...>::value>;
template<bool... Bs>
using enable_if_all = std::enable_if<seq_and<Bs...>::value>;
Eventually some helpers:
template<bool... Bs>
using enable_if_any_t = typename enable_if_any<Bs...>::type;
template<bool... Bs>
using enable_if_all_t = typename enable_if_all<Bs...>::type;
Usage
The usage is also straight-forward:
    /// SFINAE constraints on all of the parameters in Args.
    template<typename ...Args,
             enable_if_all_t<custom_conditions_v<Args>...>* = nullptr>
    void func(Args &&...args) { //... };
    /// SFINAE constraints on any of the parameters in Args.
    template<typename ...Args,
             enable_if_any_t<custom_conditions_v<Args>...>* = nullptr>
    void func(Args &&...args) { //... };



***
```

#### Professional Insights: Metaprogramming Techniques

##### Metaprogramming

In C++ Metaprogramming refers to the use of macros or templates to generate code at compile-time.
In general, macros are frowned upon in this role and templates are preferred, although they are not as generic.
Template metaprogramming often makes use of compile-time computations, whether via templates or constexpr
functions, to achieve its goals of generating code, however compile-time computations are not metaprogramming
per se.
Section 16.1: Calculating Factorials
Factorials can be computed at compile-time using template metaprogramming techniques.
```cpp
#include <iostream>
template<unsigned int n>
struct factorial
{
    enum
    {
        value = n * factorial<n - 1>::value
    };
};
template<>
struct factorial<0>
{
    enum { value = 1 };
};
int main()
{
    std::cout << factorial<7>::value << std::endl;    // prints "5040"
}
factorial is a struct, but in template metaprogramming it is treated as a template metafunction. By convention,
template metafunctions are evaluated by checking a particular member, either ::type for metafunctions that result
in types, or ::value for metafunctions that generate values.
In the above code, we evaluate the factorial metafunction by instantiating the template with the parameters we
want to pass, and using ::value to get the result of the evaluation.
The metafunction itself relies on recursively instantiating the same metafunction with smaller values. The
factorial<0> specialization represents the terminating condition. Template metaprogramming has most of the
restrictions of a functional programming language, so recursion is the primary "looping" construct.
Since template metafunctions execute at compile time, their results can be used in contexts that require compile-
time values. For example:
int my_array[factorial<5>::value];
Automatic arrays must have a compile-time deﬁned size. And the result of a metafunction is a compile-time
constant, so it can be used here.
Limitation: Most of the compilers won't allow recursion depth beyond a limit. For example, g++ compiler by default
limits recursion depeth to 256 levels. In case of g++, programmer can set recursion depth using -ftemplate-depth-
```

X option.
Version ≥ C++11
Since C++11, the std::integral_constant template can be used for this kind of template computation:
```cpp
#include <iostream>
#include <type_traits>
template<long long n>
struct factorial :
  std::integral_constant<long long, n * factorial<n - 1>::value> {};
template<>
struct factorial<0> :
  std::integral_constant<long long, 1> {};
int main()
{
    std::cout << factorial<7>::value << std::endl;    // prints "5040"
}
```

Additionally, constexpr functions become a cleaner alternative.
```cpp
#include <iostream>
constexpr long long factorial(long long n)
{
  return (n == 0) ? 1 : n * factorial(n - 1);
}
int main()
{
  char test[factorial(3)];
  std::cout << factorial(7) << '\n';
}
The body of factorial() is written as a single statement because in C++11 constexpr functions can only use a
quite limited subset of the language.
Version ≥ C++14
Since C++14, many restrictions for constexpr functions have been dropped and they can now be written much
more conveniently:
constexpr long long factorial(long long n)
{
  if (n == 0)
    return 1;
  else
    return n * factorial(n - 1);
}
Or even:
constexpr long long factorial(int n)
{
  long long result = 1;
  for (int i = 1; i <= n; ++i) {
    result *= i;
  }
  return result;
}
Version ≥ C++17
Since c++17 one can use fold expression to calculate factorial:
#include <iostream>
#include <utility>
template <class T, T N, class I = std::make_integer_sequence<T, N>>
struct factorial;
template <class T, T N, T... Is>
struct factorial<T,N,std::index_sequence<T, Is...>> {
   static constexpr T value = (static_cast<T>(1) * ... * (Is + 1));
};
int main() {
   std::cout << factorial<int, 5>::value << std::endl;
}
Section 16.2: Iterating over a parameter pack
Often, we need to perform an operation over every element in a variadic template parameter pack. There are many
ways to do this, and the solutions get easier to read and write with C++17. Suppose we simply want to print every
element in a pack. The simplest solution is to recurse:
Version ≥ C++11
void print_all(std::ostream& os) {
    // base case
}
template <class T, class... Ts>
void print_all(std::ostream& os, T const& first, Ts const&... rest) {
    os << first;
    print_all(os, rest...);
}
We could instead use the expander trick, to perform all the streaming in a single function. This has the advantage of
not needing a second overload, but has the disadvantage of less than stellar readability:
Version ≥ C++11
template <class... Ts>
void print_all(std::ostream& os, Ts const&... args) {
    using expander = int[];
    (void)expander{0,
        (void(os << args), 0)...
    };
}
```

For an explanation of how this works, see T.C's excellent answer.
Version ≥ C++17
With C++17, we get two powerful new tools in our arsenal for solving this problem. The ﬁrst is a fold-expression:
```cpp
template <class... Ts>
void print_all(std::ostream& os, Ts const&... args) {
    ((os << args), ...);
}
And the second is if constexpr, which allows us to write our original recursive solution in a single function:
template <class T, class... Ts>
void print_all(std::ostream& os, T const& first, Ts const&... rest) {
    os << first;
    if constexpr (sizeof...(rest) > 0) {        
        // this line will only be instantiated if there are further
        // arguments. if rest... is empty, there will be no call to
        // print_all(os).
        print_all(os, rest...);
    }
}
Section 16.3: Iterating with std::integer_sequence
Since C++14, the standard provides the class template
template <class T, T... Ints>
class integer_sequence;
template <std::size_t... Ints>
using index_sequence = std::integer_sequence<std::size_t, Ints...>;
and a generating metafunction for it:
template <class T, T N>
using make_integer_sequence = std::integer_sequence<T, /* a sequence 0, 1, 2, ..., N-1 */ >;
template<std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;
While this comes standard in C++14, this can be implemented using C++11 tools.
We can use this tool to call a function with a std::tuple of arguments (standardized in C++17 as std::apply):
namespace detail {
    template <class F, class Tuple, std::size_t... Is>
    decltype(auto) apply_impl(F&& f, Tuple&& tpl, std::index_sequence<Is...> ) {
        return std::forward<F>(f)(std::get<Is>(std::forward<Tuple>(tpl))...);
    }
}
template <class F, class Tuple>
decltype(auto) apply(F&& f, Tuple&& tpl) {
    return detail::apply_impl(std::forward<F>(f),
        std::forward<Tuple>(tpl),
        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}
// this will print 3
int f(int, char, double);
auto some_args = std::make_tuple(42, 'x', 3.14);
int r = apply(f, some_args); // calls f(42, 'x', 3.14)
Section 16.4: Tag Dispatching
A simple way of selecting between functions at compile time is to dispatch a function to an overloaded pair of
functions that take a tag as one (usually the last) argument. For example, to implement std::advance(), we can
dispatch on the iterator category:
namespace details {
    template <class RAIter, class Distance>
    void advance(RAIter& it, Distance n, std::random_access_iterator_tag) {
        it += n;
    }
    template <class BidirIter, class Distance>
    void advance(BidirIter& it, Distance n, std::bidirectional_iterator_tag) {
        if (n > 0) {
            while (n--) ++it;
        }
        else {
            while (n++) --it;
        }
    }
    template <class InputIter, class Distance>
    void advance(InputIter& it, Distance n, std::input_iterator_tag) {
        while (n--) {
            ++it;
        }
    }    
}
template <class Iter, class Distance>
void advance(Iter& it, Distance n) {
    details::advance(it, n,
            typename std::iterator_traits<Iter>::iterator_category{} );
}
The std::XY_iterator_tag arguments of the overloaded details::advance functions are unused function
parameters. The actual implementation does not matter (actually it is completely empty). Their only purpose is to
allow the compiler to select an overload based on which tag class details::advance is called with.
In this example, advance uses the iterator_traits<T>::iterator_category metafunction which returns one of
the iterator_tag classes, depending on the actual type of Iter. A default-constructed object of the
iterator_category<Iter>::type then lets the compiler select one of the diﬀerent overloads of details::advance.
(This function parameter is likely to be completely optimized away, as it is a default-constructed object of an empty
struct and never used.)
```

Tag dispatching can give you code that's much easier to read than the equivalents using SFINAE and enable_if.
Note: while C++17's if constexpr may simplify the implementation of advance in particular, it is not suitable for open
implementations unlike tag dispatching.
Section 16.5: Detect Whether Expression is Valid
It is possible to detect whether an operator or function can be called on a type. To test if a class has an overload of
std::hash, one can do this:
```cpp
#include <functional> // for std::hash
#include <type_traits> // for std::false_type and std::true_type
#include <utility> // for std::declval
template<class, class = void>
struct has_hash
    : std::false_type
{};
template<class T>
struct has_hash<T, decltype(std::hash<T>()(std::declval<T>()), void())>
    : std::true_type
{};
Version ≥ C++17
Since C++17, std::void_t can be used to simplify this type of construct
#include <functional> // for std::hash
#include <type_traits> // for std::false_type, std::true_type, std::void_t
#include <utility> // for std::declval
template<class, class = std::void_t<> >
struct has_hash
    : std::false_type
{};
template<class T>
struct has_hash<T, std::void_t< decltype(std::hash<T>()(std::declval<T>())) > >
    : std::true_type
{};
where std::void_t is deﬁned as:
template< class... > using void_t = void;
For detecting if an operator, such as operator< is deﬁned, the syntax is almost the same:
template<class, class = void>
struct has_less_than
    : std::false_type
{};
template<class T>
struct has_less_than<T, decltype(std::declval<T>() < std::declval<T>(), void())>
    : std::true_type
{};
These can be used to use a std::unordered_map<T> if T has an overload for std::hash, but otherwise attempt to
use a std::map<T>:
template <class K, class V>
using hash_invariant_map = std::conditional_t<
    has_hash<K>::value,
    std::unordered_map<K, V>,
    std::map<K,V>>;    
Section 16.6: If-then-else
Version ≥ C++11
The type std::conditional in the standard library header <type_traits> can select one type or the other, based
on a compile-time boolean value:
template<typename T>
struct ValueOrPointer
{
    typename std::conditional<(sizeof(T) > sizeof(void*)), T*, T>::type vop;
};
This struct contains a pointer to T if T is larger than the size of a pointer, or T itself if it is smaller or equal to a
pointer's size. Therefore sizeof(ValueOrPointer) will always be <= sizeof(void*).
Section 16.7: Manual distinction of types when given any type
T
When implementing SFINAE using std::enable_if, it is often useful to have access to helper templates that
determines if a given type T matches a set of criteria.
To help us with that, the standard already provides two types analog to true and false which are std::true_type
and std::false_type.
The following example show how to detect if a type T is a pointer or not, the is_pointer template mimic the
behavior of the standard std::is_pointer helper:
template <typename T>
struct is_pointer_: std::false_type {};
template <typename T>
struct is_pointer_<T*>: std::true_type {};
template <typename T>
struct is_pointer: is_pointer_<typename std::remove_cv<T>::type> { }
There are three steps in the above code (sometimes you only need two):
1.
The ﬁrst declaration of is_pointer_ is the default case, and inherits from std::false_type. The default case
should always inherit from std::false_type since it is analogous to a "false condition".
2.
The second declaration specialize the is_pointer_ template for pointer T* without caring about what T is
really. This version inherits from std::true_type.
3.
The third declaration (the real one) simply remove any unnecessary information from T (in this case we
remove const and volatile qualiﬁers) and then fall backs to one of the two previous declarations.
Since is_pointer<T> is a class, to access its value you need to either:
Use ::value, e.g. is_pointer<int>::value – value is a static class member of type bool inherited from
std::true_type or std::false_type;
Construct an object of this type, e.g. is_pointer<int>{} – This works because std::is_pointer inherits its
default constructor from std::true_type or std::false_type (which have constexpr constructors) and both
std::true_type and std::false_type have constexpr conversion operators to bool.
It is a good habit to provides "helper helper templates" that let you directly access the value:
template <typename T>
constexpr bool is_pointer_v = is_pointer<T>::value;
Version ≥ C++17
In C++17 and above, most helper templates already provide a _v version, e.g.:
template< class T > constexpr bool is_pointer_v = is_pointer<T>::value;
template< class T > constexpr bool is_reference_v = is_reference<T>::value;
Section 16.8: Calculating power with C++11 (and higher)
With C++11 and higher calculations at compile time can be much easier. For example calculating the power of a
given number at compile time will be following:
template <typename T>
constexpr T calculatePower(T value, unsigned power) {
    return power == 0 ? 1 : value * calculatePower(value, power-1);
}
Keyword constexpr is responsible for calculating function in compilation time, then and only then, when all the
requirements for this will be met (see more at constexpr keyword reference) for example all the arguments must
be known at compile time.
Note: In C++11 constexpr function must compose only from one return statement.
Advantages: Comparing this to the standard way of compile time calculation, this method is also useful for runtime
calculations. It means, that if the arguments of the function are not known at the compilation time (e.g. value and
power are given as input via user), then function is run in a compilation time, so there's no need to duplicate a code
(as we would be forced in older standards of C++).
```

E.g.
```cpp
void useExample() {
    constexpr int compileTimeCalculated = calculatePower(3, 3); // computes at compile time,
                               // as both arguments are known at compilation time
                               // and used for a constant expression.
    int value;
    std::cin >> value;
    int runtimeCalculated = calculatePower(value, 3);  // runtime calculated,
                                    // because value is known only at runtime.
}
Version ≥ C++17
Another way to calculate power at compile time can make use of fold expression as follows:
#include <iostream>
#include <utility>
template <class T, T V, T N, class I = std::make_integer_sequence<T, N>>
struct power;
template <class T, T V, T N, T... Is>
struct power<T, V, N, std::integer_sequence<T, Is...>> {
   static constexpr T value = (static_cast<T>(1) * ... * (V * static_cast<bool>(Is + 1)));
};
int main() {
   std::cout << power<int, 4, 2>::value << std::endl;
}
Section 16.9: Generic Min/Max with variable argument count
Version > C++11
It's possible to write a generic function (for example min) which accepts various numerical types and arbitrary
argument count by template meta-programming. This function declares a min for two arguments and recursively
for more.
template <typename T1, typename T2>
auto min(const T1 &a, const T2 &b)
-> typename std::common_type<const T1&, const T2&>::type
{
    return a < b ? a : b;
}
template <typename T1, typename T2, typename ... Args>
auto min(const T1 &a, const T2 &b, const Args& ... args)
-> typename std::common_type<const T1&, const T2&, const Args& ...>::type
{
    return min(min(a, b), args...);
}
auto minimum = min(4, 5.8f, 3, 1.8, 3, 1.1, 9);


C++11 made Template Metaprogramming (TMP) usable by mere mortals.

***
```


### 1. VARIADIC TEMPLATES

Templates that accept an arbitrary number of arguments.

```cpp
template<typename T>
T sum(T t) { return t; }

template<typename T, typename... Args>
T sum(T t, Args... args) {
    return t + sum(args...);
}

// sum(1, 2, 3, 4) -> 10
```

***

### 2. TYPE TRAITS

The `<type_traits>` header allows compile-time inspection of types.

```cpp
#include <type_traits>

static_assert(std::is_integral<int>::value, "Int must be integral");
static_assert(std::is_pointer<int*>::value, "Must be pointer");
```

Used heavily with **SFINAE** (`std::enable_if`) to restrict templates.

***

### 3. CONSTEXPR (INTRODUCTION)

`constexpr` functions can be evaluated at compile-time.

```cpp
constexpr int square(int x) { return x * x; }

int array[square(5)]; // Valid! Size 25 at compile time.
```

In C++11, `constexpr` functions were very limited (single return statement). C++14 relaxed this.


### <a name="chapter-9-c14enhancements"></a>CHAPTER 9: C++14 ENHANCEMENTS

### C++14 Overview & Philosophy

C++14 (finalized in 2014) is a **refinement and maintenance release** of C++11.

#### Timeline & Context
- **2011**: C++11 released (revolutionary)
- **2014**: C++14 released (refinement + useful features)
- **2017**: C++17 released (significant improvements)
- **2020**: C++20 released (revolutionary again)

#### C++14 Philosophy
- **Smaller, focused** improvements rather than revolution
- **Fix** C++11 issues and limitations
- **Enhance** usability and convenience
- **Add** frequently-requested features
- **Simplify** compile-time computation

#### Key Features
1. Generic lambdas with `auto` parameters
2. Return type deduction for all functions
3. Binary literals and digit separators
4. std::make_unique
5. Relaxed constexpr rules
6. Variable templates
7. Library improvements

#### Why C++14 Matters
While smaller than C++11, C++14 makes C++11 more practical:
- ✅ Fixes usability issues
- ✅ Adds convenient features
- ✅ Improves compile-time computation
- ✅ Better template support
- ✅ More standard library features

***

## GENERIC LAMBDAS

### 1.1 Auto Parameters in Lambdas

C++14 allows `auto` as lambda parameters, creating **generic lambdas**.

#### Basic Generic Lambda

```cpp
#include <iostream>
#include <vector>
using namespace std;

// C++11: Type-specific lambda
auto add11 = [](int a, int b) { return a + b; };
cout << add11(5, 3) << "\n";           // 8
// cout << add11(2.5, 3.5) << "\n";   // ERROR - int only

// C++14: Generic lambda with auto
auto add14 = [](auto a, auto b) { return a + b; };
cout << add14(5, 3) << "\n";           // 8 (int)
cout << add14(2.5, 3.5) << "\n";       // 6 (double)
cout << add14(string("Hello"), string(" World")) << "\n";  // "Hello World"
```

#### Generic Lambda Deduction

```cpp
// Each auto parameter is independently deduced
auto process = [](auto x, auto y) {
    // x and y can be different types
    cout << x << ", " << y << "\n";
};

process(5, 3.14);              // int, double
process("hello", 42);          // const char*, int
process(3.14, "world");        // double, const char*
```

#### Generic Lambdas with std::vector

```cpp
vector<int> vi = {1, 2, 3};
vector<double> vd = {1.1, 2.2, 3.3};
vector<string> vs = {"a", "b", "c"};

// Single generic lambda works with all containers
auto print = [](auto val) {
    cout << val << " ";
};

for_each(vi.begin(), vi.end(), print);
cout << "\n";

for_each(vd.begin(), vd.end(), print);
cout << "\n";

for_each(vs.begin(), vs.end(), print);
cout << "\n";
```

#### Generic Lambdas with Algorithms

```cpp
// Works with any type supporting operator*
auto square = [](auto x) { return x * x; };

vector<int> vi = {1, 2, 3};
vector<double> vd = {1.5, 2.5, 3.5};

transform(vi.begin(), vi.end(), vi.begin(), square);
// vi: {1, 4, 9}

transform(vd.begin(), vd.end(), vd.begin(), square);
// vd: {2.25, 6.25, 12.25}
```

#### Generic Lambda Compile-Time Behavior

```cpp
// Type checking still happens at compile time
auto multiply = [](auto a, auto b) { return a * b; };

cout << multiply(5, 3) << "\n";        // 15 (int)
cout << multiply(2.5, 3.0) << "\n";    // 7.5 (double)

// This would compile-time error if * not defined:
// multiply("a", "b");                // ERROR - string doesn't support *
```

#### When to Use Generic Lambdas

```cpp
// Good use case: Works with any comparable type
auto find_min = [](auto a, auto b) { return a < b ? a : b; };

int min_int = find_min(5, 3);          // 3
double min_double = find_min(2.5, 1.5);  // 1.5
string min_str = find_min("cat", "apple");  // "apple"

// Bad use case: Type-specific logic
auto process = [](auto x) {
    if (is_integral_v<decltype(x)>) {
        cout << "Integer\n";
    } else if (is_floating_point_v<decltype(x)>) {
        cout << "Float\n";
    }
    // Too complex - use template or function overloads instead
};
```

***

## RETURN TYPE DEDUCTION FOR ALL FUNCTIONS

### 2.1 Return Type Deduction (C++14 Enhancement)

C++11 allowed return type deduction with `-> auto`, but C++14 simplifies it.

#### Basic Return Type Deduction

```cpp
// C++11: Must use trailing return type
auto add_11(int a, int b) -> int { return a + b; }
auto divide_11(double a, double b) -> double { return a / b; }

// C++14: Can deduce from return statement
auto add_14(int a, int b) { return a + b; }      // Returns int
auto divide_14(double a, double b) { return a / b; }  // Returns double

auto get_string() { return string("hello"); }    // Returns string
auto get_vector() { return vector<int>{1, 2, 3}; }  // Returns vector<int>
```

#### Multiple Return Statements

```cpp
// C++14: All returns must be consistent type
auto absolute(int x) {
    if (x >= 0) {
        return x;          // int
    } else {
        return -x;         // Must also be int
    }
}

// ERROR: Different return types
// auto mixed(int x) {
//     if (x > 0) {
//         return x;      // int
//     } else {
//         return 3.14;   // double - ERROR!
//     }
// }
```

#### Return Type Deduction with Complex Types

```cpp
#include <vector>
using namespace std;

// Deduce vector
auto get_data() {
    return vector<int>{1, 2, 3, 4, 5};
}

// Deduce map
auto get_map() {
    return map<string, int>{{"a", 1}, {"b", 2}};
}

// Deduce function
auto get_comparator() {
    return [](int a, int b) { return a > b; };
}

// Works seamlessly
vector<int> v = get_data();
map<string, int> m = get_map();
auto cmp = get_comparator();
```

#### Return Type Deduction in Templates

```cpp
template<typename T, typename U>
auto add(T a, U b) {
    return a + b;  // Type deduced from a + b
}

cout << add(5, 3) << "\n";              // int
cout << add(2.5, 3.0) << "\n";          // double
cout << add(5, 3.14) << "\n";           // double (int + double = double)

// Return type varies by input types
static_assert(is_same_v<decltype(add(5, 3)), int>);
static_assert(is_same_v<decltype(add(5.0, 3)), double>);
```

#### Recursion with Return Type Deduction

```cpp
// C++14: Recursive functions can use auto return type
// (But compiler may need hints for some cases)

auto factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

cout << factorial(5) << "\n";  // 120

// More complex recursion
auto fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

cout << fibonacci(10) << "\n";  // 55
```

#### Benefits of Return Type Deduction

```cpp
// Less redundant code
// Before: Must specify return type
int add_old(int a, int b) -> int { return a + b; }

// After: Auto-deduced
auto add_new(int a, int b) { return a + b; }

// Refactoring friendly - type changes automatically
auto get_value() { return 42; }        // int
// Later: auto get_value() { return 3.14; }  // Changes to double - easy!
```

***

## AUTO FOR VARIABLES IN LAMBDAS

### 3.1 Init Capture with Auto (C++14)

Lambda capture allows variables to be initialized inside the capture list.

#### Basic Init Capture

```cpp
#include <memory>
#include <iostream>
using namespace std;

// Create a unique_ptr (move-only type)
auto ptr = make_unique<int>(42);

// C++11: Can't capture unique_ptr (can't copy)
// auto lambda11 = [ptr]() { };  // ERROR - can't copy unique_ptr

// C++14: Init capture allows move
auto lambda = [ptr = move(ptr)]() {
    cout << *ptr << "\n";
};

lambda();  // 42
// ptr is now nullptr (moved into lambda)
```

#### Init Capture with Values

```cpp
int original = 10;

// Capture with transformation
auto lambda = [copy = original * 2]() {
    return copy;  // 20
};

cout << lambda() << "\n";  // 20
cout << original << "\n";  // Still 10 (original unchanged)

// Useful for expensive copies
vector<int> large_vector = {1, 2, 3, 4, 5};
auto process = [copy = vector<int>(large_vector)]() {
    // Use copy (independent of large_vector)
};
```

#### Init Capture with Move

```cpp
class Resource {
public:
    Resource() { cout << "Created\n"; }
    ~Resource() { cout << "Destroyed\n"; }
    void use() { cout << "Using\n"; }
};

auto res = make_unique<Resource>();

// Move resource into lambda
auto lambda = [res = move(res)]() {
    if (res) {
        res->use();
    }
};

lambda();  // Using, Destroyed
// res is now nullptr
```

#### Init Capture with Complex Types

```cpp
#include <map>

map<string, int> data{{"a", 1}, {"b", 2}};

// Copy map into lambda
auto process = [data_copy = data]() {
    for (const auto& [k, v] : data_copy) {
        cout << k << ": " << v << "\n";
    }
};

// Modify copy without affecting original
auto modify = [data_copy = move(data)]() {
    data_copy["c"] = 3;
    // data is moved
};

modify();
```

#### Init Capture Patterns

```cpp
// Pattern 1: Capture with computation
auto compute = [val = 2 + 3]() { return val; };  // val = 5

// Pattern 2: Capture with function call
auto get_timestamp = [time = chrono::high_resolution_clock::now()]() {
    return time;
};

// Pattern 3: Capture with complex initialization
auto setup = [config = []() {
    map<string, string> m;
    m["key"] = "value";
    return m;
}()]() {
    // config is initialized with map
};

// Pattern 4: Move-only types
auto factory = [ptr = make_unique<int>(42)]() {
    return *ptr;
};
```

***

## BINARY LITERALS & DIGIT SEPARATORS

### 4.1 Binary Literals

C++14 introduces `0b` prefix for binary literals.

#### Binary Literal Syntax

```cpp
#include <iostream>
using namespace std;

// Decimal (C++98)
int dec = 42;

// Hexadecimal (C++98)
int hex = 0x2A;

// Octal (C++98)
int oct = 052;

// Binary (C++14)
int bin = 0b101010;

cout << dec << "\n";  // 42
cout << hex << "\n";  // 42
cout << oct << "\n";  // 42
cout << bin << "\n";  // 42
```

#### Binary Literals Use Cases

```cpp
// Bitwise operations are clearer with binary
unsigned char flags = 0b11010110;
unsigned char mask = 0b00001111;

unsigned char result = flags & mask;  // Much clearer than 0xD6 & 0x0F

// Single bit operations
unsigned int option1 = 0b00000001;
unsigned int option2 = 0b00000010;
unsigned int option3 = 0b00000100;

unsigned int enabled = option1 | option3;  // 0b00000101

// Permission bits
unsigned char read = 0b100;    // 4
unsigned char write = 0b010;   // 2
unsigned char execute = 0b001; // 1

unsigned char permissions = read | write;
```

### 4.2 Digit Separators

C++14 allows single quotes `'` as digit separators for readability.

#### Digit Separator Examples

```cpp
// Large numbers are clearer with separators
long large = 1'000'000'000;      // One billion
double pi = 3.141'592'653;       // Pi

// Binary with separators (very clear)
unsigned char bits = 0b1111'0000;
unsigned short value = 0xDEAD'BEEF;

// All numeric literals support separators
int decimal = 123'456'789;
long long big = 9'223'372'036'854'775'807;  // Max int64

double d = 1'000.123'456;                   // Works with decimals
double e = 1.234'567e3;                     // Works with exponents
```

#### Readability Improvement

```cpp
// Before (hard to count zeros)
unsigned int ip = 192168001001;  // What is this?

// After (clear structure)
unsigned int ip = 192'168'001'001;  // IP address: 192.168.1.1

// Before (hard to verify)
long big = 9223372036854775807;

// After (easy to verify)
long big = 9'223'372'036'854'775'807;  // Max int64

// Before (unclear magnitude)
double money = 1000000000;

// After (clear)
double money = 1'000'000'000;  // One billion
```

#### Digit Separator Rules

```cpp
// Valid usage
int a = 1'000'000;
int b = 0xDEAD'BEEF;
int c = 0b1111'0000;

// NOT at start or end
// int bad1 = '123;          // ERROR
// int bad2 = 123';          // ERROR

// NOT adjacent to decimal point or exponent
// double bad3 = 1.'5;       // ERROR
// double bad4 = 1e'10;      // ERROR

// Multiple separators are OK
int d = 1'000'000'000;
int e = 0xFF'FF'FF'FF;
```

***

## STD::MAKE_UNIQUE

### 5.1 std::make_unique (C++14)

`std::make_unique` creates `unique_ptr` safely and efficiently.

#### Before C++14

```cpp
#include <memory>
using namespace std;

// C++11: Two-step process
unique_ptr<int> ptr1(new int(42));
unique_ptr<string> ptr2(new string("hello"));
unique_ptr<vector<int>> ptr3(new vector<int>{1, 2, 3});

// Problem: New and unique_ptr are separate
// If exception between new and unique_ptr, memory leaks
```

#### With std::make_unique

```cpp
#include <memory>
using namespace std;

// C++14: One-step, exception-safe
auto ptr1 = make_unique<int>(42);
auto ptr2 = make_unique<string>("hello");
auto ptr3 = make_unique<vector<int>>(initializer_list<int>{1, 2, 3});

// Automatically determines type
// Exception-safe: if constructor throws, no memory leak
```

#### make_unique with Classes

```cpp
class Person {
public:
    string name;
    int age;
    
    Person(string n, int a) : name(n), age(a) {
        cout << "Person created\n";
    }
    ~Person() {
        cout << "Person destroyed\n";
    }
};

// Create unique_ptr with constructor arguments
auto person = make_unique<Person>("Alice", 30);
cout << person->name << " is " << person->age << "\n";

// Automatic cleanup when going out of scope
```

#### make_unique with Arrays (C++20)

```cpp
// C++14: Dynamic sized arrays need manual approach
unique_ptr<int[]> arr1(new int[10]);

// C++20: make_unique supports arrays
// auto arr2 = make_unique<int[]>(10);  // C++20 only

// For C++14, use the manual approach:
auto arr3 = make_unique<int[]>();  // C++20
```

#### make_unique vs new

```cpp
// Old way (manual, error-prone)
function<unique_ptr<int>()> factory_old = []() {
    return unique_ptr<int>(new int(42));
};

// New way (cleaner, safer)
function<unique_ptr<int>()> factory_new = []() {
    return make_unique<int>(42);
};

// Exception safety benefit:
class Dangerous {
public:
    Dangerous(unique_ptr<Resource> r) : resource(move(r)) { }
private:
    unique_ptr<Resource> resource;
};

// Safe: If Dangerous constructor throws, r is still managed
auto danger = make_unique<Dangerous>(make_unique<Resource>());

// Unsafe: If Dangerous constructor throws, new Resource() leaks
// auto danger = unique_ptr<Dangerous>(
//     new Dangerous(unique_ptr<Resource>(new Resource())));
```

#### make_unique Best Practices

```cpp
// Prefer make_unique over new + unique_ptr
// Exception-safe
auto ptr1 = make_unique<MyClass>(arg1, arg2);

// More concise
auto ptr2 = make_unique<MyClass>();

// Automatic type deduction
auto ptr3 = make_unique<string>("hello");

// Use in containers
vector<unique_ptr<Resource>> resources;
resources.push_back(make_unique<Resource>());
resources.push_back(make_unique<Resource>());
// Automatic cleanup when vector destroyed
```

***

## RELAXED CONSTEXPR RESTRICTIONS

### 6.1 Enhanced constexpr Functions

C++14 relaxes constexpr restrictions, allowing more complex compile-time computation.

#### C++11 constexpr Limitations

```cpp
// C++11: constexpr function must have exactly one statement
constexpr int square_11(int x) {
    return x * x;  // Only return statement allowed
}

// C++11: Can't use local variables or loops
// constexpr int factorial_11(int n) {
//     int result = 1;           // ERROR - variable not allowed
//     for (int i = 2; i <= n; i++) {  // ERROR - loops not allowed
//         result *= i;
//     }
//     return result;
// }
```

#### C++14 constexpr Enhancements

```cpp
// C++14: Local variables allowed
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

cout << factorial(5) << "\n";  // Computed at compile-time if possible

// Compile-time constant
int arr[factorial(5)];  // Array of size 120

// C++14: More complex logic allowed
constexpr bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

static_assert(is_prime(7));    // Compile-time check
static_assert(!is_prime(4));   // Compile-time check
```

#### C++14 constexpr Features

```cpp
// Control flow statements
constexpr int abs_diff(int a, int b) {
    if (a > b) {
        return a - b;
    } else {
        return b - a;
    }
}

// Multiple return points
constexpr int sign(int x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

// Loops
constexpr int sum_range(int start, int end) {
    int total = 0;
    for (int i = start; i < end; i++) {
        total += i;
    }
    return total;
}

cout << sum_range(1, 10) << "\n";  // 45

// Fibonacci with better performance
constexpr int fib(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int next = a + b;
        a = b;
        b = next;
    }
    return b;
}

cout << fib(20) << "\n";  // 6765
```

#### constexpr Still Has Limitations

```cpp
// C++14: Still can't do
// - Dynamic memory allocation (new/delete)
// - Floating-point in some contexts (limited)
// - Most library functions

constexpr void* bad() {
    // return new int(42);  // ERROR
}

// But can call other constexpr functions
constexpr int helper() { return 42; }
constexpr int caller() {
    return helper() * 2;  // OK
}
```

#### Practical constexpr Uses

```cpp
// Compile-time lookup table
constexpr int digit_to_value(char d) {
    if (d >= '0' && d <= '9') return d - '0';
    if (d >= 'a' && d <= 'f') return d - 'a' + 10;
    if (d >= 'A' && d <= 'F') return d - 'A' + 10;
    return -1;
}

// Compile-time string parsing
constexpr int hex_to_int(const char* str) {
    int result = 0;
    for (int i = 0; str[i]; i++) {
        int digit = digit_to_value(str[i]);
        if (digit < 0) break;
        result = result * 16 + digit;
    }
    return result;
}

constexpr int hex_value = hex_to_int("FF");  // 255, computed at compile-time

// Compile-time array generation
constexpr int powers[10] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

// Compile-time validation
constexpr bool validate_date(int year, int month, int day) {
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
    return true;
}

static_assert(validate_date(2024, 12, 25));
```

***

## VARIABLE TEMPLATES

### 7.1 Template Variables (C++14)

Variables can be templates, not just functions and classes.

#### Basic Variable Template

```cpp
#include <iostream>
using namespace std;

// Template variable
template<typename T>
constexpr T pi = T(3.141592653589793);

// Use with different types
cout << pi<double> << "\n";        // 3.14159 (double)
cout << pi<float> << "\n";         // 3.14159 (float)
cout << pi<int> << "\n";           // 3 (int)

// Can use in computations
double circle_area = pi<double> * 5 * 5;  // Area of circle with radius 5
float sphere_volume = (4.0/3.0) * pi<float> * 3 * 3 * 3;
```

#### Variable Template with Type Traits

```cpp
#include <type_traits>

// Type trait as variable template
template<typename T>
constexpr bool is_integral_v = is_integral<T>::value;

template<typename T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;

// Usage (cleaner than ::value)
if (is_integral_v<int>) { }        // true
if (is_integral_v<double>) { }     // false
if (is_floating_point_v<double>) { }  // true
```

#### Useful Variable Templates

```cpp
// Concept-like variable template
template<typename T>
constexpr bool is_arithmetic_type = 
    is_integral_v<T> || is_floating_point_v<T>;

static_assert(is_arithmetic_type<int>);
static_assert(is_arithmetic_type<double>);
// static_assert(is_arithmetic_type<string>);  // false

// Size information
template<typename T>
constexpr size_t sizeof_v = sizeof(T);

cout << sizeof_v<int> << "\n";      // 4
cout << sizeof_v<double> << "\n";   // 8

// Min/max values
template<typename T>
constexpr T max_value = numeric_limits<T>::max();

template<typename T>
constexpr T min_value = numeric_limits<T>::min();

cout << max_value<int> << "\n";
cout << max_value<unsigned char> << "\n";
```

#### C++17 Standard Variable Templates

```cpp
// C++17 standard library additions
#include <type_traits>

// These are now variable templates in C++17
is_integral_v<int>;            // true
is_floating_point_v<double>;   // true
is_same_v<int, int>;           // true
remove_const_v<const int>;     // int
is_pointer_v<int*>;            // true

// Work like the old ::value but more concise
// is_integral<int>::value;      // Old way
is_integral_v<int>;            // New way (C++17)
```

***

## AGGREGATE MEMBER INITIALIZATION

### 8.1 Extended Aggregate Initialization

C++14 extends what can be aggregate-initialized.

#### Basic Aggregate Initialization (C++98)

```cpp
#include <iostream>
using namespace std;

struct Point {
    int x;
    int y;
};

// C++98: Brace initialization
Point p1 = {10, 20};
Point p2{30, 40};

cout << p1.x << ", " << p1.y << "\n";  // 10, 20
```

#### With Base Classes (C++14)

```cpp
struct Base {
    int b;
};

struct Derived : Base {
    int d;
};

// C++14: Can initialize base class members
Derived obj{1, 2};  // b=1, d=2
cout << obj.b << ", " << obj.d << "\n";  // 1, 2
```

#### Nested Aggregates

```cpp
struct Address {
    string street;
    string city;
};

struct Person {
    string name;
    int age;
    Address address;
};

// Nested initialization
Person p{"Alice", 30, {"123 Main St", "NYC"}};

cout << p.name << " lives at " << p.address.street << "\n";
```

#### C++14 vs C++11 Differences

```cpp
struct Point {
    int x = 0;  // Default member initializer (C++11)
    int y = 0;
};

// C++11: Default initializer sets x=0, y=0
Point p1;           // x=0, y=0
Point p2{};         // x=0, y=0

// Explicit initialization
Point p3{10, 20};   // x=10, y=20

// Partial initialization with defaults
// Behavior similar between C++11 and C++14
```

***

## MEMBER FUNCTION REF/CONST-REF QUALIFIERS

### 9.1 Lvalue vs Rvalue Member Functions

C++11 introduced, C++14 standardized: member functions can be qualified as `&` or `&&`.

#### Member Function Overloading

```cpp
#include <iostream>
#include <string>
using namespace std;

class Text {
private:
    string data;

public:
    Text(string s) : data(s) {}

    // For lvalue (normal objects)
    string& get_data() & {
        cout << "Lvalue version\n";
        return data;
    }

    // For rvalue (temporaries)
    string get_data() && {
        cout << "Rvalue version\n";
        return move(data);
    }

    // Const lvalue
    const string& get_data() const& {
        cout << "Const lvalue version\n";
        return data;
    }
};

int main() {
    Text t("hello");
    
    // Calls lvalue version
    auto& result1 = t.get_data();      // "Lvalue version"
    
    // Calls const lvalue version
    const auto& result2 = t.get_data();  // "Const lvalue version"
    
    // Calls rvalue version
    auto result3 = Text("world").get_data();  // "Rvalue version"
    
    return 0;
}
```

#### Practical Use Case

```cpp
class Vector {
private:
    int* data;
    int size;

public:
    Vector() : data(nullptr), size(0) {}
    Vector(int n) : data(new int[n]), size(n) {}

    // Cheap copy for lvalue - return reference
    int* get_data() & {
        return data;
    }

    // Cheap move for rvalue - return by value
    int* get_data() && {
        int* tmp = data;
        data = nullptr;
        return tmp;
    }

    ~Vector() { delete[] data; }
};

Vector createVector(int n) {
    return Vector(n);
}

int main() {
    Vector v(100);
    
    // Lvalue: efficient reference
    int* ptr1 = v.get_data();
    
    // Rvalue: efficient move
    int* ptr2 = createVector(100).get_data();
    
    return 0;
}
```

#### Const/Volatile Combinations

```cpp
class Object {
public:
    // All combinations possible:
    void method() & { }           // Lvalue
    void method() const& { }      // Const lvalue
    void method() && { }          // Rvalue
    void method() const&& { }     // Const rvalue
    void method() volatile& { }   // Volatile lvalue
    // ... more combinations
};
```

***

## STD::INTEGER_SEQUENCE

### 10.1 Compile-Time Integer Sequences

`std::integer_sequence` provides compile-time sequences of integers.

#### Basic Usage

```cpp
#include <utility>
#include <iostream>
using namespace std;

// Create a sequence 0, 1, 2, 3, 4
using seq = integer_sequence<int, 0, 1, 2, 3, 4>;

// More practical: Generate sequence
using seq5 = make_integer_sequence<int, 5>;  // 0, 1, 2, 3, 4

// Use with function
template<typename T, T... Is>
void print_sequence(integer_sequence<T, Is...>) {
    ((cout << Is << " "), ...);  // C++17 fold expression
    cout << "\n";
}

print_sequence(make_integer_sequence<int, 10>());  // 0 1 2 3 4 5 6 7 8 9
```

#### Unpacking Tuple

```cpp
#include <tuple>

// Convert tuple to function arguments
template<typename F, typename Tuple, size_t... Is>
auto apply_impl(F&& f, Tuple&& t, index_sequence<Is...>) {
    return forward<F>(f)(get<Is>(forward<Tuple>(t))...);
}

template<typename F, typename Tuple>
auto apply(F&& f, Tuple&& t) {
    return apply_impl(
        forward<F>(f),
        forward<Tuple>(t),
        make_index_sequence<tuple_size_v<decay_t<Tuple>>>()
    );
}

// Usage
auto add = [](int a, int b, int c) { return a + b + c; };
auto result = apply(add, make_tuple(1, 2, 3));  // 6
```

#### Array Initialization

```cpp
template<typename T, size_t N, size_t... Is>
void fill_array_impl(array<T, N>& arr, index_sequence<Is...>) {
    (..., (arr[Is] = Is * Is));  // Fill with squares
}

template<typename T, size_t N>
void fill_array(array<T, N>& arr) {
    fill_array_impl(arr, make_index_sequence<N>());
}

array<int, 5> arr;
fill_array(arr);
// arr: {0, 1, 4, 9, 16}
```

***

## LIBRARY IMPROVEMENTS

### 11.1 STL Enhancements in C++14

#### std::quoted for String I/O

```cpp
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

string text = "Hello \"World\"";

// Without quoted
cout << text << "\n";
// Output: Hello "World"

// With quoted (C++14)
cout << quoted(text) << "\n";
// Output: "Hello \"World\""

// Useful for CSV and JSON
cout << quoted("value with spaces") << "\n";
```

#### std::less and Comparators

```cpp
// Transparent comparators (C++14)
set<int, less<>> s;  // Uses operator< for any comparable types

s.insert(5);
cout << s.count(5) << "\n";  // 1

// Can search with different type
cout << s.count(5.0) << "\n";  // Works with double too
```

#### Algorithms Returning Pair

```cpp
#include <algorithm>

vector<int> v = {1, 2, 3, 4, 5};

// Functions returning pairs of iterators
auto [first, last] = equal_range(v.begin(), v.end(), 3);
// C++17: structured binding to unpack pair

// Alternative (C++14)
auto range = equal_range(v.begin(), v.end(), 3);
auto first_elem = range.first;
auto last_elem = range.second;
```

#### std::exchange

```cpp
#include <utility>

int x = 5;
int old_value = exchange(x, 10);

cout << x << "\n";          // 10
cout << old_value << "\n";  // 5

// Useful for swapping
struct Object {
    Data data;
    Object& operator=(Object&& other) noexcept {
        data = exchange(other.data, Data());
        return *this;
    }
};
```

#### std::get with Type

```cpp
#include <tuple>

tuple<int, double, string> t{42, 3.14, "hello"};

// Get by index (C++11)
auto a = get<0>(t);  // 42

// Get by type (C++14) - must be unique type
auto b = get<double>(t);  // 3.14
auto c = get<string>(t);  // "hello"

// ERROR if type appears twice
// tuple<int, int, string> t2;
// get<int>(t2);  // Ambiguous!
```

***

## DEPRECATED FEATURES & REMOVALS

### 12.1 Features Deprecated in C++14

```cpp
// 1. std::auto_ptr (deprecated)
// Use unique_ptr instead
// auto_ptr<int> old_ptr(new int(42));  // Deprecated
auto new_ptr = make_unique<int>(42);     // Modern

// 2. std::binary_function, unary_function
// No longer needed with lambdas
// struct Plus : binary_function<int, int, int> {
//     int operator()(int a, int b) const { return a + b; }
// };

auto plus = [](int a, int b) { return a + b; };  // Modern

// 3. std::bind1st, bind2nd
// Use std::bind or lambdas
// auto partial = bind1st(plus(), 5);  // Deprecated

auto partial = [](int x) { return 5 + x; };  // Modern
```

#### 12.5 Shared Locks (Reader-Writer Mutex)

C++14 introduces `shared_timed_mutex` allowing multiple readers but exclusive writers.

```cpp
#include <shared_mutex>
#include <mutex>
#include <map>

class ThreadSafeCache {
    std::map<int, int> data;
    mutable std::shared_timed_mutex mtx; // C++14 (use shared_mutex in C++17)

public:
    // Reader: Multiple threads can hold shared_lock
    int get(int key) const {
        std::shared_lock<std::shared_timed_mutex> lock(mtx);
        if (data.find(key) != data.end()) {
            return data.at(key);
        }
        return -1;
    }

    // Writer: Only one thread can hold unique_lock
    void put(int key, int value) {
        std::unique_lock<std::shared_timed_mutex> lock(mtx);
        data[key] = value;
    }
};
```

***

## C++14 BEST PRACTICES

### What's Better with C++14

```cpp
// 1. Use generic lambdas for flexibility
auto process = [](auto x) { cout << x << "\n"; };
process(42);
process("hello");
process(3.14);

// 2. Use auto return types to avoid redundancy
auto add(int a, int b) { return a + b; }

// 3. Use make_unique for safety
auto ptr = make_unique<MyClass>(arg1, arg2);

// 4. Use binary literals for clarity
unsigned char mask = 0b11110000;

// 5. Use digit separators for readability
long big = 1'000'000'000'000;

// 6. Use init capture for move-only types
auto lambda = [ptr = move(ptr)]() { };

// 7. Use relaxed constexpr for compile-time computation
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) result *= i;
    return result;
}

// 8. Use variable templates for type information
template<typename T>
constexpr bool is_integral_v = is_integral<T>::value;
```

***

### <a name="chapter-10-c17modernfeatures"></a>CHAPTER 10: C++17 MODERN FEATURES


***
#### Professional Insights: Modern Types (Optional/Variant)

##### std::optional

Section 51.1: Using optionals to represent the absence of a
value
Before C++17, having pointers with a value of nullptr commonly represented the absence of a value. This is a good
solution for large objects that have been dynamically allocated and are already managed by pointers. However, this
solution does not work well for small or primitive types such as int, which are rarely ever dynamically allocated or
managed by pointers. std::optional provides a viable solution to this common problem.
In this example, struct Person is deﬁned. It is possible for a person to have a pet, but not necessary. Therefore,
the pet member of Person is declared with an std::optional wrapper.
```cpp
#include <iostream>
#include <optional>
#include <string>
struct Animal {
    std::string name;
};
struct Person {
    std::string name;
    std::optional<Animal> pet;
};
int main() {
    Person person;
    person.name = "John";
    if (person.pet) {
        std::cout << person.name << "'s pet's name is " <<
            person.pet->name << std::endl;
    }
    else {
        std::cout << person.name << " is alone." << std::endl;
    }
}
Section 51.2: optional as return value
std::optional<float> divide(float a, float b) {
  if (b!=0.f) return a/b;
  return {};
}
Here we return either the fraction a/b, but if it is not deﬁned (would be inﬁnity) we instead return the empty
optional.
A more complex case:
template<class Range, class Pred>
auto find_if( Range&& r, Pred&& p ) {
  using std::begin; using std::end;
  auto b = begin(r), e = end(r);
  auto r = std::find_if(b, e , p );
  using iterator = decltype(r);
  if (r==e)
    return std::optional<iterator>();
  return std::optional<iterator>(r);
}
template<class Range, class T>
auto find( Range&& r, T const& t ) {
  return find_if( std::forward<Range>(r), [&t](auto&& x){return x==t;} );
}
find( some_range, 7 ) searches the container or range some_range for something equal to the number 7.
find_if does it with a predicate.
It returns either an empty optional if it was not found, or an optional containing an iterator tothe element if it was.
This allows you to do:
if (find( vec, 7 )) {
  // code
}
or even
if (auto oit = find( vec, 7 )) {
  vec.erase(*oit);
}
without having to mess around with begin/end iterators and tests.
Section 51.3: value_or
void print_name( std::ostream& os, std::optional<std::string> const& name ) {
  std::cout "Name is: " << name.value_or("<name missing>") << '\n';
}
value_or either returns the value stored in the optional, or the argument if there is nothing store there.
This lets you take the maybe-null optional and give a default behavior when you actually need a value. By doing it
this way, the "default behavior" decision can be pushed back to the point where it is best made and immediately
needed, instead of generating some default value deep in the guts of some engine.
Section 51.4: Introduction
```

Optionals (also known as Maybe types) are used to represent a type whose contents may or may not be present.
They are implemented in C++17 as the std::optional class. For example, an object of type std::optional<int>
may contain some value of type int, or it may contain no value.
Optionals are commonly used either to represent a value that may not exist or as a return type from a function that
can fail to return a meaningful result.
Other approaches to optional
There are many other approach to solving the problem that std::optional solves, but none of them are quite
complete: using a pointer, using a sentinel, or using a pair<bool, T>.
Optional vs Pointer
In some cases, we can provide a pointer to an existing object or nullptr to indicate failure. But this is limited to
those cases where objects already exist - optional, as a value type, can also be used to return new objects without
resorting to memory allocation.
Optional vs Sentinel
A common idiom is to use a special value to indicate that the value is meaningless. This may be 0 or -1 for integral
types, or nullptr for pointers. However, this reduces the space of valid values (you cannot diﬀerentiate between a
valid 0 and a meaningless 0) and many types do not have a natural choice for the sentinel value.
Optional vs std::pair<bool, T>
Another common idiom is to provide a pair, where one of the elements is a bool indicating whether or not the
value is meaningful.
This relies upon the value type being default-constructible in the case of error, which is not possible for some types
and possible but undesirable for others. An optional<T>, in the case of error, does not need to construct anything.
Section 51.5: Using optionals to represent the failure of a
function
Before C++17, a function typically represented failure in one of several ways:
A null pointer was returned.
e.g. Calling a function Delegate *App::get_delegate() on an App instance that did not have a
delegate would return nullptr.
This is a good solution for objects that have been dynamically allocated or are large and managed by
pointers, but isn't a good solution for small objects that are typically stack-allocated and passed by
copying.
A speciﬁc value of the return type was reserved to indicate failure.
e.g. Calling a function unsigned shortest_path_distance(Vertex a, Vertex b) on two vertices that
are not connected may return zero to indicate this fact.
The value was paired together with a bool to indicate is the returned value was meaningful.
e.g. Calling a function std::pair<int, bool> parse(const std::string &str) with a string
argument that is not an integer would return a pair with an undeﬁned int and a bool set to false.
In this example, John is given two pets, Fluﬀy and Furball. The function Person::pet_with_name() is then called to
retrieve John's pet Whiskers. Since John does not have a pet named Whiskers, the function fails and std::nullopt is
returned instead.
```cpp
#include <iostream>
#include <optional>
#include <string>
#include <vector>
struct Animal {
    std::string name;
};
struct Person {
    std::string name;
    std::vector<Animal> pets;
    std::optional<Animal> pet_with_name(const std::string &name) {
        for (const Animal &pet : pets) {
            if (pet.name == name) {
                return pet;
            }
        }
        return std::nullopt;
    }
};
int main() {
    Person john;
    john.name = "John";
    Animal fluffy;
    fluffy.name = "Fluffy";
    john.pets.push_back(fluffy);
    Animal furball;
    furball.name = "Furball";
    john.pets.push_back(furball);
    std::optional<Animal> whiskers = john.pet_with_name("Whiskers");
    if (whiskers) {
        std::cout << "John has a pet named Whiskers." << std::endl;
    }
    else {
        std::cout << "Whiskers must not belong to John." << std::endl;
    }
}
```


##### std::variant

Section 56.1: Create pseudo-method pointers
This is an advanced example.
You can use variant for light weight type erasure.
```cpp
template<class F>
struct pseudo_method {
  F f;
  // enable C++17 class type deduction:
  pseudo_method( F&& fin ):f(std::move(fin)) {}
  // Koenig lookup operator->*, as this is a pseudo-method it is appropriate:
  template<class Variant> // maybe add SFINAE test that LHS is actually a variant.
  friend decltype(auto) operator->*( Variant&& var, pseudo_method const& method ) {
    // var->*method returns a lambda that perfect forwards a function call,
    // behaving like a method pointer basically:
    return [&](auto&&...args)->decltype(auto) {
      // use visit to get the type of the variant:
      return std::visit(
        [&](auto&& self)->decltype(auto) {
          // decltype(x)(x) is perfect forwarding in a lambda:
          return method.f( decltype(self)(self), decltype(args)(args)... );
        },
        std::forward<Var>(var)
      );
    };
  }
};
this creates a type that overloads operator->* with a Variant on the left hand side.
// C++17 class type deduction to find template argument of `print` here.
// a pseudo-method lambda should take `self` as its first argument, then
// the rest of the arguments afterwards, and invoke the action:
pseudo_method print = [](auto&& self, auto&&...args)->decltype(auto) {
  return decltype(self)(self).print( decltype(args)(args)... );
};
Now if we have 2 types each with a print method:
struct A {
  void print( std::ostream& os ) const {
    os << "A";
  }
};
struct B {
  void print( std::ostream& os ) const {
    os << "B";
  }
};
note that they are unrelated types. We can:
std::variant<A,B> var = A{};
(var->*print)(std::cout);
and it will dispatch the call directly to A::print(std::cout) for us. If we instead initialized the var with B{}, it would
dispatch to B::print(std::cout).
If we created a new type C:
struct C {};
then:
std::variant<A,B,C> var = A{};
(var->*print)(std::cout);
will fail to compile, because there is no C.print(std::cout) method.
Extending the above would permit free function prints to be detected and used, possibly with use of if constexpr
within the print pseudo-method.
live example currently using boost::variant in place of std::variant.
Section 56.2: Basic std::variant use
```

This creates a variant (a tagged union) that can store either an int or a string.
std::variant< int, std::string > var;
We can store one of either type in it:
var = "hello"s;
And we can access the contents via std::visit:
// Prints "hello\n":
visit( [](auto&& e) {
```cpp
  std::cout << e << '\n';
}, var );
by passing in a polymorphic lambda or similar function object.
If we are certain we know what type it is, we can get it:
auto str = std::get<std::string>(var);
but this will throw if we get it wrong. get_if:
auto* str  = std::get_if<std::string>(&var);
returns nullptr if you guess wrong.
Variants guarantee no dynamic memory allocation (other than which is allocated by their contained types). Only
one of the types in a variant is stored there, and in rare cases (involving exceptions while assigning and no safe way
to back out) the variant can become empty.
Variants let you store multiple value types in one variable safely and eﬃciently. They are basically smart, type-safe
unions.
Section 56.3: Constructing a `std::variant`
```

This does not cover allocators.
struct A {};
struct B { B()=default; B(B const&)=default; B(int){}; };
struct C { C()=delete; C(int) {}; C(C const&)=default; };
struct D { D( std::initializer_list<int> ) {}; D(D const&)=default; D()=default; };
std::variant<A,B> var_ab0; // contains a A()
std::variant<A,B> var_ab1 = 7; // contains a B(7)
std::variant<A,B> var_ab2 = var_ab1; // contains a B(7)
std::variant<A,B,C> var_abc0{ std::in_place_type<C>, 7 }; // contains a C(7)
std::variant<C> var_c0; // illegal, no default ctor for C
std::variant<A,D> var_ad0( std::in_place_type<D>, {1,3,3,4} ); // contains D{1,3,3,4}
std::variant<A,D> var_ad1( std::in_place_index<0> ); // contains A{}
std::variant<A,D> var_ad2( std::in_place_index<1>, {1,3,3,4} ); // contains D{1,3,3,4}


### C++17 Overview & Significance

C++17 (finalized in 2017) is a **major language update** rivaling C++11 in scope.

#### Timeline & Context
- **2011**: C++11 released (revolutionary)
- **2014**: C++14 released (refinement)
- **2017**: C++17 released (major overhaul)
- **2020**: C++20 released (revolutionary again)

#### C++17 Philosophy
- **Fix** fundamental issues with C++11/14
- **Add** major features requested by industry
- **Improve** performance and expressivity
- **Simplify** complex code patterns
- **Standardize** common idioms

#### Key Themes
1. **Pattern Matching** - Structured bindings
2. **Null Safety** - optional, variant
3. **Performance** - string_view, if constexpr
4. **Expressivity** - Fold expressions
5. **Safety** - Filesystem library
6. **Parallelism** - Parallel algorithms
7. **Type Deduction** - CTAD
8. **Flexibility** - std::any

#### Why C++17 Matters
C++17 addresses real pain points:
- ✅ Safer null handling (optional)
- ✅ Type-safe unions (variant)
- ✅ Zero-copy string operations (string_view)
- ✅ Compile-time branching (if constexpr)
- ✅ Pattern matching (structured bindings)
- ✅ Safe filesystem access
- ✅ Automatic template deduction
- ✅ Flexible type storage (any)

***

## STRUCTURED BINDINGS

### 1.1 Introduction to Structured Bindings

Structured bindings allow unpacking objects into individual variables.

#### Basic Structured Bindings

```cpp
#include <tuple>
#include <iostream>
using namespace std;

// Before C++17: Verbose
tuple<int, double, string> t = {42, 3.14, "hello"};
int a = get<0>(t);
double b = get<1>(t);
string c = get<2>(t);

// C++17: Clean and simple
auto [x, y, z] = t;
cout << x << ", " << y << ", " << z << "\n";  // 42, 3.14, hello
```

#### Structured Bindings with Pairs

```cpp
#include <map>

map<string, int> ages{{"Alice", 30}, {"Bob", 25}};

// Before C++17: Awkward
for (const auto& pair : ages) {
    const string& name = pair.first;
    int age = pair.second;
    cout << name << " is " << age << "\n";
}

// C++17: Natural
for (const auto& [name, age] : ages) {
    cout << name << " is " << age << "\n";
}
```

#### Structured Bindings with Arrays

```cpp
// Arrays
int arr[3] = {1, 2, 3};
auto [a, b, c] = arr;
cout << a << ", " << b << ", " << c << "\n";  // 1, 2, 3

// Fixed-size arrays
array<double, 4> coords = {1.0, 2.0, 3.0, 4.0};
auto [x, y, z, w] = coords;
```

#### Structured Bindings with Structs

```cpp
struct Person {
    string name;
    int age;
    string city;
};

Person p{"Alice", 30, "NYC"};

// Before C++17
string name = p.name;
int age = p.age;
string city = p.city;

// C++17
auto [name, age, city] = p;
cout << name << " is " << age << " from " << city << "\n";
```

#### Structured Bindings with Return Values

```cpp
pair<bool, int> divide(int a, int b) {
    if (b == 0) return {false, 0};
    return {true, a / b};
}

// Before C++17: Awkward
auto result = divide(10, 2);
if (result.first) {
    cout << "Result: " << result.second << "\n";
}

// C++17: Clear
auto [success, value] = divide(10, 2);
if (success) {
    cout << "Result: " << value << "\n";
}
```

#### Structured Bindings with References

```cpp
int x = 5, y = 10;

// Modify through references
auto& [rx, ry] = tuple<int&, int&>(x, y);
rx = 20;
cout << x << "\n";  // 20 (modified)

// Const references
const auto& [cx, cy] = tuple<int, int>(5, 10);
// cx = 100;  // ERROR - const
```

#### Practical Use Cases

```cpp
// Function returning multiple values
tuple<bool, string, int> parse_config(const string& path) {
    // Parse and return success, name, port
    return {true, "server", 8080};
}

auto [ok, name, port] = parse_config("/etc/config");
if (ok) {
    cout << "Server " << name << " on port " << port << "\n";
}

// Iterating over map with unpacking
map<string, vector<int>> data{
    {"a", {1, 2, 3}},
    {"b", {4, 5, 6}}
};

for (auto [key, values] : data) {
    cout << key << ": ";
    for (int v : values) cout << v << " ";
    cout << "\n";
}

// Database-like access
vector<tuple<int, string, double>> records{
    {1, "Alice", 95.5},
    {2, "Bob", 87.0},
    {3, "Carol", 92.5}
};

for (auto [id, name, score] : records) {
    cout << id << ": " << name << " scored " << score << "\n";
}
```

#### Structured Bindings Rules

```cpp
// Auto deduction (copies)
tuple<int, int> t{1, 2};
auto [a, b] = t;           // a, b are copies

// References
auto& [x, y] = t;          // x, y reference t's elements
const auto& [cx, cy] = t;  // const references

// Move
auto&& [mx, my] = move(t); // rvalue references

// Partial binding (with operator[])
struct Container {
    int& operator[](size_t i);  // Must return reference
};

Container c;
auto [elem] = c;           // Gets copy via operator[]

// Multiple items
auto [a, b, c, d] = tuple{1, 2, 3, 4};
auto [x, y, z] = array{10, 20, 30};
```

### 1.2 Structured Bindings Under the Hood

The compiler generates a hidden variable.

**Code:**
```cpp
auto [x, y] = my_pair;
```

**Compiler Logic:**
```cpp
auto __hidden = my_pair;
auto& x = __hidden.first;  // Aliases
auto& y = __hidden.second;
```

**Implication**:
*   `x` and `y` are NOT variables; they are names referring to subobjects of the hidden variable.
*   If you use `auto& [x, y]`, the hidden variable is a reference.

***

## OPTIONAL & VARIANT

### 2.1 std::optional - Safe Nullable Values

`std::optional` represents a value that may or may not be present.

#### Basic optional Usage

```cpp
#include <optional>
#include <iostream>
using namespace std;

// Function that might not return a value
optional<int> parse_int(const string& s) {
    try {
        return stoi(s);
    } catch (...) {
        return nullopt;  // No value
    }
}

// Usage
auto result1 = parse_int("42");
if (result1.has_value()) {
    cout << "Value: " << result1.value() << "\n";
}

// Or using operator*
if (result1) {
    cout << "Value: " << *result1 << "\n";
}

// Default value
auto value = result1.value_or(0);  // 0 if no value
```

#### optional with Complex Types

```cpp
struct User {
    int id;
    string name;
};

optional<User> find_user(int id) {
    if (id < 0) return nullopt;
    return User{id, "User" + to_string(id)};
}

// Usage
if (auto user = find_user(42)) {
    cout << user->name << "\n";
} else {
    cout << "User not found\n";
}
```

#### optional Operations

```cpp
optional<int> opt(42);

// Check
if (opt) cout << "Has value\n";          // true
if (opt.has_value()) cout << "Has\n";    // true

// Access
cout << opt.value() << "\n";              // 42
cout << *opt << "\n";                     // 42
cout << opt.value_or(0) << "\n";          // 42

// Modify
opt.value() = 100;
cout << *opt << "\n";                     // 100

// Reset
opt = nullopt;
if (opt) cout << "Has value\n";          // false

// Or assign
opt = 99;
cout << *opt << "\n";                     // 99
```

#### optional with Chaining

```cpp
optional<int> process(optional<int> input) {
    if (!input) return nullopt;
    return input.value() * 2;
}

auto result = process(optional<int>(5));
if (result) {
    cout << result.value() << "\n";  // 10
}

// Chain operations
auto chain = parse_int("42")
    .and_then([](int x) -> optional<int> { return x * 2; })
    .or_else([]() { return optional<int>(0); });
```

***

### 2.2 std::variant - Type-Safe Union

`std::variant` is a type-safe union that holds one of several types.

#### Basic variant Usage

```cpp
#include <variant>
#include <iostream>
using namespace std;

// Can hold int, double, or string
variant<int, double, string> value;

// Store int
value = 42;
cout << get<int>(value) << "\n";  // 42

// Store double
value = 3.14;
cout << get<double>(value) << "\n";  // 3.14

// Store string
value = string("hello");
cout << get<string>(value) << "\n";  // hello

// Check type
cout << value.index() << "\n";  // 2 (string is third)
```

#### variant with Type Checking

```cpp
void process(variant<int, double, string> value) {
    if (holds_alternative<int>(value)) {
        cout << "Integer: " << get<int>(value) << "\n";
    } else if (holds_alternative<double>(value)) {
        cout << "Double: " << get<double>(value) << "\n";
    } else if (holds_alternative<string>(value)) {
        cout << "String: " << get<string>(value) << "\n";
    }
}

process(42);              // "Integer: 42"
process(3.14);            // "Double: 3.14"
process("hello");         // "String: hello"
```

#### variant with Visitor Pattern

```cpp
struct Visitor {
    void operator()(int i) const {
        cout << "Integer: " << i << "\n";
    }
    
    void operator()(double d) const {
        cout << "Double: " << d << "\n";
    }
    
    void operator()(const string& s) const {
        cout << "String: " << s << "\n";
    }
};

variant<int, double, string> v = 42;
visit(Visitor(), v);  // "Integer: 42"

v = 3.14;
visit(Visitor(), v);  // "Double: 3.14"

v = string("hello");
visit(Visitor(), v);  // "String: hello"
```

#### variant with Lambdas (C++20 or using overload trick)

```cpp
// C++17 overload trick
template<typename... Ts>
struct overload : Ts... { using Ts::operator()...; };
template<typename... Ts>
overload(Ts...) -> overload<Ts...>;

variant<int, double, string> v = 42;

// Visit with lambdas
visit(overload{
    [](int i) { cout << "Integer: " << i << "\n"; },
    [](double d) { cout << "Double: " << d << "\n"; },
    [](const string& s) { cout << "String: " << s << "\n"; }
}, v);  // "Integer: 42"
```

#### Practical variant Example

```cpp
variant<int, string> parse_value(const string& input) {
    try {
        return stoi(input);  // Try as int
    } catch (...) {
        return input;        // Return as string
    }
}

auto result = parse_value("42");
if (holds_alternative<int>(result)) {
    cout << "Parsed as int: " << get<int>(result) << "\n";
} else {
    cout << "Parsed as string: " << get<string>(result) << "\n";
}
```

***

## STD::ANY

### 3.1 Type-Erased Storage with std::any

`std::any` can hold any copyable type.

#### Basic any Usage

```cpp
#include <any>
#include <iostream>
using namespace std;

any value;

// Store different types
value = 42;
cout << any_cast<int>(value) << "\n";  // 42

value = 3.14;
cout << any_cast<double>(value) << "\n";  // 3.14

value = string("hello");
cout << any_cast<string>(value) << "\n";  // hello

// Check type
if (value.type() == typeid(string)) {
    cout << "It's a string\n";
}
```

#### any with Type Checking

```cpp
any value = 42;

if (value.type() == typeid(int)) {
    cout << "Value: " << any_cast<int>(value) << "\n";
}

// Safe cast (throws if wrong type)
try {
    double d = any_cast<double>(value);  // Wrong type
} catch (const bad_any_cast& e) {
    cout << "Type mismatch: " << e.what() << "\n";
}

// Check before casting
if (value.type() == typeid(int)) {
    int i = any_cast<int>(value);
}
```

#### any in Collections

```cpp
vector<any> data;
data.push_back(42);
data.push_back(3.14);
data.push_back(string("hello"));
data.push_back(vector<int>{1, 2, 3});

// Process
for (auto& item : data) {
    if (item.type() == typeid(int)) {
        cout << "Int: " << any_cast<int>(item) << "\n";
    } else if (item.type() == typeid(double)) {
        cout << "Double: " << any_cast<double>(item) << "\n";
    } else if (item.type() == typeid(string)) {
        cout << "String: " << any_cast<string>(item) << "\n";
    }
}
```

#### any vs variant

```cpp
// variant<int, double, string>: Fixed types, type-safe
variant<int, double, string> v = 42;
cout << get<int>(v) << "\n";  // Type-safe, no runtime check needed

// any: Any type, runtime type checking
any a = 42;
cout << any_cast<int>(a) << "\n";  // Runtime check, potential exception

// Use variant when types are known
// Use any when types are truly dynamic
```

***

## STD::STRING_VIEW

### 4.1 Non-Owning String References

`std::string_view` provides efficient, zero-copy string operations.

#### Basic string_view

```cpp
#include <string_view>
#include <iostream>
using namespace std;

string s = "Hello, World!";
string_view sv = s;

cout << sv << "\n";           // "Hello, World!"
cout << sv.length() << "\n";  // 13
cout << sv[0] << "\n";        // 'H'
cout << sv.data() << "\n";    // "Hello, World!"
```

#### string_view from Different Sources

```cpp
// From std::string
string s = "hello";
string_view sv1 = s;

// From C-string
const char* cstr = "world";
string_view sv2 = cstr;

// From string literal
string_view sv3 = "test";

// Substring
string_view sv4 = sv1.substr(1, 3);  // "ell"
```

#### string_view Operations

```cpp
string_view sv = "Hello, World!";

// Searching
cout << sv.find("World") << "\n";      // 7
cout << sv.find(',') << "\n";          // 5

// Comparing
cout << sv.compare("Hello, World!") << "\n";  // 0 (equal)

// Prefix/suffix
cout << sv.starts_with("Hello") << "\n";  // true
cout << sv.ends_with("!") << "\n";        // true

// Substrings
auto hello = sv.substr(0, 5);           // "Hello"
auto world = sv.substr(7);              // "World!"

// Remove prefix/suffix (C++20)
// sv.remove_prefix(7);
// sv.remove_suffix(1);
```

#### Performance Benefits of string_view

```cpp
// OLD: Copy string
void process_old(const string& s) {
    // s might be copied
}

// NEW: No copy
void process_new(string_view sv) {
    // sv references the string, no copy
}

// Usage
string data = "important";
process_old(data);  // Might copy
process_new(data);  // No copy!

// Works with literals too
process_new("temporary");  // No copy, no allocation
```

#### string_view Limitations

```cpp
string_view sv = "hello";

// What you CAN'T do:
// sv[0] = 'H';                    // ERROR - can't modify
// sv.resize(3);                   // ERROR - can't resize
// string s(sv);                   // OK - explicit conversion needed

// Works only while source exists
string_view sv2;
{
    string temp = "danger";
    sv2 = temp;
    // temp destroyed here
}
// sv2 now points to destroyed string - DANGER!

// Safe way: Make a copy if needed
string copy(sv2);
```

#### string_view Use Cases

```cpp
// Parse tokens without copying
string_view parse_token(string_view& input) {
    size_t pos = input.find(' ');
    if (pos == string_view::npos) {
        string_view token = input;
        input = "";
        return token;
    }
    string_view token = input.substr(0, pos);
    input = input.substr(pos + 1);
    return token;
}

// Check file extensions efficiently
bool is_cpp_file(string_view filename) {
    return filename.ends_with(".cpp") || 
           filename.ends_with(".h") ||
           filename.ends_with(".hpp");
}

// Efficient URL parsing
void parse_url(string_view url) {
    size_t protocol_end = url.find("://");
    string_view protocol = url.substr(0, protocol_end);
    // ... continue parsing
}
```

***

## IF CONSTEXPR

### 5.1 Compile-Time Conditional Code

`if constexpr` allows branching at compile-time.

#### Basic if constexpr

```cpp
#include <type_traits>

template<typename T>
void process(T value) {
    if constexpr (is_integral_v<T>) {
        cout << "Integer: " << value << "\n";
    } else if constexpr (is_floating_point_v<T>) {
        cout << "Float: " << value << "\n";
    } else {
        cout << "Other type\n";
    }
}

process(42);        // "Integer: 42" - int branch only compiled
process(3.14);      // "Float: 3.14" - double branch only compiled
process("hello");   // "Other type"
```

#### if constexpr Advantages

```cpp
// Before C++17: SFINAE (complex, verbose)
template<typename T>
enable_if_t<is_integral_v<T>>
old_way(T x) { cout << "Int\n"; }

template<typename T>
enable_if_t<is_floating_point_v<T>>
old_way(T x) { cout << "Float\n"; }

// After C++17: if constexpr (clean, readable)
template<typename T>
void new_way(T x) {
    if constexpr (is_integral_v<T>) {
        cout << "Int\n";
    } else if constexpr (is_floating_point_v<T>) {
        cout << "Float\n";
    }
}
```

#### if constexpr with Complex Logic

```cpp
template<typename T>
void serialize(const T& value) {
    if constexpr (is_arithmetic_v<T>) {
        // Serialize numbers efficiently
        write_binary(value);
    } else if constexpr (is_same_v<T, string>) {
        // Serialize strings
        write_string(value);
    } else if constexpr (requires { value.size(); }) {
        // Serialize containers
        for (const auto& item : value) {
            serialize(item);
        }
    }
}
```

#### if constexpr with Concepts (C++20-like)

```cpp
template<typename T>
void print_info(const T& value) {
    cout << "Type: " << typeid(T).name() << "\n";
    
    if constexpr (requires { value.size(); }) {
        cout << "Size: " << value.size() << "\n";
    }
    
    if constexpr (requires { value.data(); }) {
        cout << "Data pointer available\n";
    }
    
    if constexpr (is_default_constructible_v<T>) {
        T temp;  // Can create temporary
    }
}
```

### 5.2 The Death of SFINAE?

`if constexpr` replaces SFINAE for *implementation details*, but not for *overload resolution*.

**SFINAE (Old Way - C++11/14):**
```cpp
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
gcd(T a, T b) { return b == 0 ? a : gcd(b, a % b); }

template <typename T>
typename std::enable_if<!std::is_integral<T>::value, T>::type
gcd(T a, T b) { static_assert(std::is_integral<T>::value, "GCD not for floats"); }
```

**if constexpr (New Way - C++17):**
```cpp
template <typename T>
T gcd(T a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return b == 0 ? a : gcd(b, a % b);
    } else {
        static_assert(always_false<T>, "GCD not for floats");
    }
}
```
*Result*: Much cleaner, easier to debug errors.

***

## FOLD EXPRESSIONS

### 6.1 Operating on Parameter Packs

Fold expressions simplify operations on variadic templates.

#### Basic Fold Expressions

```cpp
#include <iostream>
using namespace std;

// Sum with fold
template<typename... Args>
int sum(Args... args) {
    return (... + args);  // Left fold: ((a + b) + c) + d
}

cout << sum(1, 2, 3, 4) << "\n";  // 10

// Product with fold
template<typename... Args>
int product(Args... args) {
    return (... * args);  // Left fold: ((a * b) * c) * d
}

cout << product(2, 3, 4) << "\n";  // 24

// Logical AND
template<typename... Args>
bool all_true(Args... args) {
    return (... && args);
}

cout << all_true(true, true, true) << "\n";      // true
cout << all_true(true, false, true) << "\n";     // false
```

#### Fold Directions

```cpp
// Left fold: (... + args) = ((a + b) + c) + d
template<typename... Args>
int left_fold(Args... args) {
    return (... + args);
}

// Right fold: (args + ...) = a + (b + (c + d))
template<typename... Args>
int right_fold(Args... args) {
    return (args + ...);
}

// For addition, result is the same
left_fold(1, 2, 3);   // ((1 + 2) + 3) = 6
right_fold(1, 2, 3);  // (1 + (2 + 3)) = 6

// For subtraction, different!
// left: ((1 - 2) - 3) = -4
// right: (1 - (2 - 3)) = 2
```

#### Fold with Default Value

```cpp
// No default value
template<typename... Args>
int sum_no_default(Args... args) {
    return (... + args);  // Error if no args
}

// With default value
template<typename... Args>
int sum_default(Args... args) {
    return (args + ... + 0);  // 0 if no args
}

sum_default();              // 0
sum_default(1, 2, 3);       // 6
```

#### Practical Fold Examples

```cpp
// Print all arguments
template<typename... Args>
void print_all(Args... args) {
    ((cout << args << " "), ...);
}

print_all(1, "hello", 3.14, "world");
// Output: 1 hello 3.14 world

// Check if any true
template<typename... Args>
bool any_true(Args... args) {
    return (... || args);
}

any_true(false, false, true, false);  // true

// Maximum of values
template<typename... Args>
int maximum(Args... args) {
    return max({args...});  // Fold into initializer
}

cout << maximum(3, 1, 4, 1, 5, 9) << "\n";  // 9
```

### 6.2 Advanced Fold Expressions

The comma operator `,` is the most powerful fold operator. It evaluates LHS, discards it, then RHS.

#### Calling Function on Pack
```cpp
template<typename... Args>
void log_all(Args... args) {
    (..., (cout << "[LOG] " << args << "\n")); 
}
```

#### Validating All Arguments
```cpp
template<typename... Args>
bool validate_all(Args... args) {
    // Returns true if ALL args are valid
    return (... && (args.is_valid()));
}
```

***

## CLASS TEMPLATE ARGUMENT DEDUCTION (CTAD)

### 7.1 Automatic Template Type Deduction

CTAD allows deduction of class template parameters from constructor arguments.

#### Before CTAD (C++14)

```cpp
#include <vector>
#include <map>
using namespace std;

// Must specify types explicitly
vector<int> v{1, 2, 3};
pair<string, int> p("hello", 42);
map<string, int> m{{"a", 1}, {"b", 2}};
```

#### With CTAD (C++17)

```cpp
// Types deduced automatically!
vector v{1, 2, 3};              // Deduced as vector<int>
pair p("hello", 42);             // Deduced as pair<string, int>
map m{pair("a", 1), pair("b", 2)}; // Deduced as map<string, int>

// Works with custom classes too
template<typename T, typename U>
struct Pair {
    T first;
    U second;
};

Pair p{42, 3.14};  // Deduced as Pair<int, double>
```

#### CTAD with Custom Deduction Guides

```cpp
template<typename T>
struct Container {
    vector<T> data;
};

// Without guide: Would fail
// Container c{1, 2, 3};  // ERROR - can't deduce T

// With deduction guide
template<typename T>
Container(initializer_list<T>) -> Container<T>;

// Now it works!
Container c{1, 2, 3};  // Deduced as Container<int>
```

#### Practical CTAD Examples

```cpp
// Multiple parameters
template<typename T, typename U>
class Pair {
public:
    Pair(T first, U second) : first(first), second(second) {}
    T first;
    U second;
};

Pair p{1, "hello"};  // Deduced as Pair<int, const char*>

// Arrays
array a{1, 2, 3, 4, 5};  // Deduced as array<int, 5>

// Aggregates
struct Point {
    int x, y;
};

Point pt{10, 20};  // No deduction needed (aggregate), but works
```

#### CTAD Benefits

```cpp
// Reduces verbosity
auto v1 = vector<int>{1, 2, 3};  // C++11 way
auto v2 = vector{1, 2, 3};       // C++17 way

// More readable with complex types
map m1{pair<const int, string>{1, "a"}};  // Verbose
map m2{pair{1, "a"}};                      // Clean

// Especially useful with templates
template<typename Iter>
auto process(Iter first, Iter last) {
    vector v(first, last);  // Deduced type!
    return v;
}
```

***

## STD::FILESYSTEM

### 8.1 Portable File System Operations

`std::filesystem` provides safe, portable file system access.

#### Basic Path Operations

```cpp
#include <filesystem>
#include <iostream>
using namespace std;
namespace fs = filesystem;

// Create path
fs::path p1 = "/home/user/file.txt";
fs::path p2 = "relative/path/file.txt";

// Query components
cout << p1.filename() << "\n";      // "file.txt"
cout << p1.parent_path() << "\n";   // "/home/user"
cout << p1.extension() << "\n";     // ".txt"
cout << p1.stem() << "\n";          // "file"

// Normalize
cout << p1.lexically_normal() << "\n";  // Remove .. and .
```

#### File Existence & Type Checking

```cpp
namespace fs = filesystem;

fs::path p = "/path/to/file";

if (fs::exists(p)) {
    cout << "Path exists\n";
}

if (fs::is_regular_file(p)) {
    cout << "Is a regular file\n";
    cout << "Size: " << fs::file_size(p) << " bytes\n";
}

if (fs::is_directory(p)) {
    cout << "Is a directory\n";
}

if (fs::is_symlink(p)) {
    cout << "Is a symbolic link\n";
}
```

#### Directory Operations

```cpp
namespace fs = filesystem;

fs::path dir = "/path/to/directory";

// List directory contents
for (const auto& entry : fs::directory_iterator(dir)) {
    cout << entry.path().filename() << "\n";
    if (entry.is_directory()) {
        cout << "  (directory)\n";
    }
}

// Recursive directory listing
for (const auto& entry : fs::recursive_directory_iterator(dir)) {
    cout << entry.path().relative_path(dir) << "\n";
}
```

#### File Operations

```cpp
namespace fs = filesystem;

fs::path src = "original.txt";
fs::path dst = "copy.txt";

// Copy file
fs::copy_file(src, dst);

// Move/rename
fs::rename(src, dst);

// Delete file
fs::remove(dst);

// Delete directory (must be empty)
fs::path dir = "empty_directory";
fs::remove(dir);

// Create directory
fs::path newdir = "new_directory";
fs::create_directory(newdir);

// Create nested directories
fs::create_directories("a/b/c/d");
```

#### Practical filesystem Example

```cpp
#include <filesystem>
#include <fstream>
using namespace std;
namespace fs = filesystem;

// Find all C++ files in directory
void find_cpp_files(const fs::path& dir) {
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension();
            if (ext == ".cpp" || ext == ".h" || ext == ".hpp") {
                cout << entry.path() << "\n";
            }
        }
    }
}

// Count lines in a file
int count_lines(const fs::path& file) {
    ifstream f(file);
    int count = 0;
    string line;
    while (getline(f, line)) count++;
    return count;
}

// Safe file backup
void backup_file(const fs::path& original) {
    if (!fs::exists(original)) {
        throw runtime_error("File not found");
    }
    
    fs::path backup = original;
    backup.replace_extension(
        backup.extension().string() + ".bak"
    );
    
    fs::copy_file(original, backup, 
                  fs::copy_options::overwrite_existing);
}
```

### 8.5 Filesystem Deep Dive

#### Exception-Free API
Most `std::filesystem` functions have an overload taking `std::error_code&` to avoid exceptions.

```cpp
std::error_code ec;
if (fs::exists("/tmp/ghost", ec)) {
    // ...
}
if (ec) {
    std::cerr << "Error: " << ec.message() << "\n";
}
```

#### Space & Permissions
```cpp
auto space = fs::space("/");
cout << "Free: " << space.free / 1024 / 1024 << " MB\n";

fs::permissions("file.txt", 
    fs::perms::owner_read | fs::perms::owner_write,
    fs::perm_options::add);
```

***

## STD::INVOKE

### 9.1 Uniform Callable Invocation

`std::invoke` provides uniform way to call functions, methods, and functors.

#### Basic invoke

```cpp
#include <functional>
#include <iostream>
using namespace std;

// Regular function
int add(int a, int b) { return a + b; }

// Invoke function
cout << invoke(add, 5, 3) << "\n";  // 8

// Function pointer
int (*fp)(int, int) = add;
cout << invoke(fp, 5, 3) << "\n";   // 8

// Lambda
auto lambda = [](int a, int b) { return a * b; };
cout << invoke(lambda, 5, 3) << "\n";  // 15

// Functor
struct Multiplier {
    int operator()(int a, int b) const { return a * b; }
};

Multiplier m;
cout << invoke(m, 5, 3) << "\n";  // 15
```

#### invoke with Methods

```cpp
struct Calculator {
    int value = 0;
    
    int add(int x) { return value + x; }
    int multiply(int x) const { return value * x; }
};

Calculator calc{10};

// Invoke member function
cout << invoke(&Calculator::add, calc, 5) << "\n";     // 15
cout << invoke(&Calculator::multiply, calc, 3) << "\n"; // 30

// With pointer
Calculator* ptr = &calc;
cout << invoke(&Calculator::add, ptr, 5) << "\n";      // 15

// With shared_ptr, unique_ptr
auto up = make_unique<Calculator>();
up->value = 20;
cout << invoke(&Calculator::add, up.get(), 5) << "\n"; // 25
```

#### invoke with Data Members

```cpp
struct Person {
    string name;
    int age;
};

Person p{"Alice", 30};

// Invoke data member access
cout << invoke(&Person::name, p) << "\n";  // "Alice"
cout << invoke(&Person::age, p) << "\n";   // 30

// Modify through invoke
invoke(&Person::age, p) = 31;
cout << p.age << "\n";  // 31
```

#### Practical invoke Pattern

```cpp
// Create callable wrapper that works with anything
template<typename Func, typename... Args>
auto call_wrapper(Func&& f, Args&&... args) {
    return invoke(forward<Func>(f), forward<Args>(args)...);
}

// Use with different callables
call_wrapper(add, 5, 3);              // Function
call_wrapper(lambda, 5, 3);           // Lambda
call_wrapper(&Calculator::add, calc, 5); // Member function
```

***

## PARALLEL ALGORITHMS

### 10.1 Parallel Algorithm Execution

C++17 adds parallel execution to standard algorithms.

#### Execution Policies

```cpp
#include <algorithm>
#include <execution>
#include <vector>
using namespace std;

vector<int> v = {5, 2, 8, 1, 9, 3};

// Sequential (traditional)
sort(v.begin(), v.end());

// Parallel (may use multiple threads)
sort(execution::par, v.begin(), v.end());

// Parallel unsequenced (even less ordering guarantee)
sort(execution::par_unseq, v.begin(), v.end());

// Sequenced unsequenced (no vectorization)
sort(execution::unseq, v.begin(), v.end());
```

#### Parallel Algorithm Examples

```cpp
#include <algorithm>
#include <execution>
#include <numeric>
#include <vector>

vector<int> v = {1, 2, 3, 4, 5};

// Parallel transform
transform(execution::par, v.begin(), v.end(), v.begin(),
    [](int x) { return x * 2; });

// Parallel accumulate
int sum = reduce(execution::par, v.begin(), v.end());

// Parallel find_if
auto it = find_if(execution::par, v.begin(), v.end(),
    [](int x) { return x > 5; });

// Parallel count_if
int even_count = count_if(execution::par, v.begin(), v.end(),
    [](int x) { return x % 2 == 0; });
```

#### Performance Considerations

```cpp
vector<int> large(1000000);

// Sequential: Simple, predictable
sort(large.begin(), large.end());

// Parallel: Faster for large data (overhead for small)
sort(execution::par, large.begin(), large.end());

// Parallel unsequenced: Allows compiler optimizations
sort(execution::par_unseq, large.begin(), large.end());
```

***

## CORE LANGUAGE FEATURES

### 11.1 Additional C++17 Features

#### Nested namespaces

```cpp
// C++14
namespace A {
    namespace B {
        namespace C {
            int x = 42;
        }
    }
}

// C++17
namespace A::B::C {
    int x = 42;
}
```

#### Inline Variables

```cpp
// Header file
inline int global_var = 42;  // Definition, can be in header
inline vector<int> global_vec;

// No ODR (One Definition Rule) violation
// Can include this header in multiple translation units
```

#### Structured Exception Handling (still mostly the same, but with improvements)

```cpp
try {
    // Code
} catch (const exception& e) {
    cout << e.what() << "\n";
}
```

#### Deduced Return Types in Lambdas

```cpp
auto lambda = [](int x) -> auto {  // C++17
    return x * 2;  // Return type deduced
};
```

#### std::byte

```cpp
#include <cstddef>

byte b1{42};
byte b2 = 0xAB_B;  // Literal
cout << (int)b1 << "\n";  // Cast to see value
```

***

## LIBRARY IMPROVEMENTS

### 12.1 STL Enhancements in C++17

#### std::optional Algorithms

```cpp
optional<int> opt{42};

opt.and_then([](int x) -> optional<int> {
    return x * 2;
}).or_else([]() { return optional<int>(0); });
```

#### Improved Algorithms

```cpp
vector<int> v = {1, 2, 3, 4, 5};

// uninitialized operations now work with ranges
vector<int> v2(5);
uninitialized_copy(v.begin(), v.end(), v2.begin());

// reduce (like accumulate but parallel-friendly)
int sum = reduce(v.begin(), v.end());
```

#### std::charconv

```cpp
#include <charconv>

// Fast, exception-safe number conversion
int x = 42;
char buffer[100];
auto result = to_chars(buffer, buffer + 100, x);

string str("123");
int value;
auto [ptr, ec] = from_chars(str.data(), str.data() + str.size(), value);
if (ec == errc()) {
    cout << value << "\n";  // 123
}
```

***

## POLYMORPHIC MEMORY RESOURCES (PMR)

### 13.1 Introduction to std::pmr

C++17 introduces `std::pmr` (Polymorphic Memory Resources) in `<memory_resource>`, enabling efficient, customizable memory management without changing container types.

#### The Problem with Traditional Allocators
Traditional allocators are part of the type signature: `std::vector<int, MyAlloc<int>>` is a different type from `std::vector<int>`.

`std::pmr` erases the allocator type, allowing containers with different allocation strategies to be used interchangeably.

```cpp
#include <vector>
#include <memory_resource>
#include <iostream>

// Use pmr namespace
namespace pmr = std::pmr;

void process_data(const pmr::vector<int>& data) {
    // Works with ANY allocator (stack, heap, pool)
    for (int x : data) std::cout << x << " ";
}

int main() {
    // 1. Default allocator (Heap)
    pmr::vector<int> heap_vec = {1, 2, 3};
    process_data(heap_vec);
    
    // 2. Stack allocator (Monotonic Buffer)
    std::array<std::byte, 1024> buffer;
    pmr::monotonic_buffer_resource pool{
        buffer.data(), buffer.size(), pmr::null_memory_resource()
    };
    
    pmr::vector<int> stack_vec(&pool);
    stack_vec.push_back(4);
    stack_vec.push_back(5);
    process_data(stack_vec);
    
    return 0;
}
```

### 13.2 Memory Resources

#### Standard Memory Resources

```cpp
#include <memory_resource>

// 1. new_delete_resource (Global Heap)
auto* heap = std::pmr::new_delete_resource();

// 2. null_memory_resource (Throws bad_alloc)
auto* null = std::pmr::null_memory_resource();

// 3. monotonic_buffer_resource (Fast, no deallocation)
// Very fast for building complex structures, deallocates all at once
std::pmr::monotonic_buffer_resource fast_pool(heap);

// 4. synchronized_pool_resource (Thread-safe pool)
// Good for many small allocations of same size
std::pmr::synchronized_pool_resource thread_safe_pool(heap);

// 5. unsynchronized_pool_resource (Single-thread pool)
// Fastest for single-threaded small allocations
std::pmr::unsynchronized_pool_resource local_pool(heap);
```

#### Chaining Resources

Memory resources can be chained. If a pool runs out of memory, it requests more from an "upstream" resource.

```cpp
#include <memory_resource>
#include <vector>

void chaining_example() {
    // Buffer on stack
    std::array<std::byte, 256> buffer;
    
    // Primary: Use stack buffer
    // Upstream: If buffer full, go to heap
    std::pmr::monotonic_buffer_resource mem_res(
        buffer.data(), buffer.size(), std::pmr::new_delete_resource()
    );
    
    std::pmr::vector<int> vec(&mem_res);
    
    // These go to stack buffer
    for (int i = 0; i < 50; i++) vec.push_back(i);
    
    // If we exceed buffer, it silently falls back to heap
}
```

#### Performance Benefits

`std::pmr` allows easy implementation of **Arena Allocation** or **Stack Allocation** for standard containers, which can provide massive performance gains (cache locality, no malloc overhead) for short-lived complex data structures.

***

## C++17 BEST PRACTICES

### What's Better with C++17

```cpp
// 1. Use structured bindings to unpack
auto [x, y, z] = tuple{1, 2, 3};

// 2. Use optional for nullable values
optional<int> result = process();
if (result) { cout << result.value() << "\n"; }

// 3. Use variant for type-safe unions
variant<int, string> value = 42;

// 4. Use string_view for non-owning strings
void process(string_view sv);  // No copy!

// 5. Use if constexpr for compile-time branching
if constexpr (is_integral_v<T>) { }

// 6. Use fold expressions for parameter packs
auto sum = (... + args);

// 7. Use filesystem for file operations
for (const auto& entry : fs::directory_iterator(dir)) { }

// 8. Use invoke for uniform callable invocation
invoke(func, args...);

// 9. Use parallel algorithms for performance
sort(execution::par, v.begin(), v.end());

// 10. Use CTAD for cleaner code
vector v{1, 2, 3};  // Not vector<int>{...}
```


***

## Volume III: Modern Mastery

### <a name="chapter-11-c20revolutionaryfeatures"></a>CHAPTER 11: C++20 REVOLUTIONARY FEATURES

### C++20 Overview & Revolutionary Scope

C++20 (finalized in 2020) is a **revolutionary language update** rivaling C++11 in magnitude.

#### Timeline & Context
- **2011**: C++11 (first modern standard)
- **2014**: C++14 (refinement)
- **2017**: C++17 (major improvements)
- **2020**: C++20 (revolutionary leap)
- **2023**: C++23 (latest)

#### C++20 Philosophy
- **Revolutionize** generic programming with concepts
- **Simplify** iteration with ranges
- **Empower** asynchronous programming with coroutines
- **Standardize** previously non-standard patterns
- **Address** fundamental C++ limitations
- **Enable** modern programming paradigms

#### Key Themes
1. **Concepts** - Readable, constrained templates
2. **Ranges** - Composable, lazy evaluation
3. **Coroutines** - Asynchronous, generator patterns
4. **Spaceship** - Three-way comparison
5. **Modules** - Modularity & faster compilation
6. **Designated Initializers** - Named struct initialization
7. **Format** - Type-safe string formatting
8. **Constraints** - Compile-time validation

#### Why C++20 Matters
C++20 addresses fundamental limitations:
- ✅ Readable generic programming (concepts)
- ✅ Composable iteration (ranges)
- ✅ Async/await patterns (coroutines)
- ✅ Lazy evaluation (ranges with coroutines)
- ✅ Modular code (modules)
- ✅ Type-safe formatting (std::format)
- ✅ Compile-time validation (consteval)
- ✅ Powerful iteration patterns

***

## CONCEPTS & CONSTRAINTS

### 1.1 Introduction to Concepts

Concepts are constraints on template parameters that make templates readable and enable better error messages.

#### Basic Concept Definition

```cpp
#include <concepts>
using namespace std;

// Define a concept
template<typename T>
concept Integral = is_integral_v<T>;

// Use concept as constraint
template<Integral T>
void process(T value) {
    cout << "Integer: " << value << "\n";
}

process(42);              // OK - int satisfies Integral
process(3.14);            // ERROR - double doesn't satisfy Integral
// Error message is clear: doesn't satisfy Integral concept
```

#### Standard Library Concepts

```cpp
#include <concepts>

// Predefined concepts
template<typename T>
concept Integer = integral<T>;  // std::integral

template<typename T>
concept Floating = floating_point<T>;  // std::floating_point

template<typename T>
concept Numeric = integral<T> || floating_point<T>;

template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> convertible_to<bool>;
    { a == b } -> convertible_to<bool>;
};

// Usage
template<Comparable T>
T find_min(T a, T b) {
    return a < b ? a : b;
}

find_min(5, 3);           // OK
find_min("a", "b");       // OK - strings are comparable
// find_min(complex(1,2), complex(3,4));  // ERROR - complex not comparable
```

#### Complex Concept Definition

```cpp
#include <concepts>
#include <ranges>

// Concept with multiple requirements
template<typename T>
concept Container = requires(T c) {
    typename T::value_type;
    typename T::iterator;
    typename T::const_iterator;
    { c.begin() } -> convertible_to<typename T::iterator>;
    { c.end() } -> convertible_to<typename T::iterator>;
    { c.size() } -> convertible_to<size_t>;
    { c.empty() } -> convertible_to<bool>;
};

// Use in function
template<Container C>
void print_container(const C& c) {
    for (const auto& elem : c) {
        cout << elem << " ";
    }
    cout << "\n";
}

vector<int> v{1, 2, 3};
print_container(v);  // OK

// Custom type
struct MyContainer {
    vector<int> data;
    using value_type = int;
    using iterator = vector<int>::iterator;
    using const_iterator = vector<int>::const_iterator;
    
    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    const_iterator begin() const { return data.begin(); }
    const_iterator end() const { return data.end(); }
    size_t size() const { return data.size(); }
    bool empty() const { return data.empty(); }
};

MyContainer mc;
print_container(mc);  // OK
```

#### Concept Benefits

```cpp
// Before C++20: Complex error messages
template<typename T>
void process_old(T x) {
    // If T doesn't have operator+, error is confusing
    auto result = x + 5;
}

process_old("string");  // ERROR - cryptic, long error message

// After C++20: Clear error messages
template<typename T>
requires requires(T x) { x + 5; }
void process_new(T x) {
    auto result = x + 5;
}

process_new("string");  // ERROR - "string" doesn't satisfy concept
```

***

### 1.2 Requires Expressions

Requires expressions test compile-time properties of types.

#### Basic Requires Expression

```cpp
#include <concepts>

template<typename T>
requires requires(T x) {
    x + 1;           // Must support addition
    x.size();        // Must have size() member
    { x == x };      // Must support equality
}
void process(T x);

// Can also write as concept
template<typename T>
concept Processable = requires(T x) {
    x + 1;
    x.size();
    { x == x };
};

template<Processable T>
void process2(T x);
```

#### Requires with Return Type Checking

```cpp
#include <concepts>

template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> convertible_to<T>;
};

template<typename T>
concept Multipliable = requires(T a, T b) {
    { a * b } -> convertible_to<T>;
};

template<typename T>
concept Arithmetic = Addable<T> && Multipliable<T>;

template<Arithmetic T>
T compute(T x, T y) {
    return (x + y) * (x - y);
}

cout << compute(5, 3) << "\n";        // OK - int is Arithmetic
cout << compute(2.5, 1.5) << "\n";    // OK - double is Arithmetic
```

#### Practical Requires Examples

```cpp
// Check for operator[] and size()
template<typename T>
concept Indexable = requires(T t, size_t i) {
    { t[i] };
    { t.size() } -> convertible_to<size_t>;
};

// Check for specific method
template<typename T>
concept HasValue = requires(T t) {
    { t.value() };
};

// Check for const and non-const versions
template<typename T>
concept ConstIterable = requires(const T& t) {
    t.begin();
    t.end();
};

// Multi-type concepts
template<typename Iter, typename Sentinel>
concept SentinelFor = requires(Iter it, Sentinel s) {
    { it == s } -> convertible_to<bool>;
};
```

### 1.3 Concepts & Overload Resolution

Concepts participate in overload resolution. The compiler selects the **most constrained** template.

```cpp
template<typename T>
void process(T x) {
    cout << "Generic\n";
}

template<typename T> requires std::integral<T>
void process(T x) {
    cout << "Integral\n";
}

template<typename T> requires (std::integral<T> && sizeof(T) >= 4)
void process(T x) {
    cout << "Large Integral\n";
}

process(3.14);      // "Generic"
process((short)10); // "Integral"
process(100);       // "Large Integral" (int is >= 4 bytes)
```

***

## RANGES LIBRARY

### 2.1 Introduction to Ranges

Ranges provide a composable, lazy way to work with sequences.

#### Basic Range Operations

```cpp
#include <ranges>
#include <vector>
#include <iostream>
using namespace std;

vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Traditional algorithm
vector<int> result;
for (int x : v) {
    if (x % 2 == 0) {
        result.push_back(x * 2);
    }
}

// With ranges (composable, lazy)
auto result = v
    | ranges::views::filter([](int x) { return x % 2 == 0; })
    | ranges::views::transform([](int x) { return x * 2; });

// result is lazy - computation happens on iteration
for (int x : result) {
    cout << x << " ";  // 4 8 12 16 20
}
```

#### Range Views

```cpp
#include <ranges>
#include <vector>
using namespace std;

vector<int> v = {1, 2, 3, 4, 5};

// filter view
auto evens = v | ranges::views::filter([](int x) { return x % 2 == 0; });

// transform view
auto doubled = v | ranges::views::transform([](int x) { return x * 2; });

// take view (first N elements)
auto first3 = v | ranges::views::take(3);

// drop view (skip first N elements)
auto skip2 = v | ranges::views::drop(2);

// reverse view
auto reversed = v | ranges::views::reverse;

// iota view (generate sequence)
auto seq = ranges::views::iota(1, 11);  // 1..10

// join view (flatten nested ranges)
vector<vector<int>> matrix = {{1, 2}, {3, 4}, {5, 6}};
auto flattened = matrix | ranges::views::join;

// zip view (pair elements from two ranges)
vector<int> a = {1, 2, 3};
vector<string> b = {"a", "b", "c"};
auto zipped = ranges::views::zip(a, b);

for (auto [num, str] : zipped) {
    cout << num << ":" << str << " ";  // 1:a 2:b 3:c
}
```

#### Range Algorithms

```cpp
#include <ranges>
#include <vector>
#include <algorithm>
using namespace std;

vector<int> v = {3, 1, 4, 1, 5, 9};

// Range algorithms (work with ranges, not iterators)
ranges::sort(v);                    // In-place sort
ranges::reverse(v);                 // In-place reverse
ranges::fill(v, 0);                 // Fill with value

// Range algorithms with predicates
ranges::sort(v, ranges::greater{});  // Sort descending
auto it = ranges::find(v, 5);        // Find element
auto count = ranges::count_if(v, [](int x) { return x > 3; });

// Range operations
ranges::rotate(v.begin(), v.begin() + 2, v.end());
ranges::partition(v, [](int x) { return x % 2 == 0; });
```

#### Composing Multiple Views

```cpp
#include <ranges>
#include <vector>
#include <iostream>
using namespace std;

vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Chain multiple operations
auto result = v
    | ranges::views::filter([](int x) { return x > 2; })      // > 2
    | ranges::views::transform([](int x) { return x * x; })   // Square
    | ranges::views::take(4);                                   // First 4

// Process
for (int x : result) {
    cout << x << " ";  // 9 16 25 36
}

// All operations are lazy - no temporary vectors created
// Composition is clear and readable
```

### 2.2 Ranges Deep Dive

#### Projections
Most range algorithms accept a "projection" argument to transform data *before* comparison.

```cpp
struct User { int id; string name; };
vector<User> users = {{2, "Bob"}, {1, "Alice"}};

// Sort by ID
ranges::sort(users, {}, &User::id);

// Sort by Name (descending)
ranges::sort(users, ranges::greater{}, &User::name);
```

#### Dangling Iterators
Algorithms return `std::ranges::dangling` if the range is an rvalue (temporary) to prevent use-after-free.

```cpp
auto get_vector() { return vector{1, 2, 3}; }

auto it = ranges::find(get_vector(), 2); 
// Compile Error! 'it' would be dangling.
// The vector is destroyed at the end of the statement.
```

***

## COROUTINES

### 3.1 Introduction to Coroutines

Coroutines enable asynchronous, generator, and lazy evaluation patterns.

#### Basic Generator Coroutine

```cpp
#include <coroutine>
#include <iostream>
using namespace std;

template<typename T>
class Generator {
public:
    struct promise_type {
        T current_value;
        
        Generator get_return_object() {
            return Generator{coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        suspend_never initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        
        suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }
        
        void return_void() {}
        void unhandled_exception() {}
    };
    
    struct iterator {
        coroutine_handle<promise_type> handle;
        
        iterator(coroutine_handle<promise_type> h, bool done) 
            : handle(h) {
            if (done) {
                handle = nullptr;
            }
        }
        
        iterator& operator++() {
            handle.resume();
            if (handle.done()) {
                handle = nullptr;
            }
            return *this;
        }
        
        bool operator==(const iterator& other) const {
            return handle == other.handle;
        }
        
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
        
        T operator*() const {
            return handle.promise().current_value;
        }
    };
    
    iterator begin() {
        if (handle) {
            handle.resume();
        }
        return iterator{handle, !handle || handle.done()};
    }
    
    iterator end() {
        return iterator{nullptr, true};
    }
    
private:
    coroutine_handle<promise_type> handle;
    
    Generator(coroutine_handle<promise_type> h) : handle(h) {}
};

// Generator coroutine
Generator<int> count_up(int max) {
    for (int i = 1; i <= max; i++) {
        co_yield i;  // Yield value and suspend
    }
}

// Usage
int main() {
    for (int i : count_up(5)) {
        cout << i << " ";  // 1 2 3 4 5
    }
    return 0;
}
```

#### Async Coroutine

```cpp
#include <coroutine>
#include <iostream>
#include <chrono>
using namespace std;

class Task {
public:
    struct promise_type {
        Task get_return_object() {
            return Task{coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        suspend_never initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        
        void return_void() {}
        void unhandled_exception() {}
    };
    
    coroutine_handle<promise_type> handle;
    
    Task(coroutine_handle<promise_type> h) : handle(h) {}
    
    ~Task() {
        if (handle) {
            handle.destroy();
        }
    }
};

// Async coroutine
Task async_work() {
    cout << "Starting work\n";
    co_await std::suspend_always{};  // Suspend and resume later
    cout << "Continuing work\n";
}

int main() {
    auto task = async_work();  // Starts coroutine
    // Coroutine is suspended
    task.handle.resume();       // Resume execution
    return 0;
}
```

#### Practical Coroutine: Fibonacci Generator

```cpp
#include <coroutine>
#include <iostream>
using namespace std;

template<typename T>
class Generator { /* ... implementation ... */ };

Generator<int> fibonacci(int limit) {
    int a = 0, b = 1;
    while (a < limit) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    for (int i : fibonacci(100)) {
        cout << i << " ";  // 0 1 1 2 3 5 8 13 21 34 55 89
    }
    return 0;
}
```

### 3.2 Coroutines Deep Dive

A coroutine is a function that can suspend and resume.

#### The Awaitable Interface
To `co_await x`, `x` must be an Awaitable.

```cpp
struct Awaiter {
    bool await_ready() { return false; } // Always suspend?
    
    void await_suspend(std::coroutine_handle<> h) {
        // Schedule resumption (e.g., on a thread pool)
        // h.resume(); 
    }
    
    int await_resume() { return 42; } // Result of co_await
};

Task coroutine() {
    int result = co_await Awaiter{}; // result = 42
}
```

#### Symmetric Transfer
Returning a `coroutine_handle` from `await_suspend` performs a "tail-call" to resume another coroutine without consuming stack space.

```cpp
std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) {
    return other_handle; // Switch to other coroutine immediately
}
```

***

## SPACESHIP OPERATOR (THREE-WAY COMPARISON)

### 4.1 The Spaceship Operator <=>

The spaceship operator performs three-way comparison and returns comparison category.

#### Basic Spaceship Usage

```cpp
#include <compare>
#include <iostream>
using namespace std;

int a = 5, b = 10;

// Spaceship operator returns ordering
auto cmp = a <=> b;

if (cmp < 0) {
    cout << "a < b\n";
} else if (cmp > 0) {
    cout << "a > b\n";
} else {
    cout << "a == b\n";
}
```

#### Spaceship with Custom Types

```cpp
#include <compare>

struct Person {
    string name;
    int age;
    
    // Default spaceship (compares as tuple)
    auto operator<=>(const Person&) const = default;
};

Person p1{"Alice", 30};
Person p2{"Bob", 25};

auto cmp = p1 <=> p2;
if (cmp < 0) cout << "p1 < p2\n";
if (cmp > 0) cout << "p1 > p2\n";
```

#### Defaulted Comparison

```cpp
#include <compare>

struct Point {
    int x, y;
    
    // Default spaceship - compares lexicographically
    auto operator<=>(const Point&) const = default;
};

Point p1{1, 2};
Point p2{1, 2};
Point p3{2, 1};

cout << (p1 <=> p2 == 0) << "\n";     // true (equal)
cout << (p1 <=> p3 < 0) << "\n";      // true (p1 < p3)
```

#### Comparison Categories

```cpp
#include <compare>
#include <iostream>

// Different comparison categories
struct Comparable {
    int value;
    
    // Returns std::strong_ordering (can do all operations)
    strong_ordering operator<=>(const Comparable& other) const {
        return value <=> other.value;
    }
};

struct PartiallyComparable {
    double value;
    
    // Returns std::partial_ordering (NaN is not comparable)
    partial_ordering operator<=>(const PartiallyComparable& other) const {
        return value <=> other.value;
    }
};

Comparable c1{5}, c2{10};
cout << (c1 <=> c2 < 0) << "\n";  // true

PartiallyComparable p1{1.5}, p2{2.5};
cout << (p1 <=> p2 < 0) << "\n";  // true
```

#### Spaceship Benefits

```cpp
// Before C++20: Must define all comparison operators
struct Person {
    string name;
    int age;
    
    bool operator<(const Person& other) const {
        if (name != other.name) return name < other.name;
        return age < other.age;
    }
    
    bool operator<=(const Person& other) const { /* ... */ }
    bool operator>(const Person& other) const { /* ... */ }
    bool operator>=(const Person& other) const { /* ... */ }
    bool operator==(const Person& other) const { /* ... */ }
    bool operator!=(const Person& other) const { /* ... */ }
};

// After C++20: Default spaceship does all of it
struct Person {
    string name;
    int age;
    
    auto operator<=>(const Person&) const = default;
};
```

***

## MODULES

### 5.1 Introduction to Modules

Modules provide better code organization and faster compilation.

#### Module Definition

```cpp
// math_module.cppm (module interface unit)
export module math;

export int add(int a, int b) {
    return a + b;
}

export int multiply(int a, int b) {
    return a * b;
}

// Helper function (not exported)
int helper(int x) {
    return x * 2;
}
```

#### Using Modules

```cpp
// main.cpp
import math;
#include <iostream>
using namespace std;

int main() {
    cout << add(5, 3) << "\n";              // OK - exported
    cout << multiply(5, 3) << "\n";         // OK - exported
    // cout << helper(5) << "\n";           // ERROR - not exported
    
    return 0;
}
```

#### Module Partitions

```cpp
// math.cppm (main interface)
export module math;
export import :impl;

// math-impl.cppm (partition)
export module math:impl;

export struct Complex {
    double real, imag;
    
    Complex operator+(const Complex& other) const {
        return {real + other.real, imag + other.imag};
    }
};
```

#### Module Benefits

```
// Before modules (header files):
// - Recompilation overhead
// - Macro pollution
// - Circular dependencies
// - Header guards boilerplate

// After modules:
// - Faster compilation (parse once)
// - No macro pollution
// - No circular dependency issues
// - Clean interface definition
```

### 5.2 Modules Deep Dive

#### Global Module Fragment
For legacy headers that must be included before the module declaration.

```cpp
module; // Start fragment
#include <vector>
#include <string>

export module my_app; // End fragment, start module

export void process(std::vector<int>& v);
```

#### Private Module Partition
Hiding implementation details within the same file.

```cpp
export module calculator;

export int add(int a, int b);

module :private; // Start private implementation

int helper(int x) { return x + 1; }

int add(int a, int b) {
    return helper(a) + helper(b) - 2;
}
```

***

## DESIGNATED INITIALIZERS

### 6.1 Named Member Initialization

Designated initializers allow initializing struct/class members by name.

#### Basic Designated Initializers

```cpp
#include <iostream>
using namespace std;

struct Point {
    int x;
    int y;
    int z;
};

// Before C++20: Order matters
Point p1{1, 2, 3};  // x=1, y=2, z=3

// After C++20: Can specify by name
Point p2{.x = 10, .y = 20, .z = 30};
Point p3{.y = 20, .x = 10, .z = 30};  // Order doesn't matter
Point p4{.x = 5, .z = 15};             // y defaults to 0

cout << p2.x << " " << p2.y << " " << p2.z << "\n";  // 10 20 30
```

#### With Classes and Inheritance

```cpp
struct Base {
    int a;
};

struct Derived : Base {
    int b;
    int c;
};

// Designators for base and derived members
Derived d{.a = 1, .b = 2, .c = 3};

cout << d.a << " " << d.b << " " << d.c << "\n";  // 1 2 3
```

#### Practical Designated Initializers

```cpp
struct Config {
    string name;
    int port;
    string host;
    bool ssl;
    int timeout;
};

// Clear intent - parameters obvious
Config cfg{
    .name = "server",
    .port = 8080,
    .host = "localhost",
    .ssl = true,
    .timeout = 30
};

// Much better than:
// Config cfg{"server", 8080, "localhost", true, 30};
```

***

## CALENDAR & TIME ZONES

### 7.1 Advanced Chrono Features

C++20 adds comprehensive calendar and timezone support.

#### Calendar Types

```cpp
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

// Year, month, day
year y{2024};
month m{12};
day d{25};

// Construct date
auto date = y / m / d;  // 2024-12-25
cout << date << "\n";

// Current date
auto today = floor<days>(system_clock::now());
cout << "Today: " << today << "\n";

// Date arithmetic
auto tomorrow = date + days(1);
auto next_month = date + months(1);
auto next_year = date + years(1);
```

#### Time Zones

```cpp
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

// Get timezone
const auto& tz = locate_zone("America/New_York");

// Current time in timezone
auto now = system_clock::now();
auto zoned_time = make_zoned(tz, now);

cout << "UTC: " << now << "\n";
cout << "NY: " << zoned_time << "\n";
```

#### Formatted Time Output

```cpp
#include <chrono>
#include <format>
#include <iostream>
using namespace std;
using namespace chrono;

auto now = system_clock::now();

// Format with pattern
cout << format("{:%Y-%m-%d %H:%M:%S}", now) << "\n";
// Output: 2024-12-25 15:30:45
```

***

## STD::FORMAT

### 8.1 Type-Safe String Formatting

`std::format` provides Python-like formatting without type unsafety.

#### Basic format Usage

```cpp
#include <format>
#include <iostream>
using namespace std;

// Simple substitution
cout << format("Hello {}, you are {} years old", "Alice", 30) << "\n";
// Output: Hello Alice, you are 30 years old

// Positional arguments
cout << format("{1} {0}", "World", "Hello") << "\n";
// Output: Hello World

// Argument access by index
cout << format("{0} + {0} = {}", 5, 10) << "\n";
// Output: 5 + 5 = 10
```

#### Formatting Specifications

```cpp
#include <format>
#include <iostream>
using namespace std;

int num = 255;
double pi = 3.14159;

// Hex, binary, octal
cout << format("{:x}", num) << "\n";           // ff (hex)
cout << format("{:b}", num) << "\n";           // 11111111 (binary)
cout << format("{:o}", num) << "\n";           // 377 (octal)

// Floating point precision
cout << format("{:.2f}", pi) << "\n";          // 3.14
cout << format("{:.5f}", pi) << "\n";          // 3.14159

// Padding and alignment
cout << format("{:>10}", "hello") << "\n";     // "     hello" (right)
cout << format("{:<10}", "hello") << "\n";     // "hello     " (left)
cout << format("{:^10}", "hello") << "\n";     // "  hello   " (center)

// Number formatting
cout << format("{:,}", 1234567) << "\n";       // 1,234,567 (with separator)
cout << format("{:e}", pi) << "\n";            // 3.14e+00 (scientific)
```

#### Format with Custom Types

```cpp
#include <format>

struct Point {
    int x, y;
};

// Define formatter for Point
template<>
struct format_traits<Point> {
    static auto format(const Point& p) {
        return format_string("({}, {})", p.x, p.y);
    }
};

Point pt{10, 20};
cout << format("Point: {}", pt) << "\n";  // Point: (10, 20)
```

***

## CONSTEVAL & CONSTINIT

### 9.1 Immediate Functions and Constants

#### consteval - Immediate Functions

```cpp
#include <iostream>
using namespace std;

// Must be evaluated at compile-time
consteval int square(int x) {
    return x * x;
}

int main() {
    int arr[square(5)];           // OK - computed at compile-time
    cout << square(10) << "\n";   // OK - 100
    
    int x = 5;
    // cout << square(x) << "\n"; // ERROR - x is not compile-time constant
    
    return 0;
}
```

#### constinit - Compile-Time Initialization

```cpp
#include <iostream>
using namespace std;

// Thread-local with compile-time initialization
thread_local constinit int counter = 0;

int main() {
    counter = 10;  // Can be modified at runtime
    cout << counter << "\n";  // 10
    
    return 0;
}
```

#### Difference: constexpr vs consteval

```cpp
// constexpr: Can be evaluated at compile-time OR runtime
constexpr int add_constexpr(int a, int b) {
    return a + b;
}

// consteval: MUST be evaluated at compile-time
consteval int add_consteval(int a, int b) {
    return a + b;
}

int main() {
    int x = 5, y = 10;
    
    int c1 = add_constexpr(x, y);      // Runtime evaluation
    int c2 = add_constexpr(5, 10);     // Compile-time evaluation
    
    // int d1 = add_consteval(x, y);   // ERROR - must be compile-time
    int d2 = add_consteval(5, 10);     // OK - compile-time
    
    return 0;
}
```

***

## LAMBDA ENHANCEMENTS

### 10.1 C++20 Lambda Improvements

#### Default Constructible Lambdas

```cpp
#include <iostream>
using namespace std;

// C++20: Lambdas without captures can be default constructed
auto counter = [count = 0]() mutable { return ++count; };

// Can be default constructed
decltype(counter) c1;  // Default construct
c1();

// But lambdas with captures still can't
// auto [x] = 5;
// decltype([x]() {}) bad;  // ERROR
```

#### Stateless Lambda as Template Parameter

```cpp
#include <iostream>
using namespace std;

template<auto F>
void call_func() {
    F();
}

// Stateless lambda as template argument
call_func<[]() { cout << "Hello\n"; }>();  // OK

// Stateful lambda (captures) can't be template argument
// auto y = 5;
// call_func<[y]() { cout << y; }>();  // ERROR
```

***

## ADVANCED FEATURES

### 11.1 Additional C++20 Features

#### Spaceship Operator with Library Support

```cpp
#include <compare>
#include <vector>

// All standard library types support spaceship
vector<int> v1{1, 2, 3};
vector<int> v2{1, 2, 4};

auto cmp = v1 <=> v2;
if (cmp < 0) cout << "v1 < v2\n";
```

#### Bit Operations

```cpp
#include <bit>
#include <iostream>
using namespace std;

unsigned int x = 12;  // 0b1100

cout << bit_width(x) << "\n";           // 4 (bits needed)
cout << popcount(x) << "\n";            // 2 (number of 1s)
cout << countl_zero(x) << "\n";         // 28 (leading zeros on 32-bit)
cout << rotl(x, 2) << "\n";             // Rotate left
cout << rotr(x, 2) << "\n";             // Rotate right
cout << (x & ~(x - 1)) << "\n";         // Lowest set bit

// std::bit_cast (Safe type punning)
float f = 3.14f;
auto i = std::bit_cast<uint32_t>(f);  // Safe reinterpretation of bits
cout << std::hex << i << "\n";
```

#### std::atomic_ref

`std::atomic_ref` allows atomic operations on non-atomic objects.

```cpp
#include <atomic>
#include <thread>
#include <vector>

void process(int& counter) {
    // Treat 'counter' as atomic for this scope
    std::atomic_ref<int> atomic_counter(counter);
    atomic_counter++;
}

int main() {
    int val = 0;
    std::vector<std::thread> threads;
    for(int i=0; i<10; ++i) threads.emplace_back(process, std::ref(val));
    for(auto& t : threads) t.join();
    return 0;
}
```

#### Concepts in Standard Library

```cpp
#include <concepts>
#include <iostream>

// Standard concepts
static_assert(integral<int>);
static_assert(floating_point<double>);
static_assert(invocable<int(*)(int), int>);
static_assert(copyable<int>);
static_assert(assignable_from<int&, int>);

template<typename T>
requires copyable<T>
void copy_safe(const T& src, T& dst) {
    dst = src;
}
```

***

## LIBRARY IMPROVEMENTS

### 12.1 STL Enhancements in C++20

#### std::span (Non-owning Array View)

`std::span` provides a lightweight, non-owning view over a contiguous sequence of objects (like array, vector, or C-array).

```cpp
#include <span>
#include <vector>
#include <iostream>
#include <array>

void print_values(std::span<int> data) {
    for (int x : data) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

int main() {
    int arr[] = {1, 2, 3};
    std::vector<int> vec = {4, 5, 6};
    std::array<int, 3> std_arr = {7, 8, 9};

    // Works with all contiguous containers
    print_values(arr);        // 1 2 3
    print_values(vec);        // 4 5 6
    print_values(std_arr);    // 7 8 9
    
    // Sub-span (slicing)
    print_values(std::span(vec).subspan(1)); // 5 6
    
    return 0;
}
```

#### std::semaphore

```cpp
#include <semaphore>
#include <thread>

counting_semaphore<3> sem(3);  // Max 3 concurrent

void worker() {
    sem.acquire();
    // Critical section (at most 3 threads)
    // Do work
    sem.release();
}
```

#### std::latch & std::barrier

```cpp
#include <latch>
#include <barrier>
#include <thread>
#include <vector>

// Latch: one-time synchronization
latch finish(3);

void worker(latch& l) {
    // Do work
    l.count_down();
    l.wait();  // Wait for all to finish
}

// Barrier: reusable synchronization
barrier sync(3);

void barrier_worker(barrier& b) {
    while (true) {
        // Do work
        b.arrive_and_wait();  // Synchronize every iteration
    }
}
```

#### std::source_location (Reflection for Logging)

```cpp
#include <source_location>
#include <iostream>

void log(const char* message, 
         const std::source_location location = std::source_location::current()) {
    std::cout << "Info: " << message << "\n"
              << "File: " << location.file_name() << "("
              << location.line() << ":" << location.column() << ")\n"
              << "Func: " << location.function_name() << "\n";
}

int main() {
    log("Something happened");
    return 0;
}
```

#### std::osyncstream (Synchronized Output)

Prevents interleaved output from multiple threads.

```cpp
#include <syncstream>
#include <iostream>
#include <thread>

void worker(int id) {
    std::osyncstream(std::cout) << "Worker " << id << " is running\n";
}

int main() {
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    t1.join(); t2.join();
    return 0;
}
```

#### Ranges with Algorithms

```cpp
#include <ranges>
#include <vector>
#include <algorithm>

vector<int> v = {3, 1, 4, 1, 5};

// Ranges algorithms with pipes
v | ranges::views::sort
  | ranges::views::unique
  | ranges::views::take(3)
```
