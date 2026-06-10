# PROFILING, BENCHMARKING, AND DIAGNOSTICS


### 29.1 Deterministic Profiling (`cProfile`) vs. Sampling Profiling (`py-spy`)
Identifying execution bottlenecks is the first step toward optimization. Python developers must choose between two primary profiling methodologies depending on the target environment.

#### 1. Deterministic Profiling via `cProfile`
`cProfile` is a built-in module that provides **deterministic profiling**. It monitors every function call, function return, and exception raise event in the program.

Under the hood, `cProfile` registers a profile hook at the C level using CPython's execution frame hooks (similar to `sys.setprofile`):

```
[CPython VM Evaluator]
          |
          +---> Triggers Hook ---> [cProfile Event handler] (Logs timestamp)
          |
   [Execute Bytecode]
          |
          +---> Triggers Hook ---> [cProfile Event handler] (Calculates duration)
```

##### Advantages
*   **Exact Counts**: Provides exact call counts for every function in the execution tree.
*   **Granular Statistics**: Tracks exact cumulative time spent inside a function vs. time spent in its child calls.

##### Disadvantages
*   **High Overhead**: Hooking into every call/return introduces significant execution overhead (typically slowing programs down by 2x to 10x).
*   **Measurement Distortion**: For fast helper functions or recursion loops, the execution cost of the profiler hook itself can be larger than the function being profiled, distorting the final statistics.

#### 2. Sampling Profiling via `py-spy`
`py-spy` is an out-of-process sampling profiler written in Rust. Instead of intercepting the executing bytecode, it queries the operating system to read the virtual memory pages of the target Python process directly (using syscalls like `process_vm_readv` on Linux, `vm_read` on macOS, or `ReadProcessMemory` on Windows).

```
+------------------------------------+      +------------------------------------+
|          Python Process            |      |          py-spy Process            |
|  [_PyRuntime / PyThreadState]      | <=== |  process_vm_readv()                |
|  - Active Frame stack pointers     |      |  - Resolves C pointers             |
|  - Executing bytecode indices      |      |  - Renders flamegraph in real time |
+------------------------------------+      +------------------------------------+
```

At regular intervals (e.g. 100 times per second), `py-spy` performs the following steps:
1.  Locates CPython's global runtime structure (`_PyRuntime` or the active `PyThreadState`).
2.  Reads the thread's call stack, traversing the linked list of interpreter frames (`PyFrameObject`).
3.  Resolves the code objects and file names associated with the frames to reconstruct the Python-level stack trace.

##### Advantages
*   **Near-Zero Overhead**: Does not modify bytecode execution or trigger frame hooks, resulting in less than 1% CPU overhead.
*   **Production Safe**: Can be attached to running, high-traffic production web servers or daemons without slowing them down.

---

### 29.2 Memory Leak Diagnostics via `tracemalloc`
Memory leaks in Python usually occur when objects remain referenced inside global structures, module namespaces, or circular caches, preventing reference counting and GC from reclaiming them.

#### 1. tracemalloc Architecture
The `tracemalloc` module tracks allocations at the allocator level. When enabled, it hooks into CPython's internal `PyObject_Malloc` and memory deallocation routines.

For every block of memory allocated, `tracemalloc` stores:
*   The virtual memory address and allocation size in bytes.
*   The C-level call stack.
*   The Python-level stack trace (up to a configurable number of frames).

#### 2. Memory Leak Snapshot Script
Below is a complete script demonstrating how to capture and compare memory snapshots to isolate leaks:

```python
import tracemalloc
import time

# Start tracing memory allocations, storing up to 10 frames of stack trace
tracemalloc.start(10)

def simulate_memory_leak():
    # Helper list that holds references, preventing deallocation
    global leak_accumulator
    if 'leak_accumulator' not in globals():
        leak_accumulator = []
        
    # Allocate a large list of string objects
    leaked_data = [f"Leaked String Index {i}" for i in range(10_000)]
    leak_accumulator.append(leaked_data)

def main():
    # 1. Take initial baseline snapshot
    print("Capturing baseline memory snapshot...")
    snapshot_baseline = tracemalloc.take_snapshot()
    
    # Run operations that allocate memory and leak it
    print("Running leaky operations...")
    for _ in range(5):
        simulate_memory_leak()
        time.sleep(0.1)
        
    # 2. Take secondary snapshot after operations
    print("Capturing comparison snapshot...")
    snapshot_current = tracemalloc.take_snapshot()
    
    # 3. Compare snapshots, grouping allocations by file line number
    stats = snapshot_current.compare_to(snapshot_baseline, 'lineno')
    
    print("\n=== TOP MEMORY ALLOCATION DIFFERENCES ===")
    for stat in stats[:3]:
        print(stat)
        # Print the exact traceback line that allocated the leaked memory
        print("Traceback:")
        for frame in stat.traceback:
            print(f"  File {frame.filename}, line {frame.lineno}: {frame.line}")
        print("-" * 50)

if __name__ == '__main__':
    main()
```

---

### 29.3 Crash Debugging via `faulthandler`
When a Python program crashes due to a low-level C error (such as a segmentation fault in a C extension, stack overflow, or memory corruption), the OS immediately terminates the process with a core dump. Standard Python try-except blocks cannot catch these crashes, and Python stack traces are lost, leaving developers with only a generic `Segmentation Fault` message.

The `faulthandler` module registers signal handlers at the operating system level for critical signals:

| Signal | Description | Trigger Example |
| :--- | :--- | :--- |
| **`SIGSEGV`** | Segmentation fault. | Dereferencing a invalid or `NULL` pointer in C. |
| **`SIGFPE`** | Floating-point exception. | Division by zero or overflow at C level. |
| **`SIGBUS`** | Bus error. | Unaligned memory access or hardware fault. |
| **`SIGILL`** | Illegal instruction. | Execution of corrupted machine code stencils. |

When one of these signals is caught:
1.  The OS suspends normal process execution and invokes the handler registered by `faulthandler`.
2.  `faulthandler` safely queries CPython's current thread state (`PyThreadState_Get()`).
3.  It prints the Python call stack of all running threads directly to the standard error output (`sys.stderr`), using only async-signal-safe system calls.
4.  The process then terminates as usual.

To enable crash traceback logging at the start of your application:

```python
import faulthandler
import sys

# Enable crash reporting, directing output to stderr
faulthandler.enable(file=sys.stderr, all_threads=True)
```

Alternatively, you can enable it from the terminal without modifying code by setting the environment variable:

```bash
export PYTHONFAULTHANDLER=1
```

---