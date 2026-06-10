# UNDER THE HOOD: BUILT-IN DATA STRUCTURES


### 5.1 Python List Memory Management
A Python list is a contiguous dynamic array of `PyObject*` pointers. Because list sizes change dynamically, CPython overallocates memory blocks to achieve $O(1)$ amortized append performance.

#### The List Growth Formula:
When a list exceeds its current capacity during an `append` or `insert` call, CPython recalculates its allocation size in `Objects/listobject.c` using the formula:
$$\text{new\_allocated} = \text{newsize} + (\text{newsize} \gg 3) + (\text{newsize} < 9 \,?\, 3 : 6)$$

Here is the resulting capacity growth trace:
| Item Count (`newsize`) | Allocated Capacity | Overallocation Factor |
|---|---|---|
| 0 | 0 | - |
| 1 | 4 | $400\%$ |
| 5 | 8 | $160\%$ |
| 9 | 16 | $177\%$ |
| 17 | 25 | $147\%$ |
| 1000 | 1129 | $112.9\%$ |

This formula balances memory overhead and reallocation speed. By adding $\approx 12.5\%$ extra slots as the list grows, it minimizes heap reallocations and memory copying costs.

### 5.2 Python Dict: Hashing and Collision pertubation
A Python dictionary is a sparse hash table. CPython uses open addressing (probing) to resolve collisions.

#### Collision Perturbation Formula:
When a hash collision occurs, CPython searches for the next index using a pseudo-random probing formula. Rather than a simple linear probe (which causes clustering), it shifts the upper bits of the hash code down to contribute to the index calculation:
$$i = (5 \times i + 1 + \text{perturb}) \pmod{\text{size}}$$

In each probe step:
$$\text{perturb} \gg= 5$$
As $\text{perturb}$ drops to zero, the formula collapses to:
$$i = (5 \times i + 1) \pmod{\text{size}}$$
This ensures every slot in the table is eventually checked.

#### The Compact Dict Layout (PEP 468 / Python 3.6+):
Previously, dictionary tables were stored as sparse arrays of `PyDictEntry` structs (24 bytes per bucket):
```
Pre-3.6 Sparse Dictionary Layout (Huge footprint, unpreserved order):
[ Index 0: <hash, key_ptr, value_ptr> (24 bytes) ]
[ Index 1: <Empty / 24 bytes null padding>       ]
[ Index 2: <hash, key_ptr, value_ptr> (24 bytes) ]
```

The modern compact dict layout splits this into two arrays:
1.  **Indices Array**: A sparse array of small integers (typically 1 byte each) mapping hash codes to positions in the entries array.
2.  **Entries Array**: A dense, contiguous array of `PyDictKeyEntry` structs (containing `hash`, `key_ptr`, and `value_ptr`) stored in insertion order.

```
Modern Compact Dictionary Layout (PEP 468):
Indices Array: [ 0, -1, 1 ]  (Fast index references)
Entries Array (Dense, ordered):
[ Slot 0: <hash1, key1, val1> ]
[ Slot 1: <hash2, key2, val2> ]
```
This saves up to 40% memory and preserves insertion order by default.

### 5.3 Sets and Tuples Optimizations
*   **Sets (`PySetObject`)**: Implemented similarly to compact dicts, but contain only keys. Set lookups bypass value lookups entirely, using specialized open-addressing C loops.
*   **Tuples (`PyTupleObject`)**: Tuples are immutable, so they are allocated as a single contiguous block of memory containing both the `PyVarObject` header and the array of `PyObject*` pointers.
    - *Allocation Cache*: CPython avoids calling the system memory allocator when creating short tuples. It maintains a **free list** of recycled tuple objects for sizes up to 20. When a tuple is garbage-collected, its memory is not released to the OS; instead, it is placed in the free list for quick reuse.

---
