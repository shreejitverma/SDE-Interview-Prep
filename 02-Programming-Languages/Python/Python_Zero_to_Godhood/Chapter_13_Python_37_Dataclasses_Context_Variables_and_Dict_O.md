# Python 3.7: Dataclasses, Context Variables, and Dict Ordering Guarantees


### 13.1 PEP 557: Dataclasses under the hood
Introduced in Python 3.7, the `@dataclass` decorator automates the generation of boilerplate methods (such as `__init__`, `__repr__`, and `__eq__`) by inspecting class type annotations.

#### 1. Import-Time Code Generation Mechanics
Rather than intercepting attribute access or performing dynamic lookups at runtime, `@dataclass` is an import-time code generator:
1.  **Annotation Reading**: When the class is imported, the decorator scans the class's `__annotations__` dictionary to identify the fields and their annotated types.
2.  **String Code Assembly**: The decorator constructs a string representation of the Python source code for the requested magic methods. For example:
    ```python
    def __init__(self, name: str, age: int):
        self.name = name
        self.age = age
    ```
3.  **Compilation and Execution**: It compiles this source string using the built-in `compile(source, "<string>", "exec")` function. It then executes the compiled code block via `exec()` within the class's namespace context to instantiate real function objects.
4.  **Attachment**: The compiled function objects are attached directly to the class's dictionary (`__dict__`).

Because these methods are compiled into native CPython bytecode at import time, calling them at runtime incurs zero wrapper or interception overhead, executing at the exact same speed as manually written boilerplate methods.

#### 2. Specialized Flags and post-init Hooks
*   **Immutability (`frozen=True`)**: If a class is declared with `@dataclass(frozen=True)`, the decorator dynamically generates custom `__setattr__` and `__delattr__` methods:
    ```python
    def __setattr__(self, name, value):
        if type(self) is cls:
            raise FrozenInstanceError(f"cannot assign to field {name!r}")
        super().__setattr__(name, value)
    ```
    This prevents direct attribute mutation. *Note*: Developers can bypass this protection by modifying attributes via `object.__setattr__(self, name, value)`.
*   **The `__post_init__` Hook**: If the class defines a `__post_init__` method, the generated `__init__` function automatically appends a call to `self.__post_init__()` as its final instruction. This is useful for validating fields or computing dependent properties after initialization.

---

### 13.2 PEP 567: Context Variables (`contextvars`)
PEP 567 introduced context variables to provide a safe, high-performance mechanism for managing task-local state in asynchronous runtimes.

#### 1. The Thread-Local Storage Flaw in Async Code
Historically, multi-threaded programs isolated state using thread-local storage (`threading.local`). However, this model breaks down in asynchronous runtimes (like `asyncio`):
*   In `asyncio`, many independent tasks run concurrently on a **single OS thread**, yielding control back and forth.
*   If task A sets a value in thread-local storage and awaits an I/O operation, control yields to task B on the same thread. Task B can read or overwrite task A's thread-local value, causing severe state leakage and race conditions.

#### 2. Hash Array Mapped Trie (HAMT) Internals
Context Variables (`contextvars`) solve this by isolating state per-task using a C-level **Hash Array Mapped Trie (HAMT)** data structure:
*   **Immutability**: A HAMT is an immutable, persistent key-value mapping structure.
*   **Structural Sharing**: When a task modifies a context variable (`var.set(val)`), CPython does not modify the mapping in-place. Instead, it creates a new trie node representing the updated state. Unchanged branches are shared with the previous trie structure (structural sharing), minimizing memory allocation.
*   **Performance**: Due to its shallow 32-way branching factor, HAMT guarantees $O(\log_{32} N) \approx O(1)$ lookup, insertion, and deletion speeds.
*   **$O(1)$ Context Switching**: Because the context state is represented by an immutable HAMT node, saving or restoring a task's context during an `await` suspension is as simple as copying a pointer to the root trie node.

```
HAMT Structural Sharing on Update:
     [ Root A ]                     [ Root B ] (New Context)
      /      \                       /      \
  [ Node1 ] [ Node2 ]            [ Node1 ] [ Node3 ] (New/Updated Entry)
             (Shared Branch)
```

#### 3. Task Context Switching
In `asyncio`, each `Task` holds a reference to its own `contextvars.Context` object containing the HAMT structure.
1.  When suspending at an `await` point, the task yields.
2.  Before resuming a different task, the event loop calls `PyContext_Enter(new_context)` at the C level.
3.  This restores the exact task-local state variables for the active task in $O(1)$ time, completely isolating concurrent execution paths.

---

### 13.3 Dictionary Insertion-Order Guarantee
In Python 3.6, compact dictionaries preserved insertion order as an implementation side-effect. In Python 3.7, this behavior was officially codified as a language specification guarantee.

#### 1. Scope of the Guarantee
The order preservation applies to:
*   Dictionary iteration (e.g. `for key in my_dict`).
*   Views returned by `.keys()`, `.values()`, and `.items()`.
*   Keyword arguments passing (`**kwargs` preserves call-site order).
*   Class namespaces (attributes declared inside class definitions preserve their order inside `__dict__` and `__annotations__`).

