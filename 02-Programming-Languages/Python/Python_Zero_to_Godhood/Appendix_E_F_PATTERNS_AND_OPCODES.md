# Appendix E: Design Patterns in Python

While Python's dynamic nature makes some classic "Gang of Four" patterns redundant, others are transformed into elegant, language-native idioms.

### E.1 The Singleton Pattern
In Python, the most "Godhood" way to implement a singleton is at the **Module Level**. Since modules are cached in `sys.modules`, any state defined at the top level is shared across the entire process.
*   **Alternative**: Using `__new__` to control instantiation.
```python
class Singleton:
    _instance = None
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
        return cls._instance
```

### E.2 The Factory Pattern
Python's "Everything is an Object" philosophy means classes and functions are first-class citizens. A factory can simply be a dictionary mapping keys to classes.
```python
factories = {
    'fast': FastVector,
    'slow': SlowVector
}
obj = factories['fast'](x=10, y=20)
```

### E.3 The Strategy Pattern
Instead of complex inheritance hierarchies, use **Higher-Order Functions** (Chapter 34). Pass the algorithm as a function/lambda to the consumer.

### E.4 The Observer Pattern
Implemented using the `signals` or `events` pattern. The `weakref` module (Chapter 33) is essential here to prevent the observer registry from keeping objects alive and causing memory leaks.

---

# Appendix F: The Complete Opcodes Reference

This table provides a reference for the most common opcodes in the CPython 3.13 Virtual Machine.

| Opcode | Args | Description |
| :--- | :--- | :--- |
| `CACHE` | 0 | Specialized inline cache entry (skipped by interpreter). |
| `RESUME` | 0 | Internal entry point for functions/generators. |
| `LOAD_CONST` | const_idx | Pushes `co_consts[const_idx]` onto the stack. |
| `LOAD_FAST` | var_num | Pushes local variable `var_num` onto the stack. |
| `STORE_FAST` | var_num | Pops TOS and stores it in local variable `var_num`. |
| `LOAD_GLOBAL` | name_idx | Pushes `co_names[name_idx]` from global/builtin namespace. |
| `BINARY_OP` | op_id | Pops two items, performs operation `op_id` (e.g., ADD, SUB). |
| `BUILD_LIST` | count | Pops `count` items and pushes a new list. |
| `CALL` | argc | Calls a function with `argc` arguments. |
| `COMPARE_OP` | op_id | Performs comparison (e.g., `==`, `<`). |
| `JUMP_FORWARD` | delta | Increments instruction pointer by `delta`. |
| `POP_JUMP_IF_FALSE` | target | Pops TOS; if false, jumps to `target`. |
| `RETURN_VALUE` | 0 | Returns TOS to the caller. |
| `YIELD_VALUE` | 0 | Yields TOS from a generator. |

---
**This concludes the technical reference.**
---
