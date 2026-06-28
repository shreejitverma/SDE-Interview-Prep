# Chapter 4: Type–Class Unification, Descriptors, and C3 MRO (Python 2.2–2.3)

Python 2.2 made the single most important change to the object model after its inception: it
**unified types and classes**. Before it, user-defined classes and built-in types were
different kinds of thing at the C level, and you could not subclass `list`. After it, a class
*is* a type, every class descends from `object`, and three mechanisms — the **descriptor
protocol**, the **C3 linearized MRO**, and the **`__new__`/`__init__` split** — became the
machinery underneath properties, methods, `super()`, and `@classmethod`. This chapter is the
origin story and the working reference for all three. It is where the dunder-to-slot wiring of
Chapter 2 becomes a usable protocol you can implement yourself.

> **One home.** Descriptors and the MRO are taught in full here. Metaclasses and the
> construction of `type` itself get their dedicated treatment in Vol VIII; this chapter
> establishes the model they build on and cross-references forward.

## Section Index
- **4.1** New-style classes: unifying types and classes (PEP 252/253)
- **4.2** The descriptor protocol
- **4.3** The attribute-lookup algorithm and descriptor precedence
- **4.4** Methods, `classmethod`, `staticmethod`, and `property` as descriptors
- **4.5** Method Resolution Order and C3 linearization
- **4.6** Instance lifecycle: `__new__` vs `__init__`
- **4.7** Performance, the senior-engineer contrast, and anti-patterns
- **4.8** Summary and cross-references

---

## 4.1 New-style classes: unifying types and classes (PEP 252/253)

**Why this exists.** Before Python 2.2 there were two parallel universes. *Classic classes*
(`class C:`) were all instances of one C type, `PyClass_Type`; their instances were all
`PyInstance_Type`. *Built-in types* (`int`, `list`, `str`) were `PyTypeObject`s. The two could
not mix: `type(c)` of any classic instance returned `<type 'instance'>` rather than the class,
and **you could not subclass a built-in type** — the classic-class machinery had no idea how
to manage a built-in's C memory layout.

**PEP 252 and PEP 253** introduced **new-style classes**: a class that derives (directly or
transitively) from `object` is itself a `PyTypeObject` — an instance of `type`. This collapsed
the two universes into one:

```python
# Caption: in Python 3 every class is new-style; type(instance) IS the class.
class NewStyle:        # implicitly subclasses object in Python 3
    pass

n = NewStyle()
print("type(n):", type(n).__name__)
print("issubclass(NewStyle, object):", issubclass(NewStyle, object))
print("type(NewStyle) is type:", type(NewStyle) is type)
```

Verified output (CPython 3.13.5):

```text
type(n): NewStyle
issubclass(NewStyle, object): True
type(NewStyle) is type: True
```

In Python 3 the classic class is gone entirely — every class is new-style — but the
*mechanisms* unification introduced are exactly what the rest of this chapter dissects. The
key C-level consequence, from Chapter 2: every class is a `PyTypeObject` whose **slots**
(`tp_init`, `tp_new`, `tp_getattro`, `tp_descr_get`, …) hold the function pointers the
interpreter calls. When you write a Python method named `__init__`, CPython installs a **slot
wrapper** (`slot_tp_init`) into `tp_init` that calls your Python function; conversely, a
built-in's C slot is exposed to Python as a **wrapper descriptor**
(`<slot wrapper '__init__' of 'object' objects>`). Python methods and C slots are bridged in
both directions.

---

## 4.2 The descriptor protocol

**Why this exists.** A **descriptor** is an object that customizes what happens when it is
accessed as a *class attribute* of another object. It is the single mechanism behind methods,
`property`, `classmethod`, `staticmethod`, `__slots__` members, and `functools.cached_property`
— and you can implement it yourself. The protocol is three optional methods:

```python
def __get__(self, instance, owner=None): ...   # attribute is read
def __set__(self, instance, value): ...        # attribute is assigned
def __delete__(self, instance): ...            # attribute is deleted
```

The presence of `__set__`/`__delete__` splits descriptors into two kinds, and the distinction
governs lookup precedence (§4.3):

- **Data descriptor** — defines `__set__` and/or `__delete__` (with or without `__get__`).
  `property` is the canonical example.
- **Non-data descriptor** — defines only `__get__`. Plain functions are the canonical example.

