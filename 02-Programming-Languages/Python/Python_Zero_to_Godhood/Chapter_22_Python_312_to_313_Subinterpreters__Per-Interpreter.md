# Python 3.12 to 3.13: Subinterpreters & Per-Interpreter GIL Parallelism


### 22.1 Isolation of Interpreter State (PEP 684)
Before Python 3.12, multi-threaded concurrency in CPython was constrained by a single process-wide Global Interpreter Lock (GIL). Although subinterpreters could be spawned via the C-API, they all shared this single GIL and process-wide global structures, preventing them from running concurrently across multiple CPU cores. 

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

#### 2. The `PyInterpreterState` Structure and Refactoring
To achieve GIL isolation, CPython moved all state variables out of C-static/global variables into fields nested inside the `PyInterpreterState` structure. This structure is defined in CPython's internal headers (`Include/internal/pycore_interp.h`):

```c
struct _is {
    struct _is *next;                 /* Linkage for global list of interpreters */
    struct _ceval_state ceval;        /* Per-interpreter evaluation loop configuration and GIL */
    struct _gc_state gc;              /* Per-interpreter garbage collector generations */
    
    /* Private Heaps and Object Storage */
    PyObject *modules;                /* Isolated dictionary containing loaded modules (sys.modules) */
    PyObject *sysdict;                /* Isolated sys module context variables */
    PyObject *builtins;               /* Builtins dictionary context */
    
    /* Execution Contexts */
    struct _dict_state dict_state;    /* Per-interpreter dictionary structures and caching */
    struct _types_state types;        /* Per-interpreter type caches and static type tables */
    
    int64_t id;                       /* Unique identifier for the subinterpreter */
    int gil_status;                   /* Configuration representing if this interpreter owns a GIL */
};
typedef struct _is PyInterpreterState;
```

*   `struct _ceval_state ceval`: Wraps the evaluation loop structures, including the interpreter's private Global Interpreter Lock (`ceval.gil`). This lock is completely isolated from other interpreters.
*   `struct _gc_state gc`: Isolates the generational lists and flags of the cyclic Garbage Collector. A collection run in one subinterpreter does not block or stop other running interpreters.
*   `PyObject *modules`: Isolates module namespaces. When subinterpreter 1 executes `import sys`, it resolves to a different dictionary instance than the `sys` module in subinterpreter 2.

---

### 22.2 The C-API Subinterpreter Initialization Configuration
From a C extension, subinterpreters can be configured and created using the Python C-API. In Python 3.12+, the runtime exposes structure configurations that specify whether the newly created interpreter should share the GIL or initialize its own:

```c
/* C-API Example: Creating an Isolated Subinterpreter with its own GIL */
#include <Python.h>

void execute_in_subinterpreter() {
    // 1. Define configuration for isolated subinterpreter
    PyInterpreterConfig config = {
        .use_main_obmalloc = 0,      /* Use separate mimalloc heap allocations */
        .allow_fork = 0,             /* Disallow fork operations for safety */
        .allow_exec = 0,             /* Disallow exec operations */
        .allow_threads = 1,          /* Enable threading within subinterpreter */
        .allow_daemon_threads = 0,
        .check_multi_interp_extensions = 1, /* Enforce strict multi-interpreter extension isolation */
        .gil = PyInterpreterConfig_OWN_GIL  /* REQUEST PRIVATE PER-INTERPRETER GIL */
    };
    
    PyThreadState *sub_tstate = NULL;
    PyStatus status = Py_NewInterpreterFromConfig(&sub_tstate, &config);
    
    if (PyStatus_Exception(status)) {
        fprintf(stderr, "Failed to initialize subinterpreter.\n");
        return;
    }
    
    // The current OS thread now holds the GIL for the new subinterpreter.
    // We can execute code inside this isolated context:
    PyRun_SimpleString("import sys; print('Subinterpreter ID:', sys.getsizeof(sys.modules))");
    
    // 2. Shut down the subinterpreter and release the private GIL
    Py_EndInterpreter(sub_tstate);
}
```

---

### 22.3 Cross-Interpreter Communication Channels
Because subinterpreters run on separate heaps with private reference counting states, passing raw object pointers (`PyObject*`) between them is strictly forbidden. Sharing pointers would lead to race conditions when multiple threads increment or decrement reference counts concurrently.

To pass data, the runtime uses a **shared-nothing** message passing architecture, serializing objects across isolation boundaries.

```
+---------------------------------------+
|           Subinterpreter 0            |
|  [PyObject (dict)] --> serialize      |
+---------------------------------------+
                            |
                            v (Isolated C Memory Buffer)
+---------------------------------------+
|        Process Global Queue           |
|            [Data Buffer]              |
+---------------------------------------+
                            |
                            v (Read and copy)
+---------------------------------------+
|           Subinterpreter 1            |
|  deserialize --> [PyObject (dict)]    |
+---------------------------------------+
```

1.  **Serialization**: The sender interpreter extracts the data. It uses serialization protocols (such as `pickle` or a specialized C-level marshal mechanism) to copy the object's value into a raw, C-native memory buffer.
2.  **Transmission**: The buffer is queued in a thread-safe global memory channel that belongs to the process-global space (external to any specific interpreter heap).
3.  **Deserialization**: The receiving interpreter pulls the buffer, decodes the values, and allocates new objects representing the data on its own heap.

#### Parallel Computation Code Example
In Python 3.12+, you can interact with subinterpreters using the experimental `_xxsubinterpreters` module. Below is a complete script demonstrating true multi-core parallel computation by distributing work across subinterpreters:

```python
import threading
import time
import _xxsubinterpreters as interpreters

# Worker script to execute inside the subinterpreter
worker_script = """
import time
import _xxsubinterpreters as interpreters

# Get the channel to send results back
channel_id = interpreters.get_current_channel()
# Perform some CPU-heavy computation
result = sum(i * i for i in range(10_000_000))

# Send the calculation result back to the main interpreter
interpreters.channel_send(channel_id, result)
"""

def run_worker(interp_id, channel_id):
    # Bind the current thread to the subinterpreter and run the computation
    interpreters.run_string(interp_id, worker_script, shared={'channel_id': channel_id})

def main():
    # 1. Create a channel for communication
    channel_id = interpreters.channel_create()
    
    # 2. Create an isolated subinterpreter (with its own GIL by default in 3.12+)
    interp_id = interpreters.create()
    
    # 3. Spawn a separate OS thread to execute the subinterpreter concurrently
    thread = threading.Thread(target=run_worker, args=(interp_id, channel_id))
    
    print("Spawning worker thread...")
    start_time = time.perf_counter()
    thread.start()
    
    # The main interpreter runs in parallel on the primary thread
    main_result = sum(i * i for i in range(5_000_000))
    print("Main interpreter calculation complete:", main_result)
    
    # 4. Wait for the subinterpreter to send its result and join the thread
    sub_result = interpreters.channel_recv(channel_id)
    thread.join()
    
    duration = time.perf_counter() - start_time
    print(f"Subinterpreter calculation complete: {sub_result}")
    print(f"Total parallel execution duration: {duration:.4f} seconds")
    
    # 5. Destroy the subinterpreter and clean up channels
    interpreters.destroy(interp_id)
    interpreters.channel_destroy(channel_id)

if __name__ == '__main__':
    main()
```

---

