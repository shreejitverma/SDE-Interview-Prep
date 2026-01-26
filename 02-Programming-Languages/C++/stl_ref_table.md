# Complete C++ Standard Template Library (STL) Reference - Table Format

## Summary
This document contains an exhaustive list of **231 C++ STL functions and classes** organized by category with descriptions and examples.

---

## CONTAINERS

### Sequence Containers (5 total)

| Name | Description | Key Operations | Example |
|------|-------------|-----------------|---------|
| vector | Dynamic resizable array | push_back, pop_back, operator[], size | `vector<int> v; v.push_back(5);` |
| deque | Double-ended queue | push_front, push_back, pop_front | `deque<int> d; d.push_front(3);` |
| list | Doubly-linked list | insert, erase, push_front, splice | `list<int> l; l.push_back(5);` |
| forward_list | Singly-linked list | push_front, insert_after (C++11) | `forward_list<int> fl;` |
| array | Fixed-size array wrapper | operator[], at (C++11) | `array<int, 5> arr;` |

### Associative Containers (4 total)

| Name | Description | Key Property | Example |
|------|-------------|--------------|---------|
| set | Sorted unique elements | Ordered, no duplicates | `set<int> s; s.insert(5);` |
| multiset | Sorted with duplicates | Ordered, duplicates allowed | `multiset<int> ms;` |
| map | Key-value pairs, unique keys | Ordered keys | `map<int,string> m; m[1]="one";` |
| multimap | Key-value pairs, duplicate keys | Ordered keys | `multimap<int,string> mm;` |

### Unordered Associative Containers (4 total)

| Name | Description | Key Property | Example |
|------|-------------|--------------|---------|
| unordered_set | Hash-based unique elements | Fast lookup O(1) avg | `unordered_set<int> us;` |
| unordered_multiset | Hash-based with duplicates | Fast lookup | `unordered_multiset<int> ums;` |
| unordered_map | Hash-based key-value pairs | Fast key lookup | `unordered_map<int,string> um;` |
| unordered_multimap | Hash-based key-value pairs | Duplicate keys | `unordered_multimap<int,string> umm;` |

### Container Adapters (3 total)

| Name | Pattern | Description | Example |
|------|---------|-------------|---------|
| stack | LIFO | Last In, First Out | `stack<int> st; st.push(5); st.pop();` |
| queue | FIFO | First In, First Out | `queue<int> q; q.push(5); q.pop();` |
| priority_queue | Heap | Max-heap by default | `priority_queue<int> pq;` |

---

## CONTAINER MEMBER FUNCTIONS

### Universal Container Functions (27 functions)

#### Iterator Functions
| Function | Returns | Purpose |
|----------|---------|---------|
| begin() | iterator | Points to first element |
| end() | iterator | Points past last element |
| rbegin() | reverse_iterator | Reverse begin |
| rend() | reverse_iterator | Reverse end |
| cbegin() | const_iterator | Const begin (C++11) |
| cend() | const_iterator | Const end (C++11) |

#### Capacity Functions
| Function | Returns | Purpose |
|----------|---------|---------|
| size() | size_t | Number of elements |
| empty() | bool | Is container empty |
| max_size() | size_t | Maximum possible size |
| capacity() | size_t | Allocated capacity (vector only) |
| reserve(n) | void | Reserve capacity (vector/string) |
| shrink_to_fit() | void | Reduce capacity (C++11) |

#### Element Access Functions
| Function | Returns | Purpose |
|----------|---------|---------|
| front() | reference | First element |
| back() | reference | Last element |
| at(n) | reference | Element with bounds check |
| operator[] | reference | Element without bounds check |
| data() | pointer | Pointer to array (C++11) |

#### Modification Functions
| Function | Purpose | Parameters |
|----------|---------|------------|
| clear() | Remove all elements | None |
| insert() | Insert element(s) | position, value/range |
| erase() | Remove element(s) | position or range |
| push_back() | Add to end | value |
| pop_back() | Remove from end | None |
| push_front() | Add to front | value (list/deque) |
| pop_front() | Remove from front | None (list/deque) |
| emplace() | Construct in place | position, args (C++11) |
| emplace_back() | Construct at end | args (C++11) |
| emplace_front() | Construct at front | args (C++11) |
| swap() | Swap contents | other container |
| operator= | Assignment | other container |

### Associative Container Functions (7 functions)

