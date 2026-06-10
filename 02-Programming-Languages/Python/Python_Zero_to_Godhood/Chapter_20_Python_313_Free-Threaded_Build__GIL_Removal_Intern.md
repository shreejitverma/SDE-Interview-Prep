# Python 3.13: Free-Threaded Build & GIL Removal Internals


### 20.1 The Free-Threaded CPython Paradigm Shift
Introduced provisionally in Python 3.13 (PEP 703), the free-threaded build of CPython (also known as the "no-GIL" build) allows executing bytecode in parallel across multiple OS threads without the constraint of the Global Interpreter Lock (GIL). 

In standard CPython, the GIL serves as a single global mutex protecting the interpreter state and all Python objects from concurrent access. This guarantees that only one thread executes bytecode at a time, simplifying the VM design and preventing memory races. However, it severely limits Python's ability to scale on multi-core processors.

To remove the GIL safely, the CPython runtime has been fundamentally re-engineered. Moving from single-threaded assumptions to a scalable parallel execution engine required redesigning three pillars of the runtime:
1.  **Reference Counting**: Overcoming atomic bus contention on object refcount updates.
2.  **Memory Allocation**: Ensuring lock-free, thread-local allocations for object instantiation.
3.  **Collection Mutability**: Protecting built-in data structures (dictionaries, lists, sets) from race conditions.

---

### 20.2 Biased Reference Counting (BRC) and Object Headers
Reference counting is the primary mechanism for memory management in CPython. In a standard build, incrementing/decrementing reference counts is a simple, non-atomic operation: `op->ob_refcnt++`. In a thread-parallel environment, standard operations would cause race conditions. However, using standard atomic operations (`atomic_add` / `atomic_sub` or `LOCK XADD` instructions) globally would destroy performance due to cache coherency traffic (bus locks) across CPU cores.

To solve this, PEP 703 introduces **Biased Reference Counting (BRC)**. BRC divides an object's reference count state based on thread ownership.

#### 1. Biased Object Headers
An object is "biased" toward the thread that allocated it (the owner thread). The owner thread uses fast, non-atomic CPU instructions to modify the reference count. Non-owner threads (foreign threads) must use atomic operations on a separate "shared" reference count field.

Under no-GIL configurations, the standard `PyObject` header (`Include/object.h`) is redefined:

```c
typedef struct _object {
    uintptr_t ob_ref_local;     /* Local reference count and owner thread ID */
    uintptr_t ob_ref_shared;    /* Shared reference count and object status flags */
    PyTypeObject *ob_type;      /* Pointer to object type descriptor */
} PyObject;
```

#### 2. Bit Layout and Fields representation
The fields `ob_ref_local` and `ob_ref_shared` encode multiple pieces of metadata to optimize memory usage:

*   `ob_ref_local`:
    *   **Thread ID Bits (Upper Bits)**: Holds the memory address of the owner thread's thread-state structure (`PyThreadState*`).
    *   **Local Refcount (Lower Bits)**: A small counter tracking reference updates made by the owner thread. Typically, the lower 16 or 32 bits are used for this count.
*   `ob_ref_shared`:
    *   **Shared Refcount (Upper Bits)**: A signed counter tracking reference additions/subtractions made by non-owner threads.
    *   **Status Flags (Lower Bits)**: Bits reserved for encoding object states:
        *   `STATE_STATIC` (bit 0): Set if the object is static (e.g., statically allocated builtin types).
        *   `STATE_IMMORTAL` (bit 1): Set if the object is immortal (reference counts are never modified).
        *   `STATE_DEFERRED` (bit 2): Set if the object uses deferred reference counting (GC-managed objects).

#### 3. BRC Reference Counting Algorithm
When modifying a reference (via `Py_INCREF` or `Py_DECREF`), the runtime performs owner-thread detection:

$$\text{Owner Thread State Pointer} = \text{ob\_ref\_local} \& \text{THREAD\_STATE\_MASK}$$

