---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://docs.python.org/3/library/asyncio-task.html, https://docs.python.org/3/library/asyncio-eventloop.html, https://docs.python.org/3/library/asyncio-runner.html, https://docs.python.org/3/library/asyncio-sync.html, https://docs.python.org/3/library/asyncio-queue.html, https://docs.python.org/3/library/asyncio-stream.html, https://docs.python.org/3/library/asyncio-subprocess.html, https://docs.python.org/3/library/asyncio-dev.html, https://docs.python.org/3/library/asyncio-graph.html, https://docs.python.org/3/library/contextvars.html, https://docs.python.org/3/library/contextlib.html, https://docs.python.org/3/library/unittest.html, https://docs.python.org/3/whatsnew/3.11.html, https://docs.python.org/3/whatsnew/3.12.html, https://docs.python.org/3/whatsnew/3.13.html, https://docs.python.org/3/whatsnew/3.14.html, https://docs.python.org/3.14/howto/remote_debugging.html, https://peps.python.org/pep-0492/, https://peps.python.org/pep-0525/, https://peps.python.org/pep-0654/, https://pytest-asyncio.readthedocs.io/en/stable/, https://anyio.readthedocs.io/en/stable/threads.html, https://pypi.org/project/uvloop/]
---

# Asyncio deep dive

The asyncio questions an interviewer asks after "explain the event loop", ordered by how often they come up, with the follow-up chains that separate someone who has used `async def` from someone who has run asyncio services in production.
AS1-AS3 (sync vs async, what `await` does, the event loop), AS5 (gather vs TaskGroup vs wait), AS6 (lost task references), AS9-AS10 (timeouts and cancellation), and AS12-AS13 (blocking the loop) are near-certain; AS30 (predict the output) is how a concurrency-heavy screen opens.
This note goes deeper than the short answers in [Python core](02-Python-Core.md) Y9-Y10, [FastAPI](04-FastAPI.md) A2 and A18, [Flask](03-Flask.md) F13, [Testing, debugging, production](10-Testing-Debugging-Production.md) T10, and [Coding round](11-Coding-Round.md) K11-K12, and pairs with [Concurrency fundamentals and threading](14-Concurrency-Fundamentals-and-Threading.md), [Multiprocessing and parallelism](15-Multiprocessing-and-Parallelism.md), and [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md).
Every code sample here was run on 2026-09-30 under Python 3.14.7 (GIL build) with pytest 9.1.1, pytest-asyncio 1.4.0, anyio 4.15.1, and Starlette 1.7.0: 61 pytest tests passed (46 harness tests that executed all 44 Python blocks, compared the 13 "Output:" blocks against real stdout, and ran the 5 in-note test files under the TOML config shown in AS28, plus the 15 tests inside those files); uvloop numbers used uvloop 0.22.1 in a separate venv.
Performance numbers are single-machine numbers from an Apple M3 Pro (12 cores, macOS), labeled where they appear.

---

## AS1. What is the difference between synchronous and asynchronous execution? When does async actually help? (must know)

- **Synchronous**: each call runs to completion before the next starts; while a call waits on the network, the thread sits idle.
- **Asynchronous (asyncio)**: one thread runs an event loop; when a task reaches `await` on something that is not ready (a socket read, a timer), it gives the thread back to the loop, which runs another task.
  The waits overlap, so N concurrent 100 ms calls take about 100 ms, not N x 100 ms.
- Async is **concurrency, not parallelism**: only one piece of Python runs at any instant, so it helps I/O-bound work (network, database, timers) and does nothing for CPU-bound work.
- It is **cooperative**: a task switches only at an `await` that actually suspends; code between awaits runs without interruption, which is both why it is cheap and why one blocking call freezes everything.

Timeline, three requests that each wait 100 ms on a remote service:

```text
Synchronous: one thread, calls run back to back
t (ms)   0         100        200        300
req A    [s|..wait..|r]
req B                [s|..wait..|r]
req C                           [s|..wait..|r]
total ~300 ms; the thread is idle during every "wait"

Asynchronous: one thread, one event loop
t (ms)   0         100
req A    [s|..wait..|r]
req B     [s|..wait..|r]
req C      [s|..wait..|r]
total ~100 ms; s = send and suspend at await, r = loop resumes the task when the reply arrives

CPU-bound work under asyncio: no overlap at all
task A   [#####]
task B          [#####]
task C                 [#####]
total = sum of the work; the loop cannot switch until a task awaits
```

```python
import asyncio
import time


def fetch_sync(delay: float) -> float:
    time.sleep(delay)                      # the thread waits; nothing else can run
    return delay


async def fetch_async(delay: float) -> float:
    await asyncio.sleep(delay)             # the task waits; the loop runs other tasks
    return delay


async def cpu(n: int) -> int:
    return sum(i * i for i in range(n))    # no await inside: nothing to overlap


start = time.perf_counter()
for _ in range(3):
    fetch_sync(0.1)
sync_io = time.perf_counter() - start


async def io_main() -> None:
    await asyncio.gather(fetch_async(0.1), fetch_async(0.1), fetch_async(0.1))


start = time.perf_counter()
asyncio.run(io_main())
async_io = time.perf_counter() - start

assert sync_io >= 0.3
assert async_io < 0.2                      # the three waits overlapped

start = time.perf_counter()
for _ in range(3):
    asyncio.run(cpu(1_000_000))
sequential_cpu = time.perf_counter() - start


async def cpu_main() -> None:
    await asyncio.gather(cpu(1_000_000), cpu(1_000_000), cpu(1_000_000))


start = time.perf_counter()
asyncio.run(cpu_main())
gathered_cpu = time.perf_counter() - start
assert gathered_cpu > 0.7 * sequential_cpu  # no speedup for CPU work
```

Observed on the M3 Pro: about 0.31 s synchronous vs 0.10 s asynchronous for the I/O case, and the CPU case takes the same time either way.

| Model | Runs Python in parallel? | Switches when | Good for | Cost per unit |
| --- | --- | --- | --- | --- |
| Synchronous | No | Never | Scripts, simple CLIs | None |
| Threads (GIL build) | No (yes on free-threaded 3.14t) | OS preempts; GIL handed off every 5 ms or on blocking I/O | Blocking libraries, moderate I/O concurrency | ~an OS thread each (stack, scheduler) |
| asyncio | No | Only at `await` that suspends | 1k-100k sockets, high I/O concurrency | ~1-2 KB and a few microseconds per task (see AS25) |
| Processes | Yes | OS preempts | CPU-bound work | A whole interpreter each, pickling for IPC |

**Follow-ups they ask:**

- "Is asyncio faster than threads?"
  Not per request; its win is that 10,000 idle connections cost 10,000 small objects instead of 10,000 threads, and switches happen at known points so shared state needs fewer locks.
- "Why not just use threads?"
  Threads are fine for tens to low hundreds of concurrent blocking calls and work with every sync library; asyncio wins at high connection counts and gives you cancellation and timeouts that actually work (a thread cannot be cancelled).
- "Does async make a single request faster?"
  No; latency of one request is unchanged, throughput under concurrency improves.

See [Concurrency fundamentals and threading](14-Concurrency-Fundamentals-and-Threading.md) for the thread side and [Multiprocessing and parallelism](15-Multiprocessing-and-Parallelism.md) for CPU-bound work.

---

## AS2. What does `async def` return, and what does `await` actually do? Coroutine vs Task vs Future. (must know)

- Calling an `async def` function runs **none** of its body; it returns a **coroutine object**, a suspended frame.
  Something must drive it: `await` it from another coroutine, wrap it in a Task, or hand it to `asyncio.run`.
- `await x` calls `x.__await__()` and delegates to that iterator, exactly like `yield from`.
  When the chain bottoms out at a Future that is not done, the Future **yields itself** up through every coroutine frame to the Task driving it; the Task registers a done-callback on the Future and returns control to the loop.
  When the Future gets a result, its callback schedules the Task, which calls `coro.send(None)` and execution continues right after the `await`.
- A **Future** is a placeholder for a result that some callback will set later; a **Task** is a Future subclass that drives a coroutine step by step; an **awaitable** is anything with `__await__` (coroutines, Tasks, Futures, custom objects).
- A coroutine that is created but never awaited is garbage collected with `RuntimeWarning: coroutine '...' was never awaited`, and its side effects never happen.

```python
import asyncio
import gc
import inspect
import warnings


async def add(a: int, b: int) -> int:
    await asyncio.sleep(0)
    return a + b


coro = add(1, 2)
assert inspect.iscoroutine(coro)
assert inspect.getcoroutinestate(coro) == "CORO_CREATED"   # nothing has run yet
coro.close()                                                # discard it without the warning


class Suspend:
    """The smallest awaitable: its __await__ yields once, like a pending Future does."""

    def __await__(self):
        value = yield "suspended"          # this value travels out to whoever drives the coroutine
        return value                       # and this becomes the result of `await Suspend()`


async def demo() -> str:
    got = await Suspend()
    return f"resumed with {got}"


c = demo()
assert c.send(None) == "suspended"         # run until the first real suspension point
try:
    c.send(42)                             # what a Task does when the awaited thing completes
except StopIteration as stop:
    assert stop.value == "resumed with 42"


async def main() -> None:
    loop = asyncio.get_running_loop()
    fut = loop.create_future()                   # a bare Future: someone else sets the result
    loop.call_later(0.01, fut.set_result, "done")
    assert await fut == "done"

    task = asyncio.create_task(add(2, 3))        # a Task starts on the next loop iteration
    assert isinstance(task, asyncio.Future)      # Task is a Future subclass
    assert not task.done()
    assert await task == 5
    assert await task == 5                       # a finished Task can be awaited again; a coroutine cannot

    once = add(1, 1)
    await once
    try:
        await once
    except RuntimeError as exc:
        assert "cannot reuse already awaited coroutine" in str(exc)


asyncio.run(main())

with warnings.catch_warnings(record=True) as caught:
    warnings.simplefilter("always")
    add(1, 2)                                    # BUG: created, never awaited
    gc.collect()
assert "coroutine 'add' was never awaited" in str(caught[0].message)
```

| Object | What it is | Created by | Runs by itself? | Await twice? |
| --- | --- | --- | --- | --- |
| Coroutine | Suspended `async def` frame | Calling an `async def` function | No | No (`RuntimeError`) |
| `asyncio.Future` | Result slot plus done-callbacks, bound to one loop | `loop.create_future()`, library internals | No code to run | Yes |
| `asyncio.Task` | Future that drives a coroutine | `create_task`, `TaskGroup.create_task`, `gather`, `asyncio.run` | Yes, scheduled on the loop | Yes |
| `concurrent.futures.Future` | Thread-safe result slot for executors | `executor.submit`, `run_coroutine_threadsafe` | No | Not awaitable; wrap with `asyncio.wrap_future` |
| Awaitable | Anything with `__await__` | Any of the above, custom classes | Depends | Depends |

**Follow-ups they ask:**

- "Does `await coro()` create a Task?"
  No; it runs the coroutine inside the current task, so there is no concurrency and no scheduling overhead.
  Concurrency comes only from Tasks (`create_task`, `gather`, `TaskGroup`).
- "Where does the loop come in?"
  Only at the bottom: a Future registers its readiness with the loop (a timer for `sleep`, a selector registration for a socket), and the loop's callback sets the result.
- "Generators vs coroutines?"
  Native coroutines (PEP 492) are built on the same send/yield machinery as generators, but `async def` objects are not iterable and cannot be used with `yield from`; the old `@asyncio.coroutine` decorator was removed in 3.11.

---

## AS3. How does the event loop work internally? (must know)

- The loop keeps two queues: `_ready`, a FIFO deque of callbacks (Handles) to run now, and `_scheduled`, a heap of timer handles ordered by deadline.
- `call_soon(cb)` appends to `_ready`; `call_later(delay, cb)` and `call_at(when, cb)` push onto the heap; `add_reader(fd, cb)` registers the file descriptor with the **selector** (kqueue on macOS and BSD, epoll on Linux, IOCP via the Proactor loop on Windows).
- One iteration (`_run_once`) does: compute a timeout (0 if `_ready` is non-empty, else time until the earliest timer, else block forever), call `selector.select(timeout)`, append the callbacks of every ready fd to `_ready`, move expired timers to `_ready`, then run exactly the callbacks that were in `_ready` at that moment.
  Callbacks added while running go to the next iteration, so nothing can starve the selector.
- A Task is just a callback (`Task.__step`) that sends into its coroutine; when the coroutine yields a pending Future, the Task adds its wakeup as the Future's done-callback, and when the Future completes the wakeup is `call_soon`-ed.

```text
            +--------------------------------------------------+
            |                 one loop iteration               |
            |                                                  |
 timers --> | timeout = 0 if ready else (earliest timer - now) |
 (heap)     | events  = selector.select(timeout)   <-- sockets |
            | ready  += callbacks for readable/writable fds    |
            | ready  += timers whose deadline has passed       |
            | run the N callbacks in ready (FIFO)              |
            |   Task.__step -> coro.send(None) -> next await   |
            +--------------------------------------------------+
                         ^                          |
                         |   fut.set_result(...)    |
                         +-- call_soon(task.wakeup) +
```

The ordering rules, observed:

