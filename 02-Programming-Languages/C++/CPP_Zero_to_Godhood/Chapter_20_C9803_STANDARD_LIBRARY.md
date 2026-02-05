# C++98/03 STANDARD LIBRARY


## Standard Template Library

---

## Introduction to STL

The Standard Template Library (STL) is a collection of template classes and functions that provide:
- **Containers** - Data structures to hold objects
- **Iterators** - Objects to traverse containers
- **Algorithms** - Functions to manipulate data
- **Function Objects** - Objects that act like functions

### Key Advantages
- Generic programming (templates)
- High performance (optimized)
- Code reuse
- Type-safe
- Well-tested and standardized

---

## STL Components Overview

```

          STL (Standard Template Library)  

                                           
         
   CONTAINERS       ITERATORS          
         
    Sequence       Input             
    Associative    Output            
    Adapters       Forward           
      Bidirectional|     |
                      Random Access     
    --     
   ALGORITHMS            
    FUNCTION OBJ.        
    Searching           
    Sorting        Predicates        
    Modifying      Comparators       
    Numeric        Functors          
           

```

---

## CONTAINERS - COMPLETE REFERENCE

## Container Characteristics

| Container | Type | Insert | Delete | Search | Random Access | Memory |
|-----------|------|--------|--------|--------|---------------|--------|
| vector | Sequence | O(n) | O(n) | O(n) | O(1) | Contiguous |
| list | Sequence | O(1) | O(1) | O(n) | O(n) | Scattered |
| deque | Sequence | O(n) | O(n) | O(n) | O(1) | Blocks |
| map | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| set | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| multimap | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| multiset | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| unordered_map | Hash | O(1) avg | O(1) avg | O(1) avg | - | Hash |
| unordered_set | Hash | O(1) avg | O(1) avg | O(1) avg | - | Hash |
| priority_queue | Adapter | O(log n) | O(log n) | O(n) | - | Heap |
| queue | Adapter | O(1) | O(1) | O(n) | - | - |
| stack | Adapter | O(1) | O(1) | O(n) | - | - |

---

## 1.1 VECTOR - Dynamic Array

### What is Vector?
A dynamic array that grows automatically. Use this most of the time.

### Declaration & Initialization

```cpp
#include <vector>
using namespace std;

// Empty vector
vector<int> v1;

// Vector with initial size
vector<int> v2(10);           // 10 elements, initialized to 0

// Vector with initial values
vector<int> v3(5, 10);        // 5 elements, all set to 10

// Copy constructor
vector<int> v4(v3);           // Copy of v3

// From array (C++11)
int arr[] = {1, 2, 3, 4, 5};
vector<int> v5(arr, arr + 5); // From array

// Initializer list (C++11)
vector<int> v6 = {1, 2, 3, 4, 5};

// Deduction (C++17)
vector v7 = {1, 2, 3};        // Type deduced as vector<int>
```

### Accessing Elements

```cpp
vector<int> v = {10, 20, 30, 40, 50};

// 1. Using operator[]
cout << v[0] << "\n";         // 10 - No bounds checking

// 2. Using at()
cout << v.at(0) << "\n";      // 10 - With bounds checking, throws exception

// 3. Front and back
cout << v.front() << "\n";    // 10 (first element)
cout << v.back() << "\n";     // 50 (last element)

// 4. Direct pointer access (C++11)
int* ptr = v.data();          // Pointer to underlying array
cout << ptr[0] << "\n";       // 10
```

### Modifying Elements

```cpp
vector<int> v = {10, 20, 30};

// Assignment
v[0] = 100;
v.at(1) = 200;

// Adding elements
v.push_back(40);              // Add to end: {10, 200, 30, 40}
v.insert(v.begin() + 1, 15);  // Insert 15 at index 1: {10, 15, 200, 30, 40}

// Removing elements
v.pop_back();                 // Remove last: {10, 15, 200, 30}
v.erase(v.begin() + 1);       // Remove at index 1: {10, 200, 30}
v.erase(v.begin(), v.begin() + 2);  // Remove first 2: {30}

// Clear all
v.clear();                    // Empty vector
```

