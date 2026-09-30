---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://docs.python.org/3/library/multiprocessing.html, https://docs.python.org/3/library/multiprocessing.shared_memory.html, https://docs.python.org/3/library/concurrent.futures.html, https://docs.python.org/3/library/concurrent.interpreters.html, https://docs.python.org/3/whatsnew/3.14.html, https://docs.python.org/3/whatsnew/3.13.html, https://docs.python.org/3/whatsnew/3.12.html, https://docs.python.org/3/library/os.html#os.process_cpu_count, https://docs.python.org/3/library/gc.html#gc.freeze, https://docs.python.org/3/library/faulthandler.html, https://docs.python.org/3/howto/logging-cookbook.html, https://docs.python.org/3/library/fcntl.html, https://docs.python.org/3/howto/free-threading-python.html, https://peps.python.org/pep-0734/, https://peps.python.org/pep-0703/, https://peps.python.org/pep-0779/, https://peps.python.org/pep-0683/, https://docs.gunicorn.org/en/stable/settings.html, https://numpy.org/doc/stable/reference/random/parallel.html]
---

# Multiprocessing and parallelism

How to get real CPU parallelism out of Python: `multiprocessing`, `ProcessPoolExecutor`, start methods, IPC, shared memory, subinterpreters, and free-threading, with the production failure modes interviewers probe.
MP1-MP6 (why processes, start methods, the main guard, pickling, Pool vs executor), MP8-MP9 (worker errors and dead workers), MP17-MP18 (queues and the join deadlock), MP21 (shared memory), MP26 (free-threading vs processes), and MP31 (processes inside web servers) are near-certain; MP33 (predict the output) is how a concurrency-heavy screen often opens.
This note goes deeper than the short answers in [Python core](02-Python-Core.md) Y9, [FastAPI](04-FastAPI.md) A2 and A19, [Flask](03-Flask.md) F10, and [Docker, Kubernetes, CI/CD, cloud](09-Docker-Kubernetes-CICD-Cloud.md) D8, and pairs with [Concurrency fundamentals and threading](14-Concurrency-Fundamentals-and-Threading.md), [Asyncio deep dive](16-Asyncio-Deep-Dive.md), and [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md).
Every code sample here was run on 2026-09-30 under Python 3.14.7 (GIL build) with NumPy 2.5.3 and FastAPI 0.142.0, plus the free-threaded 3.14.6 build for MP26: 60 pytest tests passed and 5 were skipped (blocks with no printed output, each covered by a dedicated test).
Performance numbers are single-machine numbers from an Apple M3 Pro (12 cores: 6 performance, 6 efficiency; macOS 27), labeled where they appear; the start-method defaults shown in output are the macOS ones.

---

## MP1. Why does multiprocessing exist, what does it cost, and when does it lose to a single process? (must know)

> "In the default CPython build the GIL lets only one thread run Python bytecode at a time, so threads do not speed up CPU-bound pure-Python code.
> `multiprocessing` sidesteps that by running separate interpreter processes, each with its own GIL, so they really run in parallel on different cores.
> The price is process startup, memory for each interpreter, and pickling every argument and result through a pipe.
> So it wins when each task does a lot of CPU work relative to the data it moves, and it loses badly for tiny tasks, where the IPC overhead dwarfs the work.
> The first fix for tiny tasks is batching with `chunksize`; the second is not using processes at all."

| Cost | Where it comes from | Rough size on this machine | How to reduce it |
| --- | --- | --- | --- |
| Startup | `spawn` starts a fresh interpreter and re-imports your main module | 65-80 ms from `Pool(4)` to the first result | start the pool once and reuse it; `forkserver` with preloaded modules on Linux |
| Memory | each worker is a full interpreter plus your imported modules and data | about 21 MB RSS per idle spawned worker (measured with `ps`), before your imports and data | fewer workers, `fork` plus copy-on-write where safe, shared memory for big arrays |
| Per-task IPC | the task and its arguments are pickled, written to a pipe, read and unpickled; same for the result | tens of microseconds per round trip | `chunksize`, fewer and bigger tasks, pass file paths or shared-memory names instead of data |
| Serialization limits | lambdas, closures, locks, sockets, and DB connections do not pickle | - | module-level functions, per-worker `initializer` |

The small-task trap, measured: squaring 20,000 integers, which takes half a millisecond in a list comprehension.

```python
import time
from concurrent.futures import ProcessPoolExecutor
from multiprocessing import Pool


def square(x: int) -> int:
    return x * x


def timed(fn):
    start = time.perf_counter()
    result = fn()
    return result, time.perf_counter() - start


if __name__ == "__main__":
    data = list(range(20_000))
    expected, serial = timed(lambda: [square(x) for x in data])

    start = time.perf_counter()
    with Pool(4) as pool:
        pool.map(square, range(4), chunksize=1)                          # first result: workers are up
        startup = time.perf_counter() - start
        r1, pool_c1 = timed(lambda: pool.map(square, data, chunksize=1))
        r2, pool_auto = timed(lambda: pool.map(square, data))            # Pool.map picks a chunksize
        r3, imap_c1 = timed(lambda: list(pool.imap(square, data)))      # imap defaults to chunksize=1
    with ProcessPoolExecutor(4) as ex:
        r4, ppe_c1 = timed(lambda: list(ex.map(square, data)))          # default chunksize=1
        r5, ppe_big = timed(lambda: list(ex.map(square, data, chunksize=1_000)))
    assert expected == r1 == r2 == r3 == r4 == r5
    assert pool_auto < pool_c1 and ppe_big < ppe_c1

    print(f"serial list comprehension     {serial * 1000:8.1f} ms")
    print(f"Pool(4) start to first result {startup * 1000:8.1f} ms")
    print(f"Pool.map chunksize=1          {pool_c1 * 1000:8.1f} ms")
    print(f"Pool.map default chunksize    {pool_auto * 1000:8.1f} ms")
    print(f"Pool.imap chunksize=1         {imap_c1 * 1000:8.1f} ms")
    print(f"Executor.map chunksize=1      {ppe_c1 * 1000:8.1f} ms")
    print(f"Executor.map chunksize=1000   {ppe_big * 1000:8.1f} ms")
```

One run printed (Python 3.14.7, M3 Pro; the machine was shared with other jobs, so treat the numbers as rough; over five runs, start to first result took 65-80 ms, `Pool.map` with `chunksize=1` 357-569 ms, `Executor.map` with `chunksize=1` 1.5-3.4 s, and the batched versions 1-10 ms):

```text
serial list comprehension          0.5 ms
Pool(4) start to first result     79.6 ms
Pool.map chunksize=1             357.2 ms
Pool.map default chunksize         1.4 ms
Pool.imap chunksize=1            399.4 ms
Executor.map chunksize=1        2000.4 ms
Executor.map chunksize=1000        9.8 ms
```

- `Pool.map` already batches: with no `chunksize` it uses `ceil(len(items) / (4 * workers))`, which is why it is hundreds of times faster than `chunksize=1`.
  It also converts the input to a list first, so it is not lazy.
- `Pool.imap`, `Pool.imap_unordered`, and `ProcessPoolExecutor.map` default to `chunksize=1`: one pickle round trip per item.
  `Executor.map` with `chunksize=1` was the slowest here, thousands of times slower than the plain loop.
- Even the best parallel version is slower than the serial loop, because the work per item is nanoseconds and the IPC is microseconds.
  Processes pay off only when the work per task is large compared to the bytes moved; CPU-heavy tasks in [Python core](02-Python-Core.md) Y9 and MP26 here show the win.

**Follow-ups they ask:**

- "How do you pick `chunksize`?"
  Aim for each chunk to take at least a few milliseconds of CPU; start with `len(items) / (workers * 4)` like `Pool.map` does, then measure.
  Too big and the last worker finishes long after the others (poor load balancing, see MP7).
- "How would you prove processes help?"
  Time serial vs parallel on realistic input sizes, check that CPU usage actually hits N cores, and count bytes pickled per task.
- "Is multiprocessing the only way around the GIL?"
  No: C extensions that release the GIL (NumPy, hashlib, see MP27), subinterpreters (MP25), the free-threaded build (MP26), or moving the work to another service or a queue worker (MP31).

---

## MP2. What are the `fork`, `spawn`, and `forkserver` start methods, and what is the default on each platform in 3.14? (must know)

- **`fork`**: the child is a copy of the parent made by `fork()`: same memory (copy-on-write), same imported modules, same globals; nothing is re-imported and the target does not need to be picklable.
  Fast, but only the calling thread survives in the child, so locks held by other threads stay locked forever (MP3).
- **`spawn`**: the child is a brand-new interpreter that imports your main module as `__mp_main__`, then unpickles the target and arguments.
  Slowest to start, safest, and everything passed must be picklable.
- **`forkserver`**: a single-threaded server process is started once; each new child is forked from that clean server instead of from your (possibly threaded) program.
  Arguments are pickled like `spawn`; startup cost is between the two, and `set_forkserver_preload()` can pre-import heavy modules into the server.

| Platform | Default in 3.14 | History |
| --- | --- | --- |
| Linux and other POSIX except macOS | `forkserver` | was `fork` through 3.13; changed in 3.14 (gh-84559) |
| macOS | `spawn` | changed from `fork` in 3.8 because macOS system libraries are not fork-safe (bpo-33725) |
| Windows | `spawn` | the only method; there is no `fork()` |

Verified in the 3.14.7 source (`multiprocessing/context.py`): the default is `forkserver` when the platform can pass file descriptors and is not `darwin`, otherwise `spawn`.

```python
import multiprocessing as mp
import sys

CONFIG = {"mode": "default"}


def report(q) -> None:
    q.put(CONFIG["mode"])


if __name__ == "__main__":
    print("platform:", sys.platform, "| default start method:", mp.get_start_method())
    CONFIG["mode"] = "set at runtime in the parent"      # after import, only in parent memory
    for method in ("fork", "spawn", "forkserver"):
        ctx = mp.get_context(method)                      # a context object; does not change the global default
        q = ctx.Queue()
        p = ctx.Process(target=report, args=(q,))
        p.start()
        print(f"{method:10s} child sees: {q.get(timeout=10)}")
        p.join()
```

Output (macOS):

```text
platform: darwin | default start method: spawn
fork       child sees: set at runtime in the parent
spawn      child sees: default
forkserver child sees: default
```

- Only the `fork` child sees state the parent created at runtime; `spawn` and `forkserver` children rebuild module state by importing.
- Use `mp.get_context("spawn")` and pass the context around (`ctx.Process`, `ctx.Queue`, `ProcessPoolExecutor(mp_context=ctx)`), rather than `set_start_method()`, which can be called once per program and raises `RuntimeError: context has already been set` the second time (MP6 shows it).
  Libraries should never call `set_start_method()`.

**What changed for Linux users in 3.14:** code that silently relied on `fork` inheritance (a lambda as `target`, a global filled in at runtime, an unpicklable argument) now fails or behaves differently on Linux, exactly as it always did on macOS and Windows.
If you must keep the old behavior temporarily, ask for it explicitly with `mp.get_context("fork")`, and plan to remove it.

**Follow-ups they ask:**

- "Which would you choose on Linux?"
  `forkserver` or `spawn` for anything with threads (every real service has some: logging, DB pools, gRPC, metrics exporters); `fork` only for a single-threaded batch script where copy-on-write sharing of a big read-only structure matters, and even then measure.
- "Why is `spawn` slow?"
  It execs a new interpreter and re-imports your main module and everything it imports; a module that imports pandas and a model at top level pays that per worker.

---

## MP3. Why is `fork` unsafe when the parent has threads?

> "`fork()` copies the whole address space but only the thread that called it.
> If another thread held a lock at that instant, the child gets the lock in the locked state with no thread that will ever release it.
> The next time the child touches that lock it deadlocks: typical victims are the logging module's handler locks, malloc arenas in C libraries, and database driver or SSL state.
> Since 3.12, `os.fork()` in a multi-threaded process emits a `DeprecationWarning`, and 3.14 moved Linux's default off `fork`."

```text
parent                                  child after fork()
thread A: main  --fork()-->             thread A only
thread B: holds lock L                  (thread B does not exist)
          (inside critical section)     lock L: still LOCKED, owner gone
                                        lock.acquire() -> waits forever
```

A deterministic reproduction: a background thread holds a lock at the moment of the fork.

```python
import multiprocessing as mp
import re
import threading
import warnings

lock = threading.Lock()          # stands in for a logging, malloc, or DB-driver lock
holding = threading.Event()
release = threading.Event()


def holder() -> None:
    with lock:                   # a background thread is inside the critical section...
        holding.set()
        release.wait()


def child(q) -> None:
    q.put(lock.acquire(timeout=1.0))   # real code calls acquire() with no timeout: hangs forever


if __name__ == "__main__":
    t = threading.Thread(target=holder)
    t.start()
    holding.wait()                     # ...at the moment we fork

    for method in ("fork", "spawn"):
        ctx = mp.get_context(method)
        q = ctx.Queue()
        with warnings.catch_warnings(record=True) as caught:
            warnings.simplefilter("always")
            p = ctx.Process(target=child, args=(q,))
            p.start()
        print(f"{method}: child acquired the lock: {q.get(timeout=10)}")
        p.join()
        for w in caught:
            msg = re.sub(r"pid=\d+", "pid=N", str(w.message))
            print(f"{method}: {w.category.__name__}: {msg}")

    release.set()
    t.join()
```

Output:

```text
fork: child acquired the lock: False
fork: DeprecationWarning: This process (pid=N) is multi-threaded, use of fork() may lead to deadlocks in the child.
spawn: child acquired the lock: True
```

- The `fork` child cannot take the lock (it would hang forever without the timeout), and Python warns at fork time.
- The `spawn` child re-imported the module and got a fresh, unlocked lock.
- macOS adds a second problem: Apple's system frameworks (Objective-C runtime, CoreFoundation, some networking and keychain code) are not fork-safe, so a forked child can crash with an `objc[...] ... may have been in progress in another thread when fork() was called` error.
  That is why macOS switched to `spawn` in 3.8; setting `OBJC_DISABLE_INITIALIZE_FORK_SAFETY=YES` only hides the symptom.
- Other fork hazards: the child inherits open sockets and DB connections (two processes on one connection corrupts the protocol), unflushed stdout buffers (MP33, snippet 2), and the parent's random-number state (MP33, snippet 6).

**Follow-ups they ask:**

- "Can `os.register_at_fork` fix it?"
  It lets a library reset its own state in the child (the stdlib `random` module reseeds this way), but you cannot fix locks held inside C libraries you do not own.
- "But gunicorn forks workers, is that broken?"
  The master forks before starting application threads, which is the safe pattern; see MP31.

---

## MP4. Why do you need `if __name__ == "__main__":`, and what happens without it? (must know)

