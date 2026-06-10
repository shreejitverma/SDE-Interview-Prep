# Python 3.11 to 3.12: Exception Groups (except*) and Traceback trees


### 19.1 Exception Groups (`ExceptionGroup` & `BaseExceptionGroup` / PEP 654)
Introduced in Python 3.11, `ExceptionGroup` and `BaseExceptionGroup` allow raising and handling multiple unrelated exceptions simultaneously. This is critical for concurrent frameworks (such as `asyncio` TaskGroups or Trio) where multiple background operations can crash concurrently.

#### 1. Exception Group Class Layout and Hierarchy
The class hierarchy separates groups of standard exceptions from groups containing system-level exceptions:

```python
class BaseExceptionGroup(BaseException):
    def __init__(self, message: str, exceptions: Sequence[BaseException]) -> None:
        self.message = message
        self.exceptions = list(exceptions)

class ExceptionGroup(BaseExceptionGroup, Exception):
    pass
```

*   `BaseExceptionGroup` inherits directly from `BaseException`. It can wrap any exception instance, including system-terminating errors like `KeyboardInterrupt`, `SystemExit`, or `GeneratorExit`.
*   `ExceptionGroup` inherits from both `BaseExceptionGroup` and `Exception`. It can only wrap exceptions that inherit from `Exception`. If a program attempts to instantiate an `ExceptionGroup` containing a `BaseException` that is not an `Exception` subclass, CPython raises a `TypeError` at runtime.

#### 2. CPython C-Struct Representation
At the C level, exception groups are managed by the `PyExceptionGroupObject` structure, which extends the standard exception header to support the nested tree structure:

```c
/* Include/cpython/exceptions.h */
typedef struct {
    PyBaseExceptionObject base; /* Base exception header containing dict, traceback, context */
    PyObject *msg;              /* Message describing the group (PyUnicodeObject) */
    PyObject *excs;             /* Tuple containing the child exception objects (PyTupleObject) */
} PyExceptionGroupObject;
```

When an exception group is instantiated:
1.  `base.args` is populated with a tuple containing the `message` and `exceptions` sequence.
2.  `msg` holds the string identifier directly for fast attribute lookup.
3.  `excs` holds the underlying exceptions wrapped inside a flat, immutable tuple.

---

### 19.2 The `except*` Clause and Exception Tree Filtering
To handle individual branches of an exception group, Python 3.11 introduced the `except*` statement. Unlike a traditional `except` statement, which catches a single exception instance, `except*` extracts a matching subset from the Exception Group hierarchy, letting unmatched exceptions propagate.

#### 1. Compilation and Bytecode Mechanics
The compiler generates a specialized sequence of bytecodes for `except*` blocks to split the exception tree dynamically at runtime.

Consider the two patterns below:

```python
# Standard try/except
try:
    raise ValueError("Error")
except ValueError as e:
    pass
```

Disassembly of standard `except`:
```
  1           0 NOP
              2 SETUP_FINALLY           8 (to 12)

  2           4 LOAD_GLOBAL              1 (ValueError)
              6 LOAD_CONST               1 ('Error')
              8 CALL                     1
             10 RAISE_VARARGS            1

  3     >>   12 PUSH_EXC_INFO
             14 CHECK_EXCEPT             1 (ValueError)
             16 POP_JUMP_IF_FALSE       14 (to 46)
             18 STORE_FAST               0 (e)
             ...
```

Now, consider the `except*` variant:
```python
# Modern try/except*
try:
    raise ExceptionGroup("Main Group", [ValueError("Error A"), TypeError("Error B")])
except* ValueError as eg:
    print("Caught Val:", eg.exceptions)
```

Disassembly of `except*`:
```
  1           0 NOP
              2 SETUP_FINALLY          12 (to 16)

  2           4 LOAD_GLOBAL              1 (ExceptionGroup)
              6 LOAD_CONST               1 ('Main Group')
              8 ... (building ValueError and TypeError objects)
             10 BUILD_LIST               2
             12 CALL                     2
             14 RAISE_VARARGS            1

  3     >>   16 PUSH_EXC_INFO
             18 CHECK_EXCEPT_STAR        1 (ValueError)
             20 POP_JUMP_IF_FALSE       16 (to 52)
             22 STORE_FAST               0 (eg)
             ...
```

#### 2. Bytecode Analysis and VM Stack Transitions
The crucial bytecode introduced for PEP 654 is `CHECK_EXCEPT_STAR`. Its execution flow proceeds as follows:

```
Stack State Before CHECK_EXCEPT_STAR:
+------------------------+
|  ValueError (type tag)  | <-- TOP OF STACK (TOS)
+------------------------+
|  ExceptionGroup Object  | <-- TOS1
+------------------------+

Execution details of CHECK_EXCEPT_STAR:
1. Pops the target exception type (TOS: ValueError).
2. Inspects the active ExceptionGroup (TOS1).
3. Executes a C-level filtering function:
   - Splits the ExceptionGroup into a matched group containing all ValuerError instances.
   - Pushes the matched group.
   - Pushes the remaining unmatched exception group (containing TypeError).
   
Stack State After CHECK_EXCEPT_STAR (on match):
+------------------------+
| Matched ExceptionGroup | <-- TOS (Bound to 'eg' local variable)
+------------------------+
| Unmatched ExceptionGrp | <-- TOS1 (Propagated or checked by next handler)
+------------------------+
```

If the match group is empty (no `ValueError` instances were found), `CHECK_EXCEPT_STAR` pushes `None` to TOS, and `POP_JUMP_IF_FALSE` jumps directly to the next handler block.

---