### Size & Capacity

```cpp
vector<int> v = {1, 2, 3};

cout << v.size() << "\n";      // 3 - Number of elements
cout << v.capacity() << "\n";  // 3+ - Allocated space
cout << v.empty() << "\n";     // false

// Reserve space (optimization)
v.reserve(100);                // Allocate for 100 elements
cout << v.capacity() << "\n";  // 100

// Resize
v.resize(5, 0);                // Resize to 5, new elements = 0
v.resize(2);                   // Shrink to 2 elements

// Shrink to fit (C++11)
v.shrink_to_fit();             // Release unused capacity
```

### Iterating Through Vector

```cpp
vector<int> v = {10, 20, 30, 40, 50};

// 1. Traditional for loop
for (int i = 0; i < v.size(); i++) {
    cout << v[i] << " ";
}

// 2. Iterator loop
for (vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    cout << *it << " ";
}

// 3. Auto with iterator (C++11)
for (auto it = v.begin(); it != v.end(); ++it) {
    cout << *it << " ";
}

// 4. Range-based for (C++11)
for (int val : v) {
    cout << val << " ";
}

// 5. Reverse iteration
for (auto it = v.rbegin(); it != v.rend(); ++it) {
    cout << *it << " ";
}
```

### Comparison & Assignment

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

### Complete Vector Example

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> nums;
    
    // Adding elements
    for (int i = 1; i <= 5; i++) {
        nums.push_back(i * 10);
    }
    
    // Display
    cout << "Vector contents: ";
    for (int num : nums) {
        cout << num << " ";
    }
    cout << "\n";
    
    // Modify
    nums[2] = 999;
    
    // Insert
    nums.insert(nums.begin() + 2, 777);
    
    // Remove
    nums.erase(nums.begin() + 1);
    
    // Display after modifications
    cout << "After modifications: ";
    for (int num : nums) {
        cout << num << " ";
    }
    cout << "\nSize: " << nums.size() << "\n";
    
    return 0;
}
```

---

## 1.2 DEQUE - Double Ended Queue

### What is Deque?
Fast insertion/deletion at both ends. Like vector but with efficient operations on both sides.

```cpp
#include <deque>
using namespace std;

// Declaration
deque<int> dq;

// Adding elements
dq.push_back(10);          // Add to end
dq.push_front(5);          // Add to front: {5, 10}

// Removing elements
dq.pop_back();             // Remove from end: {5}
dq.pop_front();            // Remove from front: {}

// Accessing
dq.push_back(20);
dq.push_back(30);
cout << dq.front() << "\n"; // 20
cout << dq.back() << "\n";  // 30
cout << dq[0] << "\n";      // 20 - supports random access

// Iteration
for (int val : dq) {
    cout << val << " ";
}

// Size operations
cout << dq.size() << "\n";
cout << dq.empty() << "\n";

// Clearing
dq.clear();
```

### Deque vs Vector
- **Deque**: Better for push_front/pop_front operations
- **Vector**: Better for push_back/pop_back and random access

---

## 1.3 LIST - Doubly Linked List

### What is List?
Efficient insertion/deletion anywhere. No random access.

```cpp
#include <list>
using namespace std;

// Declaration
list<int> lst;

// Adding elements
lst.push_back(10);         // Add to end
lst.push_front(5);         // Add to front: {5, 10}

// Insert at position
auto it = lst.begin();
++it;
lst.insert(it, 7);         // Insert 7 at position 1: {5, 7, 10}

// Removing elements
lst.pop_back();            // Remove from end
lst.pop_front();           // Remove from front
lst.erase(it);             // Remove at iterator
lst.remove(5);             // Remove all elements with value 5
lst.clear();               // Clear all

// Accessing
cout << lst.front() << "\n"; // First element
cout << lst.back() << "\n";  // Last element
// NO random access: lst[0] - NOT AVAILABLE

// Iteration
for (int val : lst) {
    cout << val << " ";
}

