# Chapter 11: Native Async/Await and New Operators (Python 3.5)

Python 3.5 turned the coroutine pattern of Chapters 9–10 into first-class syntax. **PEP 492** gave
`async def` and `await` — a coroutine that is a *distinct type* from a generator, not a clever use
of one — along with `async for` and `async with`. Two more operators arrived the same release:
**PEP 465's `@`** matrix-multiplication operator and **PEP 448's** generalized `*`/`**` unpacking.
This chapter is the canonical home for the async/await *syntax and object model*; the event-loop
internals it drives are Chapter 10 (model) and Vol IX (depth).

## Section Index
- **11.1** Native coroutines: `async def`, `await`, and `PyCoroObject` (PEP 492)
- **11.2** `async for` and `async with`
- **11.3** The matrix-multiplication operator `@` (PEP 465)
- **11.4** Extended unpacking generalizations (PEP 448)
- **11.5** Performance and anti-patterns
- **11.6** Summary and cross-references

---

## 11.1 Native coroutines: `async def`, `await`, and `PyCoroObject` (PEP 492)

**Why this exists.** Through 3.4, a coroutine *was* a generator (`@asyncio.coroutine` + `yield
from`, Chapter 10). That conflation was dangerous: a coroutine exposed the iterator protocol, so
you could accidentally `for`-loop over it or call `next()` and step it outside the event loop; and
`yield from` meant two different things (delegate to a generator vs. await I/O). **PEP 492** split
them: `async def` produces a **coroutine object of a distinct C type** (`PyCoroObject`) that does
*not* implement the iterator protocol, and `await` is a dedicated keyword.

```python
# Caption: async def returns a coroutine — a distinct type that is not an iterator.
import asyncio

async def get_val():
    return 42

c = get_val()                                  # does NOT run the body; returns a coroutine
print("type:", type(c).__name__)
print("has __await__:", hasattr(c, "__await__"), "| has __next__:", hasattr(c, "__next__"))
print("run result:", asyncio.run(get_val()))
c.close()                                       # close the un-awaited coroutine
```

Verified output (CPython 3.13.5):

```text
type: coroutine
has __await__: True | has __next__: False
run result: 42
```

Calling an `async def` builds a `PyCoroObject` and runs *nothing* until it is driven by `await` or
an event loop (`asyncio.run`). The coroutine has `__await__` but no `__next__` — you cannot iterate
it, which is exactly the safety PEP 492 bought.

**The `await` protocol.** `await expr` requires `expr` to be **awaitable**: a coroutine, or any
object whose type implements `__await__` (the C `am_await` slot in the `tp_as_async` table) and
returns an iterator. You can build your own awaitable:

```python
# Caption: a custom awaitable — __await__ yields control to the loop, then returns a value.
import asyncio

class Delayed:
    def __init__(self, value):
        self.value = value
    def __await__(self):
        yield                 # suspend once, handing control to the event loop
        return self.value

async def use_awaitable():
    return await Delayed("custom-result")

print("result:", asyncio.run(use_awaitable()))
```

Verified output (CPython 3.13.5):

```text
result: custom-result
```

**What the interpreter actually does.** `await` compiles to `GET_AWAITABLE` followed by a
`SEND`/`YIELD_VALUE` resume loop — the modern descendant of the `yield from` machinery from
Chapter 9, now driving an awaitable rather than a generator:

```python
# Caption: modern await bytecode (3.13) — GET_AWAITABLE + SEND/YIELD_VALUE loop.
import dis
async def main():
    val = await get_val()
    return val
dis.dis(main)
```

Verified output (CPython 3.13.5, excerpt):

```text
 14   RETURN_GENERATOR
      POP_TOP
 L1:  RESUME      0
 15   LOAD_GLOBAL 1 (get_val + NULL)
      CALL        0
      GET_AWAITABLE 0
      LOAD_CONST  0 (None)
 L2:  SEND        3 (to L5)       # send into the awaitable; jump to L5 when it finishes
 L3:  YIELD_VALUE 1               # suspend this coroutine, yield to the loop
 L4:  RESUME      3
      JUMP_BACKWARD_NO_INTERRUPT 5 (to L2)
 L5:  END_SEND
      STORE_FAST  0 (val)
```

