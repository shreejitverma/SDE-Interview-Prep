# Chapter 8: Standard Library Consolidation and the New GIL (Python 3.1–3.2)

The 3.1–3.2 releases were consolidation, not revolution — but two of their additions shaped how
Python is written for the next decade: **Antoine Pitrou's new GIL** (3.2), which replaced a
pathological thread-switching scheme, and **`concurrent.futures`** (PEP 3148), which gave thread
and process parallelism one clean API. Alongside them came the collection types (`OrderedDict`,
`Counter`) and `argparse`. This chapter treats each at the level appropriate to its release and
points to the volume where it is taught in full — the GIL's complete model and the free-threaded
build are Vol VII; concurrency engineering is Vol IX; the collections and `argparse` references
are Vol XII and XV.

## Section Index
- **8.1** The new GIL: from the convoy effect to interval switching (3.2)
- **8.2** `concurrent.futures`: unified thread and process pools (PEP 3148)
- **8.3** Standard-library consolidation: `OrderedDict`, `Counter`, `argparse`
- **8.4** Performance and anti-patterns
- **8.5** Summary and cross-references

---

## 8.1 The new GIL: from the convoy effect to interval switching (3.2)

**Why this exists.** The **GIL** (Global Interpreter Lock) is the mutex that lets only one thread
execute Python bytecode at a time, so CPython's reference counting (Chapter 2) needs no per-object
locking. Before 3.2, the GIL was handed off by a **bytecode ticker**: after a thread ran ~100
bytecodes (`sys.checkinterval`), it released the GIL and immediately tried to reacquire it. On a
multi-core machine this produced the **convoy effect** — the just-released thread, still hot on
its core, usually won the GIL back before a thread sleeping on another core could wake, so the
waiter starved and the machine burned cycles on failed mutex handoffs. Worse, the ticker counted
*bytecodes*, not time, so a single long opcode could hold the GIL arbitrarily long.

**The fix (Python 3.2): time-based switching.** Pitrou's GIL switches on a **time interval**
(default 5 ms) instead of a bytecode count. A waiting thread waits on a condition variable for
the interval; if it times out still holding no GIL, it sets a shared `gil_drop_request` flag. The
running thread checks that flag at safe points in the eval loop and, when set, drops the GIL and
hands off — so a contending thread is guaranteed a turn within roughly one interval, killing the
convoy. The interval is the old `checkinterval`'s replacement and is observable:

```python
# Caption: the new GIL switches on time, not bytecode count.
import sys
print("switch interval (seconds):", sys.getswitchinterval())
```

Verified output (CPython 3.13.5):

```text
switch interval (seconds): 0.005
```

That 5 ms is the maximum a CPU-bound thread holds the GIL before being asked to yield. Crucially,
**this changed fairness, not parallelism**: even with the new GIL, two CPU-bound Python threads
still cannot run bytecode simultaneously. True multi-core CPU parallelism requires processes
(§8.2), subinterpreters, or the free-threaded build — all developed in **Vol VII**, the canonical
home for the GIL model. Here, the takeaway is the 3.2 milestone: time-sliced, convoy-free
hand-off.

---

## 8.2 `concurrent.futures`: unified thread and process pools (PEP 3148)

**Why this exists.** Before 3.2, running work concurrently meant hand-managing `threading.Thread`
or `multiprocessing.Process` objects and wiring up your own result plumbing. **PEP 3148** added
`concurrent.futures`: one `Executor` interface with two interchangeable backends — a
`ThreadPoolExecutor` and a `ProcessPoolExecutor` — and a `Future` object that carries each task's
eventual result or exception.

`submit()` returns a `Future` immediately; `as_completed()` yields futures as they finish; `map()`
applies a function across an iterable. Because the API is identical across backends, you choose
threads vs. processes by changing one class name:

```python
# Caption: one API, two backends. Threads for I/O-bound; processes for CPU-bound.
# NOTE: pool code lives under `if __name__ == "__main__"` — required for the spawn
# start method (macOS/Windows default), which re-imports this module in each worker.
import time
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor, as_completed

def io_task(n):          # mostly waiting -> threads release the GIL during the sleep
    time.sleep(0.01)
    return n * n

def cpu_task(n):         # pure computation -> needs separate processes to parallelize
    return sum(i * i for i in range(n))

def main():
    with ThreadPoolExecutor(max_workers=4) as ex:
        futs = [ex.submit(io_task, i) for i in range(5)]
        thread_results = sorted(f.result() for f in as_completed(futs))
    print("thread pool (I/O):  ", thread_results)

    with ProcessPoolExecutor(max_workers=2) as ex:
        process_results = list(ex.map(cpu_task, [1000, 2000, 3000]))
    print("process pool (CPU): ", process_results)

if __name__ == "__main__":
    main()
```

Verified output (CPython 3.13.5):

```text
thread pool (I/O):   [0, 1, 4, 9, 16]
process pool (CPU):  [332833500, 2664667000, 8995500500]
```

**Two mechanics you must internalize.**

- **Threads parallelize I/O, not CPU.** A `ThreadPoolExecutor` shares one interpreter and one GIL;
  its threads overlap only while blocked in C-level I/O (socket/file/`sleep`) that *releases* the
  GIL. For CPU-bound work, threads give no speedup — use processes.
- **Processes pay a serialization tax.** A `ProcessPoolExecutor` runs separate interpreters with
  separate memory, so arguments and results cross the boundary via **`pickle`** through an OS
  pipe. The cost model is roughly
  $t_{\text{pickle}} + t_{\text{IPC}} + t_{\text{compute}} + t_{\text{IPC}} + t_{\text{unpickle}}$;
  for tiny tasks the transfer dominates and a process pool is *slower* than serial code. Pool over
  coarse-grained work, not microtasks.

