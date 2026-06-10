# CONCURRENCY MECHANICS & THE GLOBAL INTERPRETER LOCK


### 10.1 The Global Interpreter Lock (GIL) Architecture
The CPython interpreter uses a global lock called the **Global Interpreter Lock (GIL)** to ensure thread safety.
*   **Why CPython requires the GIL**: CPython's internal memory management (reference counting) and shared resources (like class and module namespaces dictionaries) are not thread-safe. Without the GIL, concurrent modifications by multiple threads could cause memory corruption.
*   **CPython Thread Model**: CPython threads are standard OS threads managed by the host operating system's scheduler. However, a thread must hold the GIL to execute Python bytecode.

```
Multi-Threading Execution under the GIL:
Thread 1: [ Acquires GIL ] --> [ Executes Bytecode ] --> [ Releases GIL ]
                                                               |
Thread 2:                       [ Waiting for GIL ] ------> [ Acquires GIL ] --> [ Executes Bytecode ]
```

### 10.2 GIL Release and Acquisition Intervals
*   **The Switch Interval**: CPython threads do not hold the GIL indefinitely. The interpreter forces the running thread to release the GIL after a set interval (configured via `sys.setswitchinterval(interval)`, defaulting to 5 milliseconds).
*   **Voluntary Release**: The GIL is automatically released during blocking operations:
    - POSIX System I/O calls (`read`, `write`, `select`).
    - Standard compression/hashing C routines.
    - Custom C/C++ extensions explicitly wrapping blocks with the macros `Py_BEGIN_ALLOW_THREADS` and `Py_END_ALLOW_THREADS`.

### 10.3 Threading vs. Multiprocessing memory layouts
*   **Threading (`threading`)**: Threads share a single CPython interpreter instance and the same virtual memory space (heap). This allows fast data sharing but restricts execution to a single core due to the GIL.
*   **Multiprocessing (`multiprocessing`)**: Spawns separate OS processes, each with its own independent CPython interpreter and memory space (with its own GIL). This bypasses the GIL to run computations in parallel across multiple CPU cores, but requires Inter-Process Communication (IPC) (like pipes, queues, or shared memory structures) to exchange data.

---
