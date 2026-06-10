# Python 3.3: Yield From Generators and Implicit Namespace Packages


### 9.1 PEP 380: Generator Delegation via `yield from`
`yield from <iterable>` delegates generator operations directly to a sub-generator or iterable, acting as a transparent channel between the caller and the active sub-generator.

#### 1. Bytecode Compilation and Stack Transitions
When the compiler encounters `yield from`, it emits two specialized bytecodes:
* **`GET_YIELD_FROM_ITER`**: Pops the target iterable from the evaluation stack, checks if it is a generator or iterator, and prepares it for delegation. If it is a generator, it is pushed back directly; otherwise, CPython calls `PyObject_GetIter` to obtain an iterator.
* **`YIELD_FROM`**: Establishes the active delegation channel in the main loop. The VM pops the sub-generator, pulls its yielded value, and pushes it back as the output of the active generator, suspending the frame.

The stack transitions during delegation:
```
Active Frame Stack (During yield from)
+------------------------+
|      Return Value      | <-- (After StopIteration catches)
+------------------------+
|   Sub-generator Ref    | <-- Managed dynamically by YIELD_FROM
+------------------------+
|     Receiver Value     | <-- Received from caller's send()
+------------------------+
```

#### 2. C-Level Generator Mechanics: `PyGenObject`
CPython represents generators at the C-level as `PyGenObject` (`Include/genobject.h`):
```c
typedef struct {
    PyObject_HEAD
    struct _frame *gi_frame;        /* Execution frame holding variables and instruction pointer */
    char gi_running;                /* Active execution flag (1 if running, 0 if suspended) */
    PyObject *gi_code;              /* Code object associated with the generator */
    PyObject *gi_weakreflist;       /* List of weak references */
    PyObject *gi_name;              /* Generator name string */
    PyObject *gi_qualname;          /* Qualified name string */
    _PyErr_StackItem gi_exc_state;   /* Exception state saved across suspensions */
} PyGenObject;
```
* **Suspension Phase**: When a generator yields, the VM sets `gi_running = 0`, saves the current instruction pointer (`f_lasti`) and stack depth within `gi_frame`, and returns control to the caller.
* **Resuming Phase**: When `send()` or `next()` is invoked, CPython sets `gi_running = 1`, restores the thread state's active frame to `gi_frame`, and resumes execution at the saved instruction pointer.

#### 3. Exception & Value Routing Protocol
The CPython implementation inside `Python/ceval.c` maps `yield from` delegation rules:
* **`send()` Routing**: When the caller sends a value via `generator.send(val)`, the `YIELD_FROM` instruction catches the value, bypasses the delegator's frame, and passes it directly to the sub-generator:
  ```c
  retval = _PyGen_Send(sub_gen, sent_val, &val);
  ```
* **`throw()` Propagation**: If the caller raises an exception using `generator.throw(type, val, tb)`, the VM passes the exception to the sub-generator's `throw()` method. If the sub-generator catches the exception and yields a new value, execution continues. If the sub-generator raises `StopIteration` or propagates a different exception, the delegating generator is resumed or unwound.
* **`close()` Cleanup**: When `close()` is called on the delegating generator, the VM invokes the `close()` method of the sub-generator. If the sub-generator does not terminate, it raises a `RuntimeError`.
* **Return Value Unpacking**: When the sub-generator completes by raising `StopIteration`, CPython extracts the `value` attribute from the exception object:
  ```python
  # CPython StopIteration Unpacking Logic
  except StopIteration as e:
      result = e.value # Maps to return value of sub-generator
  ```
  The value is pushed onto the stack, replacing the sub-generator reference, and execution of the delegating generator resumes.

---

### 9.2 PEP 420: Implicit Namespace Packages
PEP 420 introduced implicit namespace packages, allowing packages to span multiple directories on disk without an `__init__.py` file.

#### 1. Import Search Algorithm in `importlib`
During `import foo`, the CPython import engine (`sys.meta_path` hooks) searches directory paths:
1. **Finders Traversal**: The import engine iterates over finders registered in `sys.meta_path` (primarily `PathFinder` which uses paths from `sys.path`).
2. **Standard Package Check**: For each search path in `sys.path`, `PathFinder` checks for a subdirectory `foo` containing `__init__.py`. If found, it returns a standard module spec.
3. **Namespace Path Accumulation**: If no standard package is found, but one or more subdirectories named `foo` exist across `sys.path`, `PathFinder` does not terminate with an error. Instead, it scans all search paths and accumulates all matching directory paths into a list.
4. **Spec Initialization**: It returns a `ModuleSpec` with:
   * `loader` set to `_NamespaceLoader`.
   * `submodule_search_locations` containing the accumulated directory list.
5. **Caching**: It registers the paths in `sys.path_importer_cache` to speed up subsequent submodule lookups.

#### 2. Namespace Modules
The loader `_NamespaceLoader` instantiates a module whose `__file__` attribute is `None`, and whose `__path__` contains the list of accumulated directories:
```python
# Namespace module path list
import company.core
print(company.core.__path__)
# Output: _NamespacePath(['/path1/company/core', '/path2/company/core'])
```
This allows separate wheels or libraries to distribute modules into the same namespace package dynamically.

---

### 9.3 PEP 393: Flexible String Representation
PEP 393 redesigned the internal representation of Unicode strings (`str`) to reduce memory usage. CPython now dynamically selects the narrowest character array encoding based on the maximum code point in the string.

