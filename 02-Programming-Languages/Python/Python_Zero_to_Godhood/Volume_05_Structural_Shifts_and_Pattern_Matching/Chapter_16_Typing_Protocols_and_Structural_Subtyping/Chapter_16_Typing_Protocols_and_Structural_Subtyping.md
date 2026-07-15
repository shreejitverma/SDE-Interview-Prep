# Chapter 16: Typing Protocols and Structural Subtyping (Python 3.8–3.10)

The 3.8–3.10 releases turned Python's type hints from a thin annotation layer into a real gradual
type *system*. The centerpiece is **`Protocol`** (PEP 544): **structural subtyping**, i.e. static
duck typing — a class satisfies an interface by *shape*, not by inheritance. Around it came a
toolbox the modern ecosystem depends on: builtin generics (`list[int]`, PEP 585), `X | Y` unions
(PEP 604), `TypedDict`, `Literal`, and `Final`. This chapter teaches these as the **version features
they are**; the type system as a discipline — variance in depth, `TypeIs`/`TypeGuard`, overloads,
stubs, mypy vs pyright — is the dedicated subject of **Vol XV**, cross-referenced throughout.

## Section Index
- **16.1** Nominal vs. structural subtyping; `Protocol` (PEP 544)
- **16.2** `@runtime_checkable` and its limits
- **16.3** The 3.8–3.10 typing toolbox: generics, unions, TypedDict, Literal, Final
- **16.4** Variance: covariance, contravariance, invariance, and LSP
- **16.5** Type erasure and the gradual-typing boundary
- **16.6** Performance and anti-patterns
- **16.7** Summary and cross-references

---

## 16.1 Nominal vs. structural subtyping; `Protocol` (PEP 544)

**Why this exists.** Classic OO subtyping is **nominal**: `Dog` is an `Animal` only if it *declares*
`class Dog(Animal)`. But Python's runtime has always been duck-typed — "if it has `.render()`, it
renders." Before PEP 544 there was no way to express that contract *statically*: a function taking
"anything with `.render()`" had to be typed `Any`, losing all checking. **`Protocol`** gives
**structural subtyping** — a class matches a protocol if it has the right members, *no inheritance
required*:

```python
# Caption: Book satisfies Renderable structurally, without inheriting from it.
from typing import Protocol, runtime_checkable

@runtime_checkable
class Renderable(Protocol):
    def render(self) -> str: ...

class Book:                                   # no base class relationship to Renderable
    def render(self) -> str:
        return "Book Text"

def display(item: Renderable) -> str:         # static checkers accept any structural match
    return item.render()

print("duck-typed call:", display(Book()))
print("runtime isinstance:", isinstance(Book(), Renderable))
```

Verified output (CPython 3.13.5):

```text
duck-typed call: Book Text
runtime isinstance: True
```

A static checker (mypy/pyright) verifies that `Book` has a `render(self) -> str` with a compatible
signature; at runtime, `Protocol` is built by the `typing._ProtocolMeta` metaclass, which collects
the protocol's member names into `_protocol_attrs`. This is the **senior-engineer contrast**: it is
Go's interfaces (implicit satisfaction by method set) brought to Python, but checked statically —
versus Java/C++ where conforming to an interface requires an explicit `implements`/inheritance
declaration.

---

## 16.2 `@runtime_checkable` and its limits

By default a `Protocol` is a *static-only* construct — `isinstance(obj, SomeProtocol)` raises
`TypeError`. The **`@runtime_checkable`** decorator enables `isinstance`/`issubclass` by overriding
the metaclass's `__instancecheck__`/`__subclasscheck__` to walk the subject's MRO checking that each
`_protocol_attrs` name *exists*. The crucial caveat: it checks **existence only — not signatures or
types**:

```python
# Caption: runtime_checkable verifies attribute NAMES exist, not their signatures.
from typing import Protocol, runtime_checkable

@runtime_checkable
class Renderable(Protocol):
    def render(self) -> str: ...

class BadRender:
    def render(self, a, b):        # wrong signature — but the NAME 'render' exists
        return "wrong sig"
class NoRender:
    pass

print("isinstance ignores signature:", isinstance(BadRender(), Renderable))
print("isinstance(NoRender):", isinstance(NoRender(), Renderable))
```

