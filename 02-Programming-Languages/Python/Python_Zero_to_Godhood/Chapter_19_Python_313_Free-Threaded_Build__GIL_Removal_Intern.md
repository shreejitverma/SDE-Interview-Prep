# Python 3.13: Free-Threaded Build & GIL Removal Internals


### 20.1 The Free-Threaded CPython Paradigm Shift
Introduced provisionally in Python 3.13 (PEP 703), the free-threaded build of CPython allows executing bytecode in parallel across multiple OS threads without the constraint of the Global Interpreter Lock (GIL). 
To maintain thread safety and prevent race conditions on shared objects, CPython's runtime engine has been fundamentally rewritten, moving away from single-threaded assumptions to scalable multi-threaded internals.

---

### 20.2 Biased Reference Counting (BRC)
Reference counting is the primary mechanism for memory management in CPython. In a standard build, incrementing/decrementing reference counts is a simple, non-atomic operation: `op->ob_refcnt++`. In a thread-parallel environment, standard operations would cause race conditions. Using standard atomic operations (`atomic_add` / `atomic_sub`) globally would destroy performance due to cache coherency traffic (bus locks) across CPU cores.

To solve this, PEP 703 introduces **Biased Reference Counting (BRC)**. BRC divides an object's reference count state based on thread ownership.

#### 1. Biased Object Headers
An object is "biased" toward the thread that allocated it (the owner thread). The owner thread uses fast, non-atomic CPU instructions to modify the reference count. Non-owner threads (foreign threads) must use atomic operations on a separate "shared" reference count field.

#### 2. C-level Struct representation of reference layout
In the free-threaded CPython header definition (`object.h` under no-GIL configurations), the object header is redefined:

```c
typedef struct _object {
    uintptr_t ob_ref_local;     /* Local reference count accessed only by owner thread */
    uintptr_t ob_ref_shared;    /* Shared reference count accessed atomically by foreign threads */
    PyTypeObject *ob_type;      /* Pointer to object type descriptor */
} PyObject;
```

#### 3. BRC Reference Counting Algorithm
When a reference is modified, CPython checks the current thread ID against the owner thread ID encoded inside the object's header:
$$\text{Is Owner} = (\text{Current Thread ID} == \text{ob\_ref\_local} \ \& \ \text{THREAD\_ID\_MASK})$$

```
                  [Reference Count Change Request]
                                 |
                     Is Current Thread the Owner?
                               /   \
                             Yes    No
                             /       \
     [Non-Atomic Local Increment]   [Atomic Shared Increment]
      (Fast path: CPU registers)     (Slow path: CAS / Bus Lock)
```

If the owner thread detects that the local reference count drops to zero, it merges the shared reference count. If the combined total reference count reaches zero, the object is safely deallocated.

---

### 20.3 Thread-Safe Allocation via mimalloc
CPython's classic allocator (`pymalloc`) is optimized for small objects but is single-threaded and not thread-safe. In the free-threaded build, `pymalloc` is replaced by **mimalloc**, a highly efficient, thread-safe, metadata-compact memory allocator developed by Microsoft.
*   **Thread-Local Pools**: Mimalloc organizes memory in thread-local heaps, allowing allocation to proceed without acquiring global locks in most cases.
*   **Thread-Safe Free Lists**: When a foreign thread deallocates an object, it does not write directly back to the owner thread's main free list. Instead, it writes to a thread-safe "atomic free list" associated with the target page, preventing cross-thread allocation bottlenecks.

---

### 20.4 Hazard Pointers and Deferred Reference Counting
Some global or long-lived objects (such as interned strings, code constants, or modules) are read frequently by many threads but rarely modified.
To bypass BRC overhead on these objects:
1. **Immortal Objects**: CPython marks these objects as immortal by setting a special bitmask in their reference count field. The interpreter completely skips reference counting modifications for immortal objects.
2. **Hazard Pointers**: When traversing lock-free data structures (like internal dictionaries or type caches), threads use **Hazard Pointers** to declare they are accessing an object. This ensures the object will not be freed by the GC or another thread while in use, avoiding the need to perform expensive atomic reference count updates for transient reads.

---