```
                   [Reference Count Change Request]
                                  |
               Get Current ThreadState (Tstate)
                                  |
              Is Tstate == (ob_ref_local & MASK)?
                                /   \
                              Yes    No
                              /       \
      [Non-Atomic Local Increment]   [Atomic Shared Increment]
       (Fast path: CPU registers)     (Slow path: CAS / Bus Lock)
```

##### Increment Code Walkthrough
```c
static inline void
_Py_INCREF_Specialized(PyObject *op, PyThreadState *tstate)
{
    uintptr_t local = op->ob_ref_local;
    // Fast path: current thread is the owner
    if ((local & _Py_THREAD_ID_MASK) == (uintptr_t)tstate) {
        op->ob_ref_local = local + _Py_REF_INCREMENT;
    }
    // Slow path: foreign thread
    else {
        _Py_Incref_Shared(op);
    }
}
```

##### Decrement and Deallocation Walkthrough
When a reference is decremented, if the local count reaches zero, the owner thread merges the shared reference count:

```c
static inline void
_Py_DECREF_Specialized(PyObject *op, PyThreadState *tstate)
{
    uintptr_t local = op->ob_ref_local;
    if ((local & _Py_THREAD_ID_MASK) == (uintptr_t)tstate) {
        uintptr_t new_local = local - _Py_REF_INCREMENT;
        if ((new_local & _Py_REF_MASK) == 0) {
            // Local count hit zero; merge shared reference counts to check for deallocation
            _Py_Merge_And_Dealloc(op);
        } else {
            op->ob_ref_local = new_local;
        }
    } else {
        _Py_Decref_Shared(op);
    }
}
```

During `_Py_Merge_And_Dealloc(op)`, the owner thread atomically reads and clears `ob_ref_shared`. If the sum of `ob_ref_local` (cleared of thread state bits) and `ob_ref_shared` is less than or equal to zero, it calls the type's `tp_dealloc` slot to free the object.

---

### 20.3 Thread-Safe Allocation via mimalloc
CPython's classic allocator (`pymalloc`) is optimized for small objects but relies on single-threaded assumptions. Under a free-threaded runtime, sharing a global allocator lock would lead to lock contention. Consequently, PEP 703 replaces `pymalloc` with **mimalloc**, a thread-safe, metadata-compact allocator developed by Microsoft.

#### 1. Mimalloc Structural Hierarchy
Mimalloc organizes memory allocations into hierarchical structures to minimize thread synchronization:

```
+-------------------------------------------------------+
|                       OS Page                         |
|  +-------------------------------------------------+  |
|  |                    Segment                      |  |
|  |  +------------------+     +------------------+  |  |
|  |  |      Page 1      |     |      Page 2      |  |  |
|  |  |  +------------+  |     |  +------------+  |  |  |
|  |  |  | Local Heap |  |     |  | Local Heap |  |  |  |
|  |  |  +------------+  |     |  +------------+  |  |  |
|  |  +------------------+     +------------------+  |  |
|  +-------------------------------------------------+  |
+-------------------------------------------------------+
```

*   **Segments (typically 4MB)**: Large contiguous memory blocks requested from the operating system.
*   **Pages (typically 64KB - 512KB)**: Subdivisions of segments containing blocks of a single size class (e.g., 32-byte blocks, 64-byte blocks).
*   **Heaps**: Thread-local handles containing lists of active pages.

#### 2. Thread-Local Allocations
Every OS thread maintains its own thread-local mimalloc heap (`mi_heap_t`). When a thread executes `PyObject_New()`, it attempts to allocate from its thread-local heap:
1.  It locates the active page corresponding to the requested size class.
2.  It pops a block from the page's thread-local **free list**. This list is accessed only by the owning thread, requiring no atomic locks or memory barriers.

#### 3. Cross-Thread Deallocations and Thread-Safe Free Lists
When a foreign thread deallocates an object, writing directly back to the owner thread's thread-local free list would cause race conditions. To resolve this, mimalloc uses a secondary, atomic free list for each page:

```
[Foreign Thread deallocates block]
               |
               v (Atomic CAS operation)
      +------------------+
      |  Atomic Free List| (Attached to Page)
      +------------------+
               |
               v (During thread-local heap maintenance)
      +------------------+
      | Local Free List  | (Accessed non-atomically by Owner Thread)
      +------------------+
```

