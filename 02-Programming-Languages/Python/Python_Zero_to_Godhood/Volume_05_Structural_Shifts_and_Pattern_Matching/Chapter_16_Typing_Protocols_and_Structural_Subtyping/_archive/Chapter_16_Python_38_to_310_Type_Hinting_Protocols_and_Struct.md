# Python 3.8 to 3.10: Type Hinting Protocols and Structural Subtyping


### 16.1 Nominal vs. Structural Subtyping
Python type hints support two distinct typing paradigms: Nominal Subtyping and Structural Subtyping (implemented via **Protocols** / PEP 544).

#### 1. Nominal Subtyping
Nominal subtyping resolves type compatibility based on explicit inheritance hierarchies. A class `Dog` is considered a subtype of `Animal` if and only if it explicitly inherits from it:
```python
class Animal:
    def breathe(self) -> None:
        pass

class Dog(Animal):  # Explicitly nominal
    pass
```
Under this model, even if a class defines all methods of `Animal`, static type checkers will reject it if there is no explicit base class relationship.

#### 2. Structural Subtyping (Static Duck Typing)
Structural subtyping resolves type compatibility based on the structure (methods and attributes) of the class rather than its name. This is defined using `typing.Protocol`:

```python
from typing import Protocol

class Renderable(Protocol):
    def render(self) -> str:
        ...

class Book:
    # Book does NOT explicitly inherit from Renderable
    def render(self) -> str:
        return "Book Text"

def display(item: Renderable) -> None:
    print(item.render())

# Static type checkers accept this call
display(Book())
```
Type checkers verify that the interface `Book` implements all attributes and methods declared in the protocol `Renderable` with matching signatures and types.

---

### 16.2 Runtime Protocols & `@runtime_checkable`
By default, protocols are static constructs erased during compilation. At runtime, evaluating `isinstance(Book(), Renderable)` raises a `TypeError`. However, decorating a protocol with `@runtime_checkable` enables standard runtime checks.

#### 1. The `_ProtocolMeta` Metaclass Internals
When a class inherits from `typing.Protocol`, CPython uses the custom metaclass `typing._ProtocolMeta` (`Lib/typing.py`).
1.  **Attribute Collection**: During class creation, `_ProtocolMeta` scans the namespace dict and parent MRO to collect all declared non-dunder attributes (methods, properties, and variable annotations).
2.  **Caching**: It caches these names in a private set on the class structure named `_protocol_attrs`.

#### 2. Metaclass Dunder Overrides
`@runtime_checkable` overrides the dunder methods of `_ProtocolMeta`:
*   **`__instancecheck__`**: Overrides `isinstance()`.
*   **`__subclasscheck__`**: Overrides `issubclass()`.

##### CPython `__subclasscheck__` Protocol Logic:
```python
def __subclasscheck__(cls, subclass):
    if not isinstance(subclass, type):
        raise TypeError("issubclass() arg 1 must be a class")
    
    # Fast path: nominal subclass checks
    if super().__subclasscheck__(subclass):
        return True
        
    # Slow path: structural MRO checking
    for attr in cls._protocol_attrs:
        # Check if attribute exists in the subclass dictionary or any of its MRO parents
        for entry in subclass.__mro__:
            if attr in entry.__dict__:
                break
        else:
            return False  # Attribute missing in the entire MRO chain
    return True
```

##### Critical Runtime Considerations:
> [!WARNING]
> *   **Omission of Type Safety**: Runtime `isinstance` checks only verify the **existence** of the attribute names. They do not inspect method signatures, parameter counts, or variable types. A class implementing `def render(self, a, b): pass` will successfully pass an `isinstance(obj, Renderable)` check, even though it violates the protocol signature statically.
> *   **Performance Penalties**: Traversing the `__mro__` and inspecting `__dict__` for every protocol attribute introduces considerable execution overhead. Avoid using runtime protocol checks inside critical loops.

---

### 16.3 Static Type Checking and Variance
Static type checkers compile type annotations into a directed graph of subtypes to enforce type boundaries. The relationships between generic types depend on **Variance**.

#### 1. Mathematical Definition of Variance
Let $A$ and $B$ be types where $A$ is a subtype of $B$ ($A \subseteq B$). Let $F$ be a generic container type constructor (e.g., `List[T]`, `Iterable[T]`, or `Callable[[T], R]`).
*   **Covariance**: The container type preserves the subtype relationship:
    $$A \subseteq B \implies F(A) \subseteq F(B)$$
    In Python, this is defined via `T = TypeVar('T', covariant=True)`.
    *Example*: `Iterable[T]` is covariant. Because `Dog` $\subseteq$ `Animal`, `Iterable[Dog]` $\subseteq$ `Iterable[Animal]`. A list of dogs can be safely read as a list of animals.
*   **Contravariance**: The container type reverses the subtype relationship:
    $$A \subseteq B \implies F(B) \subseteq F(A)$$
    In Python, this is defined via `T = TypeVar('T', contravariant=True)`.
    *Example*: `Callable[[T], None]` arguments are contravariant. A function that accepts any `Animal` can be safely used where a function accepting a `Dog` is expected:
    $$\text{Callable[[Animal], None]} \subseteq \text{Callable[[Dog], None]}$$
*   **Invariance**: There is no relationship between container types:
    $$F(A) \not\subseteq F(B) \quad \text{and} \quad F(B) \not\subseteq F(A)$$
    In Python, standard generic classes are invariant by default.
    *Example*: `List[T]` is invariant because it allows both read (covariant) and write (contravariant) operations. Allowing a list of dogs to be treated as a list of animals could let someone insert a cat into the list, violating type safety.

#### 2. Liskov Substitution Principle (LSP)
The variance of function arguments and return values is derived from the **Liskov Substitution Principle**: if $S$ is a subtype of $T$, then objects of type $T$ may be replaced with objects of type $S$ without altering any of the desirable properties of the program.

Applying LSP to function subtypes leads to this mapping:
$$A \subseteq B \text{ and } C \subseteq D \implies (B \to C) \subseteq (A \to D)$$
This implies that for a function to substitute another:
1.  **Arguments must be Contravariant**: It must accept a broader set of inputs. (Narrowing input scope is unsafe).
2.  **Return values must be Covariant**: It must guarantee a narrower set of outputs. (Broadening output scope is unsafe).

```
Liskov Function Subtyping Mapping:
Input (Contravariant):   Animal (Parent)  ----->   Dog (Child)
                                                     |
                                                     v
Output (Covariant):       Dog (Child)     ----->   Animal (Parent)
```

#### 3. Compile-Time Type Erasure
CPython compiles type annotations into bytecode, but completely ignores them during execution (unless checked by runtime libraries via `__annotations__`). 
Static type checking is entirely completed during pre-run compilation. Once bytecode is emitted, generic variables (e.g. `List[int]`) revert back to standard untyped collections (e.g. `List`) in memory, ensuring that typing extensions introduce zero runtime memory overhead.

---

