# C Extensions & Python C-API Interoperability


### 24.1 Writing a Pure C Extension Module
Python allows writing modules directly in C or C++ to access low-level OS APIs, optimize performance-critical paths, or link with native hardware drivers.

#### 1. Minimal C-Extension Boilerplate
A complete C-extension module requires defining the methods table, the module descriptor, and the dynamic initialization hook:

```c
#define PY_SSIZE_T_CLEAN
#include <Python.h>

/* 1. Core C Function Implementation */
static PyObject* method_add(PyObject* self, PyObject* args) {
    long a, b;
    /* Parse Python arguments tuple into native C types */
    if (!PyArg_ParseTuple(args, "ll", &a, &b)) {
        return NULL; /* Raises TypeError automatically */
    }
    long sum = a + b;
    /* Convert native C type back to PyObject integer wrapper */
    return PyLong_FromLong(sum);
}

/* 2. Methods Table Registration */
static PyMethodDef CustomMethods[] = {
    {"add", method_add, METH_VARARGS, "Calculate the sum of two integers."},
    {NULL, NULL, 0, NULL} /* Sentinel marker to end array */
};

/* 3. Module Definition Structure */
static struct PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
    "custom_c_math",            /* Module Name */
    "A custom high-performance C extension module.", /* Docstring */
    -1,                         /* Module state size (-1 = global state) */
    CustomMethods
};

/* 4. Initialization Hook called upon 'import custom_c_math' */
PyMODINIT_FUNC PyInit_custom_c_math(void) {
    return PyModule_Create(&custommodule);
}
```

---

### 24.2 Reference Counting and Ownership Rules in C-API
When interacting with the Python C-API, developers must manually manage reference counting to avoid memory leaks or segfaults due to premature deallocation.

#### 1. Reference Ownership Categories
*   **New References**: The C function receives ownership of the reference. It must decrement the reference count when finished, or pass ownership back to CPython.
    *   *Examples*: `PyLong_FromLong()`, `PyList_New()`, `PyObject_Call()`.
*   **Borrowed References**: The C function receives a pointer without ownership. It does not own the reference and must not decrement it unless it explicitly calls `Py_INCREF()` to claim ownership.
    *   *Examples*: `PyTuple_GetItem()`, `PyList_GetItem()`.

```c
PyObject* list = PyList_New(1);      /* Returns a New Reference */
PyObject* item = PyList_GetItem(list, 0); /* Returns a Borrowed Reference */

Py_INCREF(item);                    /* Converts 'item' to a New Reference */
Py_DECREF(list);                    /* Safely frees list; item remains valid */
Py_DECREF(item);                    /* Frees item */
```

---

### 24.3 Interoperability Toolchains: Pybind11 vs. Cython
Writing pure C extensions can be verbose and error-prone. Modern projects utilize toolchains to automate bindings:

#### 1. Pybind11
Pybind11 is a header-only library that exposes C++ types and functions to Python using advanced template metaprogramming. It generates C-API wrapper code during compile time, automatically converting types (e.g., `std::vector` to Python `list`).

#### 2. Cython
Cython is a superset of Python that supports calling C functions and declaring C types. The Cython compiler translates annotated `.pyx` files into optimized C code.
*   **Static Type Declarations**: Using `cdef` keywords avoids Python dynamic lookup overhead by compiling directly to native C variables and structs.
*   **Direct Struct Lookups**: It accesses C-level structs directly, completely bypassing the dictionary lookup layer for microsecond-level execution times.

---
