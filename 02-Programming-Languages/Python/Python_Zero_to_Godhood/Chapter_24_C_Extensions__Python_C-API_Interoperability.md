# C Extensions & Python C-API Interoperability


### 24.1 Writing a Pure C Extension Module
Python allows writing modules directly in C or C++ to access low-level OS APIs, optimize performance-critical paths, or link with native libraries. A C extension is compiled into a shared library (a `.so` file on Unix, or a `.pyd` file on Windows) that Python can load dynamically at runtime using `import`.

#### 1. Detailed C-Extension with Error Handling and Keywords
A robust C extension must handle keyword arguments, perform error checks, raise Python exceptions on failure, and manage resources. 

Below is an implementation of a custom C module exposing a safe division function:

```c
#define PY_SSIZE_T_CLEAN
#include <Python.h>

/* 1. Core C Function Implementation with Keywords & Exception Handling */
static PyObject* 
custom_safe_divide(PyObject* self, PyObject* args, PyObject* keywds) {
    double numerator;
    double denominator;
    
    /* Argument keyword names array */
    static char *kwlist[] = {"numerator", "denominator", NULL};
    
    /* Parse Python argument tuple & dict into native double values */
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "dd", kwlist, &numerator, &denominator)) {
        return NULL; /* Returns NULL to signal an exception occurred during parsing */
    }
    
    /* Check for division by zero and raise a Python ZeroDivisionError */
    if (denominator == 0.0) {
        PyErr_SetString(PyExc_ZeroDivisionError, "Denominator cannot be zero.");
        return NULL; /* Return NULL to propagate the raised exception */
    }
    
    double result = numerator / denominator;
    
    /* Convert native C double back to a Python PyFloatObject */
    return PyFloat_FromDouble(result);
}

/* 2. Methods Table Registration */
static PyMethodDef CustomMethods[] = {
    {
        "safe_divide", 
        (PyCFunction)(void(*)(void))custom_safe_divide, 
        METH_VARARGS | METH_KEYWORDS, 
        "Divides numerator by denominator, returning float safely."
    },
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
PyMODINIT_FUNC 
PyInit_custom_c_math(void) {
    return PyModule_Create(&custommodule);
}
```

*   **`METH_VARARGS | METH_KEYWORDS`**: Tells the interpreter that this function accepts both positional arguments (parsed into `args` as a tuple) and keyword arguments (parsed into `keywds` as a dictionary).
*   **`PyArg_ParseTupleAndKeywords`**: Resolves both types of inputs, mapping them into the C double variables based on the format string `"dd"` (double, double) and the keyword list `kwlist`.
*   **Exception Signaling**: Returning a `NULL` pointer tells CPython's execution frame that an exception has been set inside the thread state. The interpreter pauses bytecode execution and executes its exception handler path.

---

### 24.2 Reference Counting and Ownership Rules in C-API
When interacting with the Python C-API, developers must manually manage reference counting. Standard Python code delegates reference updates to the compiler/interpreter, but in C, a single missing decref causes a memory leak, and an extra decref causes a crash (segmentation fault).

#### 1. Reference Ownership Categories
Every pointer returning from a C-API function belongs to one of three reference categories:

##### New References
The C function receives absolute ownership of the reference. It must decrement the reference count when finished, or pass ownership back to CPython (e.g., by returning the object from the function).

*   *Examples*: `PyLong_FromLong()`, `PyList_New()`, `PyObject_Call()`.

##### Borrowed References
The C function receives a pointer without ownership. It does not own the reference and must not decrement it unless it explicitly calls `Py_INCREF()` to claim ownership. If the owner of the object frees it, a borrowed pointer becomes dangling.

*   *Examples*: `PyTuple_GetItem()`, `PyList_GetItem()`.

##### Stolen References
Some C-API functions take ownership of references passed to them. The caller no longer owns the reference and must not decrement it, as the receiver will decrement it during its own deallocation pass.

*   *Examples*: `PyTuple_SetItem()`, `PyList_SetItem()`.

```c
void add_item_to_tuple_example(void) {
    PyObject* my_tuple = PyTuple_New(1);              /* Returns New Reference */
    PyObject* my_val = PyLong_FromLong(42);           /* Returns New Reference */
    
    /* PyTuple_SetItem steals the reference to 'my_val' */
    PyTuple_SetItem(my_tuple, 0, my_val);
    
    /* 
       Do NOT call Py_DECREF(my_val). 'my_tuple' now owns it.
       If we decref'd my_val, freeing my_tuple would trigger a double-free crash.
    */
    
    Py_DECREF(my_tuple); /* Safely deallocates tuple and my_val */
}
```

#### 2. Safe Reference Macros
CPython provides macros to handle reference updates safely:
*   `Py_INCREF(op)` / `Py_DECREF(op)`: Non-null safe updates. Passing a `NULL` pointer triggers a crash.
*   `Py_XINCREF(op)` / `Py_XDECREF(op)`: Null-safe updates. They check if `op` is `NULL` and do nothing if it is, protecting code in error-handling block cleanups:

```c
void error_handling_cleanup_example(PyObject *a, PyObject *b) {
    // If a or b failed allocation and were set to NULL, XDECREF handles it safely
    Py_XDECREF(a);
    Py_XDECREF(b);
}
```

---

### 24.3 C-API Stable ABI (PEP 384)
Standard C extensions are compiled against a specific Python version's headers (e.g., Python 3.10). They depend on concrete structure offsets (such as `sizeof(PyObject)` or offsets of `ob_type`). When Python updates its internal layouts (e.g., changing the type struct in 3.11), these compiled binaries break, requiring recompilation for the new Python version.

To resolve this compilation dependency, PEP 384 introduced the **Stable ABI** (Application Binary Interface) / Limited API.

```
+---------------------------------------+
|        Standard C Extension           |
|  Directly accesses structures (e.g.   | ---> Fast, but must compile
|  op->ob_refcnt, op->ob_type)          |      for every Python version.
+---------------------------------------+

+---------------------------------------+
|          Stable ABI Extension         |
|  Uses opaque pointers and accessors   | ---> Compile once. Runs on
|  (e.g., Py_REFCNT(op), PyType_GetSlot) |      Python 3.5, 3.6, ... 3.13+
+---------------------------------------+
```

#### 1. Opaque Pointers and Opaque Structures
When compiling under the Stable ABI, structure definitions (like `PyObject` or `PyTypeObject`) are treated as **opaque structures**. You cannot compile code that accesses their internal fields directly:

```c
/* Standard API (will not compile under Py_LIMITED_API) */
PyTypeObject* type = op->ob_type;

/* Stable ABI equivalent */
PyTypeObject* type = Py_TYPE(op);
```

#### 2. Defining the Limited API
To restrict compilation to the Stable ABI, define `Py_LIMITED_API` before importing `<Python.h>` or set it as a compiler flag:

```c
#define Py_LIMITED_API 0x030A0000  /* Targets Python 3.10 and later */
#define PY_SSIZE_T_CLEAN
#include <Python.h>
```

This guarantees that the resulting compiled binary links only with stable symbols exported by the dynamic library (`libpython3.so` or `python3.dll`), allowing the extension to load and run on any subsequent Python version without recompilation.

---