```python
# Caption: a minimal data descriptor that logs and validates writes.
class Positive:
    def __set_name__(self, owner, name):      # 3.6+: learn the attribute name
        self.private = "_" + name
    def __get__(self, obj, owner=None):
        if obj is None:
            return self                        # accessed on the class, not an instance
        return getattr(obj, self.private)
    def __set__(self, obj, value):
        if value <= 0:
            raise ValueError("must be positive")
        setattr(obj, self.private, value)

class Account:
    balance = Positive()
    def __init__(self, b):
        self.balance = b

a = Account(100)
print("balance:", a.balance)
try:
    a.balance = -5
except ValueError as e:
    print("rejected:", e)
```

Verified output (CPython 3.13.5):

```text
balance: 100
rejected: must be positive
```

`__set_name__` (PEP 487, Python 3.6) is the modern touch: the descriptor is told its own
attribute name at class-creation time, so it can compute a private backing slot without the
boilerplate the 2.x originals required.

---

## 4.3 The attribute-lookup algorithm and descriptor precedence

**What the interpreter actually does.** Every `obj.name` read goes through the type's
`tp_getattro` slot, which for ordinary objects is `PyObject_GenericGetAttr`
(`Objects/object.c`). Its algorithm is precise and worth memorizing, because the ordering is
the source of every descriptor behavior:

1. **Search the type's MRO** (`_PyType_Lookup`) for `name`; call any result `descr`.
2. **Data-descriptor short-circuit.** If `descr` is a **data descriptor** (its type defines
   `__set__`/`__delete__`), call `descr.__get__(obj, type(obj))` and return — *the instance
   dict is never consulted.*
3. **Instance dict.** Otherwise, if `name` is in `obj.__dict__`, return that value.
4. **Non-data descriptor / class attribute.** Otherwise, if `descr` exists: if it is a
   non-data descriptor, call `descr.__get__(...)`; if it is a plain class attribute, return it.
5. **`__getattr__` fallback.** If nothing was found, raise `AttributeError` — which triggers
   `__getattr__(self, name)` if the class defines it.

```text
obj.name
   │
   ▼
 MRO has a DATA descriptor? ──yes──▶ descr.__get__(obj, type)   (instance dict ignored)
   │ no
   ▼
 name in obj.__dict__? ───────yes──▶ return obj.__dict__[name]
   │ no
   ▼
 MRO has a NON-DATA descriptor? ─yes─▶ descr.__get__(obj, type)
   │ no
   ▼
 MRO has a plain class attr? ───yes──▶ return it
   │ no
   ▼
 raise AttributeError ──▶ __getattr__(self, name) if defined
```

The precedence is observable. A data descriptor wins over an instance-dict entry of the same
name; a non-data descriptor loses to it:

```python
# Caption: data descriptors beat the instance dict; non-data descriptors do not.
class DataDesc:
    def __get__(self, obj, owner): return "data-descriptor"
    def __set__(self, obj, value): pass          # makes it a DATA descriptor
class NonDataDesc:
    def __get__(self, obj, owner): return "non-data-descriptor"

class C:
    d = DataDesc()
    n = NonDataDesc()

c = C()
c.__dict__["d"] = "instance-d"     # attempt to shadow the data descriptor
c.__dict__["n"] = "instance-n"     # attempt to shadow the non-data descriptor
print("c.d ->", c.d)               # data descriptor wins
print("c.n ->", c.n)               # instance dict wins
```

Verified output (CPython 3.13.5):

```text
c.d -> data-descriptor
c.n -> instance-n
```

This is *why* you cannot accidentally clobber a `property` (data descriptor) by assigning to
`self.x` in `__init__` — the property's `__set__` intercepts the assignment — but you *can*
shadow a method (non-data descriptor) by storing a callable in the instance dict. It is also
why instance attributes are normally cheap: they live in the instance dict (step 3) and only
methods/properties pay the MRO walk.

---

## 4.4 Methods, `classmethod`, `staticmethod`, and `property` as descriptors

Every flavor of "method" is a descriptor; the differences are entirely in their `__get__`:

- **Plain function** → non-data descriptor. `obj.method` evaluates
  `function.__get__(obj, type(obj))`, which returns a **bound method** object pairing the
  function with `obj`. `Class.method` calls `__get__(None, Class)` and returns the raw
  function.
- **`@classmethod`** → `__get__` ignores the instance and binds the **class**, so the first
  argument is `cls`.
- **`@staticmethod`** → `__get__` returns the underlying function unchanged; no binding.
- **`@property`** → a **data descriptor** wrapping `fget`/`fset`/`fdel`.

```python
# Caption: a method is a function (non-data descriptor) that binds via __get__.
class Demo:
    def method(self):
        return "called"

d = Demo()
print("type(d.method).__name__:", type(d.method).__name__)       # method (bound)
print("type(Demo.method).__name__:", type(Demo.method).__name__) # function (raw)
print("manual __get__ == d.method:", Demo.method.__get__(d, Demo) == d.method)
```

