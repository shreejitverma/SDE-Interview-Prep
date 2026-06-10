# The Typing System: Static Analysis vs. Runtime Enforcement


Python's type system has evolved from simple comments to a sophisticated language-level feature. This chapter deconstructs how types exist in the runtime.

### 67.1 `typing` Internals: The `GenericAlias` and `SpecialForm`

When you write `list[int]`, you are creating a `types.GenericAlias` object.
*   **The `__getitem__` Hook**: Classes like `list` or `dict` implement `__class_getitem__` to support the bracket syntax.
*   **Runtime Overhead**: Type hints are evaluated at import time. Large-scale use of complex nested types can noticeably slow down the startup of a Python application.

### 67.2 Static vs. Runtime Verification

*   **Static Analysis**: Tools like `mypy` or `pyright` scan the AST (Chapter 31) and verify types without running the code.
*   **Runtime Enforcement**: Libraries like `pydantic` or `beartype` intercept function calls or class instantiation to verify types at execution time.
*   **`inspect.get_type_hints()`**: This function is the "Godhood" way to retrieve types at runtime, handling forward references (strings like `"MyClass"`) by evaluating them in the correct namespace.

### 67.3 Protocols and Structural Subtyping (PEP 544)

Protocols allow for "static duck typing."
*   **Internals**: A `Protocol` class uses a specialized metaclass that identifies which methods define the interface. Unlike `abc.ABC`, you don't need to inherit from the Protocol; you just need to implement the methods.

---
