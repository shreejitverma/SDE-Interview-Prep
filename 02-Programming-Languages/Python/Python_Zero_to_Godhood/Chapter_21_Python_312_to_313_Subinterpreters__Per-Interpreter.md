# Python 3.12 to 3.13: Subinterpreters & Per-Interpreter GIL Parallelism


### 22.1 Isolation of Interpreter State (PEP 684)
Before Python 3.12, multi-threaded concurrency was bottlenecked by the Global Interpreter Lock (GIL) because all threads shared a single global interpreter state. 
PEP 684 introduced support for **isolated subinterpreters**, each running with its own per-interpreter GIL. This configuration allows a single OS process to run multiple Python interpreters concurrently, achieving true multi-core parallel execution.

#### 1. Process Memory Layout: Shared vs. Isolated States
```
================================ OS PROCESS MEMORY ================================
|                                                                                 |
|  [Global Process State] (Loaded DLLs, static allocation, file descriptors)     |
|                                                                                 |
|  [Subinterpreter 0] (Own GIL)     [Subinterpreter 1] (Own GIL)                 |
|   - Heap: Private Objects          - Heap: Private Objects                     |
|   - Modules: Isolated Dict         - Modules: Isolated Dict                    |
|   - Garbage Collector States       - Garbage Collector States                  |
|   - PyInterpreterState Struct      - PyInterpreterState Struct                 |
|                                                                                 |
===================================================================================
```

#### 2. The `PyInterpreterState` Structure
To achieve isolation, all mutable structures have been shifted from global scopes into per-interpreter state wrappers defined in `pycore_interp.h`:

```c
struct _is {
    struct _is *next;                 /* Pointer to next subinterpreter in process */
    struct _ceval_state ceval;        /* Per-interpreter evaluation loop configuration and GIL */
    struct _gc_state gc;              /* Per-interpreter garbage collector generations */
    PyObject *modules;                /* Isolated dictionary containing loaded modules */
    PyObject *sysdict;                /* Isolated sys module context variables */
    /* ... additional fields ... */
};
typedef struct _is PyInterpreterState;
```

---

### 22.2 Inter-Interpreter Communication Channels
Because subinterpreters share no Python objects directly (to avoid reference count race conditions across distinct heaps), they cannot share pointers. Data must be passed between subinterpreters using the C-level serialization protocols of the `_xxsubinterpreters` module.

#### 1. Channel Mechanics
*   **Sender**: Serializes/marshals the Python object data into an isolated C buffer.
*   **Queue**: Pushes the serialized buffer onto an OS-level thread-safe memory queue.
*   **Receiver**: Pops the buffer from the queue, deserializes the bytes, and reconstructs a new `PyObject` structure on its private heap.

This shared-nothing execution model ensures that concurrency remains lock-free, avoiding heap synchronization bottlenecks across subinterpreters.

---