Note `RETURN_GENERATOR` at entry (a coroutine *is* a kind of generator frame under the hood) and
the `SEND`/`END_SEND` pair that replaced the pre-3.11 `YIELD_FROM`. Material showing `YIELD_FROM`
for `await` predates 3.11. The suspension is still the heap frame you have been tracking since
Chapter 5: `await` saves the frame, hands control to the loop, and resumes when the awaitable
completes.

---

## 11.2 `async for` and `async with`

PEP 492 also added asynchronous versions of the two iteration/context constructs, for the common
case where *advancing an iterator* or *acquiring a resource* is itself I/O.

- **`async for`** drives the **asynchronous iterator protocol**: `__aiter__` returns an async
  iterator; `__anext__` is a coroutine returning the next item or raising `StopAsyncIteration`.
- **`async with`** drives `__aenter__`/`__aexit__`, both coroutines — so acquiring/releasing a
  connection can `await`.
- **Async comprehensions** (`[x async for x in ...]`) followed in 3.6 (PEP 530).

```python
# Caption: async iterator and async context manager, consumed by async for / async with.
import asyncio

class AsyncRange:
    def __init__(self, n):
        self.n, self.i = n, 0
    def __aiter__(self):
        return self
    async def __anext__(self):
        if self.i >= self.n:
            raise StopAsyncIteration
        self.i += 1
        return self.i - 1

class AsyncCtx:
    async def __aenter__(self):
        print("aenter"); return self
    async def __aexit__(self, *exc):
        print("aexit")

async def demo():
    out = [x async for x in AsyncRange(3)]      # async comprehension (PEP 530, 3.6+)
    async with AsyncCtx():
        pass
    return out

print("collected:", asyncio.run(demo()))
```

Verified output (CPython 3.13.5):

```text
aenter
aexit
collected: [0, 1, 2]
```

These are the building blocks for async database cursors, streaming HTTP bodies, and async
connection pools — anywhere "get the next item" or "open the resource" must await I/O.

---

## 11.3 The matrix-multiplication operator `@` (PEP 465)

**Why this exists.** The numeric community needed a clean infix operator for matrix
multiplication: before 3.5, `A.dot(B).dot(C)` nested awkwardly and `*` was already taken by
element-wise multiplication. **PEP 465** added `@` (and in-place `@=`), mapping to the dunder
`__matmul__`/`__rmatmul__`/`__imatmul__` and the C slot `nb_matrix_multiply`. The operator carries
*no* built-in meaning — like every operator, it dispatches to the operands' type:

```python
# Caption: @ dispatches to __matmul__; here, a vector dot product.
class Vec:
    def __init__(self, data):
        self.data = data
    def __matmul__(self, other):
        return sum(a * b for a, b in zip(self.data, other.data))

print("[1,2,3] @ [4,5,6] =", Vec([1, 2, 3]) @ Vec([4, 5, 6]))

import operator
print("operator.matmul exists:", hasattr(operator, "matmul"))
```

Verified output (CPython 3.13.5):

```text
[1,2,3] @ [4,5,6] = 32
operator.matmul exists: True
```

Dispatch follows the standard binary-operator rules: try `type(A).__matmul__(A, B)`; on
`NotImplemented` try `type(B).__rmatmul__(B, A)`; a subclass's reflected method takes precedence.
The payoff is readability where it matters most — `result = (A @ B @ C) + (D @ E)` instead of
chained `.dot()` calls. NumPy implements `nb_matrix_multiply` on `ndarray` to call optimized
BLAS/LAPACK routines that **release the GIL** during the heavy C computation (Vol IX), so `@` on
large arrays is both readable *and* parallelizable.

---

## 11.4 Extended unpacking generalizations (PEP 448)

**Why this exists.** Before 3.5, `*` and `**` unpacking were limited to a single use in restricted
positions. **PEP 448** generalized them: multiple unpackings in collection literals and in calls.

```python
# Caption: multiple * and ** unpackings in literals and calls.
a, b = [1, 2], [3, 4]
print("[*a, *b, 5]:", [*a, *b, 5])

d1, d2 = {"x": 1}, {"y": 2}
print("{**d1, **d2, 'z': 3}:", {**d1, **d2, "z": 3})

def f(*args):
    return args
print("f(*a, *b):", f(*a, *b))
```

