---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://docs.python.org/3/reference/datamodel.html, https://docs.python.org/3/library/stdtypes.html, https://docs.python.org/3/library/asyncio-task.html, https://docs.python.org/3/howto/free-threading-python.html, https://docs.python.org/3/whatsnew/3.14.html, https://peps.python.org/pep-0779/, https://peps.python.org/pep-0790/, https://docs.python.org/3/howto/mro.html, https://docs.python.org/3/library/dataclasses.html, https://docs.pydantic.dev/latest/concepts/models/, https://mypy.readthedocs.io/en/stable/, https://packaging.python.org/en/latest/guides/writing-pyproject-toml/, https://docs.astral.sh/uv/]
---

# Python core

The Python language questions a contract screen asks a developer with 5+ years, ordered by how often they come up.
Y1 (mutable defaults), Y6 (decorators), Y7 (generators), Y9 (GIL and concurrency), and Y10 (asyncio) are near-certain; Y23 (predict the output) is how many screens open.
Every code sample here was run on 2026-09-29 under Python 3.14.7 with Pydantic 2.13.5 and mypy 2.3.1 (free-threading numbers under 3.14.6t): 36 pytest tests passed: all 34 Python blocks executed as scripts, the 14 "Output:" blocks compared against real stdout, the Y17 mypy output checked, and the TOML parsed.

---

## Y1. What is the difference between mutable and immutable objects, and what is the mutable default argument trap? (must know)

- Immutable: `int`, `float`, `bool`, `str`, `bytes`, `tuple`, `frozenset`, `None`; mutable: `list`, `dict`, `set`, `bytearray`, and most user objects.
- Python passes object references ("call by sharing"): a function can mutate an argument it receives, but rebinding the parameter name does not affect the caller.
- Default argument values are evaluated **once, at `def` time**, and stored on the function in `__defaults__`.
  A mutable default is therefore shared across every call.
- Fix: default to `None` (or a sentinel) and create the object inside the function.

```python
from datetime import datetime


def append_bad(item, bucket=[]):
    bucket.append(item)
    return bucket


def append_good(item, bucket=None):
    if bucket is None:
        bucket = []
    bucket.append(item)
    return bucket


assert append_bad(1) == [1]
assert append_bad(2) == [1, 2]                  # the same list object, reused
assert append_bad.__defaults__ == ([1, 2],)     # the state lives on the function
assert append_good(1) == [1]
assert append_good(2) == [2]


def log_event(msg, at=datetime.now()):          # same bug: timestamp frozen at import time
    return at


assert log_event("a") is log_event("b")


def rename(items):
    items.append("mutated")                     # visible to the caller
    items = ["rebound"]                         # local rebinding only
    return items


orders = ["o1"]
assert rename(orders) == ["rebound"]
assert orders == ["o1", "mutated"]
```

**Follow-ups they ask:**

- "Is a tuple containing a list immutable?"
  The tuple is, the list inside it is not, so the tuple is not hashable: `hash(([1],))` raises `TypeError`.
- "Is the same trap in dataclasses and Pydantic?"
  `@dataclass` refuses a `list` default with `ValueError` and makes you use `field(default_factory=list)`.
  Pydantic copies a mutable default per instance, so `tags: list[str] = []` is safe on a `BaseModel` (shown in Y16).
- Production angle: a shared default list in a request handler leaks data between requests and grows without bound.

---

## Y2. How are list, tuple, dict, and set implemented, and what are their time complexities? (must know)

- `list`: a dynamic array of object pointers, over-allocated so `append` is amortized O(1); inserting or popping at the front shifts everything (O(n)).
- `tuple`: a fixed-size array of pointers; immutable, hashable if its items are, slightly smaller and faster to create than a list.
- `dict`: a hash table with open addressing; since 3.6 CPython uses a "compact dict" (a dense entries array in insertion order plus a sparse index table), and insertion order is a language guarantee since 3.7.
  It resizes when about two thirds full.
- `set`: a hash table of keys only, no ordering guarantee.
- Keys and set members must be hashable, and `a == b` must imply `hash(a) == hash(b)`.

| Operation | list | dict / set (average) | Notes |
| --- | --- | --- | --- |
| index / lookup | O(1) `xs[i]` | O(1) `d[k]`, `k in s` | dict and set worst case O(n) with pathological hashes |
| `x in container` | O(n) | O(1) | convert a list to a set before repeated membership tests |
| append / add | amortized O(1) | amortized O(1) | resizes are O(n) but rare |
| insert(0) / pop(0) | O(n) | - | use `collections.deque` for O(1) at both ends |
| pop() from end | O(1) | `popitem()` O(1) | |
| delete by value | O(n) `remove` | O(1) `del d[k]`, `discard` | |
| sort | O(n log n) Timsort, stable | - | `sorted(d)` sorts keys |
| slice `xs[a:b]` | O(b - a) copy | - | slices copy, they are not views |
| union / intersection | - | O(len(a) + len(b)) / O(min(len(a), len(b))) | |

`heapq` gives O(log n) push and pop on a list and O(n) `heapify`; `bisect` gives O(log n) search on a sorted list (insert is still O(n)).

```python
import sys
from collections import deque

# Over-allocation: the list's size in bytes changes only occasionally as it grows.
xs, sizes = [], set()
for i in range(64):
    xs.append(i)
    sizes.add(sys.getsizeof(xs))
assert len(sizes) < 20

# Hashability: defining __eq__ without __hash__ makes a class unhashable.
class Key:
    def __init__(self, v):
        self.v = v

    def __eq__(self, other):
        return isinstance(other, Key) and self.v == other.v


assert Key.__hash__ is None
try:
    {Key(1): "x"}
except TypeError as e:
    assert "unhashable" in str(e)

# Order is preserved; deque is O(1) at both ends.
d = {"b": 1, "a": 2}
d["c"] = 3
assert list(d) == ["b", "a", "c"]
q = deque([1, 2, 3], maxlen=3)
q.append(4)                          # maxlen evicts from the left
assert list(q) == [2, 3, 4] and q.popleft() == 2
```

**Pitfalls:** `x in some_list` inside a loop is a hidden O(n^2); mutating a dict while iterating raises `RuntimeError: dictionary changed size during iteration`; a mutable object used as a key whose hash-relevant fields change is lost in the table.

---

## Y3. What is the difference between a shallow copy and a deep copy?

- Assignment copies nothing: both names point to the same object.
- A **shallow copy** (`copy.copy`, `list(xs)`, `xs[:]`, `d.copy()`, `{**d}`) creates a new outer container that holds the **same** inner objects.
- A **deep copy** (`copy.deepcopy`) recursively copies everything, uses a memo dict to handle cycles and shared references, and is slow on large structures.
- In services, prefer immutable data or explicit construction over `deepcopy` in hot paths.

```python
import copy

book = {"symbol": "IBM", "levels": [[100, 5], [101, 7]]}
alias = book
shallow = copy.copy(book)
deep = copy.deepcopy(book)

book["levels"][0][1] = 999
assert alias is book
assert shallow["levels"][0][1] == 999    # inner lists are shared
assert deep["levels"][0][1] == 5         # fully independent

grid = [[0] * 3] * 3                     # three references to ONE inner list
grid[0][0] = 1
assert grid == [[1, 0, 0], [1, 0, 0], [1, 0, 0]]
grid = [[0] * 3 for _ in range(3)]       # correct: a new inner list per row
grid[0][0] = 1
assert grid == [[1, 0, 0], [0, 0, 0], [0, 0, 0]]

cyclic = [1]
cyclic.append(cyclic)
clone = copy.deepcopy(cyclic)            # memo handles the cycle
assert clone[1] is clone
```

Customize with `__copy__` and `__deepcopy__(self, memo)`.
Python 3.13 added `copy.replace(obj, **changes)` for dataclasses, named tuples, and other types that implement `__replace__`.

---

## Y4. What is the difference between `is` and `==`? What is interning?

