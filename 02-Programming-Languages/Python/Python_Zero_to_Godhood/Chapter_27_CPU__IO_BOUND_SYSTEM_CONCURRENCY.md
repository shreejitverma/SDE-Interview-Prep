# CPU & I/O BOUND SYSTEM CONCURRENCY


### 27.1 Concurrency Paradigms: Threads vs. Processes vs. Coroutines (Asyncio)
Python supports three core concurrency paradigms, each targeting different system limitations. The table below outlines their architectural differences:

| Paradigm | Execution Type | GIL Limitation | Primary Use Case | Overhead | Communication Method |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Multithreading** (`threading`) | Preemptive multitasking (managed by OS). | Yes (standard CPython blocks CPU scaling). | I/O-bound tasks (waiting for sockets, files). | Medium (thread stacks, context switches). | Shared memory (requires locks, mutexes). |
| **Multiprocessing** (`multiprocessing`) | Preemptive parallel execution (separate OS processes). | No (each process has its own GIL/heap). | CPU-bound computations (math, data parsing). | High (process fork/spawn cost, private heaps). | IPC (Pipes, queues, shared memory, pickles). |
| **Asynchronous** (`asyncio` / coroutines) | Cooperative multitasking (single thread, explicit yields). | Yes (single-threaded). | High-concurrency network services (web servers). | Low (coroutine objects are lightweight heap structures). | Direct variable access (no locks required). |

---

### 27.2 Asyncio Event Loops and `uvloop` Internals
Python's standard `asyncio` event loop uses select-based system calls (`selectors` module, e.g. `epoll` on Linux, `kqueue` on macOS) to track file descriptors. While functional, the default implementation is written in pure Python and introduces overhead when wrapping callback schedules.

#### 1. uvloop Optimization
`uvloop` is a drop-in replacement for the default asyncio event loop. Written in Cython and built on top of **libuv** (the high-performance asynchronous I/O engine powering Node.js), it replaces CPython's loop implementation with C-level structures.

```
CPython Default asyncio Loop:
[Asyncio Code] ---> [Pure Python Event Loop] ---> [selectors (epoll/kqueue)]

uvloop Event Loop:
[Asyncio Code] ---> [Cython Wrapper] ---> [libuv (C-native epoll/kqueue)]
```

Libuv optimizes execution paths by:
*   **System Call Reduction**: Batching read/write operations to minimize transitions between user space and kernel space.
*   **Direct Memory Buffers**: Allocating internal buffers directly in C, avoiding intermediate Python byte allocations.
*   **Zero-Overhead Timers**: Implementing timer queues using binary heaps at the C level.

#### 2. Configuring uvloop in Python
To use `uvloop`, register it as the default event loop policy at the entry point of your application:

```python
import asyncio
import sys

# Register uvloop policy on supported platforms
if sys.platform != 'win32':
    import uvloop
    asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())

async def main():
    print("Running on optimized uvloop engine.")

if __name__ == '__main__':
    asyncio.run(main())
```

With `uvloop`, network throughput can increase by 2x to 4x, reaching speeds comparable to implementations in Go or Node.js.

---

### 27.3 Multiprocessing Shared Memory IPC
Standard multiprocessing in Python relies on **Pipes** and **Queues** for Inter-Process Communication (IPC). When a process sends an object to another process:
1.  The sender **pickles** (serializes) the object into bytes.
2.  The bytes are written to an OS socket or pipe.
3.  The receiver reads the bytes and **unpickles** (deserializes) them to allocate new objects on its private heap.

For large data structures (like lists of floats or images), this serialization pipeline degrades performance.

#### 1. Shared Memory Architecture
Python 3.8 introduced the `multiprocessing.shared_memory` module, which maps a block of virtual memory across the address spaces of multiple OS processes. Both processes can read and write to the same physical RAM block directly, avoiding serialization overhead.

```
Process 1 (Address Space)               Process 2 (Address Space)
  +--------------------+                  +--------------------+
  | Virtual Memory     |                  | Virtual Memory     |
  |  [Mapped Segment] -+----+        +----+-- [Mapped Segment] |
  +--------------------+    |        |    +--------------------+
                            v        v
                      +--------------------+
                      |    Physical RAM    |
                      |   [Shared Block]   |
                      +--------------------+
```

#### 2. Robust Shared Memory Code Example
Below is a complete script demonstrating parent and child processes communicating using shared memory:

```python
import time
from multiprocessing import Process
from multiprocessing.shared_memory import SharedMemory

def child_process_worker(shm_name):
    # 1. Attach to existing shared memory block using unique name
    existing_shm = SharedMemory(name=shm_name)
    
    # 2. Access buffer as a memoryview array slice
    buffer = existing_shm.buf
    
    print(f"[Child] Connected to shared memory. Current contents: {bytes(buffer[:10])}")
    
    # Modify data in place (zero-copy modification)
    for i in range(10):
        buffer[i] = ord('A') + i
        
    print(f"[Child] Modified buffer contents in place.")
    
    # 3. Clean up the shared memory handle
    existing_shm.close()

def main():
    # 1. Allocate a shared memory block of 1024 bytes
    shm = SharedMemory(create=True, size=1024)
    shm_name = shm.name
    print(f"[Parent] Allocated Shared Memory block named: {shm_name}")
    
    # Initialize buffer contents
    shm.buf[:10] = b"0123456789"
    print(f"[Parent] Initial buffer contents: {bytes(shm.buf[:10])}")
    
    # 2. Spawn a child process, passing the shared memory block name
    p = Process(target=child_process_worker, args=(shm_name,))
    p.start()
    p.join()  # Wait for child process to finish writing
    
    # 3. Parent reads the updated data immediately without IPC serialization
    print(f"[Parent] Updated buffer contents read by Parent: {bytes(shm.buf[:10])}")
    
    # 4. Release and destroy shared memory block
    shm.close()
    shm.unlink()  # Instructs OS to free the shared memory resource

if __name__ == '__main__':
    main()
```

---