Verified output (CPython 3.13.5):

```text
isinstance ignores signature: True
isinstance(NoRender): False
```

So `@runtime_checkable` is a *coarse* gate — it answers "does this object have these attribute
names?", not "does it correctly implement the protocol." Use it for capability dispatch
(`isinstance(x, SupportsClose)`), never as a substitute for the static check, and **avoid it in hot
loops**: each call traverses the MRO per protocol attribute, far costlier than a nominal
`isinstance`.

---

## 16.3 The 3.8–3.10 typing toolbox

Five additions in this window are now everyday vocabulary:

**Builtin generics (PEP 585, 3.9).** You can subscript the builtin containers directly —
`list[int]`, `dict[str, int]` — instead of importing `typing.List`/`typing.Dict`. The subscription
produces a `types.GenericAlias` and is purely an annotation; the runtime container is unchanged.

**`X | Y` unions (PEP 604, 3.10).** `int | str` replaces `typing.Union[int, str]`, and — unlike the
old `Union` — it works in `isinstance`:

```python
# Caption: builtin generics (PEP 585) and X | Y unions (PEP 604).
def first(xs: list[int]) -> int:        # no typing.List needed
    return xs[0]
print("list[int] annotation:", first([10, 20]))
print("dict[str,int] type:", type(dict[str, int]).__name__)

def parse(x: int | str) -> str:         # PEP 604 union
    return str(x)
print("union annotation:", parse(5), parse("z"))
print("isinstance with union:", isinstance(5, int | str), isinstance(1.0, int | str))
```

Verified output (CPython 3.13.5):

```text
list[int] annotation: 10
dict[str,int] type: GenericAlias
union annotation: 5 z
isinstance with union: True False
```

**`TypedDict` (PEP 589, 3.8)** types a *dict with a fixed set of string keys and per-key value
types* — at runtime it is a plain `dict`. **`Literal` (PEP 586, 3.8)** restricts a value to specific
constants (`Literal["r", "w"]`). **`Final` (PEP 591, 3.8)** marks a name as non-reassignable to the
checker (not enforced at runtime):

```python
# Caption: TypedDict, Literal, Final — static contracts, ordinary runtime objects.
from typing import TypedDict, Literal, Final, get_type_hints

class Movie(TypedDict):
    title: str
    year: int

m: Movie = {"title": "Inception", "year": 2010}
print("TypedDict at runtime:", type(m).__name__, m)

def set_mode(mode: Literal["r", "w"]) -> str:
    return mode
print("Literal arg:", set_mode("r"))

MAX: Final = 100
print("Final (not runtime-enforced):", MAX)
print("get_type_hints(parse):", get_type_hints(parse))
```

Verified output (CPython 3.13.5):

```text
TypedDict at runtime: dict {'title': 'Inception', 'year': 2010}
Literal arg: r
Final (not runtime-enforced): 100
get_type_hints(parse): {'x': int | str, 'return': <class 'str'>}
```

All five are **static contracts**: a checker enforces them; the runtime sees ordinary objects (a
`dict`, a `str`, an `int`, a union object usable in `isinstance`).

---

## 16.4 Variance: covariance, contravariance, invariance, and LSP

Variance governs how subtyping of a type parameter relates to subtyping of the generic. Let `A ⊆ B`
mean "A is a subtype of B," and `F[T]` a generic:

- **Covariant** — preserves direction: `A ⊆ B ⟹ F[A] ⊆ F[B]`. Read-only producers are covariant:
  an `Iterable[Dog]` *is an* `Iterable[Animal]` (you can only read animals out).
- **Contravariant** — reverses direction: `A ⊆ B ⟹ F[B] ⊆ F[A]`. Consumers are contravariant: a
  `Callable[[Animal], None]` *is a* `Callable[[Dog], None]` (something that accepts any animal can
  stand in where a dog-acceptor is needed).
- **Invariant** — no relationship. **`list[T]` is invariant** because it is *both* readable and
  writable: if `list[Dog]` were a `list[Animal]`, you could `.append(Cat())` into it and corrupt the
  element type.

