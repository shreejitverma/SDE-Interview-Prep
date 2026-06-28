# Chapter 5: Decorators, Context Managers, and the 2.x Twilight (Python 2.4–2.7)

The late 2.x line added three features that turned Python's first-class functions and frames
into reusable structure: **decorators** (callable transformers applied with `@`), **context
managers** (the `with` protocol for deterministic acquire/release), and **generator
coroutines** (two-way `send`/`throw`/`close`). None is new syntax for its own sake. Decorators
are function composition made declarative; context managers are Python's answer to C++ RAII
built on `try`/`finally`; and generator coroutines are the suspendable frame that, a decade
later, became `async`/`await`. This chapter teaches all three at working depth and grounds each
in the bytecode the 3.13 interpreter actually emits.

## Section Index
- **5.1** Decorators: composition made declarative (PEP 318, PEP 3129)
- **5.2** Context managers and `with` (PEP 343)
- **5.3** Generator coroutines: `send`, `throw`, `close` (PEP 342)
- **5.4** Performance and anti-patterns
- **5.5** Summary and cross-references

---

## 5.1 Decorators: composition made declarative (PEP 318, PEP 3129)

**Why this exists.** Before Python 2.4, wrapping a function meant defining it and then
rebinding the name — with the transformation written *after* and *away from* the definition:

```python
# Pre-2.4 idiom: the wrapping is separated from the definition.
def query():
    ...
query = transaction(log(query))   # easy to miss; reads bottom-up, far from def
```

**PEP 318** introduced `@decorator` syntax so the transformation sits *on* the definition. It
is pure compile-time sugar: `@d` above `def f` means "after building `f`, replace it with
`d(f)`." Stacked decorators apply **bottom-up** (nearest the `def` first):

```python
# Caption: stacked decorators compose bottom-up; functools.wraps preserves identity.
import functools

def dec1(f):
    @functools.wraps(f)
    def wrapper(*args, **kwargs):
        return "dec1(" + f(*args, **kwargs) + ")"
    return wrapper

def dec2(f):
    @functools.wraps(f)
    def wrapper(*args, **kwargs):
        return "dec2(" + f(*args, **kwargs) + ")"
    return wrapper

@dec1
@dec2
def target():
    return "target"

print("result:", target())
print("__name__ preserved:", target.__name__)
```

Verified output (CPython 3.13.5):

```text
result: dec1(dec2(target))
__name__ preserved: target
```

The result `dec1(dec2(target))` is exactly the bottom-up composition
$\text{target} = \text{dec1}(\text{dec2}(\text{target}))$. The bytecode makes the mechanism
explicit — the decorators are loaded, the function is built, then the calls are applied
outermost-last:

```text
# dis of  @dec1 / @dec2 / def t(): return 1   on CPython 3.13.5
LOAD_NAME    0 (dec1)
LOAD_NAME    1 (dec2)
LOAD_CONST   0 (<code object t>)
MAKE_FUNCTION
CALL         0          # dec2(t)
CALL         0          # dec1(dec2(t))
STORE_NAME   2 (t)
RETURN_CONST 1 (None)
```

This is current bytecode: `MAKE_FUNCTION` now takes no inline argument (closures/defaults are
applied by separate `SET_FUNCTION_ATTRIBUTE` opcodes when needed), and the call opcode is the
unified `CALL`, not the pre-3.11 `CALL_FUNCTION`. Older texts showing `CALL_FUNCTION 1` predate
3.11.

**`functools.wraps` is not optional.** A naive wrapper replaces the function's identity — its
`__name__`, `__doc__`, `__qualname__`, `__wrapped__`, and `__module__` all become the
wrapper's. `@functools.wraps(f)` copies them across so introspection, tracebacks, and tools
still see the original. Omitting it is the single most common decorator bug.

**Decorators with arguments** are decorator *factories* — a function returning a decorator:

```python
# Caption: a parameterized decorator is a function that returns a decorator.
import functools

def repeat(n):
    def decorator(f):
        @functools.wraps(f)
        def wrapper(*args, **kwargs):
            return [f(*args, **kwargs) for _ in range(n)]
        return wrapper
    return decorator

@repeat(3)
def greet(name):
    return f"hi {name}"

print(greet("ada"))
```

Verified output (CPython 3.13.5):

```text
['hi ada', 'hi ada', 'hi ada']
```