- `==` compares values by calling `__eq__`; `is` compares identity (the same object, same `id()`).
- Use `is` only for singletons: `None`, `True`, `False`, `NotImplemented`, `Ellipsis`, and your own sentinel objects.
- CPython caches small integers from -5 to 256 and interns many strings (identifier-like literals, and anything passed to `sys.intern`), so `is` can appear to work on values; never rely on it.
- `x == None` can be overridden (NumPy and SQLAlchemy overload `==`), so `x is None` is both correct and faster.

```python
import sys

a, b = [1, 2], [1, 2]
assert a == b and a is not b

x, y = int("256"), int("256")
assert x is y                            # small-int cache
x, y = int("257"), int("257")
assert x == y and x is not y             # distinct objects built at runtime

built = "".join(["hel", "lo"])
literal = "hello"
assert built == literal and built is not literal
assert sys.intern(built) is literal      # interned: now the same object

nan = float("nan")
assert nan != nan                        # IEEE 754
assert [nan] == [nan]                    # containers check identity before ==
```

**Trick:** `a = 257; b = 257; a is b` is `True` in a script (both constants live in the same compiled code object) but `False` when typed on two separate REPL lines.
Python 3.8+ emits a `SyntaxWarning` for `x is 257` (on 3.14: `"is" with 'int' literal. Did you mean "=="?`) precisely because the answer is an implementation detail.

---

## Y5. Explain `*args`, `**kwargs`, keyword-only, and positional-only parameters.

- `*args` collects extra positional arguments into a tuple; `**kwargs` collects extra keyword arguments into a dict.
- Parameters after `*` (or after `*args`) are **keyword-only**; use them for flags and options so call sites stay readable.
- Parameters before `/` are **positional-only** (3.8, PEP 570); you can rename them without breaking callers, and the same names can still arrive through `**kwargs`.
- At the call site, `*` and `**` unpack sequences and mappings.
- Full order: `def f(pos_only, /, normal, *args, kw_only, **kwargs)`.

```python
def place_order(symbol, qty, /, side="buy", *, tif="DAY", **extra):
    return {"symbol": symbol, "qty": qty, "side": side, "tif": tif, "extra": extra}


assert place_order("IBM", 10) == {"symbol": "IBM", "qty": 10, "side": "buy", "tif": "DAY", "extra": {}}
assert place_order("IBM", 10, "sell", tif="IOC", account="A1")["extra"] == {"account": "A1"}
out = place_order("IBM", 10, symbol="also-ok")         # the name is free for **extra
assert out["symbol"] == "IBM" and out["extra"] == {"symbol": "also-ok"}

for bad_call in (lambda: place_order(symbol="IBM", qty=10),   # positional-only
                 lambda: place_order("IBM", 10, "sell", "IOC")):  # tif is keyword-only
    try:
        bad_call()
    except TypeError:
        pass
    else:
        raise AssertionError("expected TypeError")


def forward(*args, **kwargs):
    return place_order(*args, **kwargs)            # transparent forwarding (decorators)


args, kwargs = ("MSFT", 5), {"tif": "GTC"}
assert forward(*args, **kwargs)["tif"] == "GTC"
```

---

## Y6. How do decorators work? Show one with `functools.wraps`, one with arguments, and a class-based one. (must know)

- A decorator is a callable that takes a function and returns a replacement; `@d` above `def f` is exactly `f = d(f)`.
- Always use `functools.wraps(func)` on the wrapper so `__name__`, `__doc__`, `__module__`, `__qualname__`, and `__wrapped__` survive.
  Without it, logs, tracing, Flask endpoint names, and FastAPI signature introspection see `wrapper`.
- A decorator **with arguments** is a factory: `@retry(times=3)` calls `retry(times=3)`, which returns the real decorator.
- A **class-based** decorator stores state on the instance and implements `__call__`.
- Stacking applies bottom-up and runs top-down: `@a @b def f` is `f = a(b(f))`.

```python
import functools
import time


def timed(func):
    @functools.wraps(func)                      # keep __name__, __doc__, __wrapped__
    def wrapper(*args, **kwargs):
        start = time.perf_counter()
        try:
            return func(*args, **kwargs)
        finally:
            wrapper.last_elapsed = time.perf_counter() - start
    wrapper.last_elapsed = 0.0
    return wrapper


def retry(times: int, exceptions: tuple[type[BaseException], ...] = (Exception,)):
    """Decorator factory: retry(times=3) returns the real decorator."""
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            for attempt in range(1, times + 1):
                try:
                    return func(*args, **kwargs)
                except exceptions:
                    if attempt == times:
                        raise
        return wrapper
    return decorator


class CountCalls:
    """Class-based decorator: the state lives on the instance."""

    def __init__(self, func):
        functools.update_wrapper(self, func)
        self.func = func
        self.calls = 0

    def __call__(self, *args, **kwargs):
        self.calls += 1
        return self.func(*args, **kwargs)


attempts = []


@timed
@retry(times=3, exceptions=(ConnectionError,))
def fetch_quote(symbol: str) -> float:
    """Fetch a quote from a flaky upstream."""
    attempts.append(symbol)
    if len(attempts) < 3:
        raise ConnectionError("upstream reset")
    return 101.25


@CountCalls
def ping():
    return "pong"


assert fetch_quote("IBM") == 101.25 and len(attempts) == 3
assert fetch_quote.__name__ == "fetch_quote"
assert fetch_quote.__doc__ == "Fetch a quote from a flaky upstream."
assert fetch_quote.last_elapsed >= 0
ping()
ping()
assert ping.calls == 2 and ping.__name__ == "ping"
```

Stacking order, traced:

```python
import functools

trace = []


def tag(name):
    def decorator(func):
        trace.append(f"decorate {name}")
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            trace.append(f"enter {name}")
            return func(*args, **kwargs)
        return wrapper
    return decorator


@tag("outer")
@tag("inner")
def handler():
    trace.append("body")


handler()
print(trace)
```

Output:

```text
['decorate inner', 'decorate outer', 'enter outer', 'enter inner', 'body']
```

**What goes wrong in production:**

- Flask: `@app.route(...)` must be the **top** decorator.
  `route` registers whatever it receives, so a `@login_required` placed above it wraps a function Flask never calls, and the endpoint is silently unprotected.
- Decorating an `async def` with a sync wrapper returns the coroutine without awaiting it inside the wrapper, so timing and retry logic measure nothing.
  Write an `async def wrapper` that awaits, and branch on `inspect.iscoroutinefunction(func)` if one decorator must support both.
- A class-based decorator on a **method** loses `self` binding unless the class implements `__get__`; prefer a function decorator for methods.
- The retry version tested in the [Coding round](11-Coding-Round.md) adds exponential backoff and jitter.

---

## Y7. What are iterators and generators? What does `yield from` do, and why do generators save memory? (must know)

