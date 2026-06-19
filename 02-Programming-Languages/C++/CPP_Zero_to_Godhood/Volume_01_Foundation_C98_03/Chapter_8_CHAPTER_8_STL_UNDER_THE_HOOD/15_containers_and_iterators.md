# Chapter 15: Containers and Iterators

> *The architectural wonders of the Standard Template Library.*

Welcome to Part IV. Up until now, you have been crafting tools by hand: arrays, linked lists, and custom string logic. While this is great for learning, doing it in production code is a waste of time and highly prone to bugs.

The **Standard Template Library (STL)** is a massive collection of expertly crafted, mathematically optimized, and rigorously tested components that ship with every C++ compiler. 

The STL is built on three pillars:
1.  **Containers**: Data structures that hold objects (like vectors, maps, and sets).
2.  **Iterators**: Universal pointers that know how to navigate those structures.
3.  **Algorithms**: Functions (like sorting and searching) that operate on those structures using Iterators.

In this chapter, we will master the first two pillars.

---

## 15.1 The Architecture of the STL

The genius of the STL is its separation of concerns.

If you have 5 containers (Array, List, Tree, Hash Map, Deque) and you want to write a `sort()` function for each, you would normally have to write 5 different `sort()` functions because each structure stores memory differently.

The STL solves this by inserting **Iterators** in the middle. 
An Iterator is simply an object that acts like a pointer. It knows how to `++` (go to the next element) and `*` (get the value). 

Because every container provides an Iterator, the creators of the STL only had to write the `std::sort()` algorithm *once*. It simply asks for a "Begin Iterator" and an "End Iterator" and sorts everything in between, completely oblivious to what the actual container is!

## 15.2 `std::vector` (The Default Choice)

The `std::vector` is a dynamic array. It is the gold standard of C++. Unless you have a mathematically proven reason to use something else, **always use `std::vector`**.

Why? Because a vector stores its elements in a single, contiguous block of memory. This maximizes **Cache Locality**. The CPU can load chunks of the vector into its ultra-fast L1 cache, making iteration blindingly fast.

```cpp
#include <vector>
#include <iostream>

int main() {
    std::vector<int> scores;

    // Adding elements (Grows automatically!)
    scores.push_back(100);
    scores.push_back(200);

    // Access with bounds-checking (Throws an error if out of bounds)
    std::cout << scores.at(0) << "\n"; 

    // Access without bounds-checking (Fast, but dangerous like C-Arrays)
    std::cout << scores[1] << "\n"; 
}
```

> [!TIP]
> **Performance Tip: `reserve()`**
> When a vector runs out of space, it must allocate a larger block of memory, copy everything over, and delete the old block. This is slow. If you know you are going to add 10,000 items, tell the vector in advance: `scores.reserve(10000);`. It will rent the massive locker once, avoiding reallocations.

## 15.3 `std::list` and `std::deque`

### `std::list` (The Doubly Linked List)
A `std::list` stores elements in scattered memory locations, linking them together with pointers.
*   **Pros**: You can insert or remove elements in the middle in $O(1)$ time (if you already have an iterator pointing there).
*   **Cons**: Terrible Cache Locality. No random access (you cannot do `list[5]`).

### `std::deque` (The Double-Ended Queue)
A `std::deque` (pronounced "deck") is implemented as a sequence of fixed-size memory blocks. 
*   **Pros**: It allows extremely fast $O(1)$ insertions at *both* the front and the back. 
*   **Cons**: Slightly slower random access than a vector.

## 15.4 Associative Containers (`map` and `set`)

While Vectors and Lists are "Sequence" containers, Maps and Sets are "Associative" containers. They are usually implemented under the hood as **Red-Black Trees** (a type of self-balancing Binary Search Tree).

### `std::map` (Key-Value Pairs)
A map stores data like a dictionary. You look up a "Value" using a "Key". The keys are automatically sorted alphabetically (or numerically).

```cpp
#include <map>
#include <string>
#include <iostream>

int main() {
    std::map<std::string, int> ages;

    ages["Alice"] = 30;
    ages["Bob"] = 25;

    // Fast O(log N) lookup
    std::cout << "Alice is " << ages["Alice"] << " years old.\n";
}
```

### `std::set` (Unique Sorted Elements)
A set is like a map, but it only stores Keys. It is mathematically guaranteed to only contain unique elements. If you try to insert `10` five times, the set will still only contain one `10`.

## 15.5 Container Adapters (`stack` and `queue`)

Adapters are not new data structures. They are simply restricted wrappers around existing structures (usually a `deque` or `vector`). They force you to follow specific access rules.

*   **`std::stack`**: LIFO (Last In, First Out). You can only push or pop from the top.
*   **`std::queue`**: FIFO (First In, First Out). You push to the back and pop from the front.
*   **`std::priority_queue`**: Elements are automatically sorted as you insert them, so the "highest priority" item is always at the top. (Usually implemented as a Binary Heap).

```cpp
#include <stack>

int main() {
    std::stack<int> s;
    s.push(10);
    s.push(20);
    
    std::cout << s.top(); // 20
    s.pop();              // Removes 20
}
```

## 15.6 Iterators: The Universal Pointers

An Iterator is an object that simulates a pointer. Every STL container has a `begin()` and an `end()`.

> [!WARNING]
> **The `end()` Trap**
> The `end()` iterator does **NOT** point to the last element. It points to the imaginary slot *one past* the last element. This allows loops to know exactly when to stop.

```cpp
#include <vector>
#include <iostream>

int main() {
    std::vector<int> numbers;
    numbers.push_back(10);
    numbers.push_back(20);
    numbers.push_back(30);

    // The classic STL Iterator loop
    for (std::vector<int>::iterator it = numbers.begin(); it != numbers.end(); ++it) {
        std::cout << *it << "\n"; // Dereference the iterator to get the value
    }
}
```

### Iterator Categories
Not all iterators are created equal, because not all containers are equal:
1.  **Forward Iterators**: Can only move forward (`++`). Used by `std::forward_list`.
2.  **Bidirectional Iterators**: Can move forward (`++`) and backward (`--`). Used by `std::list`, `std::map`, `std::set`.
3.  **Random Access Iterators**: Can jump anywhere instantly (`it + 5`). Used by `std::vector`, `std::deque`.

---

Now that our data is beautifully organized into Containers, and we know how to navigate them using Iterators, we are ready to manipulate that data. In the next chapter, we will unlock the third pillar of the STL: **Algorithms**.
