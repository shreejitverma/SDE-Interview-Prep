# Python 2.4 to 2.7: Decorators, Context Managers, & the 2.x Twilight


### 5.1 Decorators: Syntactic Sugar and Compiler Mechanics (PEP 318 & PEP 3129)
Prior to Python 2.4, wrapping a function with utility behavior required declaring the function first, then immediately reassigning it in the host namespace:
```python
def query():
    pass
query = transaction(log(query))
```
This approach separated the metadata from the function declaration. PEP 318 introduced the `@decorator` syntax to resolve this.

#### 1. Compiler Translation & Bytecode Analysis
The `@` symbol is syntactic sugar resolved at compile-time. When the compiler encounters decorators, it loads the decorators onto the evaluation stack *before* compiling the target function, then applies them in bottom-up (reverse-textual) order.

Consider this nested decorator structure:
```python
@dec1
@dec2
def target():
    pass
```

The compiler translates this structure into the following bytecode operations:
```
  1           0 LOAD_NAME                0 (dec1)       /* Push dec1 onto stack */
              3 LOAD_NAME                1 (dec2)       /* Push dec2 onto stack */
              6 LOAD_CONST               0 (<code object target>)
              9 LOAD_CONST               1 ('target')
             12 MAKE_FUNCTION            0              /* Create function object target */
             15 CALL_FUNCTION            1              /* Call dec2(target) */
             18 CALL_FUNCTION            1              /* Call dec1(dec2(target)) */
             21 STORE_NAME               2 (target)     /* Bind result to name target */
```

#### 2. Decorator Execution Flow
1. The compiler pushes the decorator references `dec1` and `dec2` onto the evaluation stack.
2. `MAKE_FUNCTION` instantiates a `PyFunctionObject` from the compiled `<code object target>` and pushes it onto the stack.
3. The VM executes `CALL_FUNCTION 1`, popping `target` as the argument and `dec2` as the callable, pushing the returned wrapped function onto the stack.
4. The VM executes the second `CALL_FUNCTION 1`, popping the wrapped function and `dec1` as the callable, pushing the final wrapped function onto the stack.
5. `STORE_NAME` binds the resulting callable to the name `target` in the local namespace.

This bottom-up composition is mathematically equivalent to:
$$\text{target} = \text{dec1}\left(\text{dec2}\left(\text{target}\right)\right)$$

#### 3. Class Decorators (PEP 3129)
Introduced in Python 2.6, class decorators apply the same syntactic translation to class creation. After the class body is evaluated and the class type object is constructed, the compiler passes the type object to the decorator:
```python
@class_decorator
class Model(object):
    pass
```
This translates directly to:
$$\text{Model} = \text{class\_decorator}\left(\text{Model}\right)$$

---

### 5.2 Context Managers and the `with` Statement (PEP 343)
Introduced in Python 2.5, the `with` statement encapsulates clean acquisition and release patterns for resources.

#### 1. The Context Manager Protocol
An object is a context manager if it implements:
* `__enter__(self)`: Acquires the resource and returns the object to be bound to the `as` target (if present).
* `__exit__(self, exc_type, exc_val, exc_tb)`: Invoked when leaving the block.
  * If an exception was raised, `exc_type`, `exc_val`, and `exc_tb` contain the exception details. If `__exit__` returns a truthy value, the exception is silenced.
  * If no exception occurred, all three arguments are passed as `None`.

#### 2. Compiler Translation to Try-Finally
The compiler translates a `with` statement block:
```python
with expression as target:
    suite
```
Into a low-level equivalent logic block:
```python
mgr = expression
exit_method = type(mgr).__exit__
value = type(mgr).__enter__(mgr)
exc = True
try:
    try:
        target = value  # Bind target if "as" clause exists
        suite
    except:
        exc = False
        if not exit_method(mgr, *sys.exc_info()):
            raise
finally:
    if exc:
        exit_method(mgr, None, None, None)
```

#### 3. Bytecode Implementation and the Block Stack
To guarantee that `__exit__` is called even when exceptions propagate, the VM uses its internal **block stack** inside the execution frame.

When the VM executes a `with` block:
1. **`SETUP_WITH`**: Evaluates the context manager expression, calls `__enter__`, pushes the `__exit__` method onto the evaluation stack, pushes a `finally` block onto the frame's block stack, and pushes the `__enter__` return value onto the evaluation stack.
2. **`WITH_CLEANUP`** (or modern equivalent block unwinding): When leaving the `with` block (normally or via an exception), the VM pops the block off the block stack.
   * If an exception propagates, the VM leaves the exception (`type`, `value`, `traceback`) on the evaluation stack and calls `__exit__(type, value, traceback)`.
   * If `__exit__` returns `True`, the VM clears the exception state, preventing propagation. If `False`, the VM re-throws the exception.

---

### 5.3 Generator Enhancements: Coroutines and Frame Suspension (PEP 342)
Introduced in Python 2.5, PEP 342 expanded generators from simple, passive data producers into active coroutines by allowing bidirectional communication.

#### 1. Bidirectional Methods
* **`.send(value)`**: Resumes the generator and passes `value` back. Inside the generator, the active `yield` expression evaluates to this `value`. Calling `.send(None)` is equivalent to `next()`.
* **`.throw(type, value=None, traceback=None)`**: Raises the specified exception inside the generator frame at the suspended `yield` point.
* **`.close()`**: Raises a `GeneratorExit` exception at the suspended `yield` point. The generator must clean up resources and exit. If it yields another value instead of returning or exiting, the VM raises a `RuntimeError`.