#### 1. C-Level Layout Headers
CPython defines three structs in `Include/unicodeobject.h` to represent strings:
* **`PyASCIIObject`** (ASCII only, characters $\le 127$):
  ```c
  typedef struct {
      PyObject_HEAD
      Py_ssize_t length;          /* Number of code points */
      Py_hash_t hash;             /* Cached hash value; -1 if uncomputed */
      struct {
          unsigned int interned:2; /* Interned state (e.g. SGI_INTERNED) */
          unsigned int kind:3;    /* character size kind (1, 2, or 4 bytes) */
          unsigned int compact:1; /* compact layout flag */
          unsigned int ascii:1;   /* ASCII flag (1 if ASCII only) */
          unsigned int ready:1;   /* Ready state */
      } state;
      wchar_t *wstr;              /* Legacy wchar_t representation cache */
  } PyASCIIObject;
  ```
  The raw characters are stored in memory immediately following this header.
* **`PyCompactUnicodeObject`** (Non-ASCII compact strings, characters $\le 65535$):
  ```c
  typedef struct {
      PyASCIIObject _base;
      Py_ssize_t utf8_length;     /* Length of UTF-8 representation */
      char *utf8;                 /* Pointer to UTF-8 representation cache */
      Py_ssize_t wstr_length;     /* Length of wchar_t representation */
  } PyCompactUnicodeObject;
  ```
* **`PyUnicodeObject`** (Non-compact legacy strings):
  Used primarily for backward compatibility with the C-API. It adds a pointer to the character data memory address (`data.any`).

#### 2. String Kind Allocation and Promotion Rules
The character array representation is determined by the `kind` field:
* `PyUnicode_1BYTE_KIND` (Latin-1, characters $\le 255$): 1 byte per character.
* `PyUnicode_2BYTE_KIND` (UCS-2, characters $\le 65535$): 2 bytes per character.
* `PyUnicode_4BYTE_KIND` (UCS-4, characters $> 65535$): 4 bytes per character.

If a string operation (e.g., concatenation or substitution) appends a character that exceeds the current string's maximum code point, CPython allocates a new string with the promoted `kind` and converts the existing characters:
```python
# String Promotion Simulation
s = "abc"      # Kind: 1-byte (ASCII)
s += ""       # Promoted to Kind: 1-byte (Latin-1)
s += ""       # Promoted to Kind: 2-byte (UCS-2)
s += ""      # Promoted to Kind: 4-byte (UCS-4)
```
During promotion from 1-byte to 2-byte, CPython executes a C conversion loop:
```c
/* C-level character expansion loop */
for (Py_ssize_t i = 0; i < length; i++) {
    dest_2byte[i] = (Py_UCS2)source_1byte[i];
}
```
This design maintains $O(1)$ indexing for all strings while optimizing memory usage for ASCII and Latin-1 strings.

---

### 9.4 `memoryview` and the Buffer Protocol (`Py_buffer`)
The buffer protocol allows Python objects (e.g., `bytes`, `bytearray`, `array.array`) to expose their raw memory buffer directly to other objects without copying data.

#### 1. The `Py_buffer` Struct
The interface is defined by the `Py_buffer` struct in `Include/object.h`:
```c
typedef struct {
    void *buf;                  /* Pointer to the start of the memory block */
    PyObject *obj;              /* Reference to the parent object providing the buffer */
    Py_ssize_t len;             /* Total length of the buffer in bytes */
    Py_ssize_t itemsize;        /* Size of a single element in bytes */
    int readonly;               /* Read-only flag (1 if read-only, 0 if writable) */
    const char *format;         /* Format string describing element type (struct syntax) */
    int ndim;                   /* Number of dimensions */
    Py_ssize_t *shape;          /* Array of sizes for each dimension */
    Py_ssize_t *strides;        /* Array of step strides in bytes for each dimension */
    Py_ssize_t *suboffsets;     /* Suboffsets for nested arrays */
    void *internal;             /* Private storage for the buffer provider */
} Py_buffer;
```

#### 2. Multidimensional Strides Mathematics
The offset in bytes of an element in a multi-dimensional buffer is computed using strides:
$$\text{Offset} = \text{start\_offset} + \sum_{i=0}^{n-1} \text{index}_i \times \text{strides}_i$$
For a 2D matrix of shape `[2, 3]` containing 32-bit C-integers (`itemsize = 4`) stored in row-major order:
* Shape array: `[2, 3]`
* Strides array: `[12, 4]` (since each row is $3 \times 4 = 12$ bytes, and each column is 4 bytes).
To access index `[1, 2]`:
$$\text{Offset} = (1 \times 12) + (2 \times 4) = 12 + 8 = 20 \text{ bytes}$$

CPython can transpose or slice buffers in $O(1)$ time by altering the `shape` and `strides` arrays without moving or copying the underlying data in memory.

#### 3. Buffer Locking & Safety
To prevent memory corruption, objects must coordinate resizing with active buffers:
1. **Buffer Export**: When `memoryview` is created on a `bytearray`, it invokes the provider's `bf_getbuffer` slot, which populates the `Py_buffer` struct and increments the provider's export counter (`ob_exports++`).
2. **Resizing Ban**: While `ob_exports > 0`, any operation that attempts to resize or reallocate the memory buffer (e.g., `bytearray.append()` or `bytearray.extend()`) is blocked and raises a `BufferError`:
   ```python
   # Buffer protection verification
   data = bytearray(b"raw_bytes")
   view = memoryview(data)
   data.extend(b"_new")  # Raises BufferError: Existing exports prevent resizing
   ```
3. **Buffer Release**: When the `memoryview` is garbage collected or explicitly closed, it calls `PyBuffer_Release()`, which invokes the provider's `bf_releasebuffer` slot to decrement the export counter (`ob_exports--`). Once `ob_exports` reaches 0, the buffer can be resized or freed.