| Function | Returns | Purpose |
|----------|---------|---------|
| count(key) | size_t | Number of elements with key |
| find(key) | iterator | Iterator to key |
| equal_range(key) | pair<iterator,iterator> | Range of elements with key |
| lower_bound(key) | iterator | First element >= key |
| upper_bound(key) | iterator | First element > key |
| key_comp() | comparison function | Key comparator |
| value_comp() | comparison function | Value comparator |

### Hash Container Functions (9 functions)

| Function | Purpose |
|----------|---------|
| bucket(key) | Get bucket index for key |
| bucket_count() | Number of buckets |
| bucket_size(n) | Elements in bucket n |
| load_factor() | Average elements per bucket |
| max_load_factor() | Set/get max load factor |
| rehash(n) | Set bucket count |
| reserve(n) | Reserve capacity |
| hash_function() | Get hash function |
| key_eq() | Get key equality function |

### List-Specific Functions (7 functions)

| Function | Purpose | Parameters |
|----------|---------|------------|
| splice() | Transfer elements | dest_position, source_list |
| remove(val) | Remove specific value | value |
| remove_if(pred) | Remove satisfying predicate | predicate |
| unique() | Remove consecutive duplicates | Optional comparator |
| merge(other) | Merge sorted lists | other sorted list |
| sort() | Sort list | Optional comparator |
| reverse() | Reverse order | None |

---

## STL ALGORITHMS

### Non-Modifying Search Algorithms (12 functions)

| Function | Purpose | Returns |
|----------|---------|---------|
| find(range, value) | Find exact match | iterator to element or end |
| find_if(range, pred) | Find satisfying predicate | iterator to element or end |
| find_if_not(range, pred) | Find not satisfying predicate | iterator to element or end |
| find_first_of(r1, r2) | Find first element from r2 in r1 | iterator or end |
| adjacent_find(range) | Find adjacent equal elements | iterator to first or end |
| search(r1, r2) | Find subsequence r2 in r1 | iterator or end |
| search_n(range, n, val) | Find n consecutive equal values | iterator or end |
| count(range, value) | Count exact matches | count as size_t |
| count_if(range, pred) | Count satisfying predicate | count as size_t |
| mismatch(r1, r2) | Find first difference | pair of iterators |
| equal(r1, r2) | Compare ranges | bool |
| is_permutation(r1, r2) | Check permutation | bool |

### Modifying Algorithms (23 functions)

| Function | Purpose | Modifies |
|----------|---------|----------|
| copy(src, dst) | Copy elements | destination |
| copy_if(src, dst, pred) | Copy if predicate | destination |
| copy_n(src, n, dst) | Copy n elements | destination |
| copy_backward(src, dst) | Copy in reverse | destination |
| move(src, dst) | Move elements | destination |
| move_backward(src, dst) | Move in reverse | destination |
| swap(a, b) | Swap two elements | both elements |
| swap_ranges(r1, r2) | Swap two ranges | both ranges |
| transform(src, dst, op) | Apply operation | destination |
| replace(range, old, new) | Replace value | range |
| replace_if(range, pred, new) | Replace if predicate | range |
| replace_copy(src, dst, old, new) | Copy and replace | destination |
| replace_copy_if(src, dst, pred, new) | Copy and replace if | destination |
| fill(range, value) | Fill with value | range |
| fill_n(iter, n, value) | Fill n elements | starting at iter |
| generate(range, gen) | Generate with function | range |
| generate_n(iter, n, gen) | Generate n elements | starting at iter |
| remove(range, value) | Remove value | range (logical) |
| remove_if(range, pred) | Remove if predicate | range (logical) |
| remove_copy(src, dst, value) | Copy excluding value | destination |
| remove_copy_if(src, dst, pred) | Copy excluding if | destination |
| unique(range) | Remove consecutive duplicates | range |
| unique_copy(src, dst) | Copy removing duplicates | destination |
| reverse(range) | Reverse order | range |
| reverse_copy(src, dst) | Copy reversed | destination |
| rotate(range, middle) | Rotate left | range |
| rotate_copy(src, dst, middle) | Copy rotated | destination |
| shuffle(range, gen) | Random shuffle | range |

### Sorting Algorithms (7 functions)

| Function | Time Complexity | Stable | Purpose |
|----------|-----------------|--------|---------|
| sort() | O(n log n) | No | General-purpose sort |
| stable_sort() | O(n log n) | Yes | Preserves equal order |
| partial_sort(range, middle) | O(n log k) | No | Sort first k elements |
| partial_sort_copy() | O(n log k) | No | Copy with partial sort |
| nth_element() | O(n) average | No | Place nth element |
| is_sorted() | O(n) | - | Check if sorted |
| is_sorted_until() | O(n) | - | Find first unsorted |

