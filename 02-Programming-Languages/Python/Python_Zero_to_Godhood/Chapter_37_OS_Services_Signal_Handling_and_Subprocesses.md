# OS Services, Signal Handling, and Subprocesses


Python is widely used for system administration and process orchestration because it provides a near-one-to-one mapping to operating system primitives while managing the complexity of cross-platform differences.

### 37.1 `os`: The System Call Bridge

The `os` module is the primary interface to the OS.
*   **System Calls**: Most functions in `os` are thin wrappers around C standard library calls (`open`, `read`, `write`, `fork`, `exec`).
*   **Environment**: `os.environ` is a mapping object that syncs with the process's environment block.
*   **Filesystem**: Provides low-level manipulation of file descriptors (`os.dup`, `os.pipe`) and path metadata (`os.stat`).

### 37.2 `io`: The Stream Hierarchy

The `io` module provides the foundations for Python's file handling. It uses a tiered architecture:
1.  **Raw I/O**: `FileIO` objects represent raw OS file descriptors. They perform unbuffered system calls.
2.  **Buffered I/O**: `BufferedReader`, `BufferedWriter`, and `BufferedRandom`. These maintain internal C-level buffers to minimize the number of expensive system calls.
3.  **Text I/O**: `TextIOWrapper` handles encoding and decoding (e.g., UTF-8 to Unicode) on top of a buffered binary stream.

### 37.3 `signal`: Asynchronous Event Handling

Signals are software interrupts sent to a process. Handling them in a virtual machine like CPython is complex.

#### 1. The Main Thread Restriction
In Python, signals are always received by the main thread, regardless of which thread was executing when the signal arrived.
*   **Internals**: When a signal arrives, the OS interrupts the process. The C-level signal handler in CPython sets a flag.
*   **The Check**: The evaluation loop (`ceval.c`) periodically checks this flag. If set, it invokes the Python-level signal handler. This ensures that Python code only runs at "safe" points where the VM state is consistent.

#### 2. `signal.set_wakeup_fd`
For integration with event loops (like `asyncio`), `set_wakeup_fd` writes a byte to a file descriptor whenever a signal is received, allowing a `select()` or `poll()` call to wake up and handle the signal.

### 37.4 `subprocess`: Orchestrating External Processes

The `subprocess` module is the modern replacement for `os.system` and `os.spawn`.

#### 1. Process Creation
*   **POSIX**: Uses `fork()` and `exec()`. Between the fork and exec, it handles closing file descriptors and setting up pipes.
*   **Windows**: Uses the `CreateProcess()` API.

#### 2. Pipe Multiplexing
`subprocess.communicate()` reads data from `stdout` and `stderr` while writing to `stdin`.
*   **Deadlock Prevention**: It uses `selectors` (or threads on Windows) to read from multiple pipes simultaneously. This prevents the "buffer full" deadlock that occurs if you try to read from one pipe while the process is blocked trying to write to another.

#### 3. `shlex`: Safe Command Parsing
When building command strings, always use `shlex.split()` to ensure that arguments with spaces or special characters are handled correctly, preventing shell injection vulnerabilities.

---


---