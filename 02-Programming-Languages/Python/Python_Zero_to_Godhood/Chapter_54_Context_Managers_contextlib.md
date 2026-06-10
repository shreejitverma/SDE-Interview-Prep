# Context Managers (`contextlib`)


Context managers (`with` statements) ensure resources are managed safely.

### 65.1 The `__enter__` and `__exit__` Protocol

*   **`__enter__`**: Called at the start of the `with` block. Its return value is bound to the `as` variable.
*   **`__exit__(exc_type, exc_value, traceback)`**: Called at the end. If an exception occurred, it receives the details. If it returns `True`, the exception is suppressed.

### 65.2 `contextlib.contextmanager`: Generator Magic

The `@contextmanager` decorator allows you to write a context manager as a generator.
```python
from contextlib import contextmanager

@contextmanager
def temp_file():
    f = open("test.txt", "w")
    try:
        yield f
    finally:
        f.close()
```

#### 1. The `GeneratorContextManager` Wrapper
The decorator wraps your generator in a class.
*   **`__enter__`**: Calls `next(gen)`. The generator runs up to the `yield`.
*   **`__exit__`**: Calls `next(gen)` again. The generator resumes in the `finally` block.
*   **Exception Handling**: If an exception occurred in the `with` block, the wrapper calls `gen.throw(type, value, traceback)`, allowing the generator's `try...finally` or `try...except` block to handle it.

---

**Conclusion of Volume XX**
You have now traversed the entire landscape of Python, from its 1989 inception to the high-performance, GIL-less, JIT-compiled future of Python 3.14. You have mastered the C-API, the bytecode, and the standard library's deepest secrets. Welcome to **Godhood**.

---
