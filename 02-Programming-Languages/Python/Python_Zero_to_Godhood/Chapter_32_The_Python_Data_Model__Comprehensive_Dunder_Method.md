# The Python Data Model & Comprehensive Dunder Methods


The "Data Model" is the formal description of Python's objects and their interactions. While most developers know `__init__`, the "Godhood" level of understanding requires knowing how these high-level methods map directly to functional pointers in the CPython C source code.

### 32.1 The Philosophy: Protocols over Types

Python uses "duck typing," but it is more accurately described as a **Protocol-based language**. If an object implements the methods required by a protocol, it *is* that thing. These protocols are implemented using **Special Methods** (Dunder methods).

### 32.2 Object Lifecycle and Representation

#### 1. Creation and Initialization
*   `__new__(cls, ...)`: The actual constructor. It returns a new instance of `cls`. It maps to the `tp_new` slot in C.
*   `__init__(self, ...)`: The initializer. It configures the instance created by `__new__`. It maps to `tp_init`.

#### 2. String Representations
*   `__repr__(self)`: The "official" string representation, ideally usable to recreate the object. Used by the debugger and REPL. Maps to `tp_repr`.
*   `__str__(self)`: The "informal" or user-friendly string representation. Maps to `tp_str`.

### 32.3 The Mapping to C Slots

Every Python class is an instance of `PyTypeObject`. This C struct contains a vast array of "slots"function pointers that the VM calls when executing operations.

| Python Method | C Slot | Description |
| :--- | :--- | :--- |
| `__call__` | `tp_call` | Called when object is invoked like a function. |
| `__iter__` | `tp_iter` | Returns an iterator object. |
| `__next__` | `tp_iternext` | Returns the next item from an iterator. |
| `__getattr__` | `(dynamic)` | Called if attribute lookup fails. |
| `__getattribute__` | `tp_getattro` | Called for EVERY attribute lookup. |

### 32.4 Numeric and Container Protocols

To save space and optimize lookup, CPython groups related methods into sub-structs:

#### 1. `tp_as_number`
Methods like `__add__`, `__sub__`, and `__mul__` are stored here. When you write `a + b`, the VM looks at `a->ob_type->tp_as_number->nb_add`.

#### 2. `tp_as_sequence` and `tp_as_mapping`
*   **Sequence**: `__len__` (`sq_length`), `__getitem__` (`sq_item`).
*   **Mapping**: `__getitem__` (`mp_subscript`), `__setitem__` (`mp_ass_subscript`).

Note that `__getitem__` is overloaded; if the object is a sequence, it expects an integer; if it's a mapping, it expects a hashable key.

### 32.5 Comprehensive Comparison: Rich Comparisons

Python 3 unified comparisons into **Rich Comparisons** (`tp_richcompare`).
*   `__lt__`, `__le__`, `__eq__`, `__ne__`, `__gt__`, `__ge__`.

These all map to a single C function that receives an `op` argument (e.g., `Py_EQ`, `Py_LT`). If you implement only `__eq__`, Python does not automatically derive `__ne__` or others, unlike some older versions.

### 32.6 The `__slots__` Optimization (Recap)

As covered in Chapter 26, `__slots__` prevents the creation of `__dict__`. Internally, this changes the `PyTypeObject` flags and allocates space for the attributes directly in the object struct, mapping them via **Member Descriptors**.

---


---

