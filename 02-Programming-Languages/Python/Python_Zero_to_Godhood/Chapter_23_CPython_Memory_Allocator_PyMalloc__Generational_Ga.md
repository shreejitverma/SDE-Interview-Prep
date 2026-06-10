# CPython Memory Allocator (PyMalloc) & Generational Garbage Collection


### 23.1 CPython's Memory Allocation Engine (PyMalloc)
For large allocations (greater than 512 bytes), CPython forwards the request directly to the system's standard C library allocator (`malloc()`). However, for small objects ($\le 512$ bytes)which represent the vast majority of Python allocationsstandard operating system allocators introduce high fragmentation and locking overhead. 
To resolve this, CPython implements a custom small-object allocator called **PyMalloc**.

#### 1. PyMalloc Memory Hierarchy
PyMalloc structures memory into three distinct layers to minimize operating system allocation calls:
*   **Arenas (256 KB)**: Contiguous memory blocks allocated from the operating system, aligned to 256 KB boundaries. Arenas manage memory at the virtual memory level and contain exactly 64 pools.
*   **Pools (4 KB)**: Subdivisions of arenas that match the operating system's virtual page size. Each pool is dedicated to a single **size class** (e.g., all blocks in a pool are 32 bytes, or all are 64 bytes).
*   **Blocks**: The actual chunks of memory returned to the interpreter. Blocks range from 8 bytes to 512 bytes, aligned to 8-byte steps (giving 64 distinct size classes).

#### 2. C-level Struct Definitions in obmalloc.c
The structures for pools and arenas are defined in CPython's memory management source file (`Objects/obmalloc.c`).

##### Pool Header Struct
Every 4 KB pool begins with a header that tracks block allocations and linked lists of free pools:

```c
/* Objects/obmalloc.c */
typedef uint8_t block;

struct pool_header {
    union { 
        block *as_block; 
        uint32_t as_uint; 
    } nextfree;             /* Pointer to the next available free block in the pool */
    union { 
        block *as_block; 
        uint32_t as_uint; 
    } firstfree;            /* Pointer to the first free block in the pool */
    struct pool_header *nextpool; /* Link to the next pool in the active list */
    struct pool_header *prevpool; /* Link to the previous pool in the active list */
    uint32_t refcount;      /* Number of allocated blocks currently in use */
    uint32_t szidx;         /* Size class index of this pool */
    uint32_t freeblocks;    /* Number of free blocks remaining in the pool */
};
typedef struct pool_header *poolp;
```

*   `nextfree`: Points to the next block in a singly linked list of free blocks inside the pool. PyMalloc uses **in-place pointers** inside the free blocks themselves to implement this list without consuming extra memory.
*   `firstfree`: Tracks the boundary of allocated blocks versus unallocated space in the pool.
*   `szidx`: Specifies which size class this pool belongs to.

##### Arena Object Struct
CPython tracks arenas using an array of `arena_object` descriptors:

```c
/* Objects/obmalloc.c */
struct arena_object {
    uintptr_t address;          /* Base virtual memory address of the 256 KB block */
    block* pool_address;        /* Pointer to the first pool inside this arena */
    uint32_t nfreepools;        /* Number of pools currently free in this arena */
    uint32_t ntotalpools;       /* Total number of pools inside this arena (usually 64) */
    struct arena_object* nextarena; /* Link to the next arena */
    struct arena_object* prevarena; /* Link to the previous arena */
};
```

#### 3. Size Class Formula
The size class for an allocation request of size $S$ is calculated by:

$$\text{Class Index} = \left\lceil \frac{S}{8} \right\rceil - 1$$

This mapped alignment restricts memory fragmentation, ensuring that small object requests are answered in $\mathcal{O}(1)$ time by locating the active pool array matching the calculated class index.

---

### 23.2 Generational Garbage Collection and Cycle Detection
While reference counting manages the vast majority of object lifecycles, it cannot identify self-referencing cyclic loops (e.g., `x = []; x.append(x)`). To reclaim cyclic memory leaks, CPython runs a cyclic Garbage Collector (GC) as a background system.