- With `spawn` and `forkserver`, each child imports your main script (as `__mp_main__`) to find the target function.
- Any top-level code runs again in every child; if that top-level code starts processes, each child would start more children.
- Python detects this during child bootstrap and raises `RuntimeError: An attempt has been made to start a new process before the current process has finished its bootstrapping phase`.
- The guard makes the process-starting code run only when the file is executed as the main program, not when it is imported by a child.

```python
import multiprocessing as mp


def work(x: int) -> int:
    return x * 2


ctx = mp.get_context("spawn")        # the default on macOS and Windows anyway
with ctx.Pool(2) as pool:            # BUG: no main guard; every child re-runs this on import
    print(pool.map(work, [1, 2, 3]))
```

What happened on 3.14.7 (macOS, `spawn`), with three variants of the same bug:

| Variant | Parent sees | Child sees |
| --- | --- | --- |
| `Process(...).start()` at top level | continues; `p.exitcode == 1` | `RuntimeError: An attempt has been made to start a new process before the current process has finished its bootstrapping phase` |
| `ProcessPoolExecutor` at top level | `BrokenProcessPool: A process in the process pool was terminated abruptly...` | the same `RuntimeError` |
| `Pool` at top level (the code above) | **hangs forever**: the pool keeps replacing workers that die on startup (hundreds of tracebacks in 15 s, killed by a timeout) | the same `RuntimeError`, over and over |

- The error text itself names the fix: the `if __name__ == '__main__':` idiom, with `freeze_support()` only needed for frozen Windows executables.
- Everything else at module top level (reading config, loading a model, opening a DB connection, printing) also runs once per spawned worker; see MP33 snippet 3.
  Keep modules import-safe: definitions at top level, side effects inside `main()`.
- Interactive sessions and notebooks: functions defined in `__main__` of a REPL or notebook cannot be imported by spawned children; put workers in a `.py` module and import them.

---

## MP5. What gets pickled when you submit a task? Why do lambdas and closures fail, and how do you fix it? (must know)

- **The function is pickled by reference**: the module name and qualified name, not the code.
  The child imports that module and looks the name up, so the function must be defined at the top level of an importable module.
- **Arguments and return values are pickled by value**: the child works on a copy, mutations never flow back, and big arguments cost a full serialize-copy-deserialize per task.
- **Lambdas, nested functions, and closures** have qualified names like `<lambda>` or `make_scaler.<locals>.inner` that cannot be looked up, so pickling fails.
- **Bound methods** pickle the whole instance (`self`) by value, per task; a `self` that holds a pool, a lock, or a big DataFrame is either unpicklable or expensive.

```python
import functools
import pickle
import re
from concurrent.futures import ProcessPoolExecutor


def scale(x: int, factor: int) -> int:
    return x * factor


def make_scaler(factor: int):
    def inner(x: int) -> int:            # closure: pickled by qualified name, which is not importable
        return x * factor
    return inner


class Scaler:
    def __init__(self, factor: int) -> None:
        self.factor = factor             # state is pickled by value

    def __call__(self, x: int) -> int:
        return x * self.factor


if __name__ == "__main__":
    print(pickle.dumps(scale))           # a reference: module name plus qualified name, no code
    candidates = {
        "lambda": lambda x: x * 3,
        "closure": make_scaler(3),
        "partial(module function)": functools.partial(scale, factor=3),
        "callable instance": Scaler(3),
    }
    with ProcessPoolExecutor(2) as ex:
        for name, fn in candidates.items():
            try:
                print(f"{name:25s} -> {list(ex.map(fn, [1, 2]))}")
            except Exception as exc:
                msg = re.sub(r" at 0x[0-9a-f]+", "", str(exc))     # drop the memory address
                print(f"{name:25s} -> {type(exc).__name__}: {msg}")
```

Output:

```text
b'\x80\x05\x95\x16\x00\x00\x00\x00\x00\x00\x00\x8c\x08__main__\x94\x8c\x05scale\x94\x93\x94.'
lambda                    -> PicklingError: Can't pickle <function <lambda>>: it's not found as __main__.<lambda>
closure                   -> PicklingError: Can't pickle local object <function make_scaler.<locals>.inner>
partial(module function)  -> [3, 6]
callable instance         -> [3, 6]
```

The pickle of `scale` is just `__main__` and `scale`: that is why the child must be able to import the same code.

