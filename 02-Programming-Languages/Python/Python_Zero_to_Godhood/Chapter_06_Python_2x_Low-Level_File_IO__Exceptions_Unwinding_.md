# Python 2.x: Low-Level File I/O & Exceptions Unwinding Blocks


### 6.1 Low-Level File Management and Standard Streams wrapping
Under Python 2.x, the built-in `file` type and `open()` function were thin wrappers around standard C stdio library streams (`FILE *`).

#### 1. The CPython 2.x `PyFileObject` Layout
Let's inspect the `PyFileObject` C structure defined in CPython 2.x's `Include/fileobject.h`:
```c
typedef struct {
    PyObject_HEAD
    FILE *f_fp;                 /* Pointer to standard C stdio stream */
    PyObject *f_name;           /* File name string object */
    PyObject *f_mode;           /* File mode string object */
    int (*f_close)(FILE *);     /* Close wrapper function */
    int f_softspace;            /* Print statement space tracking state */
    int f_binary;               /* Binary mode flag indicator */
    PyObject *f_encoding;       /* File encoding string */
    PyObject *f_errors;         /* Error handling mode */
    PyObject *f_newlines;       /* Decoded newlines object */
} PyFileObject;
```

#### 2. The `f_softspace` Printing Flag
In Python 2.x, the print statement was keyword-based rather than a function. The runtime tracked layout separation using the `f_softspace` slot inside the standard output stream:
* When a print statement completed printing an item, it evaluated the next token. If a trailing comma was present (e.g., `print x,`), the VM set `f_softspace = 1` and omitted the newline character.
* During the subsequent print operation, CPython checked `f_softspace`. If it was `1`, it wrote a space to the file stream before printing the next element and reset `f_softspace = 0`.

#### 3. Redundant Locking Bottlenecks under the GIL
Because Python 2.x wrapped standard C `FILE *` streams, I/O operations were subject to the underlying C library's internal buffering and synchronization locking. 
* Standard C libraries acquire internal mutex locks per read/write call to ensure multi-threaded safety.
* In CPython, since the Global Interpreter Lock (GIL) already guarantees single-threaded interpreter execution, these nested C stdio locks were redundant, introducing unnecessary CPU context-switching overhead and locking bottlenecks during high-frequency concurrent file operations.

#### 4. The Layered I/O System (PEP 3116)
To eliminate stdio bottlenecks, Python 3.0 replaced the old stream-based system with a layered I/O architecture backported to Python 2.6+. This system interacts directly with OS file descriptors via system calls (e.g., `read(2)`, `write(2)`), bypassing C's stdio library:
1. **Raw Layer (`RawIOBase` / `FileIO`)**: A thin wrapper over the POSIX OS file descriptor, executing raw system-level reads and writes.
2. **Buffered Layer (`BufferedIOBase` / `BufferedReader` / `BufferedWriter`)**: Maintains an internal memory block (typically 8KB) to minimize context switches between user space and kernel space.
3. **Text Layer (`TextIOBase` / `TextIOWrapper`)**: Handles encoding/decoding and system-specific universal newline translations.

---

### 6.2 Exception Mechanics and the Thread State slots
CPython manages exceptions at the thread level, storing active and caught exception states inside the execution thread's state structure.

#### 1. Exception Slots in `PyThreadState`
CPython maintains exception metadata on a per-thread basis within the `PyThreadState` struct (`Include/pystate.h`):
```c
typedef struct _ts {
    struct _ts *next;
    PyInterpreterState *interp;
    struct _frame *frame;       /* Active execution frame */
    
    /* Exception currently propagating */
    PyObject *curexc_type;
    PyObject *curexc_value;
    PyObject *curexc_traceback;

    /* Exception currently caught and handled */
    PyObject *exc_type;
    PyObject *exc_value;
    PyObject *exc_traceback;
} PyThreadState;
```

* **`curexc_*` (Current Exception)**: The active exception currently propagating. When an instruction fails (e.g., division by zero), CPython sets these pointers and returns `NULL` up the C execution chain.
* **`exc_*` (Caught Exception)**: The exception currently being handled inside an active `except` block. This keeps the exception accessible via `sys.exc_info()` during nesting, while preventing active propagation from overwriting the caught state.

#### 2. The String Exception Era
In early Python versions (pre-2.6), exceptions were not required to be class instances; you could raise plain string literals:
```python
# Pre-Python 2.6 Behavior
raise "DatabaseConnectionFailed"
```
During lookup, the VM evaluated string exceptions by identity (`is` check) rather than class inheritance. This made hierarchy categorization and error composition difficult. Python 2.6 deprecated and removed string exceptions, requiring all exceptions to inherit from the built-in base type `BaseException`.

---

### 6.3 The Try-Except-Finally Block Stack and Stack Restoration
CPython handles structured control flow (loops, exception blocks) using a static **block stack** located inside each execution frame (`PyFrameObject`).

#### 1. The `PyTryBlock` Structure
The block stack consists of up to 20 (`CO_MAXBLOCKS`) `PyTryBlock` structs:
```c
typedef struct {
    int b_type;                 /* Block type: SETUP_LOOP, SETUP_EXCEPT, SETUP_FINALLY */
    int b_handler;              /* Target instruction offset of the handler */
    int b_level;                /* Stack pointer depth at block entry */
} PyTryBlock;
```

#### 2. VM Exception Unwinding Flow
When an instruction returns `NULL`, the VM loop (`_PyEval_EvalFrameDefault`) enters unwinding mode:

```
[Exception Raised: C API returns NULL]
                  |
                  v
[Is frame->f_blockstack empty?]
        |             |
       Yes            No
        |             |
        v             v
[Pop Frame]     [Pop Try Block]
[Traceback]           |
        |       [Is block SETUP_EXCEPT or SETUP_FINALLY?]
        v             |                  |
[Repeat in            No                Yes
 caller]              |                  |
                      v                  v
                [Continue loop]   1. Restore stack pointer: stack_pointer = frame->b_level
                                  2. Push traceback, value, type onto stack
                                  3. Set PC: frame->f_lasti = block->b_handler
                                  4. Resume bytecode execution in handler
```

1. **Stack Cleanup via `b_level`**: The VM pops the current try block. It immediately restores the frame's evaluation stack pointer back to the offset saved in `b_level`. This discards any unused variables or intermediate states created inside the `try` block, preventing memory leaks.
2. **Context Setup**: The VM pushes the exception's `traceback`, `value`, and `type` (in Python 2.x) onto the evaluation stack.
4. **Frame Traversal fallback**: If the frame's block stack is exhausted without finding a handler, CPython pops the frame, instantiates a `PyTracebackObject` mapping the exception to the current line number, and continues unwinding in the caller's frame (`frame->f_back`).

---

