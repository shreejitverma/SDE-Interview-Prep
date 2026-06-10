# Metaclasses, Descriptor Protocol, and type Slots


### 25.1 The Metaclass Class Creation Pipeline
In Python, classes are objects themselves. A **metaclass** is the class of a class; it defines how classes are constructed. The default metaclass for all objects is `type`.

#### 1. Metaclass Lookup and Construction Path
When CPython executes a class definition block:
```python
class MyClass(metaclass=MyMeta):
    x = 1
```

The VM resolves class creation by running these sequential steps:
1. **Isolate Namespace**: The interpreter executes the class body code inside a temporary namespace dictionary.
2. **Resolve Metaclass**: It checks the inheritance tree for explicit metaclasses. If none are declared, it defaults to `type`.
3. **Execute `__new__`**: It invokes `MyMeta.__new__(meta, name, bases, namespace)`. Under the hood, this calls `type.__new__` which allocates the `PyTypeObject` struct on the heap, setting up C-level slots.
4. **Execute `__init__`**: It initializes class state parameters before returning the type object.

---

### 25.2 The Descriptor Protocol and Attribute Lookup Precedence
A descriptor is an object that customizes attribute access behavior by implementing methods in the descriptor protocol:
*   `__get__(self, instance, owner)`
*   `__set__(self, instance, value)`
*   `__delete__(self, instance)`

#### 1. Data Descriptors vs. Non-Data Descriptors
*   **Data Descriptor**: Implements both `__get__` and `__set__` (or `__delete__`).
*   **Non-Data Descriptor**: Only implements `__get__` (typically used for methods).

#### 2. Attribute Lookup Precedence Hierarchy
When retrieving an attribute `obj.name`, CPython resolves the lookup path in this strict order:

```
                          [Lookup obj.name]
                                 |
           Does "name" exist in class MRO as a Data Descriptor?
                               /   \
                             Yes    No
                             /       \
         [Call descriptor __get__]  Does "name" exist in instance __dict__?
                                       /   \
                                     Yes    No
                                     /       \
                       [Return from __dict__]  Does "name" exist in class MRO
                                               as a Non-Data Descriptor?
                                                 /   \
                                               Yes    No
                                               /       \
                                   [Call descriptor]  Is there a class attribute?
                                                        /   \
                                                      Yes    No
                                                      /       \
                                         [Return value]   Raise AttributeError
```

---

### 25.3 CPython Type Slots
To optimize attribute access and function calls at the C level, CPython uses **Type Slots**. Instead of looking up methods like `__repr__` or `__add__` in the class dictionary at runtime, CPython populates static function pointers directly inside the `PyTypeObject` struct definition:

```c
typedef struct _typeobject {
    PyObject_VAR_HEAD
    const char *tp_name;                 /* For printing, in format <module>.<name> */
    Py_ssize_t tp_basicsize, tp_itemsize; /* For allocation */
    
    /* Type Slots (Static Function Pointers) */
    destructor tp_dealloc;               /* Deallocation handler */
    reprfunc tp_repr;                    /* Representation builder */
    getattrfunc tp_getattr;              /* Attribute lookup hook */
    setattrfunc tp_setattr;              /* Attribute assignment hook */
    
    /* Protocol Table Slots */
    PyNumberMethods *tp_as_number;       /* Math function pointers (e.g. nb_add) */
    PySequenceMethods *tp_as_sequence;   /* Indexing function pointers (e.g. sq_item) */
    PyMappingMethods *tp_as_mapping;     /* Map lookup function pointers (e.g. mp_subscript) */
} PyTypeObject;
```
When a descriptor or custom method is compiled, CPython populates these function pointers. This allows the VM to execute operations (like integer addition `a + b`) using direct C function calls (`tp_as_number->nb_add(a, b)`) instead of slow dictionary lookups.

---