| Fix | When |
| --- | --- |
| Move the function to module top level | the default answer |
| `functools.partial(module_func, fixed_arg=...)` | binding extra arguments (partials of top-level functions pickle fine) |
| A small callable class with state in `__init__` | configurable workers; state is pickled by value once per task or chunk |
| `initializer` plus a module global | heavy or unpicklable per-worker resources: DB connections, models (MP10) |
| `cloudpickle` (used by joblib's loky backend, Ray, Dask) | libraries that serialize lambdas by value; not what the stdlib does |

**Things that never pickle:** locks, `mp.Queue` outside inheritance (MP6), open files and DB connections, generators, `Pool` objects (`NotImplementedError: pool objects cannot be passed between processes or pickled`), and custom exceptions with a mismatched `__init__` (MP8).

---

## MP6. `multiprocessing.Pool` or `concurrent.futures.ProcessPoolExecutor`: which do you use and why? (must know)

> "For new code I use `ProcessPoolExecutor`: it has the same `Future` API as `ThreadPoolExecutor`, so I can swap threads and processes, it plugs into asyncio with `run_in_executor`, and it fails loudly with `BrokenProcessPool` when a worker dies.
> I reach for `Pool` when I need `imap_unordered`, `starmap`, or its automatic chunking, and I know its sharp edges: a dead worker or an unpicklable exception can make it hang."

| | `multiprocessing.Pool` | `ProcessPoolExecutor` |
| --- | --- | --- |
| API | `map`, `imap`, `imap_unordered`, `starmap`, `apply_async`, callbacks | `submit` -> `Future`, `map`, `as_completed`, `wait` |
| Default `chunksize` for `map` | computed: `ceil(n / (4 * workers))` | 1 |
| Laziness | `imap*` are lazy; `map` materializes the input | `map` submits everything up front, unless `buffersize=` (3.14) |
| Worker crash (segfault, OOM kill, `os._exit`) | worker is replaced, the task is lost, `map` **hangs** (MP9) | `BrokenProcessPool` on pending futures; the executor is unusable |
| Unpicklable exception from a worker | result thread dies, **hangs** (MP8) | `BrokenProcessPool` |
| Workers | daemonic: a worker cannot start its own pool | not daemonic: nested executors work |
| Recycling | `maxtasksperchild` | `max_tasks_per_child` (3.11+) |
| Force stop | `terminate()` | `terminate_workers()` / `kill_workers()` (3.14), `shutdown(cancel_futures=True)` (3.9+) |
| asyncio | wrap by hand | `loop.run_in_executor(executor, fn, *args)` |
| `with` exit | calls `terminate()`: kills workers, even mid-task | calls `shutdown(wait=True)`: waits for queued and running work |

The `with` difference bites people: `with Pool() as p: p.apply_async(...)` without `get()` can kill the work before it runs, while an executor's `with` blocks until everything submitted has finished.

```python
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor


def use_queue(q) -> None:
    q.put(1)


def nested_pool(_: int) -> str:
    try:
        with mp.Pool(1) as inner:
            return f"inner pool ok: {inner.map(abs, [-1])}"
    except AssertionError as exc:
        return f"AssertionError: {exc}"


def nested_executor(_: int) -> str:
    with ProcessPoolExecutor(1) as inner:
        return f"inner executor ok: {list(inner.map(abs, [-1]))}"


if __name__ == "__main__":
    mp.set_start_method("spawn")
    try:
        mp.set_start_method("fork")
    except RuntimeError as exc:
        print("set_start_method twice:", exc)

    q = mp.Queue()
    with mp.Pool(1) as pool:
        try:
            pool.apply(use_queue, (q,))              # a Queue as a task argument
        except RuntimeError as exc:
            print("Queue as task arg:", exc)
        print("Pool worker starting a Pool ->", pool.apply(nested_pool, (0,)))
    with ProcessPoolExecutor(1) as ex:
        print("Executor worker starting an executor ->", ex.submit(nested_executor, 0).result())
```

Output:

```text
set_start_method twice: context has already been set
Queue as task arg: Queue objects should only be shared between processes through inheritance
Pool worker starting a Pool -> AssertionError: daemonic processes are not allowed to have children
Executor worker starting an executor -> inner executor ok: [1]
```

- A `Queue` must be handed to a process when it is created (a `Process` argument, or a pool's `initargs`), not as a per-task argument; for per-task use, pass a `Manager().Queue()` proxy.
- Nested process pools are usually a design smell anyway: they multiply process count past the CPU count.

---

## MP7. Explain `map`, `imap`, `imap_unordered`, `starmap`, `apply_async`, `submit`, and `as_completed`. How do chunking and load balancing interact?

```python
import itertools
import time
from concurrent.futures import ProcessPoolExecutor, as_completed
from multiprocessing import Pool


def nap(seconds: float) -> float:
    time.sleep(seconds)
    return seconds


def power(base: int, exp: int) -> int:
    return base ** exp


def square(x: int) -> int:
    return x * x


if __name__ == "__main__":
    delays = [0.3, 0.1, 0.2]
    with Pool(3) as pool:
        print("map           ", pool.map(nap, delays))                    # input order, blocks for all
        print("imap          ", list(pool.imap(nap, delays)))             # input order, lazy iterator
        print("imap_unordered", list(pool.imap_unordered(nap, delays)))   # completion order
        print("starmap       ", pool.starmap(power, [(2, 3), (3, 2)]))    # unpacks argument tuples
        pending = pool.apply_async(power, (2, 10))                        # one call -> AsyncResult
        print("apply_async   ", pending.get(timeout=5))

    with ProcessPoolExecutor(3) as ex:
        futures = [ex.submit(nap, d) for d in delays]
        print("as_completed  ", [f.result() for f in as_completed(futures)])
        lazy = ex.map(square, itertools.count(), buffersize=4)            # 3.14: bounded read-ahead
        print("map buffersize", list(itertools.islice(lazy, 5)))           # infinite input is fine
```

Output:

```text
map            [0.3, 0.1, 0.2]
imap           [0.3, 0.1, 0.2]
imap_unordered [0.1, 0.2, 0.3]
starmap        [8, 9]
apply_async    1024
as_completed   [0.1, 0.2, 0.3]
map buffersize [0, 1, 4, 9, 16]
```

| Call | Order of results | Memory | Use when |
| --- | --- | --- | --- |
| `Pool.map` / `Executor.map` | input order | all results held; `Pool.map` also lists the input | you need everything, in order |
| `Pool.imap` | input order, lazily | a slow early item blocks later ones from being yielded | streaming in order |
| `Pool.imap_unordered` / `as_completed` | completion order | stream results as they finish | progress bars, first-come processing |
| `Pool.starmap` | input order | like `map` | functions of several arguments |
| `apply_async` / `submit` | one handle per call | one pending object per call | heterogeneous tasks, callbacks, fine-grained cancel |
| `Executor.map(..., buffersize=n)` (3.14) | input order | at most `n` tasks in flight | huge or infinite inputs |

**Chunking vs load balancing:**

```text
4 workers, 16 tasks, one slow task (S) at index 0
chunksize=8:  w1 [S . . . . . . .]==========>   w2 [. . . . . . . .]   w3 idle   w4 idle
chunksize=1:  w1 [S]==========>                  w2 [.][.][.][.][.]    w3 [.][.][.][.][.]   w4 [.][.][.][.]
```

- Big chunks minimize IPC but can strand a slow chunk on one worker while others idle (and fewer chunks than workers leaves cores unused).
- Small chunks balance load but pay IPC per item (MP1).
- Rules of thumb: several chunks per worker; each chunk at least a few milliseconds; shuffle inputs when cost correlates with position (sorted by size, for example); use `imap_unordered` with a moderate `chunksize` for skewed workloads.

---

## MP8. How do exceptions from workers reach the parent, and what can break that? (must know)

- The worker catches the exception, pickles it, and sends it back; `future.result()` or `pool.map()` re-raises **the same exception type** in the parent.
- The worker's formatted traceback is attached as `__cause__` (a `_RemoteTraceback` holding the text), so the parent's traceback shows both stacks.
- In `pool.map`, the first failing item fails the whole call; results of the other items are lost.
  Catch errors inside the worker and return them as values when you need per-item outcomes.
- **Custom exceptions must survive pickling**: `Exception.__reduce__` rebuilds the object with `cls(*self.args)`, so an `__init__` whose signature does not match `args` fails when the parent unpickles it.

```python
from concurrent.futures import ProcessPoolExecutor
from multiprocessing import Pool


def parse(text: str) -> int:
    return int(text)


class BadApiError(Exception):
    def __init__(self, status: int, body: str) -> None:
        super().__init__(f"{status}: {body}")      # args == ("503: down",): cannot rebuild the object
        self.status = status


class ApiError(Exception):
    def __init__(self, status: int, body: str) -> None:
        super().__init__(status, body)             # args == (503, "down"): pickles round trip
        self.status = status
        self.body = body

    def __str__(self) -> str:
        return f"{self.status}: {self.body}"


def call_bad(_: int) -> None:
    raise BadApiError(503, "down")


def call_good(_: int) -> None:
    raise ApiError(503, "down")


if __name__ == "__main__":
    with ProcessPoolExecutor(2) as ex:
        future = ex.submit(parse, "12x")
        try:
            future.result()
        except ValueError as exc:                  # same type as in the worker, re-raised in the parent
            remote = exc.__cause__                 # the worker's traceback, as text
            print("executor:", repr(exc))
            print("cause:", type(remote).__name__, "| mentions parse():", "in parse" in str(remote))

    with Pool(2) as pool:
        try:
            pool.map(parse, ["1", "2", "x"])       # one bad item fails the whole map
        except ValueError as exc:
            print("pool.map:", repr(exc))

    with ProcessPoolExecutor(2) as ex:
        try:
            ex.submit(call_good, 1).result()
        except ApiError as exc:
            print("good custom error:", exc, exc.status)

    with ProcessPoolExecutor(2) as ex:
        try:
            ex.submit(call_bad, 1).result(timeout=10)
        except Exception as exc:                   # the unpickling TypeError breaks the pool
            print("bad custom error:", type(exc).__name__)
```

Output:

```text
executor: ValueError("invalid literal for int() with base 10: '12x'")
cause: _RemoteTraceback | mentions parse(): True
pool.map: ValueError("invalid literal for int() with base 10: 'x'")
good custom error: 503: down 503
bad custom error: BrokenProcessPool
```

- `BadApiError` passes one formatted string to `super().__init__`, so unpickling calls `BadApiError("503: down")` and fails with a `TypeError` in the parent's result-handling thread.
- `ProcessPoolExecutor` reports it as `BrokenProcessPool` (misleading: no process died), and `Pool` is worse: its result-handler thread dies and the call **never returns** (MP33 snippet 7).
- Fix: pass every constructor argument to `super().__init__(*args)` and override `__str__`, or define `__reduce__`.
  Keep worker exceptions simple, and test them with `pickle.loads(pickle.dumps(exc))`.

---

## MP9. What happens when a worker process dies mid-task? (must know)

> "With `ProcessPoolExecutor` you get `BrokenProcessPool` on the in-flight and pending futures and the executor is dead; you have to create a new one.
> With `multiprocessing.Pool` the dead worker is silently replaced, but the task it was running is lost and never retried, so `map()` waits forever.
> Causes in production are segfaults in C extensions, the kernel OOM killer, `os._exit` or `sys.exit` in library code, and someone running `kill -9`."

```python
import multiprocessing as mp
import os
from concurrent.futures import ProcessPoolExecutor
from concurrent.futures.process import BrokenProcessPool


def work(x: int) -> int:
    if x == 3:
        os._exit(1)            # stands in for a segfault in a C extension or the OOM killer
    return x * 10


if __name__ == "__main__":
    ex = ProcessPoolExecutor(2)
    futures = [ex.submit(work, x) for x in range(6)]
    outcomes = []
    for f in futures:
        try:
            outcomes.append(f.result())
        except BrokenProcessPool:
            outcomes.append("broken")
    print("executor:", outcomes)
    try:
        ex.submit(work, 1)
    except BrokenProcessPool as exc:
        print("submit after death:", type(exc).__name__)
    ex.shutdown()

    with mp.Pool(2) as pool:
        pending = pool.map_async(work, range(6))
        try:
            print(pending.get(timeout=3))
        except mp.TimeoutError:
            print("pool.map: no result after 3s; the lost task is never retried")
```

Output (the same in 5 of 5 runs; tasks that finished before the crash was detected keep their results):

```text
executor: [0, 10, 20, 'broken', 'broken', 'broken']
submit after death: BrokenProcessPool
pool.map: no result after 3s; the lost task is never retried
```

**How to make it robust:**

- Always use timeouts on `Pool` results (`get(timeout=...)`), or prefer `ProcessPoolExecutor`.
- Catch `BrokenProcessPool`, log which items were pending, create a fresh executor, and retry idempotent items with a retry budget; quarantine the item that keeps killing workers (it is a poison pill).
- Find the cause: a negative `exitcode` is the signal number (-9 is `SIGKILL`, often the OOM killer; -11 is `SIGSEGV`), `dmesg` or `kubectl describe pod` shows OOM kills, and `faulthandler` prints the Python stack on a segfault (MP32).
- Bound memory per worker with `max_tasks_per_child` (MP11) and smaller chunks.

---

## MP10. How do you do per-worker setup, like a database connection or an ML model?

- Pass an `initializer` (and `initargs`) to the pool: it runs **once in each worker process** before any task, and typically stores the resource in a module-level global.
- Tasks then use the global, so the unpicklable resource never crosses a process boundary and is not rebuilt per task.
- Pass large read-only inputs (a model path, a lookup table) through `initargs` once per worker instead of as a task argument once per task.

```python
import os
import sqlite3
import tempfile
from concurrent.futures import ProcessPoolExecutor

_conn: sqlite3.Connection | None = None       # one per worker process, never pickled


def init_worker(db_path: str) -> None:
    global _conn
    _conn = sqlite3.connect(db_path)           # opened once per worker, reused by every task


def lookup(key: int) -> tuple[int, int, str]:
    assert _conn is not None
    (name,) = _conn.execute("SELECT name FROM items WHERE id = ?", (key,)).fetchone()
    return os.getpid(), id(_conn), name


if __name__ == "__main__":
    with tempfile.TemporaryDirectory() as tmp:
        db_path = os.path.join(tmp, "items.db")
        with sqlite3.connect(db_path) as conn:
            conn.execute("CREATE TABLE items (id INTEGER PRIMARY KEY, name TEXT)")
            conn.executemany("INSERT INTO items VALUES (?, ?)", [(i, f"item-{i}") for i in range(12)])
        conn.close()

        with ProcessPoolExecutor(2, initializer=init_worker, initargs=(db_path,)) as ex:
            rows = list(ex.map(lookup, range(12)))

    conns_per_pid: dict[int, set[int]] = {}
    for pid, conn_id, _ in rows:
        conns_per_pid.setdefault(pid, set()).add(conn_id)
    print("names:", [name for _, _, name in rows][:3], "...")
    print("one connection per worker:", all(len(c) == 1 for c in conns_per_pid.values()))
```

Output:

```text
names: ['item-0', 'item-1', 'item-2'] ...
one connection per worker: True
```

**What goes wrong:**

- Opening the connection in the parent and passing it to tasks: it does not pickle, and under `fork` the parent and children would share one socket and corrupt the protocol.
- Forgetting cleanup: there is no finalizer hook; rely on process exit, use `max_tasks_per_child` to recycle, or register `atexit` in the initializer (runs on normal worker exit).
- An initializer that raises: on 3.14.7 `ProcessPoolExecutor` failed the first task with `BrokenProcessPool: A process in the process pool was terminated abruptly...`, while `Pool` restarted the failing worker over and over (74 tracebacks in 2 s) and the task never ran.
  Validate configuration in the parent before starting the pool.
- Loading a 2 GB model in each of 16 workers is 32 GB; for big read-only models prefer fewer workers with threads inside (if the model's kernels release the GIL, MP27), a model server, or shared memory for the weights.

---

## MP11. What are `maxtasksperchild` and `max_tasks_per_child` for?

- They make each worker exit after N tasks and be replaced by a fresh process.
- Use them to contain slow memory leaks or fragmentation in C extensions, and to release resources that a library never frees.
- The price is a process start (and your initializer) every N tasks; pick N so that restarts are a small fraction of runtime.

```python
import multiprocessing as mp
import os
from concurrent.futures import ProcessPoolExecutor


def whoami(_: int) -> int:
    return os.getpid()


if __name__ == "__main__":
    with mp.Pool(2, maxtasksperchild=1) as pool:
        pids = pool.map(whoami, range(6), chunksize=1)
    print("Pool maxtasksperchild=1, 6 tasks -> distinct pids:", len(set(pids)))

    with ProcessPoolExecutor(2, max_tasks_per_child=1) as ex:           # 3.11+
        pids = list(ex.map(whoami, range(6)))
    print("Executor max_tasks_per_child=1, 6 tasks -> distinct pids:", len(set(pids)))

    try:
        ProcessPoolExecutor(2, mp_context=mp.get_context("fork"), max_tasks_per_child=1)
    except ValueError as exc:
        print("with fork:", exc)
```

Output:

```text
Pool maxtasksperchild=1, 6 tasks -> distinct pids: 6
Executor max_tasks_per_child=1, 6 tasks -> distinct pids: 6
with fork: max_tasks_per_child is incompatible with the 'fork' multiprocessing start method; supply a different mp_context.
```

- `ProcessPoolExecutor(max_tasks_per_child=...)` (3.11+) refuses the `fork` context, and when no context is given it switches to `spawn` for you (verified in the 3.14.7 source), because replacing workers by forking from a threaded parent is unsafe.
- `Pool`'s `maxtasksperchild` counts tasks, and a chunk counts as one task, so with a big `chunksize` recycling happens far less often than you think.
- Celery's `worker_max_tasks_per_child` and gunicorn's `max_requests` (plus `max_requests_jitter`) are the same idea for queue workers and web workers.

---

## MP12. How many worker processes should you start? `os.cpu_count()` or `os.process_cpu_count()`?

> "For CPU-bound work, about one process per core the job may actually use; more just adds context switching and memory.
> `os.cpu_count()` is the machine's logical CPUs; `os.process_cpu_count()` (3.13+) is the CPUs this process is allowed to run on, which respects CPU affinity, and it is what the pools use by default in 3.13+.
> Neither sees a container's CPU quota, so in Kubernetes I size from the cgroup limit or an environment variable."

```python
import os
from concurrent.futures import ProcessPoolExecutor
from multiprocessing import Pool

if __name__ == "__main__":
    print("os.cpu_count()        :", os.cpu_count())            # logical CPUs in the machine
    print("os.process_cpu_count():", os.process_cpu_count())    # 3.13+: CPUs this process may use
    with ProcessPoolExecutor() as ex:
        print("Executor default      :", ex._max_workers)       # private attribute, demo only
    with Pool() as pool:
        print("Pool default          :", pool._processes)       # private attribute, demo only
```

Output on this machine, then with overrides:

```text
os.cpu_count()        : 12
os.process_cpu_count(): 12
Executor default      : 12
Pool default          : 12
```

```text
$ PYTHON_CPU_COUNT=3 python mp15_cpu_count.py     # 3.13+: overrides both functions
os.cpu_count()        : 3
os.process_cpu_count(): 3
Executor default      : 3
Pool default          : 3
$ python -X cpu_count=2 mp15_cpu_count.py         # same override as a command-line option
os.cpu_count()        : 2
os.process_cpu_count(): 2
Executor default      : 2
Pool default          : 2
```

| Situation | Worker count |
| --- | --- |
| CPU-bound pure Python on a laptop or VM | `os.process_cpu_count()` |
| Container with a CPU limit of 2 on a 64-core node | 2 (read `/sys/fs/cgroup/cpu.max` or set `PYTHON_CPU_COUNT`/an env var); `os.cpu_count()` would say 64 |
| Mixed CPU and I/O per task | somewhat more than cores, measured |
| Each task already multithreaded (NumPy BLAS, PyTorch) | fewer processes, or cap library threads (`OMP_NUM_THREADS=1`) to avoid oversubscription |
| Windows | `ProcessPoolExecutor` caps the default at 61 and rejects `max_workers` above 61 |
| Apple Silicon | performance and efficiency cores differ in speed, so N workers rarely give N x (MP25-MP26 saw about 2.5-3x on 4 workers) |

The cgroup-based sizing helper lives in [Docker, Kubernetes, CI/CD, cloud](09-Docker-Kubernetes-CICD-Cloud.md) D8.

---

## MP13. How do timeouts and cancellation work with a process pool?

- `future.result(timeout=...)` raises `TimeoutError` but **does not stop the task**; it keeps running in the worker.
- `future.cancel()` only works while the future is still pending; once running (or already handed to a worker's call queue) it returns `False`.
- `executor.shutdown(cancel_futures=True)` (3.9+) cancels everything still pending, then waits for the running ones.
- To actually stop running work: `terminate_workers()` or `kill_workers()` (3.14) on an executor, or `pool.terminate()`; the pool is unusable afterwards.
  For per-task hard timeouts, give each task its own `Process` and `terminate()` it, or put the deadline inside the task.

```python
import time
from concurrent.futures import ProcessPoolExecutor, TimeoutError


def nap(seconds: float) -> float:
    time.sleep(seconds)
    return seconds


def wait_until(predicate, timeout: float = 10.0) -> None:
    deadline = time.monotonic() + timeout
    while not predicate():
        if time.monotonic() > deadline:
            raise RuntimeError("condition not reached")
        time.sleep(0.01)


if __name__ == "__main__":
    ex = ProcessPoolExecutor(1)
    first = ex.submit(nap, 1.0)
    rest = [ex.submit(nap, 0.1) for _ in range(4)]
    wait_until(first.running)
    wait_until(rest[0].running)          # the executor pre-queues one extra call and marks it RUNNING

    try:
        first.result(timeout=0.1)
    except TimeoutError:
        print("result(timeout) raised TimeoutError; the task is still running:", first.running())
    print("cancel running task:", first.cancel())
    print("cancel queued tasks:", [f.cancel() for f in rest])
    print("states:", [f._state for f in [first, *rest]])          # private attribute, demo only

    start = time.perf_counter()
    ex.shutdown(wait=True, cancel_futures=True)                     # 3.9+: drop what is still pending
    print(f"shutdown waited ~{round(time.perf_counter() - start)}s for the running task")
```

Output:

```text
result(timeout) raised TimeoutError; the task is still running: True
cancel running task: False
cancel queued tasks: [False, True, True, True]
states: ['RUNNING', 'RUNNING', 'CANCELLED', 'CANCELLED', 'CANCELLED']
shutdown waited ~1s for the running task
```

- The second task is `RUNNING` although the only worker is busy: the executor moves up to `max_workers + 1` calls into its call queue and marks them running (verified in `concurrent/futures/process.py`, `EXTRA_QUEUED_CALLS = 1`), so they can no longer be cancelled.
- `Pool` equivalents: `async_result.get(timeout=...)` raises `multiprocessing.TimeoutError` (the same caveat applies: the task keeps running), and there is no per-task cancel.
- Python threads cannot be killed at all; processes can, which is one real reason to choose processes for untrusted or runaway work.

---

## MP14. Walk through the `Process` API: `start`, `join`, `exitcode`, `terminate`, `kill`, `daemon`, `close`.

```python
import multiprocessing as mp
import subprocess
import sys
import time
from multiprocessing.connection import wait


def ok() -> None:
    pass


def fails() -> None:
    raise ValueError("boom")          # the child prints the traceback to its own stderr


def exits_3() -> None:
    sys.exit(3)


def sleeps(ready) -> None:
    ready.set()
    time.sleep(60)


def state(pid: int) -> str:
    out = subprocess.run(["ps", "-o", "stat=", "-p", str(pid)], capture_output=True, text=True)
    return out.stdout.strip() or "gone"


if __name__ == "__main__":
    for target in (ok, fails, exits_3):
        p = mp.Process(target=target)
        p.start()
        p.join()
        print(f"{target.__name__:8s} exitcode={p.exitcode}")

    for action in ("terminate", "kill", "interrupt"):          # interrupt() is new in 3.14
        ready = mp.Event()
        p = mp.Process(target=sleeps, args=(ready,))
        p.start()
        ready.wait(10)                                          # signal only after the child is running
        getattr(p, action)()
        p.join()
        print(f"{action:9s} exitcode={p.exitcode}")

    p = mp.Process(target=ok)
    p.start()
    wait([p.sentinel])                  # child has exited, but nobody has called waitpid() yet
    print("before join:", state(p.pid)[0])     # Z = zombie: exit status kept for the parent
    p.join()                            # reaps it
    print("after join:", state(p.pid))
    p.close()                           # release the Process object's resources (3.7+)
```

Output (the `fails` and `interrupt` children also print tracebacks to stderr):

```text
ok       exitcode=0
fails    exitcode=1
exits_3  exitcode=3
terminate exitcode=-15
kill      exitcode=-9
interrupt exitcode=1
before join: Z
after join: gone
```

| Member | Meaning |
| --- | --- |
| `start()` | create the OS process (fork, spawn, or forkserver) and run `run()` in it; call once |
| `join(timeout=None)` | wait for exit and reap it; returns `None` even on timeout, so check `is_alive()` or `exitcode` afterwards |
| `exitcode` | `None` while running; `0` success; `1` uncaught exception; `n` for `sys.exit(n)`; `-N` killed by signal N |
| `terminate()` | `SIGTERM` (exit -15); `finally` blocks and `atexit` handlers in the child do **not** run, and descendants are orphaned |
| `kill()` | `SIGKILL` (exit -9, 3.7+); cannot be caught |
| `interrupt()` | `SIGINT` (3.14): raises `KeyboardInterrupt` in the child, so `finally` blocks do run; exit code 1 above |
| `daemon=True` | the parent's normal exit terminates it; a daemonic process cannot start children (MP6) |
| `is_alive()` | polls with `waitpid(WNOHANG)`, so it also reaps a finished child |
| `sentinel` | a handle that becomes ready when the process ends; wait on many with `multiprocessing.connection.wait` |
| `close()` | release the `Process` object's resources (3.7+); most methods then raise `ValueError` |
| `pid`, `name`, `ident` | OS pid, human label (shows in logs), same as pid |

A terminated child holding a lock, or halfway through writing to a `Queue`, can leave it locked or corrupted for everyone else; the docs warn about this explicitly, which is why graceful shutdown (an `Event` or a sentinel message) beats `terminate()`.

---

## MP15. What are zombie and orphan processes, and why does `join()` matter?

- **Zombie**: the child has exited but the parent has not collected its exit status with `waitpid()`; it holds a process-table slot and shows as `Z` in `ps`.
  `join()` (and `is_alive()`, and `multiprocessing.active_children()`) reap it.
  The MP14 output shows the child in state `Z` between exiting and `join()`, and gone afterwards.
- A long-running parent that starts processes and never joins them accumulates zombies until it hits the per-user process limit (`fork: Resource temporarily unavailable`).
- **Orphan**: the parent died first; the child is re-parented to PID 1 (`init`/`systemd` on Linux, `launchd` on macOS) and keeps running, holding ports, files, and locks.
  `daemon=True` does not help when the parent is killed with `SIGKILL`: the cleanup that terminates daemonic children runs only on a normal interpreter exit.

```python
import multiprocessing as mp
import os
import signal
import subprocess
import sys
import time


def child(ready) -> None:
    ready.set()
    if os.environ.get("ORPHAN_DEMO") == "watch":
        parent = mp.parent_process()              # 3.8+: a handle on the parent process
        while parent.is_alive():                  # the fix: exit when the parent is gone
            time.sleep(0.05)
        return
    time.sleep(30)                                # the bug: nothing notices that the parent died


def ppid_of(pid: int) -> str:
    out = subprocess.run(["ps", "-o", "ppid=", "-p", str(pid)], capture_output=True, text=True)
    return out.stdout.strip() or "gone"


if __name__ == "__main__":
    if len(sys.argv) > 1:                         # middle process: start a child, report, then die hard
        ready = mp.Event()
        p = mp.Process(target=child, args=(ready,), daemon=True)
        p.start()
        ready.wait(10)
        print(p.pid, flush=True)
        os.kill(os.getpid(), signal.SIGKILL)      # no atexit handlers, no daemon cleanup

    for mode in ("orphan", "watch"):
        env = dict(os.environ, ORPHAN_DEMO=mode)
        middle = subprocess.Popen([sys.executable, __file__, "middle"], env=env,
                                  stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)
        pid = int(middle.stdout.readline())       # not communicate(): the orphan keeps the pipe open
        middle.wait()
        middle.stdout.close()
        time.sleep(0.5)
        print(f"{mode}: middle exit={middle.returncode}, child ppid now = {ppid_of(pid)}")
        if ppid_of(pid) != "gone":
            os.kill(pid, signal.SIGKILL)          # clean up the orphan ourselves
```

Output:

```text
orphan: middle exit=-9, child ppid now = 1
watch: middle exit=-9, child ppid now = gone
```

- A child can watch its parent with `multiprocessing.parent_process()` (3.8+) and exit when `is_alive()` turns false; on Linux, `prctl(PR_SET_PDEATHSIG)` (through a C extension or `ctypes`) asks the kernel to signal the child when the parent dies.
- In containers, PID 1 is often your Python app, which does not reap re-parented grandchildren; run under `tini` (`docker run --init`) so zombies get reaped.
- When the parent was killed, its `resource_tracker` also printed `leaked semaphore objects to clean up at shutdown` for the queues and events it had created (visible on stderr when running this demo).

---

## MP16. How do you handle Ctrl-C and `SIGTERM` with a process pool?

> "A terminal delivers Ctrl-C to the whole foreground process group, so every worker gets `SIGINT` too.
> Without a plan, each worker dies with its own `KeyboardInterrupt` traceback, `Pool` replaces them, and `with ProcessPoolExecutor()` blocks on exit until all queued work finishes.
> So I make workers ignore `SIGINT` in the initializer, let the parent catch `KeyboardInterrupt` or handle `SIGTERM`, and have the parent stop the pool explicitly."

```python
import multiprocessing as mp
import os
import signal
import time
from concurrent.futures import ProcessPoolExecutor


def init_worker(barrier) -> None:
    signal.signal(signal.SIGINT, signal.SIG_IGN)   # the parent owns shutdown; workers ignore Ctrl-C
    barrier.wait()                                 # demo only: make sure every worker is ready


def slow(x: int) -> int:
    time.sleep(30)
    return x


def ctrl_c_like_a_terminal() -> None:
    # A terminal sends SIGINT to the whole foreground process group: the parent and every worker.
    for pid in [c.pid for c in mp.active_children()] + [os.getpid()]:
        os.kill(pid, signal.SIGINT)
    time.sleep(1)                                  # the KeyboardInterrupt lands in the parent here


if __name__ == "__main__":
    barrier = mp.Barrier(3)
    pool = mp.Pool(2, initializer=init_worker, initargs=(barrier,))
    barrier.wait()
    start = time.perf_counter()
    try:
        pending = pool.map_async(slow, range(4))
        ctrl_c_like_a_terminal()
        pending.get(timeout=60)
    except KeyboardInterrupt:
        pool.terminate()                           # do not wait 30 s for the running tasks
        pool.join()
        print(f"Pool: stopped in {time.perf_counter() - start:.0f}s")

    barrier = mp.Barrier(3)
    ex = ProcessPoolExecutor(2, initializer=init_worker, initargs=(barrier,))
    futures = [ex.submit(slow, i) for i in range(4)]
    barrier.wait()
    start = time.perf_counter()
    try:
        ctrl_c_like_a_terminal()
        [f.result() for f in futures]
    except KeyboardInterrupt:
        ex.terminate_workers()                     # 3.14; shutdown() alone would wait for running tasks
        ex.shutdown(cancel_futures=True)
        print(f"Executor: stopped in {time.perf_counter() - start:.0f}s")
```

Output:

```text
Pool: stopped in 0s
Executor: stopped in 0s
```

- `pool.terminate()` and `ex.terminate_workers()` stop running tasks immediately; without them, a variant of this script that relied on the executor's `with` block took 2.2 s for four 1-second tasks on two workers after the `KeyboardInterrupt`, because `shutdown(wait=True)` also ran every queued task.
- For a graceful stop (finish the current task, then exit), send workers a stop message or set a shared `Event` they check between units of work; reserve `terminate()` for the timeout path.
- `SIGTERM` (what Kubernetes and systemd send) does not raise anything by default; install a handler in the parent (`signal.signal(signal.SIGTERM, ...)`) that triggers the same shutdown path, and give it less time than the orchestrator's grace period.

---

## MP17. Compare `Queue`, `SimpleQueue`, `JoinableQueue`, and `Pipe`. (must know)

| | `Pipe()` | `SimpleQueue` | `Queue` | `JoinableQueue` | `Manager().Queue()` |
| --- | --- | --- | --- | --- | --- |
| Endpoints | exactly two (duplex by default) | many producers, many consumers | many to many | many to many | many to many, any process that has the proxy |
| Mechanism | one OS pipe or socket pair | pipe plus locks, `put` blocks while writing | pipe plus a **background feeder thread** in each producer | `Queue` plus task counting | proxy to a `queue.Queue` in a manager server process |
| `put` blocks? | `send` blocks if the pipe buffer is full | yes, while writing | no, buffered in memory (unless `maxsize` reached) | no | a round trip to the manager |
| Extras | `poll(timeout)`, `send_bytes` | minimal | `maxsize`, `put/get` timeouts, `qsize` (not on macOS), `close`, `join_thread`, `cancel_join_thread` | `task_done()`, `join()` | picklable: can be passed as a task argument |
| Pitfalls | two processes reading the same end at once corrupts data | no timeouts | join-before-drain deadlock (MP18), `empty()` lies right after `put` (MP33) | forgetting `task_done` hangs `join()` | slowest (MP19) |

```python
import multiprocessing as mp
import time

N = 50_000


def pipe_echo(conn) -> None:
    while (msg := conn.recv()) is not None:
        conn.send(msg.upper())                    # duplex: the same end sends and receives
    conn.close()


def consume(q) -> None:
    while True:
        item = q.get()
        if item is None:
            q.task_done()
            return
        q.task_done()                             # JoinableQueue: one task_done per get


def drain(q, ready) -> None:
    ready.set()
    for _ in range(N):
        q.get()


def throughput(make_queue) -> float:
    q, ready = make_queue(), mp.Event()
    p = mp.Process(target=drain, args=(q, ready))
    p.start()
    ready.wait()                                  # exclude process startup from the timing
    start = time.perf_counter()
    for i in range(N):
        q.put(i)
    p.join()
    return N / (time.perf_counter() - start)


def pipe_drain(conn, ready) -> None:
    ready.set()
    for _ in range(N):
        conn.recv()


def pipe_throughput() -> float:
    reader, writer = mp.Pipe(duplex=False)        # (receive end, send end)
    ready = mp.Event()
    p = mp.Process(target=pipe_drain, args=(reader, ready))
    p.start()
    ready.wait()
    start = time.perf_counter()
    for i in range(N):
        writer.send(i)
    p.join()
    return N / (time.perf_counter() - start)


if __name__ == "__main__":
    parent, child = mp.Pipe()
    p = mp.Process(target=pipe_echo, args=(child,))
    p.start()
    parent.send("ping")
    print("pipe reply:", parent.recv())
    parent.send(None)
    p.join()

    jq = mp.JoinableQueue()
    c = mp.Process(target=consume, args=(jq,))
    c.start()
    for i in range(100):
        jq.put(i)
    jq.put(None)
    jq.join()                                     # blocks until every item is marked done
    c.join()
    print("joinable queue: all items processed")

    for name, rate in [("Queue", throughput(mp.Queue)),
                       ("SimpleQueue", throughput(mp.SimpleQueue)),
                       ("Pipe", pipe_throughput())]:
        print(f"{name:12s} {rate / 1000:6.0f}k small messages/s")
```

One run printed (small integers, 50,000 messages, child already started; Python 3.14.7, M3 Pro; over five runs, Queue 155-257k/s, SimpleQueue 182-295k/s, Pipe 267-445k/s):

```text
pipe reply: PING
joinable queue: all items processed
Queue           195k small messages/s
SimpleQueue     266k small messages/s
Pipe            331k small messages/s
```

- All of these pickle every message; throughput for large payloads is bounded by pickling and memory copies, not by the queue type (MP21 measures an 80 MB array).
- A `Queue` producer's `put()` returns before the data is in the pipe; the feeder thread writes it later, which is the root of both MP18 and the `empty()` surprise.

---

## MP18. Why can joining a process that has put items on a `Queue` deadlock? (must know)

> "A process that has put items on a `multiprocessing.Queue` will not terminate until its feeder thread has flushed everything into the underlying pipe.
> If the item is bigger than the OS pipe buffer and nobody is reading, the child blocks at exit, and a parent that calls `join()` before `get()` waits for the child forever.
> The rule from the docs: drain the queue before joining the processes that fed it."

```text
parent                              child
p.start()                           q.put(1 MB)   -> feeder thread fills the pipe buffer, blocks
p.join()   ---- waits for child --> run() returns; exit waits for feeder thread to flush
   (never returns)                     (never flushes: nobody reads)
```

```python
import multiprocessing as mp


def produce(q) -> None:
    q.put(b"x" * 1_000_000)      # bigger than the OS pipe buffer
    # run() returns, but the process cannot exit until its feeder thread has flushed the item


if __name__ == "__main__":
    q = mp.Queue()
    p = mp.Process(target=produce, args=(q,))
    p.start()

    p.join(timeout=2)            # WRONG ORDER: without a timeout this line never returns
    print("after join(timeout=2): child alive =", p.is_alive(), "exitcode =", p.exitcode)

    data = q.get()               # drain first...
    p.join()                     # ...then join
    print("after draining:", len(data), "bytes, exitcode =", p.exitcode)
```

Output:

```text
after join(timeout=2): child alive = True exitcode = None
after draining: 1000000 bytes, exitcode = 0
```

- Fixes: `get()` everything first and then `join()`; or use a sentinel so the consumer knows when to stop reading; or keep reading with timeouts while polling `is_alive()`.
- `q.cancel_join_thread()` in the child makes it exit without flushing, which avoids the hang by **losing data**; only use it when the data does not matter.
- The same shape appears with `Pool` and executors when a result is huge; stream big results through files or shared memory and send back only a path or name.

---

## MP19. What is a `Manager`, and why are proxies slow?

- `multiprocessing.Manager()` starts a **server process** that owns real Python objects (`dict`, `list`, `Queue`, `Lock`, `Namespace`, ...) and hands out **proxies**.
- Every method call on a proxy is a pickled request over a socket or pipe to the server, a method call there, and a pickled reply: one IPC round trip per operation.
- Proxies are picklable, so unlike `mp.Queue` they can be passed as task arguments, and they work across machines (`BaseManager` with an address and authkey).

```python
import multiprocessing as mp
import time


def add_items(shared, lock, n: int) -> None:
    for _ in range(n):
        with lock:                               # proxy ops are atomic one by one, not as a sequence
            shared["count"] = shared["count"] + 1


def append_nested(shared) -> None:
    shared["tags"].append("from-child")          # mutates a COPY returned by the proxy: lost
    tags = shared["tags"]
    tags.append("reassigned")
    shared["tags"] = tags                        # reassigning through the proxy works


if __name__ == "__main__":
    with mp.Manager() as manager:                # starts a server process that owns the objects
        shared = manager.dict(count=0, tags=[])
        lock = manager.Lock()
        workers = [mp.Process(target=add_items, args=(shared, lock, 500)) for _ in range(4)]
        for w in workers:
            w.start()
        for w in workers:
            w.join()
        print("count:", shared["count"])

        p = mp.Process(target=append_nested, args=(shared,))
        p.start()
        p.join()
        print("tags:", shared["tags"])

        n = 20_000
        start = time.perf_counter()
        for i in range(n):
            shared["k"] = i
        proxy_rate = n / (time.perf_counter() - start)
    local, start = {}, time.perf_counter()
    for i in range(n):
        local["k"] = i
    local_rate = n / (time.perf_counter() - start)
    print(f"proxy dict writes: {proxy_rate / 1000:.0f}k/s, local dict writes: {local_rate / 1e6:.0f}M/s")
```

One run printed (Python 3.14.7, M3 Pro; over five runs, 43-73k proxy writes/s vs 27-31M local writes/s):

```text
count: 2000
tags: ['reassigned']
proxy dict writes: 52k/s, local dict writes: 28M/s
```

- A proxy write is several hundred times slower than a local dict write here; use a Manager for low-rate coordination, not in a hot loop.
- Each proxy operation is atomic on its own, but `shared["count"] = shared["count"] + 1` is two round trips, so it needs a lock (the `count: 2000` line held because of it).
- `shared["tags"].append(x)` changes a **copy** returned by the proxy and is silently lost; read, modify, and assign back, or store nested manager proxies.
- The manager process is one more thing that can die; if it does, every later proxy call fails with a connection error instead of returning data.

---

## MP20. How do `Value` and `Array` work, and why is `counter.value += 1` still a race?

- `mp.Value("i", 0)` and `mp.Array("d", n)` allocate a C type in shared memory that every process maps; no pickling after creation, very fast reads and writes.
- By default they come wrapped with an `RLock` (`lock=True`), but **each** `.value` read and write takes the lock separately, so `+=` (read, add, write) can interleave between processes and lose updates.
- Hold `counter.get_lock()` across the read-modify-write, or use `lock=False` (`RawValue`, `RawArray`) when processes write disjoint slots.

```python
import multiprocessing as mp

N = 50_000


def unsafe(counter) -> None:
    for _ in range(N):
        counter.value += 1               # a locked read, then a locked write: two steps, not one


def safe(counter) -> None:
    for _ in range(N):
        with counter.get_lock():         # hold the Value's own lock across read-modify-write
            counter.value += 1


def fill(arr, start: int, stop: int) -> None:
    for i in range(start, stop):
        arr[i] = i * i                   # disjoint slices: no lock needed


def run(target, counter) -> int:
    procs = [mp.Process(target=target, args=(counter,)) for _ in range(4)]
    for p in procs:
        p.start()
    for p in procs:
        p.join()
    return counter.value


if __name__ == "__main__":
    lost = run(unsafe, mp.Value("i", 0))
    print("unsafe: lost updates:", lost < 4 * N)
    print("safe:", run(safe, mp.Value("i", 0)))

    arr = mp.Array("d", 8, lock=False)   # raw shared ctypes array of 8 doubles
    halves = [mp.Process(target=fill, args=(arr, 0, 4)), mp.Process(target=fill, args=(arr, 4, 8))]
    for p in halves:
        p.start()
    for p in halves:
        p.join()
    print("array:", list(arr))
```

Output:

```text
unsafe: lost updates: True
safe: 200000
array: [0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 36.0, 49.0]
```

In five direct runs of the unsafe version, the 4 x 50,000 increments ended at 60,653 to 66,853 instead of 200,000: about two thirds of the updates were lost, because four processes on four cores really do interleave.

---

## MP21. How do you share a large NumPy array between processes without copying it? (must know)

> "I put the array in `multiprocessing.shared_memory.SharedMemory`, wrap the buffer in `np.ndarray(shape, dtype, buffer=shm.buf)` on each side, and send only the segment's name, shape, and dtype through the queue.
> Every process then reads the same physical pages, so there is no pickling and no copy per task.
> The owner creates it and is the only one that calls `unlink()`; everyone calls `close()` when done.
> Concurrent writers still need a lock or disjoint slices; shared memory gives you sharing, not synchronization."

```text
            SharedMemory segment "psm_ab12" (80 MB, one physical copy)
            +---------------------------------------------------+
parent  --> | np.ndarray view (writes data)                      |
            +---------------------------------------------------+
               ^ only the name "psm_ab12" goes through the Queue
child   --> np.ndarray view over SharedMemory(name="psm_ab12")
```

```python
import multiprocessing as mp
import time
from multiprocessing import shared_memory

import numpy as np

SHAPE, DTYPE = (10_000_000,), np.float64          # 80 MB


def sum_from_queue(ready, q_in, q_out) -> None:
    ready.set()
    arr = q_in.get()                              # unpickles a full 80 MB copy
    q_out.put(float(arr.sum()))


def sum_from_shm(ready, q_in, q_out) -> None:
    ready.set()
    shm = shared_memory.SharedMemory(name=q_in.get())   # attach by name: no copy
    try:
        arr = np.ndarray(SHAPE, dtype=DTYPE, buffer=shm.buf)
        q_out.put(float(arr.sum()))
        del arr                                   # drop the view first: using it after close() segfaults
    finally:
        shm.close()                               # unmap in this process; the owner unlinks


def round_trip(target, payload) -> tuple[float, float]:
    ready, q_in, q_out = mp.Event(), mp.Queue(), mp.Queue()
    p = mp.Process(target=target, args=(ready, q_in, q_out))
    p.start()
    ready.wait()                                  # exclude process startup from the timing
    start = time.perf_counter()
    q_in.put(payload)
    total = q_out.get()
    elapsed = time.perf_counter() - start
    p.join()
    return total, elapsed


if __name__ == "__main__":
    data = np.arange(SHAPE[0], dtype=DTYPE)
    q_total, q_time = round_trip(sum_from_queue, data)

    shm = shared_memory.SharedMemory(create=True, size=data.nbytes)
    try:
        view = np.ndarray(data.shape, dtype=data.dtype, buffer=shm.buf)
        view[:] = data                            # producers normally write here directly
        s_total, s_time = round_trip(sum_from_shm, shm.name)   # only the name crosses the pipe
        del view
    finally:
        shm.close()
        shm.unlink()                              # exactly one owner unlinks, or the segment leaks
    assert q_total == s_total == float(data.sum())
    print(f"80 MB array to a child and back: Queue {q_time * 1000:.0f} ms, shared memory {s_time * 1000:.0f} ms")

    items = shared_memory.ShareableList(["alpha", 42, 3.5, None])  # fixed length and slot sizes
    try:
        items[1] += 1
        print("ShareableList:", list(items))
    finally:
        items.shm.close()
        items.shm.unlink()
```

One run printed (Python 3.14.7, NumPy 2.5.3, M3 Pro; over six runs, Queue 34-72 ms, shared memory 10-13 ms):

```text
80 MB array to a child and back: Queue 57 ms, shared memory 10 ms
ShareableList: ['alpha', 43, 3.5, None]
```

- The shared-memory time is almost all the child's `sum()` over 80 MB plus first-touch page mapping; the Queue path adds pickling, a full pipe transfer, and unpickling into a new 80 MB allocation.
  The gap grows with size and with the number of workers, because each worker would otherwise need its own copy.
- `del arr` before `shm.close()` matters: on 3.14.7 with NumPy 2.5.3, `close()` succeeded silently while an ndarray still viewed the buffer, and the next read through that ndarray killed the process with `SIGSEGV` (exit status 139 in 3 of 3 runs, snippet below).
  No exception, no traceback: a hard crash in the worker, which the parent sees as `BrokenProcessPool` or a negative `exitcode`.

```python
from multiprocessing import shared_memory

import numpy as np

if __name__ == "__main__":
    shm = shared_memory.SharedMemory(create=True, size=8 * 1024 * 1024)
    arr = np.ndarray((1024 * 1024,), dtype=np.float64, buffer=shm.buf)
    shm.unlink()
    shm.close()                  # BUG: no error, although arr still points into the mapping
    print("closed without error", flush=True)
    print(arr.sum())             # reads unmapped memory
```

Output (the shell then reports exit status 139, 128 + `SIGSEGV`):

```text
closed without error
```

- `ShareableList` stores a fixed-length list of `int`, `float`, `bool`, `str`, `bytes`, and `None`; slot sizes are fixed at creation, so it cannot grow and a longer string cannot replace a shorter one.
- `SharedMemoryManager` (in `multiprocessing.managers`) creates segments and unlinks them all when its `with` block exits, which is the easiest way to avoid leaks.
- Alternatives: `np.memmap` over a file (the OS page cache is shared, and it survives restarts), Arrow/Plasma-style object stores (Ray), and joblib's automatic memmapping of large arrays.

---

## MP22. What goes wrong with shared-memory lifecycle: `close`, `unlink`, and the `resource_tracker`?

- `close()` unmaps the segment in this process; `unlink()` asks the OS to delete it once everyone has closed it.
  POSIX shared memory outlives processes: a segment that nobody unlinks stays until reboot (on Linux you can see it in `/dev/shm`).
- Python's `resource_tracker` process records segments (and semaphores) that were created, and at shutdown unlinks the ones still registered, with a warning.
- The trap: on POSIX, **attaching** by name also registers the segment with the attaching process's tracker by default.
  If an unrelated process (with its own tracker) attaches, closes, and exits, its tracker deletes the segment out from under the owner.
  3.13 added `SharedMemory(..., track=False)` to opt out.

```python
import subprocess
import sys
import time
from multiprocessing import shared_memory

ATTACH = """
import sys
from multiprocessing import shared_memory
shm = shared_memory.SharedMemory(name=sys.argv[1], track=sys.argv[2] == "True")
shm.close()
"""


def exists(name: str) -> bool:
    try:
        shared_memory.SharedMemory(name=name, track=False).close()
        return True
    except FileNotFoundError:
        return False


if __name__ == "__main__":
    for track in (True, False):
        shm = shared_memory.SharedMemory(create=True, size=1024)
        # an unrelated process (its own resource tracker) attaches, closes, and exits
        subprocess.run([sys.executable, "-c", ATTACH, shm.name, str(track)], capture_output=True)
        print(f"reader track={track}: segment still exists = {exists(shm.name)}")
        shm.close()
        try:
            shm.unlink()
        except FileNotFoundError:
            print("  owner's unlink(): FileNotFoundError, the reader's tracker already removed it")
```

Output (stdout; the owner's own tracker then also warns on stderr that it cannot find the segment it still had registered):

```text
reader track=True: segment still exists = False
  owner's unlink(): FileNotFoundError, the reader's tracker already removed it
reader track=False: segment still exists = True
```

Forgetting `unlink()` entirely:

```python
from multiprocessing import shared_memory

if __name__ == "__main__":
    shm = shared_memory.SharedMemory(create=True, size=1024)
    shm.buf[0] = 1
    shm.close()                  # BUG: close() without unlink(): the segment outlives the process
```

What the tracker printed on stderr, after the program exited (the name is random):

```text
.../multiprocessing/resource_tracker.py:475: UserWarning: resource_tracker: There appear to be 1 leaked shared_memory objects to clean up at shutdown: {'/psm_1a3b9ee0'}
```

- Children started from the same parent share the parent's tracker, so attaching in a pool worker is fine; the problem is independent programs (two services, a CLI and a daemon).
- The same warning with `leaked semaphore objects` means a process died (or was killed) without cleaning up queues, locks, or events; after a `SIGKILL` it is expected noise, after a normal exit it is a bug.

---

## MP23. Which synchronization primitives work across processes?

`multiprocessing` mirrors `threading`: `Lock`, `RLock`, `Semaphore`, `BoundedSemaphore`, `Event`, `Condition`, and `Barrier`, implemented with OS semaphores so they work across processes on one machine.
They must be passed at process creation (a `Process` argument or pool `initargs`); for pool tasks, use the `Manager()` versions.

```python
import multiprocessing as mp
import time


def limited(start, sem, active, peak) -> None:
    start.wait()                                  # Event: every process begins together
    with sem:                                     # Semaphore(2): at most two inside at once
        with active.get_lock():
            active.value += 1
            peak.value = max(peak.value, active.value)
        time.sleep(0.1)
        with active.get_lock():
            active.value -= 1


def phased(barrier, log, lock) -> None:
    for phase in range(2):
        with lock:                                # Lock: serialize appends to the shared log
            log.append(phase)
        barrier.wait()                            # Barrier: nobody starts phase 1 before all finish 0


if __name__ == "__main__":
    start, sem = mp.Event(), mp.Semaphore(2)
    active, peak = mp.Value("i", 0), mp.Value("i", 0)
    procs = [mp.Process(target=limited, args=(start, sem, active, peak)) for _ in range(6)]
    for p in procs:
        p.start()
    t0 = time.perf_counter()
    start.set()
    for p in procs:
        p.join()
    elapsed = time.perf_counter() - t0
    print(f"peak concurrency: {peak.value}; 6 x 0.1s tasks took >= 0.3s: {elapsed >= 0.3}")

    with mp.Manager() as manager:
        log, lock, barrier = manager.list(), mp.Lock(), mp.Barrier(3)
        procs = [mp.Process(target=phased, args=(barrier, log, lock)) for _ in range(3)]
        for p in procs:
            p.start()
        for p in procs:
            p.join()
        print("phase log:", list(log))
```

Output:

```text
peak concurrency: 2; 6 x 0.1s tasks took >= 0.3s: True
phase log: [0, 0, 0, 1, 1, 1]
```

| Primitive | Cross-process use |
| --- | --- |
| `Lock` / `RLock` | protect a shared file, `Value`, or shared-memory region; `RLock` can be re-acquired by its owner |
| `Semaphore(n)` | cap concurrency on a scarce resource: DB connections, GPU slots, a rate-limited API |
| `Event` | start gun, shutdown flag, "ready" signal |
| `Condition` | wait for a predicate on shared state, with `notify` |
| `Barrier(n)` | phase-by-phase algorithms: nobody starts phase k+1 until all finish phase k |

- They cost a system call per operation where contended; a process that dies while holding a `Lock` leaves it locked for everyone (there is no owner-death recovery), which is one more reason to avoid `terminate()`.
- All of these are per machine; see MP24 for independent programs and multiple hosts.

---

## MP24. How do you lock across independent processes, or across hosts?

- **Independent programs on one host** (two cron jobs, a CLI and a daemon): an advisory file lock with `fcntl.flock(fd, LOCK_EX)`.
  The kernel releases it when the file is closed or the process dies, so a crash cannot leave it stuck.
- `flock` locks belong to the open file description: two separate `open()` calls conflict even in one process, and a forked child shares its parent's lock.
- `fcntl.lockf` (POSIX record locks) is per process and is released when the process closes **any** descriptor for that file, a classic surprise; prefer `flock` for whole-file mutual exclusion.
- Advisory means only cooperating processes honor it, and network file systems (NFS) make both flavors unreliable.

```python
import fcntl
import os
import subprocess
import sys
import tempfile
from contextlib import contextmanager


@contextmanager
def file_lock(path: str, blocking: bool = True):
    fd = os.open(path, os.O_RDWR | os.O_CREAT, 0o644)
    try:
        flags = fcntl.LOCK_EX | (0 if blocking else fcntl.LOCK_NB)
        fcntl.flock(fd, flags)            # released on unlock, on close, or when the process dies
        yield
    finally:
        os.close(fd)


def increment(counter_path: str, lock_path: str, times: int) -> None:
    for _ in range(times):
        with file_lock(lock_path):
            with open(counter_path) as f:
                value = int(f.read() or 0)
            with open(counter_path, "w") as f:
                f.write(str(value + 1))


if __name__ == "__main__":
    if len(sys.argv) == 4:                # child mode: an independent program, not a multiprocessing child
        increment(sys.argv[1], sys.argv[2], int(sys.argv[3]))
        sys.exit(0)

    with tempfile.TemporaryDirectory() as tmp:
        counter, lock = os.path.join(tmp, "counter"), os.path.join(tmp, "counter.lock")
        open(counter, "w").close()
        procs = [subprocess.Popen([sys.executable, __file__, counter, lock, "200"]) for _ in range(4)]
        for p in procs:
            p.wait()
        print("counter:", open(counter).read())

        with file_lock(lock):
            try:
                with file_lock(lock, blocking=False):   # a second open() conflicts, even in one process
                    print("acquired twice?")
            except BlockingIOError:
                print("non-blocking attempt while held: BlockingIOError")
```

Output:

```text
counter: 800
non-blocking attempt while held: BlockingIOError
```

**Across hosts**, OS locks do not exist; use the shared system:

- PostgreSQL: `SELECT ... FOR UPDATE`, `SELECT ... FOR UPDATE SKIP LOCKED` for job queues, or `pg_advisory_lock(key)` / `pg_try_advisory_xact_lock(key)`.
- Redis: `SET key token NX PX ttl` with a random token and a compare-and-delete release script; add a fencing token when correctness matters, because a paused holder can outlive its TTL.
- Better still, design the race away: unique constraints, idempotency keys, optimistic concurrency with a version column.

Details and code: [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md) and [SQL and SQLAlchemy](06-SQL-and-SQLAlchemy.md).

---

## MP25. What are subinterpreters in 3.14, and when would you use `InterpreterPoolExecutor`?

> "PEP 734 in 3.14 added `concurrent.interpreters` and `concurrent.futures.InterpreterPoolExecutor`.
> Each subinterpreter has its own GIL, its own modules, and its own globals, inside one OS process, so pure-Python CPU work runs in parallel on threads without the cost of separate processes.
> Sharing is deliberately limited: you pass shareable values or use interpreter queues, and anything else is pickled.
> I would consider it for CPU-bound pure-Python tasks where process startup or memory hurts, as long as every extension module I import supports subinterpreters."

Verified in 3.14.7: `concurrent.interpreters` exports `create`, `get_current`, `get_main`, `list_all`, `Interpreter` (with `exec`, `call`, `call_in_thread`, `prepare_main`, `close`), `create_queue`, `Queue`, `is_shareable`, `ExecutionFailed`, and `NotShareableError`.

```python
import math
import time
from concurrent import interpreters                      # 3.14, PEP 734
from concurrent.futures import (InterpreterPoolExecutor, ProcessPoolExecutor,
                                ThreadPoolExecutor)


def cpu_work(n: int) -> int:
    return sum(i * i for i in range(n))


def timed(fn) -> float:
    start = time.perf_counter()
    fn()
    return time.perf_counter() - start


if __name__ == "__main__":
    interp = interpreters.create()                       # own GIL, own modules, own globals
    interp.prepare_main(greeting="hello")                # copy shareable values into its __main__
    interp.exec("answer = 6 * 7; print(greeting, answer)")
    print("call:", interp.call(math.factorial, 10))      # runs in the subinterpreter, returns the result
    try:
        interp.exec("raise ValueError('inside')")
    except interpreters.ExecutionFailed as exc:          # the original exception does not cross over
        print("ExecutionFailed:", exc.excinfo.type.__name__, exc.excinfo.msg)
    print("shareable:", interpreters.is_shareable(b"bytes"), interpreters.is_shareable([1, 2]))
    interp.close()

    jobs = [3_000_000] * 4
    serial = timed(lambda: [cpu_work(n) for n in jobs])
    for name, cls in [("threads", ThreadPoolExecutor),
                      ("interpreters", InterpreterPoolExecutor),
                      ("processes", ProcessPoolExecutor)]:
        with cls(4) as ex:
            ex.submit(cpu_work, 1).result()              # warm up one worker
            elapsed = timed(lambda: list(ex.map(cpu_work, jobs)))
        print(f"{name:12s} speedup vs serial: {serial / elapsed:.1f}x")
```

One run printed (Python 3.14.7, M3 Pro; over five runs, threads 1.0-1.1x, interpreters 2.5-2.9x, processes 2.0-2.2x; with 8 M-iteration jobs instead of 3 M, processes caught up at 2.7x vs 2.9x):

```text
hello 42
call: 3628800
ExecutionFailed: ValueError inside
shareable: True False
threads      speedup vs serial: 1.0x
interpreters speedup vs serial: 2.5x
processes    speedup vs serial: 2.0x
```

| | Threads (GIL build) | Subinterpreters | Processes |
| --- | --- | --- | --- |
| CPU parallelism for pure Python | no | yes (one GIL each) | yes |
| Startup | microseconds | a fresh interpreter: imports run again, but no new process | a new process plus imports |
| Memory | shared | one process, separate module state per interpreter | separate address spaces |
| Sharing | everything (and every race) | shareable types (`bytes`, `str`, `int`, `float`, `bool`, `None`, tuples of those, interpreter queues, `memoryview`); everything else pickled | pickled messages, shared memory |
| Isolation of a crash | none | none: a segfault kills the whole process | full |
| Extension modules | all | only those that declare multi-interpreter support; others raise `ImportError` (NumPy 2.5.3 did, below) | all |
| Maturity | old | new in 3.14 | old |

What is shareable, and what happens with an extension module and an exception:

```python
from concurrent import interpreters
from concurrent.futures import InterpreterPoolExecutor

def fail() -> None:
    raise KeyError("missing")


if __name__ == "__main__":
    values = [b"x", "s", 1, 1.5, True, None, (1, "a"), memoryview(b"ab"), interpreters.create_queue(),
              [1], {"a": 1}, {1}, bytearray(b"x")]
    print({type(v).__name__: interpreters.is_shareable(v) for v in values})

    interp = interpreters.create()
    try:
        interp.exec("import numpy")
    except interpreters.ExecutionFailed as exc:
        print("numpy in a subinterpreter:", exc.excinfo.type.__name__)
    else:
        print("numpy in a subinterpreter: imported")
    interp.close()

    with InterpreterPoolExecutor(1) as ex:
        try:
            ex.submit(fail).result()
        except KeyError as exc:
            print("InterpreterPoolExecutor re-raised:", type(exc).__name__)
```

Output:

```text
{'bytes': True, 'str': True, 'int': True, 'float': True, 'bool': True, 'NoneType': True, 'tuple': True, 'memoryview': True, 'Queue': True, 'list': False, 'dict': False, 'set': False, 'bytearray': False}
numpy in a subinterpreter: ImportError
InterpreterPoolExecutor re-raised: KeyError
```

- Exceptions do not cross over as themselves when you use `Interpreter.exec`: you get `ExecutionFailed` with a snapshot (`excinfo.type`, `excinfo.msg`); `InterpreterPoolExecutor` re-raises the original exception type (`KeyError` above).
- `interpreters.is_shareable([1, 2])` is `False`: a list is mutable and would need locking across GILs, so it is copied (pickled) instead.
- Good fit: CPU-heavy pure-Python functions with small inputs and outputs.
  Poor fit: code that needs NumPy or other extensions not yet ported, huge shared object graphs, or crash isolation.

---

## MP26. Could the free-threaded build replace processes for CPU-bound work? (must know)

> "The free-threaded build (`python3.14t`, PEP 703, officially supported but still optional in 3.14 per PEP 779) removes the GIL, so plain threads run Python bytecode in parallel.
> For CPU-bound work that means threads can match processes without pickling, without startup cost, and with shared memory by default.
> The costs are a single-threaded slowdown the 3.14 docs put at roughly 5-10%, extension modules that must be rebuilt for it (an unported extension re-enables the GIL at import with a warning), and every data race that the GIL used to hide now being real."

The same CPU-bound job (4 x 5 M loop iterations) on the default build and the free-threaded build:

```python
import sys
import sysconfig
import time
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor


def cpu_work(n: int) -> int:
    total = 0
    for i in range(n):
        total += i * i
    return total


def best_of(fn, repeats: int = 3) -> float:
    times = []
    for _ in range(repeats):
        start = time.perf_counter()
        fn()
        times.append(time.perf_counter() - start)
    return min(times)


if __name__ == "__main__":
    build = "free-threaded" if sysconfig.get_config_var("Py_GIL_DISABLED") else "default"
    print(f"Python {sys.version.split()[0]} ({build} build), GIL enabled: {sys._is_gil_enabled()}")
    jobs = [5_000_000] * 4
    serial = best_of(lambda: [cpu_work(n) for n in jobs])
    with ThreadPoolExecutor(4) as tp, ProcessPoolExecutor(4) as pp:
        list(pp.map(cpu_work, [1] * 8))                       # start the worker processes first
        threads = best_of(lambda: list(tp.map(cpu_work, jobs)))
        procs = best_of(lambda: list(pp.map(cpu_work, jobs)))
    print(f"serial {serial:.2f}s | 4 threads {threads:.2f}s ({serial / threads:.1f}x)"
          f" | 4 processes {procs:.2f}s ({serial / procs:.1f}x)")
```

Runs printed (M3 Pro; `python3.14t` is 3.14.6, the only free-threaded 3.14 build installed; the table summarizes nine runs of each, these three included):

```text
$ python3.14 (3.14.7, GIL) mp25_free_threading.py
Python 3.14.7 (default build), GIL enabled: True
serial 0.57s | 4 threads 0.58s (1.0x) | 4 processes 0.18s (3.1x)
$ python3.14 (3.14.7, GIL) mp25_free_threading.py
Python 3.14.7 (default build), GIL enabled: True
serial 0.57s | 4 threads 0.56s (1.0x) | 4 processes 0.20s (2.9x)
$ python3.14 (3.14.7, GIL) mp25_free_threading.py
Python 3.14.7 (default build), GIL enabled: True
serial 0.59s | 4 threads 0.59s (1.0x) | 4 processes 0.20s (2.9x)
$ python3.14t (3.14.6, free-threaded) mp25_free_threading.py
Python 3.14.6 (free-threaded build), GIL enabled: False
serial 0.58s | 4 threads 0.20s (2.9x) | 4 processes 0.17s (3.4x)
$ python3.14t (3.14.6, free-threaded) mp25_free_threading.py
Python 3.14.6 (free-threaded build), GIL enabled: False
serial 0.54s | 4 threads 0.19s (2.9x) | 4 processes 0.18s (3.1x)
$ python3.14t (3.14.6, free-threaded) mp25_free_threading.py
Python 3.14.6 (free-threaded build), GIL enabled: False
serial 0.55s | 4 threads 0.19s (3.0x) | 4 processes 0.19s (2.8x)
```

| Build | 4 threads | 4 processes |
| --- | --- | --- |
| 3.14.7 default (GIL) | 0.9-1.0x | 2.4-3.1x |
| 3.14.6 free-threaded | 2.3-3.5x | 2.6-3.5x |

- On the free-threaded build, threads matched processes for this job, with no pickling and no worker startup; processes still win when tasks need isolation or when a library is not free-threading safe.
- Check at runtime: `sys._is_gil_enabled()` (3.13+) and `sysconfig.get_config_var("Py_GIL_DISABLED")`; force the GIL off with `PYTHON_GIL=0` or `-X gil=0`.
- Shared mutable state is now truly concurrent: built-in `dict` and `list` operations stay internally consistent (per-object locks), but compound operations like `d[k] = d.get(k, 0) + 1` race exactly as in any language; see [Concurrency fundamentals and threading](14-Concurrency-Fundamentals-and-Threading.md).
- Production status in 2026: check every C extension in the dependency tree for free-threading wheels before betting a service on it; [fill in: whether your current or past stack (trading services, ML platform) could run on it, and what would block it].

---

## MP27. When are threads enough for CPU-heavy work?

- When the heavy part runs in C code that **releases the GIL**: NumPy (most array kernels, sorting, BLAS calls), `hashlib` and `zlib` on large buffers, many image, compression, and crypto libraries, and database drivers while they wait.
- Your own compiled code can release it too: Cython `with nogil:` blocks, Numba `@njit(nogil=True)` (and `parallel=True` with `prange` for built-in multithreading), Rust via PyO3's `allow_threads`, C++ via pybind11's `gil_scoped_release`.
- Then threads give parallel speedup with shared memory and none of the pickling, which is usually the best trade on the default build.

```python
import hashlib
import time
from concurrent.futures import ThreadPoolExecutor

import numpy as np

BLOBS = [bytes([i]) * 64_000_000 for i in range(4)]                  # 4 x 64 MB
ARRAYS = [np.random.default_rng(i).random(4_000_000) for i in range(4)]


def sha(blob: bytes) -> str:
    return hashlib.sha256(blob).hexdigest()       # C code releases the GIL for large buffers


def byte_sum(blob: bytes) -> int:
    return sum(blob[::64])                        # C builtins, but they never release the GIL


def np_sort(arr: np.ndarray) -> float:
    return float(np.sort(arr)[-1])                # NumPy releases the GIL inside the sort kernel


def speedup(fn, inputs) -> float:
    best_serial = best_threaded = float("inf")
    with ThreadPoolExecutor(4) as ex:
        for _ in range(3):
            start = time.perf_counter()
            serial = [fn(x) for x in inputs]
            best_serial = min(best_serial, time.perf_counter() - start)
            start = time.perf_counter()
            threaded = list(ex.map(fn, inputs))
            best_threaded = min(best_threaded, time.perf_counter() - start)
            assert serial == threaded
    return best_serial / best_threaded


if __name__ == "__main__":
    print(f"hashlib.sha256, 4 threads: {speedup(sha, BLOBS):.1f}x")
    print(f"numpy sort,     4 threads: {speedup(np_sort, ARRAYS):.1f}x")
    print(f"sum() of bytes, 4 threads: {speedup(byte_sum, BLOBS):.1f}x")
```

One run printed (best of 3; Python 3.14.7, NumPy 2.5.3, M3 Pro; over five runs, 3.3-3.5x for both GIL-releasing cases and 0.9-1.0x for the GIL-holding one):

```text
hashlib.sha256, 4 threads: 3.5x
numpy sort,     4 threads: 3.4x
sum() of bytes, 4 threads: 0.9x
```

- `sum(blob[::64])` runs entirely in C builtins but never releases the GIL, so four threads give nothing: "it is written in C" is not the test, "does it release the GIL" is.
- Watch for oversubscription: NumPy's BLAS and PyTorch already use a thread pool per process, so 8 processes x 8 BLAS threads on 8 cores thrashes; set `OMP_NUM_THREADS`, `OPENBLAS_NUM_THREADS`, or `MKL_NUM_THREADS` per worker.
- **GPU**: the parallelism is inside the device kernels, launched from one Python thread; use processes only to drive multiple GPUs (one process per GPU), and use `spawn` because CUDA cannot be re-initialized in a forked child.

---

## MP28. Implement a map-reduce over a large file with a process pool.

- **Split** the file into byte ranges, several per worker for load balancing.
- **Map**: each worker opens the file itself (pass the path, not the data), seeks to its range, and processes every line that **starts** inside its range; a line straddling a boundary belongs to the chunk where it starts.
- **Reduce** the partial results in the parent (or in a tree, if partial results are big).

```python
import os
import random
import tempfile
import time
from collections import Counter
from concurrent.futures import ProcessPoolExecutor
from functools import reduce


def chunk_ranges(path: str, n_chunks: int) -> list[tuple[int, int]]:
    size = os.path.getsize(path)
    step = -(-size // n_chunks)                           # ceiling division
    return [(start, min(start + step, size)) for start in range(0, size, step)]


def count_chunk(path: str, start: int, end: int) -> Counter:
    """Map: count words in every line that STARTS inside [start, end)."""
    counts: Counter = Counter()
    with open(path, "rb") as f:
        if start:
            f.seek(start - 1)
            f.readline()                                  # skip the line owned by the previous chunk
        while f.tell() < end:
            line = f.readline()
            if not line:
                break
            counts.update(line.split())
    return counts


def word_count(path: str, workers: int) -> Counter:
    ranges = chunk_ranges(path, workers * 4)              # a few chunks per worker to balance load
    with ProcessPoolExecutor(workers) as ex:
        partials = ex.map(count_chunk, [path] * len(ranges), *zip(*ranges))
        return reduce(lambda a, b: a + b, partials, Counter())   # reduce in the parent


def make_file(path: str, lines: int, seed: int = 7) -> None:
    rng = random.Random(seed)
    vocab = [f"w{i}".encode() for i in range(1000)]
    with open(path, "wb") as f:
        for _ in range(lines):
            f.write(b" ".join(rng.choices(vocab, k=rng.randint(1, 20))) + b"\n")


if __name__ == "__main__":
    with tempfile.TemporaryDirectory() as tmp:
        path = os.path.join(tmp, "words.txt")
        make_file(path, 400_000)
        start = time.perf_counter()
        expected = count_chunk(path, 0, os.path.getsize(path))
        serial = time.perf_counter() - start
        start = time.perf_counter()
        parallel = word_count(path, workers=4)
        elapsed = time.perf_counter() - start
        assert parallel == expected
        size_mb = os.path.getsize(path) / 1e6
        print(f"{size_mb:.0f} MB: serial {serial:.2f}s, 4 processes {elapsed:.2f}s"
              f" ({serial / elapsed:.1f}x, including pool startup)")
```

One run printed (Python 3.14.7, M3 Pro; over five runs, 1.6-2.0x):

```text
21 MB: serial 0.53s, 4 processes 0.33s (1.6x, including pool startup)
```

- The boundary rule: at a nonzero `start`, seek to `start - 1` and discard through the next newline; if the byte before `start` is a newline, that discards nothing, so a line beginning exactly at `start` is kept.
  The tests check the parallel result against the serial one for 1 to 17 chunks on random line lengths.
- Only paths and offsets go to workers and only small `Counter`s come back, so IPC stays tiny; the speedup is capped by process startup, the serial reduce, and disk or page-cache bandwidth.
- For a real job, the same shape scales out with Dask bags, Spark, or a cloud batch service; [fill in: a batch job you parallelized, such as risk or feature computation, the speedup, and the bottleneck you hit].

---

## MP29. Build a multi-stage pipeline with queues between processes.

```text
 [producer] --raw(maxsize=100)--> [transform x3] --parsed(maxsize=100)--> [aggregator] --> result
   one STOP per transform worker        each worker forwards one STOP     stops after 3 STOPs
```

```python
import multiprocessing as mp

STOP = None                                   # sentinel: picklable and unambiguous


def produce(out_q, n: int, n_consumers: int) -> None:
    for i in range(n):
        out_q.put(f"{i},{i * 2}")             # blocks when the queue is full: backpressure
    for _ in range(n_consumers):
        out_q.put(STOP)                       # one sentinel per downstream worker


def transform(in_q, out_q) -> None:
    while (line := in_q.get()) is not STOP:
        a, b = map(int, line.split(","))
        out_q.put(a + b)
    out_q.put(STOP)                           # tell the next stage this worker is done


def aggregate(in_q, result_q, n_upstream: int) -> None:
    total, finished = 0, 0
    while finished < n_upstream:              # stop only after every upstream worker has finished
        item = in_q.get()
        if item is STOP:
            finished += 1
        else:
            total += item
    result_q.put(total)


if __name__ == "__main__":
    n, workers = 10_000, 3
    raw, parsed, result = mp.Queue(maxsize=100), mp.Queue(maxsize=100), mp.Queue()
    stages = [mp.Process(target=produce, args=(raw, n, workers))]
    stages += [mp.Process(target=transform, args=(raw, parsed)) for _ in range(workers)]
    stages += [mp.Process(target=aggregate, args=(parsed, result, workers))]
    for p in stages:
        p.start()
    total = result.get(timeout=30)            # drain before join
    for p in stages:
        p.join()
    print("total:", total, "| expected:", sum(3 * i for i in range(n)))
    print("exit codes:", [p.exitcode for p in stages])
```

Output:

```text
total: 149985000 | expected: 149985000
exit codes: [0, 0, 0, 0, 0]
```

- **Backpressure**: bounded queues (`maxsize`) make a fast producer block instead of buffering the whole input in memory.
- **Shutdown protocol**: one sentinel per consumer, forwarded downstream, and the aggregator counts sentinels from every upstream worker; a single sentinel would stop only one of three workers.
- Use `None` or another value that compares equal after pickling as the sentinel; `object()` identity does not survive pickling (MP33 snippet 4).
- Drain the final queue before joining (MP18), and put a timeout on the final `get()` so a crashed stage fails the run instead of hanging it.
- Failure handling: if a stage can crash, the parent should watch `exitcode`s (or the stage `sentinel`s with `multiprocessing.connection.wait`) and tear the pipeline down, because the other stages will otherwise wait forever.

---

## MP30. What would you use beyond the standard library, or beyond one machine?

| Tool | What it is | Reach for it when |
| --- | --- | --- |
| joblib | `Parallel(n_jobs=-1)(delayed(f)(x) for x in xs)` over the loky process backend (cloudpickle, reusable workers, automatic memmapping of large NumPy arrays) | scikit-learn-style batch work on one machine |
| Ray | distributed tasks (`@ray.remote`), stateful actors, a shared-memory object store with zero-copy NumPy reads | ML pipelines, hyperparameter search, many machines, GPUs |
| Dask | parallel NumPy-like arrays, pandas-like dataframes, and task graphs, with a distributed scheduler | data larger than memory, pandas code that needs to scale out |
| Celery (or RQ, Dramatiq, Arq) | a task queue: web app enqueues, worker processes (prefork pool by default) consume via RabbitMQ or Redis | background jobs from a web service, retries, scheduling |
| Spark / cloud batch | cluster data processing | terabyte-scale ETL |
| Numba, Cython, Rust | compile the hot loop and release the GIL | a single hot function dominates |
| CuPy, PyTorch, RAPIDS | GPU arrays and dataframes | massively data-parallel numeric work |

- Interview framing: first make the single-process version fast (vectorize, better algorithm), then use all cores on one machine, and only then distribute, because distribution adds serialization, failure modes, and operational cost.
- Celery's own pitfalls (acks, retries, visibility timeouts, idempotency) are in [Microservices and messaging](08-Microservices-and-Messaging.md) M17.

---

## MP31. How does multiprocessing fit into a Flask or FastAPI service? (must know)

> "The web server already gives me processes: gunicorn or uvicorn run several worker processes, and each one handles requests with threads or an event loop.
> I never create a process pool per request: that pays process startup on every call and can fork-bomb the box under load.
> For short CPU-bound work inside a FastAPI app, I create one `ProcessPoolExecutor` per server process in the lifespan and call it with `run_in_executor`, so the event loop stays free.
> For anything long, I put a job on a queue and let separate worker processes (Celery, RQ, or a Kafka consumer) do it, and return 202 with a job id."

```python
import asyncio
from concurrent.futures import ProcessPoolExecutor
from contextlib import asynccontextmanager

from fastapi import FastAPI, Request


def fib_digits(n: int) -> int:                   # CPU-bound; module level so workers can import it
    a, b = 0, 1
    for _ in range(n):
        a, b = b, a + b
    return len(str(a))


@asynccontextmanager
async def lifespan(app: FastAPI):
    app.state.cpu_pool = ProcessPoolExecutor(max_workers=2)   # once per server process, not per request
    try:
        yield
    finally:
        app.state.cpu_pool.shutdown(wait=True, cancel_futures=True)


app = FastAPI(lifespan=lifespan)


@app.get("/fib/{n}")
async def fib(n: int, request: Request) -> dict:
    if n > 50_000:
        return {"error": "too large; submit it as a background job instead"}
    loop = asyncio.get_running_loop()
    digits = await loop.run_in_executor(request.app.state.cpu_pool, fib_digits, n)
    return {"n": n, "digits": digits}           # the event loop kept serving while a worker computed
```

Tested with FastAPI's `TestClient` (`/fib/1000` returns 209 digits, and the oversized request is rejected).

- The executor is created once per server worker process, inside `lifespan`, and shut down on exit; a gunicorn master with 4 workers and 2 pool processes each runs 12 processes, so count them against the CPU limit.
- `run_in_executor` keeps the event loop responsive; calling `fib_digits` directly inside `async def` would block every request on that worker ([FastAPI](04-FastAPI.md) A2, [Asyncio deep dive](16-Asyncio-Deep-Dive.md) AS13).
- In Flask (sync workers), the request thread would simply wait on `future.result(timeout=...)`; the pool still saves the per-request process startup and keeps concurrency bounded.

**Copy-on-write, `preload_app`, and why refcounting defeats it:**

- With `preload_app = True`, gunicorn imports the app once in the master, then forks workers, so large read-only data (config, lookup tables, models) starts out shared through copy-on-write pages.
- CPython writes into objects just by reading them: every reference taken or dropped updates `ob_refcnt` in the object header, and a GC pass writes its bookkeeping into tracked objects too.
  Each write copies a whole 4 KB (Linux) or 16 KB (Apple Silicon) page into the worker, so "shared" memory slowly becomes private in every worker.
- 3.12's immortal objects (PEP 683) stop refcount writes only for `None`, `True`, small ints, interned strings, and similar; your dicts and lists are still written.
- `gc.freeze()` (3.7+) moves every object the master has created into a permanent generation that the collector ignores, so GC passes in the workers do not touch them; the `gc` docs recommend `gc.disable()` early in the parent, `gc.freeze()` right before forking, and `gc.enable()` in the children.
- It does not stop refcount writes; large numeric data belongs in NumPy arrays (one object, one refcount, a big untouched buffer) or shared memory, not in millions of small Python objects.
- Preloading is also a fork-safety question: do not open DB pools, start threads, or create gRPC or Kafka clients at import time in the master; create them per worker in `post_fork` or the app's lifespan (MP3).

```python
# gunicorn.conf.py - a Python file that gunicorn executes; hooks receive (server, worker)
import gc
import multiprocessing

bind = "0.0.0.0:8000"
workers = multiprocessing.cpu_count()        # size to the container's CPU limit instead in Kubernetes
preload_app = True                           # import the app once in the master, then fork workers

gc.disable()                                 # avoid GC churn (and freed holes) while the app loads


def pre_fork(server, worker):
    gc.freeze()                              # move every tracked object to the permanent generation


def post_fork(server, worker):
    gc.enable()                              # each worker collects only objects it creates itself
```

- Tested by calling the hooks: after `pre_fork` the freeze count is positive, and `post_fork` re-enables the collector.
  How much memory this saves depends on the app; measure worker unique memory (Linux `smem`, the USS/PSS columns) before and after, since RSS double-counts shared pages.
  [fill in: whether you have tuned gunicorn or uvicorn worker memory in a real service, and the before/after numbers]

**Follow-ups they ask:**

- "Why not just use threads for CPU work in the web process?"
  Under the GIL they would compete with request handling on the same core budget; a separate pool or queue worker isolates it, and on the free-threaded build the answer changes (MP26).
- "Where do the concurrency models of gunicorn, uvicorn, and Celery differ?"
  See [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md) and [Flask](03-Flask.md) F10.

---

## MP32. How do you debug multiprocessing problems?

**Logging from children**: loggers are per process, and several processes writing to one file interleave and can corrupt lines.
Send records to the parent through a queue and let one listener write them.

```python
import logging
import logging.handlers
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor


def init_worker(log_queue) -> None:
    root = logging.getLogger()
    root.handlers[:] = [logging.handlers.QueueHandler(log_queue)]   # ship records to the parent
    root.setLevel(logging.INFO)


def work(x: int) -> int:
    logging.getLogger("worker").info("processing %d", x)
    return x * x


if __name__ == "__main__":
    log_queue = mp.Queue()                    # fine here: handed over at worker start via initargs
    handler = logging.StreamHandler()
    handler.setFormatter(logging.Formatter("%(processName)s %(name)s: %(message)s"))
    listener = logging.handlers.QueueListener(log_queue, handler)
    listener.start()                          # one thread in the parent owns the real handlers
    try:
        with ProcessPoolExecutor(2, initializer=init_worker, initargs=(log_queue,)) as ex:
            print(list(ex.map(work, range(3))))
    finally:
        listener.stop()                       # flushes what is left in the queue
```

One run printed (which worker handled which item varies from run to run):

```text
[0, 1, 4]
SpawnProcess-2 worker: processing 0
SpawnProcess-2 worker: processing 1
SpawnProcess-2 worker: processing 2
```

**Crashes and hangs**: `faulthandler` prints the Python stack of a process on a fatal signal, or on a timer for hangs.

```python
import ctypes
import faulthandler
import multiprocessing as mp
import time


def crash() -> None:
    faulthandler.enable()                     # print the Python stack on SIGSEGV, SIGABRT, ...
    ctypes.string_at(0)                       # stands in for a buggy C extension


def hang() -> None:
    faulthandler.dump_traceback_later(0.5, exit=True)   # watchdog: dump every thread's stack, then exit
    time.sleep(60)                            # stands in for a deadlock


if __name__ == "__main__":
    for target in (crash, hang):
        p = mp.Process(target=target)
        p.start()
        p.join(10)
        print(f"{target.__name__}: exitcode={p.exitcode}")
```

Output on stdout (stderr also held the dumps: `Fatal Python error: Segmentation fault` with the Python stack ending in `crash`, plus a C stack trace, which 3.14's faulthandler adds; then `Timeout (0:00:00.500000)!` with the stack ending in `hang`):

```text
crash: exitcode=-11
hang: exitcode=1
```

| Symptom | First checks |
| --- | --- |
| Hangs with no output | a `join()` before draining a queue (MP18); a `Pool` whose worker died (MP9) or whose exception could not be unpickled (MP8); a fork after threads (MP3); `py-spy dump --pid <pid>` on each process; `faulthandler.dump_traceback_later` |
| `BrokenProcessPool` | a negative `exitcode` is a signal: -9 OOM killer or `kill -9`, -11 segfault; check `dmesg`, container OOMKilled status, `faulthandler` output |
| Exception but no useful traceback | read `exc.__cause__` (the remote traceback text); log inside the worker with `logger.exception` |
| `PicklingError`, or an `AttributeError` about a missing attribute on `__mp_main__` | the function or class is not importable by the child: move it to module level (MP5) |
| `RuntimeError: ... bootstrapping phase` | missing `if __name__ == "__main__":` (MP4) |
| Processes left after the app exits | orphans from a killed parent (MP15); `ps -o pid,ppid,stat,command`, `pstree`; use `parent_process()` watchdogs and `tini` in containers |
| `resource_tracker: ... leaked semaphore` or `shared_memory` warnings | a process died without cleanup, or `unlink()` was forgotten (MP22) |
| Works on Linux (3.13 and earlier), fails on macOS or 3.14 Linux | code relied on `fork` inheritance: now under `spawn` or `forkserver` (MP2) |

- Reproduce in one process first: run the worker function directly in the parent (or with a `ThreadPoolExecutor`) to get a normal traceback and a debugger; then add processes back.
- `PYTHONFAULTHANDLER=1` in the environment enables `faulthandler` in every child, because spawned children inherit the environment.
- `multiprocessing.log_to_stderr(logging.DEBUG)` shows the library's own view: process starts, joins, and finalizers.

---

## MP33. Predict the output: what goes wrong? (must know)

Each snippet was run as a script under 3.14.7 on macOS (default start method `spawn` unless the snippet asks for `fork`); the answers are the observed output.

**Snippet 1: a global mutated in the child**

```python
import multiprocessing as mp

counter = 0


def bump() -> None:
    global counter
    counter += 100
    print("child sees", counter)


if __name__ == "__main__":
    counter = 1
    p = mp.Process(target=bump)
    p.start()
    p.join()
    print("parent sees", counter)
```

Output:

```text
child sees 100
parent sees 1
```

The child has its own copy of the module (re-imported under `spawn`, so `counter` starts at 0, not the parent's runtime value 1), and nothing flows back.
Use a return value, a `Queue`, a `Value`, or shared memory.

**Snippet 2: output printed twice**

```python
import os
import sys

if __name__ == "__main__":
    print("starting job")          # stdout is a pipe or file here, so this sits in a buffer
    pid = os.fork()
    if pid == 0:
        print("child")
        sys.exit(0)                # exiting flushes the inherited buffer too
    os.waitpid(pid, 0)
    print("parent")
```

Output when stdout is a pipe or a file (as in `python p2.py | cat`, cron, or a container log):

```text
starting job
child
starting job
parent
```

`print` buffered "starting job" (stdout is block-buffered when it is not a terminal), `fork()` copied the buffer, and both processes flushed it.
In a terminal (line-buffered) it prints once, which is why this shows up only in production logs.
`multiprocessing`'s own fork path flushes `sys.stdout` and `sys.stderr` before forking; raw `os.fork()` does not, so flush first or use `print(..., flush=True)`.

**Snippet 3: top-level side effects in every worker**

```python
import multiprocessing as mp

print("loading model...")          # module top level: runs in the parent AND in every spawned worker


def predict(x: int) -> int:
    return x + 1


if __name__ == "__main__":
    pool = mp.Pool(2)
    print(pool.map(predict, [1, 2, 3]))
    pool.close()                   # let both workers finish starting and exit normally
    pool.join()
```

Output:

```text
loading model...
loading model...
loading model...
[2, 3, 4]
```

Three times: once in the parent and once in each spawned worker, which re-imports the main module.
If "loading model" takes 20 s and 2 GB, every worker pays it; do it in an `initializer` (MP10) or behind the main guard.
(With `with mp.Pool(2) as pool:` instead of `close()`/`join()`, a test run printed it only twice: the `with` exit calls `terminate()`, which killed the second worker before it finished importing.)

**Snippet 4: a sentinel that is never recognized**

```python
import multiprocessing as mp

STOP = object()                    # BUG: identity does not survive pickling


def worker(q, out) -> None:
    item = q.get()
    out.put(item is STOP)


if __name__ == "__main__":
    q, out = mp.Queue(), mp.Queue()
    p = mp.Process(target=worker, args=(q, out))
    p.start()
    q.put(STOP)
    print("worker recognised STOP:", out.get(timeout=10))
    p.join()
```

Output:

```text
worker recognised STOP: False
```

Pickling makes a new object on the other side, so `is` never matches; in a real consumer loop (`while (item := q.get()) is not STOP`) the worker never stops.
Use `None`, a string, or an enum member (enums unpickle to the same member).

**Snippet 5: `qsize()` and `empty()`**

```python
import multiprocessing as mp

if __name__ == "__main__":
    q = mp.Queue()
    q.put(1)
    try:
        print("qsize:", q.qsize())
    except NotImplementedError:
        print("qsize: NotImplementedError")
    print("empty:", q.empty())     # may be True right after put: the feeder thread has not flushed yet
```

Output (macOS; `empty()` was `True` in 20 of 20 runs):

```text
qsize: NotImplementedError
empty: True
```

`qsize()` is not implemented on macOS (it relies on `sem_getvalue`), and `empty()` right after `put()` is `True` because the feeder thread has not written the item yet.
Never use `empty()` or `qsize()` for control flow across processes; use sentinels and blocking `get(timeout=...)`.

**Snippet 6: "Random" numbers in forked workers**

```python
import multiprocessing as mp
import random

import numpy as np


def draw(q) -> None:
    q.put((np.random.randint(1_000_000), random.randint(0, 1_000_000)))


if __name__ == "__main__":
    np.random.randint(10)          # the parent touched the global RNG (for example, during startup)
    random.randint(0, 10)
    for method in ("fork", "spawn"):
        ctx = mp.get_context(method)
        q = ctx.Queue()
        procs = [ctx.Process(target=draw, args=(q,)) for _ in range(3)]
        for p in procs:
            p.start()
        draws = [q.get(timeout=10) for _ in procs]
        for p in procs:
            p.join()
        same_np = len({d[0] for d in draws}) == 1
        same_py = len({d[1] for d in draws}) == 1
        print(f"{method}: numpy draws identical={same_np}, random draws identical={same_py}")
```

Output:

```text
fork: numpy draws identical=True, random draws identical=False
spawn: numpy draws identical=False, random draws identical=False
```

With `fork`, the children inherit NumPy's global RNG state from the parent and draw identical numbers (a Monte Carlo run silently loses its independence).
The stdlib `random` module reseeds in the child through `os.register_at_fork`, and `spawn` children start fresh.
The robust fix is explicit per-worker streams: `np.random.default_rng(np.random.SeedSequence(seed).spawn(n)[i])`, which is also reproducible.

**Snippet 7: a custom exception that cannot be unpickled**

```python
import multiprocessing as mp


class ApiError(Exception):
    def __init__(self, status: int, body: str) -> None:
        super().__init__(f"{status}: {body}")


def call(_: int) -> None:
    raise ApiError(503, "down")


if __name__ == "__main__":
    with mp.Pool(2) as pool:
        try:
            pool.map_async(call, [1]).get(timeout=3)
        except mp.TimeoutError:
            print("no exception, no result: the pool's result thread died unpickling ApiError")
        pool.terminate()
```

Output (stderr also shows `TypeError: ApiError.__init__() missing 1 required positional argument: 'body'` in `Thread-3 (_handle_results)`):

```text
no exception, no result: the pool's result thread died unpickling ApiError
```

Without the timeout this program never ends, and even with it the pool's cleanup then fails with `AssertionError: Cannot have cache with result_handler not alive` (exit status 1); see MP8 for the fix.

**Snippet 8: a lambda passed to `Pool.map`**

```python
import multiprocessing as mp

if __name__ == "__main__":
    with mp.Pool(2) as pool:
        try:
            pool.map(lambda x: x * 2, [1, 2, 3])
        except Exception as exc:
            print(type(exc).__module__, type(exc).__name__)
```

Output:

```text
_pickle PicklingError
```

`_pickle.PicklingError`, raised in the parent before any work starts, because the function is pickled by name (MP5).

**Snippet 9: no main guard, one `Process`**

```python
import multiprocessing as mp


def work() -> None:
    print("child ran")


p = mp.get_context("spawn").Process(target=work)   # BUG: no main guard
p.start()
p.join()
print("exitcode", p.exitcode)
```

Output on stdout (stderr holds the child's `RuntimeError: An attempt has been made to start a new process before the current process has finished its bootstrapping phase.`):

```text
exitcode 1
```

"child ran" never prints: the child dies while importing the main module, before it reaches `work`, and the parent carries on with `exitcode` 1 (MP4).

---

## MP34. What do you check for in a code review of multiprocessing code?

- `if __name__ == "__main__":` guard, worker functions at module level, no heavy work at import time.
- Explicit start method via `get_context` where behavior depends on it; no `fork` in a process that has threads.
- One pool, created once and reused; never per request or per loop iteration.
- Arguments and results are small: pass paths, ids, or shared-memory names, not large objects; `chunksize` set for many small tasks.
- Every blocking call has a timeout: `result(timeout=)`, `get(timeout=)`, `join(timeout=)` followed by a liveness check.
- Queues drained before `join()`; sentinels are `None` or equal-comparable values; one sentinel per consumer.
- `BrokenProcessPool` handled; negative exit codes logged with their signal; poison items quarantined.
- Custom exceptions round-trip through `pickle`.
- Shared state: `Value` updates inside `get_lock()`, `Manager` proxies not used in hot loops, nested proxy mutations reassigned.
- Shared memory: one owner unlinks, everyone closes, views deleted before `close()`, `track=False` for independent attachers.
- Signals: workers ignore `SIGINT`; the parent handles `SIGINT` and `SIGTERM` and stops the pool; no `terminate()` on processes that hold locks unless you accept the corruption.
- Worker count derived from `os.process_cpu_count()` or the container limit, and multiplied correctly across web workers, pools, and library thread pools.
- Logging goes through a `QueueHandler`, and `faulthandler` is enabled in workers that run C extensions.

---

## Go deeper

- [Python core](02-Python-Core.md) Y9 (GIL and the threading, multiprocessing, asyncio choice) and Y19 (memory management).
- [Concurrency fundamentals and threading](14-Concurrency-Fundamentals-and-Threading.md), [Asyncio deep dive](16-Asyncio-Deep-Dive.md), and [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md).
- [Docker, Kubernetes, CI/CD, cloud](09-Docker-Kubernetes-CICD-Cloud.md) D8 (worker counts from cgroup limits) and D11 (CPU throttling and OOMKilled).
- [Microservices and messaging](08-Microservices-and-Messaging.md) M17 (Celery).
- Vault: [The GIL](../Python_Zero_to_Godhood/Chapter_10_CONCURRENCY_MECHANICS__THE_GLOBAL_INTERPRETER_LOCK.md), [Free-threaded Python internals](../Python_Zero_to_Godhood/Chapter_20_FREE-THREADED_PYTHON_GIL_REMOVAL_INTERNALS.md), [Subinterpreters](../Python_Zero_to_Godhood/Chapter_22_SUBINTERPRETERS__MULTI-CORE_PARALLELISM.md), [CPU and I/O bound concurrency (26)](../Python_Zero_to_Godhood/Chapter_26_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md), [CPU and I/O bound concurrency (27)](../Python_Zero_to_Godhood/Chapter_27_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md), [Shared memory and proxies](../Python_Zero_to_Godhood/Chapter_55_Advanced_Concurrency_Shared_Memory_and_Proxies.md), [Signals and subprocesses](../Python_Zero_to_Godhood/Chapter_37_OS_Services_Signal_Handling_and_Subprocesses.md).
- OS background (GIOS): [Processes and process management](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management.md), [Threads and concurrency](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L2-Threads-and-Concurrency.md), [Memory management](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-3-Resource-Management/P3L2-Memory-Management.md), [Inter-process communication](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-3-Resource-Management/P3L3-Inter-Process-Communication.md), [Synchronization constructs](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-3-Resource-Management/P3L4-Synchronization-Constructs.md).
- Official docs: [multiprocessing](https://docs.python.org/3/library/multiprocessing.html) (especially "Programming guidelines"), [multiprocessing.shared_memory](https://docs.python.org/3/library/multiprocessing.shared_memory.html), [concurrent.futures](https://docs.python.org/3/library/concurrent.futures.html), [concurrent.interpreters](https://docs.python.org/3/library/concurrent.interpreters.html), [free-threading HOWTO](https://docs.python.org/3/howto/free-threading-python.html), [gc.freeze](https://docs.python.org/3/library/gc.html#gc.freeze), [logging cookbook: multiple processes](https://docs.python.org/3/howto/logging-cookbook.html), [NumPy parallel random generation](https://numpy.org/doc/stable/reference/random/parallel.html).
- PEPs: [PEP 703](https://peps.python.org/pep-0703/) (free-threading), [PEP 779](https://peps.python.org/pep-0779/) (supported status), [PEP 734](https://peps.python.org/pep-0734/) (`concurrent.interpreters`), [PEP 683](https://peps.python.org/pep-0683/) (immortal objects).
