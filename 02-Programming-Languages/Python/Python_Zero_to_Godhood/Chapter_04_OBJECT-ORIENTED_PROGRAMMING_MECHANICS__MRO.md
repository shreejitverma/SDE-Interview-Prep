# OBJECT-ORIENTED PROGRAMMING MECHANICS & MRO


### 4.1 Method Resolution Order (MRO) & C3 Linearization
In pre-2.2 versions of Python, classes were defined as "classic classes."
*   **Classic Classes MRO (DFLR)**: Classic class methods were resolved using **Depth-First, Left-to-Right (DFLR)** lookup. In a diamond inheritance hierarchy where classes share a common ancestor, DFLR searches all the way up to the ancestor via the first parent class, before evaluating sibling classes. This resulted in the ancestor overriding custom methods written in sibling classes.
*   **New-Style Classes MRO (C3 Linearization)**: Python 2.2 introduced new-style classes (inheriting from `object`), which calculate their lookup order using **C3 Linearization**. This guarantees:
    1.  **Local Precedence**: Parent classes are searched in the order declared in the class definition.
    2.  **Monotonicity**: A class's MRO respects the relative ordering defined in all its parent classes' MROs.

#### The C3 Algorithm:
Let $L(C)$ be the MRO list of class $C$. For a class $C$ inheriting from parents $B_1, B_2, \dots, B_N$:
$$L(C) = [C] + \text{merge}(L(B_1), L(B_2), \dots, L(B_N), [B_1, B_2, \dots, B_N])$$

The **merge** operation selects a candidate head from the lists:
1.  Look at the head (first element) of the first list being merged.
2.  If this head does not appear in the tail (from index 1 to the end) of any of the other lists being merged, it is a valid candidate.
3.  Add the candidate to $L(C)$, remove it from all merging lists, and repeat the merge step.
4.  If the candidate head is in the tail of any other list, skip it and check the head of the next list in the merge block.
5.  If all lists are empty, the merge completes successfully. If no candidate head can be selected, the hierarchy is invalid (raises a `TypeError: Cannot create a consistent method resolution order`).

#### Mathematical Step-by-Step Calculation:
Let's compute the MRO for the following class definitions:
```python
class O(object): pass
class X(O): pass
class Y(O): pass
class A(X, Y): pass
```
We know:
$$L(O) = [O, \text{object}]$$
$$L(X) = [X] + \text{merge}(L(O), [O]) = [X] + \text{merge}([O, \text{object}], [O]) = [X, O, \text{object}]$$
$$L(Y) = [Y] + \text{merge}(L(O), [O]) = [Y, O, \text{object}]$$

Now we compute $L(A)$:
$$L(A) = [A] + \text{merge}(L(X), L(Y), [X, Y])$$
$$L(A) = [A] + \text{merge}([X, O, \text{object}], [Y, O, \text{object}], [X, Y])$$

1.  Evaluate head of first list: `X`. Is `X` in the tail of $[Y, O, \text{object}]$ or $[X, Y]$? No (it is only at the head of $[X, Y]$). We extract `X`:
    $$L(A) = [A, X] + \text{merge}([O, \text{object}], [Y, O, \text{object}], [Y])$$
2.  Evaluate head of first list: `O`. Is `O` in the tail of $[Y, O, \text{object}]$ or $[Y]$? Yes, `O` is in the tail of $[Y, O, \text{object}]$ (index 1). We cannot extract it.
3.  Move to the next list $[Y, O, \text{object}]$. Evaluate head: `Y`. Is `Y` in the tail of $[O, \text{object}]$ or $[Y]$? No. We extract `Y`:
    $$L(A) = [A, X, Y] + \text{merge}([O, \text{object}], [O, \text{object}])$$
4.  Evaluate head: `O`. Is `O` in the tail of any remaining list? No. Extract `O`:
    $$L(A) = [A, X, Y, O] + \text{merge}([\text{object}], [\text{object}])$$
5.  Extract `object`:
    $$L(A) = [A, X, Y, O, \text{object}]$$

### 4.2 The Descriptor Protocol & Attribute Lookup Chain
CPython routes all attribute lookups (`obj.name`) through the default C function `object___getattribute__()` (which corresponds to `object.__getattribute__`).

#### Data vs. Non-Data Descriptors:
*   **Data Descriptor**: Implements both `__get__` AND `__set__` (or `__delete__`).
*   **Non-Data Descriptor**: Implements only `__get__` (e.g., standard methods and functions).

#### The Complete Attribute Lookup Resolution Chain:
When evaluating `obj.name`, CPython follows this strict precedence order:
1.  **Data Descriptor**: Checks the class MRO for a matching data descriptor. If found, returns `Descriptor.__get__(descriptor, obj, Class)`.
2.  **Instance Dict**: Checks the instance's dictionary `obj.__dict__` directly. If the key exists, returns it.
3.  **Non-Data Descriptor / Class Variable**: Checks the class MRO for a matching non-data descriptor or standard class variable. If a non-data descriptor is found, returns `Descriptor.__get__(descriptor, obj, Class)`. If a class variable is found, returns its raw value.
4.  **Fallback (Class Dict)**: If still unresolved, checks the class namespace dictionary for a plain value.
5.  **Fallback Method (`__getattr__`)**: If no attribute is found, throws `AttributeError` unless the class implements `__getattr__`, which is then invoked as a fallback.

```
Attribute Lookup Flowchart:
[Lookup obj.name]
       |
       v
[Class MRO has Data Descriptor?] --Yes--> [Call Descriptor.__get__]
       | No
       v
[Instance dict obj.__dict__ has key?] --Yes--> [Return value from dict]
       | No
       v
[Class MRO has Non-Data Descriptor?] --Yes--> [Call Descriptor.__get__]
       | No
       v
[Class MRO has plain class variable?] --Yes--> [Return class variable]
       | No
       v
[Class defines __getattr__?] --Yes--> [Call __getattr__]
       | No
       +-----------------------------> [Raise AttributeError]
```

#### Method Binding Mechanics:
Python methods are plain functions attached to a class dictionary. Because functions implement only `__get__`, they are **non-data descriptors**. 
*   **Bound Methods**: When we call `obj.method()`, `method` is loaded via `__get__`. The function's `__get__` method wraps the function and the instance together, returning a temporary `PyMethod` object where `self` is bound to the first argument:
    `obj.method` is equivalent to `Class.method.__get__(obj, Class)`.
*   **Unbound Functions**: If called directly on the class (`Class.method`), `__get__` is invoked with `None` as the instance, returning the raw function object itself.

```python
class Demo:
    def method(self):
        pass

d = Demo()
# Bound method check
print("Bound Method:", d.method)
print("Is bound method object?", type(d.method).__name__)

# Verification of descriptors conversion
bound_manually = Demo.method.__get__(d, Demo)
print("Bound manually matches?", bound_manually == d.method) # True
```

### 4.3 Instance Lifecycle: Allocation vs. Initialization
CPython splits object creation into two phases, controlled by different slots:
1.  **Allocation (`__new__`)**:
    *   Maps to the `tp_new` C slot.
    *   This is a static method responsible for allocating memory on the heap (using `PyType_GenericNew()`) and initializing the object's `PyObject` headers (`ob_refcnt` and `ob_type`).
    *   Must return a new instance of the class (or subclass).
2.  **Initialization (`__init__`)**:
    *   Maps to the `tp_init` C slot.
    *   This is an instance method called immediately after `__new__` returns a valid instance. It populates the instance dictionary `__dict__` with variables.

---