### 19.3 Exception Tree Filtering Algorithm
The core mechanics of splitting exception groups are defined in CPython's exception runtime library. The algorithm must recursively inspect the tree of exceptions and cleanly separate them.

```
                  ExceptionGroup ("Main")
                  /                     \
          TypeError("B")            ExceptionGroup ("Sub")
                                    /                     \
                             ValueError("A")       KeyError("C")
```

If we execute `except* ValueError`:
1.  The runtime calls the internal C function `exception_group_filter(eg, match_value_error_func)`.
2.  It traverses the children of the group:
    *   `TypeError("B")`: Does not match. Added to the `unmatched` collection.
    *   `ExceptionGroup("Sub")`: Recursively calls `exception_group_filter`.
        *   Inside `ExceptionGroup("Sub")`:
            *   `ValueError("A")`: Matches! Added to the sub-matched collection.
            *   `KeyError("C")`: Does not match. Added to the sub-unmatched collection.
        *   Since there were matches inside `Sub`, a new `ExceptionGroup("Sub")` is instantiated, containing only `ValueError("A")`. This is returned as the sub-matched tree.
        *   A new `ExceptionGroup("Sub")` is instantiated containing only `KeyError("C")` and returned as the sub-unmatched tree.
3.  The top-level execution collects the returned structures:
    *   `matched` tree: `ExceptionGroup("Main", [ExceptionGroup("Sub", [ValueError("A")])])`
    *   `unmatched` tree: `ExceptionGroup("Main", [TypeError("B"), ExceptionGroup("Sub", [KeyError("C")])])`

Here is the equivalent C-like pseudocode of the recursive filtering function:

```c
PyObject* exception_group_filter(PyObject *eg, PyObject *match_type) {
    PyObject *matched_list = PyList_New(0);
    PyObject *unmatched_list = PyList_New(0);
    
    PyObject *excs = ((PyExceptionGroupObject*)eg)->excs;
    Py_ssize_t size = PyTuple_GET_SIZE(excs);
    
    for (Py_ssize_t i = 0; i < size; i++) {
        PyObject *exc = PyTuple_GET_ITEM(excs, i);
        if (PyExceptionGroup_Check(exc)) {
            // Recursive split
            PyObject *sub_match = NULL, *sub_unmatch = NULL;
            split_exception_group(exc, match_type, &sub_match, &sub_unmatch);
            if (sub_match != NULL) {
                PyList_Append(matched_list, sub_match);
            }
            if (sub_unmatch != NULL) {
                PyList_Append(unmatched_list, sub_unmatch);
            }
        } else {
            // Leaf node matching
            if (PyErr_GivenExceptionMatches(exc, match_type)) {
                PyList_Append(matched_list, exc);
            } else {
                PyList_Append(unmatched_list, exc);
            }
        }
    }
    
    PyObject *matched_group = NULL;
    if (PyList_Size(matched_list) > 0) {
        matched_group = create_exception_group_from_list(eg, matched_list);
    }
    PyObject *unmatched_group = NULL;
    if (PyList_Size(unmatched_list) > 0) {
        unmatched_group = create_exception_group_from_list(eg, unmatched_list);
    }
    
    return PyTuple_Pack(2, matched_group, unmatched_group);
}
```

---

### 19.4 Traceback Representation and `add_note()` Internals

#### 1. Traceback Trees
Because exception groups represent hierarchical trees of errors, CPython's traceback generator is modified to format tracebacks recursively as tree diagrams.

When printed, the output represents the nesting level using visual indicators:

```
  + Exception Group Traceback (most recent call last):
  |   File "example.py", line 4, in <module>
  |     raise ExceptionGroup("Main Group", [ValueError("Error A"), TypeError("Error B")])
  | ExceptionGroup: Main Group (2 sub-exceptions)
  +-+---------------- 1 ----------------
    | ValueError: Error A
    +---------------- 2 ----------------
    | TypeError: Error B
    +-----------------------------------
```

If there are nested exception groups, the branch indicators prefix the output recursively (e.g., `  +-+---------------- 1.1 ----------------`).

#### 2. PEP 678 Exception Notes and C-Level Internals
PEP 678 introduces `BaseException.add_note(note)`, enabling users to attach custom text to exceptions without modifying their instantiation arguments. This is highly valuable for diagnostic tools, test frameworks (like pytest), and tracing asynchronous executions.

When called, `add_note()` stores the string inside a `__notes__` list attribute on the exception object. The C-level implementation handles memory allocations and type checks directly:

```c
/* Objects/exceptions.c */
static PyObject *
BaseException_add_note(PyBaseExceptionObject *self, PyObject *note)
{
    if (!PyUnicode_Check(note)) {
        PyErr_SetString(PyExc_TypeError, "note must be a string");
        return NULL;
    }

    PyObject *dict = self->dict;
    if (dict == NULL) {
        dict = PyDict_New();
        if (dict == NULL) {
            return NULL;
        }
        self->dict = dict;
    }

    PyObject *notes = PyDict_GetItemWithError(dict, &_Py_ID(__notes__));
    if (notes == NULL) {
        if (PyErr_Occurred()) {
            return NULL;
        }
        notes = PyList_New(0);
        if (notes == NULL) {
            return NULL;
        }
        if (PyDict_SetItem(dict, &_Py_ID(__notes__), notes) < 0) {
            Py_DECREF(notes);
            return NULL;
        }
        Py_DECREF(notes);
    }

    if (PyList_Append(notes, note) < 0) {
        return NULL;
    }

    Py_RETURN_NONE;
}
```

At runtime, the traceback printing logic calls `PyObject_GetAttr(exc, &_Py_ID(__notes__))`. If the list is found, it iterates over each note string and prints it directly after the exception traceback and type representation.

---

---