// Reverse iteration
for (auto it = lst.rbegin(); it != lst.rend(); ++it) {
    cout << *it << " ";
}

// Size
cout << lst.size() << "\n";

// Useful operations
lst.reverse();             // Reverse the list
lst.sort();                // Sort the list
lst.unique();              // Remove consecutive duplicates
lst.sort(greater<int>()); // Sort in descending order
```

### List-Specific Operations

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

---

## 1.4 MAP - Sorted Key-Value Pairs

### What is Map?
Stores key-value pairs, sorted by key. Logarithmic operations.

```cpp
#include <map>
using namespace std;

// Declaration
map<string, int> ages;

// Insertion
ages["Alice"] = 30;
ages["Bob"] = 25;
ages["Carol"] = 28;
ages.insert({{"David", 32}});
ages.insert({"Eve", 27});

// Accessing
cout << ages["Alice"] << "\n";  // 30
cout << ages.at("Bob") << "\n"; // 25 - with bounds checking

// Check existence
if (ages.find("Alice") != ages.end()) {
    cout << "Alice found\n";
}

// Safe access with count
if (ages.count("Bob")) {
    cout << "Bob exists\n";
}

// Size
cout << ages.size() << "\n";

// Iteration
for (auto& pair : ages) {
    cout << pair.first << ": " << pair.second << "\n";
}

// With auto (C++11)
for (const auto& [name, age] : ages) {  // Structured binding (C++17)
    cout << name << ": " << age << "\n";
}

// Reverse iteration
for (auto it = ages.rbegin(); it != ages.rend(); ++it) {
    cout << it->first << ": " << it->second << "\n";
}
```

### Map Operations

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

### Map with Custom Comparator

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

---

## 1.5 SET - Sorted Unique Elements

### What is Set?
Stores unique, sorted elements. Like map but key only (no value).

```cpp
#include <set>
using namespace std;

// Declaration
set<int> nums;

// Insertion
nums.insert(30);
nums.insert(10);
nums.insert(20);
nums.insert(10);  // Duplicate - ignored
// {10, 20, 30}

// Accessing
auto it = nums.find(20);
if (it != nums.end()) {
    cout << "Found: " << *it << "\n";
}

// Count
cout << nums.count(20) << "\n";  // 1 or 0

// Iteration
for (int val : nums) {
    cout << val << " ";
}

// Erase
nums.erase(20);                // Erase by value
nums.erase(nums.begin());      // Erase by iterator
nums.erase(nums.lower_bound(15), nums.upper_bound(25));  // Range erase

// Size and empty
cout << nums.size() << "\n";
cout << nums.empty() << "\n";

// Clear
nums.clear();
```

### Set with Strings

```cpp
set<string> words;

words.insert("zebra");
words.insert("apple");
words.insert("mango");
words.insert("apple");  // Duplicate ignored

// Prints in alphabetical order
for (const string& word : words) {
    cout << word << "\n";
}
// Output: apple, mango, zebra
```

### Multiset - Allows Duplicates

```cpp
#include <set>

multiset<int> nums;

nums.insert(10);
nums.insert(20);
nums.insert(10);
nums.insert(20);
// {10, 10, 20, 20}

cout << nums.count(10) << "\n";  // 2

// All operations similar to set
for (int val : nums) {
    cout << val << " ";
}
```

---

## 1.6 MULTIMAP - Key-Value Pairs with Duplicate Keys

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

---

## 1.7 UNORDERED_MAP - Hash-Based Key-Value Pairs

### What is Unordered Map?
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

### Unordered Set

```cpp
#include <unordered_set>

unordered_set<int> nums = {30, 10, 20};

// Similar to set but unordered
if (nums.count(10)) {
    cout << "Found 10\n";
}

for (int val : nums) {
    cout << val << " ";  // Order is arbitrary
}
```

---

## 1.8 QUEUE - FIFO (First In, First Out)

### What is Queue?
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

### Queue Example - Task Processing

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

---

## 1.9 STACK - LIFO (Last In, First Out)

### What is Stack?
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

### Stack Example - Balanced Parentheses

```cpp
#include <stack>
#include <string>

