# CPython Memory Allocator (PyMalloc) & Generational Garbage Collection


### 23.1 CPython's Memory Allocation Engine (PyMalloc)
For large allocations (greater than 512 bytes), CPython forwards the request directly to the system's standard C library `malloc()`. However, for small objects ($\le 512$ bytes)which represent the vast majority of Python allocationsstandard operating system allocators introduce high fragmentation and locking overhead. 
To resolve this, CPython implements a custom small-object allocator called **PyMalloc**.

#### 1. PyMalloc Memory Hierarchy
PyMalloc structures memory into three layers:
```
+-------------------------------------------------------------+
|                     ARENA (256 KB)                          |
|  Aligned to 256 KB boundaries in virtual memory.            |
|  Contains exactly 64 Pools.                                 |
|                                                             |
|   +-------------------+ +-------------------+               |
|   |    POOL (4 KB)    | |    POOL (4 KB)    |  ...          |
|   |  Allocates one    | |  Allocates one    |               |
|   |  size-class only. | |  size-class only. |               |
|   |  +--------------+ | |  +--------------+ |               |
|   |  | BLOCK (8B-512B)| |  | BLOCK (8B-512B)| |               |
|   |  +--------------+ | |  +--------------+ |               |
|   +-------------------+ +-------------------+               |
+-------------------------------------------------------------+
```

1. **Arenas**: Contiguous virtual memory blocks of 256 KB, aligned to 256 KB boundaries. Arenas manage virtual memory allocations from the OS and contain exactly 64 pools.
2. **Pools**: Blocks of 4 KB (matching the OS virtual page size). A pool is dedicated to a single **size class** (e.g., all blocks in a pool are 32 bytes, or all are 64 bytes).
3. **Blocks**: The actual chunks of memory returned to the interpreter. Blocks range from 8 bytes to 512 bytes, aligned to 8-byte steps (giving 64 distinct size classes).

#### 2. Size Class Formula
The size class for an allocation request of size $S$ is calculated by:
$$\text{Class Index} = \left\lceil \frac{S}{8} \right\rceil - 1$$
This structured layout allows PyMalloc to bypass the general-purpose allocator heap search and perform near-instantaneous $\mathcal{O}(1)$ allocation and free operations using thread-local lookups.

---

### 23.2 Generational Garbage Collection Cycle Detection
While reference counting manages the vast majority of object lifecycles, it cannot identify self-referencing cyclic loops (e.g., `x = []; x.append(x)`). To reclaim cyclic memory leaks, CPython runs a cyclic Garbage Collector (GC) as a background system.

#### 1. C-level GC Header Layout
Every object tracked by the GC has an extra header prefix in memory before its normal `PyObject` structure. The GC cast looks up objects using the `PyGC_Head` struct:

```c
typedef union _gc_head {
    struct {
        union _gc_head *gc_next;    /* Pointer to next object in the generation list */
        union _gc_head *gc_prev;    /* Pointer to previous object in the generation list */
        Py_ssize_t gc_refs;         /* GC reference count copy used during cycle checks */
    } gc;
    double dummy;                   /* Forces alignment boundary adjustments */
} PyGC_Head;
```

#### 2. The Three Generations
The GC divides tracked objects into three generations based on survival age:
*   **Generation 0 (Gen 0)**: Where new objects are registered. Collected frequently.
*   **Generation 1 (Gen 1)**: Objects that survived a Gen 0 collection pass.
*   **Generation 2 (Gen 2)**: Long-lived objects. Collected least frequently.

Collection is triggered when the number of allocations minus deallocations in a generation exceeds a configured threshold:
$$\text{Allocations} - \text{Deallocations} > \text{Threshold}$$

#### 3. The Cycle Detection Algorithm
To find and break reference cycles without locking the entire runtime heap, CPython executes the following steps:
1. **Copy Reference Counts**: The GC copy-assigns the reference count of every object in the collection generation to its `gc_refs` field in the `PyGC_Head` struct.
2. **Subtract Internal References**: For each object, the GC traverses its linked references (via `tp_traverse` slots). If a target object is also in the collection generation, the GC decrements the target's `gc_refs` count.
3. **Isolate Garbage**:
    *   If an object's `gc_refs` drops to 0, it means the object is only reachable via references *within* the generation cycle (candidates for garbage).
    *   If `gc_refs > 0`, the object is reachable from outside the generation. The object and all objects it references are marked as reachable and moved back to the main list.
4. **Deallocate Cycles**: Any remaining objects with `gc_refs == 0` are confirmed unreachable. The GC breaks the reference cycle by clearing references and deallocating the memory.

---