#### 1. GC Memory Prefix and Header Layout
Every object tracked by the GC has an extra header prefix in memory before its normal `PyObject` structure. When `PyObject_GC_New()` is called, it allocates memory block of size:

$$\text{Allocation Size} = \text{sizeof(PyGC\_Head)} + \text{sizeof(PyObject)}$$

```
+--------------------------------------------------------+
|                      PyGC_Head                         |
|  - gc_next: Linkage to other GC tracked objects        |
|  - gc_prev: Linkage to other GC tracked objects        |
|  - gc_refs: Temporary reference state                  |
+--------------------------------------------------------+
|                      PyObject                          | <--- Object Pointer Returned to User
|  - ob_refcnt: Standard reference counter               |
|  - ob_type: Type pointer                               |
+--------------------------------------------------------+
```

The pointer returned to the user points directly to the `PyObject` start. The VM accesses the GC header by subtracting the header size:

$$\text{PyGC\_Head Pointer} = (\text{PyGC\_Head*})\text{op} - 1$$

The C-level layout of `PyGC_Head` is defined in `Include/internal/pycore_gc.h`:

```c
typedef union _gc_head {
    struct {
        uintptr_t gc_next;   /* Pointer to next object in generation list */
        uintptr_t gc_prev;   /* Pointer to previous object in generation list */
        Py_ssize_t gc_refs;  /* Reference count copy used during cycle checks */
    } gc;
    double dummy;            /* Forces double-word alignment adjustments */
} PyGC_Head;
```

#### 2. The Three Generations
The GC divides tracked objects into three generations based on survival age:
*   **Generation 0 (Gen 0)**: Where new objects are registered. Collected frequently.
*   **Generation 1 (Gen 1)**: Objects that survived a Gen 0 collection pass.
*   **Generation 2 (Gen 2)**: Long-lived objects. Collected least frequently.

Collection is triggered when the number of allocations minus deallocations in a generation exceeds a configured threshold:

$$\text{Allocations} - \text{Deallocations} > \text{Threshold}$$

Default thresholds are `(700, 10, 10)`. You can view or change them via `gc.get_threshold()` and `gc.set_threshold()`.

---

### 23.3 The Cycle Detection Algorithm
To find and break reference cycles, the GC executes the following steps during a collection pass:

#### 1. Initializing gc_refs
The GC copy-assigns the reference count of every object in the collection generation to its `gc_refs` field:

$$\text{gc\_refs} = \text{ob\_refcnt}$$

#### 2. Traversing Internal References
For each object in the generation list, the GC calls its `tp_traverse` slot (a C function that lists all objects referenced by the object). If a referenced object is also in the collection generation, the GC decrements that object's `gc_refs`:

$$\text{gc\_refs} = \text{gc\_refs} - 1$$

After traversing all objects, any object that still has `gc_refs > 0` must be reachable from outside the generation list (e.g., from local variables, global parameters, or a higher generation).

#### 3. Resolving and Splitting Collections
The GC splits the candidate list into two sets: `reachable` and `unreachable`.

```
                  [GC Collection Candidates]
                              |
              Does object have gc_refs > 0?
                           /     \
                         Yes      No
                         /         \
            [Reachable Set]       [Unreachable Set]
```

1.  If an object has `gc_refs > 0`, it is moved back to the `reachable` list.
2.  All objects reachable *from* that object (transitively traversed via `tp_traverse`) are also marked as `reachable` and moved out of the `unreachable` list, even if their `gc_refs` was zero.
3.  Any remaining objects in the `unreachable` set are confirmed to be part of a cyclic garbage loop and are marked for deallocation.

#### 4. Clearing Cycles and Deallocation
The GC breaks the reference cycle by calling the `tp_clear` slot on each object in the `unreachable` set. This sets internal pointers (like dictionary entries or list items) to `None` or clears them, which decrements the reference counts of the target objects and allows the standard reference counter to deallocate the memory.

---
