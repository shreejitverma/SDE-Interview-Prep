# Abstract Base Classes (`abc`)


Abstract Base Classes provide a way to define interfaces and enforce that subclasses implement specific methods.

### 64.1 The Virtual Subclassing Mechanism

Normally, `isinstance(obj, Class)` checks the MRO. `abc` allows for "virtual" subclassing using `register()`.
*   **`ABCMeta.__subclasscheck__`**: This dunder method is overridden by the `ABC` metaclass. It allows an object to be considered an instance of an ABC even if it doesn't inherit from it, provided it implements the required protocol.

### 64.2 `@abstractmethod`

This decorator marks a method as abstract.
*   **Internals**: It sets an attribute `__isabstractmethod__ = True` on the function.
*   **Enforcement**: During class instantiation, the C-level `tp_new` check scans the class's dictionary for any attributes with this flag. If found, it raises a `TypeError` preventing instantiation of the abstract class.

---