When a foreign thread frees memory:
1.  It atomically prepends the freed block to the page's **atomic free list** (`thread_free`) using a lock-free Compare-And-Swap (CAS) loop.
2.  When the owner thread exhausts its thread-local `free` list, it cleans up and merges the `thread_free` list into its local list. This deferred merging drastically reduces cache line bouncing across CPU cores.

---

### 20.4 Lock-free Collections & Thread Safety
In standard CPython, code executing modifications to dictionaries, lists, or sets does not need to worry about internal consistency, as the GIL serializes all operations. Without the GIL, concurrent reads/writes could corrupt collection structures, leading to segmentation faults or memory corruption.

#### 1. Dictionary Locking System
CPython's compact dictionary layout was modified to support concurrent read and write operations.
*   **Read Paths (Lock-free)**: Operations like dict lookups (`PyDict_GetItem`) read the hash index and table entries without acquiring locks. They rely on atomic pointer reads and memory barriers to ensure they read consistent states.
*   **Write Paths (Fine-grained Locks)**: Modifying operations (insertion, deletion) acquire a localized lock associated with the dictionary instance. Instead of locking the global runtime, CPython locks only the specific dictionary being modified.

To avoid allocating a full OS mutex for every dictionary (which would consume excessive memory), CPython uses a pool of shared locks, indexing into them using a hash of the dictionary's memory address.

#### 2. Lock Arrays and PyMutex
To support light, low-overhead synchronization, CPython implements a highly efficient locking primitive: **PyMutex**.

```c
typedef struct {
    uint8_t v; /* Lock state byte */
} PyMutex;
```

*   **State Byte `v`**:
    *   Bit 0: Indicates if the lock is held (1) or free (0).
    *   Bit 1: Indicates if there are threads waiting in the queue (1) or not (0).
*   **Fast Path**: A thread attempts to acquire the lock using a single atomic test-and-set instruction (`atomic_compare_exchange` on bit 0). If successful, it proceeds without sleeping or system calls.
*   **Slow Path**: If the lock is held, the thread registers itself in a global wait queue associated with the lock's memory address and suspends execution, waiting to be woken up by the lock holder.

---

### 20.5 Generational Garbage Collection (GC) in a GIL-less World
CPython's cyclic garbage collector identifies self-referencing loops that reference counting alone cannot reclaim. In a standard build, the GC runs synchronously on the main thread during allocations. In a free-threaded environment, running the GC requires coordinated synchronization across all running threads to prevent mutator threads from modifying object references during the collection pass.

#### 1. Stop-the-World (STW) Synchronization
To run the cyclic collection safely, CPython implements a Stop-the-World mechanism:

```
[GC Thread initiates GC] ---> Sets global GC status flag
                                      |
     [All other threads reach safe-points / bytecode loop boundary]
                                      |
                          Suspends all mutator threads
                                      |
                          Executes GC Collection Pass
                                      |
                           Resumes all mutator threads
```

1.  A thread triggering a collection sets a global GC request flag.
2.  All other running threads check this flag at defined bytecode boundaries (safe-points).
3.  Upon detecting the flag, threads suspend their execution and block on an OS condition variable.
4.  Once all threads are verified as stopped, the GC thread safely performs the collection pass, scanning object references and breaking cyclic loops.
5.  After completion, the GC thread signals the condition variable, resuming all suspended threads.

#### 2. Deferred Reference Counting (DRC)
For objects created during compilation (such as functions, classes, code objects, and constants), standard reference counting is completely bypassed. These objects are marked as `STATE_DEFERRED` or `STATE_IMMORTAL` inside `ob_ref_shared`.
*   **Immortal Objects**: Reference count updates are completely skipped. They are never deallocated during the lifetime of the process.
*   **Deferred Objects**: Reference count updates are ignored at runtime. The GC takes full responsibility for tracking these objects and determining when they are no longer reachable, freeing their memory during GC cycles. This bypasses atomic reference operations on highly accessed structural components.

---