```python
import asyncio
import selectors
import socket


async def main() -> None:
    loop = asyncio.get_running_loop()
    log: list[str] = []
    done = loop.create_future()
    a, b = socket.socketpair()
    a.setblocking(False)
    b.setblocking(False)

    def on_readable() -> None:
        log.append(f"reader: {b.recv(100).decode()}")
        loop.remove_reader(b)
        done.set_result(None)

    loop.add_reader(b, on_readable)                 # registered with the selector
    loop.call_later(0.02, log.append, "call_later 20ms")
    loop.call_later(0.01, log.append, "call_later 10ms")
    loop.call_soon(log.append, "call_soon 1")
    loop.call_soon(log.append, "call_soon 2")
    loop.call_later(0.03, a.send, b"ping")          # makes b readable at ~30 ms
    log.append("main still running")                # callbacks never interrupt running code
    await done
    a.close()
    b.close()
    print("\n".join(log))
    print(selectors.DefaultSelector.__name__)


asyncio.run(main())
```

Output:

```text
main still running
call_soon 1
call_soon 2
call_later 10ms
call_later 20ms
reader: ping
KqueueSelector
```

On Linux the last line is `EpollSelector`.

**Follow-ups they ask:**

- "How does `asyncio.sleep(0)` differ from `asyncio.sleep(0.001)`?"
  `sleep(0)` is special-cased to a bare `yield`: the task is rescheduled with `call_soon` and runs again on the next iteration; a positive sleep creates a timer on the heap.
- "What wakes the loop when another thread calls `call_soon_threadsafe`?"
  A self-pipe (a socketpair registered with the selector); the method writes a byte to it, so `select()` returns (AS17).
- "Where does `await reader.read()` wait?"
  In the selector: the stream's transport registered the socket with `add_reader`, and when data arrives the protocol callback completes the Future the reader was awaiting.

---

## AS4. Build a tiny event loop from generators to show how cooperative scheduling works.

