# Python 2.2 to 2.3: Type-Class Unification, Descriptors, & C3 MRO


### 4.1 Type-Class Unification: The Inception of New-Style Classes (PEP 252 & PEP 253)
Before Python 2.2, user-defined classes and built-in types existed as entirely separate entities under the hood. This architectural separation introduced severe language inconsistencies.

#### 1. The Classic Class Inconsistency
In the classic class model (pre-Python 2.2):
* **Classic Classes**: Created using the `class MyClass:` syntax without inheriting from any class. All user-defined classes were represented by a single C type, `PyClass_Type`, defined by the `PyClassObject` struct.
* **Classic Instances**: Every instance of any user-defined class was represented by a single C type, `PyInstance_Type`, mapped to a `PyInstanceObject` struct.
* **The Dichotomy**: The `type()` of any classic class instance always returned `<type 'instance'>`, rather than the class itself. The actual class of the instance was accessible only via the `__class__` attribute:
  ```python
  # Python 1.5 - 2.1 Classic Class Behavior
  class Classic:
      pass

  c = Classic()
  print(type(c))       # <type 'instance'>
  print(c.__class__)   # <class '__main__.Classic'>
  ```
* **Built-in Types**: In contrast, built-in types (e.g., `int`, `list`, `dict`, `str`) were instances of `PyType_Type`, represented in C by a static `PyTypeObject` struct.
* **The Subclassing Barrier**: Because of this dichotomy, user-defined classes could not inherit from built-in types. Subclassing `list` or `dict` to extend their behavior was impossible, because the classic class object engine could not parse or manage the memory layouts of built-in C types.

#### 2. The Solution: New-Style Classes and `object`
Introduced in Python 2.2 by PEP 252 and PEP 253, **new-style classes** unified types and classes.
* **The Unified Base**: A new-style class is defined by inheriting (directly or indirectly) from the built-in `object` type (the root of the unified type tree).
* **Type Unification**: Under the unified model, user-defined classes are themselves type objects (instances of `type`), just like built-in types. Checking `type(c)` now returns the class itself.
  ```python
  # Python 2.2+ New-Style Class Behavior
  class NewStyle(object):
      pass

  n = NewStyle()
  print(type(n))  # <class '__main__.NewStyle'>
  ```

#### 3. Low-Level C Representation: `PyTypeObject`
Every class (built-in or new-style user-defined) is represented at the C-level as an instance of `PyTypeObject`. Let's inspect the core fields in CPython's `Include/object.h`:

```c
typedef struct _typeobject {
    PyObject_VAR_HEAD
    const char *tp_name;                 /* For printing, in format "<module>.<name>" */
    Py_ssize_t tp_basicsize, tp_itemsize; /* For allocation sizes */

    /* Methods to implement standard operations */
    destructor tp_dealloc;
    printfunc tp_print;
    getattrfunc tp_getattr;
    setattrfunc tp_setattr;
    
    /* Attribute lookup slot */
    getattrofunc tp_getattro;            /* Pointing to PyObject_GenericGetAttr */
    setattrofunc tp_setattro;            /* Pointing to PyObject_GenericSetAttr */

    /* Protocol slot mappings */
    PyNumberMethods *tp_as_number;
    PySequenceMethods *tp_as_sequence;
    PyMappingMethods *tp_as_mapping;

    /* Inheritance and lookup structures */
    PyObject *tp_dict;                  /* Namespace dictionary */
    PyObject *tp_bases;                 /* Tuple of base classes */
    PyObject *tp_mro;                   /* Method Resolution Order tuple */
    
    /* Allocation / Initialization slots */
    newfunc tp_new;                     /* __new__ allocation entry */
    initproc tp_init;                   /* __init__ initialization entry */
    allocfunc tp_alloc;                 /* Low-level memory allocator */
    
    /* Flags and inheritance info */
    unsigned long tp_flags;
    struct _typeobject *tp_base;        /* Direct base pointer */
} PyTypeObject;
```