Verified output (CPython 3.13.5):

```text
type(d.method).__name__: method
type(Demo.method).__name__: function
manual __get__ == d.method: True
```

The bound method holds `obj` in `__self__` and the function in `__func__`; calling it prepends
`__self__` as the first positional argument. This is the entire mechanism of `self` — there is
no magic, only a non-data descriptor returning a small wrapper.

---

## 4.5 Method Resolution Order and C3 linearization

**Why this exists.** With multiple inheritance, "which base defines this method?" needs a
total order over the ancestors that is *consistent*. Classic classes used naive **depth-first,
left-to-right (DFLR)**, which in a diamond reached the shared ancestor *before* a sibling that
overrode it — so the ancestor's method shadowed the override, the opposite of what
specialization demands. Python 2.3 adopted **C3 linearization**, which guarantees two
properties:

- **Local precedence**: bases appear in the order you declared them.
- **Monotonicity**: if `X` precedes `Y` in some class's MRO, `X` precedes `Y` in every
  subclass's MRO too.

**The algorithm.** Let `L(C)` be the MRO of `C` with parents `B₁…Bₙ`:

$$L(C) = [C] + \mathrm{merge}\big(L(B_1), L(B_2), \dots, L(B_n), [B_1, B_2, \dots, B_n]\big)$$

`merge` repeatedly takes a **good head** — the head of some list that does not appear in the
*tail* (everything past the head) of any other list — appends it, and removes it from all
lists. If no good head exists, the hierarchy is inconsistent and CPython raises `TypeError`.

**Worked example (simple diamond).** For `O→{X,Y}→A`:

$$L(X)=[X,O,\text{object}],\quad L(Y)=[Y,O,\text{object}]$$
$$L(A)=[A]+\mathrm{merge}([X,O,\text{object}],[Y,O,\text{object}],[X,Y])$$

Take `X` (not in any tail) → take `Y` (now `O` is blocked because it is in `Y`'s tail until
`Y` is removed) → take `O` → take `object`:

$$L(A)=[A,X,Y,O,\text{object}]$$

```python
# Caption: C3 in practice — confirm the worked result against the real __mro__.
class O: pass
class X(O): pass
class Y(O): pass
class A(X, Y): pass
print("A.__mro__:", [c.__name__ for c in A.__mro__])
```

Verified output (CPython 3.13.5):

```text
A.__mro__: ['A', 'X', 'Y', 'O', 'object']
```