Verified output (CPython 3.13.5):

```text
[*a, *b, 5]: [1, 2, 3, 4, 5]
{**d1, **d2, 'z': 3}: {'x': 1, 'y': 2, 'z': 3}
f(*a, *b): (1, 2, 3, 4)
```

**Modern bytecode.** The original 3.5 implementation used `BUILD_LIST_UNPACK`/`BUILD_MAP_UNPACK`
(building temporary lists then merging). Python 3.9 replaced those with leaner incremental opcodes —
`LIST_EXTEND`, `LIST_APPEND`, `DICT_UPDATE`, `DICT_MERGE` — that build the result in place:

```python
# Caption: 3.9+ builds unpacked literals incrementally (no temporary-list merge).
import dis
dis.dis(compile("[*a, *b, 5]", "<s>", "eval"))
```

Verified output (CPython 3.13.5):

```text
  1  BUILD_LIST   0
     LOAD_NAME    0 (a)
     LIST_EXTEND  1
     LOAD_NAME    1 (b)
     LIST_EXTEND  1
     LOAD_CONST   0 (5)
     LIST_APPEND  1
     RETURN_VALUE
```

**Collision rules differ by context.** In a `dict` literal, a duplicate key is *silently
overwritten* by the rightmost value (`{**d1, **d2}` lets `d2` win). In a function *call*,
`f(**d1, **d2)` with overlapping keys raises `TypeError: got multiple values for keyword argument`
— because keyword arguments must be unambiguous. Dict merging via `|`/`|=` (PEP 584) is Vol V.

---

## 11.5 Performance and anti-patterns

- **The un-awaited coroutine.** Calling `async def` without `await`ing or scheduling it does
  nothing and emits `RuntimeWarning: coroutine ... was never awaited`. Always `await` it, pass it
  to `asyncio.create_task`/`gather`, or `close()` it.
- **Blocking the loop** (restated from Chapter 10) — a synchronous call inside a coroutine stalls
  every task. `await` only non-blocking operations; offload blocking work.
- **`@` has no inherent meaning** — it does whatever the operands' `__matmul__` defines. For plain
  Python objects it raises `TypeError` unless you implement it; it shines with NumPy, not lists.
- **Unpacking copies.** `[*a, *b]` builds a new list (O(total length)); `{**d1, **d2}` a new dict.
  Convenient, but do not use it inside hot loops where you could extend in place.
- **`**` keyword collisions in calls raise**, unlike dict literals — don't assume "right wins"
  semantics carry over to function calls.

---

## 11.6 Summary and cross-references

- **PEP 492** makes `async def` produce a **distinct coroutine type** (`PyCoroObject`) with
  `__await__` and *no* iterator protocol; `await` compiles to `GET_AWAITABLE` + a
  `SEND`/`YIELD_VALUE` loop (the 3.11+ successor to `YIELD_FROM`).
- **`async for`/`async with`** drive `__aiter__`/`__anext__`/`StopAsyncIteration` and
  `__aenter__`/`__aexit__`; async comprehensions arrived in 3.6 (PEP 530).
- **PEP 465** adds `@`/`@=` (`__matmul__`/`__imatmul__`); the operator is meaning-free and
  dispatches to the type — the basis of NumPy's BLAS-backed matrix product.
- **PEP 448** generalizes `*`/`**` unpacking in literals and calls; 3.9+ compiles it to
  `LIST_EXTEND`/`DICT_MERGE`. Dict literals let the right key win; call kwargs collisions raise.

**Cross-references.** `yield from` and the suspendable frame `await` generalizes → Chapter 9. The
event loop, Futures, and Tasks → Chapter 10. Event-loop internals, `TaskGroup`, cancellation, and
structured concurrency → Vol IX. NumPy, BLAS, and GIL-releasing numerics → Vol IX. `dict` merge
operators `|`/`|=` (PEP 584) → Vol V. f-strings and further 3.6 ergonomics → Vol IV.

---

*Volume III complete. Volume IV opens the "expressive modern Python" era (3.6–3.7): f-strings,
variable annotations, dataclasses, and context variables.*
