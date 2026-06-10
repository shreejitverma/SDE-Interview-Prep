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
