# Chapter 10: Asyncio Inception, Pathlib, and Enum (Python 3.4)

Python 3.4 shipped three additions that each replaced a pile of ad-hoc idioms with a designed
abstraction: **asyncio** (PEP 3156) gave single-threaded I/O concurrency a standard event loop;
**pathlib** (PEP 428) replaced string-based path juggling with path *objects*; and **enum**
(PEP 435) gave Python real enumerations. This chapter teaches the model behind each. For asyncio
we focus on the **event loop and the Future/Task machinery** — its 3.4 inception — and defer the
`async`/`await` *syntax* to Chapter 11 and the full internals and structured concurrency to
Vol IX.

## Section Index
- **10.1** Asyncio inception: the event loop, selectors, Futures, and Tasks (PEP 3156)
- **10.2** `pathlib`: object-oriented filesystem paths (PEP 428)
- **10.3** `enum`: enumerations via a metaclass (PEP 435)
- **10.4** Performance and anti-patterns
- **10.5** Summary and cross-references

---

## 10.1 Asyncio inception: the event loop, selectors, Futures, and Tasks (PEP 3156)

**Why this exists.** For I/O-bound workloads — thousands of sockets mostly *waiting* — a thread
per connection wastes memory and pays context-switch and GIL-contention costs, while raw callback
code becomes "callback hell." **asyncio** offers a third model: **one thread, one event loop,
cooperative multitasking**. Tasks voluntarily suspend at `await` points; while one waits on I/O,
the loop runs others. No preemption, no per-connection thread, no data races on shared state
within the loop.

**What the interpreter/loop actually does.** asyncio is built on the OS's **I/O multiplexing**:

1. The loop registers each socket/file descriptor with the `selectors` module, which maps to the
   best available syscall — `epoll` (Linux), `kqueue` (macOS/BSD), or `select`/`poll` elsewhere —
   noting which event (readable/writable) each task awaits.
2. The loop calls `selector.select()`, which **blocks the single thread in the kernel** until at
   least one descriptor is ready (or a timer fires).
3. On wake, the kernel returns the ready descriptors; the loop runs each one's callback, which
   resumes the suspended task.

Two objects carry results: a **`Future`** is a placeholder for a value that will exist later
(states PENDING → FINISHED/CANCELLED, with done-callbacks); a **`Task`** is a `Future` subclass
that drives a coroutine. The task's `_step` calls `coro.send(...)` to advance the coroutine to its
next suspension point, registers itself as the done-callback of whatever future the coroutine is
waiting on, and yields to the loop; when that future completes, `_step` runs again and resumes the
coroutine with the result.

**The historical syntax (3.4, removed in 3.11).** In 3.4 a coroutine was a generator decorated
with `@asyncio.coroutine` that delegated with `yield from` — exactly the `yield from` channel from
Chapter 9, now feeding the event loop:

```python
# Caption: 3.4-era generator coroutine. NOT executed here — @asyncio.coroutine was REMOVED
# in Python 3.11. Shown to connect asyncio to the yield-from machinery of Chapter 9.
import asyncio

@asyncio.coroutine
def fetch_data():
    yield from asyncio.sleep(1)     # delegate to asyncio.sleep's awaitable
    return "payload_data"
```

We confirmed the decorator is gone:

```python
# Caption: the legacy decorator no longer exists.
import asyncio
print("hasattr(asyncio, 'coroutine'):", hasattr(asyncio, "coroutine"))
```

Verified output (CPython 3.13.5):

```text
hasattr(asyncio, 'coroutine'): False
```

**The modern equivalent** uses Chapter 11's `async`/`await` syntax but the *same* event-loop model.
The payoff — true overlap of waits on one thread — is measurable: three 0.05 s sleeps run
**concurrently**, finishing in ~0.05 s, not 0.15 s:

```python
# Caption: the event loop overlaps waits — concurrency on a single thread.
import asyncio, time

async def worker(name, delay):
    await asyncio.sleep(delay)
    return f"{name} done"

async def main():
    t0 = time.perf_counter()
    results = await asyncio.gather(worker("A", 0.05), worker("B", 0.05), worker("C", 0.05))
    return results, time.perf_counter() - t0

results, elapsed = asyncio.run(main())
print("results:", results)
print("elapsed (~0.05s, not 0.15s):", round(elapsed, 3), "s")
```

Verified output (CPython 3.13.5):

```text
results: ['A done', 'B done', 'C done']
elapsed (~0.05s, not 0.15s): 0.051 s
```

**The senior-engineer contrast.** This is cooperative scheduling, like Go's goroutines or
Node's event loop — but explicit: a task runs uninterrupted until it `await`s, so there is no
preemption and no lock needed for data touched only between awaits. The flip side (the headline
anti-pattern, §10.4) is that a *blocking* call (a synchronous `requests.get`, a CPU-bound loop)
stalls the **entire** loop and every other task. The `async`/`await` syntax is Chapter 11; event
loop internals, `TaskGroup`, cancellation, and structured concurrency are Vol IX.

---

## 10.2 `pathlib`: object-oriented filesystem paths (PEP 428)

**Why this exists.** Before 3.4, a path was a `str`, and manipulating it meant a grab-bag of
`os.path` functions (`join`, `dirname`, `splitext`) plus manual separator handling. **pathlib**
makes a path a typed object with methods, an overloaded `/` join operator, and a clean split
between *pure* (string-only) and *concrete* (touches the filesystem) operations:

```text
            PurePath              (path algebra only; no I/O)
           /        \
 PurePosixPath   PureWindowsPath
       |                |
   PosixPath       WindowsPath    (+ filesystem I/O: exists, glob, read_text, ...)
           \        /
             Path                 (instantiates Posix/Windows for the host OS)
```