bool isBalanced(string expr) {
    stack<char> st;
    
    for (char c : expr) {
        if (c == '(' || c == '[' || c == '{') {
            st.push(c);
        } else if (c == ')' || c == ']' || c == '}') {
            if (st.empty()) return false;
            
            char top = st.top();
            if ((c == ')' && top == '(') ||
                (c == ']' && top == '[') ||
                (c == '}' && top == '{')) {
                st.pop();
            } else {
                return false;
            }
        }
    }
    
    return st.empty();
}
```

---

## 1.10 PRIORITY_QUEUE - Heap-Based Container

### What is Priority Queue?
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

### Priority Queue with Custom Comparator

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

---

## ITERATORS - DEEP DIVE

## Iterator Categories

```
                    
                      Iterator           
                    
                      Single pass       
                      Basic operations  
                    
                               
        
                                                    
                      
     Input               Output            Forward  
                      
     Read              Write               Read+Write
     ++, *             ++, =               All ops  
                      
                                                     
                        
                        
                    
                      Bidirectional  
                    
                     ++, --, *, =    
                    
                             
                    
                     Random Access   
                    
                     All ops + []    
                    
```

### Iterator Types

```cpp
#include <vector>
#include <list>
#include <map>

vector<int> vec;         // Random access
list<int> lst;           // Bidirectional
map<int, int> mp;        // Bidirectional
deque<int> dq;           // Random access
set<int> st;             // Bidirectional

// Declaring iterators
vector<int>::iterator it1;                 // Random access
list<int>::iterator it2;                   // Bidirectional
map<int, int>::iterator it3;               // Bidirectional
const vector<int>::iterator it4;           // Const iterator

// Auto (C++11)
auto it = vec.begin();                     // Type deduced
```

### Iterator Operations

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

### Reverse Iterators

```cpp
vector<int> v = {10, 20, 30, 40, 50};

// Reverse iteration
for (auto it = v.rbegin(); it != v.rend(); ++it) {
    cout << *it << " ";  // 50 40 30 20 10
}

// Reverse iteration with auto
for (auto val : v) {
    cout << val << " ";  // 10 20 30 40 50
}

// Reverse iteration (explicit)
for (int i = v.size() - 1; i >= 0; --i) {
    cout << v[i] << " ";  // 50 40 30 20 10
}
```

### Const Iterators

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
```

### Advanced Iterator Concepts

#### 1. Iterator Traits (std::iterator_traits)
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

#### 2. Writing a Custom Iterator
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

#### 3. Stream Iterators
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

#### 4. Insert Iterators
Special output iterators that grow the container.

*   `std::back_inserter(c)`: Calls `c.push_back(val)`. (Vector, List, Deque)
*   `std::front_inserter(c)`: Calls `c.push_front(val)`. (List, Deque)
*   `std::inserter(c, it)`: Calls `c.insert(it, val)`. (Map, Set, List, Vector)

```cpp
std::vector<int> v;
std::fill_n(std::back_inserter(v), 5, 42); // v becomes {42, 42, 42, 42, 42}
```

---

## ALGORITHMS - COMPLETE REFERENCE
# C++ STL Advanced - Extended Reference & Algorithms Library

## COMPREHENSIVE STL ALGORITHMS REFERENCE

### All Algorithms by Category (60+ Algorithms)

---

## NON-MODIFYING SEQUENCE ALGORITHMS

### 1. find Family
```cpp
#include <algorithm>

auto it = find(first, last, value);              // Find element
auto it = find_if(first, last, predicate);       // Find matching condition
auto it = find_if_not(first, last, predicate);   // Find non-matching (C++11)
```

### 2. count Family
```cpp
int n = count(first, last, value);               // Count occurrences
int n = count_if(first, last, predicate);        // Count matching
```

### 3. mismatch
```cpp
auto [it1, it2] = mismatch(first1, last1, first2);  // Find first difference
auto [it1, it2] = mismatch(first1, last1, first2, comp); // With comparator
```

