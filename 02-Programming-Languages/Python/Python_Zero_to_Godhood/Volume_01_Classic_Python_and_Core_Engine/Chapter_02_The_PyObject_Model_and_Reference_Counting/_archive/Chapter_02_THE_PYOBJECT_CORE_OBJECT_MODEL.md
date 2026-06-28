# THE PYOBJECT CORE OBJECT MODEL


### 2.1 The Unified PyObject & PyVarObject Structs
In CPython, **every python value is a heap-allocated C struct**. There are no primitive variables on the stack. The fundamental base structure is defined in `Include/object.h`:

```c
typedef struct _object {
    _PyObject_HEAD_EXTRA    // Doubly-linked list pointers for active heap tracing
    Py_ssize_t ob_refcnt;   // Reference count counter
    struct _typeobject *ob_type; // Pointer to the type definition object
} PyObject;

typedef struct {
    PyObject ob_base;       // Base type fields
    Py_ssize_t ob_size;     // Size of the variable portion (e.g. string length)
} PyVarObject;
```

#### Comparison to C++ memory:
Unlike C++, where objects are laid out contiguously on the stack or heap based on their static type declarations and resolved via virtual function tables (`vtable`), CPython objects are entirely dynamic:
1.  **Uniform Pointer Width**: Every variable in Python is a pointer (`PyObject*`), consuming exactly 8 bytes (on 64-bit platforms).
2.  **No VTable Indirection**: Polymorphism is resolved dynamically by dereferencing the type pointer `ob_type` at runtime to lookup operations.

```
Python Reference Pointer:
[Name: x] ---> [ Heap Allocation: PyObject ]
               +--------------------------------------+
               | _PyObject_HEAD_EXTRA (Tracking)      |
               | ob_refcnt: 1                         |
               | ob_type: ----> [ PyTypeObject (int)] |
               | Integer Data Value Payload           |
               +--------------------------------------+
```

### 2.2 The Type-Object Slot System
The type of an object is defined by `PyTypeObject` (which is itself a subclass of `PyObject`).
*   **Type Slots**: The type struct defines a series of "slots" (pointers to C functions) that determine how the object behaves when subjected to operators.
    - `tp_alloc`: Allocates heap memory for the struct.
    - `tp_new`: Constructor initializer (invokes allocation).
    - `tp_init`: Initialization hook (corresponds to `__init__`).
    - `tp_dealloc`: Destructor wrapper, called when reference counts hit zero.
    - `tp_hash`: Defines the hashing method for dictionaries and sets.

```python
# Customizing slots via class definitions
class CustomObject:
    def __init__(self, val):
        self.val = val

    def __hash__(self):
        # Maps to the tp_hash C slot
        return hash(self.val)

obj = CustomObject("test")
print("Hash via type slot:", hash(obj))
```

### 2.3 Reference Counting Lifecycle
CPython manages memory directly via reference counting. The macros `Py_INCREF()` and `Py_DECREF()` increment and decrement `ob_refcnt` respectively.
*   **The Decrement Process**: When `Py_DECREF(obj)` drops the reference count to zero:
    1.  The runtime invokes `obj->ob_type->tp_dealloc(obj)`.
    2.  This deallocator function decrements references of any nested objects inside this object.
    3.  The raw memory is released back to the interpreter's memory allocator (PyMalloc).
*   **Leak Debugging**: In custom C extensions, failing to decrement references leads to memory leaks. In debug builds, compiling with `Py_TRACE_REFS` tracks all active references in a global list.

```python
import sys

# Reference counts tracking
my_list = [100, 200]
print("Starting references count:", sys.getrefcount(my_list) - 1)

ref_a = my_list
ref_b = my_list
print("References after bindings:", sys.getrefcount(my_list) - 1)

del ref_a
print("References after deleting one binding:", sys.getrefcount(my_list) - 1)
```

### 2.4 Built-in Caching and Interning Optimizations
To avoid allocating memory for common objects, CPython uses global caches:
*   **Small Integer Cache**: CPython pre-allocates an array of integer objects from `-5` to `256` (`NSMALLNEGINTS` and `NSMALLPOSINTS`).
    - *Under the hood*: Any assignment in this range returns a pointer to the pre-existing static object, bypassing heap allocations.
*   **String Interning**: String literals resembling identifiers are automatically **interned**.
    - *Mechanism*: Interned strings are stored in a global dictionary. If a string is already interned, any new instantiation returns the existing string pointer. This allows matching strings via fast pointer comparison ($O(1)$ address checks) instead of byte-by-byte comparison ($O(N)$ string scans).

```python
# Small Integer Caching Check
int_a = 256
int_b = 256
print("Are 256 pointers identical?", int_a is int_b) # True (cached)

int_c = 257
int_d = 257
print("Are 257 pointers identical?", int_c is int_d) # False (dynamically allocated)

# String Interning Check
str_a = "godhood_string"
str_b = "godhood_string"
print("Are literal strings interned?", str_a is str_b) # True
```

---