**The spawn gotcha (verified the hard way).** On macOS and Windows the default start method is
**spawn**, which launches a fresh interpreter that **re-imports your module** to obtain the worker
function. Any code at module top level runs again *in every worker*. That is why the pool code
must sit under `if __name__ == "__main__":` and worker functions must be importable top-level
names (not lambdas or closures, which are unpicklable). Omitting the guard re-executes your
top-level code in each worker — and can recursively spawn pools. The full concurrency
treatment — start methods, shared memory, cancellation, back-pressure — is **Vol IX**.

---

## 8.3 Standard-library consolidation: `OrderedDict`, `Counter`, `argparse`

**`OrderedDict` (PEP 372).** Added in 3.1, when plain dicts were unordered, `OrderedDict`
remembers insertion order via an internal doubly-linked list. Since **Python 3.7 made insertion
order a language guarantee for `dict`**, most of its original purpose is gone — but it is not
obsolete. Two behaviors remain distinct: its `==` is **order-sensitive**, and it offers
`move_to_end()` (and `popitem(last=False)`), which plain dicts lack:

```python
# Caption: OrderedDict's distinguishing features today — order-sensitive eq and move_to_end.
from collections import OrderedDict

od1 = OrderedDict([("a", 1), ("b", 2)])
od2 = OrderedDict([("b", 2), ("a", 1)])
print("OrderedDict == is order-sensitive:", od1 == od2)
print("plain dict   == is order-insensitive:", {"a": 1, "b": 2} == {"b": 2, "a": 1})
od1.move_to_end("a")
print("after move_to_end('a'):", list(od1))
```

Verified output (CPython 3.13.5):

```text
OrderedDict == is order-sensitive: False
plain dict   == is order-insensitive: True
after move_to_end('a'): ['b', 'a']
```

`move_to_end` makes `OrderedDict` the natural building block for an LRU cache. Use a plain dict
when you just need ordered iteration; reach for `OrderedDict` when you need order-sensitive
equality or end-repositioning.

**`Counter`.** A `dict` subclass for tallying, with `most_common()`, multiset arithmetic, and
missing-key-returns-zero semantics:

```python
# Caption: Counter — frequency counting and multiset arithmetic.
from collections import Counter

c = Counter("mississippi")
print("most_common(2):", c.most_common(2))
print("addition:", Counter(a=3, b=1) + Counter(a=1, b=2))
```

Verified output (CPython 3.13.5):

```text
most_common(2): [('i', 4), ('s', 4)]
addition: Counter({'a': 4, 'b': 3})
```

**`argparse` (3.2)** replaced `optparse` as the standard CLI parser: declare arguments, get typed
parsing, help text, and error handling for free:

```python
# Caption: argparse — declarative, typed command-line parsing.
import argparse

parser = argparse.ArgumentParser(prog="demo")
parser.add_argument("--count", type=int, default=1)
parser.add_argument("name")
ns = parser.parse_args(["--count", "3", "widget"])
print("name:", ns.name, "| count:", ns.count)
```

Verified output (CPython 3.13.5):

```text
name: widget | count: 3
```

The full `collections` reference (including `deque`, `defaultdict`, `ChainMap`) is Vol XII;
`argparse` in depth (subcommands, custom actions, `sys.argv`) is Vol XV.

---

## 8.4 Performance and anti-patterns

- **Threads for CPU-bound work.** The classic mistake: parallelizing a numeric loop with
  `ThreadPoolExecutor` and seeing *no* speedup (or a slowdown from GIL contention). CPU →
  processes (or the free-threaded build, Vol VII); I/O → threads or `asyncio` (Vol III).
- **Process pools over microtasks.** If a task is faster than its pickling+IPC, the pool loses.
  Batch work into coarse chunks; pass references/handles, not megabytes of data.
- **Missing `if __name__ == "__main__"`** with a process pool under spawn — re-runs module-level
  code in every worker; can fork-bomb. Always guard.
- **Unpicklable pool payloads** — lambdas, closures, local functions, open sockets. Pool only
  top-level functions and picklable arguments.
- **Reaching for `OrderedDict` reflexively.** Since 3.7 a plain `dict` is ordered; only use
  `OrderedDict` for its `==` semantics or `move_to_end`.
- **Tuning `setswitchinterval` blindly.** Lowering it increases hand-off fairness but raises
  switching overhead; it does not add parallelism. Profile before touching it.

---

## 8.5 Summary and cross-references

- The **new GIL (3.2)** switches on a **5 ms time interval** (`sys.getswitchinterval()`) via a
  `gil_drop_request` hand-off, ending the **convoy effect** — but it adds fairness, *not*
  multi-core CPU parallelism.
- **`concurrent.futures`** (PEP 3148) gives one `Executor`/`Future` API over two backends:
  **threads for I/O**, **processes for CPU** (paying a `pickle`+IPC tax). The **spawn** start
  method re-imports your module — guard pool code with `if __name__ == "__main__"`.
- **`OrderedDict`** survives 3.7's ordered dicts for its order-sensitive `==` and `move_to_end`;
  **`Counter`** tallies with `most_common` and multiset arithmetic; **`argparse`** is the standard
  declarative CLI parser.

**Cross-references.** Reference counting (why the GIL exists) → Chapter 2. The GIL model, the
free-threaded build, and per-interpreter GILs → Vol VII. Threads vs. processes vs. asyncio,
shared memory, and structured concurrency → Vol IX. `asyncio` and coroutines → Vol III. The
`collections` reference → Vol XII. `argparse` in depth → Vol XV.

---

*Volume II complete. Volume III opens the iterator, generator, and async-inception story
(3.3–3.5): `yield from`, `asyncio`, and native `async`/`await`.*