**Worked example (Pedroni's monotonicity stress test).** The classic case that *defeats* the
old "keep last occurrence" 2.2 algorithm but resolves cleanly under C3:

```python
# Caption: the Pedroni example — C3 produces a single consistent order.
class Ao: pass
class Bo: pass
class Co: pass
class Do: pass
class Eo: pass
class K1(Ao, Bo, Co): pass
class K2(Do, Bo, Eo): pass
class K3(Do, Ao): pass
class Z(K1, K2, K3): pass
print("Z.__mro__:", [c.__name__ for c in Z.__mro__])
```

Verified output (CPython 3.13.5):

```text
Z.__mro__: ['Z', 'K1', 'K2', 'K3', 'Do', 'Ao', 'Bo', 'Co', 'Eo', 'object']
```

This matches the hand computation `[Z, K1, K2, K3, D, A, B, C, E, object]` exactly (with
`Do=D`, `Ao=A`, etc.) — local precedence and monotonicity both preserved.

**When C3 fails.** If the constraints are contradictory, there is no consistent order:

```python
# Caption: an impossible hierarchy is rejected at class-creation time.
try:
    class Bad1: pass
    class Bad2(Bad1): pass
    class Bad3(Bad1, Bad2): pass     # demands Bad1 before AND after Bad2
except TypeError as e:
    print("TypeError:", e)
```

Verified output (CPython 3.13.5):

```text
TypeError: Cannot create a consistent method resolution order (MRO) for bases Bad1, Bad2
```

**`super()` follows the MRO, not the parent.** `super().method()` dispatches to the *next*
class in `type(self).__mro__` after the current one — which is why cooperative multiple
inheritance works only when every class calls `super()`. This is the practical payoff of C3:
the MRO is a single linear chain the whole hierarchy agrees on.

---

## 4.6 Instance lifecycle: `__new__` vs `__init__`

CPython splits construction into two slots:

- **`__new__` → `tp_new`**: a *static* method that **allocates** and returns the instance
  (typically via `super().__new__(cls)`, ultimately `PyType_GenericNew`, which sets up the
  `PyObject` header — `ob_refcnt`, `ob_type` — from Chapter 2). It runs first.
- **`__init__` → `tp_init`**: an instance method that **initializes** the already-allocated
  object and must return `None`. It runs only if `__new__` returned an instance of `cls`.

```python
# Caption: __new__ allocates and runs first; __init__ initializes after.
class Lifecycle:
    def __new__(cls, *args, **kwargs):
        print("  __new__ (allocate)")
        return super().__new__(cls)
    def __init__(self, v):
        print("  __init__ (initialize) with", v)
        self.v = v

obj = Lifecycle(7)
print("obj.v:", obj.v)
```

Verified output (CPython 3.13.5):

```text
  __new__ (allocate)
  __init__ (initialize) with 7
obj.v: 7
```

The split is what makes immutable types possible: `int`, `str`, `tuple`, and frozen dataclasses
do their work in `__new__` (you cannot mutate them in `__init__` because there is nothing to
mutate). Override `__new__` to control allocation (singletons, instance caching, subclassing
immutables); override `__init__` for ordinary setup. Overriding `__new__` to return an object
of a *different* type silently skips `__init__` — a common surprise.

---

## 4.7 Performance, the senior-engineer contrast, and anti-patterns

**The senior-engineer contrast.** C++ supports multiple inheritance but resolves it through
virtual inheritance and per-class vtables, with no language-level linearization guarantee — the
notorious diamond requires `virtual` base classes and careful design. Java sidesteps the
problem by forbidding multiple class inheritance (only interfaces, with default methods
resolved by explicit rules). Python's C3 gives multiple inheritance a *single, predictable,
monotonic order* and a cooperative `super()` — more powerful than Java, more principled than
raw C++.

**Attribute-lookup cost and how CPython mitigates it.** Step 1 of §4.3 walks the MRO on every
method access, which would be expensive. CPython caches `_PyType_Lookup` results keyed by a
**type version tag** that is invalidated when the class (or any base) is mutated. On top of
this, the 3.11+ specializing interpreter (Chapter 1; Vol VI) specializes `LOAD_ATTR` with
inline caches, so a monomorphic attribute access on a stable class is nearly free. The
practical rules:

- **`__slots__`** (Vol VIII) removes the per-instance `__dict__`, replacing it with member
  descriptors stored in fixed offsets — less memory and faster attribute access, at the cost
  of dynamic attributes. It is itself implemented with data descriptors, tying back to §4.2.
- **Mutating classes at runtime** (monkeypatching, setting attributes on a class in a hot
  path) invalidates the type version cache and de-optimizes every dependent call site. Prefer
  configuring behavior at class-creation time.

**Anti-patterns.**
- **Forgetting `super().__init__()`** in a multiple-inheritance hierarchy breaks the
  cooperative chain; later classes' initializers silently never run.
- **Implementing only `__get__` when you need write interception**: a non-data descriptor is
  shadowed by an instance attribute of the same name (§4.3). Use a data descriptor.
- **Doing real work in `__init__` for an immutable type**: it must go in `__new__`.
- **Heavy `__getattr__`/`__getattribute__`**: `__getattribute__` runs on *every* attribute
  access and is easy to make accidentally quadratic or recursive (always delegate via
  `super().__getattribute__`).

---

## 4.8 Summary and cross-references

- Python 2.2 **unified types and classes** (PEP 252/253): every class is a `PyTypeObject`
  descending from `object`; Python 3 has only new-style classes.
- A **descriptor** customizes attribute access via `__get__`/`__set__`/`__delete__`.
  **Data descriptors** (with `__set__`/`__delete__`) outrank the instance dict; **non-data
  descriptors** (only `__get__`) do not — this single rule explains methods, `property`,
  `classmethod`, `staticmethod`, and `__slots__`.
- `PyObject_GenericGetAttr` resolves `obj.name` as: data descriptor → instance dict →
  non-data descriptor / class attr → `__getattr__`.
- **C3 linearization** gives multiple inheritance a single order with **local precedence** and
  **monotonicity**; `super()` walks that order; inconsistent hierarchies raise `TypeError`.
- Construction is **`__new__` (allocate) then `__init__` (initialize)**; immutables do their
  work in `__new__`.
- CPython makes lookup fast via the **type version cache** and 3.11+ `LOAD_ATTR`
  specialization; runtime class mutation defeats both.

**Cross-references.** The `PyObject`/`PyTypeObject` header and dunder-to-slot wiring →
Chapter 2. `property` and context managers in idiomatic use → Chapter 5. Metaclasses and the
construction of `type` → Vol VIII. `__slots__`, `weakref`, and attribute-access optimization →
Vol VIII. The specializing interpreter and `LOAD_ATTR` inline caches → Vol VI, Ch 17.
`functools.cached_property` and `cached_property` semantics → Vol IV.
