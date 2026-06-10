# Python 3.7: Dataclasses, Context Variables, and Dict Ordering Guarantees


### 13.1 PEP 557: Dataclasses under the hood
Introduced in Python 3.7, the `@dataclass` decorator automates boilerplate method generation.
*   **Code Generation**: Rather than using dynamic wrappers at runtime, `@dataclass` is a class decorator that runs at import time. It reads the class's `__annotations__` dictionary and dynamically generates the source code for methods like `__init__`, `__repr__`, and `__eq__`. It then compiles and attaches these methods to the class dict, ensuring runtime performance is identical to manually written methods.

### 13.2 Properties & Cached Properties
*   **The Property Descriptor**: The built-in `@property` decorator is a data descriptor. It intercepts access to attributes and runs custom getter, setter, and deleter methods:

```python
class Circle:
    def __init__(self, radius):
        self._radius = radius

    @property
    def radius(self):
        return self._radius

    @radius.setter
    def radius(self, value):
        if value < 0:
            raise ValueError("Radius cannot be negative")
        self._radius = value
```

*   **Cached Properties**: For expensive computations, `functools.cached_property` caches results. It operates as a non-data descriptor:
    1.  The first access computes the value and writes it directly to the instance's dictionary `__dict__`.
    2.  Subsequent lookups find the value in `__dict__` directly, bypassing the descriptor's getter logic.

### 13.3 Instance Slots Optimization
By default, every instance allocates a dictionary `__dict__` to store attributes. This allows dynamic attribute addition but consumes significant memory.
*   **Slots (`__slots__`)**: Defining `__slots__` inside a class tells CPython to use a fixed-size array instead of a dictionary to store attributes.
*   **CPython Layout**: The class MRO creates descriptor references mapping each slot to a static array offset in C. This:
    1.  Eliminates the memory overhead of the instance dictionary `__dict__` and `__weakref__`.
    2.  Speeds up attribute access by replacing dictionary key lookups with fast array indexing.
    3.  Prevents users from dynamically adding arbitrary new attributes to the instance.

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