### Binary Search Algorithms (4 functions)

| Function | Requirement | Returns |
|----------|-------------|---------|
| binary_search(range, value) | Sorted range | bool |
| lower_bound(range, value) | Sorted range | iterator to first >= value |
| upper_bound(range, value) | Sorted range | iterator to first > value |
| equal_range(range, value) | Sorted range | pair of iterators [low, high) |

### Set Operations (7 functions)

| Function | Requirement | Returns |
|----------|-------------|---------|
| includes(r1, r2) | Both sorted | bool |
| merge(r1, r2, result) | Both sorted | void |
| inplace_merge(range, middle) | Both parts sorted | void |
| set_union(r1, r2, result) | Both sorted | void |
| set_intersection(r1, r2, result) | Both sorted | void |
| set_difference(r1, r2, result) | Both sorted | void |
| set_symmetric_difference(r1, r2, result) | Both sorted | void |

### Heap Operations (6 functions)

| Function | Creates/Maintains | Purpose |
|----------|------------------|---------|
| make_heap(range) | Heap | Convert to heap |
| push_heap(range) | Heap | Add element to heap |
| pop_heap(range) | Heap | Remove max element |
| sort_heap(range) | Sorted range | Sort heap |
| is_heap(range) | Check | Is range a heap? |
| is_heap_until(range) | Check | Find first non-heap element |

### Permutation & Other Algorithms (12 functions)

| Function | Purpose | Returns/Modifies |
|----------|---------|------------------|
| next_permutation(range) | Next lexicographic permutation | bool - has next |
| prev_permutation(range) | Previous lexicographic permutation | bool - has previous |
| for_each(range, func) | Apply function to each | return value of func |
| all_of(range, pred) | All satisfy predicate | bool |
| any_of(range, pred) | Any satisfy predicate | bool |
| none_of(range, pred) | None satisfy predicate | bool |
| min(a, b) | Minimum of two values | minimum value |
| max(a, b) | Maximum of two values | maximum value |
| minmax(a, b) | Min and max | pair<T,T> |
| min_element(range) | Minimum element | iterator |
| max_element(range) | Maximum element | iterator |
| minmax_element(range) | Min and max elements | pair<iterator,iterator> |

---

## NUMERIC ALGORITHMS (5 functions)

| Function | Purpose | Requires Header |
|----------|---------|-----------------|
| accumulate(range, init) | Sum/fold elements | <numeric> |
| accumulate(range, init, op) | Fold with operation | <numeric> |
| partial_sum(range, dest) | Cumulative sums | <numeric> |
| partial_sum(range, dest, op) | Cumulative with operation | <numeric> |
| adjacent_difference(range, dest) | Differences between adjacent | <numeric> |
| inner_product(r1, r2, init) | Dot product | <numeric> |
| iota(range, value) | Fill with incrementing values | <numeric> |

---

## ITERATORS

### Iterator Types (5 types)

| Type | Capabilities | Containers |
|------|--------------|------------|
| Input Iterator | Read forward, single-pass | istream_iterator |
| Output Iterator | Write forward, single-pass | ostream_iterator, back_inserter |
| Forward Iterator | Read/write forward | forward_list, unordered_set |
| Bidirectional Iterator | Read/write forward/backward | list, set, map |
| Random Access Iterator | Read/write, any position | vector, deque, array, string |

### Iterator Functions (15 functions)

| Function | Purpose | Returns |
|----------|---------|---------|
| advance(it, n) | Move iterator n positions | void |
| distance(it1, it2) | Distance between iterators | distance as ptrdiff_t |
| next(it, n) | Iterator n positions ahead | new iterator |
| prev(it, n) | Iterator n positions behind | new iterator |
| begin(container) | Begin iterator (non-member) | iterator |
| end(container) | End iterator (non-member) | iterator |
| rbegin(container) | Reverse begin (non-member) | reverse_iterator |
| rend(container) | Reverse end (non-member) | reverse_iterator |
| cbegin(container) | Const begin (non-member) | const_iterator |
| cend(container) | Const end (non-member) | const_iterator |
| crbegin(container) | Const reverse begin | const_reverse_iterator |
| crend(container) | Const reverse end | const_reverse_iterator |
| make_move_iterator(it) | Construct move iterator | move_iterator |
| make_reverse_iterator(it) | Construct reverse iterator | reverse_iterator |
| back_inserter(container) | Back insertion iterator | back_insert_iterator |
| front_inserter(container) | Front insertion iterator | front_insert_iterator |

