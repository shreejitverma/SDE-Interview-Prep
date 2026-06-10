# Writing a C Extension from Scratch


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
