# FILE I/O AND EXCEPTION FLOWS


### 6.1 Low-Level File Management
Python's file interfaces wrap standard POSIX OS calls (`open`, `read`, `write`, `close`).
*   **The Layered I/O System**: The built-in `open()` function constructs a stack of I/O objects:
    1.  `RawIOBase` (`FileIO`): A thin C wrapper around the POSIX OS file descriptor.
    2.  `BufferedIOBase` (`BufferedReader`/`BufferedWriter`): Buffers data blocks in memory (typically in 8KB chunks) to minimize expensive system call context switches.
    3.  `TextIOBase` (`TextIOWrapper`): Encodes and decodes raw bytes to Unicode strings.

```
Text Layer:     [ TextIOWrapper (Handles Unicode encodings) ]
                      |
Buffered Layer: [ BufferedReader / BufferedWriter (8KB Memory Buffer) ]
                      |
Raw OS Layer:   [ FileIO (Wraps raw POSIX OS File Descriptor) ]
```

### 6.2 Exception Mechanics & Runtime Unwinding
When an exception occurs, CPython halts normal execution flow and unwinds the call stack:
1.  **Exception Allocation**: CPython allocates an exception instance (subclass of `BaseException`) and sets active thread state variables (`_PyErr_StackItem`).
2.  **The Block Stack**: Each execution frame maintains a block stack. Bytecodes like `try` compile to a `SETUP_FINALLY` instruction, which pushes the handler's bytecode address onto the frame's block stack.
3.  **Unwinding the Stack**:
    - When an error is raised, CPython pops blocks off the current frame's block stack.
    - If it finds a `handler` address, the VM jumps to that address to run the exception handling bytecode.
    - If the current frame's block stack is exhausted, CPython unwinds the stack: it pops the current frame off the call stack, creates a traceback object (`PyTracebackObject`), and raises the exception in the caller's frame.
    - This continues until a handler catches the exception, or the program exits with an unhandled traceback.

```
Exception Unwinding Path:
[ Exception Raised ]
       |
       v
[ Frame Block Stack has handler? ]
       |--Yes--> [ Pop block, Jump to handler address ]
       | No
       v
[ Frame Call Stack empty? ]
       |--Yes--> [ Print traceback to stderr, Terminate program ]
       | No
       v
[ Pop frame, Allocate PyTracebackObject, Repeat in caller frame ]
```

---