#### 2. Frame Suspension Mechanics (`PyFrameObject`)
When a generator encounters a `yield` statement, the execution loop in `_PyEval_EvalFrameDefault()` suspends execution:

```
[Active Thread State] -> [Running Generator Frame] -> Yield Encountered
                                                            |
                                                            v
1. Save instruction pointer: frame->f_lasti = active_instruction_offset
2. Save evaluation stack pointer depth: frame->f_stackdepth = current_depth
3. Set generator state: generator->gi_state = GEN_SUSPENDED
4. Detach frame: thread_state->frame = frame->f_back
                                                            |
                                                            v
                                            [Return Control to Caller]
```

1. **`f_lasti` Preservation**: The VM saves the offset of the next instruction in `frame->f_lasti`.
2. **Stack Conservation**: The evaluation stack pointer is frozen at its current depth.
3. **State Transition**: The generator's state is set to `GEN_SUSPENDED`.
4. **Frame Detachment**: The frame is unlinked from the active thread state execution chain, returning control back to the caller while keeping the frame alive on the heap.

---

### 5.4 Under-the-Hood CPython Data Structure Layouts

#### 1. Python List Layout (`PyListObject`)
A Python list is a contiguous dynamic array of `PyObject*` pointers. Because list sizes change dynamically, CPython overallocates memory blocks to achieve $O(1)$ amortized append performance.

Let's inspect the `PyListObject` struct defined in `Include/listobject.h`:
```c
typedef struct {
    PyObject_VAR_HEAD
    PyObject **ob_item;      /* Vector of pointers to list items */
    Py_ssize_t allocated;    /* Number of slots allocated in ob_item memory */
} PyListObject;
```

When a list grows beyond its current capacity, CPython resizes the underlying array using the formula:
$$\text{allocated} = \text{newsize} + (\text{newsize} \gg 3) + (\text{newsize} < 9 \,?\, 3 : 6)$$

This formula balances memory overhead and reallocation speed. By adding $\approx 12.5\%$ extra slots as the list grows, it minimizes heap reallocations and memory copying costs.

Here is the resulting capacity growth trace:
| Item Count (`newsize`) | Allocated Capacity | Overallocation Factor |
|---|---|---|
| 0 | 0 | - |
| 1 | 4 | $400\%$ |
| 5 | 8 | $160\%$ |
| 9 | 16 | $177\%$ |
| 17 | 25 | $147\%$ |
| 1000 | 1129 | $112.9\%$ |

#### 2. Classic Sparse Dictionary Layout (Pre-Python 3.6)
Before Python 3.6 (PEP 468), dictionaries were sparse hash tables consisting of an array of `PyDictEntry` structs.

The entry struct `PyDictEntry` was defined in `Include/dictobject.h` as:
```c
typedef struct {
    Py_ssize_t me_hash;      /* Cached hash value of me_key */
    PyObject *me_key;        /* Pointer to the key PyObject */
    PyObject *me_value;      /* Pointer to the value PyObject */
} PyDictEntry;
```

The dictionary header `PyDictObject` was defined as:
```c
struct _dictobject {
    PyObject_HEAD
    Py_ssize_t ma_fill;      /* Active entries + dummy entries */
    Py_ssize_t ma_used;      /* Active entries only */
    Py_ssize_t ma_mask;      /* size of table - 1 */
    PyDictEntry *ma_table;   /* Pointer to the sparse entry table array */
};
```

This layout was memory-inefficient:
* Every slot in the table array consumed 24 bytes (on 64-bit systems), regardless of whether it contained an entry or was empty/deleted (dummy).
* To keep search collisions low, the table size was always a power of 2 and kept at least 1/3 empty.
* Memory was highly sparse, leading to poor cache locality.

```
Classic Sparse Dictionary Memory Layout:
[ Index 0: <me_hash, key_ptr, value_ptr> (24 bytes) ]
[ Index 1: <0, NULL, NULL>               (24 bytes - null padding) ]
[ Index 2: <me_hash, key_ptr, value_ptr> (24 bytes) ]
[ Index 3: <0, NULL, NULL>               (24 bytes - null padding) ]
```

#### 3. Sets Optimization (`PySetObject`)
Python sets are implemented as open-addressed hash tables similar to dictionaries, but without values. Lookups skip value retrieval entirely, evaluating only keys and hashes in tight C loops. A set's entries are instances of `PySetEntry`:
```c
typedef struct {
    PyObject *key;
    long hash;               /* Cached hash value of key */
} PySetEntry;
```

#### 4. Tuples Optimization (`PyTupleObject`)
Tuples are immutable sequences stored as a single contiguous memory block containing the type header, the item count, and an array of `PyObject*` pointers:
```c
typedef struct {
    PyObject_VAR_HEAD
    PyObject *ob_item[1];    /* Inline array of pointers (allocated dynamically) */
} PyTupleObject;
```
* **Tuple Free Lists**: To avoid the overhead of the system allocator, CPython maintains an array of free lists for small tuples up to size 20. When a tuple under size 20 is deallocated, its memory block is cached in the corresponding size slot of the free list for immediate reuse during subsequent tuple allocations.

---
