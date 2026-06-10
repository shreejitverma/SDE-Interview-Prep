# Advanced Concurrency: Shared Memory and Proxies


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
