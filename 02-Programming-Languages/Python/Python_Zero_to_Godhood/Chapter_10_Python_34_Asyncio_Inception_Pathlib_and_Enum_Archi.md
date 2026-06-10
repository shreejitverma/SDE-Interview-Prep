# Python 3.4: Asyncio Inception, Pathlib, and Enum Architectures


### 10.1 Asyncio Inception: Generators and Event Loops (PEP 3156)

#### 1. Generator-Based Coroutine Syntax
Prior to Python 3.5's native `async/await` syntax, asyncio coroutines were defined using generators decorated with `@asyncio.coroutine` and delegated execution using `yield from`:
```python
import asyncio

@asyncio.coroutine
def fetch_data():
    yield from asyncio.sleep(1)  /* Delegate to asyncio.sleep generator */
    return "payload_data"
```

#### 2. The Event Loop and Selector Multiplexing
Under the hood, `asyncio` does not use multi-threading to achieve concurrency. Instead, it runs an **Event Loop** on a single thread. The event loop manages a collection of tasks and schedules them using operating system-level I/O multiplexing.
* **OS-Level Selector**: The event loop wraps Python's built-in `selectors` module, which maps to POSIX system calls: `select()`, `poll()`, Linux `epoll()`, or macOS `kqueue()`.
* **Multiplexing Cycle**:
  1. The event loop registers sockets or file descriptors with the OS selector, specifying the events to monitor (e.g., readability or writability) and registering associated callbacks.
  2. The loop enters a blocking poll state, calling `selector.select()`. The OS blocks the thread until one or more registered file descriptors become ready.
  3. When an event triggers, the OS returns the list of ready descriptors. The event loop iterates over this list and schedules the registered callbacks for immediate execution.

#### 3. Future and Task Execution Cycle
The event loop schedules and executes coroutines using two core structures:
* **`Future`**: Represents the eventual result of an asynchronous operation. It maintains an internal state (PENDING, FINISHED, CANCELLED) and stores a list of callbacks to invoke upon completion.
* **`Task`**: A subclass of `Future` that wraps a coroutine. It acts as the bridge between the event loop and the coroutine's execution.

##### The Step-by-Step Task Loop:
1. **Task Initialization**: The event loop schedules the `Task`. It invokes the task's `_step` method, which calls `coroutine.send(None)` to start the coroutine.
2. **Suspension**: The coroutine runs until it encounters a blocking operation: `yield from future`. It yields control back to the task, returning the pending `Future` object.
3. **Callback Registration**: The `Task` receives this pending future. It calls `future.add_done_callback(task._step)` to register its own step method as a callback on the future, then yields control back to the event loop.
4. **Polling**: The event loop performs other work.
5. **Resuming**: When the I/O event completes, the selector changes the future's state to FINISHED. The future pops and schedules all registered callbacks. The task's `_step` method executes, calling `coroutine.send(result)` to resume execution inside the coroutine.

---

### 10.2 `pathlib`: Object-Oriented Filesystem Paths (PEP 428)
Before Python 3.4, filesystem paths were treated as raw strings, requiring developers to write platform-specific path manipulation code using the `os.path` module (e.g., `os.path.join()`, `os.path.split()`). PEP 428 introduced `pathlib` to represent paths as structured objects.

#### 1. Class Hierarchy
`pathlib` splits path representations into a clear class hierarchy:
```
                        [ PurePath ] (Abstract Base / Path Math Only)
                       /            \
           [ PurePosixPath ]    [ PureWindowsPath ]
                  |                      |
             [ PosixPath ]       [ WindowsPath ]
                       \            /
                        \-[ Path ]-/ (Instantiates PosixPath or WindowsPath dynamically)
```

* **`PurePath`**: Provides pure path manipulations (string parsing, joining, metadata extraction) without executing OS-level system calls. It can be instantiated on any system (e.g., you can parse Windows paths on a Linux host using `PureWindowsPath`).
* **`Path`**: Inherits from `PurePath` and adds OS-level filesystem queries (like `.exists()`, `.glob()`, `.mkdir()`, and `.read_text()`).
* **Dynamic Instantiation**: When `Path()` is instantiated at runtime, CPython detects the host operating system and dynamically returns an instance of either `PosixPath` or `WindowsPath`.

#### 2. Path Concatenation Operator Overloading
`pathlib` overloads the division operator (`/`) using the special method `__truediv__` to implement intuitive path joining:
```python
from pathlib import Path
base_path = Path("/var")
log_path = base_path / "log" / "nginx.log"
```

Under the hood:
1. When `base_path / "log"` is evaluated, CPython calls `base_path.__truediv__("log")`.
2. The `__truediv__` method parses the argument, checks compatibility, joins the string segments using the host operating system's path separator (`/` or `\`), and returns a new `Path` object containing the joined path string, avoiding redundant string allocations.

---

### 10.3 `Enum`: Metaclass and Singletons (PEP 435)
PEP 435 introduced `Enum` to the standard library to support structured enumerations.

#### 1. Metaclass Construction: `EnumMeta`
Enums are created using the custom metaclass `EnumMeta`. When the compiler processes an `Enum` class definition:
1. `EnumMeta.__prepare__()` returns a custom dictionary structure (`_EnumDict`) that tracks insertion order and raises errors if a duplicate member name is defined.
2. `EnumMeta.__new__()` parses the class namespace, separating attributes into Enum members and standard methods.
3. For each Enum member (e.g., `RED = 1`), `EnumMeta` instantiates a singleton instance of the Enum class, assigning the name (`RED`) and value (`1`) to the instance.
4. The metaclass updates the class dictionary, replacing the raw integer literals with the instantiated singleton object references.

#### 2. Immuntability and Protections
To ensure Enums act as stable constant mappings:
* **Member Immutability**: `Enum` overrides `__setattr__` and `__delattr__` to block any attempt to modify or delete a member's value at runtime:
  ```python
  class Color(Enum):
      RED = 1

  # This raises an AttributeError
  Color.RED.value = 2
  ```
* **Iterability**: `EnumMeta` implements `__iter__`, allowing developers to iterate over enum members in declaration order.
* **Lookup**: It implements `__getitem__` (lookup by name, e.g., `Color['RED']`) and `__call__` (lookup by value, e.g., `Color(1)`).

---
