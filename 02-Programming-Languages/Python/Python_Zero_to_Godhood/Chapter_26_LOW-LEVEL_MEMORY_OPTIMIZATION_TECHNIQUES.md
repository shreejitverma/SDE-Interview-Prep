# LOW-LEVEL MEMORY OPTIMIZATION TECHNIQUES


### 26.1 `__slots__` Internals and Memory Mapping
By default, CPython allocates a dynamic dictionary (`__dict__`) for every instance of a user-defined class. This dictionary allows users to set arbitrary attributes at runtime, but introduces significant memory overhead. For small objects, storing a hash table (requiring a minimum of 8 entries, table pointers, and tracking states) can consume several hundred bytes per instance.

To optimize memory usage, developers can declare `__slots__` inside the class definition:

```python
class OptimisedPoint:
    __slots__ = ('x', 'y')
    def __init__(self, x: float, y: float) -> None:
        self.x = x
        self.y = y
```

#### 1. Compilation and Memory Layout Comparison
When a class defines `__slots__`, CPython alters the structure allocation of its instances:
*   **Without slots**: The class instances reserve a pointer to a dynamic `__dict__` struct and a `__weakref__` pointer, occupying 16 bytes (on 64-bit systems) just for metadata pointers, in addition to the hash table memory allocated when attributes are assigned.
*   **With slots**: CPython allocates a fixed array of pointers directly inside the instance struct itself. The class namespace defines **Member Descriptors** at fixed offset indices matching the slot attributes.

```
Without __slots__:
+-----------------------------------+
|             PyObject              |
|  - ob_refcnt                      |
|  - ob_type                        |
+-----------------------------------+
|  - __dict__ pointer   ------------+---> [ PyDictObject (Hash Table) ]
+-----------------------------------+
|  - __weakref__ pointer            |
+-----------------------------------+

With __slots__:
+-----------------------------------+
|             PyObject              |
|  - ob_refcnt                      |
|  - ob_type                        |
+-----------------------------------+
|  - slot pointer 0 ('x' float)    | <-- Access via fixed offset (e.g., +16)
+-----------------------------------+
|  - slot pointer 1 ('y' float)    | <-- Access via fixed offset (e.g., +24)
+-----------------------------------+
```

#### 2. Bytecode Impact of slots
Accessing attributes on instances with slots bypasses dictionary lookups. The bytecode instructions execute via optimized offset reads:

```
# Without slots
10 LOAD_FAST           0 (self)
12 LOAD_ATTR           1 (x)  ; Performs MRO lookup and falls back to __dict__

# With slots
10 LOAD_FAST           0 (self)
12 LOAD_ATTR           1 (x)  ; Detects Member Descriptor; reads offset directly
```

Because the attribute offset is known at compile time, CPython skips the hash-calculation and hash-bucket collision checks, yielding a significant performance speedup in addition to memory reduction.

---

### 26.2 Buffer Protocol and `memoryview` Zero-Copy Slicing
In high-performance I/O or numerical applications, copying large arrays of binary data introduces CPU and memory bottlenecks. CPython's **Buffer Protocol** defines a C-level interface that allows objects to expose their raw, internal memory buffers directly to other Python components without allocating secondary copies.

#### 1. C-level `Py_buffer` Struct Definition
An object exposes its memory layout by implementing the buffer protocol and populating a `Py_buffer` struct:

```c
/* Include/object.h */
typedef struct {
    void *buf;                  /* Pointer to the start of the memory block */
    PyObject *obj;              /* Reference to the exporting object (to prevent GC reclamation) */
    Py_ssize_t len;             /* Total length of the buffer in bytes */
    Py_ssize_t itemsize;        /* Size of a single element in bytes */
    int readonly;               /* Set to 1 if the buffer is read-only */
    const char *format;         /* Format string describing the elements (struct style, e.g. "f" for float) */
    int ndim;                   /* Number of dimensions */
    Py_ssize_t *shape;          /* Array of sizes for each dimension */
    Py_ssize_t *strides;        /* Array of step offsets (strides) for each dimension */
    Py_ssize_t *suboffsets;     /* Suboffsets array (used for nested arrays) */
    void *internal;             /* Private data for allocator tracking */
} Py_buffer;
```

*   `buf`: The raw C pointer to the data.
*   `strides`: Dictates the distance in bytes to jump to reach the next element in a given dimension.

#### 2. memoryview Wrapper and Zero-Copy Slicing
A `memoryview` is a Python-level wrapper around the C-level buffer protocol. It allows Python code to perform slicing operations that do not copy data. Instead, slicing creates a new `memoryview` object sharing the same `buf` pointer with adjusted `shape`, `strides`, and `len` parameters:

```python
# Zero-copy slicing demo
raw_data = bytearray(b"Mastering CPython Memory Allocation")
mv = memoryview(raw_data)

# Slice shares the memory buffer of raw_data; no new allocations occur
mv_slice = mv[10:17]
print(mv_slice.tobytes())  # Output: b"CPython"

# Modifying the slice modifies the source object directly
mv_slice[0] = ord('c')
print(raw_data)            # Output: bytearray(b"Mastering cPython Memory Allocation")
```

---

### 26.3 Compact Serialization: array vs. struct
Standard Python lists are arrays of pointers pointing to heap-allocated `PyObject` instances scattered across virtual memory. This introduces cache-locality issues (cache misses) and substantial reference counting overhead.

#### 1. array.array
The `array` module provides a compact, contiguous data structure that stores homogeneous primitive types (integers, floats) directly in a contiguous memory block, mimicking C arrays:

```python
import array
# Allocates a contiguous block of memory containing 1 million native 32-bit floats
float_array = array.array('f', [1.0] * 1_000_000)
```

Compared to a list of floats (which requires 8 bytes for the pointer, plus 24 bytes for each float object), `array.array` consumes exactly 4 bytes per element, yielding up to an 8x memory reduction and improved cache locality during sequential iterations.

#### 2. Struct Packing
The `struct` module enables packing and unpacking Python objects to and from binary representation buffers matching C structures, which is ideal for network serialization or binary file I/O:

```python
import struct

# Layout: 32-bit Integer (i), 32-bit float (f), and 8-byte string (8s)
# Total size: 4 + 4 + 8 = 16 bytes of contiguous binary data
binary_data = struct.pack('if8s', 42, 3.14159, b'data_str')

# Unpack back to Python primitives
unpacked_vals = struct.unpack('if8s', binary_data)
print(unpacked_vals)  # Output: (42, 3.1415927410125732, b'data_str')
```

---