### 4. equal
```cpp
bool eq = equal(first1, last1, first2);          // Compare ranges
bool eq = equal(first1, last1, first2, comp);    // With comparator
```

### 5. search & adjacent_find
```cpp
auto it = search(first, last, s_first, s_last);  // Find subsequence
auto it = search_n(first, last, count, value);   // Find N equal elements
auto it = adjacent_find(first, last);            // Find adjacent equal elements
auto it = adjacent_find(first, last, comp);      // With comparator
```

### 6. Logical Operations
```cpp
bool b = all_of(first, last, predicate);         // All match
bool b = any_of(first, last, predicate);         // Any matches
bool b = none_of(first, last, predicate);        // None match
```

### 7. Min/Max
```cpp
auto it = min_element(first, last);              // Find minimum
auto it = max_element(first, last);              // Find maximum
auto [minIt, maxIt] = minmax_element(first, last);  // Both (C++11)

auto it = min_element(first, last, comp);        // With comparator
auto it = max_element(first, last, comp);
auto [minIt, maxIt] = minmax_element(first, last, comp);
```

---

## MODIFYING SEQUENCE ALGORITHMS

### 1. copy Family
```cpp
copy(first, last, d_first);                      // Copy range
copy_n(first, count, d_first);                   // Copy N elements
copy_if(first, last, d_first, predicate);        // Conditional copy
copy_backward(first, last, d_last);              // Copy backwards
```

### 2. move (C++11)
```cpp
move(first, last, d_first);                      // Move range
move_backward(first, last, d_last);              // Move backwards
```

### 3. transform
```cpp
transform(first, last, d_first, op);             // Apply function
transform(first1, last1, first2, d_first, op);   // Apply to two ranges
```

### 4. fill & generate
```cpp
fill(first, last, value);                        // Fill with value
fill_n(first, count, value);                     // Fill N elements
generate(first, last, gen);                      // Generate values
generate_n(first, count, gen);                   // Generate N values
```

### 5. replace
```cpp
replace(first, last, old_value, new_value);      // Replace values
replace_if(first, last, predicate, new_value);   // Conditional replace
replace_copy(first, last, d_first, old, new);    // Copy with replace
replace_copy_if(first, last, d_first, pred, new);// Conditional copy-replace
```

### 6. swap & reverse
```cpp
swap(a, b);                                       // Swap two values
iter_swap(it1, it2);                             // Swap via iterators
reverse(first, last);                            // Reverse range
reverse_copy(first, last, d_first);              // Copy reversed
```

### 7. rotate
```cpp
rotate(first, middle, last);                     // Rotate range
rotate_copy(first, middle, last, d_first);       // Copy rotated
```

### 8. unique
```cpp
auto it = unique(first, last);                   // Remove consecutive duplicates
auto it = unique(first, last, comp);             // With comparator
auto it = unique_copy(first, last, d_first);     // Copy unique
auto it = unique_copy(first, last, d_first, comp);
```

### 9. shuffle & random
```cpp
shuffle(first, last, rng);                       // Random shuffle
random_shuffle(first, last);                     // Legacy shuffle
random_shuffle(first, last, randFunc);           // With random function
```

### 10. remove
```cpp
auto it = remove(first, last, value);            // Remove all matching
auto it = remove_if(first, last, predicate);     // Conditional remove
auto it = remove_copy(first, last, d_first, val);// Copy without matching
auto it = remove_copy_if(first, last, d_first, pred);
```

---

## SORTING & PARTITIONING ALGORITHMS

### Sorting
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

### Partitioning
```cpp
auto it = partition(first, last, predicate);     // Partition
auto it = stable_partition(first, last, pred);   // Stable partition
auto it = partition_copy(first, last, d_true, d_false, pred);  // Copy partitions

bool b = is_partitioned(first, last, predicate); // Check if partitioned
auto it = partition_point(first, last, predicate); // Find partition point
```

### Binary Search (requires sorted range)
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

