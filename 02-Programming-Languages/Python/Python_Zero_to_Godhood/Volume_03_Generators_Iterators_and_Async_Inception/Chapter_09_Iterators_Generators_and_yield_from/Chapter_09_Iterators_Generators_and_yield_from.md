# Chapter 9: Iterators, Generators, and `yield from` (Python 3.3)

This chapter is the canonical home for the **iteration model** that pervades Python — `for`
loops, comprehensions, unpacking, `in`, and ultimately `async for` all rest on it. We build it
from the bottom: the **iterator protocol** (`__iter__`/`__next__`), then **generators** as the
language's ergonomic way to write iterators by suspending a frame, then **`yield from`** (PEP
380), the delegation operator that turned generators into composable coroutines and laid the
last stone before native `async`/`await` (Chapter 11). We close with **implicit namespace
packages** (PEP 420), the other headline 3.3 feature. Chapter 5 introduced generator
`send`/`throw`/`close` at the 2.5 milestone; here we complete the model.

## Section Index
- **9.1** The iterator protocol
- **9.2** Generators: iterators written as suspended frames
- **9.3** `yield from`: delegation and the coroutine bridge (PEP 380)
- **9.4** Implicit namespace packages (PEP 420)
- **9.5** Performance and anti-patterns
- **9.6** Summary and cross-references

---

## 9.1 The iterator protocol

**Why this exists.** Python decouples *consuming* a sequence from *being* a sequence, so the same
`for` loop drives a list, a file, a database cursor, an infinite counter, or a network stream.
The contract has two halves:

- An **iterable** implements `__iter__()`, returning a fresh **iterator**.
- An **iterator** implements `__next__()`, returning the next item or raising `StopIteration`,
  and `__iter__()` returning `self` (so an iterator is also iterable).

```python
# Caption: a hand-written iterator; for-loops speak exactly this protocol.
class Countdown:
    def __init__(self, n):
        self.n = n
    def __iter__(self):
        return self
    def __next__(self):
        if self.n <= 0:
            raise StopIteration
        self.n -= 1
        return self.n + 1

print("iterate:", list(Countdown(3)))
```

Verified output (CPython 3.13.5):

```text
iterate: [3, 2, 1]
```

**What the interpreter actually does.** A `for` loop compiles to `GET_ITER` (call `__iter__` once)
followed by a `FOR_ITER` loop (call `__next__`, jump out on `StopIteration`):

```python
# Caption: the bytecode of any for-loop.
import dis
def loop(xs):
    for x in xs:
        pass
dis.dis(loop)
```

Verified output (CPython 3.13.5):

```text
 14           LOAD_FAST                0 (xs)
              GET_ITER
      L1:     FOR_ITER                 3 (to L2)
              STORE_FAST               1 (x)
              JUMP_BACKWARD            5 (to L1)
      L2:     END_FOR
              POP_TOP
              RETURN_CONST             0 (None)
```

`StopIteration` is *control flow*, not an error — `FOR_ITER` catches it internally to end the
loop (this is why a stray `StopIteration` raised *inside* a generator body is a bug; PEP 479,
Chapter handled in Vol III, converts it to `RuntimeError`).

**The senior-engineer contrast.** Java separates `Iterable` (`iterator()`) from `Iterator`
(`hasNext()`/`next()`) — a *look-ahead* model. Python has no `hasNext()`; the iterator is
**exhaustion-by-exception** (`StopIteration` on `next()`). C++ iterators are *positions* compared
against an `end()` sentinel. Python's model is the most minimal: one method, one sentinel
exception, and a hard rule that **iterators are single-pass and stateful** — once exhausted, they
stay exhausted (§9.5).

---

## 9.2 Generators: iterators written as suspended frames

**Why this exists.** Writing the `Countdown` class above is tedious; the state machine
(`self.n`, the `StopIteration`) is boilerplate. A **generator function** — any `def` containing
`yield` — lets you write the *logic* and have CPython synthesize the iterator. Calling it returns
a generator object; each `next()` runs to the next `yield` and suspends.

