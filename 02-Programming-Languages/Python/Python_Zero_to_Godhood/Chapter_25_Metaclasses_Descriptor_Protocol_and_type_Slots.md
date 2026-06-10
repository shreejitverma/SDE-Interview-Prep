# Metaclasses, Descriptor Protocol, and type Slots


### 25.1 The Metaclass Class Creation Pipeline
In Python, classes are objects themselves. A **metaclass** is the class of a class; it defines how classes are constructed. The default metaclass for all objects is `type`.

When CPython executes a class definition block:

```python
class MyClass(BaseClass, metaclass=MyMeta):
    x = 1
```

The VM resolves class creation by running these sequential steps:

#### 1. Namespace Preparation (`__prepare__`)
Before executing the class body code, CPython checks if the metaclass has a `__prepare__` method. If present, it calls:

```python
namespace = MyMeta.__prepare__('MyClass', (BaseClass,))
```

This returns a mapping object (usually a standard empty dictionary, but it can be a custom namespace like a `collections.OrderedDict` or a custom dictionary subclass). The interpreter then executes the class body code within this namespace object, populating it with attributes (methods, class variables, annotations).

#### 2. Class Object Allocation (`__new__`)
After executing the class body, CPython invokes the metaclass `__new__` method to allocate the class object:

```python
cls = MyMeta.__new__(MyMeta, 'MyClass', (BaseClass,), namespace)
```

At the C level, this redirects to the type object's allocation slot `PyType_Type.tp_new` (implemented in `Objects/typeobject.c` as `type_new`). This allocates a new `PyTypeObject` struct on the heap, setting up its class fields, base classes, and MRO (Method Resolution Order).

#### 3. Class Initialization (`__init__`)
Once the class object is allocated, CPython initializes it by calling:

```python
MyMeta.__init__(cls, 'MyClass', (BaseClass,), namespace)
```

This allows the metaclass to inspect or modify the newly created class object before returning it.

---

### 25.2 The Descriptor Protocol and Attribute Lookup Precedence
A descriptor is an object that customizes attribute access behavior by implementing methods in the descriptor protocol:
*   `__get__(self, instance, owner)`: Customizes attribute reads.
*   `__set__(self, instance, value)`: Customizes attribute writes.
*   `__delete__(self, instance)`: Customizes attribute deletions.

#### 1. Data Descriptors vs. Non-Data Descriptors
*   **Data Descriptor**: Implements both `__get__` and `__set__` (or `__delete__`).
*   **Non-Data Descriptor**: Only implements `__get__` (typically used for methods).

This separation is critical because it dictates how the attribute lookup algorithm prioritizes the descriptor over instance dictionaries.

#### 2. Attribute Lookup Precedence Hierarchy
When retrieving an attribute `obj.name` (where `obj` is an instance of class `C`), CPython resolves the lookup path in this strict order:

1.  **Class MRO Search for Data Descriptor**: Search the Method Resolution Order (MRO) of class `C` for an attribute named `name`. If found, and it is a **Data Descriptor**, call its `__get__` method and return the result:
    
    $$\text{result} = \text{DataDescriptor}.\_\_get\_\_(\text{desc}, \text{obj}, \text{C})$$

2.  **Instance Dictionary Search**: Search the instance dictionary (`obj.__dict__`). If `name` is present, return the value directly, bypassing non-data descriptors:
    
    $$\text{result} = \text{obj}.\_\_dict\_\_[\text{'name'}]$$

3.  **Class MRO Search for Non-Data Descriptor**: Search the MRO of class `C` for an attribute named `name`. If found, and it is a **Non-Data Descriptor**, call its `__get__` method:
    
    $$\text{result} = \text{NonDataDescriptor}.\_\_get\_\_(\text{desc}, \text{obj}, \text{C})$$

4.  **Class Attributes**: Search the MRO of class `C` for a standard class attribute. If found, return the value directly.
5.  **Fallback to `__getattr__`**: If the attribute is not found, and the class defines `__getattr__`, call it:
    
    $$\text{result} = \text{obj}.\_\_getattr\_\_(\text{'name'})$$

6.  **Raise AttributeError**: If all steps fail, raise `AttributeError`.

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
To optimize attribute access and function calls at the C level, CPython uses **Type Slots**. Instead of looking up methods like `__repr__` or `__add__` in the class dictionary at runtime, CPython populates static C function pointers directly inside the `PyTypeObject` struct definition:

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

#### 1. Protocol Sub-Tables
CPython groups protocol-specific slots into secondary struct tables pointed to by fields inside `PyTypeObject`:

*   **`PyNumberMethods`**: Contains pointers for numeric operators (e.g., `nb_add` for `+`, `nb_multiply` for `*`):
    
    ```c
    typedef struct {
        binaryfunc nb_add;
        binaryfunc nb_subtract;
        binaryfunc nb_multiply;
        /* ... */
    } PyNumberMethods;
    ```

*   **`PySequenceMethods`**: Contains pointers for sequence operations (e.g., `sq_item` for indexing, `sq_length` for length):
    
    ```c
    typedef struct {
        lenfunc sq_length;
        binaryfunc sq_concat;
        ssizeargfunc sq_item;
        /* ... */
    } PySequenceMethods;
    ```

*   **`PyMappingMethods`**: Contains pointers for mapping lookups (e.g., `mp_subscript` for key lookups, `mp_ass_subscript` for key updates):
    
    ```c
    typedef struct {
        lenfunc mp_length;
        binaryfunc mp_subscript;
        objobjargproc mp_ass_subscript;
    } PyMappingMethods;
    ```

#### 2. Slot Inheritance
When a class is created:
1.  CPython copy-inherits these function pointer slots from the base classes defined in MRO.
2.  If the subclass overrides a method (e.g., defines `def __repr__(self):`), CPython replaces the slot pointer (`tp_repr`) with a wrapper function (`slot_tp_repr`) that calls the Python method.
3.  This inheritance ensures that C-level calls (like `a + b` executing `tp_as_number->nb_add(a, b)`) execute without dictionary searches, optimizing runtime speed.

---

