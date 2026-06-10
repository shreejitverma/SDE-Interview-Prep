# Chapter 66: Advanced Concurrency: Shared Memory and Proxies

Building on Chapter 27, this chapter explores the high-performance communication mechanisms required for massive scale data processing in Python.

### 66.1 `multiprocessing.shared_memory`: Zero-Copy Communication

Prior to Python 3.8, `multiprocessing` relied on pickling objects and sending them via pipes/sockets, which was slow for large arrays. `shared_memory` provides a way to allocate raw memory that can be accessed by multiple processes without copying.

#### 1. The `SharedMemory` Object
*   **Creation**: One process creates the memory block with a unique name.
*   **Attachment**: Other processes "attach" to the memory using the name.
*   **Internals**: On POSIX, this uses `shm_open()` and `mmap()`. On Windows, it uses `CreateFileMapping()`.

#### 2. `ShareableList` and `ndarray` Integration
You can wrap a `SharedMemory` block in a `ShareableList` (for basic types) or use it as the buffer for a NumPy array:
```python
from multiprocessing import shared_memory
import numpy as np

# Creator
shm = shared_memory.SharedMemory(create=True, size=1024)
arr = np.ndarray((128,), dtype=np.int64, buffer=shm.buf)
arr[:] = np.arange(128)

# Consumer (in another process)
existing_shm = shared_memory.SharedMemory(name=shm.name)
arr_copy = np.ndarray((128,), dtype=np.int64, buffer=existing_shm.buf)
print(arr_copy[10]) # Output: 10
```

### 66.2 Managers and Proxies: Distributed Objects

The `multiprocessing.Manager` allows you to share complex Python objects (like dicts or custom classes) across processes using a server-client architecture.
*   **The Server Process**: A hidden process manages the "real" objects.
*   **Proxies**: Worker processes receive "Proxy" objects that look like the real thing but send every method call over a socket to the server process.
*   **Performance Note**: While flexible, proxies are much slower than shared memory because every access involves a network/IPC round-trip and synchronization.

---

# Chapter 67: The Typing System: Static Analysis vs. Runtime Enforcement

Python's type system has evolved from simple comments to a sophisticated language-level feature. This chapter deconstructs how types exist in the runtime.

### 67.1 `typing` Internals: The `GenericAlias` and `SpecialForm`

When you write `list[int]`, you are creating a `types.GenericAlias` object.
*   **The `__getitem__` Hook**: Classes like `list` or `dict` implement `__class_getitem__` to support the bracket syntax.
*   **Runtime Overhead**: Type hints are evaluated at import time. Large-scale use of complex nested types can noticeably slow down the startup of a Python application.

### 67.2 Static vs. Runtime Verification

*   **Static Analysis**: Tools like `mypy` or `pyright` scan the AST (Chapter 31) and verify types without running the code.
*   **Runtime Enforcement**: Libraries like `pydantic` or `beartype` intercept function calls or class instantiation to verify types at execution time.
*   **`inspect.get_type_hints()`**: This function is the "Godhood" way to retrieve types at runtime, handling forward references (strings like `"MyClass"`) by evaluating them in the correct namespace.

### 67.3 Protocols and Structural Subtyping (PEP 544)

Protocols allow for "static duck typing."
*   **Internals**: A `Protocol` class uses a specialized metaclass that identifies which methods define the interface. Unlike `abc.ABC`, you don't need to inherit from the Protocol; you just need to implement the methods.

---

# Chapter 68: The Python Packaging Ecosystem: PEP 517 to Wheels

Understanding how Python code is distributed is essential for senior engineering.

### 68.1 The Evolution of Installation

1.  **Legacy (`setup.py install`)**: Executed a script that performed arbitrary actions. This was insecure and non-reproducible.
2.  **Modern (PEP 517/518)**: Decouples the build backend (e.g., `setuptools`, `flit`, `poetry`) from the frontend (`pip`).
*   **`pyproject.toml`**: The source of truth for build requirements.
*   **Build Isolation**: `pip` creates a temporary virtual environment to build your package, ensuring that build dependencies don't pollute your system.

### 68.2 The Wheel Format (PEP 427)

A "Wheel" (`.whl`) is a built distribution format.
*   **Internals**: It is a ZIP file (Chapter 50) containing the code and a `.dist-info` directory with metadata (dependencies, entry points).
*   **Platform Tags**: Wheels for C extensions include tags like `manylinux2014_x86_64` to specify exactly which OS and architecture they are compatible with, avoiding the need for the end-user to have a C compiler installed.

---
