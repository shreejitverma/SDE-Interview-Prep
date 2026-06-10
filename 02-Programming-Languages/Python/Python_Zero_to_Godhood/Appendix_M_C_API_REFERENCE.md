# Appendix M: The Complete CPython C-API Reference Table

This appendix provides a quick-reference for the most important functions in the Python C-API. Mastery of these is required for building high-performance extension modules.

### M.1 Object Management
| Function | Return Type | Purpose |
| :--- | :--- | :--- |
| `Py_INCREF(obj)` | `void` | Increments the reference count. |
| `Py_DECREF(obj)` | `void` | Decrements the reference count (may trigger deallocation). |
| `Py_XINCREF(obj)`| `void` | Same as `Py_INCREF` but handles `NULL`. |
| `Py_XDECREF(obj)`| `void` | Same as `Py_DECREF` but handles `NULL`. |
| `Py_SIZE(obj)` | `Py_ssize_t`| Returns the size of a variable-sized object. |
| `Py_TYPE(obj)` | `PyTypeObject *`| Returns the type of an object. |

### M.2 Number Operations
| Function | Return Type | Purpose |
| :--- | :--- | :--- |
| `PyNumber_Add(v, w)` | `PyObject *`| Equivalent to `v + w`. |
| `PyNumber_Subtract(v, w)`| `PyObject *`| Equivalent to `v - w`. |
| `PyNumber_Multiply(v, w)`| `PyObject *`| Equivalent to `v * w`. |
| `PyNumber_Long(v)` | `PyObject *`| Converts an object to an integer. |
| `PyNumber_Float(v)` | `PyObject *`| Converts an object to a float. |

### M.3 Sequence and Mapping Operations
| Function | Return Type | Purpose |
| :--- | :--- | :--- |
| `PySequence_GetItem(o, i)`| `PyObject *`| Equivalent to `o[i]`. |
| `PySequence_SetItem(o, i, v)`| `int` | Equivalent to `o[i] = v`. |
| `PyMapping_GetItemString(o, k)`| `PyObject *`| Equivalent to `o[k]` where `k` is a C string. |
| `PyMapping_SetItemString(o, k, v)`| `int` | Equivalent to `o[k] = v`. |

### M.4 Exception Handling
| Function | Return Type | Purpose |
| :--- | :--- | :--- |
| `PyErr_SetString(type, msg)`| `void` | Sets the current exception. |
| `PyErr_Occurred()` | `PyObject *`| Checks if an exception is currently set. |
| `PyErr_Clear()` | `void` | Clears the current exception. |
| `PyErr_Print()` | `void` | Prints the current exception to `stderr`. |

### M.5 Global Interpreter Lock (GIL)
| Function | Return Type | Purpose |
| :--- | :--- | :--- |
| `PyEval_SaveThread()` | `PyThreadState *`| Releases the GIL and returns the thread state. |
| `PyEval_RestoreThread(tstate)`| `void` | Re-acquires the GIL using a saved thread state. |
| `PyGILState_Ensure()` | `PyGILState_STATE`| Ensures the current thread has the GIL. |
| `PyGILState_Release(state)`| `void` | Releases the GIL state. |

---
**This table is the "Rosetta Stone" for C-extension developers.**
---
