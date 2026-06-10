# Functional Programming Modules


Python is not a purely functional language, but it provides powerful tools for functional programming patterns. These modules are almost entirely implemented in C, offering near-native performance for higher-order operations.

### 34.1 `itertools`: Infinite and Combinatoric Iterators

The `itertools` module provides functions that create iterators for efficient looping. They are "lazy"they produce values only when requested, consuming minimal memory.

#### 1. Infinite Iterators
*   `count(start, step)`: An infinite sequence of numbers.
*   `cycle(iterable)`: Saves a copy of the iterable and repeats it indefinitely.
*   `repeat(object, times)`: Returns the same object over and over.

#### 2. Combinatoric Iterators
*   `product()`: Cartesian product (nested for-loops).
*   `permutations()` / `combinations()`: High-performance combinatorial generation.
*   **Internals**: These are implemented as C-level classes that maintain the current state of the iteration in their internal structs, avoiding the overhead of Python-level generators.

### 34.2 `functools`: Higher-Order Functions

The `functools` module is for functions that act on or return other functions.

#### 1. `partial(func, *args, **keywords)`
`partial` returns a new `partial` object (a C struct).
*   **Internals**: It stores the function, the positional arguments, and the keyword arguments. When called, it merges the stored arguments with the new ones and calls the original function. This is significantly faster than using a lambda for the same purpose because it avoids creating a new Python function object and closure.

#### 2. `lru_cache(maxsize=128, typed=False)`
The Least Recently Used (LRU) cache is implemented using a **Dictionary** and a **Doubly Linked List**.
*   **Dictionary**: Maps the function arguments (hashable) to the result.
*   **Linked List**: Tracks the order of access. When a hit occurs, the item is moved to the front. If the cache is full, the item at the tail is evicted.
*   **Thread Safety**: It uses a reentrant lock to ensure that the internal linked list remains consistent across multiple threads.

### 34.3 `operator`: Exposing C-Level Opcodes

The `operator` module exports a set of efficient functions corresponding to Python's intrinsic operators.
*   `operator.add(x, y)` is equivalent to `x + y`.
*   `operator.itemgetter(index)` is equivalent to `lambda obj: obj[index]`.
*   `operator.attrgetter(attr)` is equivalent to `lambda obj: getattr(obj, attr)`.

**Godhood Tip**: Always use `operator.itemgetter` or `attrgetter` for sorting or mapping instead of lambdas. They are implemented in C and avoid the overhead of a Python function call for every element in the collection.

### 34.4 `singledispatch`: Generic Functions

`functools.singledispatch` allows for function overloading based on the type of the first argument.
*   **Internals**: It maintains a registry (a dictionary) mapping types to implementation functions. When the generic function is called, it performs a lookup in the registry. It also handles inheritance by traversing the MRO of the input type to find the closest registered handler.

---


---