- Model a task as a generator; `yield` is the suspension point, and the value it yields is a request to the scheduler (here, "sleep for N seconds").
- The scheduler keeps a ready deque and a timer heap, exactly like `asyncio`'s `_ready` and `_scheduled`, and when nothing is ready it sleeps until the next timer, which stands in for `selector.select(timeout)`.
- This is the idea behind asyncio (and behind David Beazley's classic curio talks); the real loop adds I/O readiness, Futures, cancellation, and exception handling.

```python
# tiny_loop.py
"""A toy event loop: generators are tasks, `yield` is the suspension point."""
import heapq
import time
from collections import deque


class Sleep:
    def __init__(self, seconds: float):
        self.seconds = seconds


class TinyLoop:
    def __init__(self):
        self.ready = deque()                 # runnable now, like asyncio's loop._ready
        self.timers = []                     # heap of (deadline, seq, task), like loop._scheduled
        self.seq = 0                         # tie-breaker so the heap never compares generators
        self.results = {}

    def spawn(self, name, gen):
        self.ready.append((name, gen))

    def run(self):
        while self.ready or self.timers:
            if not self.ready:               # idle: "block in select()" until the next deadline
                time.sleep(max(0.0, self.timers[0][0] - time.monotonic()))
            now = time.monotonic()
            while self.timers and self.timers[0][0] <= now:
                self.ready.append(heapq.heappop(self.timers)[2])
            for _ in range(len(self.ready)):  # run only what was ready at the start of this pass
                name, gen = self.ready.popleft()
                try:
                    request = gen.send(None)  # resume the task until its next yield
                except StopIteration as stop:
                    self.results[name] = stop.value
                    continue
                if isinstance(request, Sleep):
                    self.seq += 1
                    heapq.heappush(self.timers, (now + request.seconds, self.seq, (name, gen)))
                else:                         # bare `yield`: go to the back of the line
                    self.ready.append((name, gen))
        return self.results
```

```python
# test_tiny_loop.py
import time

from tiny_loop import Sleep, TinyLoop


def ticker(name, steps, log):
    for i in range(steps):
        log.append(f"{name}{i}")
        yield                                  # like `await asyncio.sleep(0)`
    return f"{name} done"


def sleeper(name, steps, delay, log):
    for i in range(steps):
        log.append(f"{name}{i}")
        yield Sleep(delay)                     # like `await asyncio.sleep(delay)`
    return name


def test_bare_yields_interleave_round_robin():
    log = []
    loop = TinyLoop()
    loop.spawn("a", ticker("a", 3, log))
    loop.spawn("b", ticker("b", 2, log))
    assert loop.run() == {"b": "b done", "a": "a done"}
    assert log == ["a0", "b0", "a1", "b1", "a2"]


def test_sleeps_overlap_instead_of_adding_up():
    log = []
    loop = TinyLoop()
    loop.spawn("x", sleeper("x", 3, 0.1, log))
    loop.spawn("y", sleeper("y", 3, 0.1, log))
    start = time.monotonic()
    loop.run()
    elapsed = time.monotonic() - start
    assert 0.3 <= elapsed < 0.5                # 3 x 100 ms each, overlapped; sequential would be 0.6
    assert sorted(log[:2]) == ["x0", "y0"]
```

**Follow-ups they ask:**

- "What is missing compared with asyncio?"
  I/O readiness (the selector), Futures that let one task wait on another, exception propagation to awaiters, cancellation (throwing `CancelledError` into the generator with `gen.throw`), and thread-safe wakeups.
- "How would you add cancellation?"
  Store a flag and, on the next resume, call `gen.throw(Cancelled())` instead of `gen.send(None)`; that is literally what `Task.cancel` arranges.

---

## AS5. Compare `gather`, `TaskGroup`, `wait`, `as_completed`, `timeout`, `wait_for`, and `shield`. (must know)

- `gather`: run awaitables concurrently, results in **argument order**; the first exception propagates to the caller but siblings **keep running**; `return_exceptions=True` puts exceptions in the result list.
- `TaskGroup` (3.11+): structured concurrency; any child failure **cancels the siblings** and the body, then raises an `ExceptionGroup`; the `async with` block does not exit until every child is done.
- `wait`: takes Tasks (coroutines are rejected since 3.11), returns `(done, pending)` sets, and never raises the tasks' exceptions or cancels anything; you choose `FIRST_COMPLETED`, `FIRST_EXCEPTION`, or `ALL_COMPLETED`.
- `as_completed`: results in **completion order**; since 3.13 it is also an async iterator that yields the original Tasks.
- `timeout`/`timeout_at` (3.11+) bound a block, `wait_for` bounds one awaitable, `shield` protects an inner awaitable from the caller's cancellation.

| Tool | Returns | On a child exception | Cancels siblings? | Order | Use when |
| --- | --- | --- | --- | --- | --- |
| `await asyncio.gather(*aws)` | list of results | first exception raised, others keep running | No (only if the gather itself is cancelled) | argument | Independent calls, partial failure acceptable (`return_exceptions=True`) |
| `async with TaskGroup()` | nothing; read `task.result()` | all errors raised as `ExceptionGroup` | Yes | n/a | Default for new code: all-or-nothing work with clean cancellation |
| `await asyncio.wait(tasks, return_when=...)` | `(done, pending)` | never raises | No | set | Races, hedged requests, "first N to finish", custom policies |
| `asyncio.as_completed(aws)` | iterator of awaitables | raised when you await that item | No | completion | Streaming results to the caller as they arrive |
| `async with asyncio.timeout(s)` | n/a | `TimeoutError` at the block boundary | Cancels the current task's work inside the block | n/a | Deadline for several steps; reschedulable |
| `await asyncio.wait_for(aw, s)` | the result | `TimeoutError`, inner cancelled | n/a | n/a | Deadline for one awaitable |
| `await asyncio.shield(aw)` | the result | propagates | Caller cancel does not reach `aw` | n/a | Work that must finish even if the requester goes away |
| `asyncio.create_task(coro)` | Task | stored on the Task | n/a | n/a | Start work now, await later; keep a reference (AS6) |

```python
import asyncio


async def job(name: str, delay: float) -> str:
    await asyncio.sleep(delay)
    return name


async def main() -> None:
    fast = asyncio.create_task(job("fast", 0.01))
    slow = asyncio.create_task(job("slow", 5))
    done, pending = await asyncio.wait({fast, slow}, return_when=asyncio.FIRST_COMPLETED)
    assert done == {fast} and pending == {slow}
    slow.cancel()                                  # wait() never cancels for you
    await asyncio.gather(slow, return_exceptions=True)
    assert slow.cancelled()

    stray = job("coro", 0)
    try:
        await asyncio.wait([stray])                # bare coroutines are rejected since 3.11
    except TypeError as exc:
        assert "Passing coroutines is forbidden" in str(exc)
        stray.close()

    order = [await aw for aw in asyncio.as_completed([job("b", 0.03), job("a", 0.01), job("c", 0.02)])]
    assert order == ["a", "c", "b"]                # completion order

    tasks = [asyncio.create_task(job(n, d)) for n, d in [("b", 0.03), ("a", 0.01)]]
    seen = []
    async for t in asyncio.as_completed(tasks):    # 3.13+: yields the original Task objects
        assert t in tasks
        seen.append(await t)
    assert seen == ["a", "b"]

    assert await asyncio.gather(job("x", 0.02), job("y", 0.01)) == ["x", "y"]   # argument order


asyncio.run(main())
```

**Follow-ups they ask:**

- "Which do you reach for first?"
  `TaskGroup` for all-or-nothing fan-out, `gather(..., return_exceptions=True)` behind a semaphore for best-effort fan-out, `wait(FIRST_COMPLETED)` for races and hedging.
- "Why was passing coroutines to `wait` removed?"
  `wait` wrapped them in hidden Tasks, so the returned `done` set contained objects the caller never had, and `coro in done` was always false.

---

## AS6. What happens if you call `create_task` and do not keep the result? How do you do fire-and-forget safely? (must know)

- The event loop keeps only **weak references** to tasks (`asyncio.all_tasks` is backed by a weak set), so a task nobody references can be garbage collected **while it is still pending**.
  The loop then logs `Task was destroyed but it is pending!` and the work silently never finishes.
- It bites when the task is waiting on something that is itself only reachable from the task (a Future created inside it, a connection owned by it): the task and the Future form an unreachable cycle.
- A task that failed and was never awaited logs `Task exception was never retrieved` when it is collected, often far from the cause.
- Fixes: use a `TaskGroup` (it owns its children), or keep a strong reference in a set and discard it in a done-callback; for work that must survive the request, use a real queue (Celery, Kafka, RQ) instead of a task.

```python
import asyncio
import gc


async def waits_forever() -> None:
    await asyncio.get_running_loop().create_future()   # nothing else references this Future


async def main() -> None:
    loop = asyncio.get_running_loop()
    messages: list[str] = []
    loop.set_exception_handler(lambda _loop, ctx: messages.append(ctx["message"]))

    asyncio.create_task(waits_forever())    # BUG: the only strong reference is dropped here
    await asyncio.sleep(0)                  # the task starts and suspends on its Future
    gc.collect()                            # a collection that happens to run now
    assert messages == ["Task was destroyed but it is pending!"]

    background: set[asyncio.Task] = set()   # FIX: hold strong references until each task finishes

    def spawn(coro) -> asyncio.Task:
        task = asyncio.create_task(coro)
        background.add(task)
        task.add_done_callback(background.discard)
        return task

    spawn(waits_forever())
    await asyncio.sleep(0)
    gc.collect()
    assert len(messages) == 1 and len(background) == 1   # survived the collection
    pending = list(background)
    for task in pending:
        task.cancel()
    await asyncio.gather(*pending, return_exceptions=True)
    assert not background                                # removed by the done-callback


asyncio.run(main())
```

**Follow-ups they ask:**

- "Why would the loop hold only weak references?"
  So that a task that is truly unreachable and blocked forever can be reclaimed rather than leak; the docs state it explicitly under `create_task`: save a reference to the result.
- "Is the done-callback enough error handling?"
  No; also log `task.exception()` for tasks that failed, or the error only surfaces as "never retrieved" at collection time.
- "Fire-and-forget in FastAPI?"
  `BackgroundTasks` runs after the response in the same process and dies with it; see [FastAPI](04-FastAPI.md) A16 for when a real queue is required.

---

## AS7. Walk through `gather` semantics precisely: exceptions, siblings, and cancellation.

- Results come back in argument order regardless of completion order.
- `return_exceptions=False` (default): the first exception is raised from `await gather(...)` immediately, and the other awaitables are **not cancelled**; they keep running unobserved, which can leak work and produce "exception was never retrieved" logs.
- `return_exceptions=True`: every outcome is a list element, including `CancelledError` for a child that was cancelled.
- Cancelling the **gather** cancels every child; cancelling **one child** is treated as that child raising `CancelledError`, and the gather itself is not marked cancelled.

```python
import asyncio


async def boom() -> None:
    await asyncio.sleep(0.01)
    raise ValueError("upstream failed")


async def slow() -> str:
    await asyncio.sleep(0.1)
    return "slow done"


async def main() -> None:
    sibling = asyncio.create_task(slow())
    try:
        await asyncio.gather(boom(), sibling)
    except ValueError:
        assert not sibling.done() and not sibling.cancelled()   # still running
    assert await sibling == "slow done"                          # and it finishes later

    out = await asyncio.gather(boom(), slow(), return_exceptions=True)
    assert isinstance(out[0], ValueError) and out[1] == "slow done"

    child = asyncio.create_task(asyncio.sleep(1))
    group = asyncio.gather(child, asyncio.sleep(0.01, "ok"), return_exceptions=True)
    await asyncio.sleep(0)
    child.cancel()                                     # cancel one child only
    result = await group
    assert isinstance(result[0], asyncio.CancelledError) and result[1] == "ok"

    a = asyncio.create_task(asyncio.sleep(1))
    b = asyncio.create_task(asyncio.sleep(1))
    group = asyncio.gather(a, b)
    await asyncio.sleep(0)
    group.cancel()                                     # cancel the gather itself
    try:
        await group
    except asyncio.CancelledError:
        pass
    assert a.cancelled() and b.cancelled()


asyncio.run(main())
```

**What goes wrong in production:** a handler does `await gather(call_a(), call_b())`, `call_a` fails fast, the handler returns a 500, and `call_b` keeps holding a pooled connection for its full timeout; under an error storm the pool empties.
Use a `TaskGroup` when failure should stop the siblings.

---

## AS8. What is structured concurrency? How do `TaskGroup`, `ExceptionGroup`, and `except*` work together? (must know)

- **Structured concurrency**: every task has a parent scope that outlives it; when the `async with TaskGroup()` block exits, all its children are finished, so no task leaks past the function that started it.
- If a child raises (anything other than `CancelledError`), the group cancels the remaining children **and the body of the `async with`**, waits for them, then raises `ExceptionGroup` (or `BaseExceptionGroup`) containing every non-cancellation error.
- `except* ValueError` handles the `ValueError` members of the group and lets the rest propagate as a smaller group (PEP 654, 3.11+).
- `KeyboardInterrupt` and `SystemExit` in a child are re-raised as themselves, not wrapped.
- To stop a group early on purpose, raise a private exception from a child and catch it with `except*`; 3.14 has no `TaskGroup.cancel()` method.

```python
import asyncio


async def ok(delay: float) -> str:
    await asyncio.sleep(delay)
    return "ok"


async def fail(delay: float, exc: Exception) -> None:
    await asyncio.sleep(delay)
    raise exc


async def main() -> None:
    body_reached_end = False
    slow = None
    try:
        async with asyncio.TaskGroup() as tg:
            first = tg.create_task(ok(0.01))
            slow = tg.create_task(ok(5))
            tg.create_task(fail(0.02, ValueError("bad row")))
            tg.create_task(fail(0.02, KeyError("missing")))
            await asyncio.sleep(1)          # the body is cancelled here when a child fails
            body_reached_end = True
    except* ValueError as eg:
        assert [str(e) for e in eg.exceptions] == ["bad row"]
    except* KeyError as eg:
        assert len(eg.exceptions) == 1
    assert first.result() == "ok"          # finished before the failure
    assert slow.cancelled()                # cancelled sibling
    assert not body_reached_end

    class Done(Exception):
        """Raised by a child to stop the whole group early."""

    async def find_first(delays: list[float]) -> float:
        winner: list[float] = []

        async def probe(d: float) -> None:
            await asyncio.sleep(d)
            winner.append(d)
            raise Done

        try:
            async with asyncio.TaskGroup() as tg:
                for d in delays:
                    tg.create_task(probe(d))
        except* Done:
            pass
        return winner[0]

    assert await find_first([0.3, 0.01, 0.2]) == 0.01


asyncio.run(main())
```

**Follow-ups they ask:**

- "`gather` or `TaskGroup` in a request handler?"
  `TaskGroup`: a failure cancels the siblings, nothing leaks, and all errors are reported, not just the first.
- "How do I get partial results with a TaskGroup?"
  Catch the exception inside each child and return a value or error object; the group then never fails.
- "Can I add tasks to a group after it started?"
  Yes, from the body or from children, until the group begins shutting down; after that `create_task` raises `RuntimeError`.

---

## AS9. How do timeouts work in asyncio? `timeout`, `timeout_at`, `wait_for`, and why they are built on cancellation. (must know)

- A timeout is **cancellation plus translation**: when the deadline passes, the loop calls `task.cancel()` on the current task; the `CancelledError` unwinds to the `async with asyncio.timeout(...)` boundary, which recognizes its own cancellation (via `Task.uncancel()`) and raises `TimeoutError` instead.
- `asyncio.timeout(delay)` and `asyncio.timeout_at(when)` (3.11+) bound a whole block; `delay=None` means no deadline yet, and `cm.reschedule(when)` moves it, which is how you implement a deadline that is decided later or extended on progress.
- `asyncio.wait_for(aw, timeout)` bounds one awaitable and cancels it on timeout; since 3.12 it is implemented with `asyncio.timeout`.
- `asyncio.TimeoutError` is the builtin `TimeoutError` since 3.11; catch `TimeoutError`.
- Because timeouts are cancellation, **code that swallows `CancelledError` breaks every timeout above it** (AS30 snippet 6).

```python
import asyncio
import time


async def slow(delay: float) -> str:
    await asyncio.sleep(delay)
    return "done"


async def main() -> None:
    assert asyncio.TimeoutError is TimeoutError

    try:
        async with asyncio.timeout(0.05) as cm:
            await slow(1)
    except TimeoutError:
        assert cm.expired()

    loop = asyncio.get_running_loop()
    async with asyncio.timeout(None) as cm:          # no deadline yet
        cm.reschedule(loop.time() + 0.5)             # decided at runtime, e.g. from a request header
        assert await slow(0.01) == "done"

    try:
        async with asyncio.timeout_at(loop.time() + 0.05):   # absolute deadline shared by several steps
            await slow(0.03)
            await slow(0.03)                                 # the second step blows the budget
    except TimeoutError:
        pass

    inner = asyncio.create_task(slow(1))
    try:
        await asyncio.wait_for(inner, 0.05)
    except TimeoutError:
        assert inner.cancelled()                     # wait_for cancels what it waited on

    start = time.perf_counter()
    try:
        async with asyncio.timeout(0.2):             # outer: 200 ms
            try:
                async with asyncio.timeout(0.05):    # inner: 50 ms fires first
                    await slow(1)
            except TimeoutError:
                pass                                 # only the inner deadline is converted here
            await slow(0.01)
    except TimeoutError:
        raise AssertionError("outer deadline did not fire")
    assert time.perf_counter() - start < 0.15


asyncio.run(main())
```

**Follow-ups they ask:**

- "Where should timeouts live in a service?"
  At every network call (client timeouts), plus an overall request deadline at the edge; pass the remaining budget downstream instead of giving each hop a fresh full timeout.
- "Why can a timeout take longer than its value?"
  The cancellation is delivered only at the next `await`; a blocking call inside the block delays it, and cleanup in `finally` runs before `TimeoutError` surfaces.
- "Timeout on a sync call?"
  Impossible to interrupt a running thread; `await asyncio.wait_for(asyncio.to_thread(f), 1)` stops waiting, but the thread keeps running `f` to completion.

---

## AS10. How does cancellation work, and why must you re-raise `CancelledError`? (must know)

- `task.cancel(msg=None)` does not stop anything immediately; it arranges for `CancelledError` to be **thrown into the coroutine at its current `await`** on the next loop iteration.
  It returns `False` if the task is already done.
- The error propagates up through `try`/`finally` blocks like any exception, so `finally` and `async with` cleanup run; the awaiting code sees `CancelledError`, and `task.cancelled()` becomes true only if the coroutine let it escape.
- `CancelledError` derives from `BaseException` (since 3.8), so `except Exception` does not catch it; a bare `except:` or `except BaseException:` does and must re-raise.
- If you catch it to clean up, **re-raise it**: swallowing it makes the task look successful, breaks `asyncio.timeout`, `TaskGroup`, and `asyncio.run` shutdown, and hangs graceful shutdowns.
- `await` inside a `finally` during cancellation works, but a second `cancel()` can interrupt that cleanup; shield critical cleanup or keep it short.

```python
import asyncio


async def worker(log: list[str]) -> None:
    try:
        log.append("working")
        await asyncio.sleep(10)
    except asyncio.CancelledError as exc:
        log.append(f"cancelled: {exc.args}")
        raise                                         # re-raise: the caller must see the cancellation
    finally:
        await asyncio.sleep(0)                        # async cleanup is allowed here
        log.append("cleanup ran")


async def swallower() -> str:
    try:
        await asyncio.sleep(10)
    except asyncio.CancelledError:
        return "pretended to finish"                  # BUG: cancellation swallowed


async def main() -> None:
    log: list[str] = []
    task = asyncio.create_task(worker(log))
    await asyncio.sleep(0)
    assert task.cancel("shutdown requested") is True  # the message rides on the exception
    try:
        await task
    except asyncio.CancelledError as exc:
        assert exc.args == ("shutdown requested",)
    assert task.cancelled()
    assert log == ["working", "cancelled: ('shutdown requested',)", "cleanup ran"]
    assert task.cancel() is False                     # already done

    bad = asyncio.create_task(swallower())
    await asyncio.sleep(0)
    bad.cancel()
    assert await bad == "pretended to finish"
    assert not bad.cancelled()                        # the caller cannot tell it was cancelled

    assert not issubclass(asyncio.CancelledError, Exception)


asyncio.run(main())
```

**Follow-ups they ask:**

- "Is it ever right to suppress `CancelledError`?"
  Only at the top of a scope that itself requested the cancellation (a supervisor that cancelled its own child and awaits it); even then, call `uncancel()` if you used `cancel()` on your own task (AS11).
- "What about `except Exception` wrappers in retry decorators?"
  Since 3.8 they do not catch cancellation; a retry wrapper written for 3.7 with `except BaseException` or bare `except` is the bug.
- "Can you cancel a thread started with `to_thread`?"
  You can cancel the awaiting task; the thread keeps running until the function returns.

---

## AS11. What are `Task.cancelling()`, `Task.uncancel()`, and `asyncio.shield()` for?

- Since 3.11 a Task counts pending cancel requests: `cancel()` increments the count, `cancelling()` reads it, and `uncancel()` decrements it.
  `asyncio.timeout` and `TaskGroup` use this to tell **their own** cancellation (convert or absorb it) from a cancellation that came from outside (let it propagate).
- If you write a component that cancels its own task and then absorbs the `CancelledError`, call `uncancel()` so outer timeouts and groups still work.
- `asyncio.shield(aw)` wraps the inner awaitable so that cancelling the **caller** does not cancel the inner task; the caller still gets `CancelledError` immediately, and the inner task keeps running.
  Keep a reference to the inner task, or it is the AS6 bug again.
- Typical shield use: finishing a database commit or a payment call after the client disconnects.

```python
import asyncio


async def commit(log: list[str]) -> str:
    await asyncio.sleep(0.05)
    log.append("committed")
    return "ok"


async def handler(inner: asyncio.Task) -> str:
    return await asyncio.shield(inner)


async def main() -> None:
    log: list[str] = []
    inner = asyncio.create_task(commit(log))          # keep the strong reference
    outer = asyncio.create_task(handler(inner))
    await asyncio.sleep(0.01)
    outer.cancel()                                    # e.g. the client disconnected
    try:
        await outer
    except asyncio.CancelledError:
        pass
    assert outer.cancelled() and not inner.done()     # the commit is still in flight
    assert await inner == "ok" and log == ["committed"]

    me = asyncio.current_task()
    assert me.cancelling() == 0
    me.cancel()                                       # cancel myself, as a timeout does internally
    assert me.cancelling() == 1
    try:
        await asyncio.sleep(1)
    except asyncio.CancelledError:
        assert me.uncancel() == 0                     # absorb my own request and restore the count
    assert me.cancelling() == 0
    async with asyncio.timeout(0.5):                  # outer machinery still works afterwards
        await asyncio.sleep(0.01)


asyncio.run(main())
```

**Follow-ups they ask:**

- "Does `shield` protect against `asyncio.run` shutdown?"
  No; at exit `asyncio.run` cancels every remaining task, including the shielded inner one.
  For must-complete work, await it during shutdown or move it to a durable queue.
- "Why not just catch `CancelledError` instead of shielding?"
  Catching it inside the work changes the caller's semantics (AS10); shield keeps the caller's cancellation honest while the work finishes.

---

## AS12. What counts as blocking the event loop, and how do you detect it? (must know)

- Anything that holds the thread without reaching an `await` blocks **every** task on the loop: health checks, other requests, timers, even timeouts.
- Detect it with **debug mode** (`PYTHONASYNCIODEBUG=1`, `python -X dev`, or `asyncio.run(main(), debug=True)`), which logs `Executing <Task ...> took 0.150 seconds` for any step longer than `loop.slow_callback_duration` (default 0.1 s), and with a **loop-lag monitor** exported as a metric; `py-spy dump --pid` shows the stuck stack in production.

| Blocking call | Why it blocks | Async-friendly replacement |
| --- | --- | --- |
| `time.sleep(x)` | Sleeps the thread | `await asyncio.sleep(x)` |
| `requests.get(...)`, `urllib` | Sync socket I/O | `httpx.AsyncClient`, `aiohttp` |
| `psycopg2`, sync SQLAlchemy `Session`, `pymysql` | Sync socket I/O | `asyncpg`, psycopg 3 async, SQLAlchemy `AsyncSession` ([FastAPI](04-FastAPI.md) A18) |
| `boto3`, most vendor SDKs | Sync HTTP underneath | `aioboto3`/`aiobotocore`, or `asyncio.to_thread` |
| `open(...).read()` on large files, `json.loads` of a 50 MB body, `pandas` | Disk or CPU work | `to_thread` for disk, a process pool for CPU |
| `bcrypt`/`argon2` hashing, image resize, crypto | CPU | Process pool, or `to_thread` if the C code releases the GIL |
| `threading.Lock.acquire()`, `queue.Queue.get()` | Waits on another thread | `asyncio.Lock`, `asyncio.Queue`, or bridge with `call_soon_threadsafe` |
| DNS via `socket.gethostbyname` | Sync resolver | `loop.getaddrinfo` (asyncio already runs it in the executor) |
| Tight CPU loops over large lists | No `await` inside | Chunk with `await asyncio.sleep(0)` or offload |

Debug mode catching a blocking call:

```python
import asyncio
import logging
import time

records: list[str] = []


class Capture(logging.Handler):
    def emit(self, record: logging.LogRecord) -> None:
        records.append(record.getMessage())


logging.getLogger("asyncio").addHandler(Capture())


async def handler() -> None:
    time.sleep(0.15)                       # BUG: blocking call inside a coroutine


async def main() -> None:
    loop = asyncio.get_running_loop()
    assert loop.get_debug() and loop.slow_callback_duration == 0.1
    await asyncio.create_task(handler(), name="slow-handler")


asyncio.run(main(), debug=True)
assert any("slow-handler" in r and " took " in r for r in records), records
```

Logged (abbreviated):

```text
Executing <Task finished name='slow-handler' coro=<handler() done, defined at ...> result=None created at ...> took 0.155 seconds
```

A loop-lag monitor you can export as a metric (used again in AS13):

```python
# looplag.py
import asyncio
import time


async def measure_lag(duration: float, interval: float = 0.005) -> float:
    """Worst lateness of a periodic timer over `duration`; lag means something held the loop."""
    loop = asyncio.get_running_loop()
    worst = 0.0
    end = loop.time() + duration
    while loop.time() < end:
        before = loop.time()
        await asyncio.sleep(interval)
        worst = max(worst, loop.time() - before - interval)
    return worst


async def _demo() -> None:
    monitor = asyncio.create_task(measure_lag(0.3))
    await asyncio.sleep(0.05)
    time.sleep(0.1)                        # simulate a blocking call
    lag = await monitor
    assert lag >= 0.09, lag


if __name__ == "__main__":
    asyncio.run(_demo())
```

**Follow-ups they ask:**

- "Debug mode in production?"
  No; it adds overhead (it records creation tracebacks for every task and handle).
  Use a lag metric and alert on p99 lag instead, and enable debug mode in CI and staging.
- "How do you prove a blocking call exists?"
  Run the service under load with debug mode, or `py-spy dump` a stuck worker: the main thread will be in `socket.recv` under `requests` or similar rather than in `select`.
- "Your health check timed out and Kubernetes restarted a healthy pod: why?"
  The loop was blocked, so even `/healthz` could not be served; see [Testing, debugging, production](10-Testing-Debugging-Production.md) T10.

---

## AS13. How do you fix blocking code: `to_thread`, `run_in_executor`, process pools, async libraries, and chunking? (must know)

- **Blocking I/O in a sync library**: `await asyncio.to_thread(func, *args)` (3.9+), which runs it in the loop's default `ThreadPoolExecutor` and copies the current `contextvars` context into the thread.
  The default pool has `min(32, os.process_cpu_count() + 4)` workers (16 on the 12-core test machine), which also caps how many blocking calls can run at once.
- **CPU-bound work**: `loop.run_in_executor(process_pool, func, *args)`; arguments and results are pickled, the function must be importable at module top level, and on macOS and Windows the pool uses `spawn`, so guard the entry point with `if __name__ == "__main__":`.
- **Prefer async-native libraries** where they exist (`httpx.AsyncClient`, `asyncpg`, `redis.asyncio`); `aiofiles` just wraps file calls in a thread pool, so it is a convenience, not a performance win.
- **Chunking**: if CPU work must stay on the loop, split it and `await asyncio.sleep(0)` between chunks so other tasks run; total time does not improve, but latency for everyone else does.
- `to_thread` with CPU-bound pure Python keeps the loop responsive only because the GIL is handed over every 5 ms (`sys.getswitchinterval()`); throughput does not improve on the GIL build.

```python
import asyncio
from concurrent.futures import ProcessPoolExecutor

from looplag import measure_lag

N = 6_000_000


def cpu_heavy(n: int) -> int:
    return sum(i * i for i in range(n))


async def cpu_chunked(n: int, chunk: int = 50_000) -> int:
    total = 0
    for start in range(0, n, chunk):
        total += sum(i * i for i in range(start, min(start + chunk, n)))
        await asyncio.sleep(0)                      # yield to the loop between chunks
    return total


async def lag_while(work) -> float:
    monitor = asyncio.create_task(measure_lag(0.5))
    await asyncio.sleep(0.02)                       # let the monitor start ticking
    await work()
    return await monitor


async def main(pool: ProcessPoolExecutor) -> dict[str, float]:
    loop = asyncio.get_running_loop()
    expected = cpu_heavy(N)
    await loop.run_in_executor(pool, cpu_heavy, 10)            # warm the pool before measuring

    async def inline() -> None:
        assert cpu_heavy(N) == expected                        # blocks the loop

    async def in_thread() -> None:
        assert await asyncio.to_thread(cpu_heavy, N) == expected

    async def in_process() -> None:
        assert await loop.run_in_executor(pool, cpu_heavy, N) == expected

    async def chunked() -> None:
        assert await cpu_chunked(N) == expected

    lags = {name: await lag_while(fn) for name, fn in
            [("inline", inline), ("to_thread", in_thread), ("process", in_process), ("chunked", chunked)]}
    assert lags["inline"] > 0.1
    assert lags["process"] < 0.08 and lags["chunked"] < 0.08
    return lags


if __name__ == "__main__":
    with ProcessPoolExecutor(max_workers=2) as pool:
        lags = asyncio.run(main(pool))
    print({k: f"{v * 1000:.1f} ms" for k, v in lags.items()})
```

Single-machine numbers (Apple M3 Pro, CPython 3.14.7 GIL build, five runs, machine shared with other jobs at load average ~30-45), worst lag of a 5 ms timer while about 0.2 s of CPU work ran:

| Where the CPU work ran | Worst loop lag |
| --- | --- |
| Inline in the coroutine | 190-210 ms (the whole computation) |
| `asyncio.to_thread` | 13-19 ms (GIL hand-offs) |
| Process pool via `run_in_executor` | 0.7-0.8 ms |
| Inline, chunked with `await asyncio.sleep(0)` every 50,000 items | 3.7-5.7 ms |

**Follow-ups they ask:**

- "`to_thread` vs `run_in_executor`?"
  `to_thread` is `run_in_executor(None, ...)` plus context propagation and keyword arguments; use `run_in_executor` when you need a specific executor (a process pool, or a dedicated small thread pool so one slow dependency cannot exhaust the shared one).
- "Why not raise the default pool to 1,000 threads?"
  Every blocked thread holds a connection or file and memory; if a dependency is slow, you have just converted a latency problem into a resource-exhaustion problem.
  Bound it and apply backpressure.
- "Does free-threaded 3.14t change this?"
  CPU work in `to_thread` can then run in parallel with the loop, but the loop itself is still single-threaded and still blocked by inline CPU work; see [Concurrency fundamentals and threading](14-Concurrency-Fundamentals-and-Threading.md).

---

## AS14. `asyncio.run`, `asyncio.Runner`, one loop per thread, `get_running_loop` vs `get_event_loop`, and Jupyter.

- `asyncio.run(main())` creates a new loop, runs `main` as a Task, then cancels leftover tasks, finalizes async generators, shuts down the default executor, and closes the loop; call it once, at the top of the program.
- `asyncio.Runner` (3.11+) keeps one loop across several `runner.run(...)` calls, so resources bound to the loop (connection pools, clients) survive between calls; useful in sync CLIs and test harnesses.
- There is at most **one running loop per thread**; `asyncio.run` inside a running loop raises `RuntimeError: asyncio.run() cannot be called from a running event loop`.
- Inside coroutines and callbacks use `asyncio.get_running_loop()`; it raises if no loop is running, which is what you want.
- `asyncio.get_event_loop()` in 3.14 raises `RuntimeError` when there is no current loop instead of silently creating one (3.12 and 3.13 created one with a `DeprecationWarning`; verified on 3.12.13 and 3.13.15), and the event-loop **policy** API (`get_event_loop_policy`, `set_event_loop_policy`) is deprecated in 3.14 and slated for removal in 3.16; pass `loop_factory=` to `asyncio.run` or `Runner` instead (for example uvloop).
- **Jupyter/IPython** already runs a loop in the kernel thread, so `asyncio.run` fails there; use top-level `await main()` in the cell, or run a separate loop in a background thread (AS17); `nest_asyncio` patches the loop to allow re-entry and is a hack best kept out of services.

```python
import asyncio
import warnings


async def inner() -> str:
    return "ok"


async def calls_run() -> str:
    coro = inner()
    try:
        asyncio.run(coro)                          # inside a running loop
    except RuntimeError as exc:
        coro.close()
        return str(exc)
    return "no error"


assert asyncio.run(calls_run()) == "asyncio.run() cannot be called from a running event loop"

with asyncio.Runner() as runner:                   # one loop, several entry points
    assert runner.run(inner()) == "ok"
    first_loop = runner.get_loop()
    assert runner.run(inner()) == "ok"
    assert runner.get_loop() is first_loop

try:
    asyncio.get_running_loop()
except RuntimeError as exc:
    assert str(exc) == "no running event loop"

try:
    asyncio.get_event_loop()                       # 3.14: no implicit loop creation any more
except RuntimeError as exc:
    assert "There is no current event loop" in str(exc)

with warnings.catch_warnings(record=True) as caught:
    warnings.simplefilter("always")
    asyncio.get_event_loop_policy()
assert issubclass(caught[0].category, DeprecationWarning)
assert "slated for removal in Python 3.16" in str(caught[0].message)
```

**Follow-ups they ask:**

- "Why did old code call `loop = asyncio.get_event_loop(); loop.run_until_complete(main())`?"
  That was the pre-3.7 idiom; it leaks the loop (never closed) and relies on implicit creation that 3.14 removed.
- "Can two threads each run a loop?"
  Yes, one each; objects bound to one loop (Futures, Locks, Queues, clients) must not be used from the other.
- "How does uvicorn run my app?"
  It creates the loop itself (uvloop if installed) and calls your ASGI app inside it; your code should never call `asyncio.run` in a request path.

---

## AS15. Asyncio is single-threaded: why do you still need locks? What are the async primitives?

- Asyncio code switches only at `await`, so a read-modify-write **without an await in the middle** is atomic with respect to other tasks.
  As soon as there is an `await` between the read and the write, another task can run in between: a classic lost update.
- `asyncio.Lock` serializes a critical section that contains awaits; `Event` broadcasts "something happened"; `Condition` combines a lock with wait/notify for predicates; `Semaphore` bounds concurrency; `BoundedSemaphore` also raises `ValueError` on an extra release; `Barrier` (3.11+) makes N tasks rendezvous.
- These primitives are **not thread-safe** and are bound to the loop they are first used on; to coordinate threads use `threading` primitives, and to signal from a thread into a loop use `call_soon_threadsafe` (AS17).
- Never use a `threading.Lock` around an `await`: a second task that tries to acquire it blocks the whole loop, including the task that holds it, which is a deadlock (AS30 snippet 7).

```python
import asyncio

counter = 0


async def unsafe_increment() -> None:
    global counter
    value = counter
    await asyncio.sleep(0)             # any suspension between read and write opens the window
    counter = value + 1


async def safe_increment(lock: asyncio.Lock) -> None:
    global counter
    async with lock:
        value = counter
        await asyncio.sleep(0)
        counter = value + 1


async def main() -> None:
    global counter
    await asyncio.gather(*(unsafe_increment() for _ in range(100)))
    assert counter == 1                # 99 updates lost: everyone read 0 before anyone wrote

    counter = 0
    lock = asyncio.Lock()
    await asyncio.gather(*(safe_increment(lock) for _ in range(100)))
    assert counter == 100

    sem = asyncio.Semaphore(2)
    sem.release()                      # a plain Semaphore silently grows past its initial value
    bounded = asyncio.BoundedSemaphore(2)
    try:
        bounded.release()
    except ValueError:
        pass                           # BoundedSemaphore catches the extra release

    ready = asyncio.Event()
    woken: list[str] = []

    async def waiter(name: str) -> None:
        await ready.wait()
        woken.append(name)

    waiters = [asyncio.create_task(waiter(n)) for n in "abc"]
    await asyncio.sleep(0)
    ready.set()                        # wakes every waiter, now and in the future
    await asyncio.gather(*waiters)
    assert sorted(woken) == ["a", "b", "c"]

    cond = asyncio.Condition()
    items: list[int] = []

    async def consumer() -> int:
        async with cond:
            await cond.wait_for(lambda: len(items) >= 3)   # re-checks the predicate on each notify
            return sum(items)

    consumer_task = asyncio.create_task(consumer())
    for i in range(3):
        async with cond:
            items.append(i + 1)
            cond.notify_all()
        await asyncio.sleep(0)
    assert await consumer_task == 6

    barrier = asyncio.Barrier(3)
    order: list[str] = []

    async def phase(name: str) -> None:
        order.append(f"{name} before")
        await barrier.wait()           # nobody passes until all three arrive
        order.append(f"{name} after")

    await asyncio.gather(phase("x"), phase("y"), phase("z"))
    assert all(entry.endswith("before") for entry in order[:3])


asyncio.run(main())
```

**Follow-ups they ask:**

- "Do you need a lock around `self.cache[key] = await fetch(key)`?"
  For correctness of the dict, no; but N concurrent misses all call `fetch` (a cache stampede).
  Store the in-flight Task in the dict so later callers await the same Task (single-flight).
- "Lock held across a slow `await`?"
  Correct but serializes every caller behind the slowest call; hold locks only around the state change, not around network I/O.

---

## AS16. Build a producer-consumer worker pool with `asyncio.Queue` and graceful shutdown. (must know)

- A bounded `asyncio.Queue(maxsize=N)` gives **backpressure**: `await queue.put(item)` suspends the producer when workers fall behind, so memory stays bounded.
- Classic shutdown: the producer finishes, `await queue.join()` waits until every item has had `task_done()` called, then the workers are cancelled.
- 3.13+ shutdown: `queue.shutdown()` makes further `put` raise `asyncio.QueueShutDown`, lets consumers drain what is left, then makes `get` raise `QueueShutDown` so workers exit their loops without cancellation; `shutdown(immediate=True)` discards the remaining items and wakes blocked getters immediately.
- Always call `task_done()` in a `finally`, or `join()` hangs forever after one failure; record per-item errors instead of letting one bad item kill a worker.

```python
# worker_pool.py
import asyncio
from collections.abc import Awaitable, Callable, Iterable
from typing import Any


async def run_pool_join(items: Iterable[Any], handle: Callable[[Any], Awaitable[Any]],
                        workers: int = 4) -> tuple[list[Any], list[BaseException]]:
    """Classic pattern: bounded queue, join(), then cancel the idle workers."""
    queue: asyncio.Queue = asyncio.Queue(maxsize=workers * 2)
    results: list[Any] = []
    errors: list[BaseException] = []

    async def worker() -> None:
        while True:
            item = await queue.get()
            try:
                results.append(await handle(item))
            except Exception as exc:          # one bad item must not kill the worker
                errors.append(exc)
            finally:
                queue.task_done()             # always, or join() never returns

    async with asyncio.TaskGroup() as tg:
        pool = [tg.create_task(worker()) for _ in range(workers)]
        for item in items:
            await queue.put(item)             # suspends while the queue is full: backpressure
        await queue.join()
        for task in pool:
            task.cancel()                     # idle workers are parked in get(); cancel them
    return results, errors


async def run_pool_shutdown(items: Iterable[Any], handle: Callable[[Any], Awaitable[Any]],
                            workers: int = 4) -> tuple[list[Any], list[BaseException]]:
    """3.13+ pattern: Queue.shutdown() lets workers drain and exit on their own."""
    queue: asyncio.Queue = asyncio.Queue(maxsize=workers * 2)
    results: list[Any] = []
    errors: list[BaseException] = []

    async def worker() -> None:
        while True:
            try:
                item = await queue.get()
            except asyncio.QueueShutDown:     # raised only once the queue is shut down and empty
                return
            try:
                results.append(await handle(item))
            except Exception as exc:
                errors.append(exc)
            finally:
                queue.task_done()

    async with asyncio.TaskGroup() as tg:
        for _ in range(workers):
            tg.create_task(worker())
        for item in items:
            await queue.put(item)
        queue.shutdown()                      # no more puts; workers finish the backlog, then exit
    return results, errors
```

```python
# test_worker_pool.py
import asyncio

import pytest

from worker_pool import run_pool_join, run_pool_shutdown

in_flight = 0
peak = 0


async def handle(item: int) -> int:
    global in_flight, peak
    in_flight += 1
    peak = max(peak, in_flight)
    try:
        await asyncio.sleep(0.005)
        if item == 13:
            raise ValueError("bad item 13")
        return item * 2
    finally:
        in_flight -= 1


@pytest.mark.parametrize("run_pool", [run_pool_join, run_pool_shutdown])
def test_pool_processes_everything_and_bounds_concurrency(run_pool):
    global peak
    peak = 0
    results, errors = asyncio.run(run_pool(range(40), handle, workers=4))
    assert sorted(results) == [i * 2 for i in range(40) if i != 13]
    assert [str(e) for e in errors] == ["bad item 13"]
    assert peak == 4


def test_put_after_shutdown_raises_and_immediate_discards():
    async def main() -> None:
        queue: asyncio.Queue = asyncio.Queue()
        for i in range(3):
            queue.put_nowait(i)
        queue.shutdown(immediate=True)
        assert queue.qsize() == 0
        with pytest.raises(asyncio.QueueShutDown):
            queue.put_nowait(99)
        with pytest.raises(asyncio.QueueShutDown):
            await queue.get()
        await asyncio.wait_for(queue.join(), 1)   # join() is released too

    asyncio.run(main())
```

**Follow-ups they ask:**

- "Why bounded?"
  An unbounded queue turns a slow consumer into unbounded memory growth and latency; a bounded one pushes back on the producer, which can then slow down, shed load, or return 503.
- "Where does this show up in a service?"
  Kafka or WebSocket consumers feeding a fixed pool of async workers, or batching writes to a database; see [Microservices and messaging](08-Microservices-and-Messaging.md).
- "What if the producer is a thread?"
  Use `loop.call_soon_threadsafe(queue.put_nowait, item)`, or a `queue.Queue` read with `to_thread` (AS17).

---

## AS17. How do you bridge threads and the event loop? `call_soon_threadsafe`, `run_coroutine_threadsafe`, and a background loop thread.

- From another thread, **never** call loop methods or touch asyncio objects directly: they are not thread-safe, and a plain `call_soon` from a foreign thread does not wake the selector, so the loop may sleep until an unrelated timer fires.
- `loop.call_soon_threadsafe(cb, *args)` queues a callback and writes to the loop's self-pipe to wake it.
- `asyncio.run_coroutine_threadsafe(coro, loop)` schedules a coroutine on a loop running in another thread and returns a `concurrent.futures.Future` the calling thread can block on with `.result(timeout)`.
- Going the other way, `asyncio.wrap_future(cf_future)` makes a `concurrent.futures.Future` awaitable.
- A **background loop thread** is the standard way for sync code (a Flask app, a Celery task, a legacy service) to use an async client with a long-lived connection pool.

```python
import asyncio
import threading
import time


class LoopThread:
    """Runs one event loop in a daemon thread so synchronous code can submit coroutines."""

    def __init__(self) -> None:
        self.loop = asyncio.new_event_loop()
        self.thread = threading.Thread(target=self.loop.run_forever, name="asyncio-loop", daemon=True)
        self.thread.start()

    def run(self, coro, timeout: float | None = None):
        return asyncio.run_coroutine_threadsafe(coro, self.loop).result(timeout)

    def close(self) -> None:
        self.loop.call_soon_threadsafe(self.loop.stop)
        self.thread.join()
        self.loop.close()


async def fetch(x: int) -> int:
    await asyncio.sleep(0.01)
    return x * 10


async def fetch_both() -> list[int]:
    return await asyncio.gather(fetch(1), fetch(2))   # gather() must be called on the loop's thread


bridge = LoopThread()
assert bridge.run(fetch(4), timeout=1) == 40          # sync caller, async work
assert bridge.run(fetch_both(), timeout=1) == [10, 20]
bridge.close()


async def wake_latency(thread_safe: bool) -> float:
    loop = asyncio.get_running_loop()
    queue: asyncio.Queue = asyncio.Queue()
    loop.call_later(0.5, lambda: None)                 # an unrelated timer 500 ms away

    def producer() -> None:
        time.sleep(0.05)
        if thread_safe:
            loop.call_soon_threadsafe(queue.put_nowait, "x")
        else:
            queue.put_nowait("x")                      # BUG: touches the loop from another thread

    thread = threading.Thread(target=producer)
    start = loop.time()
    thread.start()
    await queue.get()
    thread.join()
    return loop.time() - start


assert asyncio.run(wake_latency(thread_safe=True)) < 0.2
assert asyncio.run(wake_latency(thread_safe=False)) >= 0.45     # slept until the unrelated timer fired
```

The unsafe version "worked" but delivered the item about 500 ms late, because nothing woke the selector; without that timer it would hang forever.
In debug mode it is worse and more honest: `put_nowait` raises `RuntimeError: Non-thread-safe operation invoked on an event loop other than the current one` in the producer thread after the getter's Future was already marked done, and the consumer then hung forever, even inside `asyncio.timeout(0.3)` (observed; the process had to be killed).

**Follow-ups they ask:**

- "Why not `asyncio.run(...)` per call from sync code?"
  Each call creates and destroys a loop, so pooled connections (httpx, asyncpg) bound to the old loop break or are re-created every time; a long-lived loop thread or `asyncio.Runner` avoids that.
- "How do you shut down the loop thread cleanly?"
  Cancel outstanding tasks via `run_coroutine_threadsafe`, then `call_soon_threadsafe(loop.stop)`, join the thread, and close the loop, as `close()` does.

---

## AS18. How do you call async code from sync code and sync code from async code? Where do anyio and FastAPI fit?

| Direction | Tool | Notes |
| --- | --- | --- |
| Sync program entry point to async | `asyncio.run(main())` | Once per program |
| Sync code called repeatedly, needs a persistent loop | `asyncio.Runner`, or a background loop thread plus `run_coroutine_threadsafe` | Keeps connection pools alive (AS17) |
| Async to blocking sync function | `await asyncio.to_thread(f, ...)` | Default pool, context copied |
| Async to CPU-bound function | `await loop.run_in_executor(process_pool, f, ...)` | Pickled arguments |
| Library with both flavors | `httpx.Client` / `httpx.AsyncClient`, redis-py `redis` / `redis.asyncio` | Pick the flavor matching the caller |
| Framework code that must run on asyncio or trio | `anyio` | Same API over both backends |

- **anyio** is a structured-concurrency layer that runs on asyncio or trio; Starlette and therefore FastAPI are written against it.
- FastAPI runs a plain `def` endpoint or dependency through Starlette's `run_in_threadpool`, which calls `anyio.to_thread.run_sync`; that uses anyio's default **CapacityLimiter of 40 tokens**, so at most 40 sync endpoints run at once per worker process, independent of the asyncio default executor (verified in anyio 4.15.1 and Starlette 1.7.0 source).
- Raise or lower it at startup with `anyio.to_thread.current_default_thread_limiter().total_tokens = N`, sized to what the downstream (for example a DB pool) can take.

```python
import asyncio
import inspect
import os
import threading
import time

import anyio.to_thread
from starlette.concurrency import run_in_threadpool


async def main() -> None:
    limiter = anyio.to_thread.current_default_thread_limiter()
    assert limiter.total_tokens == 40                          # anyio default, used by Starlette/FastAPI
    assert "anyio.to_thread.run_sync" in inspect.getsource(run_in_threadpool)
    assert await run_in_threadpool(sum, [1, 2, 3]) == 6

    lock = threading.Lock()
    active = peak = 0

    def blocking() -> None:
        nonlocal active, peak
        with lock:
            active += 1
            peak = max(peak, active)
        time.sleep(0.05)
        with lock:
            active -= 1

    await asyncio.gather(*(asyncio.to_thread(blocking) for _ in range(64)))
    assert peak == min(32, (os.process_cpu_count() or 1) + 4)   # asyncio's default executor size
    print(f"anyio limiter: {limiter.total_tokens}, asyncio default executor peak: {peak}")


asyncio.run(main())
```

Output:

```text
anyio limiter: 40, asyncio default executor peak: 16
```

The 16 is `min(32, 12 + 4)` on the 12-core test machine; it differs per host.

**Follow-ups they ask:**

- "My FastAPI `def` endpoints stall at about 40 concurrent requests per worker: why?"
  That is the limiter; each blocked request holds a token.
  Raise it if the downstream can take it, or make the endpoint async with an async driver.
- "Can I call an async function from a sync FastAPI dependency?"
  Not with `asyncio.run` (a loop is already running in that process); use `anyio.from_thread.run(async_fn)` from the worker thread, or make the dependency async.
- See [FastAPI](04-FastAPI.md) A2 for `def` vs `async def` endpoints and [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md) for worker models.

---

## AS19. Async generators, `async for`, `async with`, `asynccontextmanager`, `aclosing`, and async comprehensions.

- `async for` calls `__aiter__`/`__anext__`; an `async def` with `yield` is an async generator (PEP 525), ideal for paginated APIs and streaming rows.
- `async with` calls `__aenter__`/`__aexit__`; `contextlib.asynccontextmanager` builds one from an async generator, the usual shape for acquiring a connection or a lock.
- Async comprehensions: `[x async for x in agen()]` and `[await f(x) for x in xs]` (the latter is still sequential).
- **Finalization pitfall**: breaking out of `async for` does not run the generator's `finally` immediately; the loop's async-generator hook schedules `aclose()` for later (or at shutdown), so a connection or lock stays held.
  Wrap the generator in `contextlib.aclosing(...)` (3.10+) to close it deterministically.

```python
import asyncio
from contextlib import aclosing, asynccontextmanager


async def paginate(log: list[str], pages: int = 5):
    log.append("open cursor")
    try:
        for page in range(pages):
            await asyncio.sleep(0)             # e.g. fetch the next page
            yield page
    finally:
        log.append("close cursor")


@asynccontextmanager
async def connection(log: list[str]):
    log.append("acquire")
    try:
        yield "conn"
    finally:
        log.append("release")


async def main() -> None:
    log: list[str] = []
    async for page in paginate(log):
        if page == 1:
            break
    assert log == ["open cursor"]              # finally has NOT run yet
    await asyncio.sleep(0)
    await asyncio.sleep(0)
    assert log == ["open cursor", "close cursor"]   # closed later by the loop's finalizer hook

    log.clear()
    async with aclosing(paginate(log)) as pages:
        async for page in pages:
            if page == 1:
                break
    assert log == ["open cursor", "close cursor"]   # closed deterministically on exit

    log.clear()
    assert [p async for p in paginate(log, 3)] == [0, 1, 2]
    async with connection(log) as conn:
        assert conn == "conn"
    assert log[-2:] == ["acquire", "release"]


asyncio.run(main())
```

**Follow-ups they ask:**

- "Why is this a production issue?"
  A generator that holds a DB connection or a lock and is abandoned mid-iteration keeps it until the finalizer runs; under load that exhausts the pool.
- "Can you `yield` inside `async with` in an async generator?"
  Yes, but the context stays open while the consumer holds the generator; the same aclosing rule applies.

---

## AS20. How do `contextvars` work with asyncio? Show a request-ID example.

- A `ContextVar` holds a value per **context**; each Task runs in its own copy of the context, **copied when the task is created** (`create_task`, `gather`, `TaskGroup`).
- Consequences: a child task sees the parent's values as of creation; changes made in the child are invisible to the parent and to siblings; a value set after `create_task` is not seen by that task.
- `asyncio.to_thread` copies the context into the worker thread; `loop.run_in_executor` does **not**, so request IDs vanish in executor logs unless you wrap the call with `contextvars.copy_context().run`.
- This is how per-request logging correlation works in FastAPI middleware (see [Testing, debugging, production](10-Testing-Debugging-Production.md) T7).

```python
import asyncio
import contextvars
import functools
import logging

request_id: contextvars.ContextVar[str] = contextvars.ContextVar("request_id", default="-")
lines: list[str] = []


class RequestIdFilter(logging.Filter):
    def filter(self, record: logging.LogRecord) -> bool:
        record.request_id = request_id.get()
        return True


class ListHandler(logging.Handler):
    def emit(self, record: logging.LogRecord) -> None:
        lines.append(self.format(record))


handler = ListHandler()
handler.addFilter(RequestIdFilter())
handler.setFormatter(logging.Formatter("[%(request_id)s] %(message)s"))
log = logging.getLogger("svc")
log.addHandler(handler)
log.setLevel(logging.INFO)


async def call_downstream(name: str) -> None:
    await asyncio.sleep(0.01)
    log.info("called %s", name)


async def handle_request(rid: str) -> None:
    request_id.set(rid)                           # e.g. set by middleware from X-Request-ID
    async with asyncio.TaskGroup() as tg:         # children inherit a copy of this context
        tg.create_task(call_downstream("users"))
        tg.create_task(call_downstream("orders"))
    await asyncio.to_thread(log.info, "in to_thread")      # context copied into the thread
    loop = asyncio.get_running_loop()
    await loop.run_in_executor(None, log.info, "in run_in_executor")   # context NOT copied
    ctx = contextvars.copy_context()
    await loop.run_in_executor(None, functools.partial(ctx.run, log.info, "executor with ctx.run"))


async def child_sets_value() -> None:
    request_id.set("child-changed")


async def main() -> None:
    await asyncio.gather(handle_request("req-1"), handle_request("req-2"))
    request_id.set("parent")
    await asyncio.create_task(child_sets_value())
    assert request_id.get() == "parent"           # the child's change stayed in the child's copy


asyncio.run(main())
assert sorted(lines) == sorted([
    "[req-1] called users", "[req-1] called orders", "[req-1] in to_thread",
    "[-] in run_in_executor", "[req-1] executor with ctx.run",
    "[req-2] called users", "[req-2] called orders", "[req-2] in to_thread",
    "[-] in run_in_executor", "[req-2] executor with ctx.run",
])
```

**Follow-ups they ask:**

- "Why not a global or thread-local for request ID?"
  Thousands of requests share one thread in asyncio, so a thread-local would be overwritten by whichever request ran last.
- "Can a child task pass a value back through a ContextVar?"
  No; return it, or put a mutable object in the var (and accept shared-state risks).

---

## AS21. Show asyncio streams: an echo server, a client, and backpressure with `drain()`.

- `asyncio.start_server(handler, host, port)` calls `handler(reader, writer)` as a new task per connection; `asyncio.open_connection` returns the same pair for a client.
- `writer.write(data)` only appends to the transport's buffer and never blocks; `await writer.drain()` suspends while the buffer is above the high-water mark (64 KiB by default), which is the **backpressure** that stops a fast producer from buffering unbounded data for a slow peer.
- Close with `writer.close()` then `await writer.wait_closed()`; `async with server:` closes the listening socket.

```python
import asyncio


async def handle_echo(reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
    try:
        while line := await reader.readline():      # b"" means the client closed its side
            writer.write(line.upper())
            await writer.drain()                    # respect the client's read speed
    finally:
        writer.close()
        await writer.wait_closed()


async def client(port: int, text: str) -> str:
    reader, writer = await asyncio.open_connection("127.0.0.1", port)
    writer.write(text.encode() + b"\n")
    await writer.drain()
    reply = await reader.readline()
    writer.close()
    await writer.wait_closed()
    return reply.decode().strip()


async def main() -> None:
    server = await asyncio.start_server(handle_echo, "127.0.0.1", 0)   # port 0: pick a free port
    port = server.sockets[0].getsockname()[1]
    async with server:
        replies = await asyncio.gather(*(client(port, f"msg {i}") for i in range(50)))
        assert replies == [f"MSG {i}" for i in range(50)]

        stalled = asyncio.Event()

        async def never_reads(reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
            await stalled.wait()                    # a slow consumer: reads nothing
            writer.close()

        slow_server = await asyncio.start_server(never_reads, "127.0.0.1", 0)
        slow_port = slow_server.sockets[0].getsockname()[1]
        reader, writer = await asyncio.open_connection("127.0.0.1", slow_port)
        writer.write(b"x" * 20_000_000)             # returns immediately: it only buffers
        assert writer.transport.get_write_buffer_size() > 0
        try:
            await asyncio.wait_for(writer.drain(), 0.2)
            raise AssertionError("drain should wait for the slow reader")
        except TimeoutError:
            pass                                    # backpressure: the producer is held here
        writer.transport.abort()
        stalled.set()
        slow_server.close()
        await slow_server.wait_closed()


asyncio.run(main())
```

**Follow-ups they ask:**

- "What if you never call `drain()`?"
  Memory grows with every write to a slow client; with many slow clients the process is OOM-killed.
- "Streams vs protocols?"
  Streams are the high-level, coroutine-based API; `Protocol` classes are callback-based and a bit faster, used by servers like uvicorn.
- "Framing?"
  `readline()`, `readexactly(n)` for length-prefixed messages, and `readuntil(sep)`; always cap sizes (`limit=`) to avoid memory attacks.

---

## AS22. How do you run subprocesses from asyncio?

- `asyncio.create_subprocess_exec(program, *args, stdout=PIPE, stderr=PIPE)` starts a process without blocking the loop; prefer it over `create_subprocess_shell` to avoid shell injection.
- Use `await proc.communicate(input)` to feed stdin and read both pipes concurrently; reading one pipe to the end while the child fills the other is the classic pipe-buffer deadlock.
- Bound it with `asyncio.timeout`, and on timeout `proc.kill()` then `await proc.wait()` so no zombie is left.

```python
import asyncio
import sys


async def run(code: str, timeout: float, stdin: bytes = b"") -> tuple[int, str, str]:
    proc = await asyncio.create_subprocess_exec(
        sys.executable, "-c", code,
        stdin=asyncio.subprocess.PIPE, stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.PIPE,
    )
    try:
        async with asyncio.timeout(timeout):
            out, err = await proc.communicate(stdin)
    except TimeoutError:
        proc.kill()
        await proc.wait()                     # reap it: no zombie
        raise
    return proc.returncode, out.decode().strip(), err.decode().strip()


async def main() -> None:
    results = await asyncio.gather(*(run(f"print({i} * {i})", 5) for i in range(4)))
    assert [r[1] for r in results] == ["0", "1", "4", "9"]

    code, out, _ = await run("import sys; print(sys.stdin.read().upper())", 5, stdin=b"hello")
    assert (code, out) == (0, "HELLO")

    code, _, err = await run("import sys; sys.exit('boom')", 5)
    assert code == 1 and err == "boom"

    try:
        await run("import time; time.sleep(10)", 0.3)
        raise AssertionError("should have timed out")
    except TimeoutError:
        pass


asyncio.run(main())
```

**Follow-up they ask:**

- "Why not `subprocess.run` inside `async def`?"
  It blocks the loop for the child's whole lifetime; if you must, wrap it with `asyncio.to_thread`.

---

## AS23. How do you bound concurrency and rate-limit calls in asyncio?

- **Concurrency limit** (how many in flight): `asyncio.Semaphore(n)` around each call; the tested fan-out is in [Coding round](11-Coding-Round.md) K12.
- **Rate limit** (how many per second): a token bucket; the thread-safe version is K11, and the asyncio version below needs no lock because the check-and-take has no `await` inside it.
- **Backpressure**: a bounded `asyncio.Queue` between stages (AS16), so the producer slows down instead of buffering.
- Respect the provider's `Retry-After` and 429s; a client-side limiter is a courtesy, the server's limit is the truth.

```python
# async_token_bucket.py
import asyncio


class AsyncTokenBucket:
    """Allows `rate` acquisitions per second with bursts up to `capacity`; single event loop only."""

    def __init__(self, rate: float, capacity: int) -> None:
        if rate <= 0 or capacity < 1:
            raise ValueError("rate must be > 0 and capacity >= 1")
        self.rate = rate
        self.capacity = capacity
        self.tokens = float(capacity)
        self.updated: float | None = None

    async def acquire(self) -> None:
        loop = asyncio.get_running_loop()
        while True:
            now = loop.time()
            if self.updated is not None:
                self.tokens = min(self.capacity, self.tokens + (now - self.updated) * self.rate)
            self.updated = now
            if self.tokens >= 1:              # check and take with no await in between: atomic here
                self.tokens -= 1
                return
            await asyncio.sleep((1 - self.tokens) / self.rate)
```

```python
# test_async_token_bucket.py
import asyncio

import pytest

from async_token_bucket import AsyncTokenBucket


def test_burst_then_steady_rate():
    async def main() -> list[float]:
        bucket = AsyncTokenBucket(rate=50, capacity=5)     # 5 immediately, then one every 20 ms
        loop = asyncio.get_running_loop()
        start = loop.time()
        stamps = []

        async def call() -> None:
            await bucket.acquire()
            stamps.append(loop.time() - start)

        await asyncio.gather(*(call() for _ in range(10)))
        return sorted(stamps)

    stamps = asyncio.run(main())
    assert all(t < 0.01 for t in stamps[:5])               # the burst
    assert 0.08 <= stamps[-1] < 0.3                         # the remaining 5 at 50/s take about 100 ms


def test_rejects_bad_config():
    with pytest.raises(ValueError):
        AsyncTokenBucket(rate=0, capacity=1)
```

**Follow-ups they ask:**

- "Rate limit across 8 worker processes?"
  An in-process bucket limits each process separately; use a shared limiter (Redis with a Lua script or a gateway) for a global limit; see [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md).
- "Semaphore or token bucket?"
  Semaphore when the constraint is capacity (connections, memory), bucket when it is a quota per time window; production clients often need both.

---

## AS24. Write an async retry with exponential backoff and jitter that respects an overall deadline.

- Retry only transient errors (timeouts, connection resets, 429, 503), never `CancelledError` and never client errors.
- Full jitter (`uniform(0, base * 2**attempt)`) spreads retries so clients do not stampede in sync.
- Put the whole operation under one deadline with `asyncio.timeout`, so retries cannot extend a request past its budget; the sync decorator version is [Coding round](11-Coding-Round.md) K4.

```python
# async_retry.py
import asyncio
import random
from collections.abc import Awaitable, Callable
from typing import TypeVar

T = TypeVar("T")


async def retry(
    op: Callable[[], Awaitable[T]],
    *,
    attempts: int = 5,
    base: float = 0.05,
    cap: float = 1.0,
    deadline: float = 2.0,
    retry_on: tuple[type[Exception], ...] = (ConnectionError, TimeoutError),
) -> T:
    """Run op() until it succeeds, with full-jitter backoff, all inside one overall deadline."""
    async with asyncio.timeout(deadline):          # the budget covers attempts and sleeps
        for attempt in range(attempts):
            try:
                return await op()
            except retry_on:                       # CancelledError is a BaseException: never caught here
                if attempt == attempts - 1:
                    raise
                await asyncio.sleep(random.uniform(0, min(cap, base * 2 ** attempt)))
    raise AssertionError("unreachable")
```

```python
# test_async_retry.py
import asyncio

import pytest

from async_retry import retry


def flaky(failures: int, exc: Exception):
    calls = {"n": 0}

    async def op() -> str:
        calls["n"] += 1
        if calls["n"] <= failures:
            raise exc
        return "ok"

    return op, calls


def test_succeeds_after_transient_failures():
    op, calls = flaky(2, ConnectionError("reset"))
    assert asyncio.run(retry(op, base=0.001)) == "ok"
    assert calls["n"] == 3


def test_gives_up_after_attempts():
    op, calls = flaky(10, ConnectionError("reset"))
    with pytest.raises(ConnectionError):
        asyncio.run(retry(op, attempts=3, base=0.001))
    assert calls["n"] == 3


def test_does_not_retry_other_errors():
    op, calls = flaky(1, ValueError("bad request"))
    with pytest.raises(ValueError):
        asyncio.run(retry(op))
    assert calls["n"] == 1


def test_overall_deadline_wins():
    async def hangs() -> str:
        await asyncio.sleep(10)
        return "never"

    with pytest.raises(TimeoutError):
        asyncio.run(retry(hangs, deadline=0.1))
```

**Follow-up they ask:**

- "Should the per-attempt timeout also exist?"
  Yes; add `async with asyncio.timeout(per_try):` around `op()` so one hung attempt does not consume the whole budget, and catch that `TimeoutError` as retryable.

---

## AS25. Why can asyncio handle 10,000 connections but not CPU work? What does a task cost? Is uvloop worth it?

- An idle connection in asyncio is a socket registered with kqueue or epoll plus a suspended coroutine (a few KB); the loop pays only when data arrives, so tens of thousands of mostly idle connections fit in one thread.
- CPU work gets no such benefit: there is one thread, so throughput is capped at one core, and every CPU-heavy step raises latency for all connections on that loop.
- Scale CPU by running **one process per core, each with its own loop** (uvicorn or gunicorn workers), and push heavy compute to a process pool or a job queue.
- **uvloop** replaces the loop with a libuv-based one; PyPI lists `cp314` wheels for uvloop 0.22.1 (checked 2026-09-30), and it installed and ran the benchmark below under 3.14.7.
  It speeds up loop and socket overhead, not your Python code; measure before claiming a win.

```python
import asyncio
import sys
import time
import tracemalloc

try:
    import uvloop
except ImportError:
    uvloop = None


async def noop() -> None:
    return None


async def per_task_cost(n: int, eager: bool) -> float:
    loop = asyncio.get_running_loop()
    loop.set_task_factory(asyncio.eager_task_factory if eager else None)
    start = time.perf_counter()
    await asyncio.gather(*[asyncio.create_task(noop()) for _ in range(n)])
    loop.set_task_factory(None)
    return (time.perf_counter() - start) / n * 1e6          # microseconds per task


async def idle_tasks(n: int, wait: float) -> tuple[float, float]:
    tracemalloc.start()
    start = time.perf_counter()
    await asyncio.gather(*(asyncio.sleep(wait) for _ in range(n)))
    elapsed = time.perf_counter() - start
    _, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    return elapsed, peak / n                               # wall time, bytes per task at peak


async def held_connections(n: int) -> float:
    async def handle(reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
        writer.write(b"hi\n")                             # greeting: proves the server accepted us
        writer.write(await reader.readline())
        await writer.drain()
        writer.close()
        await writer.wait_closed()

    server = await asyncio.start_server(handle, "127.0.0.1", 0)
    port = server.sockets[0].getsockname()[1]
    connect_slots = asyncio.Semaphore(64)                  # the OS accept backlog is small (128 on macOS)
    all_open = asyncio.Event()
    opened = 0

    async def client(i: int) -> int:
        nonlocal opened
        async with connect_slots:                          # at most 64 connections waiting to be accepted
            reader, writer = await asyncio.open_connection("127.0.0.1", port)
            assert await reader.readline() == b"hi\n"
        opened += 1
        if opened == n:
            all_open.set()
        await all_open.wait()                              # every connection is open at the same time
        writer.write(b"%d\n" % i)
        await writer.drain()
        reply = int(await reader.readline())
        writer.close()
        await writer.wait_closed()
        return reply

    start = time.perf_counter()
    assert await asyncio.gather(*(client(i) for i in range(n))) == list(range(n))
    elapsed = time.perf_counter() - start
    server.close()
    await server.wait_closed()
    return elapsed


async def main() -> None:
    lazy = min([await per_task_cost(100_000, eager=False) for _ in range(3)])
    eager = min([await per_task_cost(100_000, eager=True) for _ in range(3)])
    wall, per_task = await idle_tasks(10_000, 0.2)
    conns = await held_connections(5_000)                  # 5,000 client + 5,000 server sockets
    print(f"create+run+gather: {lazy:.2f} us/task lazy, {eager:.2f} us/task eager")
    print(f"10,000 tasks sleeping 0.2 s: {wall:.3f} s wall, ~{per_task:.0f} bytes/task peak")
    print(f"5,000 simultaneous connections echoed: {conns:.2f} s")


if __name__ == "__main__":
    loop_factory = uvloop.new_event_loop if uvloop and "--uvloop" in sys.argv else None
    print(f"{sys.version.split()[0]} loop={'uvloop' if loop_factory else 'asyncio'}")
    asyncio.run(main(), loop_factory=loop_factory)
```

Single-machine numbers (Apple M3 Pro, 12 cores, macOS; best of 3 for task cost; not a formal benchmark):

| Measurement | asyncio loop | uvloop 0.22.1 |
| --- | --- | --- |
| Create, run, and gather one trivial task (lazy) | 2.6 us | 2.0 us |
| Same with the eager task factory (AS26) | 1.6-1.7 us | 1.6 us |
| 10,000 tasks sleeping 0.2 s at once (tracemalloc on, which inflates wall time) | 0.29-0.31 s wall, ~1.2 KB per task at peak | 0.30 s wall, ~1.5 KB per task |
| 5,000 simultaneous connections (10,000 sockets in one process), each echoing one line | 0.54-0.65 s | 0.35-0.37 s |

The machine was shared with other jobs while measuring (load average ~30-45 on 12 cores), so treat these as rough; the shape is what matters: microseconds and about a kilobyte per task, 10,000 open sockets on one thread in well under a second, and uvloop helping socket-heavy work far more than task creation.

**Follow-ups they ask:**

- "How many workers per pod?"
  Roughly one per core for CPU-leaning services, and measure; for I/O-heavy async services, fewer workers with more concurrency each is fine, bounded by the DB pool (see [Docker, Kubernetes, CI/CD](09-Docker-Kubernetes-CICD-Cloud.md) and [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md)).
- "What limits 10k connections in practice?"
  File descriptor limits (`ulimit -n`), memory per connection in your app (buffers, parsed bodies), the accept backlog, and any per-request CPU; the loop itself is rarely the limit.

---

## AS26. What is the eager task factory (3.12+), and what can it break?

- Normally `create_task` only schedules the coroutine; its first step runs on a later loop iteration.
- With `loop.set_task_factory(asyncio.eager_task_factory)` (or `Task(..., eager_start=True)`, which `create_task` forwards in 3.14), the coroutine **starts running synchronously inside `create_task`** until its first real suspension; if it finishes without suspending (a cache hit), it never touches the loop at all.
- Win: less scheduling overhead for coroutines that often complete immediately (measured in AS25).
- Risk: code that assumed `create_task` returns before the coroutine runs now sees different ordering, side effects happen earlier, and exceptions can be set on the Task before the caller has even stored it.

```python
import asyncio


async def cached_lookup(log: list[str]) -> str:
    log.append("coroutine body ran")
    return "hit"                                   # completes without ever suspending


async def main() -> None:
    loop = asyncio.get_running_loop()
    for factory in (None, asyncio.eager_task_factory):
        loop.set_task_factory(factory)
        log: list[str] = []
        task = asyncio.create_task(cached_lookup(log))
        log.append("after create_task")
        print(f"{'eager' if factory else 'lazy '}: {log}, done={task.done()}")
        await task
    loop.set_task_factory(None)


asyncio.run(main())
```

Output:

```text
lazy : ['after create_task'], done=False
eager: ['coroutine body ran', 'after create_task'], done=True
```

**Follow-up they ask:**

- "Would you turn it on globally?"
  Only after running the test suite with it enabled; it is a behavior change, not a free speedup.

---

## AS27. How do you debug and introspect a running asyncio program?

- `asyncio.all_tasks()` and `asyncio.current_task()`; name tasks (`create_task(coro, name="fetch-42")`, `task.set_name`) so dumps are readable; `task.get_coro()`, `task.get_stack()`, `task.print_stack()`.
- **3.14**: `asyncio.capture_call_graph()`, `format_call_graph()`, and `print_call_graph()` show the chain of tasks awaiting each other from inside the process, and `python -m asyncio ps PID` and `python -m asyncio pstree PID` show all tasks of **another** running process.
  On macOS the external tools need elevated privileges; run without them, they fail with "The specified process cannot be attached to due to insufficient permissions" (observed here).
- "Task exception was never retrieved": a task failed and nobody awaited it or read `task.exception()`; the message is logged when the task is garbage collected, through `loop.call_exception_handler`, which you can replace to route it to your error tracker.
- Also useful: `py-spy dump --pid` for the native stack, debug mode for slow callbacks (AS12), and the `asyncio` REPL (`python -m asyncio`) for experiments with top-level `await`.

```python
import asyncio
import gc


async def leaf() -> str:
    await asyncio.sleep(0)
    return asyncio.format_call_graph()             # 3.14: who is awaiting whom, from here up


async def fetch(i: int) -> str:
    return await leaf()


async def handler() -> str:
    async with asyncio.TaskGroup() as tg:
        t = tg.create_task(fetch(0), name="fetch-0")
    return t.result()


async def boom() -> None:
    raise RuntimeError("nobody awaited me")


async def main() -> None:
    graph = await asyncio.create_task(handler(), name="handler")
    assert "Task(name='fetch-0'" in graph and "Task(name='handler'" in graph
    assert "async fetch()" in graph

    sleeper = asyncio.create_task(asyncio.sleep(1), name="sleeper")
    await asyncio.sleep(0)
    names = {t.get_name() for t in asyncio.all_tasks()}
    assert {"sleeper", asyncio.current_task().get_name()} <= names
    sleeper.cancel()

    loop = asyncio.get_running_loop()
    reported: list[str] = []
    loop.set_exception_handler(lambda _loop, ctx: reported.append(ctx["message"]))
    orphan = asyncio.create_task(boom())
    await asyncio.sleep(0)                         # it runs and fails; nobody awaits it
    del orphan
    gc.collect()
    assert reported == ["Task exception was never retrieved"]


asyncio.run(main())
```

Example `format_call_graph()` output from inside `leaf()` (paths shortened):

```text
* Task(name='fetch-0', id=0x...)
  + Call stack:
  |   File '...', line 7, in async leaf()
  |   File '...', line 11, in async fetch()
  + Awaited by:
    * Task(name='handler', id=0x...)
      + Call stack:
      |   File '.../asyncio/taskgroups.py', line 121, in async TaskGroup._aexit()
      |   File '.../asyncio/taskgroups.py', line 72, in async TaskGroup.__aexit__()
      |   File '...', line 15, in async handler()
      + Awaited by:
        * Task(name='Task-1', id=0x...)
          ...
```

**Follow-up they ask:**

- "A worker is stuck in production; what do you do first?"
  `py-spy dump --pid` (no restart, no code change) to see whether the main thread is inside `select` (idle, waiting on something) or inside a sync call (blocked loop); then, on 3.14 with the right privileges, `python -m asyncio pstree PID` to see which tasks are waiting on what.

---

## AS28. How do you test async code? pytest-asyncio modes, loop scope, and `IsolatedAsyncioTestCase`.

- **pytest-asyncio** (1.4.0 here) runs `async def` tests and fixtures on an event loop.
  `asyncio_mode = "strict"` (the default) requires `@pytest.mark.asyncio` and `@pytest_asyncio.fixture`; `"auto"` treats every async test and fixture as asyncio.
- **Loop scope**: each test gets a fresh loop by default (`asyncio_default_test_loop_scope = "function"`); set `@pytest.mark.asyncio(loop_scope="module")` or `"session"` when tests share an async resource like a connection pool, and set `asyncio_default_fixture_loop_scope` explicitly, because leaving it unset emits a deprecation warning.
  The old `event_loop` fixture was removed in pytest-asyncio 1.0; loop scope replaces it.
- **stdlib**: `unittest.IsolatedAsyncioTestCase` runs each test method on its own loop, with `asyncSetUp`/`asyncTearDown` and `addAsyncCleanup`.
- Use `unittest.mock.AsyncMock` for async dependencies, and prefer events, queues, and short timeouts over sleeps racing each other so tests are deterministic.

```toml
[tool.pytest.ini_options]
asyncio_mode = "strict"
asyncio_default_fixture_loop_scope = "function"
asyncio_default_test_loop_scope = "function"
```

```python
# test_async_testing.py
import asyncio
import unittest
from unittest.mock import AsyncMock

import pytest
import pytest_asyncio


async def get_user_name(client, user_id: int) -> str:
    async with asyncio.timeout(1):
        data = await client.fetch(f"/users/{user_id}")
    return data["name"].title()


@pytest_asyncio.fixture
async def client():
    mock = AsyncMock()
    mock.fetch.return_value = {"name": "ada lovelace"}
    yield mock
    await asyncio.sleep(0)                          # async teardown runs on the same loop


@pytest.mark.asyncio
async def test_get_user_name(client):
    assert await get_user_name(client, 1) == "Ada Lovelace"
    client.fetch.assert_awaited_once_with("/users/1")


@pytest.mark.asyncio
async def test_timeout_is_enforced():
    slow = AsyncMock()

    async def hang(_path):
        await asyncio.sleep(10)

    slow.fetch.side_effect = hang
    with pytest.raises(TimeoutError):
        async with asyncio.timeout(0.05):
            await get_user_name(slow, 1)


@pytest.mark.asyncio(loop_scope="module")
async def test_shares_the_module_loop():
    assert asyncio.get_running_loop().is_running()


class TestWithUnittest(unittest.IsolatedAsyncioTestCase):
    async def asyncSetUp(self) -> None:
        self.client = AsyncMock()
        self.client.fetch.return_value = {"name": "grace hopper"}

    async def test_name(self) -> None:
        self.assertEqual(await get_user_name(self.client, 2), "Grace Hopper")
```

**Follow-ups they ask:**

- "How do you test FastAPI async endpoints?"
  `httpx.AsyncClient(transport=httpx.ASGITransport(app=app))` inside an async test; see [Testing, debugging, production](10-Testing-Debugging-Production.md) T4.
- "Flaky async tests?"
  Usually sleeps racing each other or shared state across loops; replace sleeps with `Event`s, give each test its own loop unless sharing is intentional, and run with `--asyncio-debug` or `PYTHONASYNCIODEBUG=1` in CI.

---

## AS29. How do you shut down an asyncio service gracefully on SIGTERM?

- Kubernetes sends SIGTERM, waits `terminationGracePeriodSeconds` (30 s default), then SIGKILLs; in that window stop accepting work, finish or checkpoint in-flight work, close connections, and exit.
- In asyncio, register `loop.add_signal_handler(signal.SIGTERM, stop_event.set)` (Unix only), then: stop the listener, let workers drain with a deadline, cancel whatever is left, and await it so `finally` blocks run.
- `asyncio.run` already installs a SIGINT handler (3.11+) that cancels the main task, and on exit cancels remaining tasks, closes async generators, and shuts down the default executor; your code still has to drain its own queues.
- ASGI servers (uvicorn) do this for you and call the app's `lifespan` shutdown; see [FastAPI](04-FastAPI.md) A14.

```python
import asyncio
import os
import signal


async def serve(log: list[str]) -> None:
    loop = asyncio.get_running_loop()
    stop = asyncio.Event()
    loop.add_signal_handler(signal.SIGTERM, stop.set)

    queue: asyncio.Queue[int] = asyncio.Queue()
    for job in range(5):
        queue.put_nowait(job)

    async def worker() -> None:
        while True:
            job = await queue.get()
            try:
                await asyncio.sleep(0.02)                  # in-flight work
                log.append(f"done {job}")
            finally:
                queue.task_done()

    workers = [asyncio.create_task(worker()) for _ in range(2)]
    loop.call_later(0.01, os.kill, os.getpid(), signal.SIGTERM)   # simulate kubectl delete pod

    await stop.wait()
    log.append("SIGTERM received: stop accepting")
    try:
        async with asyncio.timeout(2):                     # drain budget below the grace period
            await queue.join()
    except TimeoutError:
        log.append("drain timed out")
    for w in workers:
        w.cancel()
    await asyncio.gather(*workers, return_exceptions=True)
    loop.remove_signal_handler(signal.SIGTERM)
    log.append("clean exit")


log: list[str] = []
asyncio.run(serve(log))
assert log[0] == "SIGTERM received: stop accepting"
assert sorted(log[1:6]) == [f"done {i}" for i in range(5)]
assert log[-1] == "clean exit"
```

**Follow-up they ask:**

- "Why did my pod take the full 30 s to die?"
  Usually PID 1 was a shell that did not forward SIGTERM (use `exec` or an init like tini), or a task swallowed `CancelledError`; see [Docker, Kubernetes, CI/CD](09-Docker-Kubernetes-CICD-Cloud.md).

---

## AS30. Predict the output / what goes wrong: asyncio trick snippets.

Every snippet below was run; the output shown is the real stdout.

**Snippet 1: ordering with `create_task` and `sleep(0)`.**

```python
import asyncio


async def worker(name: str) -> None:
    print(f"{name} start")
    await asyncio.sleep(0)
    print(f"{name} end")


async def main() -> None:
    t = asyncio.create_task(worker("A"))
    print("main after create_task")
    await worker("B")
    await t


asyncio.run(main())
```

Output:

```text
main after create_task
B start
A start
B end
A end
```

`create_task` only schedules A; `await worker("B")` runs B inline in the main task until B's `sleep(0)` suspends it, and only then does the loop run A's first step.

**Snippet 2: `gather` with and without `return_exceptions`.**

```python
import asyncio


async def ok() -> int:
    return 1


async def boom() -> None:
    raise ValueError("x")


async def main() -> None:
    print(await asyncio.gather(ok(), boom(), return_exceptions=True))
    try:
        await asyncio.gather(ok(), boom())
    except ValueError as exc:
        print("raised", repr(exc))


asyncio.run(main())
```

Output:

```text
[1, ValueError('x')]
raised ValueError('x')
```

**Snippet 3: awaiting in a loop is sequential.**

```python
import asyncio
import time


async def call() -> None:
    await asyncio.sleep(0.1)


async def main() -> None:
    start = time.perf_counter()
    for _ in range(3):
        await call()                                   # one at a time
    sequential = time.perf_counter() - start

    start = time.perf_counter()
    await asyncio.gather(*(call() for _ in range(3)))  # concurrently
    concurrent = time.perf_counter() - start
    print(sequential >= 0.3, concurrent < 0.2)


asyncio.run(main())
```

Output:

```text
True True
```

`async def` does not make a `for` loop concurrent; only Tasks do.

**Snippet 4: the lost task reference.**

```python
import asyncio
import gc


async def waits_forever() -> None:
    await asyncio.get_running_loop().create_future()


async def main() -> None:
    loop = asyncio.get_running_loop()
    loop.set_exception_handler(lambda _loop, ctx: print(ctx["message"]))
    asyncio.create_task(waits_forever())               # reference dropped
    await asyncio.sleep(0)
    gc.collect()
    print("main done")


asyncio.run(main())
```

Output:

```text
Task was destroyed but it is pending!
main done
```

Nothing but the task referenced the Future it was waiting on, so a GC pass reclaimed both (AS6).

**Snippet 5: a sync sleep inside a coroutine.**

```python
import asyncio
import time


async def heartbeat(stamps: list[float]) -> None:
    while True:
        stamps.append(time.perf_counter())
        await asyncio.sleep(0.01)


async def main() -> None:
    stamps: list[float] = []
    hb = asyncio.create_task(heartbeat(stamps))
    await asyncio.sleep(0.05)
    time.sleep(0.2)                                    # BUG: blocks every task, heartbeat included
    await asyncio.sleep(0.05)
    hb.cancel()
    gaps = [b - a for a, b in zip(stamps, stamps[1:])]
    print("longest heartbeat gap >= 0.2 s:", max(gaps) >= 0.2)


asyncio.run(main())
```

Output:

```text
longest heartbeat gap >= 0.2 s: True
```

**Snippet 6: a swallowed `CancelledError` defeats the timeout.**

```python
import asyncio


async def stubborn() -> str:
    try:
        await asyncio.sleep(0.3)
    except asyncio.CancelledError:
        print("swallowed the cancellation")
    await asyncio.sleep(0.1)                           # keeps working past the deadline
    return "finished anyway"


async def main() -> None:
    loop = asyncio.get_running_loop()
    start = loop.time()
    try:
        async with asyncio.timeout(0.05):
            result = await stubborn()
        print("no TimeoutError:", result)
    except TimeoutError:
        print("TimeoutError")
    print("deadline blown:", loop.time() - start >= 0.15)


asyncio.run(main())
```

Output:

```text
swallowed the cancellation
no TimeoutError: finished anyway
deadline blown: True
```

The timeout cancels the task and converts the `CancelledError` into `TimeoutError` only if that error reaches the `async with` boundary; here it never did (AS9).

**Snippet 7: a `threading.Lock` held across an `await`.**

```python
import asyncio
import threading

lock = threading.Lock()


async def critical() -> None:
    with lock:                                         # BUG: thread lock held across a suspension
        await asyncio.sleep(0.01)


async def main() -> None:
    holder = asyncio.create_task(critical())
    await asyncio.sleep(0)                             # holder now owns the lock and is suspended
    acquired = lock.acquire(timeout=0.2)               # what a second task would do: blocks the loop
    print("second acquire succeeded:", acquired)
    await holder


asyncio.run(main())
```

Output:

```text
second acquire succeeded: False
```

With a plain `lock.acquire()` the program deadlocks: the loop thread waits for a lock that only the loop thread (via the suspended holder task) can release.
Use `asyncio.Lock`.

**Snippet 8: a missing `await`.**

```python
import asyncio


async def save() -> None:
    print("saved")


async def main() -> None:
    save()                                             # BUG: coroutine created, never run
    print("done")


asyncio.run(main())
```

Output:

```text
done
```

Stderr additionally shows `RuntimeWarning: coroutine 'save' was never awaited`; linters (ruff's `RUF006` for dangling tasks, and type checkers flagging unused coroutines) catch this class of bug.

**Snippet 9: what `asyncio.run` does to unfinished tasks.**

```python
import asyncio

tasks: set[asyncio.Task] = set()


async def background() -> None:
    try:
        await asyncio.sleep(1)
        print("finished")
    except asyncio.CancelledError:
        print("cancelled by asyncio.run on exit")
        raise


async def main() -> None:
    tasks.add(asyncio.create_task(background()))
    await asyncio.sleep(0)
    print("main returns")


asyncio.run(main())
```

Output:

```text
main returns
cancelled by asyncio.run on exit
```

When `main` returns, `asyncio.run` cancels every task still pending; background work must be awaited (or drained, AS29) before returning.

**Snippet 10: a cancelled child inside `gather`.**

```python
import asyncio


async def main() -> None:
    child = asyncio.create_task(asyncio.sleep(1))
    group = asyncio.gather(child, asyncio.sleep(0.01, "ok"))
    await asyncio.sleep(0)
    child.cancel()
    try:
        await group
    except asyncio.CancelledError:
        print("CancelledError; gather.cancelled() =", group.cancelled())


asyncio.run(main())
```

Output:

```text
CancelledError; gather.cancelled() = False
```

A cancelled child is treated as a child that raised `CancelledError`, so the caller sees a cancellation it never requested; pass `return_exceptions=True` or use a `TaskGroup` if children can be cancelled independently.

---

## AS31. What asyncio pitfalls do you check for in code review?

- Blocking calls in `async def`: `requests`, `time.sleep`, sync DB drivers, `boto3`, big `json.loads`, CPU loops (AS12).
- `create_task` whose result is dropped; fire-and-forget without a reference set or `TaskGroup` (AS6).
- `except BaseException`, bare `except:`, or `except asyncio.CancelledError` without re-raise (AS10).
- `gather` without `return_exceptions` where siblings hold resources; prefer `TaskGroup` (AS7-AS8).
- Unbounded fan-out: `gather` over thousands of items with no semaphore; unbounded queues (AS16, AS23).
- No timeout on network calls, or per-hop timeouts that add up past the request budget (AS9).
- Awaiting in a loop where the calls are independent (AS30 snippet 3).
- `threading.Lock` or `queue.Queue.get()` in a coroutine; asyncio objects touched from threads without `call_soon_threadsafe` (AS15, AS17).
- `asyncio.run` inside library code or request handlers; `get_event_loop()` in new code (AS14).
- Async generators holding connections without `aclosing` (AS19).
- `run_in_executor` where context propagation matters (request IDs), instead of `to_thread` (AS20).
- `writer.write` in a loop without `await writer.drain()` (AS21).
- Sync endpoints in FastAPI competing for the 40-token anyio limiter while the DB pool is smaller or larger than that (AS18).
- Tests that sleep to "wait for" a task instead of awaiting an Event or the task itself (AS28).
- **[fill in: an asyncio bug you shipped or caught in review, how you found it, and the fix]**

---

## Go deeper

- Pack: [Python core](02-Python-Core.md) Y9-Y10, [FastAPI](04-FastAPI.md) A2, A14, A16, A18, [Flask](03-Flask.md) F13, [Testing, debugging, production](10-Testing-Debugging-Production.md) T7 and T10, [Coding round](11-Coding-Round.md) K4, K11, K12, [Concurrency fundamentals and threading](14-Concurrency-Fundamentals-and-Threading.md), [Multiprocessing and parallelism](15-Multiprocessing-and-Parallelism.md), [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md).
- Vault: [Asyncio inception](../Python_Zero_to_Godhood/Volume_03_Generators_Iterators_and_Async_Inception/Chapter_10_Asyncio_Inception_Pathlib_and_Enum/Chapter_10_Asyncio_Inception_Pathlib_and_Enum.md), [Native async/await](../Python_Zero_to_Godhood/Volume_03_Generators_Iterators_and_Async_Inception/Chapter_11_Native_Async_Await_and_New_Operators/Chapter_11_Native_Async_Await_and_New_Operators.md), [The GIL](../Python_Zero_to_Godhood/Chapter_10_CONCURRENCY_MECHANICS__THE_GLOBAL_INTERPRETER_LOCK.md), [CPU- and I/O-bound concurrency (26)](../Python_Zero_to_Godhood/Chapter_26_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md), [CPU- and I/O-bound concurrency (27)](../Python_Zero_to_Godhood/Chapter_27_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md), [Exception groups](../Python_Zero_to_Godhood/Chapter_19_EXCEPTION_GROUPS_AND_TRACEBACK_ENHANCEMENTS.md), [Context managers](../Python_Zero_to_Godhood/Chapter_54_Context_Managers_contextlib.md).
- OS background: [Threads and concurrency](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L2-Threads-and-Concurrency.md), [Thread design considerations](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L4-Thread-Design-Considerations.md), [Thread performance considerations](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L5-Thread-Performance-Considerations.md) (event-driven servers vs thread-per-request).
- Official: [Coroutines and tasks](https://docs.python.org/3/library/asyncio-task.html), [Event loop](https://docs.python.org/3/library/asyncio-eventloop.html), [Runners](https://docs.python.org/3/library/asyncio-runner.html), [Synchronization primitives](https://docs.python.org/3/library/asyncio-sync.html), [Queues](https://docs.python.org/3/library/asyncio-queue.html), [Streams](https://docs.python.org/3/library/asyncio-stream.html), [Subprocesses](https://docs.python.org/3/library/asyncio-subprocess.html), [Developing with asyncio](https://docs.python.org/3/library/asyncio-dev.html), [Call graph introspection](https://docs.python.org/3/library/asyncio-graph.html), [contextvars](https://docs.python.org/3/library/contextvars.html), [Remote debugging permissions](https://docs.python.org/3.14/howto/remote_debugging.html), [What's New in 3.14](https://docs.python.org/3/whatsnew/3.14.html), [PEP 492](https://peps.python.org/pep-0492/), [PEP 525](https://peps.python.org/pep-0525/), [PEP 654](https://peps.python.org/pep-0654/), [pytest-asyncio](https://pytest-asyncio.readthedocs.io/en/stable/), [anyio threads](https://anyio.readthedocs.io/en/stable/threads.html), [uvloop on PyPI](https://pypi.org/project/uvloop/).