---

## UTILITY CLASSES & FUNCTIONS (9 items)

| Item | Purpose | Example |
|------|---------|---------|
| pair<T1, T2> | Store two values | `pair<int,string> p(1, "one")` |
| make_pair(a, b) | Create pair | `auto p = make_pair(1, "one")` |
| tuple<T...> | Store multiple values (C++11) | `tuple<int,string,double> t` |
| make_tuple(args) | Create tuple (C++11) | `auto t = make_tuple(1, "x", 3.14)` |
| tie(vars) | Unpack tuple (C++11) | `tie(x, s) = p` |
| structured_bindings | Unpack (C++17) | `auto [x, s] = p` |
| swap(a, b) | Swap values | `swap(x, y)` |
| move(val) | Rvalue reference (C++11) | `move(v1)` |
| forward(val) | Perfect forward (C++11) | `forward<T>(x)` |

---

## FUNCTION OBJECTS & PREDICATES

### Comparison Functions (6 objects)

| Object | Operation | Example |
|--------|-----------|---------|
| equal_to<T> | a == b | `equal_to<int>()(5, 5)` |
| not_equal_to<T> | a != b | `not_equal_to<int>()(5, 3)` |
| less<T> | a < b | `less<int>()(3, 5)` |
| less_equal<T> | a <= b | `less_equal<int>()(5, 5)` |
| greater<T> | a > b | `greater<int>()(5, 3)` |
| greater_equal<T> | a >= b | `greater_equal<int>()(5, 5)` |

### Arithmetic Functions (6 objects)

| Object | Operation | Example |
|--------|-----------|---------|
| plus<T> | a + b | `plus<int>()(3, 5)` |
| minus<T> | a - b | `minus<int>()(5, 3)` |
| multiplies<T> | a * b | `multiplies<int>()(3, 5)` |
| divides<T> | a / b | `divides<int>()(10, 2)` |
| modulus<T> | a % b | `modulus<int>()(10, 3)` |
| negate<T> | -a | `negate<int>()(5)` |

### Logical Functions (3 objects)

| Object | Operation | Example |
|--------|-----------|---------|
| logical_and<T> | a && b | `logical_and<bool>()(true, true)` |
| logical_or<T> | a \|\| b | `logical_or<bool>()(true, false)` |
| logical_not<T> | !a | `logical_not<bool>()(false)` |

### Adaptors (5 items)

| Adaptor | Purpose |
|---------|---------|
| bind(func, args) | Bind arguments to function (C++11) |
| not1(pred) | Negate unary predicate |
| not2(pred) | Negate binary predicate |
| mem_fun(ptr) | Member function pointer adaptor |
| mem_fn(ptr) | Member function adaptor (C++11) |

---

## STRING FUNCTIONS (28 functions)

| Function | Returns | Purpose |
|----------|---------|---------|
| length() / size() | size_t | String length |
| empty() | bool | Is empty |
| capacity() | size_t | Allocated capacity |
| reserve(n) | void | Reserve capacity |
| shrink_to_fit() | void | Reduce capacity |
| c_str() | const char* | C-style string |
| data() | const char* | Pointer to data |
| at(pos) | char& | Character with bounds check |
| operator[] | char& | Character without bounds |
| front() | char& | First character |
| back() | char& | Last character |
| substr(pos, len) | string | Substring |
| find(substr) | size_t | Position of substring |
| rfind(substr) | size_t | Position from end |
| find_first_of(chars) | size_t | First char in set |
| find_last_of(chars) | size_t | Last char in set |
| find_first_not_of(chars) | size_t | First char not in set |
| find_last_not_of(chars) | size_t | Last char not in set |
| compare(str) | int | Comparison result |
| replace(pos, len, str) | string& | Replace substring |
| erase(pos, len) | string& | Erase characters |
| insert(pos, str) | string& | Insert string |
| append(str) / += | string& | Append string |
| assign(str) | string& | Assign new value |
| clear() | void | Clear string |
| swap(other) | void | Swap contents |
| operator+ | string | Concatenate |
| operator== | bool | Equality |
| operator!= | bool | Inequality |
| operator< | bool | Less-than |

---

## COMPLETE STATISTICS

**Total STL Functions and Classes: 231**

- Containers: 16
- Container Member Functions: 54
- Algorithms: 93
- Numeric Algorithms: 5
- Iterators: 20
- Utility Classes: 9
- Function Objects: 18
- String Functions: 28

This reference covers C++98 through C++17 with notations for C++11 and C++17 features.