**Class decorators (PEP 3129, Python 2.6)** apply the same rule to a class: after the class
object is built, it is passed through the decorator — `Model = class_decorator(Model)`. This is
how `@dataclass`, `@functools.total_ordering`, and registration decorators work (Vol IV).

**When not to use a decorator.** When the transformation needs to compose state across many
call sites, or to be configurable per instance, a descriptor (Chapter 4) or an explicit
wrapper object is clearer. And a decorator that changes a function's *signature* without
updating its metadata breaks tooling — prefer `functools.wraps` plus, for strict cases,
`inspect.signature` preservation.

---

## 5.2 Context managers and `with` (PEP 343)

**Why this exists.** Resource cleanup that must happen *regardless of how a block exits* — close
a file, release a lock, roll back a transaction — is exactly what C++ expresses with RAII and
destructors. Python's reference-counting finalization (Chapter 2) is too imprecise to rely on
for this (and useless across cycles), so **PEP 343** added the `with` statement and the
**context-manager protocol**:

- `__enter__(self)` — acquire; its return value is bound to the `as` target.
- `__exit__(self, exc_type, exc_val, exc_tb)` — release. On a clean exit all three are `None`.
  On an exception they carry its details; **returning a truthy value suppresses the
  exception**, a falsy value (including `None`) lets it propagate.

```python
# Caption: __exit__ sees the exception and can suppress it by returning truthy.
class Ctx:
    def __enter__(self):
        print("  __enter__")
        return self
    def __exit__(self, exc_type, exc_val, exc_tb):
        print("  __exit__ exc_type =", exc_type.__name__ if exc_type else None)
        return exc_type is ValueError      # suppress only ValueError

with Ctx() as c:
    print("  body (clean)")
print("clean exit done")

with Ctx():
    print("  body raising ValueError")
    raise ValueError("boom")
print("ValueError was suppressed by __exit__")
```

Verified output (CPython 3.13.5):

```text
  __enter__
  body (clean)
  __exit__ exc_type = None
clean exit done
  __enter__
  body raising ValueError
  __exit__ exc_type = ValueError
ValueError was suppressed by __exit__
```

**What the compiler does.** `with` desugars to a guaranteed `try`/`finally` around the block,
with the exception path routed through `__exit__`:

```python
# Conceptual desugaring of:  with expr as target: suite
mgr = expr
value = type(mgr).__enter__(mgr)
try:
    target = value          # if an "as" clause is present
    suite
except:                     # exception path
    if not type(mgr).__exit__(mgr, *sys.exc_info()):
        raise
else:                       # clean path
    type(mgr).__exit__(mgr, None, None, None)
```

The VM implements this with dedicated opcodes (`BEFORE_WITH`/`WITH_EXCEPT_START` in modern
CPython) and the frame's **exception table** (3.11+) rather than the older runtime block stack,
but the guarantee is identical: `__exit__` runs on every exit path.

**`contextlib.contextmanager`** lets you write a context manager as a single generator —
everything before `yield` is `__enter__`, the `finally` is `__exit__`:

```python
# Caption: a generator-based context manager.
import contextlib

@contextlib.contextmanager
def managed():
    print("  acquire")
    try:
        yield 42
    finally:
        print("  release")

with managed() as v:
    print("  using", v)
```

Verified output (CPython 3.13.5):

```text
  acquire
  using 42
  release
```

`contextlib` adds the rest of the toolkit: `suppress(exc)` (swallow specific exceptions),
`closing(obj)` (call `.close()` on exit), `ExitStack` (dynamically manage a variable number of
context managers), and `nullcontext`. The full `contextlib` reference lives in Vol X; here the
point is the *protocol* and that it is Python's deterministic-cleanup primitive — use it for
every OS resource instead of trusting `__del__`.

---

## 5.3 Generator coroutines: `send`, `throw`, `close` (PEP 342)

**Why this exists.** Python 2.5's **PEP 342** upgraded generators from one-way producers into
two-way **coroutines** — a suspended frame you can resume *with a value*, inject an exception
into, or shut down. This is the conceptual seed of `async`/`await` (Vol III): a coroutine is a
frame whose execution can pause at a point and continue later.

The protocol adds three methods to a generator:

- **`.send(value)`** — resume the generator; the paused `yield` *expression* evaluates to
  `value`. `.send(None)` is equivalent to `next()`; you must prime a fresh generator with
  `next()` (or `.send(None)`) before sending a real value.
