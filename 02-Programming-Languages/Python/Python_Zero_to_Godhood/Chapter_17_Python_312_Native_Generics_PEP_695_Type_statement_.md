# Python 3.12: Native Generics (PEP 695), Type statement, and Subinterpreters


### 18.1 PEP 695 Type Parameter Syntax
Python 3.12 introduced a clean, native syntax for generic classes, generic functions, and type aliases using type parameters enclosed in square brackets.

```python
# Generic Function
def reverse[T](items: list[T]) -> list[T]:
    return items[::-1]

# Generic Class
class Stack[T]:
    def __init__(self) -> None:
        self._data: list[T] = []

# Modern Type Alias
type Vector3D[T] = tuple[T, T, T]
```

---

### 18.2 Hidden Scopes and Lazy Evaluation
Historically, generic class constraints (e.g., `T = TypeVar('T', bound=int)`) were evaluated eagerly at module import time, which often led to circular import issues and name resolution errors. 
Under PEP 695, CPython resolves this by evaluating type parameter bounds and type aliases using **hidden compiler-generated scopes**.

#### 1. Scope Encapsulation
When the compiler encounters type parameters or the `type` statement, it wraps the evaluation code inside a nested lexical scope (similar to a hidden function block).
```python
# Conceptual translation of type alias Vector3D[T]
class Vector3D:
    # Under the hood, T is evaluated lazily inside a nested compiler scope
    def __lazy_type_eval__(T):
        return tuple[T, T, T]
```

#### 2. `TypeAliasType` Struct representation
At runtime, the `type` statement creates an instance of `typing.TypeAliasType`. The CPython class definition wraps:
*   `__name__`: Name of the alias.
*   `__value__`: The aliased type (evaluated lazily upon first access via its descriptor getter).
*   `__type_params__`: Tuple of type parameters.

---