```python
# Caption: a generator is a lazy iterator; its frame lives on the heap between yields.
def gen_squares(n):
    for i in range(n):
        yield i * i

g = gen_squares(4)
print("type:", type(g).__name__, "| has a live frame:", g.gi_frame is not None)
print("values:", list(g))
```

Verified output (CPython 3.13.5):

```text
type: generator | has a live frame: True
values: [0, 1, 4, 9]
```

**What the interpreter actually does** (the mechanism from Chapter 5, now named). A generator
wraps a `PyGenObject` holding a `gi_frame`. At each `yield`, CPython saves the resume offset
(`f_lasti`) and the evaluation-stack depth in that frame, marks the generator suspended, and
returns control — the frame is *not* freed, it lives on the heap. `next()`/`send()` re-attach the
frame and resume. This is what makes generators **lazy and *O*(1) in memory**: `gen_squares(10**9)`
allocates one frame, not a billion-element list. The two-way protocol — `send(value)`,
`throw(exc)`, `close()` — was covered in Chapter 5; a generator is a producer (`next`) *and* a
consumer (`send`).

---

## 9.3 `yield from`: delegation and the coroutine bridge (PEP 380)

**Why this exists.** Before 3.3, a generator that wanted to yield everything from a sub-generator
*and* correctly forward `send`/`throw`/`close` and capture the sub-generator's return value had to
hand-write a fragile loop. **PEP 380's `yield from`** makes the delegating generator a transparent
channel to the sub-generator:

- values the sub-generator yields pass straight to the caller;
- values the caller `send`s pass straight to the sub-generator;
- exceptions the caller `throw`s are raised inside the sub-generator;
- `close()` propagates;
- and when the sub-generator returns, its **return value becomes the value of the `yield from`
  expression** (carried out via `StopIteration.value`).

```python
# Caption: yield from delegates send/throw and captures the sub-generator's return value.
def subgen():
    received = yield "a"
    yield f"got {received}"
    return "subgen-result"

def delegator():
    result = yield from subgen()       # transparent channel; captures the return
    print("delegator captured return:", result)
    yield "after"

d = delegator()
print("next:", next(d))                 # -> "a" (from subgen)
print("send:", d.send("X"))             # "X" routed into subgen -> "got X"
print("next:", next(d))                 # subgen returns; result captured; -> "after"
```

Verified output (CPython 3.13.5):

```text
next: a
send: got X
delegator captured return: subgen-result
next: after
```

The compiler implements this with `GET_YIELD_FROM_ITER` (prepare the sub-iterator) and a
delegation loop; the value/exception routing lives in `ceval.c`. The everyday use is far simpler —
flattening and composition without manual loops:

```python
# Caption: yield from composes generators cleanly (recursive flatten).
def flatten(nested):
    for item in nested:
        if isinstance(item, list):
            yield from flatten(item)
        else:
            yield item

print("flatten:", list(flatten([1, [2, [3, 4], 5], 6])))
```

Verified output (CPython 3.13.5):

```text
flatten: [1, 2, 3, 4, 5, 6]
```

**The coroutine bridge.** `yield from` made it possible to write a generator that delegates to
another generator that performs I/O and yields control upward — the exact pattern early `asyncio`
used (`@asyncio.coroutine` + `yield from`) before Python 3.5 gave it dedicated syntax. When you
read `await expr` in Chapter 11, picture `yield from` with a typed, awaitable-only channel: the
suspension mechanism is the same heap frame you have been building since Chapter 5.

---

## 9.4 Implicit namespace packages (PEP 420)

**Why this exists.** Through 3.2, a directory was a package only if it contained `__init__.py`.
That blocked a real use case: distributing one logical package (`company.core`, `company.plugins`)
across **multiple installs / directories** — each separate distribution would need its own
`company/__init__.py`, and they would collide. **PEP 420** lets a package span directories with
*no* `__init__.py`: a **namespace package**.

**The finder algorithm.** During `import foo`, `PathFinder` scans `sys.path`. If it finds a
`foo/__init__.py`, that is a normal (regular) package and the search stops. If it finds *no*
`__init__.py` but one or more directories named `foo`, it does **not** fail — it accumulates *all*
matching directories into a namespace package whose `__path__` is the list of them and whose
`__file__` is `None`.