This is the **Liskov Substitution Principle** made precise: to substitute one function for another,
arguments must be **contravariant** (accept at least as much) and returns **covariant** (promise at
most as much): `A ⊆ B, C ⊆ D ⟹ (B → C) ⊆ (A → D)`. The intuition — "be liberal in what you accept,
conservative in what you return" — is why `list` is invariant and `Iterable` is covariant. Note
PEP 695 (3.12, Vol V/Ch 18) lets the checker *infer* variance, so you rarely write
`TypeVar(..., covariant=True)` by hand anymore. The full treatment is Vol XV.

---

## 16.5 Type erasure and the gradual-typing boundary

Python's typing is **gradual** and **erased**: annotations are *metadata* stored in
`__annotations__` (Chapter 12), never checked or enforced by the interpreter. `list[int]` and `list`
are the same object at runtime; the `[int]` exists only for the checker. Type checking happens
*entirely outside the interpreter* — mypy or pyright reads the source/annotations statically and
reports errors; nothing changes in the bytecode, and there is **zero runtime overhead**.

The corollary: annotations are not validation. `def f(x: int): ...; f("nope")` runs without error.
Runtime enforcement is opt-in via libraries that *read* `__annotations__` — pydantic (validates on
construction), `dataclasses` (with validators), `beartype` (decorator) — covered in Vol XV. The
boundary to internalize: **the checker guarantees consistency of typed code at build time; the
runtime guarantees nothing about types.**

---

## 16.6 Performance and anti-patterns

- **`@runtime_checkable` is coarse and slow** — existence-only, MRO-walking. Don't put it in hot
  paths, and never treat a passing `isinstance` as proof of correct implementation.
- **You cannot `isinstance` a *parameterized* generic** — `isinstance(x, list[int])` raises; only
  the bare class works:

```python
# Caption: isinstance rejects a subscripted generic.
try:
    isinstance([1], list[int])
except TypeError as e:
    print("error:", e)
print("bare list works:", isinstance([1], list))
```

Verified output (CPython 3.13.5):

```text
error: isinstance() argument 2 cannot be a parameterized generic
bare list works: True
```

- **Annotations are not runtime checks** — reach for pydantic/validators when you need enforcement.
- **`list`/`dict` are invariant** — a function typed `list[Animal]` will (correctly) reject a
  `list[Dog]` argument from the checker; type the parameter `Sequence[Animal]` (covariant, read-only)
  if you only read from it.
- **Prefer `Protocol` over ABCs** when you want to accept third-party types you cannot make inherit
  from your base — structural typing decouples the contract from the hierarchy.

---

## 16.7 Summary and cross-references

- **`Protocol`** (PEP 544) gives **structural subtyping** (static duck typing): conformance by
  *shape*, no inheritance — Go-style interfaces checked statically.
- **`@runtime_checkable`** enables `isinstance`/`issubclass` but verifies **attribute existence
  only**, not signatures; it is coarse and slow.
- The **3.8–3.10 toolbox**: builtin generics (`list[int]`, PEP 585), `X | Y` unions (PEP 604, work in
  `isinstance`), `TypedDict` (PEP 589), `Literal` (PEP 586), `Final` (PEP 591) — all **static
  contracts** over ordinary runtime objects.
- **Variance** (covariant producers, contravariant consumers, invariant mutables) follows the
  **Liskov Substitution Principle**; `list` is invariant by necessity.
- Typing is **gradual and erased** — `__annotations__` metadata, checked by external tools, **zero
  runtime cost**, and **not** runtime validation.

**Cross-references.** Annotations as metadata → Chapter 12. PEP 695 generics/type syntax and inferred
variance → Vol V (3.12). The full type system — narrowing, `TypeIs`/`TypeGuard`, overloads,
`ParamSpec`, stubs, mypy vs pyright, gradual-typing strategy → **Vol XV**. Descriptors/ABCs vs
Protocols → Chapter 4 and Vol X. Runtime validation (pydantic) → Vol XV / Vol XVI.

---

*Volume V complete. Volume VI opens the "performance leap & runtime mechanics" era (3.11–3.12):
the specializing adaptive interpreter, PEP 695 generics, and exception groups.*