- **`.throw(exc)`** — raise `exc` at the suspended `yield` point, letting the generator handle
  or clean up.
- **`.close()`** — raise `GeneratorExit` at the `yield` point; the generator should release
  resources and return.

```python
# Caption: a running-average coroutine driven by send(), shut down by close().
def averager():
    total, count, avg = 0.0, 0, None
    while True:
        try:
            x = yield avg                 # value arrives via .send(x)
        except GeneratorExit:
            print("  averager closing")
            return
        total += x
        count += 1
        avg = total / count

g = averager()
next(g)                                   # prime to the first yield
print("avg after 10:       ", g.send(10))
print("avg after 10,20:    ", g.send(20))
print("avg after 10,20,30: ", g.send(30))
g.close()
```

Verified output (CPython 3.13.5):

```text
avg after 10:        10.0
avg after 10,20:     15.0
avg after 10,20,30:  20.0
  averager closing
```

**What the interpreter actually does.** At a `yield`, `_PyEval_EvalFrameDefault` does not
destroy the frame: it records the resume point (`f_lasti`), freezes the evaluation-stack depth,
marks the generator `GEN_SUSPENDED`, and detaches the frame from the thread state, returning
control to the caller while the frame stays alive on the heap. `.send`/`next` re-attach the
frame and resume at `f_lasti`. That **suspendable heap frame** is the mechanism `yield from`
(Vol III, Ch 9) and native coroutines (Vol III, Ch 11) generalize. We keep the depth here to
the 2.5 milestone and develop the full iterator/coroutine model in Vol III.

---

## 5.4 Performance and anti-patterns

- **Decorator call overhead.** Each decoration adds a Python-level call per invocation. In hot
  paths, a wrapper that does little but forward arguments can dominate; measure, and consider
  `functools.lru_cache`/`cache` (which is a decorator that *removes* work) or inlining.
- **Always `functools.wraps`.** Beyond cosmetics, missing metadata breaks `inspect.signature`,
  `pydoc`, framework route detection, and `typing` tools. `__wrapped__` (set by `wraps`) lets
  tools unwrap to the original.
- **Context managers over `try`/finally** for anything reused: the protocol is composable
  (`ExitStack`), nestable, and harder to get wrong than hand-written `finally` blocks. Reserve
  bare `try`/`finally` for one-off, local cleanup.
- **Prime your coroutines.** Calling `.send(non_None)` on an un-primed generator raises
  `TypeError: can't send non-None value to a just-started generator`. A priming decorator
  (`next(gen)` on creation) is a common pattern — superseded in practice by `async`/`await`
  for new code.
- **Don't swallow exceptions silently** in `__exit__`: returning a truthy value suppresses
  *all* matching exceptions, which can hide bugs. Suppress narrowly (check `exc_type`) or use
  `contextlib.suppress` with explicit exception types.

---

## 5.5 Summary and cross-references

- **Decorators** are compile-time sugar for `f = d(f)`, applied **bottom-up**; parameterized
  decorators are factories; class decorators (PEP 3129) apply the same rule to classes. Always
  use **`functools.wraps`**.
- **Context managers** (`__enter__`/`__exit__`) are Python's **deterministic cleanup**
  primitive — RAII built on a guaranteed `try`/finally; `__exit__` returning truthy suppresses
  the exception. `contextlib` provides generator-based managers and a toolkit.
- **Generator coroutines** (PEP 342) add `send`/`throw`/`close`, making a generator a
  **suspendable, resumable frame** — the conceptual origin of `async`/`await`.
- The modern bytecode for all three (`MAKE_FUNCTION`/`CALL`, `BEFORE_WITH`, generator
  suspension via `f_lasti`) differs from pre-3.11 listings; read `dis`, not folklore.

**Cross-references.** First-class functions, frames, and `f_lasti` → Chapter 1. Reference
counting vs. deterministic cleanup → Chapter 2. Descriptors (an alternative to decorators for
attribute behavior) → Chapter 4. `yield from`, full iterator/generator semantics, and native
coroutines → Vol III, Ch 9 and Ch 11. `dataclass` and `total_ordering` class decorators →
Vol IV. The full `contextlib` reference → Vol X. **CPython container internals** (list
overallocation, dict/set/tuple layout and free lists — covered in this chapter's archived
source §5.4) → Vol XII (data-structures internals).