```python
# Caption: one namespace package 'company' spanning two unrelated directories on sys.path.
# (Directory layout: path1/company/core/__init__.py and path2/company/extra/__init__.py,
#  with NO company/__init__.py in either path.)
import sys
sys.path.insert(0, "path1")
sys.path.insert(0, "path2")

import company             # no __init__.py anywhere -> namespace package
import company.core        # contributed by path1
import company.extra       # contributed by path2

print("company.__file__:", company.__file__)
print("company.__path__:", list(company.__path__))
print("core:", company.core.VALUE, "| extra:", company.extra.VALUE)
```

Verified output (CPython 3.13.5):

```text
company.__file__: None
company.__path__: ['/private/tmp/.../path2/company', '/private/tmp/.../path1/company']
core: from path1 | extra: from path2
```

Two distinct directories contribute submodules to one importable `company` namespace. This is how
large ecosystems ship plugins (`zope.*`, `google.cloud.*`, your own `company.*` internal
namespace) as independent, separately-installable distributions. **The trade-off:** namespace
packages are slightly slower to resolve (the finder must scan all of `sys.path` rather than stop
at the first hit), and accidentally omitting an `__init__.py` from a *regular* package silently
turns it into a namespace package — a subtle source of "why is my package data missing" bugs. Use
namespace packages deliberately, for genuinely split distributions; use a normal `__init__.py`
package otherwise. The complete import machinery (`importlib`, finders, loaders, `meta_path`) is
Vol X.

---

## 9.5 Performance and anti-patterns

- **Generators trade memory for laziness.** A generator over *N* items is *O*(1) memory vs. *O*(N)
  for the equivalent list — the right default for pipelines and large/streamed data. The cost is a
  per-item Python-level resume; for small, fully-materialized data a list comprehension can be
  faster (and re-iterable).
- **Iterators are single-pass.** Exhausting a generator (or any iterator) leaves it empty — a
  second `for`/`sum`/`list` over it yields nothing. Re-create the generator, or materialize to a
  list if you need multiple passes. This bites hard with `zip`, `map`, and `dict.items()` views
  fed into a function that iterates twice.
- **Don't `list()` a generator just to index it** if you only iterate once — that discards the
  memory win. Conversely, *do* materialize when you need `len`, random access, or reuse.
- **`return` inside a generator** sets `StopIteration.value` (for `yield from` capture); it does
  not return a value to a plain `next()` caller. A bare `StopIteration` raised in generator code
  is converted to `RuntimeError` (PEP 479) — never raise it manually to end a generator; just
  `return`.
- **Reach for `itertools`** (`chain`, `islice`, `tee`, `groupby`) before hand-rolling generator
  plumbing; it is C-speed and composable (Vol XII).
- **Namespace-package footgun:** a missing `__init__.py` silently creates a namespace package.
  If package data or `__init__` side effects vanish, check for an accidentally-dropped
  `__init__.py`.

---

## 9.6 Summary and cross-references

- The **iterator protocol** is `__iter__` (returns an iterator) + `__next__` (next item or
  `StopIteration`); `for` compiles to `GET_ITER`/`FOR_ITER`. Iterators are **single-pass**.
- **Generators** synthesize iterators from `yield`-containing functions by **suspending a heap
  frame** (`gi_frame`/`f_lasti`), giving *O*(1)-memory laziness and a two-way `send`/`throw`/`close`
  protocol.
- **`yield from`** (PEP 380) delegates transparently to a sub-generator — routing values and
  exceptions and **capturing its return value** — and is the direct conceptual ancestor of
  `await`.
- **Implicit namespace packages** (PEP 420) let one package span multiple directories without
  `__init__.py` (`__file__ is None`, multi-entry `__path__`), enabling split distributions.

**Cross-references.** Generator `send`/`throw`/`close` and frame suspension origins → Chapter 5.
Comprehensions vs. generator expressions → Chapter 3. PEP 393 flexible strings (covered there, not
here) → Chapter 7. The buffer protocol and `memoryview` in full → Vol IX. `asyncio` and native
`async`/`await` (which generalize `yield from`) → Chapter 11. The complete import machinery →
Vol X. `itertools` → Vol XII.
