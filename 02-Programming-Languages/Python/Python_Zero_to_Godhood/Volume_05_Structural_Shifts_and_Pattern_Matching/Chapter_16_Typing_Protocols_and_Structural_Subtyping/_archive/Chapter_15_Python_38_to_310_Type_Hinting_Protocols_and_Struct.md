# Python 3.8 to 3.10: Type Hinting Protocols and Structural Subtyping


### 16.1 Nominal vs. Structural Subtyping
*   **Nominal Subtyping**: Standard object-oriented inheritance. An class `A` is a subclass of `B` only if `A` explicitly inherits from `B` (`class A(B):`).
*   **Structural Subtyping (Protocols / PEP 544)**: Also known as static duck typing. A class `A` is compatible with a Protocol `P` if `A` implements all methods and variables defined in `P` with matching signatures, regardless of explicit inheritance declarations.

---

### 16.2 Runtime Protocols & `@runtime_checkable`
By default, PEP 544 protocols are strictly compile-time constructs erased during execution. However, annotating a protocol with the `@runtime_checkable` decorator permits the use of `isinstance()` and `issubclass()` checks at runtime.

#### 1. Internal Hook Overrides
The `@runtime_checkable` decorator replaces the standard class methods for instance validation. It overrides the metaclass dunder methods:
*   `__instancecheck__`: Called when performing `isinstance(obj, ProtocolClass)`.
*   `__subclasscheck__`: Called when performing `issubclass(SubClass, ProtocolClass)`.

#### 2. Runtime Evaluation Loop
The CPython implementation of `__subclasscheck__` on a runtime checkable protocol executes the following logic:
```python
# Conceptual runtime protocol verification
def __subclasscheck__(cls, subclass):
    if not isinstance(subclass, type):
        raise TypeError("issubclass() arg 1 must be a class")
    
    # Iterate over all defined attributes in the protocol
    for attr in cls._protocol_attrs:
        # Check if the subclass or its MRO implements the attribute
        if not any(attr in s.__dict__ for s in subclass.__mro__):
            return False
    return True
```
> [!WARNING]
> Runtime protocol checks only verify the **existence** of attributes, not their method signatures, types, or parameters. Furthermore, traversing the entire MRO of a subclass for every attribute check incurs significant runtime overhead compared to simple nominal class validations.

---

### 16.3 Static Type Checking and Variance
Static type checkers (such as Mypy or Pyright) compile type annotations into a directed graph of type variables and interfaces.

#### 1. Mathematical Definitions of Variance
Let $A$ and $B$ be types where $A$ is a subtype of $B$ ($A \subseteq B$). Let $F$ be a generic container type constructor (e.g., `List[T]`, `Iterable[T]`, or `Callable[[T], R]`).

*   **Covariance**: The container subtype relationship preserves the parameter type relationship:
    $$F(A) \subseteq F(B)$$
    *Example*: `Iterable[T]` is covariant. A list of dogs can be safely read as a list of animals.
*   **Contravariance**: The container subtype relationship reverses the parameter type relationship:
    $$F(B) \subseteq F(A)$$
    *Example*: `Callable[[T], None]` is contravariant. A function that accepts any animal can safely be used where a function accepting a dog is expected.
*   **Invariance**: There is no relationship between container types:
    $$F(A) \not\subseteq F(B) \quad \text{and} \quad F(B) \not\subseteq F(A)$$
    *Example*: `List[T]` is invariant because it allows both read (covariant) and write (contravariant) operations. Allowing a list of dogs to be treated as a list of animals could let someone insert a cat into the list, violating type safety.

---