`PurePath` does path *algebra* with no syscalls — and lets you parse foreign paths (Windows paths
on Linux). `Path` adds filesystem operations. `Path(".")` dynamically becomes a `PosixPath` or
`WindowsPath` for the host.

```python
# Caption: path objects — / for joining, rich component accessors, cross-platform pure paths.
from pathlib import Path, PureWindowsPath

p = Path("/var") / "log" / "nginx.log"     # __truediv__ joins
print("joined:", p)
print("parts:", p.parts, "| name:", p.name, "| suffix:", p.suffix, "| stem:", p.stem)
print("with_suffix:", p.with_suffix(".gz"))
print("parent:", p.parent)
print("Windows path parsed on this host:", PureWindowsPath("C:/a/b"))
print("Path('.') concrete type:", type(Path(".")).__name__)
```

Verified output (CPython 3.13.5):

```text
joined: /var/log/nginx.log
parts: ('/', 'var', 'log', 'nginx.log') | name: nginx.log | suffix: .log | stem: nginx
with_suffix: /var/log/nginx.gz
parent: /var/log
Windows path parsed on this host: C:\a\b
Path('.') concrete type: PosixPath
```

The `/` operator (`__truediv__`) reads naturally and handles separators per-OS, so the same code
is correct on Windows and POSIX. `Path` also unifies what used to be scattered calls:
`p.read_text(encoding=...)`, `p.write_bytes(...)`, `p.glob("*.log")`, `p.mkdir(parents=True)`,
`p.exists()`, `p.stat()`. Prefer `pathlib` over `os.path` in new code; the standard library now
accepts path-like objects everywhere via `os.PathLike`.

---

## 10.3 `enum`: enumerations via a metaclass (PEP 435)

**Why this exists.** Before 3.4, "enums" were bare module constants (`RED = 1`) — no namespacing,
no type safety, no iteration, no readable `repr`, and any int compared equal to them. **enum**
provides real enumerations: named, singleton, immutable, iterable members.

**What the metaclass does.** An `Enum` subclass is built by the `EnumMeta` (a.k.a. `EnumType`)
metaclass (the metaclass machinery is Chapter 4 / Vol VIII). It uses a custom namespace dict
(`_EnumDict`) during class creation to capture member definitions in order and reject duplicates,
then replaces each `NAME = value` with a **singleton instance** of the enum carrying that `name`
and `value`, and installs lookup hooks:

```python
# Caption: enum members are singletons, iterable, looked up by name or value, and immutable.
from enum import Enum

class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3

print("members:", [c.name for c in Color])          # __iter__ in declaration order
print("by name  Color['RED']:", Color["RED"])        # __getitem__
print("by value Color(2):", Color(2))                # __call__
print("singleton:", Color.RED is Color(1))           # same object every time
try:
    Color.RED.value = 99                              # immutable
except AttributeError as e:
    print("immutable ->", type(e).__name__)
```

Verified output (CPython 3.13.5):

```text
members: ['RED', 'GREEN', 'BLUE']
by name  Color['RED']: Color.RED
by value Color(2): Color.GREEN
singleton: True
immutable -> AttributeError
```

Because members are singletons, compare them with `is`/`==` and switch on them in `match`/`case`
(Chapter 15). The `enum` family extends well beyond this base — `IntEnum` (int-compatible),
`Flag`/`IntFlag` (bitwise combinations), `auto()` (auto-numbering), and `StrEnum` (3.11) — all of
which are developed in the dedicated `enum` reference in Vol XII.

---

## 10.4 Performance and anti-patterns

- **Never block the event loop.** A synchronous network call, file read, or CPU-bound loop inside
  a coroutine freezes *every* task. Use async libraries, or offload blocking work with
  `loop.run_in_executor` / `asyncio.to_thread` (Vol IX). This is the single most common asyncio
  bug.
- **Don't mix paradigms thoughtlessly.** asyncio is for I/O concurrency; CPU parallelism still
  needs processes or the free-threaded build (Vol VII). One thread means one core.
- **`pathlib` has slight overhead vs raw `str`.** Each operation constructs a new immutable
  `Path`. In a tight loop over millions of paths, `os.path` string ops can be faster; everywhere
  else, prefer `pathlib` for correctness and readability.
- **Enum pitfalls:** duplicate *values* create **aliases** (a second name for the same member),
  not new members — use `@enum.unique` to forbid that; and `IntEnum` members compare equal to
  plain ints, which can mask type errors (prefer plain `Enum` unless int-compatibility is
  required).

---

## 10.5 Summary and cross-references

- **asyncio** (PEP 3156) is a **single-threaded cooperative event loop** over OS I/O multiplexing
  (`selectors` → `epoll`/`kqueue`/`select`); **Futures** hold pending results and **Tasks** drive
  coroutines by resuming them when their awaited future completes. The 3.4 `@asyncio.coroutine` +
  `yield from` syntax is gone (removed 3.11); the model is unchanged.
- **pathlib** (PEP 428) makes paths objects: `PurePath` (algebra) vs `Path` (I/O), the `/` join
  operator, and a rich component/IO API. Prefer it over `os.path`.
- **enum** (PEP 435) builds **singleton, immutable, iterable** members via `EnumMeta`, with lookup
  by name (`Color['RED']`) and value (`Color(1)`).

**Cross-references.** `yield from` and the suspendable frame asyncio resumes → Chapter 9. Native
`async`/`await` syntax → Chapter 11. Event-loop internals, `TaskGroup`, cancellation, and
structured concurrency → Vol IX. The GIL and why one loop is one core → Vol VII. The full `enum`
family (`IntEnum`/`Flag`/`auto`/`StrEnum`) and `graphlib` → Vol XII. `pathlib`/`os` filesystem
reference → Vol XIV.