#### 4. Slot Wrapper Descriptors
Because C slots (like `tp_init`) expect standard C function signatures (e.g., `int (*tp_init)(PyObject *, PyObject *, PyObject *)`), but user-defined classes write Python methods (`def __init__(self, ...)`), CPython utilizes **slot wrappers** to bridge the boundary:
* **Python to C (Slot Fillers)**: When a new-style class defines a Python method like `__init__`, the type compiler dynamically wraps this method in a C function (such as `slot_tp_init`) and assigns it to the type object's `tp_init` slot. When `tp_init` is called, `slot_tp_init` executes the Python function.
* **C to Python (Wrapper Descriptors)**: To expose built-in C slots (like `object`'s default initialization code) to Python, CPython wraps them in descriptor objects (e.g. `__init__` is exposed as `<slot wrapper '__init__' of 'object' objects>`).

---

### 4.2 The Descriptor Protocol & Attribute Lookup Chain

The descriptor protocol is the underlying mechanism that enables properties, methods, classmethods, and staticmethods in Python.

#### 1. The Descriptor Protocol Definition
A descriptor is an object that implements at least one of the three descriptor protocol methods:
```python
def __get__(self, instance, owner=None):
    """Invoked when getting the attribute."""
    pass

def __set__(self, instance, value):
    """Invoked when setting the attribute."""
    pass

def __delete__(self, instance):
    """Invoked when deleting the attribute."""
    pass
```

* **Data Descriptor**: Implements both `__get__` AND `__set__` (and/or `__delete__`).
* **Non-Data Descriptor**: Implements only `__get__` (e.g. functions, classmethods, staticmethods).

#### 2. The Attribute Lookup Algorithm (`PyObject_GenericGetAttr`)
When an attribute is accessed (`obj.name`) on a new-style instance `obj` of class `Class`, CPython invokes the `tp_getattro` slot of the type object. By default, this points to `PyObject_GenericGetAttr` in `Objects/object.c`. 

The lookup follows this strict resolution flow:
1. **MRO Search**: CPython calls `_PyType_Lookup(Class, name)`. This searches the class's namespace dictionary `tp_dict` and the namespace dictionaries of all parent classes in the Method Resolution Order (`tp_mro`). Let the resolved object be `descr`.
2. **Data Descriptor Check**: If `descr` is found, and its type implements the `__set__` (or `__delete__`) slot (`descr->ob_type->tp_descr_set` is not NULL):
   * Call `descr->ob_type->tp_descr_get(descr, obj, Class)` and return the result immediately.
3. **Instance Dict Lookup**: Check the instance dictionary of `obj` (the `__dict__` table at `obj->ob_dict`). If `name` exists in the instance dictionary, return its associated value.
4. **Non-Data Descriptor Check**: If `descr` is found:
   * If `descr` has a `__get__` slot (`descr->ob_type->tp_descr_get` is not NULL):
     * Call `descr->ob_type->tp_descr_get(descr, obj, Class)` and return the result.
   * If `descr` does not have a `__get__` slot (e.g. a plain class attribute like a string or int):
     * Return `descr` directly.
5. **Fallback to `__getattr__`**: If the attribute is still not resolved, CPython raises an `AttributeError`, which triggers the invocation of the fallback `__getattr__(self, name)` method if defined on the class.

```
       [Start Lookup: obj.name]
                  |
                  v
       [Search MRO for "name"]
                  |
       +----------+----------+
       |                     |
   (Not Found)            (Found)
       |                     |
       |                     v
       |          [Is it a Data Descriptor?]
       |          (has __get__ and __set__)
       |                     |
       |             +-------+-------+
       |             | Yes           | No
       |             v               |
       |       [Call __get__]        |
       |       [Return result]       |
       |                             v
       +--------------------> [Check obj.__dict__]
                                     |
                             +-------+-------+
                             | Found         | Not Found
                             v               |
                       [Return dict val]     v
                                  [Is it a Non-Data Descriptor?]
                                  (has __get__ but no __set__)
                                             |
                                     +-------+-------+
                                     | Yes           | No
                                     v               v
                               [Call __get__]   [Is it class attribute?]
                               [Return result]       |
                                             +-------+-------+
                                             | Yes           | No
                                             v               v
                                       [Return value]   [Raise AttributeError]
                                                             |
                                                        [Call __getattr__]
```

#### 3. C-Level descriptor specifications: `PyGetSetDef`
In C extensions, properties are frequently defined using the `PyGetSetDef` struct in the class's `tp_getset` slot:

```c
typedef struct PyGetSetDef {
    const char *name;
    getter get;             /* C function: PyObject *(*getter)(PyObject *, void *) */
    setter set;             /* C function: int (*setter)(PyObject *, PyObject *, void *) */
    const char *doc;
    void *closure;          /* Context pointer passed to getter/setter */
} PyGetSetDef;
```

#### 4. Practical Implementation of Core Decorators
Here is how decorators leverage descriptors to modify binding behavior:
* **Bound Methods**: Python functions are non-data descriptors. When accessed via `obj.method`, `FunctionType.__get__(func, obj, Class)` is called. It returns a `PyMethod` object wrapping the function and the instance:
  $$\text{obj.method} \equiv \text{Class.method.\_\_get\_\_}(obj, \text{Class})$$
* **`@classmethod`**: Implements `__get__(self, instance, owner)`. When accessed, it ignores the `instance` argument and returns a bound method wrapping the function and the `owner` (the class object itself).
* **`@staticmethod`**: Implements `__get__(self, instance, owner)`. It returns the underlying raw function object directly, bypassing any method binding.
* **`@property`**: A data descriptor that wraps getter, setter, and deleter functions:
  ```python
  class CustomProperty(object):
      def __init__(self, fget, fset=None):
          self.fget = fget
          self.fset = fset

      def __get__(self, instance, owner):
          if instance is None:
              return self
          return self.fget(instance)

      def __set__(self, instance, value):
          if self.fset is None:
              raise AttributeError("can't set attribute")
          self.fset(instance, value)
  ```

---

### 4.3 Method Resolution Order (MRO) & C3 Linearization

Method Resolution Order determines how Python traverses the inheritance tree during attribute search.

#### 1. The Classic DFLR Algorithm & The Diamond Problem
Classic classes (pre-Python 2.2) resolved attributes using a **Depth-First, Left-to-Right (DFLR)** tree traversal.
In a diamond inheritance hierarchy:
```
     A
    / \
   B   C
    \ /
     D
```
The declaration `class D(B, C)` inherits from `B` and `C`, both of which inherit from `A`. 
* DFLR path: `[D, B, A, object, C, A, object]`.
* Removing duplicates keeping only the **first** occurrence gives: `[D, B, A, object, C]`.
* **The Failure**: If `A` defines a method `method()` and `C` overrides it, calling `D().method()` resolves to `A.method()` rather than `C.method()`. This violates the principle that specialized subclasses (`C`) should override generic ancestors (`A`).

#### 2. Python 2.2 MRO: The Last Occurrence Rule & Monotonicity Failure
To fix this, Python 2.2 introduced a new-style MRO calculation:
1. Perform DFLR traversal of the class and all its ancestors.
2. Remove all duplicates except the **last** occurrence.

For the diamond hierarchy `D(B, C)`:
* Traversal: `[D, B, A, object, C, A, object]`.
* Keeping only the **last** occurrence: `[D, B, C, A, object]`.
While this solved the diamond problem, the algorithm was **non-monotonic**.
* **Monotonicity**: If class $X$ precedes $Y$ in the MRO of class $P$, then $X$ must precede $Y$ in the MRO of any subclass $S$ derived from $P$.

##### Samuele Pedroni's Monotonicity Violation Example:
Consider this class configuration in Python 2.2:
```python
class A(object): pass
class B(object): pass
class C(object): pass
class D(object): pass
class E(object): pass

class K1(A, B, C): pass
class K2(D, B, E): pass
class K3(D, A): pass

class Z(K1, K2, K3): pass
```

Let's calculate the Python 2.2 MRO for `K1` and `K2`:
* `K1` raw: `[K1, A, object, B, object, C, object]`. Keeping last: `[K1, A, B, C, object]`. (Here, $A$ precedes $B$).
* `K2` raw: `[K2, D, object, B, object, E, object]`. Keeping last: `[K2, D, B, E, object]`. (Here, $B$ precedes $E$).
* `K3` raw: `[K3, D, object, A, object]`. Keeping last: `[K3, D, A, object]`.

Now let's resolve `Z(K1, K2, K3)` under Python 2.2:
* Raw DFLR list: `[Z, K1, A, object, B, object, C, object, K2, D, object, B, object, E, object, K3, D, object, A, object]`.
* Keeping only the last occurrence:
  `[Z, K1, C, K2, K3, D, B, E, A, object]`.
* **The Monotonicity Failure**:
  * In the parent class `K1`'s MRO, $A$ preceded $B$.
  * In the child class `Z`'s MRO, $B$ precedes $A$ (`... D, B, E, A ...`).
  * The child class has reversed the relative order of $A$ and $B$ established in the parent, violating monotonicity.

#### 3. Python 2.3+ MRO: The C3 Linearization Algorithm
To guarantee monotonicity and local precedence ordering, Python 2.3 adopted **C3 Linearization**.

##### Mathematical Formulation:
Let $L(C)$ be the linearization (MRO) of class $C$. For a class $C$ inheriting from direct parents $B_1, B_2, \dots, B_N$:
$$L(C) = [C] + \text{merge}\left(L(B_1), L(B_2), \dots, L(B_N), [B_1, B_2, \dots, B_N]\right)$$

Where $L(\text{object}) = [\text{object}]$.

##### The Merge Operation:
1. Examine the head (index 0) of the first list inside the merge block: $H = L(B_1)[0]$.
2. If $H$ does not appear in the **tail** (index 1 to the end) of any other list in the merge block, it is a **good head**.
   * Append $H$ to the linearization of $C$.
   * Remove $H$ from all lists in the merge block.
   * Repeat the merge step.
3. If $H$ appears in the tail of any other list, it is not a good head. Move to the next list in the merge block and check its head.
4. If no candidate head can be selected across all lists, the merge is impossible. Python raises a `TypeError`.

##### Step-by-Step Mathematical Calculation of `Z(K1, K2, K3)`:
Let's resolve the MRO of class `Z` from the Pedroni example using C3 Linearization.

We have the parent linearizations:
$$L(K1) = [K1, A, B, C, \text{object}]$$
$$L(K2) = [K2, D, B, E, \text{object}]$$
$$L(K3) = [K3, D, A, \text{object}]$$

Now calculate $L(Z)$:
$$L(Z) = [Z] + \text{merge}\left(L(K1), L(K2), L(K3), [K1, K2, K3]\right)$$
$$L(Z) = [Z] + \text{merge}\left([K1, A, B, C, \text{obj}], [K2, D, B, E, \text{obj}], [K3, D, A, \text{obj}], [K1, K2, K3]\right)$$

**Step 1**: Check head of first list: `K1`. 
* Does `K1` appear in the tail of $[K2, D, B, E, \text{obj}]$, $[K3, D, A, \text{obj}]$, or $[K1, K2, K3]$? No.
* Extract `K1`:
  $$L(Z) = [Z, K1] + \text{merge}\left([A, B, C, \text{obj}], [K2, D, B, E, \text{obj}], [K3, D, A, \text{obj}], [K2, K3]\right)$$

**Step 2**: Check head of first list: `A`.
* Does `A` appear in the tail of other lists? Yes, it appears in the tail of $[K3, D, A, \text{obj}]$. Skip `A`.
* Move to the next list head: `K2`.
* Does `K2` appear in the tail of other lists? No (it only appears at the head of the last list $[K2, K3]$).
* Extract `K2`:
  $$L(Z) = [Z, K1, K2] + \text{merge}\left([A, B, C, \text{obj}], [D, B, E, \text{obj}], [K3, D, A, \text{obj}], [K3]\right)$$

**Step 3**: Check head of first list: `A`.
* Does `A` appear in the tail of other lists? Yes, in the tail of $[K3, D, A, \text{obj}]$. Skip.
* Check head of second list: `D`.
* Does `D` appear in the tail of other lists? Yes, in the tail of $[K3, D, A, \text{obj}]$. Skip.
* Check head of third list: `K3`.
* Does `K3` appear in the tail of other lists? No.
* Extract `K3`:
  $$L(Z) = [Z, K1, K2, K3] + \text{merge}\left([A, B, C, \text{obj}], [D, B, E, \text{obj}], [D, A, \text{obj}]\right)$$

**Step 4**: Check head of first list: `A`.
* Does `A` appear in the tail of other lists? Yes, in the tail of $[D, A, \text{obj}]$. Skip.
* Check head of second list: `D`.
* Does `D` appear in the tail of other lists? No (it is at the head of $[D, A, \text{obj}]$).
* Extract `D`:
  $$L(Z) = [Z, K1, K2, K3, D] + \text{merge}\left([A, B, C, \text{obj}], [B, E, \text{obj}], [A, \text{obj}]\right)$$

**Step 5**: Check head of first list: `A`.
* Does `A` appear in the tail of other lists? No (it is at the head of $[A, \text{obj}]$).
* Extract `A`:
  $$L(Z) = [Z, K1, K2, K3, D, A] + \text{merge}\left([B, C, \text{obj}], [B, E, \text{obj}], [\text{obj}]\right)$$

**Step 6**: Check head of first list: `B`.
* Does `B` appear in the tail of other lists? No.
* Extract `B`:
  $$L(Z) = [Z, K1, K2, K3, D, A, B] + \text{merge}\left([C, \text{obj}], [E, \text{obj}], [\text{obj}]\right)$$

**Step 7**: Check head of first list: `C`.
* Does `C` appear in the tail of other lists? No.
* Extract `C`:
  $$L(Z) = [Z, K1, K2, K3, D, A, B, C] + \text{merge}\left([\text{obj}], [E, \text{obj}], [\text{obj}]\right)$$

**Step 8**: Check head of first list: `obj`.
* Does `obj` appear in the tail of other lists? Yes, in the tail of $[E, \text{obj}]$. Skip.
* Check head of second list: `E`.
* Does `E` appear in the tail of other lists? No.
* Extract `E`:
  $$L(Z) = [Z, K1, K2, K3, D, A, B, C, E] + \text{merge}\left([\text{obj}], [\text{obj}], [\text{obj}]\right)$$

**Step 9**: Extract `obj`:
  $$L(Z) = [Z, K1, K2, K3, D, A, B, C, E, \text{object}]$$

This calculation resolves the Method Resolution Order cleanly, preserving both local precedence and global monotonicity across all classes.

### 4.4 Instance Lifecycle: Allocation vs. Initialization
CPython splits object creation into two phases, controlled by different type slots:
1. **Allocation (`__new__`)**:
   * Maps to the `tp_new` slot in `PyTypeObject`.
   * This is a static method responsible for allocating memory on the heap (using `PyType_GenericNew()`) and initializing the object's `PyObject` headers (`ob_refcnt` and `ob_type`).
   * It must return a new instance of the class (or subclass).
2. **Initialization (`__init__`)**:
   * Maps to the `tp_init` slot in `PyTypeObject`.
   * This is an instance method called immediately after `__new__` returns a valid instance. It populates the instance dictionary `__dict__` with fields.

---
