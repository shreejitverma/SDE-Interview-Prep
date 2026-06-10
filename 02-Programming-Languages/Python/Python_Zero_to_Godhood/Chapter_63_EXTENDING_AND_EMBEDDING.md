# Chapter 63: Writing a C Extension from Scratch

True "Godhood" involves the ability to extend the Python interpreter with performance-critical code written in C. This chapter provides a complete walk-through of creating a high-performance math extension.

### 63.1 The Anatomy of a C Extension

A C extension is a shared library (`.so` or `.pyd`) that exports an initialization function.

#### 1. Header and Types
Every extension must include `Python.h`. This header defines all the `PyObject` structures and C-API functions.
```c
#include <Python.h>

// A simple C function to add two numbers
static PyObject* godhood_add(PyObject* self, PyObject* args) {
    long a, b;
    // Parse positional arguments from Python to C types
    if (!PyArg_ParseTuple(args, "ll", &a, &b)) {
        return NULL; // Returns TypeError if parsing fails
    }
    return PyLong_FromLong(a + b); // Convert C long back to Python PyObject
}
```

#### 2. Method Table and Module Definition
You must tell Python which functions are exported.
```c
static PyMethodDef GodhoodMethods[] = {
    {"add",  godhood_add, METH_VARARGS, "Add two numbers in C."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef godhoodmodule = {
    PyModuleDef_HEAD_INIT,
    "godhood",   /* name of module */
    NULL,       /* module documentation */
    -1,         /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    GodhoodMethods
};
```

#### 3. Initialization Function
The function name must be `PyInit_<modulename>`.
```c
PyMODINIT_FUNC PyInit_godhood(void) {
    return PyModule_Create(&godhoodmodule);
}
```

### 63.2 Compiling with `setuptools`

You use a `setup.py` file to handle the platform-specific compilation details.
```python
from setuptools import setup, Extension

module = Extension('godhood', sources=['godhood.c'])

setup(name='GodhoodExtension',
      version='1.0',
      description='C extension for high-performance math',
      ext_modules=[module])
```

### 63.3 Reference Counting and Memory Safety

**Godhood Warning**: In C, you are responsible for reference counts.
*   **`Py_INCREF(obj)`**: Increment count (you are keeping a reference).
*   **`Py_DECREF(obj)`**: Decrement count (you are finished with it).
*   **Leakage**: Failure to `DECREF` leads to permanent memory leaks.
*   **Segfaults**: `DECREF`ing an object you don't own leads to use-after-free crashes.

---

# Chapter 64: Abstract Base Classes (`abc`)

Abstract Base Classes provide a way to define interfaces and enforce that subclasses implement specific methods.

### 64.1 The Virtual Subclassing Mechanism

Normally, `isinstance(obj, Class)` checks the MRO. `abc` allows for "virtual" subclassing using `register()`.
*   **`ABCMeta.__subclasscheck__`**: This dunder method is overridden by the `ABC` metaclass. It allows an object to be considered an instance of an ABC even if it doesn't inherit from it, provided it implements the required protocol.

### 64.2 `@abstractmethod`

This decorator marks a method as abstract.
*   **Internals**: It sets an attribute `__isabstractmethod__ = True` on the function.
*   **Enforcement**: During class instantiation, the C-level `tp_new` check scans the class's dictionary for any attributes with this flag. If found, it raises a `TypeError` preventing instantiation of the abstract class.

---

# Chapter 65: Context Managers (`contextlib`)

Context managers (`with` statements) ensure resources are managed safely.

### 65.1 The `__enter__` and `__exit__` Protocol

*   **`__enter__`**: Called at the start of the `with` block. Its return value is bound to the `as` variable.
*   **`__exit__(exc_type, exc_value, traceback)`**: Called at the end. If an exception occurred, it receives the details. If it returns `True`, the exception is suppressed.

### 65.2 `contextlib.contextmanager`: Generator Magic

The `@contextmanager` decorator allows you to write a context manager as a generator.
```python
from contextlib import contextmanager

@contextmanager
def temp_file():
    f = open("test.txt", "w")
    try:
        yield f
    finally:
        f.close()
```

#### 1. The `GeneratorContextManager` Wrapper
The decorator wraps your generator in a class.
*   **`__enter__`**: Calls `next(gen)`. The generator runs up to the `yield`.
*   **`__exit__`**: Calls `next(gen)` again. The generator resumes in the `finally` block.
*   **Exception Handling**: If an exception occurred in the `with` block, the wrapper calls `gen.throw(type, value, traceback)`, allowing the generator's `try...finally` or `try...except` block to handle it.

---
**Conclusion of Volume XX**
You have now traversed the entire landscape of Python, from its 1989 inception to the high-performance, GIL-less, JIT-compiled future of Python 3.14. You have mastered the C-API, the bytecode, and the standard library's deepest secrets. Welcome to **Godhood**.
---