#### 2. Impact on the Ecosystem
*   **JSON Serialization**: Serialization (`json.dumps`) becomes deterministic by default.
*   **Deprecation of `collections.OrderedDict`**: While `OrderedDict` remains in the standard library (offering specialized operations like `.move_to_end()`), standard dictionaries are now preferred for general ordered mapping operations.

---

### 13.4 Properties & Cached Properties (Descriptor Protocol)
The descriptor protocol defines how Python handles attribute lookup on objects. Properties and cached properties leverage this protocol to customize attribute access.

#### 1. Data vs. Non-Data Descriptors
A descriptor is an object that implements one or more of the methods: `__get__`, `__set__`, or `__delete__`.
*   **Data Descriptor**: Implements `__set__` or `__delete__`.
*   **Non-Data Descriptor**: Only implements `__get__`.

#### 2. CPython Attribute Resolution Precedence
When resolving `instance.attribute`, the C-level function `PyObject_GenericGetAttr` searches namespaces in a strict order of precedence:
1.  **Class Search**: Search the class MRO for a descriptor. If a descriptor is found and it is a **data descriptor**, call its `__get__` slot and return the result.
2.  **Instance Dictionary**: If no data descriptor is found, check the instance's dictionary (`instance.__dict__`). If the attribute exists, return the value.
3.  **Non-Data Descriptor**: If the attribute is not in `instance.__dict__` but a **non-data descriptor** was found on the class, call its `__get__` slot and return the result.
4.  **Class Attribute**: Check if the attribute exists as a standard class attribute.
5.  **AttributeError**: Raise `AttributeError`.

#### 3. How `functools.cached_property` Exploits Precedence
The built-in `@property` is a **data descriptor** (it defines `__get__` and a default `__set__` that raises an error). Thus, it always overrides instance dictionary lookups.

In contrast, `functools.cached_property` is implemented as a **non-data descriptor** (it only implements `__get__`).

##### Conceptual Implementation of `cached_property`:
```python
class cached_property:
    def __init__(self, func):
        self.func = func
        self.__doc__ = func.__doc__

    def __get__(self, instance, owner):
        if instance is None:
            return self
        # Compute the value
        value = self.func(instance)
        # Write directly to the instance dict
        instance.__dict__[self.func.__name__] = value
        return value
```

##### Lookup Flow:
1.  **First Lookup**: `instance.attribute` is queried. CPython MRO search finds `cached_property` (non-data descriptor). Since it is not in the instance `__dict__`, CPython executes `__get__`. The method computes the value, writes it into `instance.__dict__`, and returns it.
2.  **Subsequent Lookups**: CPython MRO search finds `cached_property` (non-data descriptor). CPython then checks the instance `__dict__` and finds the cached value. Because instance dictionary lookups take precedence over non-data descriptors, the value is returned directly from `__dict__`, bypassing `__get__` completely.

---

### 13.5 Instance Slots Optimization (`__slots__`)
By default, every object instance allocates a dictionary (`__dict__`) to store attributes dynamically. This consumes significant memory.

#### 1. Struct Layout Changes
Defining `__slots__ = ('x', 'y')` inside a class alters CPython's object layout:
*   The type constructor (`PyType_Ready`) suppresses the allocation of `__dict__` and `__weakref__` pointers in the instance's structure.
*   Instead, CPython reserves raw `PyObject*` pointer array slots directly inside the object's struct layout (immediately following `PyObject_HEAD`) for `x` and `y`.

#### 2. Member Descriptors
For each slot defined in `__slots__`, CPython creates a `member_descriptor` object on the class:
*   The descriptor stores a hardcoded byte offset indicating where the variable's pointer resides relative to the head of the object structure in C memory.
*   When executing `obj.x`, the member descriptor accesses the pointer at `(char*)obj + offset` directly, replacing slow string hashing and dictionary lookups with fast C-level array indexing.

#### 3. Trade-offs and Limitations
*   **Memory Savings**: Eliminates the 100-150 byte overhead of the instance `__dict__` hash table, allowing developers to scale to millions of lightweight objects.
*   **Attribute Blocking**: Prevents developers from dynamically adding arbitrary new attributes at runtime (raises `AttributeError`).
*   **Weak References**: Since `__weakref__` is omitted, objects cannot be targets of weak references unless `'__weakref__'` is explicitly included in the `__slots__` tuple.
*   **Subclassing Rules**: Subclasses do not inherit `__slots__`. If a subclass does not declare `__slots__`, it will allocate a standard `__dict__` and `__weakref__`, rendering the parent's memory savings moot for subclass instances.

```python
import sys

class DictClass:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class SlotsClass:
    __slots__ = ('x', 'y')
    def __init__(self, x, y):
        self.x = x
        self.y = y

d = DictClass(1, 2)
s = SlotsClass(1, 2)

print("DictClass size:", sys.getsizeof(d) + sys.getsizeof(d.__dict__))
# SlotsClass has no __dict__, consuming significantly less memory
print("SlotsClass size:", sys.getsizeof(s))
```

---