- An **iterable** has `__iter__` returning an iterator; an **iterator** has `__next__` and raises `StopIteration` when done (and returns itself from `__iter__`).
- A **generator function** contains `yield`; calling it returns a generator (an iterator) whose frame is suspended at each `yield` and resumed on `next()`.
- Generators are lazy: they produce one item at a time, so memory is O(1) in the number of items, which is how you stream a 10 GB log file or a large query result.
- `yield from sub` delegates to a sub-iterator (and forwards `send`, `throw`, and the sub-generator's return value).
- A generator is single-use: once exhausted, iterating it again yields nothing.

```python
import sys
from collections.abc import Iterator


class Countdown:
    """The iterator protocol by hand."""

    def __init__(self, start: int):
        self.current = start

    def __iter__(self):
        return self

    def __next__(self) -> int:
        if self.current <= 0:
            raise StopIteration
        self.current -= 1
        return self.current + 1


def countdown(start: int) -> Iterator[int]:
    """The same thing as a generator: state lives in the suspended frame."""
    while start > 0:
        yield start
        start -= 1


def flatten(items):
    for item in items:
        if isinstance(item, (list, tuple)):
            yield from flatten(item)             # delegate to a sub-generator
        else:
            yield item


assert list(Countdown(3)) == list(countdown(3)) == [3, 2, 1]
assert list(flatten([1, [2, [3, (4,)]], 5])) == [1, 2, 3, 4, 5]

squares_list = [n * n for n in range(1_000_000)]   # materializes 1M ints
squares_gen = (n * n for n in range(1_000_000))    # a ~200-byte object
assert sys.getsizeof(squares_gen) < 1_000 < sys.getsizeof(squares_list)
assert sum(squares_gen) == sum(squares_list)
assert sum(squares_gen) == 0                       # single-use: already exhausted


# A lazy pipeline: nothing runs until the consumer pulls.
def read_lines(lines):                   # in production: with open(path) as f: yield from f
    for line in lines:
        yield line.rstrip("\n")


def parse(lines):
    for line in lines:
        level, _, msg = line.partition(" ")
        yield level, msg


def errors(records):
    return (msg for level, msg in records if level == "ERROR")


raw = ["INFO start\n", "ERROR db timeout\n", "WARN slow\n", "ERROR disk full\n"]
assert list(errors(parse(read_lines(raw)))) == ["db timeout", "disk full"]
```

**Follow-ups they ask:**

- "List comprehension or generator expression?"
  A list when you need `len`, indexing, or multiple passes; a generator when you stream once, especially into `sum`, `any`, `max`, or `"".join`.
- "What happens if you return a value from a generator?"
  It becomes `StopIteration.value`, and `yield from` evaluates to it.
- "How do you close a generator holding a file?"
  `gen.close()` raises `GeneratorExit` at the paused `yield`, so a `with` block inside the generator cleans up.
  Unclosed generators are only finalized when garbage collected.
- `itertools` (`islice`, `chain`, `groupby`, `batched` in 3.12) composes lazily; the batched file reader is in the [Coding round](11-Coding-Round.md).

---

## Y8. How do context managers work? Write one as a class and one with `contextlib`.

- `with cm as x:` calls `cm.__enter__()` (its return value binds to `x`), runs the block, then always calls `cm.__exit__(exc_type, exc, tb)`, even on exceptions.
- `__exit__` returning a truthy value **suppresses** the exception; return `False` or `None` unless suppression is the explicit purpose.
- `@contextlib.contextmanager` turns a generator into a context manager: code before `yield` is enter, after is exit; wrap the `yield` in `try`/`finally` or the cleanup is skipped on error.
- Also know `ExitStack` (a dynamic number of resources), `suppress`, `closing`, `nullcontext`, and the async forms `async with` and `@asynccontextmanager` (FastAPI's `lifespan`, see [FastAPI](04-FastAPI.md)).

```python
import sqlite3
import time
from contextlib import ExitStack, closing, contextmanager, suppress


class Timer:
    def __enter__(self):
        self.start = time.perf_counter()
        return self

    def __exit__(self, exc_type, exc, tb):
        self.elapsed = time.perf_counter() - self.start
        return False                              # never swallow exceptions


@contextmanager
def transaction(conn: sqlite3.Connection):
    try:
        yield conn
        conn.commit()
    except Exception:
        conn.rollback()
        raise


conn = sqlite3.connect(":memory:")
conn.execute("CREATE TABLE orders (id INTEGER PRIMARY KEY, qty INTEGER NOT NULL)")
conn.commit()

with Timer() as t, transaction(conn):             # several managers in one with
    conn.execute("INSERT INTO orders (qty) VALUES (10)")
assert t.elapsed >= 0

with suppress(sqlite3.IntegrityError):
    with transaction(conn):
        conn.execute("INSERT INTO orders (qty) VALUES (20)")
        conn.execute("INSERT INTO orders (qty) VALUES (NULL)")   # NOT NULL violation
assert conn.execute("SELECT COUNT(*) FROM orders").fetchone()[0] == 1   # 20 rolled back

# Pitfall: sqlite3's own "with conn:" commits or rolls back but does NOT close.
with ExitStack() as stack:                        # closed in reverse order on exit
    conns = [stack.enter_context(closing(sqlite3.connect(":memory:"))) for _ in range(3)]
try:
    conns[0].execute("SELECT 1")
except sqlite3.ProgrammingError as e:
    assert "closed" in str(e)
```

**Production angle:** SQLAlchemy sessions, DB connections from a pool, locks, temp files, and HTTP clients all belong in `with`; a missed `close()` in a request path exhausts the pool under load, not in tests.

---

## Y9. What is the GIL? When do you use threading, multiprocessing, or asyncio? (must know)

> "The GIL is a lock in the standard CPython build that lets only one thread execute Python bytecode at a time.
> It protects the interpreter's internals, like reference counts, but not your data structures, so you still need locks for compound operations.
> Blocking I/O and many C extensions release it, so threads work well for I/O-bound work.
> For CPU-bound pure Python I use processes, or push the hot loop into NumPy, a C extension, or a separate service.
> For thousands of concurrent network calls I use asyncio.
> Python 3.13 added an experimental free-threaded build, and 3.14 made it officially supported but still optional; the default build still has the GIL."

| Workload | Tool | Why | Cost |
| --- | --- | --- | --- |
| I/O-bound, tens to hundreds of concurrent calls, sync libraries | `threading` / `ThreadPoolExecutor` | GIL is released while waiting on sockets, files, `time.sleep` | a reserved stack per thread (size is OS dependent), races on shared state |
| I/O-bound, thousands of connections | `asyncio` | one thread, cooperative switching at `await`, tiny per-task cost | needs async libraries end to end; one blocking call stalls everything |
| CPU-bound pure Python | `multiprocessing` / `ProcessPoolExecutor` | separate interpreters, separate GILs, real parallelism | process startup, pickling of arguments and results, memory per process |
| CPU-bound numeric | NumPy, Numba, Cython, Rust/C++ extensions | the kernel runs outside the interpreter and can release the GIL | build complexity |
| CPU-bound, 3.14+ | free-threaded build (`python3.14t`) or `InterpreterPoolExecutor` | threads or subinterpreters run truly in parallel | extension compatibility, 5-10% single-thread slowdown on the free-threaded build |

```python
import time
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor


def cpu_work(n: int) -> int:
    return sum(i * i for i in range(n))


def io_work(delay: float) -> float:
    time.sleep(delay)            # releases the GIL, like a socket read or a DB call
    return delay


def timed(fn) -> float:
    start = time.perf_counter()
    fn()
    return time.perf_counter() - start


if __name__ == "__main__":       # required: spawn-based workers re-import this module
    with ThreadPoolExecutor(8) as pool:
        io_threads = timed(lambda: list(pool.map(io_work, [0.2] * 8)))
    print(f"8 x 0.2s sleeps on 8 threads: {io_threads:.2f}s")

    jobs = [10_000_000] * 4
    serial = timed(lambda: [cpu_work(n) for n in jobs])
    with ThreadPoolExecutor(4) as pool:
        threads = timed(lambda: list(pool.map(cpu_work, jobs)))
    with ProcessPoolExecutor(4) as pool:
        procs = timed(lambda: list(pool.map(cpu_work, jobs)))
    print(f"cpu: serial={serial:.2f}s threads={threads:.2f}s processes={procs:.2f}s")
```

One run on 2026-09-29 on a 12-core Apple Silicon Mac under 3.14.7 printed:

```text
8 x 0.2s sleeps on 8 threads: 0.21s
cpu: serial=1.38s threads=1.38s processes=0.54s
```

Threads overlap the sleeps perfectly but give no CPU speedup under the GIL; four processes give about 2.5x, with process startup and pickling eating the rest.
The same four-thread CPU loop run under the free-threaded 3.14.6t build on that machine was about 2.6x to 2.9x faster than serial, while the default 3.14.7 build showed no speedup (0.98x to 1.05x).

**Free-threading status (verify before quoting, it moves fast):**

- 3.13: PEP 703 free-threaded build shipped as **experimental**, a separate `python3.13t` binary.
- 3.14: PEP 779 made it **officially supported** (phase II) but still **not the default**; the What's New puts the single-threaded penalty at roughly 5-10%.
  Importing a C extension that does not declare free-threading support re-enables the GIL at runtime with a warning, unless you force it off with `PYTHON_GIL=0` or `-X gil=0`.
- 3.14 also added `concurrent.interpreters` (PEP 734) and `concurrent.futures.InterpreterPoolExecutor`: one GIL per subinterpreter.
- Check at runtime with `sys._is_gil_enabled()` and `sysconfig.get_config_var("Py_GIL_DISABLED")`.
- 3.15 is scheduled for 2026-10-01 (PEP 790); the GIL build remains the default plan until a later PEP says otherwise.

**Follow-ups they ask:**

- "If there is a GIL, why do I need locks?"
  `counter += 1` is a read, an add, and a store; a thread switch between them loses updates.
  The GIL makes single bytecodes atomic, not your invariants.
- "Why does `multiprocessing` need `if __name__ == '__main__':`?"
  With the `spawn` start method (the default on macOS and Windows) each worker re-imports your main module; without the guard it would recursively start workers.
  In 3.14 the default on Linux changed from `fork` to `forkserver`.
- "Gunicorn or Uvicorn workers?"
  Web servers scale CPU across **processes** (workers) and handle I/O concurrency inside each process with threads or an event loop.

---

## Y10. How does asyncio work? Explain the event loop, `await`, `gather` versus `TaskGroup`, and what happens with a blocking call. (must know)

- The **event loop** runs on one thread and multiplexes many tasks; it waits on sockets with the OS selector (kqueue, epoll) and resumes whichever coroutine is ready.
- `async def` makes a coroutine function; calling it creates a coroutine object that does nothing until awaited or wrapped in a task.
- `await` is a **suspension point**: the coroutine yields control back to the loop until the awaited thing completes; switching happens only there (cooperative multitasking).
- `asyncio.gather` runs awaitables concurrently and returns results in argument order; by default the first exception propagates but the others keep running, while `return_exceptions=True` returns exceptions as values.
- `asyncio.TaskGroup` (3.11) is structured concurrency: if one task fails, the rest are cancelled and all errors are raised together as an `ExceptionGroup`.
  Prefer it for new code.
- A **blocking call** (`time.sleep`, `requests.get`, a sync DB driver, a big CPU loop) freezes every task on the loop; use an async library or offload with `asyncio.to_thread` or `loop.run_in_executor`.

```python
import asyncio
import time


async def fetch(name: str, delay: float) -> str:
    await asyncio.sleep(delay)               # suspension point: the loop runs other tasks
    return name


async def fails_fast() -> None:
    await asyncio.sleep(0.01)
    raise ValueError("bad upstream")


async def blocking() -> None:
    time.sleep(0.2)                          # BUG: blocks the whole event loop


async def main() -> None:
    start = time.perf_counter()
    results = await asyncio.gather(fetch("a", 0.2), fetch("b", 0.2), fetch("c", 0.2))
    assert results == ["a", "b", "c"]                     # argument order, not completion order
    assert time.perf_counter() - start < 0.35             # concurrent: about 0.2s, not 0.6s

    out = await asyncio.gather(fetch("ok", 0.01), fails_fast(), return_exceptions=True)
    assert out[0] == "ok" and isinstance(out[1], ValueError)

    slow = None
    try:
        async with asyncio.TaskGroup() as tg:             # 3.11+
            slow = tg.create_task(fetch("slow", 5))
            tg.create_task(fails_fast())
    except* ValueError as eg:
        assert len(eg.exceptions) == 1
    assert slow.cancelled()                               # the sibling was cancelled

    try:
        async with asyncio.timeout(0.05):                 # 3.11+
            await fetch("too slow", 1)
    except TimeoutError:
        pass

    start = time.perf_counter()
    await asyncio.gather(blocking(), blocking())
    assert time.perf_counter() - start >= 0.4             # serialized: the loop was frozen

    start = time.perf_counter()
    await asyncio.gather(asyncio.to_thread(time.sleep, 0.2), asyncio.to_thread(time.sleep, 0.2))
    assert time.perf_counter() - start < 0.35             # offloaded to threads


asyncio.run(main())
```

**What goes wrong in production:**

- A sync SQLAlchemy session or `requests` call inside an `async def` FastAPI endpoint serializes every request on that worker.
  Either use async drivers or declare the endpoint with plain `def`, which FastAPI runs in a threadpool (see [FastAPI](04-FastAPI.md)).
- Fire-and-forget `asyncio.create_task(...)` without keeping a reference: the loop holds only a weak reference, so the task can be garbage collected mid-flight.
  Keep tasks in a set and discard them in a done callback, or use a `TaskGroup`.
- Unbounded `gather` over 10,000 URLs opens 10,000 sockets; bound it with a `Semaphore` (tested in the [Coding round](11-Coding-Round.md)).
- Swallowing `asyncio.CancelledError` (it is a `BaseException` since 3.8) breaks timeouts and shutdown; re-raise it.
- Debugging: `asyncio.run(main(), debug=True)` or `PYTHONASYNCIODEBUG=1` logs callbacks slower than 100 ms, which finds blocking calls; 3.14 adds `python -m asyncio ps PID` and `pstree PID` to inspect a running process.

---

## Y11. Explain LEGB scope, closures, `nonlocal` and `global`, and the late-binding lambda trap.

- Name lookup order is **L**ocal, **E**nclosing function, **G**lobal (module), **B**uiltins.
- Scope is decided at compile time: any assignment to a name anywhere in a function makes it local for the whole function, which is why reading it before the assignment raises `UnboundLocalError`.
- A **closure** is an inner function that captures variables from its enclosing scope in cells (`func.__closure__`).
- `nonlocal x` rebinds the enclosing function's variable; `global x` rebinds the module variable (a design smell in services: hidden shared state).
- Closures capture **variables, not values**: the lookup happens when the inner function runs, so lambdas made in a loop all see the loop variable's final value.

```python
counter_total = 0


def make_counter():
    count = 0

    def increment():
        nonlocal count                   # rebind the enclosing variable
        count += 1
        return count

    return increment


def bump_global():
    global counter_total
    counter_total += 1


c = make_counter()
c()
c()
assert c() == 3
assert c.__closure__[0].cell_contents == 3
bump_global()
assert counter_total == 1

late = [lambda: i for i in range(3)]
assert [f() for f in late] == [2, 2, 2]          # i is looked up at call time
bound = [lambda i=i: i for i in range(3)]
assert [f() for f in bound] == [0, 1, 2]         # a default arg captures the value now
```

The same trap bites `for handler in handlers: button.on_click(lambda: run(handler))` and callbacks scheduled in a loop; fix with a default argument or `functools.partial(run, handler)`.

---

## Y12. What is the MRO, how does C3 linearization work, and what does `super()` really call?

- The **method resolution order** is the list of classes searched for an attribute, stored in `Cls.__mro__`.
- C3 linearization guarantees that a class comes before its bases, and that the order of bases in each class statement is preserved; if no order satisfies both, class creation fails with `TypeError`.
- `super()` does not mean "the parent": it means "the next class **after this one in the MRO of the instance's type**", which is what makes cooperative multiple inheritance (mixins) work.
- For cooperation, every class in the chain calls `super().__init__(**kwargs)` and consumes only its own keyword arguments.

```python
class Base:
    def __init__(self, **kwargs):
        self.log = ["Base"]
        super().__init__(**kwargs)


class Cached(Base):
    def __init__(self, ttl=60, **kwargs):
        super().__init__(**kwargs)
        self.ttl = ttl
        self.log.append("Cached")


class Audited(Base):
    def __init__(self, auditor="system", **kwargs):
        super().__init__(**kwargs)
        self.auditor = auditor
        self.log.append("Audited")


class Repo(Cached, Audited):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.log.append("Repo")


r = Repo(ttl=5, auditor="ops")
assert [c.__name__ for c in Repo.__mro__] == ["Repo", "Cached", "Audited", "Base", "object"]
assert r.log == ["Base", "Audited", "Cached", "Repo"]     # Cached's super() is Audited here
assert (r.ttl, r.auditor) == (5, "ops")

try:
    class Broken(Base, Cached):                          # base listed before its subclass
        pass
except TypeError as e:
    assert "consistent method resolution order" in str(e)
```

Rule of thumb for interviews: prefer composition; use multiple inheritance only for small, stateless mixins (logging, serialization) that all cooperate through `super()`.

---

## Y13. What is the difference between `@classmethod`, `@staticmethod`, and `@property`?

- `@classmethod` receives the class as `cls`; use it for **alternative constructors** (`from_dict`, `from_row`) because it builds the right subclass.
- `@staticmethod` receives nothing implicit; it is a plain function namespaced in the class, for helpers that need neither `self` nor `cls`.
- `@property` turns a method into a computed attribute with optional setter and deleter, so you can add validation without changing callers.
- `functools.cached_property` (3.8) computes once per instance and stores the result in the instance `__dict__`; it does not work with `__slots__` and does not invalidate itself.

```python
from functools import cached_property


class Order:
    def __init__(self, symbol: str, price: float, qty: int):
        self.symbol = symbol
        self.price = price                   # runs the property setter
        self.qty = qty

    @property
    def price(self) -> float:
        return self._price

    @price.setter
    def price(self, value: float) -> None:
        if value <= 0:
            raise ValueError("price must be positive")
        self._price = value

    @classmethod
    def from_dict(cls, data: dict) -> "Order":
        return cls(data["symbol"], float(data["price"]), int(data["qty"]))

    @staticmethod
    def is_valid_symbol(symbol: str) -> bool:
        return symbol.isalpha() and symbol.isupper()

    @cached_property
    def notional(self) -> float:
        return self.price * self.qty


class LimitOrder(Order):
    pass


o = LimitOrder.from_dict({"symbol": "IBM", "price": "100", "qty": "3"})
assert type(o) is LimitOrder                 # classmethod built the subclass
assert Order.is_valid_symbol("IBM") and not Order.is_valid_symbol("ibm")
try:
    o.price = -1
except ValueError:
    pass
assert o.notional == 300.0
o.price = 200.0
assert o.notional == 300.0                   # stale: cached_property never recomputes
del o.notional                               # invalidate explicitly
assert o.notional == 600.0
```

---

## Y14. Which dunder methods matter most, and what does `__slots__` do?

- `__repr__` (unambiguous, for developers and logs) versus `__str__` (readable, for users); if only `__repr__` exists, `str()` uses it.
- `__eq__` and `__hash__` go together: defining `__eq__` sets `__hash__` to `None` unless you define it too, and only immutable value objects should be hashable.
- Return `NotImplemented` (not `False`) from binary operators for unsupported types so Python can try the reflected operation.
- Others worth naming: `__lt__` plus `functools.total_ordering`, `__len__`, `__bool__`, `__getitem__`, `__iter__`, `__contains__`, `__call__`, `__enter__`/`__exit__`, `__init_subclass__`, `__getattr__` (only on a miss) versus `__getattribute__` (every access).
- `__slots__` replaces the per-instance `__dict__` with fixed storage: less memory and faster attribute access, but no new attributes, no `cached_property`, and every class in the hierarchy needs slots for the saving to hold.

```python
import functools
import tracemalloc


@functools.total_ordering
class Money:
    __slots__ = ("cents", "currency")

    def __init__(self, cents: int, currency: str = "USD"):
        self.cents = cents
        self.currency = currency

    def __repr__(self) -> str:
        return f"Money({self.cents!r}, {self.currency!r})"

    def __str__(self) -> str:
        return f"{self.cents / 100:.2f} {self.currency}"

    def __eq__(self, other):
        if not isinstance(other, Money):
            return NotImplemented
        return (self.cents, self.currency) == (other.cents, other.currency)

    def __hash__(self) -> int:
        return hash((self.cents, self.currency))

    def __lt__(self, other):
        if not isinstance(other, Money) or other.currency != self.currency:
            return NotImplemented
        return self.cents < other.cents

    def __add__(self, other):
        if not isinstance(other, Money) or other.currency != self.currency:
            return NotImplemented
        return Money(self.cents + other.cents, self.currency)

    def __bool__(self) -> bool:
        return self.cents != 0


a, b = Money(150), Money(99)
assert str(a + b) == "2.49 USD" and repr(b) == "Money(99, 'USD')"
assert sorted([a, b]) == [b, a] and a >= b              # >= from total_ordering
assert len({Money(1), Money(1)}) == 1 and not Money(0)
try:
    a.note = "x"
except AttributeError:
    pass                                                # no __dict__, no new attributes


class Plain:
    def __init__(self, a, b):
        self.a, self.b = a, b


class Slotted:
    __slots__ = ("a", "b")

    def __init__(self, a, b):
        self.a, self.b = a, b


def bytes_for(cls, n=10_000) -> int:
    tracemalloc.start()
    objs = [cls(i, i) for i in range(n)]
    size, _ = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    return size


plain, slotted = bytes_for(Plain), bytes_for(Slotted)
assert slotted < plain
print(f"slots use {slotted / plain:.0%} of the memory of a plain class")
```

On 3.14.7 this printed `slots use 68% of the memory of a plain class`; the gap is smaller than on old Pythons because CPython 3.11+ stores plain instance attributes inline and builds the `__dict__` lazily, so measure before claiming a saving.

---

## Y15. ABCs or Protocols: which do you use for interfaces?

- An **ABC** (`abc.ABC` with `@abstractmethod`) is **nominal**: implementations must inherit, and instantiating a subclass with unimplemented abstract methods raises `TypeError`.
  Good when you own the hierarchy and want shared template methods.
- A **Protocol** (`typing.Protocol`, PEP 544, 3.8) is **structural**: any class with matching methods satisfies it, checked by mypy or pyright, with no inheritance.
  Good for ports and adapters over third-party classes, and for test fakes.
- `@runtime_checkable` allows `isinstance` against a Protocol, but it only checks that the attributes exist, not their signatures.

```python
from abc import ABC, abstractmethod
from typing import Protocol, runtime_checkable


class Repository(ABC):
    @abstractmethod
    def get(self, key: str) -> dict | None: ...

    def get_or_raise(self, key: str) -> dict:         # shared template method
        row = self.get(key)
        if row is None:
            raise KeyError(key)
        return row


class Incomplete(Repository):
    pass


class MemoryRepo(Repository):
    def __init__(self):
        self.rows = {"42": {"id": "42"}}

    def get(self, key):
        return self.rows.get(key)


try:
    Incomplete()
except TypeError as e:
    assert "abstract" in str(e)
assert MemoryRepo().get_or_raise("42") == {"id": "42"}


@runtime_checkable
class SupportsClose(Protocol):
    def close(self) -> None: ...


class FileHandle:                                   # no inheritance needed
    def close(self) -> None:
        pass


class Liar:
    close = 42                                      # not callable, still passes isinstance


assert isinstance(FileHandle(), SupportsClose)
assert not isinstance(object(), SupportsClose)
assert isinstance(Liar(), SupportsClose)            # runtime check is attribute presence only
```

---

## Y16. Compare dataclasses, Pydantic models, NamedTuple, and attrs. When do you use each? (must know)

> "Dataclasses generate `__init__`, `__repr__`, and `__eq__` for plain internal data, with zero runtime validation.
> Pydantic validates and coerces untrusted input at the boundary: request bodies, config, messages from Kafka, third-party API responses.
> So I validate once at the edge with Pydantic and pass dataclasses or domain objects inside the service."

| | `@dataclass` | Pydantic `BaseModel` (v2) | `typing.NamedTuple` | `attrs` (`@define`) |
| --- | --- | --- | --- | --- |
| Runtime validation | none (type hints are not checked) | yes, with type coercion in lax mode, `strict=True` to disable | none | opt-in validators and converters |
| Mutability | mutable, `frozen=True` available | mutable, `frozen=True` via `ConfigDict` | immutable (it is a tuple) | mutable, `@frozen` available |
| Serialization | `dataclasses.asdict` | `model_dump`, `model_dump_json`, JSON Schema | `_asdict` | `attrs.asdict` |
| Speed | fast construction | validation cost (the core is Rust, `pydantic-core`) | fastest, tuple-sized | fast, slots by default |
| Dependency | stdlib | third party | stdlib | third party |
| Typical use | internal DTOs, domain objects | API schemas (FastAPI), settings, external data | small immutable records, tuple unpacking | when you want dataclass-plus features without Pydantic |

```python
from dataclasses import FrozenInstanceError, dataclass, field
from typing import NamedTuple

from pydantic import BaseModel, ConfigDict, Field, ValidationError, field_validator


@dataclass(frozen=True, slots=True)
class Fill:
    symbol: str
    qty: int
    tags: list[str] = field(default_factory=list)


class Point(NamedTuple):
    x: float
    y: float


class OrderIn(BaseModel):
    model_config = ConfigDict(extra="forbid", str_strip_whitespace=True)

    symbol: str = Field(min_length=1, max_length=12)
    qty: int = Field(gt=0)
    tags: list[str] = []                      # safe: Pydantic copies the default per instance

    @field_validator("symbol")
    @classmethod
    def upper(cls, v: str) -> str:
        return v.upper()


f = Fill("IBM", "10")                         # no validation: qty is the string "10"
assert f.qty == "10"
try:
    f.qty = 5
except FrozenInstanceError:
    pass

p = Point(1.0, 2.0)
assert p == (1.0, 2.0) and p.x == 1.0

o = OrderIn.model_validate({"symbol": " ibm ", "qty": "10"})    # lax mode: "10" -> 10
assert o.symbol == "IBM" and o.qty == 10
assert o.model_dump() == {"symbol": "IBM", "qty": 10, "tags": []}

for bad in ({"symbol": "IBM", "qty": 0}, {"symbol": "IBM", "qty": 1, "side": "buy"}):
    try:
        OrderIn.model_validate(bad)
    except ValidationError as e:
        assert e.error_count() == 1
    else:
        raise AssertionError("expected ValidationError")

a, b = OrderIn(symbol="A", qty=1), OrderIn(symbol="B", qty=1)
a.tags.append("x")
assert b.tags == []

try:
    @dataclass
    class Bad:
        tags: list = []
except ValueError as e:
    assert "default_factory" in str(e)
```

**Follow-ups they ask:** Pydantic v1 to v2 renames (`parse_obj` to `model_validate`, `dict()` to `model_dump`, `@validator` to `@field_validator`, `class Config` to `model_config = ConfigDict(...)`, `orm_mode` to `from_attributes`) are covered in [FastAPI](04-FastAPI.md).

---

## Y17. How do you use type hints and mypy in a large codebase?

- Hints are not enforced at runtime; they are read by type checkers (mypy, pyright), IDEs, and frameworks that introspect them on purpose (FastAPI, Pydantic, SQLAlchemy `Mapped[...]`).
- Modern syntax: builtin generics `list[int]` (3.9), `X | None` (3.10), `Self` (3.11), the `type` alias statement and `def f[T](...)` generics (3.12, PEP 695), `@override` (3.12).
- Useful constructs: `Literal`, `TypedDict` for dict-shaped JSON, `Protocol` for structural interfaces, `Annotated[T, metadata]` (FastAPI dependencies and Pydantic constraints), `Callable`, `ParamSpec` for typed decorators, `TypeGuard`/`TypeIs` for narrowing.
- Rollout in a legacy codebase: run mypy in CI on new and touched modules first, add `--strict` per package through config, keep `# type: ignore[code]` specific, and ratchet.
  **[fill in: how typing was rolled out, or not, during the 1M+ LOC Python 3.8 migration]**

```python
# check with mypy
from typing import Literal, TypedDict

type Side = Literal["buy", "sell"]            # 3.12 type alias statement


class OrderDict(TypedDict):
    symbol: str
    qty: int
    side: Side


def first[T](items: list[T]) -> T | None:     # 3.12 generic syntax
    return items[0] if items else None


def notional(order: OrderDict, price: float) -> float:
    return order["qty"] * price


o: OrderDict = {"symbol": "IBM", "qty": 10, "side": "buy"}
print(notional(o, 101.5))
bad: OrderDict = {"symbol": "IBM", "qty": "10", "side": "hold"}   # runs fine, mypy flags it
n: int = first([1, 2, 3])                                         # T | None is not int
```

It runs and prints `1015.0`, because nothing checks hints at runtime.
mypy output:

```text
error: Incompatible types (expression has type "str", TypedDict item "qty" has type "int")  [typeddict-item]
error: Incompatible types (expression has type "Literal['hold']", TypedDict item "side" has type "Literal['buy', 'sell']")  [typeddict-item]
error: Incompatible types in assignment (expression has type "int | None", variable has type "int")  [assignment]
```

---

## Y18. What are your best practices for exceptions? (must know)

- Define a small **custom hierarchy** rooted at one `AppError` so callers and the API layer can catch by category and map to HTTP status codes in one handler.
- Catch the **narrowest** exception you can handle; never `except:` or `except Exception: pass`; if you log and continue, use `logger.exception(...)` so the traceback is kept.
- `raise NewError(...) from exc` chains the cause (`__cause__`) so the traceback shows both; `from None` hides an internal detail on purpose.
- `finally` always runs (cleanup); `else` runs only if no exception was raised; never `return` from `finally`, since it swallows the in-flight exception (3.14 emits a `SyntaxWarning`, PEP 765).
- `ExceptionGroup` and `except*` (3.11) report several independent failures at once (batch validation, `TaskGroup`); `err.add_note(...)` (3.11) attaches context without wrapping.
- EAFP (try it, catch the error) is idiomatic Python and avoids check-then-act races, for example on files and dict keys.

```python
class AppError(Exception):
    """Root of the service's exception hierarchy."""
    status_code = 500


class NotFoundError(AppError):
    status_code = 404


class UpstreamError(AppError):
    status_code = 502


def load_price(symbol: str, cache: dict[str, float]) -> float:
    try:
        return cache[symbol]
    except KeyError as exc:
        raise NotFoundError(f"no price for {symbol}") from exc      # keep the cause


try:
    load_price("IBM", {})
except AppError as err:                                             # catch by category
    assert err.status_code == 404
    assert isinstance(err.__cause__, KeyError)


def validate(rows: list[dict]) -> None:
    errors = [ValueError(f"row {i}: qty must be positive")
              for i, row in enumerate(rows) if row["qty"] <= 0]
    if errors:
        raise ExceptionGroup("invalid batch", errors)


caught = []
try:
    validate([{"qty": 1}, {"qty": 0}, {"qty": -5}])
except* ValueError as eg:
    caught.extend(str(e) for e in eg.exceptions)
assert caught == ["row 1: qty must be positive", "row 2: qty must be positive"]

try:
    try:
        int("abc")
    except ValueError as e:
        e.add_note("while parsing field 'qty' of order 42")
        raise                                                       # bare raise keeps the traceback
except ValueError as e:
    assert e.__notes__ == ["while parsing field 'qty' of order 42"]

cleaned = []
try:
    try:
        raise UpstreamError("pricing service timed out")
    finally:
        cleaned.append("released connection")                       # runs on the way out
except UpstreamError:
    pass
assert cleaned == ["released connection"]
```

---

## Y19. How does Python manage memory? How would you find a memory leak in a service?

- CPython frees most objects immediately through **reference counting**: when the count hits zero the object is deallocated.
- Reference **cycles** (objects that point at each other) never reach zero, so a **cyclic garbage collector** runs periodically; it is generational (thresholds from `gc.get_threshold()`, `(2000, 10, 10)` on 3.14.7).
  3.14.0 to 3.14.4 shipped an incremental collector, and 3.14.5 reverted to the 3.13 generational one after reports of memory pressure in production.
- Small objects come from the `pymalloc` arena allocator, so freed memory is often reused by Python rather than returned to the OS; RSS staying flat after a spike is not by itself a leak.
- Leak sources in real services: unbounded module-level caches and dicts, `functools.lru_cache(maxsize=None)` on methods (the cache keeps `self` alive), listeners and callbacks never unregistered, and large objects held by exception tracebacks.
- Tools: `tracemalloc` snapshots diffed over time, `gc.get_objects()` and `objgraph`, `memray`, and `py-spy dump` for a live process.

```python
import functools
import gc
import sys
import weakref


class Node:
    def __init__(self, name):
        self.name = name
        self.peer = None


a = Node("a")
base = sys.getrefcount(a)
alias = a
assert sys.getrefcount(a) == base + 1
del alias
assert sys.getrefcount(a) == base

collected = []
x, y = Node("x"), Node("y")
x.peer, y.peer = y, x                         # a reference cycle
weakref.finalize(x, collected.append, "x")
del x, y                                      # counts never reach zero
gc.collect()                                  # the cycle collector frees them
assert collected == ["x"]


class PriceService:
    @functools.lru_cache(maxsize=None)        # cache key includes self: a leak
    def lookup(self, symbol: str) -> str:
        return symbol.lower()


svc = PriceService()
ref = weakref.ref(svc)
svc.lookup("IBM")
del svc
gc.collect()
assert ref() is not None                      # still alive, held by the class-level cache
PriceService.lookup.cache_clear()
assert ref() is None
```

---

## Y20. What does `if __name__ == "__main__":` do, and how do imports work?

- Every module has `__name__`; it is the dotted module name when imported and `"__main__"` when the file is run as a script or with `python -m`.
- The guard keeps script-only code (argument parsing, starting a server) from running on import, which matters for tests, `multiprocessing` spawn workers, and reuse.
- An `import` runs the module's top-level code **once** and caches it in `sys.modules`; later imports return the cached module.
- Circular imports fail when module A needs a name from B at import time while B is still executing; fix by moving the shared piece to a third module, importing inside the function, or importing the module rather than the name (`import b` then `b.thing` at call time).
- Avoid side effects at import time (DB connections, reading env and failing, starting threads); build them in an app factory or FastAPI `lifespan` instead.
- Run package modules as `python -m package.module` so absolute imports resolve from the project root.

```python
import sys


def main(argv: list[str]) -> int:
    print(f"args={argv}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))       # exit code for shells and CI
```

Output:

```text
args=[]
```

---

## Y21. How do you manage dependencies and packaging? pip, venv, Poetry, or uv?

- One virtual environment per project (`python -m venv .venv`), never `sudo pip` into the system Python.
- `pyproject.toml` (PEP 518, 517, 621) is the single project file: metadata, dependencies, build backend, and tool config (pytest, mypy, ruff).
- Separate **abstract** dependencies (ranges in `pyproject.toml`) from a **lock file** that pins the full resolved tree with hashes, so CI, Docker images, and production install the same bits.
- Put secrets in the environment or a secrets manager, never in `pyproject.toml` or the repo.

| Tool | What it is | Lock file | Notes |
| --- | --- | --- | --- |
| `pip` + `venv` | stdlib-adjacent baseline | none native; `pip freeze` or `pip-tools` `pip-compile --generate-hashes` | universal, slower resolver, supports `pip install --group dev` for PEP 735 groups |
| Poetry | dependency manager and build backend | `poetry.lock` | mature, its own resolver and CLI |
| uv | Rust-based installer, resolver, venv and Python version manager | `uv.lock` (cross-platform) | much faster installs, `uv sync`, `uv run`, `uv add`, `uv python install`, pip-compatible `uv pip` |

```toml
[project]
name = "pricing-service"
version = "1.4.0"
requires-python = ">=3.12"
dependencies = [
    "fastapi>=0.115",
    "sqlalchemy>=2.0",
    "pydantic>=2.7",
]

[dependency-groups]
dev = ["pytest>=8", "mypy>=1.10", "ruff>=0.6"]

[build-system]
requires = ["hatchling"]
build-backend = "hatchling.build"

[tool.pytest.ini_options]
testpaths = ["tests"]

[tool.mypy]
strict = true
```

```sh
uv sync --locked            # CI and Docker: fail if uv.lock is out of date
uv add httpx                # add a dependency and update the lock
uv run pytest -q            # run inside the project environment
```

**[fill in: which tool the 1M+ LOC Python 3.8 migration used for dependency pinning and how conflicts were resolved]**

---

## Y22. Which Python 3.8 to 3.14 features should a senior developer be able to name?

| Version | Features worth naming |
| --- | --- |
| 3.8 | walrus `:=` (PEP 572), positional-only `/` (PEP 570), f-string `f"{x=}"` debugging, `functools.cached_property`, `typing.Protocol`, `TypedDict`, `Literal`, `Final` |
| 3.9 | dict union `d1 \| d2` (PEP 584), builtin generics `list[int]` (PEP 585), `str.removeprefix`/`removesuffix`, `zoneinfo` |
| 3.10 | structural pattern matching `match`/`case` (PEP 634), `X \| Y` union types (PEP 604), `zip(strict=True)`, parenthesized context managers, dataclass `slots=True` and `kw_only=True`, much better error messages |
| 3.11 | Faster CPython (an average of about 1.25x on pyperformance), `ExceptionGroup` and `except*` (PEP 654), `asyncio.TaskGroup` and `asyncio.timeout`, `tomllib`, `typing.Self`, `add_note`, fine-grained error locations in tracebacks |
| 3.12 | type parameter syntax `def f[T]` and the `type` statement (PEP 695), f-strings formalized in the grammar so quotes can be reused inside (PEP 701), `itertools.batched`, `typing.override`, per-interpreter GIL in the C API (PEP 684), `distutils` removed |
| 3.13 | experimental free-threaded build (PEP 703), experimental JIT (PEP 744), new interactive REPL, `copy.replace`, dead batteries removed (PEP 594: `cgi`, `telnetlib`, and others), defined `locals()` semantics (PEP 667) |
| 3.14 | free-threading officially supported (PEP 779), deferred annotations (PEP 649/749), template strings `t"..."` (PEP 750), `concurrent.interpreters` (PEP 734), `except A, B:` without parentheses (PEP 758), `SyntaxWarning` for `return`/`break`/`continue` leaving `finally` (PEP 765), `compression.zstd` (PEP 784), `uuid.uuid7`, `python -m asyncio ps` |

A quick tour you can type from memory (3.10+ syntax):

```python
import re


def classify(event: dict) -> str:
    match event:                                         # 3.10 structural pattern matching
        case {"type": "order", "qty": int(qty)} if qty > 0:
            return f"order of {qty}"
        case {"type": "cancel", "id": str(order_id)}:
            return f"cancel {order_id}"
        case _:
            return "unknown"


assert classify({"type": "order", "qty": 5}) == "order of 5"
assert classify({"type": "cancel", "id": "A1"}) == "cancel A1"
assert classify({"type": "order", "qty": -1}) == "unknown"

lines = ["id=7", "noise", "id=42"]
ids = [int(m.group(1)) for line in lines if (m := re.match(r"id=(\d+)", line))]  # 3.8 walrus
assert ids == [7, 42]

qty = 3
assert f"{qty=}" == "qty=3"                              # 3.8 self-documenting f-string
assert {"a": 1} | {"b": 2} == {"a": 1, "b": 2}           # 3.9 dict union
assert "v1.2".removeprefix("v") == "1.2"                  # 3.9
d = {"k": 1}
assert f"{d["k"]}" == "1"                                # 3.12: same quotes inside an f-string
```

**Migration note** for the 3.8 upgrade story in [Pitch and resume](01-Pitch-and-Resume.md): 3.8 reached end of life in October 2024, and the upgrade path is to run the test suite with warnings as errors, fix deprecations version by version, and pin C-extension wheels that lag.
**[fill in: the concrete blockers you hit, how you staged the rollout, and the measured outcome]**

---

## Y23. Predict the output: the classic trick snippets.

Interviewers paste one of these and ask what it prints; say your reasoning out loud before answering.

**1. Late-binding closures.**

```python
funcs = [lambda: i * 10 for i in range(3)]
print([f() for f in funcs])
```

Output:

```text
[20, 20, 20]
```

Each lambda looks up `i` when called, after the comprehension finished; fix with `lambda i=i: i * 10`.

**2. List multiplication aliases the inner list.**

```python
grid = [[0] * 3] * 3
grid[0][0] = 1
print(grid)
```

Output:

```text
[[1, 0, 0], [1, 0, 0], [1, 0, 0]]
```

**3. Augmented assignment on a list inside a tuple.**

```python
t = ([1],)
try:
    t[0] += [2]
except TypeError as e:
    print("TypeError:", e)
print(t)
```

Output:

```text
TypeError: 'tuple' object does not support item assignment
([1, 2],)
```

`+=` first calls `list.__iadd__` (which mutates in place and succeeds), then tries to store the result back into the tuple (which fails).

**4. Equal keys collapse in a dict.**

```python
d = {1: "int", 1.0: "float", True: "bool"}
print(d)
```

Output:

```text
{1: 'bool'}
```

`1 == 1.0 == True` and their hashes match, so it is one key; the first key object is kept and the last value wins.

**5. Class attributes are shared.**

```python
class Order:
    tags = []

    def add(self, tag):
        self.tags.append(tag)


a, b = Order(), Order()
a.add("urgent")
print(b.tags)
```

Output:

```text
['urgent']
```

Initialize per-instance state in `__init__`; `self.tags.append` mutates the class-level list.

**6. `UnboundLocalError` from a later assignment.**

```python
x = 10


def f():
    try:
        print(x)
    except UnboundLocalError as e:
        print(type(e).__name__)
    x = 5


f()
```

Output:

```text
UnboundLocalError
```

The assignment makes `x` local to the whole function at compile time.

**7. `return` in `finally` swallows the exception.**

```python
def f():
    try:
        raise ValueError("lost")
    finally:
        return "finally wins"


print(f())
```

Output:

```text
finally wins
```

The in-flight exception is discarded; since 3.14 the compiler emits `SyntaxWarning: 'return' in a 'finally' block` for this (PEP 765).

**8. Generators are single-use.**

```python
g = (n * n for n in range(4))
print(sum(g), sum(g))
```

Output:

```text
14 0
```

**9. Rounding and floating point.**

```python
print(round(2.5), round(3.5), round(-0.5))
print(0.1 + 0.2 == 0.3, abs(0.1 + 0.2 - 0.3) < 1e-9)
```

Output:

```text
2 4 0
False True
```

`round` uses banker's rounding (half to even); use `decimal.Decimal` with an explicit rounding mode for money, and `math.isclose` for float comparison.

**10. NaN and container identity.**

```python
nan = float("nan")
print(nan == nan, [nan] == [nan], nan in [nan])
```

Output:

```text
False True True
```

Container comparison and `in` check identity (`is`) before `==`.

**11. Removing from a list while iterating it.**

```python
nums = [1, 2, 2, 3]
for n in nums:
    if n == 2:
        nums.remove(n)
print(nums)
```

Output:

```text
[1, 2, 3]
```

Removing the first 2 shifts the second 2 into the slot the iterator already passed; iterate over a copy or build a new list with a comprehension.

**12. `+=` mutates, `+` rebinds.**

```python
a = [1]
b = a
a += [2]
a = a + [3]
print(a, b)
```

Output:

```text
[1, 2, 3] [1, 2]
```

`a += [2]` extends the shared list in place; `a = a + [3]` builds a new list and rebinds only `a`.

---

## Go deeper

- Vault: [Ultimate Python Advanced Guide](../Ultimate-Python-Advanced-Guide.md), [Python Advanced Guide](../Python-Advanced-Guide.md), [Ultimate Python Design Patterns](../Ultimate-Python-Design-Patterns.md).
- Internals: [Built-in data structures](../Python_Zero_to_Godhood/Chapter_05_UNDER_THE_HOOD_BUILT-IN_DATA_STRUCTURES.md), [The Python data model](../Python_Zero_to_Godhood/Chapter_32_The_Python_Data_Model__Comprehensive_Dunder_Method.md), [Memory allocator and GC](../Python_Zero_to_Godhood/Chapter_23_MEMALLOC_UNDER_THE_HOOD_ARENAS_POOLS_AND_THE_GC.md), [Metaclasses and descriptors](../Python_Zero_to_Godhood/Chapter_25_METACLASSES_AND_DESCRIPTOR_PROTOCOL_ARCHITECTURE.md).
- Concurrency: [The GIL](../Python_Zero_to_Godhood/Chapter_10_CONCURRENCY_MECHANICS__THE_GLOBAL_INTERPRETER_LOCK.md), [CPU- and I/O-bound concurrency](../Python_Zero_to_Godhood/Chapter_26_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md), [Free-threaded Python](../Python_Zero_to_Godhood/Chapter_20_FREE-THREADED_PYTHON_GIL_REMOVAL_INTERNALS.md), [Subinterpreters](../Python_Zero_to_Godhood/Chapter_22_SUBINTERPRETERS__MULTI-CORE_PARALLELISM.md).
- Language features: [Exception groups](../Python_Zero_to_Godhood/Chapter_19_EXCEPTION_GROUPS_AND_TRACEBACK_ENHANCEMENTS.md), [Context managers](../Python_Zero_to_Godhood/Chapter_54_Context_Managers_contextlib.md), [ABCs](../Python_Zero_to_Godhood/Chapter_53_Abstract_Base_Classes_abc.md), [Typing](../Python_Zero_to_Godhood/Chapter_56_The_Typing_System_Static_Analysis_vs_Runtime_Enfor.md), [Packaging](../Python_Zero_to_Godhood/Chapter_57_The_Python_Packaging_Ecosystem_PEP_517_to_Wheels.md), [Python 3.14 and beyond](../Python_Zero_to_Godhood/Chapter_61_The_Future_of_Python_314_and_Beyond.md), [Anti-patterns and pitfalls](../Python_Zero_to_Godhood/Chapter_70_Python_Anti-Patterns_and_Common_Pitfalls.md).
- Pack: [FastAPI](04-FastAPI.md) for Pydantic v2 and async endpoints, [Testing, debugging, production](10-Testing-Debugging-Production.md) for profiling, [Coding round](11-Coding-Round.md) for these ideas as timed exercises.
- Official: [Data model](https://docs.python.org/3/reference/datamodel.html), [asyncio tasks](https://docs.python.org/3/library/asyncio-task.html), [Free-threading HOWTO](https://docs.python.org/3/howto/free-threading-python.html), [What's New in 3.14](https://docs.python.org/3/whatsnew/3.14.html), [MRO](https://docs.python.org/3/howto/mro.html), [Pydantic models](https://docs.pydantic.dev/latest/concepts/models/), [mypy](https://mypy.readthedocs.io/en/stable/), [Writing pyproject.toml](https://packaging.python.org/en/latest/guides/writing-pyproject-toml/), [uv](https://docs.astral.sh/uv/).
