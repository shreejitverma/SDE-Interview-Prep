# Python 1.x: The PyObject Model & Reference Counting Core


### 2.1 The Unified `PyObject` & `PyVarObject` Structs
In CPython, **every python value is a heap-allocated C struct**. There are no primitive variables on the stack. The fundamental base structure is defined in `Include/object.h`:

```c
/* CPython source definition from Include/object.h */
#ifdef Py_TRACE_REFS
/* Doubly-linked list pointers for active heap tracing in debug builds */
#define _PyObject_HEAD_EXTRA            \
    struct _object *_ob_next;           \
    struct _object *_ob_prev;
#else
#define _PyObject_HEAD_EXTRA
#endif

typedef struct _object {
    _PyObject_HEAD_EXTRA                /* Doubly-linked list pointers */
    Py_ssize_t ob_refcnt;               /* Reference count counter */
    struct _typeobject *ob_type;        /* Pointer to the type definition object */
} PyObject;

typedef struct {
    PyObject ob_base;                   /* Base type fields */
    Py_ssize_t ob_size;                 /* Size of the variable portion (e.g. sequence length) */
} PyVarObject;
```

#### 1. Comparison to C++ Memory Layout
Unlike C++, where objects are laid out contiguously on the stack or heap based on their static type declarations and resolved via virtual function tables (`vtable`), CPython objects are entirely dynamic:
1. **Uniform Pointer Width**: Every variable in Python is a pointer (`PyObject*`), consuming exactly 8 bytes (on 64-bit platforms).
2. **No VTable Indirection**: Polymorphism is resolved dynamically by dereferencing the type pointer `ob_type` at runtime to lookup operations.

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

---

### 2.2 The Type-Object Slot System
The type of an object is defined by the `PyTypeObject` struct (which is itself a subclass of `PyObject`).

#### 1. Type Struct Slots layout
The type struct defines a series of "slots" (pointers to C functions) that determine how the object behaves when subjected to operators.
```c
/* Conceptual CPython definition from Include/cpython/object.h */
struct _typeobject {
    PyObject_VAR_HEAD
    const char *tp_name;                 /* For printing, in format <module>.<name> */
    Py_ssize_t tp_basicsize, tp_itemsize; /* For allocation */
    
    /* Type Slots (Static Function Pointers) */
    destructor tp_dealloc;               /* Deallocation handler */
    reprfunc tp_repr;                    /* Representation builder */
    getattrfunc tp_getattr;              /* Attribute lookup hook */
    setattrfunc tp_setattr;              /* Attribute assignment hook */
    hashfunc tp_hash;                    /* Hashing calculation function pointer */
    
    /* Protocol Table Slots */
    PyNumberMethods *tp_as_number;       /* Math function pointers (e.g. nb_add) */
    PySequenceMethods *tp_as_sequence;   /* Indexing function pointers (e.g. sq_item) */
    PyMappingMethods *tp_as_mapping;     /* Map lookup function pointers (e.g. mp_subscript) */
};
typedef struct _typeobject PyTypeObject;
```

#### 2. Dunder Methods to Slot Mapping
When defining a class in Python, the interpreter automatically maps custom dunder methods to these internal C slots:
*   `__init__` maps to `tp_init`.
*   `__new__` maps to `tp_new`.
*   `__hash__` maps to `tp_hash`.
*   `__add__` maps to `tp_as_number->nb_add`.
*   `__getitem__` maps to `tp_as_mapping->mp_subscript` or `tp_as_sequence->sq_item`.

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

---

### 2.3 Reference Counting Lifecycle
CPython manages memory directly via reference counting. Reference counts are modified using C macros.

#### 1. C-Level Reference Modification Macros
Defined in `object.h`, the macros expand as:
```c
#define Py_INCREF(op) (                         \
    _Py_INC_REFTOTAL  _Py_REF_DEBUG_EXTRA      \
    (op)->ob_refcnt++)

#define Py_DECREF(op) do {                      \
    if (_Py_DEC_REFTOTAL  _Py_REF_DEBUG_EXTRA   \
        --(op)->ob_refcnt == 0)                 \
        _Py_Dealloc((PyObject *)(op));          \
} while (0)
```
To prevent segmentation faults when referencing a null pointer, CPython provides the NULL-safe macros `Py_XINCREF(op)` and `Py_XDECREF(op)`, which check `if (op != NULL)` before modifying the counts.

#### 2. The Deallocation Pipeline
When `Py_DECREF(obj)` drops the reference count to zero:
1. **Invoke Deallocator**: The VM invokes `obj->ob_type->tp_dealloc(obj)`.
2. **Recursive Decrements**: The deallocator function decrements references of any nested objects inside this object. For example, if a list is deallocated, it decrements the reference counts of all items it contains.
3. **Release Memory**: The deallocator releases the raw memory back to PyMalloc or the system allocator.

#### 3. Leak Tracking in C Extensions
In custom C extensions, failing to decrement references leads to memory leaks. In debug builds, compiling CPython with `Py_TRACE_REFS` tracks all active references in a global doubly-linked list.
We can trace reference counts using the `sys` module:

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

---

### 2.4 Built-in Caching and Interning Optimizations
To avoid allocating memory for common objects, CPython uses global caches:

#### 1. Small Integer Cache
CPython pre-allocates an array of integer objects from `-5` to `256` (`NSMALLNEGINTS` and `NSMALLPOSINTS`).
*   **Mechanism**: Any assignment in this range returns a pointer to the pre-existing static object, bypassing heap allocations.
*   **C-level array**: Inside `objects/longobject.c`, this is defined as:
```c
static PyLongObject small_ints[NSMALLNEGINTS + NSMALLPOSINTS];
```

#### 2. String Interning
String literals resembling identifiers are automatically **interned**.
*   **Mechanism**: Interned strings are stored in a global dictionary inside `unicodeobject.c`. If a string is already interned, any new instantiation returns the existing string pointer.
*   **Performance**: This allows matching strings via fast pointer comparison ($O(1)$ address checks) instead of byte-by-byte comparison ($O(N)$ string scans).
```c
/* Under the hood string comparison */
if (str1 == str2) {
    return 1; /* Match! (O(1) address check) */
}
```

#### 3. Empty Collections and Singletons
To save memory:
*   **Empty Tuple Cache**: CPython allocates exactly one global empty tuple object (`&_Py_EmptyTuple`). Every call to `tuple()` returns a pointer to this singleton.
*   **Built-in Singletons**: `None`, `True`, `False`, `Ellipsis`, and `NotImplemented` are statically allocated global structures.

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

# Empty tuple check
tup_a = ()
tup_b = tuple()
print("Are empty tuples cached singletons?", tup_a is tup_b) # True
```

---
