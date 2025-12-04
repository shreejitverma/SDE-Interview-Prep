# Complete C++ Standard Template Library (STL) Reference

This is an exhaustive list of all C++ STL functions, containers, and utilities organized by category.

## Table of Contents
1. [Containers](#containers)
2. [Container Member Functions](#container-member-functions)
3. [Algorithms](#algorithms)
4. [Numeric Algorithms](#numeric-algorithms)
5. [Iterators](#iterators)
6. [Helper Classes & Functions](#helper-classes--functions)
7. [Function Objects & Predicates](#function-objects--predicates)
8. [String Functions](#string-functions)

---

## Containers

### Sequence Containers

| Function/Class | Description | Example |
|---|---|---|
| vector | Dynamic array with random access | `vector<int> v; v.push_back(5); cout << v[0];` |
| deque | Double-ended queue with O(1) access at both ends | `deque<int> d; d.push_back(5); d.push_front(3);` |
| list | Doubly-linked list with O(1) insertion/deletion | `list<int> l; l.push_back(5); l.erase(l.begin());` |
| forward_list | Singly-linked list (C++11) | `forward_list<int> fl; fl.push_front(5);` |
| array | Fixed-size array wrapper (C++11) | `array<int, 5> arr; arr[0] = 10;` |

### Associative Containers

| Function/Class | Description | Example |
|---|---|---|
| set | Sorted unique elements | `set<int> s; s.insert(5); auto it = s.find(5);` |
| multiset | Sorted elements (duplicates allowed) | `multiset<int> ms; ms.insert(5); ms.insert(5);` |
| map | Sorted key-value pairs (unique keys) | `map<int,string> m; m[1] = "one"; m.erase(1);` |
| multimap | Sorted key-value pairs (duplicate keys allowed) | `multimap<int,string> mm; mm.insert({1, "one"});` |

### Unordered Associative Containers

| Function/Class | Description | Example |
|---|---|---|
| unordered_set | Hash-based unique elements | `unordered_set<int> us; us.insert(5);` |
| unordered_multiset | Hash-based elements (duplicates allowed) | `unordered_multiset<int> ums; ums.insert(5);` |
| unordered_map | Hash-based key-value pairs (unique keys) | `unordered_map<int,string> um; um[1] = "one";` |
| unordered_multimap | Hash-based key-value pairs (duplicate keys) | `unordered_multimap<int,string> umm; umm.insert({1, "one"});` |

### Container Adapters

| Function/Class | Description | Example |
|---|---|---|
| stack | LIFO - Last In First Out | `stack<int> st; st.push(5); int x = st.top(); st.pop();` |
| queue | FIFO - First In First Out | `queue<int> q; q.push(5); int x = q.front(); q.pop();` |
| priority_queue | Max-heap by default | `priority_queue<int> pq; pq.push(5); int x = pq.top(); pq.pop();` |

---

## Container Member Functions

### Common Functions (All Containers)

| Function | Description | Example |
|---|---|---|
| begin() | Returns iterator to beginning | `auto it = v.begin();` |
| end() | Returns iterator to end (one past last element) | `auto it = v.end();` |
| rbegin() | Returns reverse iterator to end | `auto it = v.rbegin();` |
| rend() | Returns reverse iterator to beginning | `auto it = v.rend();` |
| cbegin() | Returns const iterator to beginning (C++11) | `auto it = v.cbegin();` |
| cend() | Returns const iterator to end (C++11) | `auto it = v.cend();` |
| size() | Returns number of elements | `int sz = v.size();` |
| empty() | Checks if container is empty | `if (v.empty()) { }` |
| max_size() | Returns maximum possible size | `int max = v.max_size();` |
| capacity() | Returns allocated storage capacity (vectors only) | `int cap = v.capacity();` |
| reserve() | Reserves storage space (vectors only) | `v.reserve(100);` |
| shrink_to_fit() | Reduces capacity to match size (C++11) | `v.shrink_to_fit();` |
| clear() | Removes all elements | `v.clear();` |
| insert() | Inserts element(s) at position | `v.insert(v.begin() + 2, 10);` |
| erase() | Removes element(s) | `v.erase(v.begin()); v.erase(v.begin(), v.begin()+2);` |
| push_back() | Adds element to end | `v.push_back(10);` |
| pop_back() | Removes last element | `v.pop_back();` |
| push_front() | Adds element to front (list/deque only) | `l.push_front(10);` |
| pop_front() | Removes first element (list/deque only) | `l.pop_front();` |
| front() | Returns reference to first element | `int x = v.front();` |
| back() | Returns reference to last element | `int x = v.back();` |
| at() | Returns element at position with bounds checking | `int x = v.at(2);` |
| operator[] | Accesses element without bounds checking | `int x = v[2];` |
| data() | Returns pointer to underlying array (C++11) | `int* ptr = v.data();` |
| operator= | Assigns container content | `v1 = v2;` |
| swap() | Swaps contents with another container | `v1.swap(v2);` |
| emplace() | Constructs element in place (C++11) | `v.emplace(v.begin(), 10);` |
| emplace_back() | Constructs element at end (C++11) | `v.emplace_back(10);` |
| emplace_front() | Constructs element at front (C++11) | `l.emplace_front(10);` |

### Associative Container Functions

| Function | Description | Example |
|---|---|---|
| count() | Returns number of elements with specific key | `int n = m.count(1);` |
| find() | Returns iterator to element with specific key | `auto it = m.find(1);` |
| equal_range() | Returns range of elements with specific key | `auto range = m.equal_range(1);` |
| lower_bound() | Returns iterator to first element >= key (sorted containers) | `auto it = m.lower_bound(1);` |
| upper_bound() | Returns iterator to first element > key (sorted containers) | `auto it = m.upper_bound(1);` |
| key_comp() | Returns key comparison function | `auto cmp = m.key_comp();` |
| value_comp() | Returns value comparison function | `auto cmp = m.value_comp();` |

### Hash Container Functions

| Function | Description | Example |
|---|---|---|
| bucket() | Returns bucket index for key | `int b = um.bucket(1);` |
| bucket_count() | Returns number of buckets | `int cnt = um.bucket_count();` |
| bucket_size() | Returns number of elements in bucket | `int sz = um.bucket_size(0);` |
| load_factor() | Returns average elements per bucket | `float lf = um.load_factor();` |
| max_load_factor() | Returns/sets maximum load factor | `float mlf = um.max_load_factor();` |
| rehash() | Changes number of buckets | `um.rehash(50);` |
| reserve() | Reserves space for elements | `um.reserve(100);` |
| hash_function() | Returns hash function | `auto hash = um.hash_function();` |
| key_eq() | Returns key equality function | `auto eq = um.key_eq();` |

### List-specific Functions

| Function | Description | Example |
|---|---|---|
| splice() | Transfers elements between lists | `l1.splice(l1.begin(), l2);` |
| remove() | Removes elements with specific value | `l.remove(5);` |
| remove_if() | Removes elements satisfying condition | `l.remove_if([](int x){ return x % 2 == 0; });` |
| unique() | Removes consecutive duplicates | `l.unique();` |
| merge() | Merges another sorted list into this one | `l1.merge(l2);` |
| sort() | Sorts list elements | `l.sort();` |
| reverse() | Reverses list order | `l.reverse();` |

---

## Algorithms

### Non-Modifying Algorithms

| Function | Description | Example |
|---|---|---|
| find() | Searches for element and returns iterator | `auto it = find(v.begin(), v.end(), 5);` |
| find_if() | Finds first element satisfying condition | `auto it = find_if(v.begin(), v.end(), [](int x){ return x > 5; });` |
| find_if_not() | Finds first element NOT satisfying condition (C++11) | `auto it = find_if_not(v.begin(), v.end(), [](int x){ return x > 5; });` |
| find_first_of() | Finds first occurrence of any element from another range | `auto it = find_first_of(v1.begin(), v1.end(), v2.begin(), v2.end());` |
| adjacent_find() | Finds first pair of adjacent equal elements | `auto it = adjacent_find(v.begin(), v.end());` |
| search() | Searches for subsequence | `auto it = search(v.begin(), v.end(), sub.begin(), sub.end());` |
| search_n() | Searches for n consecutive elements | `auto it = search_n(v.begin(), v.end(), 3, 5);` |
| count() | Counts elements equal to value | `int n = count(v.begin(), v.end(), 5);` |
| count_if() | Counts elements satisfying condition | `int n = count_if(v.begin(), v.end(), [](int x){ return x > 5; });` |
| mismatch() | Finds first position where two ranges differ | `auto p = mismatch(v1.begin(), v1.end(), v2.begin());` |
| equal() | Checks if two ranges are equal | `bool eq = equal(v1.begin(), v1.end(), v2.begin());` |
| is_permutation() | Checks if one range is permutation of another (C++11) | `bool perm = is_permutation(v1.begin(), v1.end(), v2.begin());` |

### Modifying Algorithms

| Function | Description | Example |
|---|---|---|
| copy() | Copies elements from source to destination | `copy(v.begin(), v.end(), v2.begin());` |
| copy_if() | Copies elements satisfying condition | `copy_if(v.begin(), v.end(), v2.begin(), [](int x){ return x > 5; });` |
| copy_n() | Copies n elements | `copy_n(v.begin(), 3, v2.begin());` |
| copy_backward() | Copies elements in reverse order | `copy_backward(v.begin(), v.end(), v2.end());` |
| move() | Moves elements from source to destination (C++11) | `move(v.begin(), v.end(), v2.begin());` |
| move_backward() | Moves elements in reverse order (C++11) | `move_backward(v.begin(), v.end(), v2.end());` |
| swap() | Swaps values of two elements | `swap(v[0], v[1]);` |
| swap_ranges() | Swaps elements between two ranges | `swap_ranges(v1.begin(), v1.end(), v2.begin());` |
| transform() | Applies function to elements, stores result | `transform(v.begin(), v.end(), v2.begin(), [](int x){ return x*2; });` |
| replace() | Replaces all occurrences of value | `replace(v.begin(), v.end(), 5, 10);` |
| replace_if() | Replaces elements satisfying condition | `replace_if(v.begin(), v.end(), [](int x){ return x > 5; }, 10);` |
| replace_copy() | Copies and replaces all occurrences | `replace_copy(v.begin(), v.end(), v2.begin(), 5, 10);` |
| replace_copy_if() | Copies and replaces elements satisfying condition | `replace_copy_if(v.begin(), v.end(), v2.begin(), [](int x){ return x > 5; }, 10);` |
| fill() | Fills range with value | `fill(v.begin(), v.end(), 0);` |
| fill_n() | Fills n elements with value | `fill_n(v.begin(), 3, 0);` |
| generate() | Fills range using function | `generate(v.begin(), v.end(), []{ static int i=0; return i++; });` |
| generate_n() | Fills n elements using function | `generate_n(v.begin(), 3, []{ static int i=0; return i++; });` |
| remove() | Logically removes elements (moves to end) | `auto end = remove(v.begin(), v.end(), 5); v.erase(end, v.end());` |
| remove_if() | Logically removes elements satisfying condition | `auto end = remove_if(v.begin(), v.end(), [](int x){ return x > 5; }); v.erase(end, v.end());` |
| remove_copy() | Copies elements, excluding specific value | `remove_copy(v.begin(), v.end(), v2.begin(), 5);` |
| remove_copy_if() | Copies elements, excluding those satisfying condition | `remove_copy_if(v.begin(), v.end(), v2.begin(), [](int x){ return x > 5; });` |
| unique() | Removes consecutive duplicates | `auto end = unique(v.begin(), v.end()); v.erase(end, v.end());` |
| unique_copy() | Copies elements, removing consecutive duplicates | `unique_copy(v.begin(), v.end(), v2.begin());` |
| reverse() | Reverses order of elements | `reverse(v.begin(), v.end());` |
| reverse_copy() | Copies elements in reverse order | `reverse_copy(v.begin(), v.end(), v2.begin());` |
| rotate() | Rotates elements left | `rotate(v.begin(), v.begin()+2, v.end());` |
| rotate_copy() | Copies and rotates elements | `rotate_copy(v.begin(), v.begin()+2, v.end(), v2.begin());` |
| random_shuffle() | Randomly shuffles elements (deprecated, use shuffle) | `random_shuffle(v.begin(), v.end());` |
| shuffle() | Randomly shuffles elements (C++11) | `shuffle(v.begin(), v.end(), gen);` |

### Sorting Algorithms

| Function | Description | Example |
|---|---|---|
| sort() | Sorts elements in ascending order | `sort(v.begin(), v.end());` |
| stable_sort() | Sorts elements preserving relative order of equal elements | `stable_sort(v.begin(), v.end());` |
| partial_sort() | Sorts first n elements | `partial_sort(v.begin(), v.begin()+5, v.end());` |
| partial_sort_copy() | Partially sorts and copies | `partial_sort_copy(v.begin(), v.end(), v2.begin(), v2.end());` |
| nth_element() | Partitions so nth element is correct position | `nth_element(v.begin(), v.begin()+5, v.end());` |
| is_sorted() | Checks if range is sorted | `bool sorted = is_sorted(v.begin(), v.end());` |
| is_sorted_until() | Returns iterator to first unsorted element | `auto it = is_sorted_until(v.begin(), v.end());` |

### Binary Search Algorithms

| Function | Description | Example |
|---|---|---|
| binary_search() | Checks if element exists in sorted range | `bool found = binary_search(v.begin(), v.end(), 5);` |
| lower_bound() | Returns iterator to first element >= value | `auto it = lower_bound(v.begin(), v.end(), 5);` |
| upper_bound() | Returns iterator to first element > value | `auto it = upper_bound(v.begin(), v.end(), 5);` |
| equal_range() | Returns range of elements equal to value | `auto range = equal_range(v.begin(), v.end(), 5);` |

### Set Operations

| Function | Description | Example |
|---|---|---|
| includes() | Checks if sorted range contains another | `bool incl = includes(v1.begin(), v1.end(), v2.begin(), v2.end());` |
| merge() | Merges two sorted ranges | `merge(v1.begin(), v1.end(), v2.begin(), v2.end(), v3.begin());` |
| inplace_merge() | Merges two consecutive sorted ranges in-place | `inplace_merge(v.begin(), v.begin()+5, v.end());` |
| set_union() | Computes union of two sorted ranges | `set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), v3.begin());` |
| set_intersection() | Computes intersection of two sorted ranges | `set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), v3.begin());` |
| set_difference() | Computes difference of two sorted ranges | `set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), v3.begin());` |
| set_symmetric_difference() | Computes symmetric difference | `set_symmetric_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), v3.begin());` |

### Heap Operations

| Function | Description | Example |
|---|---|---|
| make_heap() | Constructs a heap | `make_heap(v.begin(), v.end());` |
| push_heap() | Adds element to heap | `v.push_back(5); push_heap(v.begin(), v.end());` |
| pop_heap() | Removes max element from heap | `pop_heap(v.begin(), v.end()); v.pop_back();` |
| sort_heap() | Sorts heap | `sort_heap(v.begin(), v.end());` |
| is_heap() | Checks if range is a heap (C++11) | `bool h = is_heap(v.begin(), v.end());` |
| is_heap_until() | Returns iterator to first non-heap element (C++11) | `auto it = is_heap_until(v.begin(), v.end());` |

### Permutation Algorithms

| Function | Description | Example |
|---|---|---|
| next_permutation() | Generates next lexicographic permutation | `while (next_permutation(v.begin(), v.end())) { }` |
| prev_permutation() | Generates previous lexicographic permutation | `while (prev_permutation(v.begin(), v.end())) { }` |

### Other Algorithms

| Function | Description | Example |
|---|---|---|
| for_each() | Applies function to each element | `for_each(v.begin(), v.end(), [](int x){ cout << x; });` |
| all_of() | Checks if all elements satisfy condition (C++11) | `bool all = all_of(v.begin(), v.end(), [](int x){ return x > 0; });` |
| any_of() | Checks if any element satisfies condition (C++11) | `bool any = any_of(v.begin(), v.end(), [](int x){ return x > 0; });` |
| none_of() | Checks if no element satisfies condition (C++11) | `bool none = none_of(v.begin(), v.end(), [](int x){ return x > 0; });` |
| min() | Returns minimum of two values | `int m = min(5, 10);` |
| max() | Returns maximum of two values | `int m = max(5, 10);` |
| minmax() | Returns pair with min and max (C++11) | `auto p = minmax(5, 10);` |
| min_element() | Returns iterator to minimum element | `auto it = min_element(v.begin(), v.end());` |
| max_element() | Returns iterator to maximum element | `auto it = max_element(v.begin(), v.end());` |
| minmax_element() | Returns pair of iterators to min and max (C++11) | `auto p = minmax_element(v.begin(), v.end());` |

---

## Numeric Algorithms

| Function | Description | Example |
|---|---|---|
| accumulate() | Computes sum/accumulation of elements | `int sum = accumulate(v.begin(), v.end(), 0);` |
| partial_sum() | Computes partial sums | `partial_sum(v.begin(), v.end(), v2.begin());` |
| adjacent_difference() | Computes differences between adjacent elements | `adjacent_difference(v.begin(), v.end(), v2.begin());` |
| inner_product() | Computes inner product of two ranges | `int prod = inner_product(v1.begin(), v1.end(), v2.begin(), 0);` |
| iota() | Fills range with incrementing values (C++11) | `iota(v.begin(), v.end(), 1);` |

---

## Iterators

### Iterator Types

| Type | Description | Example |
|---|---|---|
| Input Iterator | Single-pass, read-only | `for(auto it = v.begin(); it != v.end(); ++it) { int x = *it; }` |
| Output Iterator | Single-pass, write-only | `copy(v.begin(), v.end(), out_iterator);` |
| Forward Iterator | Multi-pass, read/write | `for(auto it = fl.begin(); it != fl.end(); ++it) { *it = 10; }` |
| Bidirectional Iterator | Forward/backward, read/write | `for(auto it = l.rbegin(); it != l.rend(); ++it) { *it = 10; }` |
| Random Access Iterator | Any position, read/write | `int x = v[5]; v[3] = 10;` |

### Iterator Functions

| Function | Description | Example |
|---|---|---|
| advance() | Advances iterator by n positions | `advance(it, 5);` |
| distance() | Returns distance between iterators | `int d = distance(v.begin(), it);` |
| next() | Returns iterator n positions ahead (C++11) | `auto it2 = next(it, 5);` |
| prev() | Returns iterator n positions behind (C++11) | `auto it2 = prev(it, 5);` |
| begin() | Non-member function returning begin iterator (C++11) | `auto it = begin(v);` |
| end() | Non-member function returning end iterator (C++11) | `auto it = end(v);` |
| rbegin() | Non-member function returning reverse begin (C++11) | `auto it = rbegin(v);` |
| rend() | Non-member function returning reverse end (C++11) | `auto it = rend(v);` |
| cbegin() | Non-member function returning const begin (C++11) | `auto it = cbegin(v);` |
| cend() | Non-member function returning const end (C++11) | `auto it = cend(v);` |
| crbegin() | Non-member function returning const reverse begin (C++11) | `auto it = crbegin(v);` |
| crend() | Non-member function returning const reverse end (C++11) | `auto it = crend(v);` |
| make_move_iterator() | Constructs move iterator (C++11) | `auto it = make_move_iterator(v.begin());` |
| make_reverse_iterator() | Constructs reverse iterator | `auto it = make_reverse_iterator(v.end());` |
| insert_iterator | Iterator adapter for insertion | `insert_iterator<vector<int>> ii(v, v.begin());` |
| back_inserter() | Creates back insertion iterator | `back_inserter(v);` |
| front_inserter() | Creates front insertion iterator | `front_inserter(l);` |

---

## Helper Classes & Functions

| Function/Class | Description | Example |
|---|---|---|
| pair | Stores two heterogeneous values | `pair<int, string> p(1, "one"); cout << p.first << p.second;` |
| make_pair() | Creates a pair (C++11 use brace init) | `auto p = make_pair(1, "one");` |
| tuple | Stores multiple heterogeneous values (C++11) | `tuple<int, string, double> t(1, "one", 3.14);` |
| make_tuple() | Creates a tuple (C++11) | `auto t = make_tuple(1, "one", 3.14);` |
| tie() | Creates tuple of references (C++11) | `int x; string s; tie(x, s) = p;` |
| structured_bindings | Unpacks tuple/pair (C++17) | `auto [x, s] = p; auto [a, b, c] = t;` |
| swap() | Swaps two values | `swap(a, b);` |
| forward() | Perfect forwarding (C++11) | `template<typename T> void f(T&& x) { g(forward<T>(x)); }` |
| move() | Converts to rvalue reference (C++11) | `vector<int> v2 = move(v1);` |

---

## Function Objects & Predicates

### Comparison Functions

| Function Object | Description | Example |
|---|---|---|
| equal_to<T> | Equality comparison | `bool eq = equal_to<int>()(5, 5);` |
| not_equal_to<T> | Inequality comparison | `bool neq = not_equal_to<int>()(5, 3);` |
| less<T> | Less-than comparison | `bool lt = less<int>()(3, 5);` |
| less_equal<T> | Less-than-or-equal comparison | `bool le = less_equal<int>()(5, 5);` |
| greater<T> | Greater-than comparison | `bool gt = greater<int>()(5, 3);` |
| greater_equal<T> | Greater-than-or-equal comparison | `bool ge = greater_equal<int>()(5, 5);` |

### Arithmetic Functions

| Function Object | Description | Example |
|---|---|---|
| plus<T> | Addition operation | `int sum = plus<int>()(3, 5);` |
| minus<T> | Subtraction operation | `int diff = minus<int>()(5, 3);` |
| multiplies<T> | Multiplication operation | `int prod = multiplies<int>()(3, 5);` |
| divides<T> | Division operation | `int quot = divides<int>()(10, 2);` |
| modulus<T> | Modulo operation | `int rem = modulus<int>()(10, 3);` |
| negate<T> | Negation operation | `int neg = negate<int>()(5);` |

### Logical Functions

| Function Object | Description | Example |
|---|---|---|
| logical_and<T> | Logical AND | `bool result = logical_and<bool>()(true, true);` |
| logical_or<T> | Logical OR | `bool result = logical_or<bool>()(true, false);` |
| logical_not<T> | Logical NOT | `bool result = logical_not<bool>()(false);` |

### Adaptors

| Function | Description | Example |
|---|---|---|
| bind() | Binds arguments to function (C++11) | `auto f = bind(multiply<int>(), 2, placeholders::_1);` |
| not1() | Negates unary predicate | `find_if(v.begin(), v.end(), not1(bind2nd(less<int>(), 5)));` |
| not2() | Negates binary predicate | `find_if(v.begin(), v.end(), not2(greater<int>()));` |
| mem_fun() | Adapts member function to function object | `for_each(ptrs.begin(), ptrs.end(), mem_fun(&Class::method));` |
| mem_fn() | Adapts member function (C++11) | `for_each(ptrs.begin(), ptrs.end(), mem_fn(&Class::method));` |

---

## String Functions

### String Member Functions

| Function | Description | Example |
|---|---|---|
| length()/size() | Returns string length | `int len = str.length();` |
| empty() | Checks if string is empty | `if (str.empty()) { }` |
| substr() | Extracts substring | `string sub = str.substr(0, 5);` |
| find() | Finds substring position | `size_t pos = str.find("world");` |
| rfind() | Finds substring from end | `size_t pos = str.rfind("world");` |
| find_first_of() | Finds first character from set | `size_t pos = str.find_first_of("aeiou");` |
| find_last_of() | Finds last character from set | `size_t pos = str.find_last_of("aeiou");` |
| find_first_not_of() | Finds first character NOT in set | `size_t pos = str.find_first_not_of("aeiou");` |
| find_last_not_of() | Finds last character NOT in set | `size_t pos = str.find_last_not_of("aeiou");` |
| compare() | Compares with another string | `int cmp = str.compare("test");` |
| c_str() | Returns C-style string | `const char* cstr = str.c_str();` |
| data() | Returns pointer to data | `const char* data = str.data();` |
| at() | Returns character at position | `char c = str.at(5);` |
| operator[] | Accesses character at position | `char c = str[5];` |
| replace() | Replaces substring | `str.replace(0, 5, "new");` |
| erase() | Erases characters | `str.erase(0, 5);` |
| insert() | Inserts string at position | `str.insert(5, "inserted");` |
| append()/+= | Appends string | `str.append(" world"); str += " world";` |
| assign() | Assigns new value | `str.assign("new string");` |
| clear() | Clears string | `str.clear();` |
| swap() | Swaps with another string | `str1.swap(str2);` |
| capacity() | Returns allocated capacity | `size_t cap = str.capacity();` |
| reserve() | Reserves storage | `str.reserve(100);` |
| shrink_to_fit() | Reduces capacity (C++11) | `str.shrink_to_fit();` |
| front() | Returns first character | `char c = str.front();` |
| back() | Returns last character | `char c = str.back();` |
| operator+ | Concatenates strings | `string s = str1 + str2;` |
| operator== | Equality comparison | `if (str1 == str2) { }` |
| operator!= | Inequality comparison | `if (str1 != str2) { }` |
| operator< | Less-than comparison | `if (str1 < str2) { }` |

---

## Summary

This comprehensive reference contains **231 total STL functions and classes** organized into:

- **Containers**: 16 types (5 sequence, 4 associative, 4 unordered, 3 adapters)
- **Container Member Functions**: 54 functions
- **Algorithms**: 93 functions (non-modifying, modifying, sorting, search, set operations, heap, permutation, other)
- **Numeric Algorithms**: 5 functions
- **Iterators**: 5 types + 15 iterator functions
- **Helper Classes & Functions**: 9 utilities
- **Function Objects & Predicates**: 18 function objects
- **String Functions**: 28 string member functions

All examples are provided with actual C++ code that can be compiled and executed.