---

## NUMERIC ALGORITHMS

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

---

## SET OPERATIONS (require sorted ranges)

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

---

## HEAP OPERATIONS

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

---

## PERMUTATION ALGORITHMS

```cpp
bool b = next_permutation(first, last);          // Next lexicographic permutation
bool b = next_permutation(first, last, comp);
bool b = prev_permutation(first, last);          // Previous permutation
bool b = prev_permutation(first, last, comp);

bool b = is_permutation(first1, last1, first2);  // Check if permutation
bool b = is_permutation(first1, last1, first2, comp);
```

---

## COMPLETE STL ALGORITHMS QUICK REFERENCE

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

---

## CONTAINERS DETAILED OPERATIONS

### Vector Operations
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

### Map Operations
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

### Set Operations
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

---

## ALGORITHM COMPLEXITY CHEAT SHEET

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

---

## CONTAINER COMPLEXITY COMPARISON

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

---

## ITERATOR COMPARISON TABLE

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

---

## PRACTICAL STL PATTERNS

### Pattern 1: Find & Remove
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

### Pattern 2: Filter & Copy
```cpp
vector<int> v = {1, 2, 3, 4, 5};
vector<int> even;

copy_if(v.begin(), v.end(), back_inserter(even),
    [](int x) { return x % 2 == 0; });
// even: {2, 4}
```

### Pattern 3: Transform & Collect
```cpp
vector<int> v = {1, 2, 3};
vector<int> squared;

transform(v.begin(), v.end(), back_inserter(squared),
    [](int x) { return x * x; });
// squared: {1, 4, 9}
```

### Pattern 4: Partition & Process
```cpp
vector<int> v = {1, 2, 3, 4, 5, 6};

auto it = partition(v.begin(), v.end(),
    [](int x) { return x % 2 == 0; });

// Process even numbers
for (auto i = v.begin(); i != it; ++i) {
    cout << *i << " ";  // 2 4 6
}
```

### Pattern 5: Sort & Deduplicate
```cpp
vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

sort(v.begin(), v.end());
auto it = unique(v.begin(), v.end());
v.erase(it, v.end());
// v: {1, 2, 3, 4, 5, 6, 9}
```

### Pattern 6: Group & Count
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

### Pattern 7: Custom Sorting
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

### Pattern 8: Merge Ranges
```cpp
vector<int> v1 = {1, 3, 5};
vector<int> v2 = {2, 4, 6};
vector<int> merged;

merge(v1.begin(), v1.end(), v2.begin(), v2.end(),
    back_inserter(merged));
// merged: {1, 2, 3, 4, 5, 6}
```

---

## STL WITH LAMBDAS (C++11 and later)

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

---

## STRINGS - MASTER GUIDE

## 4.1 String Basics

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

### Size and Capacity

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

---

## 4.2 Accessing Characters

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

---

## 4.3 String Concatenation

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

---

## 4.4 String Searching

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

---

## 4.5 String Manipulation

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

---

## 4.6 String Iteration

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

---

## 4.7 String Conversion

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

---

## 4.8 String Comparison

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

---

## 4.9 String from Stringstream

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

---

## FILE I/O - COMPLETE COVERAGE

## 5.1 File I/O Basics

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

## 5.2 File Operations

### Open Modes

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

---

## 5.3 Writing to Files

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

---

## 5.4 Reading from Files

### Line by Line

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

### Word by Word

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

### Character by Character

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

### Specific Format

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

---

## 5.5 Binary File I/O

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

---

## 5.6 File Position

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

---

## 5.7 Error Handling

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

---

## 5.8 Complete File I/O Example

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

---

## FUNCTION OBJECTS & COMPARATORS

## 6.1 Function Objects (Functors)

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

## 6.2 Standard Comparators

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

---

## STL BEST PRACTICES

## 7.1 Container Selection

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

## 7.2 Algorithm Selection

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

## 7.3 Memory Management

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

## 7.4 Performance Tips

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

---

### Containers Intro (C++98)

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

---
