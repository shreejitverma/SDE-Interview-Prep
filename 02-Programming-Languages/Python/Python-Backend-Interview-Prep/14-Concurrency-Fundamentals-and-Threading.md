---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://docs.python.org/3/library/threading.html, https://docs.python.org/3/library/queue.html, https://docs.python.org/3/library/concurrent.futures.html, https://docs.python.org/3/howto/free-threading-python.html, https://docs.python.org/3/library/threadsafety.html, https://docs.python.org/3/whatsnew/3.14.html, https://peps.python.org/pep-0703/, https://peps.python.org/pep-0779/, https://docs.python.org/3/library/sys.html, https://docs.python.org/3/library/faulthandler.html, https://docs.python.org/3/library/contextvars.html, https://docs.python.org/3/library/collections.html, https://docs.python.org/3/library/logging.html]
---

# Concurrency fundamentals and threading

The concurrency vocabulary, the GIL and free-threaded Python, the `threading` API, every synchronization primitive, races, deadlocks, classic thread patterns, and `ThreadPoolExecutor`, at the depth an interviewer who "will examine concurrency heavily" goes to.
CT1 (vocabulary), CT3 (the comparison table), CT4 and CT6 (the GIL and why `x += 1` is not atomic), CT21 (reproduce and fix a race), and CT23 (deadlock) are near-certain; CT33 is the "predict the output" round.
This note goes deeper than [Python core](02-Python-Core.md) Y9 and Y10; processes live in [Multiprocessing and parallelism](15-Multiprocessing-and-Parallelism.md), the event loop in [Asyncio deep dive](16-Asyncio-Deep-Dive.md), and servers plus the concurrency coding round in [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md).
Every code sample here was run on 2026-09-30 under CPython 3.14.7 (default build), and the four marked "Runs on both builds" also under the free-threaded 3.14.6t build: 54 pytest tests passed (51 Python blocks executed as scripts, 41 of them with their printed output compared exactly).
Timings are single-machine numbers from an Apple M3 Pro laptop (12 cores) measured while it was busy, so read them as orders of magnitude and ratios, not absolutes.

---

## CT1. Define concurrency vs parallelism, synchronous vs asynchronous, blocking vs non-blocking, and CPU-bound vs I/O-bound. (must know)

> "Concurrency is about structure: several tasks are in progress over the same period and their steps interleave.
> Parallelism is about execution: several tasks run at the same instant on different cores.
> You can have concurrency without parallelism, which is exactly what asyncio, or threads under the GIL, give you on one core.
> Synchronous versus asynchronous is about the calling convention: does the caller wait for the result, or get a handle and carry on?
> Blocking versus non-blocking is about the operating system: can this call put my thread to sleep?
> CPU-bound versus I/O-bound is about where the time goes, and I decide it by measuring CPU time against wall time, not by guessing."

| Pair | Precise meaning | Python example | The confusion interviewers probe |
| --- | --- | --- | --- |
| Concurrency vs parallelism | Overlapping lifetimes vs simultaneous execution | asyncio on one thread vs 4 processes | "Threads in Python are parallel" (not for Python bytecode on the GIL build) |
| Synchronous vs asynchronous | Caller waits for the result vs gets a future, callback, or awaitable | `requests.get()` vs `pool.submit(requests.get, url)` or `await client.get(url)` | "Async means non-blocking" (an `async def` that calls `time.sleep` blocks the whole loop) |
| Blocking vs non-blocking | The call may suspend the OS thread vs returns immediately (`EAGAIN`) | `sock.recv()` vs `sock.setblocking(False)` | Orthogonal to sync/async: a thread pool gives an async API over blocking calls |
| CPU-bound vs I/O-bound | Time spent executing instructions vs waiting for disk, network, locks | hashing, JSON parsing, pricing loops vs DB queries, HTTP calls | Many "I/O" services are CPU-bound on serialization; measure it |
| Preemptive vs cooperative | The scheduler can interrupt you anywhere vs you yield explicitly | OS threads vs asyncio tasks at `await` | Under the GIL, threads are still preempted (see CT5) |

```text
Concurrency on one core: A, B, C interleave; total work is not faster, waiting overlaps.
core 0 : [A][B][A][C][B][A][C]

Parallelism on three cores: work itself runs at the same time.
core 0 : [A A A A A A A]
core 1 : [B B B B B B B]
core 2 : [C C C C C C C]

Two I/O-bound threads under the GIL: only one runs bytecode at a time, but the waits overlap.
T1  : [run]......wait on socket......[run]
T2  :      [run]......wait on DB.........[run]
GIL :  T1   T2                        T1   T2
```

How to tell CPU-bound from I/O-bound: compare process CPU time with wall-clock time for one call.

```python
import time


def cpu_share(fn) -> float:
    """CPU seconds divided by wall seconds: near 1.0 is CPU-bound, near 0.0 is waiting."""
    wall_start, cpu_start = time.perf_counter(), time.process_time()
    fn()
    wall = time.perf_counter() - wall_start
    cpu = time.process_time() - cpu_start
    return cpu / wall


io_bound = cpu_share(lambda: time.sleep(0.2))               # stands in for a DB call or HTTP request
cpu_bound = cpu_share(lambda: sum(i * i for i in range(2_000_000)))
print(f"sleep: {io_bound:.2f} CPU/wall, arithmetic loop: {cpu_bound:.2f} CPU/wall")
assert io_bound < 0.1 and cpu_bound > 10 * io_bound   # relative check: a loaded machine lowers both
```

Blocking versus non-blocking at the socket level, which is what asyncio builds on:

```python
import socket

a, b = socket.socketpair()
b.setblocking(False)
try:
    b.recv(1024)                    # nothing to read yet: a non-blocking socket refuses to wait
except BlockingIOError as exc:
    print("non-blocking recv:", type(exc).__name__)
a.sendall(b"ping")
b.settimeout(1.0)                   # blocking again, but bounded: the production default you want
print("blocking recv:", b.recv(1024))
a.close()
b.close()
```

Output:

```text
non-blocking recv: BlockingIOError
blocking recv: b'ping'
```

**Follow-ups they ask:**

- "Is asyncio parallel?"
  No: one thread runs one coroutine at a time; it gives concurrency for I/O waits, never CPU parallelism.
- "Is a thread pool synchronous or asynchronous?"
  `pool.submit()` is an asynchronous API (you get a future); the work inside is ordinary blocking code on another thread.
- "Give me a service that looks I/O-bound but is not."
  A JSON API returning large payloads: the DB query takes 5 ms but `json.dumps` and Pydantic validation take 40 ms of CPU.
  [fill in: a service from your own work where profiling showed CPU where you expected I/O]

---

## CT2. Compare preemptive and cooperative multitasking, and processes, threads, and coroutines. What does each cost?

> "Threads are scheduled preemptively by the OS: a thread can be interrupted between any two instructions, which is why shared state needs locks.
> Coroutines are cooperative: an asyncio task only gives up control at an `await`, so code between awaits cannot be interleaved with another task, but one task that never awaits starves all of them.
> A process has its own address space, so it is isolated and can crash or be killed alone, but it costs tens of milliseconds to spawn and data must be copied or pickled between processes.
> Threads share memory, cost tens of microseconds to create, and a crash or a stuck thread affects the whole process.
> Coroutines are the cheapest: a pending task is under a kilobyte, so you can have a hundred thousand."

| | Process | Thread | Coroutine (asyncio task) |
| --- | --- | --- | --- |
| Scheduled by | OS, preemptive | OS, preemptive (plus the GIL hand-off on the default build) | Event loop, cooperative at `await` |
| Memory | Own address space; tens of MB RSS for a Python interpreter | Shared heap; 16 MiB virtual stack reserved on macOS, about 35 KiB resident when parked | Shared heap; about 840 B traced per pending task |
| Create cost (measured below) | about 64 to 77 ms (`spawn`) | about 29 to 58 us start plus join | about 2.5 to 3 us per task via `gather` |
| Hand-off cost (measured below) | IPC: pickle plus pipe or socket | about 2 to 5 us through `queue.SimpleQueue` | about 14 to 20 us through `asyncio.Queue` |
| Isolation | Full: a segfault or leak kills one worker | None: one thread can corrupt shared state or crash the process | None, plus one blocking call stalls every task |
| Can be killed | Yes (`terminate`, `kill`) | No (see CT15) | Yes, by cancellation at the next `await` |
| CPU parallelism | Yes | Only on the free-threaded build or in GIL-releasing C code | No |

The per-switch cost surprises people: a thread hand-off through `SimpleQueue` was faster than a task hand-off through `asyncio.Queue`, because each asyncio hand-off goes through a future and an event-loop iteration.
The asyncio advantage is memory and scale (thousands of idle connections), not raw switch latency.

```python
import asyncio
import ctypes
import multiprocessing as mp
import os
import queue
import subprocess
import sys
import threading
import time
import tracemalloc


def noop() -> None:
    pass


def per_item(fn, n: int) -> float:
    start = time.perf_counter()
    fn(n)
    return (time.perf_counter() - start) / n


def start_join_threads(n: int) -> None:
    for _ in range(n):
        t = threading.Thread(target=noop)
        t.start()
        t.join()


def gather_tasks(n: int) -> None:
    async def anoop() -> None:
        pass

    async def main() -> None:
        await asyncio.gather(*(anoop() for _ in range(n)))

    asyncio.run(main())


def spawn_processes(n: int) -> None:
    ctx = mp.get_context("spawn")
    for _ in range(n):
        p = ctx.Process(target=noop)
        p.start()
        p.join()


def thread_handoff(n: int) -> None:
    ping, pong = queue.SimpleQueue(), queue.SimpleQueue()
    echo = threading.Thread(target=lambda: [pong.put(ping.get()) for _ in range(n)])
    echo.start()
    for i in range(n):
        ping.put(i)
        pong.get()
    echo.join()


def task_handoff(n: int) -> None:
    async def main() -> None:
        ping, pong = asyncio.Queue(), asyncio.Queue()

        async def echo() -> None:
            for _ in range(n):
                await pong.put(await ping.get())

        echo_task = asyncio.create_task(echo())
        for i in range(n):
            await ping.put(i)
            await pong.get()
        await echo_task

    asyncio.run(main())


def current_rss_bytes() -> int:
    ps = subprocess.run(["ps", "-o", "rss=", "-p", str(os.getpid())], capture_output=True, text=True, check=True)
    return int(ps.stdout) * 1024                      # ps reports KiB on macOS and Linux


def resident_bytes_per_parked_thread(n: int) -> float:
    stop = threading.Event()
    before = current_rss_bytes()
    parked = [threading.Thread(target=stop.wait) for _ in range(n)]
    for t in parked:
        t.start()
    after = current_rss_bytes()
    stop.set()
    for t in parked:
        t.join()
    return (after - before) / n


def traced_bytes_per_pending_task(n: int) -> float:
    async def main() -> float:
        stop = asyncio.Event()
        tracemalloc.start()
        pending = [asyncio.create_task(stop.wait()) for _ in range(n)]
        await asyncio.sleep(0)
        size, _ = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        stop.set()
        await asyncio.gather(*pending)
        return size / n

    return asyncio.run(main())


def macos_thread_stack_mib() -> float:
    libc = ctypes.CDLL(None)
    libc.pthread_self.restype = ctypes.c_void_p
    libc.pthread_get_stacksize_np.argtypes = [ctypes.c_void_p]
    libc.pthread_get_stacksize_np.restype = ctypes.c_size_t
    out: list[int] = []
    t = threading.Thread(target=lambda: out.append(libc.pthread_get_stacksize_np(libc.pthread_self())))
    t.start()
    t.join()
    return out[0] / 2**20


if __name__ == "__main__":
    print(f"thread start+join     : {per_item(start_join_threads, 2000) * 1e6:7.1f} us")
    print(f"asyncio task (gather) : {per_item(gather_tasks, 100_000) * 1e6:7.1f} us")
    print(f"spawn process         : {per_item(spawn_processes, 5) * 1e3:7.1f} ms")
    print(f"thread hand-off       : {per_item(thread_handoff, 20_000) / 2 * 1e6:7.1f} us")
    print(f"asyncio hand-off      : {per_item(task_handoff, 20_000) / 2 * 1e6:7.1f} us")
    print(f"resident per thread   : {resident_bytes_per_parked_thread(1000) / 1024:7.0f} KiB")
    print(f"traced per task       : {traced_bytes_per_pending_task(10_000):7.0f} B")
    if sys.platform == "darwin":
        print(f"thread stack reserved : {macos_thread_stack_mib():7.1f} MiB")
```

Single-machine numbers: one of three runs on 2026-09-30 on an Apple M3 Pro laptop (12 cores: 6 performance, 6 efficiency) under CPython 3.14.7, taken while the machine was busy (load average above 20):

```text
thread start+join     :    28.9 us
asyncio task (gather) :     2.5 us
spawn process         :    63.9 ms
thread hand-off       :     1.8 us
asyncio hand-off      :    13.9 us
resident per thread   :      35 KiB
traced per task       :     840 B
thread stack reserved :    16.0 MiB
```

Across the three runs: thread start+join 29 to 58 us, task 2.5 to 3.0 us, process spawn 64 to 77 ms, thread hand-off 1.8 to 5.2 us, asyncio hand-off 14 to 20 us; the memory figures did not change.

**Follow-ups they ask:**

- "Why do threads cost so much less memory than their stack size?"
  The stack is reserved virtual memory; only touched pages become resident.
  You still hit limits: address space on 32-bit, `ulimit -u` or cgroup `pids.max` in containers, and scheduler overhead with thousands of runnable threads.
- "Is a thread switch in Python an OS context switch?"
  Yes, and on the default build it is also a GIL hand-off, which is why a CPU-bound thread delays an I/O thread's wake-up (CT5).
- "Where does a coroutine's state live?"
  In its frame object on the heap, not on a C stack, which is why it can be suspended cheaply and why deep recursion inside a coroutine still uses the C stack of the one loop thread.

---

## CT3. Compare threading, multiprocessing, asyncio, subinterpreters, and free-threading in one table. (must know)

> "I pick by workload.
> Blocking I/O with sync libraries: a thread pool.
> Thousands of concurrent connections with async libraries: asyncio.
> CPU-bound pure Python: processes today, and on 3.14 subinterpreters or the free-threaded build when my dependencies support it.
> Numeric work: NumPy or another C extension that releases the GIL, and then threads are fine.
> A web service already scales CPU with worker processes, so inside each worker I only need I/O concurrency."

| | `threading` (default build) | `multiprocessing` / `ProcessPoolExecutor` | `asyncio` | Subinterpreters (3.14 `InterpreterPoolExecutor`) | Free-threaded build (`python3.14t`) |
| --- | --- | --- | --- | --- | --- |
| CPU parallelism for Python code | No (one GIL) | Yes | No | Yes (one GIL per interpreter) | Yes |
| Best for | Blocking I/O, GIL-releasing C code | CPU-bound pure Python, isolation | Many sockets, streaming, fan-out | CPU-bound work with small, shareable inputs | CPU-bound threads sharing memory |
| Memory sharing | Everything shared | Nothing by default; pickling, `shared_memory`, managers | Everything shared, single thread | Isolated; share via `concurrent.interpreters` queues or pickling | Everything shared |
| Unit cost | ~30-60 us, ~35 KiB resident | ~65-75 ms spawn, MBs each | ~3 us, under 1 KiB | Between threads and processes | Same as threads |
| Failure isolation | None | Strong | None | Partial (same process, separate state) | None |
| Main pitfalls | Races, deadlocks, no kill | Pickling, start methods, fork after threads | Blocking calls stall the loop, cancellation | Extension module support, young API | Races that the GIL hid, extension compatibility |
| Deep dive | This note | [Multiprocessing](15-Multiprocessing-and-Parallelism.md) | [Asyncio](16-Asyncio-Deep-Dive.md) | [Multiprocessing](15-Multiprocessing-and-Parallelism.md) | CT9, CT10 |

```text
Is the time spent waiting (I/O) or computing (CPU)?
|
+-- waiting --> Are the client libraries async (httpx, asyncpg, aiokafka)?
|               +-- yes, and many connections --> asyncio
|               +-- no, or tens of calls ------> ThreadPoolExecutor
|
+-- computing --> Is the hot loop in NumPy or another GIL-releasing extension?
                  +-- yes --> threads are fine
                  +-- no ---> ProcessPoolExecutor (or 3.14t threads / InterpreterPoolExecutor if deps allow)
```

The three executors share one API, so switching is a one-line change once the work function is picklable:

```python
from concurrent.futures import InterpreterPoolExecutor, ProcessPoolExecutor, ThreadPoolExecutor


def square(x: int) -> int:
    return x * x


if __name__ == "__main__":
    for executor in (ThreadPoolExecutor, ProcessPoolExecutor, InterpreterPoolExecutor):
        with executor(max_workers=2) as pool:
            print(executor.__name__, list(pool.map(square, range(5))))
```

Output:

```text
ThreadPoolExecutor [0, 1, 4, 9, 16]
ProcessPoolExecutor [0, 1, 4, 9, 16]
InterpreterPoolExecutor [0, 1, 4, 9, 16]
```

**Follow-ups they ask:**

- "Why not always use processes?"
  Spawn cost, per-process memory, and pickling every argument and result; for small tasks the overhead exceeds the work.
- "Why not always use asyncio?"
  It needs async libraries end to end, and one blocking call or CPU-heavy function stalls every connection (see [Testing, debugging, production](10-Testing-Debugging-Production.md) T10).
- "Can you mix them?"
  Yes, and production services do: asyncio in the request path, `asyncio.to_thread` for a sync SDK, and a process pool for CPU-heavy steps (CT30).

---

## CT4. What exactly does the GIL protect, and what does it not protect? (must know)

> "The GIL is a mutex inside the default CPython build: a thread must hold it to execute bytecode or touch Python objects through the C API.
> It exists so that the interpreter's own bookkeeping, above all reference counts, the memory allocator, and the internals of dicts and lists, never sees two threads at once.
> It does not protect my program's invariants.
> It is released and re-acquired between bytecodes, so any operation that takes more than one step, like read-modify-write or check-then-act, can be interleaved.
> So the GIL makes the interpreter thread-safe, not my code."

| Protected by the GIL (default build) | Not protected |
| --- | --- |
| Reference counts (`ob_refcnt` increments are plain, non-atomic adds) | Multi-statement invariants (debit one account, credit another) |
| pymalloc arenas and free lists | Read-modify-write (`x += 1`, `d[k] += 1`, `lst[i] = lst[i] + 1`) |
| Internal consistency of one `dict`, `list`, `set` operation | Check-then-act (`if k not in cache: cache[k] = load()`) |
| Interpreter-wide state: GC, import machinery, the eval loop | Iteration while another thread mutates the container |
| C extensions that never release it | C extensions that release it and then touch shared buffers |
| | Anything across processes, or across machines |

A compound invariant breaks even though every single statement is "safe"; the events force the observer to run at the worst moment so the result is deterministic:

```python
import threading

accounts = {"alice": 100, "bob": 0}
debited = threading.Event()
observed = threading.Event()
seen: list[int] = []


def transfer(amount: int) -> None:
    accounts["alice"] -= amount           # each statement is safe for the dict itself...
    debited.set()                         # (force the auditor to run exactly here)
    observed.wait(timeout=5)
    accounts["bob"] += amount             # ...but the invariant "total == 100" spans two statements


def audit() -> None:
    debited.wait(timeout=5)
    seen.append(sum(accounts.values()))
    observed.set()


threads = [threading.Thread(target=transfer, args=(30,)), threading.Thread(target=audit)]
for t in threads:
    t.start()
for t in threads:
    t.join()
print("total seen mid-transfer:", seen[0], "final:", sum(accounts.values()))
```

Output:

```text
total seen mid-transfer: 70 final: 100
```

The fix is one lock that both `transfer` and `audit` take, so the auditor can only observe states before or after a whole transfer.

**Follow-ups they ask:**

- "Why not fine-grained locks per object instead of a GIL?"
  That is what the free-threaded build does (PEP 703): biased reference counting, per-object locks, and mimalloc.
  The cost was single-threaded speed and C-extension compatibility, which is why it took until 3.13 and is still optional.
- "Does the GIL make C extensions thread-safe?"
  Only while they hold it; code between `Py_BEGIN_ALLOW_THREADS` and `Py_END_ALLOW_THREADS` must not touch Python objects.
- "Is there one GIL per process?"
  One per interpreter; since 3.12 subinterpreters can each have their own (PEP 684), which is what `InterpreterPoolExecutor` uses.

---

## CT5. When is the GIL released, and what is the switch interval?

> "A thread releases the GIL when it makes a blocking call: socket and file I/O, `time.sleep`, `select`, waiting on a lock, waiting for a subprocess.
> Well-written C extensions release it around long computations: hashlib on buffers over 2 KiB, zlib, NumPy ufuncs and BLAS, many database drivers.
> And a CPU-bound thread is forced to offer it up: every 5 ms by default another waiting thread sets a drop request, and the running thread hands the GIL over at its next safe point.
> That 5 ms is `sys.getswitchinterval()`, and it is why a CPU-heavy thread adds milliseconds of latency to every I/O thread in the same process."

| Released | Held |
| --- | --- |
| `socket.recv/send/accept`, file `read/write`, `select`, `time.sleep` | Pure-Python loops, comprehensions, `json` encoding of Python objects |
| `Lock.acquire` / `Event.wait` / `Queue.get` while blocked | Most `dict`/`list`/`str` methods |
| `hashlib` updates on data of 2048 bytes or more (`HASHLIB_GIL_MINSIZE` in CPython's `Modules/hashlib.h`) | `hashlib` on tiny inputs (the release costs more than the hash) |
| `zlib`, `bz2`, `lzma` compression, many NumPy operations, `sqlite3` statement execution | Code in C extensions that never call `Py_BEGIN_ALLOW_THREADS` |
| `subprocess` waits, `os.waitpid` | `re` matching (it runs with the GIL held) |
| Forced: a pending drop request, honored at check points such as function entry and loop back-edges | Between those check points, bytecodes run uninterrupted |

The switch interval, measured: a pure-Python busy thread delays a 1 ms `sleep` by about one switch interval, and shrinking the interval shrinks the delay.

```python
import statistics
import sys
import threading
import time


def spin(stop: threading.Event) -> None:
    while not stop.is_set():
        pass                                  # pure-Python busy loop: holds the GIL except at forced switches


def extra_wake_latency_ms(samples: int = 50) -> float:
    extra = []
    for _ in range(samples):
        start = time.perf_counter()
        time.sleep(0.001)                     # ask for 1 ms; the excess includes waiting to get the GIL back
        extra.append((time.perf_counter() - start - 0.001) * 1000)
    return statistics.median(extra)


print(f"switch interval: {sys.getswitchinterval() * 1000:.1f} ms")
print(f"no CPU hog          : {extra_wake_latency_ms():.2f} ms extra")
for interval in (0.005, 0.0005):
    sys.setswitchinterval(interval)
    stop = threading.Event()
    hog = threading.Thread(target=spin, args=(stop,))
    hog.start()
    print(f"CPU hog, {interval * 1000:.1f} ms interval: {extra_wake_latency_ms():.2f} ms extra")
    stop.set()
    hog.join()
sys.setswitchinterval(0.005)
```

Single-machine numbers, Apple M3 Pro laptop (12 cores: 6 performance, 6 efficiency), CPython 3.14.7 (three runs gave the same values to within 0.01 ms):

```text
switch interval: 5.0 ms
no CPU hog          : 0.27 ms extra
CPU hog, 5.0 ms interval: 6.54 ms extra
CPU hog, 0.5 ms interval: 0.91 ms extra
```

Under the free-threaded 3.14.6t build with the GIL off, the hog added nothing (0.26 ms extra in both rows): there is no GIL to wait for.

**Follow-ups they ask:**

- "Should I lower the switch interval in production?"
  Rarely: more switches mean more GIL hand-offs and lower throughput for CPU threads.
  The real fix is to move the CPU work out of the latency-sensitive process (a process pool or a separate service).
- "Why is the idle overhead not zero?"
  OS timer slack; macOS rounds short sleeps up, which is why measurements need a baseline.
- "Does a thread blocked in `sock.recv()` hold the GIL?"
  No; it released the GIL before the system call and re-acquires it when data arrives.

---

## CT6. Why is `x += 1` not atomic, even with the GIL? Show the bytecode. (must know)

> "Because it is several bytecodes: load the value, add, store it back.
> If another thread runs between the load and the store, both threads write back the same old value plus one and an update is lost.
> `counter[k] += 1` is worse: it is a subscript load, an add, and a subscript store, and each of those can call Python code if the key or container is a user type.
> On the free-threaded build this loses updates immediately.
> On the default build the naive loop rarely shows it (no lost updates in my runs on 3.12, 3.13, and 3.14), because the interpreter only switches threads at a few check points such as function entry and loop back-edges.
> That is an implementation detail, and any Python-level call between the read and the write brings the race back."

```python
import dis

counter = 0


def bump() -> None:
    global counter
    counter += 1


def bump_key(d: dict, k: str) -> None:
    d[k] += 1


dis.dis(bump)
dis.dis(bump_key)
```

Output (CPython 3.14.7; opcode names change between versions):

```text
  6           RESUME                   0

  8           LOAD_GLOBAL              0 (counter)
              LOAD_SMALL_INT           1
              BINARY_OP               13 (+=)
              STORE_GLOBAL             0 (counter)
              LOAD_CONST               1 (None)
              RETURN_VALUE
 11           RESUME                   0

 12           LOAD_FAST_BORROW_LOAD_FAST_BORROW 1 (d, k)
              COPY                     2
              COPY                     2
              BINARY_OP               26 ([])
              LOAD_SMALL_INT           1
              BINARY_OP               13 (+=)
              SWAP                     3
              SWAP                     2
              STORE_SUBSCR
              LOAD_CONST               1 (None)
              RETURN_VALUE
```

The bytecode is a load, a `BINARY_OP (+=)`, and a store; nothing makes that sequence indivisible.
Measured on both builds with four threads each doing a million increments:

```python
# Runs on both builds: python3.14 (GIL) and python3.14t (free-threaded).
import sys
import threading

N = 1_000_000
counter = 0


def bump_many() -> None:
    global counter
    for _ in range(N):
        counter += 1


threads = [threading.Thread(target=bump_many) for _ in range(4)]
for t in threads:
    t.start()
for t in threads:
    t.join()
gil = sys._is_gil_enabled()
print(f"GIL {'on' if gil else 'off'}: counter={counter:,} expected={4 * N:,}")
if not gil:
    assert counter < 4 * N                # free-threaded: updates were lost in every run observed
```

Three runs each on 2026-09-30, Apple M3 Pro laptop (12 cores: 6 performance, 6 efficiency):

```text
GIL on: counter=4,000,000 expected=4,000,000     (3.14.7 default build, every run; also 3.12.13 and 3.13.15)
GIL off: counter=1,034,046 expected=4,000,000    (3.14.6 free-threaded build; 1,010,158 to 1,161,710 over six runs)
```

So the naive demo "works" on the default build and fails badly on the free-threaded one.
Neither result is a guarantee; CT21 shows how to make the race reliable on the default build.

**Follow-ups they ask:**

- "So on 3.14 with the GIL, I do not need a lock for a counter?"
  You do: the behavior is not documented, it changes if `+=` touches a property, a `__getitem__`, or a `__hash__` written in Python, and it breaks on the free-threaded build.
- "What is atomic then?"
  See CT7; the short answer is "a single C-level operation on a built-in type", and you should not build on it.
- "How would you prove a race exists?"
  Force the interleaving with a `Barrier` or `Event` between the read and the write, or shrink `sys.setswitchinterval` and add a Python call in between, then run it under the free-threaded build too.

---

## CT7. Which operations are effectively atomic under the GIL, and why is relying on that a bad idea?

> "Single operations on built-in types that run entirely in C are effectively atomic: `list.append`, `list.pop()`, `dict[key] = value` with a str or int key, `dict.setdefault`, `dict.pop(key, default)`, `deque.append` and `popleft`.
> Anything with two steps is not: read-modify-write, check-then-act, and iteration.
> I do not rely on it because it is an implementation property, not a language guarantee, it silently stops holding when a key's `__hash__` or `__eq__` is Python code, and a reviewer cannot see it.
> The 3.14 docs now document per-type guarantees for the free-threaded build and still recommend an explicit lock."

| Effectively atomic (one C operation) | Not atomic |
| --- | --- |
| `lst.append(x)`, `lst.pop()`, `lst[i]`, `lst[i] = x`, `lst.clear()` | `lst[i] = lst[i] + 1`, `if lst: lst.pop()` |
| `d[k]`, `d.get(k)`, `k in d`, `d[k] = v`, `del d[k]` (built-in key types) | `d[k] = d[k] + 1`, `if k in d: del d[k]` |
| `d.setdefault(k, v)`, `d.pop(k, None)`, `d.popitem()`, `d.copy()` | `for k, v in d.items(): ...` while another thread writes |
| `s.add(x)`, `s.discard(x)`, `s.pop()` | `if x not in s: s.add(x); count += 1` |
| `deque.append/appendleft/pop/popleft` (documented thread-safe) | `if dq: dq.popleft()` |
| `next(it)` on a C iterator such as `itertools.count` (default build only) | Sharing any iterator across threads (the free-threading HOWTO says it is not thread-safe) |

"Atomic" operations run your code when keys are user types, and every Python-level call is a place where a thread switch can happen:

```python
calls: list[str] = []


class Key:
    def __hash__(self) -> int:
        calls.append("hash")
        return 1

    def __eq__(self, other: object) -> bool:
        calls.append("eq")
        return True


d = {Key(): 1}
calls.clear()
d[Key()] = 2                              # one "atomic" store...
print(calls)                              # ...ran two Python functions in the middle of it

cache: dict[str, str] = {}
assert cache.setdefault("cfg", "first") == "first"
assert cache.setdefault("cfg", "second") == "first"   # insert-if-absent and read back, in one call
assert cache.pop("missing", None) is None              # instead of: if k in d: del d[k]
```

Output:

```text
['hash', 'eq']
```

**Follow-ups they ask:**

- "Then why does the standard library rely on `deque` being thread-safe?"
  Because `collections.deque` documents it ("thread-safe, memory efficient appends and pops from either side"), and `queue.Queue` is built on a deque plus a lock and conditions.
  Documented guarantees are fine to rely on; accidental ones are not.
- "What does the free-threaded build guarantee?"
  The [thread-safety page](https://docs.python.org/3/library/threadsafety.html) lists the per-type guarantees; its summary is that operations with multiple accesses, and iteration, are never atomic.

---

## CT8. Show a measured CPU-bound vs I/O-bound threading benchmark on the GIL build and the free-threaded build.

> "On the default build, four threads running a pure-Python loop take as long as running it four times serially, sometimes longer from GIL contention.
> Four threads sleeping, or hashing large buffers in hashlib, overlap almost perfectly because those calls release the GIL.
> On the free-threaded 3.14t build with the GIL off, the same CPU loop scaled 2.1 to 3.4 times on four threads on my laptop, and the I/O case is unchanged.
> The same free-threaded binary with `PYTHON_GIL=1` behaves like the default build, which isolates the GIL as the cause."

```python
# Runs on both builds: python3.14 (GIL) and python3.14t (free-threaded).
import hashlib
import sys
import sysconfig
import threading
import time


def cpu_task(n: int) -> int:
    total = 0
    for i in range(n):
        total += i * i
    return total


def io_task(delay: float) -> None:
    time.sleep(delay)


def hash_task(buf: bytes) -> str:
    return hashlib.sha256(buf).hexdigest()


def run_threads(fn, args_list) -> float:
    threads = [threading.Thread(target=fn, args=args) for args in args_list]
    start = time.perf_counter()
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    return time.perf_counter() - start


def run_serial(fn, args_list) -> float:
    start = time.perf_counter()
    for args in args_list:
        fn(*args)
    return time.perf_counter() - start


WORKERS = 4
jobs = {
    "cpu (pure Python loop)": (cpu_task, [(3_000_000,)] * WORKERS),
    "io (time.sleep 0.1 s)": (io_task, [(0.1,)] * WORKERS),
    "hashlib sha256 128 MiB": (hash_task, [(b"x" * (128 << 20),)] * WORKERS),
}
build = "free-threaded build" if sysconfig.get_config_var("Py_GIL_DISABLED") else "default build"
print(f"{sys.version.split()[0]} ({build}, GIL {'on' if sys._is_gil_enabled() else 'off'}), {WORKERS} threads")
for name, (fn, args_list) in jobs.items():
    serial = run_serial(fn, args_list)
    threaded = run_threads(fn, args_list)
    print(f"  {name:24} serial={serial:.2f}s threads={threaded:.2f}s speedup={serial / threaded:.1f}x")
```

Single-machine numbers: three runs of each configuration on 2026-09-30, Apple M3 Pro laptop (12 cores: 6 performance, 6 efficiency), machine busy (load average above 20); one representative run of each:

```text
3.14.7 (default build, GIL on), 4 threads
  cpu (pure Python loop)   serial=0.33s threads=0.33s speedup=1.0x
  io (time.sleep 0.1 s)    serial=0.42s threads=0.11s speedup=4.0x
  hashlib sha256 128 MiB   serial=0.20s threads=0.06s speedup=3.5x
3.14.6 (free-threaded build, GIL on), 4 threads          <- PYTHON_GIL=1 python3.14t
  cpu (pure Python loop)   serial=0.32s threads=0.30s speedup=1.1x
  io (time.sleep 0.1 s)    serial=0.41s threads=0.11s speedup=3.9x
  hashlib sha256 128 MiB   serial=0.20s threads=0.05s speedup=3.7x
3.14.6 (free-threaded build, GIL off), 4 threads         <- python3.14t
  cpu (pure Python loop)   serial=0.32s threads=0.12s speedup=2.7x
  io (time.sleep 0.1 s)    serial=0.41s threads=0.11s speedup=3.9x
  hashlib sha256 128 MiB   serial=0.20s threads=0.05s speedup=3.7x
```

Ranges over six runs per configuration (two sessions): CPU loop 0.8x to 1.1x with the GIL on and 2.1x to 3.4x with it off; sleep 3.9x to 4.1x everywhere; hashlib 3.4x to 3.8x everywhere.

How to read it:

- CPU loop: no speedup with the GIL on (0.8x to 1.1x; a dip below 1.0 is GIL hand-off overhead), real scaling with it off.
  Four threads on a 6 performance plus 6 efficiency core M3 Pro do not give 4x because of scheduling onto efficiency cores and free-threading overheads.
- Sleep: about 4x everywhere, because waiting never needed the GIL.
- hashlib on large buffers: 3.4x to 3.8x even with the GIL on, because the hashing runs with the GIL released.
- The default build (Homebrew 3.14.7) and the free-threaded build (uv 3.14.6) are different binaries, so compare serial columns across them with care; the `PYTHON_GIL=1` row is the clean comparison.

**Follow-ups they ask:**

- "What is the single-threaded cost of the free-threaded build?"
  The 3.14 What's New says roughly 5-10% depending on platform and compiler; the free-threading HOWTO quotes pyperformance averages from about 1% on macOS aarch64 to 8% on x86-64 Linux.
  Measure your own workload on your own hardware.
- "Why did threads make the CPU case slower on the default build?"
  Every forced switch is a GIL hand-off with a condition variable signal and an OS wake-up; with four threads contending, that overhead is pure loss.

---

## CT9. What is free-threaded Python? What is its status in 3.13 and 3.14, and how do you detect it? (must know)

> "PEP 703 made the GIL optional through a separate build, `python3.13t` and `python3.14t`, with a different ABI tag (`cp314t`).
> In 3.13 it was experimental.
> In 3.14 PEP 779 moved it to phase II, officially supported but still not the default; making it the default needs a future decision.
> I detect the build with `sysconfig.get_config_var('Py_GIL_DISABLED')` and whether the GIL is actually off right now with `sys._is_gil_enabled()`, because importing a C extension that has not declared free-threading support turns the GIL back on at runtime with a warning.
> `PYTHON_GIL=0` or `-X gil=0` forces it off anyway, which is only for testing."

| Question | 3.13 | 3.14 |
| --- | --- | --- |
| Status | Experimental (PEP 703) | Officially supported, phase II (PEP 779), not the default |
| Binary | `python3.13t`, separate install option | `python3.14t`, ABI tag `cp314t` |
| Specializing adaptive interpreter | Disabled in free-threaded mode | Enabled (What's New 3.14) |
| Single-thread penalty | Larger | "roughly 5-10%" (What's New); about 1% macOS aarch64 to 8% x86-64 Linux on pyperformance (HOWTO) |
| Threads inherit the caller's `contextvars` | No | Yes by default on 3.14t (`sys.flags.thread_inherit_context`), no on the default build |

| Knob | Effect |
| --- | --- |
| `sysconfig.get_config_var("Py_GIL_DISABLED")` | `1` on a free-threaded build: the recommended check for build decisions |
| `sys._is_gil_enabled()` | Whether the GIL is on in this process right now (3.13+) |
| `python -VV`, `sys.version` | Contain "free-threading build" |
| `PYTHON_GIL=0` / `-X gil=0` | Keep the GIL off even if an extension asks for it; on a default build `PYTHON_GIL=0` is a fatal startup error |
| `PYTHON_GIL=1` / `-X gil=1` | Turn the GIL on in a free-threaded build: the A/B switch for benchmarks and bug hunts |
| `Py_mod_gil` slot = `Py_MOD_GIL_NOT_USED` | How a C extension declares it is safe without the GIL; without it, import re-enables the GIL |

```python
# Runs on both builds: python3.14 (GIL) and python3.14t (free-threaded).
import sys
import sysconfig

free_threaded_build = bool(sysconfig.get_config_var("Py_GIL_DISABLED"))
gil_now = sys._is_gil_enabled()
print(f"{sys.version.split()[0]}: free-threaded build={free_threaded_build}, GIL enabled now={gil_now}")
print(f"threads inherit contextvars by default: {bool(sys.flags.thread_inherit_context)}")
if not free_threaded_build:
    assert gil_now                        # a default build cannot turn the GIL off
```

Run under the three configurations:

```text
3.14.7: free-threaded build=False, GIL enabled now=True        <- python3.14
threads inherit contextvars by default: False
3.14.6: free-threaded build=True, GIL enabled now=False        <- python3.14t
threads inherit contextvars by default: True
3.14.6: free-threaded build=True, GIL enabled now=True         <- PYTHON_GIL=1 python3.14t
threads inherit contextvars by default: True
```

And `PYTHON_GIL=0 python3.14` (default build) refuses to start: `Fatal Python error: config_read_gil: Disabling the GIL is not supported by this build`.

**Follow-ups they ask:**

- "What happens to a C extension on 3.14t that was not built for it?"
  A `cp314` wheel cannot even be installed for `cp314t`; the extension must be compiled for the free-threaded ABI.
  If it is compiled but does not declare `Py_MOD_GIL_NOT_USED`, importing it re-enables the GIL for the whole process and prints a warning.
- "Is `sys._is_gil_enabled` private?"
  The underscore marks it as CPython-specific, but it is documented in the `sys` docs and is the documented runtime check.
- "Will 3.15 make it the default?"
  Not as far as I know; PEP 779 sets criteria for phase II only, and phase III (default) needs a later PEP.
  Verify on the day, because this moves quickly.

---

## CT10. What does free-threading change for application code, and what would you say about adopting it?

> "Semantics stay the same: built-in containers keep their per-operation safety through per-object locks, so a `dict` will not be corrupted.
> What changes is timing: races that the GIL made rare become frequent, like the counter in CT6 that lost about 70% of its updates.
> Code that was correct with locks stays correct; code that was 'correct by GIL' breaks.
> My adoption plan: check every C dependency ships `cp314t` wheels, run the test suite under 3.14t with `PYTHON_GIL=0`, add stress tests that hammer shared objects, benchmark against `PYTHON_GIL=1` on the same binary, and only then move CPU-bound threaded work onto it.
> For a web service that already scales with worker processes, the win is small; the win is for CPU-bound work that shares large in-memory state and cannot afford pickling."

| Area | Default build | Free-threaded build |
| --- | --- | --- |
| Data races on your variables | Rare, timing-dependent | Frequent, same bugs |
| Built-in containers | One operation at a time under the GIL | Per-object locks ("critical sections"); same per-operation guarantees documented |
| Sharing one iterator across threads | Works by accident | "Not thread-safe ... threads may see duplicate or missing elements" (HOWTO) |
| Reference counting | Plain increments | Biased reference counting; some objects immortal (code constants, interned strings) |
| `contextvars` in new threads | Empty context | Copy of the starter's context (`thread_inherit_context=1`) |
| Warnings filters in threads | Global | Context-aware (`sys.flags.context_aware_warnings=1`) |
| C extensions | Anything works | Must be rebuilt and declare support, or the GIL comes back |

**What to say in the interview:**

- It is real and supported in 3.14, it is opt-in, and the default build is still what production runs by default.
- The first adopters are CPU-bound, threaded, shared-memory workloads such as in-process analytics, simulations, and inference pre-processing.
- The risk is in dependencies and in latent races, not in the language; I would gate it behind a stress-test suite and TSan-instrumented runs of any in-house C extension.
- [fill in: whether you have tried 3.13t or 3.14t on a real codebase, and what broke]

---

## CT11. How do you create, start, join, and name threads? Target function or subclass?

> "Pass a `target` and `args` for one-off work, and subclass `Thread` overriding `run()` only when the thread owns state and a lifecycle, like a poller with a `stop()` method.
> `start()` creates the OS thread and calls `run()` in it; calling `run()` yourself just runs it in the current thread.
> `join(timeout)` always returns `None`, so after a timeout I check `is_alive()`.
> I name every thread, because the name shows up in logs, in `faulthandler` dumps, and since 3.14 in the OS thread name that `top` and debuggers show."

```python
import threading
import time


def fetch(url: str, results: dict[str, int]) -> None:
    time.sleep(0.05)                                   # stand-in for a blocking HTTP call
    results[url] = len(url)                            # distinct key per thread: no read-modify-write


class Heartbeat(threading.Thread):
    def __init__(self, stop: threading.Event) -> None:
        super().__init__(name="heartbeat", daemon=True)
        self.stop_event = stop
        self.beats = 0

    def run(self) -> None:                             # override run(), never start()
        while not self.stop_event.wait(0.01):
            self.beats += 1


results: dict[str, int] = {}
workers = [threading.Thread(target=fetch, args=(url, results), name=f"fetch-{i}")
           for i, url in enumerate(["/a", "/bb", "/ccc"])]
stop = threading.Event()
heartbeat = Heartbeat(stop)
heartbeat.start()
for w in workers:
    w.start()
for w in workers:
    w.join(timeout=2)
    assert not w.is_alive(), f"{w.name} still running"   # join() returns None even on timeout
stop.set()
heartbeat.join()
print(sorted(results.items()), heartbeat.beats > 0, threading.current_thread().name)
print([w.name for w in workers], threading.active_count())
```

Output:

```text
[('/a', 2), ('/bb', 3), ('/ccc', 4)] True MainThread
['fetch-0', 'fetch-1', 'fetch-2'] 1
```

| API | Use |
| --- | --- |
| `threading.current_thread()`, `main_thread()` | Who am I; is this the main thread (signal handlers only run there) |
| `get_ident()` vs `get_native_id()` | Python's opaque id (reused after exit) vs the OS thread id you see in `top -H`, `py-spy`, and `ps -M` |
| `enumerate()`, `active_count()` | Live `Thread` objects, for leak checks in tests |
| `Thread(daemon=True)` | Do not block interpreter exit (CT12) |
| `Thread(context=...)` (3.14+) | Which `contextvars.Context` the thread starts with (CT14) |
| `threading.stack_size(n)` | Stack size for threads created afterwards; rarely needed |

**Follow-ups they ask:**

- "Can you start a thread twice?"
  No: `RuntimeError: threads can only be started once` (CT33).
- "Why not subclass everywhere?"
  A subclass couples the work to the threading mechanism; a plain function can move to a pool, a process, or `asyncio.to_thread` without changes.

---

## CT12. What are daemon threads, and what happens to threads at interpreter shutdown?

> "At exit, the main thread first joins every non-daemon thread, then runs `atexit` handlers, then finalizes.
> Daemon threads are not waited for: they are stopped abruptly, their `finally` blocks and context managers do not run, and files or transactions they hold may be left half-written.
> I use daemon threads only for work that is safe to lose, like a metrics heartbeat, and give everything else a stop `Event` and an explicit join during shutdown."

```text
main thread returns
   |
   v
threading._shutdown(): join every non-daemon thread   <- a stuck non-daemon thread hangs exit here
   |
   v
atexit handlers run (last registered first)
   |
   v
interpreter finalization: daemon threads are frozen where they stand; no finally, no cleanup
```

```python
import subprocess
import sys
import textwrap

child = textwrap.dedent("""
    import atexit, threading, time
    atexit.register(lambda: print("atexit handler"))

    def worker():
        try:
            time.sleep(0.2)
        finally:
            print("non-daemon finally")

    def background():
        try:
            time.sleep(10)
        finally:
            print("daemon finally")          # never printed

    threading.Thread(target=worker).start()
    threading.Thread(target=background, daemon=True).start()
    print("main thread done")
""")
result = subprocess.run([sys.executable, "-c", child], capture_output=True, text=True, timeout=10)
print(result.stdout, end="")
assert result.returncode == 0
```

Output:

```text
main thread done
non-daemon finally
atexit handler
```

**Follow-ups they ask:**

- "My service hangs on Ctrl+C or on SIGTERM. Why?"
  A non-daemon thread is blocked forever (a `queue.get()` with no timeout, a socket with no timeout), and `threading._shutdown` waits for it.
  Fix: stop events, timeouts on every blocking call, and join with a timeout in the shutdown path.
- "Why not make everything a daemon, then?"
  Because a daemon thread killed mid-write corrupts files and leaves transactions open; exit must be designed, not avoided.
- "Where do signals get delivered?"
  Python signal handlers only run in the main thread, which is one more reason the main thread should not block uninterruptibly on `join()` without a timeout.

---

## CT13. How do you get results and exceptions out of a thread? (must know)

> "A `Thread` has no return value and an exception in it does not propagate to `join()`: it goes to `threading.excepthook`, which prints it to stderr, and the thread just ends.
> So I use `concurrent.futures`: `submit` returns a `Future` whose `result()` returns the value or re-raises the worker's exception in my thread.
> If I must use a raw thread, I capture the result or exception in the thread and re-raise it in `join`, or send it back through a queue.
> In production I also set `threading.excepthook` to log through the logging system so a dead worker is never silent."

```python
import threading
from concurrent.futures import ThreadPoolExecutor


def parse(text: str) -> int:
    return int(text)


class ResultThread(threading.Thread):
    """A thread whose join() returns the target's result or re-raises its exception."""

    def __init__(self, target, *args) -> None:
        super().__init__()
        self._fn, self._fn_args = target, args
        self._result: object = None
        self._exc: BaseException | None = None

    def run(self) -> None:
        try:
            self._result = self._fn(*self._fn_args)
        except BaseException as exc:                  # hand everything to the joiner
            self._exc = exc

    def join(self, timeout: float | None = None) -> object:
        super().join(timeout)
        if self.is_alive():
            raise TimeoutError(f"{self.name} still running")
        if self._exc is not None:
            raise self._exc
        return self._result


ok = ResultThread(parse, "42")
ok.start()
assert ok.join() == 42
bad = ResultThread(parse, "forty-two")
bad.start()
try:
    bad.join()
except ValueError as exc:
    print("re-raised in the joiner:", type(exc).__name__)

seen: list[tuple[str, str]] = []
threading.excepthook = lambda args: seen.append((args.thread.name, args.exc_type.__name__))
plain = threading.Thread(target=parse, args=("x",), name="parser")
plain.start()
plain.join()                                          # returns normally: the error went to the hook
threading.excepthook = threading.__excepthook__
print("excepthook saw:", seen)

with ThreadPoolExecutor(max_workers=2) as pool:
    good, failed = pool.submit(parse, "7"), pool.submit(parse, "seven")
    assert good.result(timeout=5) == 7
    print("future.exception():", type(failed.exception(timeout=5)).__name__)
```

Output:

```text
re-raised in the joiner: ValueError
excepthook saw: [('parser', 'ValueError')]
future.exception(): ValueError
```

**Follow-ups they ask:**

- "What does the default excepthook do with `SystemExit`?"
  Ignores it silently, per the docs; every other exception is printed to `sys.stderr`.
- "What if nobody calls `result()` on a failed future?"
  The exception is stored and never shown: a silent failure (CT33).
  Always consume futures, or add a done-callback that logs failures.

---

## CT14. How do `threading.local` and `contextvars` behave with threads and thread pools?

> "`threading.local` gives each thread its own attribute namespace; it is how old-style libraries keep a per-thread DB connection or request.
> Its trap is thread pools: a worker thread is reused, so a value set by one task is still there for the next task on that thread, which leaks request data unless every task resets it.
> `contextvars` are per-context instead of per-thread; asyncio gives each task its own context.
> On the default 3.14 build a new thread starts with an empty context; on the free-threaded build it starts with a copy of the starter's context; `Thread(context=...)` in 3.14, or `ctx.run` in any version, makes it explicit.
> `asyncio.to_thread` copies the context for you; `loop.run_in_executor` does not."

```python
# Runs on both builds: python3.14 (GIL) and python3.14t (free-threaded).
import contextvars
import sys
import threading
from concurrent.futures import ThreadPoolExecutor

request_id = contextvars.ContextVar("request_id", default="-")
request_id.set("req-42")
seen: dict[str, str] = {}


def record(key: str) -> None:
    seen[key] = request_id.get()


threads = [
    threading.Thread(target=record, args=("default",)),
    threading.Thread(target=record, args=("context=copy",), context=contextvars.copy_context()),  # 3.14+
    threading.Thread(target=contextvars.copy_context().run, args=(record, "ctx.run")),           # any version
]
for t in threads:
    t.start()
for t in threads:
    t.join()
inherit = bool(sys.flags.thread_inherit_context)
assert seen == {"default": "req-42" if inherit else "-", "context=copy": "req-42", "ctx.run": "req-42"}
print(seen)

local = threading.local()


def handle(user: str | None) -> str | None:
    if user is not None:
        local.user = user
    return getattr(local, "user", None)


with ThreadPoolExecutor(max_workers=1) as pool:
    first = pool.submit(handle, "alice").result()
    second = pool.submit(handle, None).result()       # a later "request" on the same worker thread
print("second request sees:", second)                 # leaked from the first request
assert (first, second) == ("alice", "alice")
```

Output (default build; on 3.14t the first entry is `'req-42'`):

```text
{'default': '-', 'context=copy': 'req-42', 'ctx.run': 'req-42'}
second request sees: alice
```

**Follow-ups they ask:**

- "Flask's `request` is thread-local, is it not?"
  Flask and Werkzeug use `contextvars` (`ContextVar`-backed locals), which work for both threads and async; see [Flask](03-Flask.md).
- "How do you avoid the pool leak?"
  Set and reset in a `try/finally` per task, or use a `ContextVar` with a token (`token = var.set(x)` then `var.reset(token)`), or pass the value explicitly.

---

## CT15. Why can you not kill a thread in Python, and how do you cancel one?

> "There is no API to kill a thread, on purpose: killing it at an arbitrary instruction could leave a lock held, a file half-written, or an invariant broken, with no cleanup.
> Cancellation must be cooperative: the thread checks a `threading.Event` between units of work and uses `event.wait(timeout)` instead of `time.sleep` so the stop request interrupts the wait.
> Blocking calls need their own timeouts so the thread gets back to the check.
> If I truly need to kill work, I run it in a process, which can be terminated."

```python
import threading
import time


class Poller(threading.Thread):
    def __init__(self, interval: float) -> None:
        super().__init__(name="poller")
        self.interval = interval
        self._stop_event = threading.Event()
        self.polls = 0

    def run(self) -> None:
        while not self._stop_event.is_set():
            self.polls += 1                           # one short unit of work
            self._stop_event.wait(self.interval)      # an interruptible sleep

    def stop(self, timeout: float = 5.0) -> None:
        self._stop_event.set()
        self.join(timeout)
        if self.is_alive():
            raise TimeoutError("poller did not stop")


poller = Poller(interval=10.0)       # with time.sleep(10), stop() would take up to 10 s
poller.start()
time.sleep(0.05)
start = time.perf_counter()
poller.stop()
elapsed = time.perf_counter() - start
print(f"stopped after {poller.polls} poll(s), in under 0.5 s: {elapsed < 0.5}")
```

Output:

```text
stopped after 1 poll(s), in under 0.5 s: True
```

| Blocking call | How to make it cancellable |
| --- | --- |
| `time.sleep(n)` | `stop_event.wait(n)` |
| `queue.get()` | `get(timeout=...)` in a loop, a poison pill (CT26), or `Queue.shutdown()` (3.13+) |
| `sock.recv()` | `sock.settimeout(...)`, or `shutdown()`/`close()` the socket from the stopping thread |
| `lock.acquire()` | `acquire(timeout=...)` |
| Third-party call with no timeout | Run it in a process you can terminate, or in a daemon thread you abandon |

**Follow-ups they ask:**

- "What about `ctypes.pythonapi.PyThreadState_SetAsyncExc`?"
  It schedules an exception in another thread, delivered only when that thread next runs bytecode, so it cannot interrupt a blocking system call, and it can fire inside a `finally` or while a lock is half-updated.
  It is a debugging hack, not a cancellation mechanism.
- "How is this different in asyncio?"
  `task.cancel()` raises `CancelledError` at the task's current `await`, which is cooperative too but has language support; see [Asyncio deep dive](16-Asyncio-Deep-Dive.md).

---

## CT16. Lock vs RLock: what is the difference, when is reentrancy a smell, and how do timeouts work? (must know)

> "A `Lock` is a plain mutex with no owner: the same thread acquiring it twice deadlocks itself, and any thread may release it.
> An `RLock` records its owner and a recursion count, so the owner can re-acquire it, and only the owner can release it.
> I default to `Lock`.
> Needing an `RLock` usually means a public locked method calls another public locked method; I refactor into a private `_unlocked` helper that documents 'caller holds the lock', which keeps critical sections explicit.
> For robustness I use `acquire(timeout=...)` on any lock whose holder might be slow, so a hang becomes an error I can log."

| | `Lock` | `RLock` |
| --- | --- | --- |
| Same thread acquires twice | Deadlocks (or returns `False` with a timeout) | Succeeds, count goes to 2 |
| Release from another thread | Allowed (so it can be used as a signal, though `Event` is clearer) | `RuntimeError` |
| `locked()` | Yes | Yes, new in 3.14 |
| Cost | Cheaper | Slightly more (owner and count bookkeeping) |
| Typical use | Protect a critical section | Re-entrant call paths, `Condition`'s default lock, logging handlers |

```python
import threading

lock = threading.Lock()
assert lock.acquire(blocking=False) is True
assert lock.acquire(timeout=0.1) is False        # same thread again: not reentrant; without a timeout this hangs
releaser = threading.Thread(target=lock.release) # any thread may release a Lock
releaser.start()
releaser.join()
assert not lock.locked()

rlock = threading.RLock()
with rlock:
    with rlock:                                  # re-entry by the owner is fine
        assert rlock.locked()                    # RLock.locked() is new in 3.14

errors: list[str] = []


def foreign_release() -> None:
    try:
        rlock.release()
    except RuntimeError as exc:
        errors.append(f"RLock: {exc}")


rlock.acquire()
intruder = threading.Thread(target=foreign_release)
intruder.start()
intruder.join()
rlock.release()
try:
    lock.acquire(blocking=False, timeout=1)
except ValueError as exc:
    errors.append(f"Lock: {exc}")
print(*errors, sep="\n")
```

Output:

```text
RLock: cannot release un-acquired lock
Lock: can't specify a timeout for a non-blocking call
```

The refactor that removes the need for an `RLock`:

```python
import threading


class Inventory:
    def __init__(self) -> None:
        self._lock = threading.Lock()
        self._stock: dict[str, int] = {}

    def add(self, sku: str, qty: int) -> None:
        with self._lock:
            self._add_unlocked(sku, qty)

    def add_many(self, items: list[tuple[str, int]]) -> None:
        with self._lock:                               # one critical section for the whole batch
            for sku, qty in items:
                self._add_unlocked(sku, qty)           # calling self.add() here would self-deadlock

    def _add_unlocked(self, sku: str, qty: int) -> None:
        """Caller must hold self._lock."""
        self._stock[sku] = self._stock.get(sku, 0) + qty

    def snapshot(self) -> dict[str, int]:
        with self._lock:
            return dict(self._stock)


inv = Inventory()
workers = [threading.Thread(target=inv.add_many, args=([("AAPL", 1), ("MSFT", 2)] * 500,)) for _ in range(8)]
for w in workers:
    w.start()
for w in workers:
    w.join()
assert inv.snapshot() == {"AAPL": 4000, "MSFT": 8000}
```

**Follow-ups they ask:**

- "Is `threading.Lock` fair?"
  No ordering guarantee is documented; the docs say which waiting thread proceeds "is not defined".
  If you need FIFO fairness, hand work through a `queue.Queue` instead.
- "Why does `acquire(blocking=False, timeout=1)` raise?"
  Because the two arguments contradict each other; the docs forbid a timeout when `blocking` is false.

---

## CT17. How does a Condition work? Why must `wait()` be in a `while` loop, and what does `wait_for` do?

> "A `Condition` pairs a lock with a wait queue.
> A consumer takes the lock, checks a predicate on shared state, and if it is false calls `wait()`, which atomically releases the lock and sleeps; `notify()` wakes a waiter, which re-acquires the lock before `wait()` returns.
> By the time the waiter runs, another thread may already have consumed what it was woken for, and wake-ups can also be spurious, so the predicate must be re-checked in a `while` loop.
> `wait_for(predicate, timeout)` is that loop, with correct timeout accounting.
> `notify_all` is safer than `notify` when waiters wait for different predicates, at the cost of waking everyone."

The `if` bug, made deterministic: two consumers wait, the producer adds one item and calls `notify_all`, and both wake up.

```python
import threading
import time


def run_consumers(use_wait_for: bool) -> list[str]:
    cv = threading.Condition()
    items: list[int] = []
    waiting = 0
    outcomes: list[str] = []

    def consumer() -> None:
        nonlocal waiting
        with cv:
            waiting += 1
            if use_wait_for:
                if cv.wait_for(lambda: items, timeout=0.3):   # re-checks the predicate on every wake-up
                    outcomes.append(f"got {items.pop()}")
                else:
                    outcomes.append("timed out, took nothing")
            else:
                if not items:                                 # BUG: checked once, before sleeping
                    cv.wait()
                try:
                    outcomes.append(f"got {items.pop()}")
                except IndexError:
                    outcomes.append("IndexError")

    threads = [threading.Thread(target=consumer) for _ in range(2)]
    for t in threads:
        t.start()
    while True:                                   # both consumers hold the lock until they are inside wait()
        with cv:
            if waiting == 2:
                items.append(1)
                cv.notify_all()                   # wakes both; only one item exists
                break
        time.sleep(0.001)
    for t in threads:
        t.join(timeout=5)
    return sorted(outcomes)


print("if     :", run_consumers(use_wait_for=False))
print("wait_for:", run_consumers(use_wait_for=True))
```

Output:

```text
if     : ['IndexError', 'got 1']
wait_for: ['got 1', 'timed out, took nothing']
```

```text
consumer                               producer
with cv:                                .
  while not items:                      .
    cv.wait()  -- releases lock, sleeps .
                                        with cv:
                                          items.append(x)
                                          cv.notify()   -- marks a waiter runnable
                                        (releases lock)
  (wakes, re-acquires lock)
  re-check predicate: another consumer may have taken x
  items.pop()
```

**Follow-ups they ask:**

- "What is a lost wake-up?"
  Notifying before the other thread starts waiting: the notification is not remembered.
  Checking the predicate under the same lock that the notifier holds when it changes state prevents it, which is why a `Condition` owns a lock.
- "Condition vs Event?"
  An `Event` is a latched boolean with no lock you can hold; a `Condition` protects arbitrary shared state and a predicate over it.
- "Must I hold the lock to call `notify`?"
  Yes; `notify()` on an un-acquired condition raises `RuntimeError`.

---

## CT18. Semaphore vs BoundedSemaphore: what are they for?

> "A semaphore is a counter of permits: `acquire` takes one and blocks at zero, `release` returns one.
> I use it to cap concurrency, like at most 10 in-flight calls to a rate-limited partner API or at most N DB connections.
> A `BoundedSemaphore` raises `ValueError` if it is released more times than acquired, which turns a double-release bug into a crash instead of a silently raised limit, so it is the one I use.
> Unlike a lock, a semaphore has no owner, so it can also be used to signal between threads."

```python
import threading
import time

api_slots = threading.BoundedSemaphore(3)      # at most 3 concurrent calls to a partner API
in_flight = 0
peak = 0
counter_lock = threading.Lock()


def call_partner_api() -> None:
    global in_flight, peak
    with api_slots:
        with counter_lock:
            in_flight += 1
            peak = max(peak, in_flight)
        time.sleep(0.02)                         # the slow remote call
        with counter_lock:
            in_flight -= 1


callers = [threading.Thread(target=call_partner_api) for _ in range(12)]
for c in callers:
    c.start()
for c in callers:
    c.join()
print("peak concurrency never above 3:", peak <= 3)

loose = threading.Semaphore(1)
loose.release()                                  # silently becomes 2 permits: the bug goes unnoticed
bounded = threading.BoundedSemaphore(1)
try:
    bounded.release()
except ValueError as exc:
    print("BoundedSemaphore:", exc)
```

Output:

```text
peak concurrency never above 3: True
BoundedSemaphore: Semaphore released too many times
```

**Follow-ups they ask:**

- "Semaphore(1) vs Lock?"
  Functionally similar, but a lock communicates mutual exclusion and has `locked()`; use a semaphore when the count can exceed one.
- "A semaphore in asyncio?"
  `asyncio.Semaphore` has the same shape but must not be shared with threads; see [Coding round](11-Coding-Round.md) K12.

---

## CT19. What are Event, Barrier, and Timer for?

> "An `Event` is a latched flag: `set()` wakes every waiter and stays set until `clear()`; I use it for 'ready' and 'stop' signals.
> A `Barrier(n)` blocks until n threads arrive, then releases them all; I use it in tests to force interleavings and in phased computations.
> A `Timer` runs a function once after a delay in its own thread and can be cancelled before it fires; for anything recurring or many timers I use a scheduler or an event loop instead, because each `Timer` is a whole thread."

```python
import threading

ready = threading.Event()
assert ready.wait(timeout=0.01) is False         # wait() returns the flag, so a timeout is visible
ready.set()
assert ready.wait() is True and ready.is_set()

arrivals: list[str] = []
start_line = threading.Barrier(3, action=lambda: arrivals.append("all arrived"))  # action runs once


def runner(name: str) -> None:
    arrivals.append(name)
    start_line.wait(timeout=5)


runners = [threading.Thread(target=runner, args=(f"r{i}",)) for i in range(3)]
for r in runners:
    r.start()
for r in runners:
    r.join()
assert sorted(arrivals[:3]) == ["r0", "r1", "r2"] and arrivals[3] == "all arrived"

lonely = threading.Barrier(2, timeout=0.05)
try:
    lonely.wait()                                # nobody else comes: the barrier breaks
except threading.BrokenBarrierError:
    print("barrier broken:", lonely.broken)

fired = threading.Event()
timer = threading.Timer(0.01, fired.set)
timer.start()
assert fired.wait(timeout=2)
cancelled = threading.Timer(10.0, print, args=("never printed",))
cancelled.start()
cancelled.cancel()
cancelled.join(timeout=1)
print("cancelled timer finished early:", not cancelled.is_alive())
```

Output:

```text
barrier broken: True
cancelled timer finished early: True
```

**Follow-ups they ask:**

- "Why does `Event.wait()` return a bool?"
  So you can tell a timeout from a set event without a second check.
- "Can a Barrier be reused?"
  Yes, it resets after each release; once broken (a timeout or `abort()`), every waiter gets `BrokenBarrierError` until `reset()`.

---

## CT20. Which queue types exist, and how do `task_done`, `join`, and `shutdown` work?

> "`queue.Queue` is a thread-safe FIFO built from a deque, a lock, and three conditions: not empty, not full, and all tasks done.
> `LifoQueue` is a stack, `PriorityQueue` a heap, and `SimpleQueue` an unbounded C-implemented FIFO without task tracking.
> A bounded `Queue(maxsize)` gives backpressure: `put` blocks when consumers fall behind.
> `join()` waits until every item put has had a matching `task_done()`, so consumers call `task_done` in a `finally`.
> Since 3.13, `shutdown()` makes further `put`s raise `ShutDown` and makes `get` raise `ShutDown` once the queue is drained, which replaces poison pills for many designs."

| Type | Order | Bounded | `task_done`/`join` | Notes |
| --- | --- | --- | --- | --- |
| `Queue` | FIFO | Optional `maxsize` | Yes | The default choice |
| `LifoQueue` | LIFO | Optional | Yes | Depth-first work, recency |
| `PriorityQueue` | Smallest first (`heapq`) | Optional | Yes | Use `(priority, seq, item)` or an ordered dataclass so ties never compare payloads |
| `SimpleQueue` | FIFO | No | No | C implementation, reentrant `put` (safe from `__del__` and signal handlers), used inside `ThreadPoolExecutor` |

```python
import itertools
import queue
import threading
from dataclasses import dataclass, field

fifo, lifo = queue.Queue(), queue.LifoQueue()
for q in (fifo, lifo):
    for x in (1, 2, 3):
        q.put(x)
print("fifo", [fifo.get() for _ in range(3)], "lifo", [lifo.get() for _ in range(3)])


@dataclass(order=True)
class Job:
    priority: int
    seq: int                                      # tie-breaker: FIFO among equal priorities
    payload: dict = field(compare=False)          # dicts are unorderable: keep them out of comparisons


prio: queue.PriorityQueue[Job] = queue.PriorityQueue()
seq = itertools.count()
for p, name in [(2, "report"), (0, "margin-call"), (2, "email")]:
    prio.put(Job(p, next(seq), {"name": name}))
print("priority", [prio.get().payload["name"] for _ in range(3)])

small = queue.Queue(maxsize=1)
small.put("a")
try:
    small.put("b", timeout=0.01)                  # backpressure: the queue is full
except queue.Full:
    print("put timed out: queue.Full")

work: queue.Queue[int] = queue.Queue()
done: list[int] = []


def worker() -> None:
    while True:
        try:
            item = work.get()
        except queue.ShutDown:                    # 3.13+: raised once shut down and drained
            return
        try:
            done.append(item * 2)
        finally:
            work.task_done()                      # always, even if processing raised


workers = [threading.Thread(target=worker) for _ in range(2)]
for w in workers:
    w.start()
for i in range(5):
    work.put(i)
work.join()                                       # returns when every put() has a task_done()
work.shutdown()                                   # wakes the blocked get() calls with ShutDown
for w in workers:
    w.join(timeout=5)
try:
    work.put(99)
except queue.ShutDown:
    print("put after shutdown: queue.ShutDown")
print("done", sorted(done))
```

Output:

```text
fifo [1, 2, 3] lifo [3, 2, 1]
priority ['margin-call', 'report', 'email']
put timed out: queue.Full
put after shutdown: queue.ShutDown
done [0, 2, 4, 6, 8]
```

**Follow-ups they ask:**

- "Is `q.qsize()` or `q.empty()` reliable?"
  Only as a hint: by the time you act on it another thread may have changed it (check-then-act).
  Use `get(timeout=...)` or `get_nowait()` with `queue.Empty` instead.
- "What does `shutdown(immediate=True)` do?"
  Drains the queue, discounting the drained items from the unfinished count so `join()` returns, and makes `get` raise immediately.
- "`queue.Queue` vs `multiprocessing.Queue` vs `asyncio.Queue`?"
  Threads in one process, processes (pickled over a pipe), and coroutines on one loop, respectively; none of them is safe to use across the other boundaries.

---

## CT21. Reproduce a lost update and a check-then-act race reliably, then fix them three ways. (must know)

> "A race is a bug whose outcome depends on timing, so to prove one I remove the timing: I force the bad interleaving with a `Barrier` between the read and the write, which makes the failure happen every run.
> For a statistical reproduction on the default build, I shrink `sys.setswitchinterval` and put a Python call between the read and the write.
> Then I fix it one of three ways: a lock around the whole read-modify-write or check-then-act; ownership, where one thread owns the state and everyone else sends it messages through a queue; or a design with no shared mutation, like per-thread partial results merged after join."

**Race 1: lost update, forced deterministically.**
Both threads read the balance, then both write; one write is lost every time.

```python
import threading


class Account:
    def __init__(self, balance: int) -> None:
        self.balance = balance


account = Account(100)
both_have_read = threading.Barrier(2)


def withdraw(amount: int) -> None:
    current = account.balance            # read
    both_have_read.wait(timeout=5)       # force the bad interleaving: both read before either writes
    account.balance = current - amount   # write: clobbers the other thread's write


threads = [threading.Thread(target=withdraw, args=(amount,)) for amount in (30, 50)]
for t in threads:
    t.start()
for t in threads:
    t.join()
print(f"final balance {account.balance}, correct answer 20")
assert account.balance in (70, 50)       # whichever write landed last wins
```

It prints 70 or 50 depending on which write lands last, and never 20.

**Race 2: lost update, statistically, on the default build.**
A method call between read and write gives the interpreter a switch point, and a 1 us switch interval makes switches frequent:

```python
import sys
import threading

sys.setswitchinterval(1e-6)              # switch threads far more often than the default 5 ms


class Counter:
    def __init__(self) -> None:
        self.value = 0

    def add(self, n: int) -> None:
        current = self.value
        self.value = self.plus(current, n)   # a Python-level call between the read and the write

    @staticmethod
    def plus(a: int, b: int) -> int:
        return a + b


counter = Counter()
N = 100_000


def work() -> None:
    for _ in range(N):
        counter.add(1)


threads = [threading.Thread(target=work) for _ in range(4)]
for t in threads:
    t.start()
for t in threads:
    t.join()
lost = 4 * N - counter.value
print(f"lost {lost:,} of {4 * N:,} updates")
assert lost > 0
```

Five runs on the Apple M3 Pro laptop (12 cores: 6 performance, 6 efficiency) under 3.14.7 lost between 193,798 and 205,656 of 400,000 updates, about half.

**Race 3: check-then-act, forced.**
Both withdrawals pass the balance check before either acts, so the account is overdrawn:

```python
import threading

ledger: list[int] = [100]                # deposits and withdrawals; balance = sum(ledger)
both_checked = threading.Barrier(2)


def withdraw_if_enough(amount: int) -> None:
    if sum(ledger) >= amount:            # check
        both_checked.wait(timeout=5)     # both threads pass the check before either acts
        ledger.append(-amount)           # act (append itself is atomic; the pair is not)


threads = [threading.Thread(target=withdraw_if_enough, args=(80,)) for _ in range(2)]
for t in threads:
    t.start()
for t in threads:
    t.join()
print("balance after two 80 withdrawals from 100:", sum(ledger))
```

Output:

```text
balance after two 80 withdrawals from 100: -60
```

**The three fixes, tested under contention:**

```python
import queue
import threading
from collections import Counter
from concurrent.futures import ThreadPoolExecutor


class LockedAccount:
    """Fix 1: one lock around the check and the act."""

    def __init__(self, balance: int) -> None:
        self._lock = threading.Lock()
        self._balance = balance

    def withdraw(self, amount: int) -> bool:
        with self._lock:
            if self._balance < amount:
                return False
            self._balance -= amount
            return True

    @property
    def balance(self) -> int:
        with self._lock:
            return self._balance


class AccountOwner:
    """Fix 2: ownership. Only the owner thread touches the balance; others send messages."""

    def __init__(self, balance: int) -> None:
        self._balance = balance
        self._inbox: queue.Queue = queue.Queue()
        self._thread = threading.Thread(target=self._run, name="account-owner")
        self._thread.start()

    def _run(self) -> None:
        while (msg := self._inbox.get()) is not None:
            amount, reply = msg
            ok = self._balance >= amount
            if ok:
                self._balance -= amount
            reply.put(ok)

    def withdraw(self, amount: int) -> bool:
        reply: queue.SimpleQueue = queue.SimpleQueue()
        self._inbox.put((amount, reply))
        return reply.get(timeout=5)

    def close(self) -> int:
        self._inbox.put(None)
        self._thread.join(timeout=5)
        return self._balance


def count_words(lines: list[str]) -> Counter:
    """Fix 3: no shared mutation. Each task builds its own Counter; merge after."""
    return Counter(word for line in lines for word in line.split())


def hammer(withdraw) -> int:
    with ThreadPoolExecutor(max_workers=8) as pool:
        return sum(pool.map(lambda _: withdraw(10), range(200)))


locked = LockedAccount(1_000)
assert hammer(locked.withdraw) == 100 and locked.balance == 0      # exactly 100 withdrawals of 10 succeed

owner = AccountOwner(1_000)
assert hammer(owner.withdraw) == 100 and owner.close() == 0

text = ["buy sell buy", "sell hold", "buy"] * 1_000
chunks = [text[i::4] for i in range(4)]
with ThreadPoolExecutor(max_workers=4) as pool:
    total = sum(pool.map(count_words, chunks), Counter())
assert total == Counter({"buy": 3_000, "sell": 2_000, "hold": 1_000})
print("lock, ownership, and confinement all correct under 8 threads")
```

Output:

```text
lock, ownership, and confinement all correct under 8 threads
```

| Fix | Strength | Cost |
| --- | --- | --- |
| Lock | Simple, local | Contention, deadlock risk when locks nest |
| Ownership (actor, single writer) | No locks in business logic, natural ordering | A thread per owner, latency of a round trip, back-pressure design |
| No shared mutation (confinement, immutable snapshots, merge) | Scales, nothing to get wrong | Needs a mergeable result; not always possible |

**Follow-ups they ask:**

- "How do you test for races in CI?"
  Deterministic interleaving tests with barriers for known bugs, stress tests with many threads and a small switch interval, and running the suite on the free-threaded build.
- "Where do these races show up in web services?"
  Module-level caches and counters in a threaded Flask or gunicorn `gthread` worker, and cross-process versions of the same bug in the database; see [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md).

---

## CT22. Which built-ins are thread-safe: dict, list, deque, print, logging? What happens if you iterate a dict while another thread mutates it?

> "Single operations on dict, list, and set are safe in the sense that the object is never corrupted; compound operations are not.
> `collections.deque` documents thread-safe appends and pops from both ends, which is why it backs `queue.Queue`.
> `logging` is thread-safe: each handler takes its own `RLock` around `emit`, so records from different threads do not interleave within a handler.
> `print` is not a single write: it writes the text and then the end separately, so lines from different threads can interleave.
> Iterating a dict while another thread inserts raises `RuntimeError: dictionary changed size during iteration`; I iterate over a snapshot taken under the lock that writers use."

| Object / call | Thread-safe? | Detail |
| --- | --- | --- |
| `dict` / `list` / `set` single operations | Yes (not corrupted) | Compound operations are races (CT7) |
| Iterating a container another thread mutates | No | dict and set raise `RuntimeError` on a size change; a list silently skips or repeats |
| `collections.deque` append/pop at either end | Yes, documented | `len(dq)` then `popleft()` is still check-then-act |
| `queue.Queue` and friends | Yes, designed for it | The hand-off primitive of choice |
| `logging` | Yes, per handler lock | Not safe for several processes writing one file; see [Multiprocessing](15-Multiprocessing-and-Parallelism.md) |
| `print` | Not atomic per line | Text and `end` are separate writes; use logging or one `sys.stdout.write(line + "\n")` |
| A generator shared across threads | No | Concurrent `next()` raises `ValueError: generator already executing`; give each thread its own iterator or feed a queue |
| `random` module functions | Calls do not corrupt state | All threads share one hidden `Random`; for reproducible runs give each thread its own `random.Random(seed)` |

```python
import threading

prices = {"AAPL": 1, "MSFT": 2}
paused = threading.Event()
inserted = threading.Event()


def writer() -> None:
    paused.wait(timeout=5)
    prices["NVDA"] = 3                  # insert while the reader is in the middle of iterating
    inserted.set()


t = threading.Thread(target=writer)
t.start()
try:
    for symbol in prices:
        paused.set()
        inserted.wait(timeout=5)
except RuntimeError as exc:
    print("RuntimeError:", exc)
t.join()

snapshot = prices.copy()                # iterate a snapshot (taken under the writers' lock in real code)
print(sorted(snapshot))
```

Output:

```text
RuntimeError: dictionary changed size during iteration
['AAPL', 'MSFT', 'NVDA']
```

**Follow-ups they ask:**

- "Is `list(d.items())` a safe snapshot without a lock?"
  On the default build it is a single C call, so in practice yes; on the free-threaded build the docs call `d.copy()` atomic.
  With a lock it is correct everywhere and states the intent, so that is what I write.
- "Why can logging still deadlock?"
  A handler that logs from inside `emit`, or a `fork()` while another thread holds a handler lock; logging re-initializes its own handler locks in the child for that reason, but other libraries do not (CT25).

---

## CT23. What causes deadlock? Show one, detect it, diagnose it, and fix it. (must know)

> "Deadlock needs all four Coffman conditions: mutual exclusion, hold and wait, no preemption, and a circular wait.
> In practice it is two threads taking the same two locks in opposite orders.
> I prevent it by breaking circular wait with a global lock order, by never calling unknown code such as callbacks or logging while holding a lock, and by acquiring with timeouts so a deadlock becomes an error.
> To diagnose a hung process I dump every thread's stack: `faulthandler` (including on a signal), `sys._current_frames()`, or `py-spy dump --pid` from outside without stopping it."

| Coffman condition | How to break it |
| --- | --- |
| Mutual exclusion | Avoid shared mutable state: confinement, immutable data, queues |
| Hold and wait | Acquire everything up front, or release what you hold before waiting |
| No preemption | `acquire(timeout=...)` and back off, releasing what you hold |
| Circular wait | A global lock order (by rank, or by `id()` for peer locks) |

A deadlock forced with a barrier, detected with a join timeout, and diagnosed with stack dumps; the second `acquire` has a timeout so the demo recovers instead of hanging:

```python
import faulthandler
import sys
import tempfile
import threading

accounts_lock = threading.Lock()
audit_lock = threading.Lock()
both_hold_one = threading.Barrier(2)
both_gave_up = threading.Barrier(2)
outcome: dict[str, str] = {}


def worker(name: str, first: threading.Lock, second: threading.Lock) -> None:
    with first:
        both_hold_one.wait(timeout=5)                 # guarantee the circular wait
        got = second.acquire(timeout=1.0)             # a timeout turns a hang into a visible failure
        outcome[name] = "acquired" if got else "timed out"
        if got:
            second.release()
        both_gave_up.wait(timeout=5)                  # keep holding `first` until both have given up


t1 = threading.Thread(target=worker, args=("t1", accounts_lock, audit_lock), name="t1")
t2 = threading.Thread(target=worker, args=("t2", audit_lock, accounts_lock), name="t2")
t1.start()
t2.start()
t1.join(timeout=0.3)
assert t1.is_alive() and t2.is_alive()                # symptom: nothing finishes

frames = sys._current_frames()                        # thread id -> innermost Python frame
for t in (t1, t2):
    print(t.name, "is blocked in", frames[t.ident].f_code.co_name)

with tempfile.TemporaryFile(mode="w+") as dump:       # faulthandler needs a real file descriptor
    faulthandler.dump_traceback(file=dump, all_threads=True)
    dump.seek(0)
    text = dump.read()
print("faulthandler frames in worker():", text.count("in worker"))

for t in (t1, t2):
    t.join(timeout=5)
print("outcome:", dict(sorted(outcome.items())))
```

Output:

```text
t1 is blocked in worker
t2 is blocked in worker
faulthandler frames in worker(): 2
outcome: {'t1': 'timed out', 't2': 'timed out'}
```

The fix is a single global order; a helper makes it impossible to get wrong at call sites:

```python
import threading
from contextlib import ExitStack, contextmanager


@contextmanager
def ordered(*locks: threading.Lock):
    """Acquire locks in one global order (by id) no matter how the caller lists them."""
    with ExitStack() as stack:
        for lock in sorted(set(locks), key=id):
            stack.enter_context(lock)
        yield


a, b = threading.Lock(), threading.Lock()
transfers = 0


def transfer_many(first: threading.Lock, second: threading.Lock) -> None:
    global transfers
    for _ in range(5_000):
        with ordered(first, second):
            transfers += 1


threads = [threading.Thread(target=transfer_many, args=(a, b)),
           threading.Thread(target=transfer_many, args=(b, a))]   # opposite orders at the call sites
for t in threads:
    t.start()
for t in threads:
    t.join(timeout=10)
assert not any(t.is_alive() for t in threads) and transfers == 10_000
print("10,000 transfers with opposite argument orders, no deadlock")
```

Output:

```text
10,000 transfers with opposite argument orders, no deadlock
```

Diagnosing a hung production process:

```bash
# From outside, without stopping the process (py-spy is a separate install; not run here)
py-spy dump --pid 12345

# From inside: register once at startup, then `kill -USR1 <pid>` dumps every thread's stack to stderr
python -X faulthandler app.py          # also dumps on fatal signals such as SIGSEGV
```

```python
import faulthandler
import os
import signal
import tempfile

with tempfile.TemporaryFile(mode="w+") as log:
    faulthandler.register(signal.SIGUSR1, file=log, all_threads=True)   # do this at startup
    os.kill(os.getpid(), signal.SIGUSR1)                                 # what `kill -USR1 <pid>` does
    faulthandler.unregister(signal.SIGUSR1)
    log.seek(0)
    print("stack dump on SIGUSR1 mentions the current thread:", "Current thread" in log.read())
```

Output:

```text
stack dump on SIGUSR1 mentions the current thread: True
```

**Follow-ups they ask:**

- "Does `RLock` prevent deadlock?"
  Only self-deadlock from re-entry; two `RLock`s in opposite orders deadlock just the same.
- "What about deadlocks with one lock?"
  Calling back into code that takes the same `Lock`, waiting on a future whose work needs the lock you hold, or a pool task waiting on another task in the same full pool (CT30).
- "How would you find the lock order violation before production?"
  Code review rule (no nested locks without the helper), lock-order assertions in debug builds, and stress tests with timeouts that fail loudly.

---

## CT24. What are livelock, starvation, and priority inversion, and how do they show up in Python?

> "Livelock is threads that keep reacting to each other without progress, like two threads that each grab one lock, see the other lock busy, back off, and retry in lockstep forever; the fix is randomized backoff or a global order.
> Starvation is a thread that could run but never gets its turn: readers starving writers in a reader-preference RW lock, or an I/O thread starved of the GIL by CPU-bound threads.
> Priority inversion is a high-priority task waiting on a lock held by a low-priority task that cannot run; Python threads have no priorities, but the same shape appears when a latency-critical request waits on a lock held by a batch job that is itself waiting for the GIL behind CPU-bound threads."

| Problem | Python symptom | Mitigation |
| --- | --- | --- |
| Deadlock | Threads blocked forever, 0% CPU | Lock order, timeouts, fewer locks |
| Livelock | Threads busy, 100% CPU, no progress | Randomized (jittered) backoff, a lock order instead of try-and-retreat |
| Starvation | Some requests' latency grows without bound | Fair queues (FIFO hand-off), writer preference, bounded reader batches |
| Priority inversion / convoy | p99 latency spikes when a CPU job runs | Move CPU work to processes, keep critical sections short, never hold a lock across I/O |

Try-lock with jittered backoff, the standard livelock cure when a fixed order is impossible:

```python
import random
import threading
import time


def acquire_both(first: threading.Lock, second: threading.Lock, attempts: int = 1_000) -> bool:
    for _ in range(attempts):
        first.acquire()
        if second.acquire(blocking=False):
            return True                                # caller releases both
        first.release()                                # retreat: do not hold while waiting
        time.sleep(random.uniform(0, 0.001))           # jitter breaks the lockstep
    return False


a, b = threading.Lock(), threading.Lock()
done = {"ab": 0, "ba": 0}


def worker(key: str, first: threading.Lock, second: threading.Lock) -> None:
    for _ in range(300):
        assert acquire_both(first, second)
        try:
            done[key] += 1
        finally:
            second.release()
            first.release()


threads = [threading.Thread(target=worker, args=("ab", a, b)), threading.Thread(target=worker, args=("ba", b, a))]
for t in threads:
    t.start()
for t in threads:
    t.join(timeout=30)
print(done)
```

Output:

```text
{'ab': 300, 'ba': 300}
```

---

## CT25. Why does `fork()` after starting threads deadlock?

> "`fork()` copies the whole address space but only the calling thread.
> Any lock that another thread held at that instant is copied in the locked state, and its owner does not exist in the child, so the child blocks forever the first time it touches that lock: a logging handler, an allocator lock in a C library, or your own lock.
> That is why 3.12 warns when you fork a multi-threaded process, why macOS has defaulted to `spawn` since 3.8, and why 3.14 changed the Linux default for `multiprocessing` and `ProcessPoolExecutor` to `forkserver`.
> The rule is: create processes before threads, or use `spawn` or `forkserver`."

```python
import os
import threading
import time
import warnings

lock = threading.Lock()
held = threading.Event()


def holder() -> None:
    with lock:
        held.set()
        time.sleep(1)                               # holding the lock while the main thread forks


threading.Thread(target=holder, daemon=True).start()
held.wait(timeout=5)
with warnings.catch_warnings(record=True) as caught:
    warnings.simplefilter("always")
    pid = os.fork()                                 # POSIX only
if pid == 0:                                        # child: the holder thread does not exist here
    got = lock.acquire(timeout=0.5)                 # without a timeout this blocks forever
    os._exit(0 if not got else 1)
_, status = os.waitpid(pid, 0)
print("child could not acquire the inherited lock:", os.waitstatus_to_exitcode(status) == 0)
print("warning:", str(caught[0].message).split(",")[1].strip())
```

Output:

```text
child could not acquire the inherited lock: True
warning: use of fork() may lead to deadlocks in the child.
```

Start methods, `os.register_at_fork`, and the `forkserver` default are covered in [Multiprocessing and parallelism](15-Multiprocessing-and-Parallelism.md).

---

## CT26. Implement producer-consumer with a bounded queue and poison pills. (must know)

> "A bounded `queue.Queue` between producers and consumers gives thread-safe hand-off and backpressure: when consumers fall behind, `put` blocks and producers slow down instead of memory growing.
> To stop, I first join the producers so all real work is queued, then put one poison pill per consumer; FIFO order guarantees the pills come after the work.
> Each consumer handles per-item errors so one bad message does not kill the thread, and calls `task_done` in a `finally`.
> On 3.13+ `Queue.shutdown()` can replace the pills."

```python
import queue
import threading

STOP = object()                                   # a unique sentinel nobody can enqueue by accident


def producer(q: queue.Queue, items: range) -> None:
    for item in items:
        q.put(item)                               # blocks while the queue is full: backpressure


def consumer(q: queue.Queue, results: list[int], errors: list[Exception]) -> None:
    while True:
        item = q.get()
        try:
            if item is STOP:
                return                            # finally still runs task_done()
            if item % 7 == 0:
                raise ValueError(f"bad item {item}")
            results.append(item * item)           # list.append is atomic; a lock is needed for anything more
        except Exception as exc:                  # one poison message must not kill the consumer
            errors.append(exc)
        finally:
            q.task_done()


def pipeline(n_items: int, n_producers: int = 2, n_consumers: int = 3, maxsize: int = 4):
    q: queue.Queue = queue.Queue(maxsize=maxsize)
    results: list[int] = []
    errors: list[Exception] = []
    producers = [threading.Thread(target=producer, args=(q, range(i, n_items, n_producers)))
                 for i in range(n_producers)]
    consumers = [threading.Thread(target=consumer, args=(q, results, errors)) for _ in range(n_consumers)]
    for t in producers + consumers:
        t.start()
    for t in producers:
        t.join()                                  # all real work is now queued
    for _ in consumers:
        q.put(STOP)                               # one pill per consumer, after the work
    for t in consumers:
        t.join(timeout=10)
    assert not any(t.is_alive() for t in consumers)
    return results, errors


results, errors = pipeline(100)
print(len(results), "processed,", len(errors), "failed, sum of squares", sum(results))
assert len(results) + len(errors) == 100
```

Output:

```text
85 processed, 15 failed, sum of squares 278615
```

```text
producers --put()--> [ bounded queue, maxsize=4 ] --get()--> consumers
     ^                          |
     +---- blocks when full ----+          one STOP per consumer, enqueued after producers join
```

**Follow-ups they ask:**

- "Why one pill per consumer?"
  Each consumer exits on the first pill it sees; with fewer pills some consumers block in `get()` forever and `join()` hangs.
- "What if a producer crashes?"
  Join producers inside `try/finally` so the pills are still sent, and record the producer error; otherwise consumers wait forever.
- "How would you make it ordered?"
  Tag items with a sequence number and reorder on output, or use one consumer per key (partitioning, the Kafka model).
- "Threads or asyncio for this?"
  Threads if the work calls blocking libraries; `asyncio.Queue` if everything is async; processes if each item is CPU-heavy.

---

## CT27. Implement a reader-writer lock. What are the trade-offs?

> "Readers may share, writers need exclusivity.
> I build it from one `Condition`: a reader waits while a writer is active or waiting, a writer waits until no reader or writer is active.
> Counting waiting writers and blocking new readers when one exists gives writer preference, which prevents writer starvation under constant read load, at the cost of possibly starving readers under constant writes.
> In Python it only pays off when read sections are long and release the GIL, like I/O or a C call; for short in-memory reads a plain `Lock` is faster and simpler, and a copy-on-write snapshot is often better still."

```python
import threading
import time
from contextlib import contextmanager


class RWLock:
    """Many readers or one writer; a waiting writer blocks new readers (writer preference)."""

    def __init__(self) -> None:
        self._cond = threading.Condition()
        self._readers = 0
        self._writer = False
        self._writers_waiting = 0

    @contextmanager
    def read(self):
        with self._cond:
            self._cond.wait_for(lambda: not self._writer and self._writers_waiting == 0)
            self._readers += 1
        try:
            yield
        finally:
            with self._cond:
                self._readers -= 1
                if self._readers == 0:
                    self._cond.notify_all()

    @contextmanager
    def write(self):
        with self._cond:
            self._writers_waiting += 1
            try:
                self._cond.wait_for(lambda: not self._writer and self._readers == 0)
            finally:
                self._writers_waiting -= 1
            self._writer = True
        try:
            yield
        finally:
            with self._cond:
                self._writer = False
                self._cond.notify_all()


def poll_until(predicate, timeout: float = 5.0) -> None:
    deadline = time.monotonic() + timeout
    while not predicate():
        assert time.monotonic() < deadline, "condition never became true"
        time.sleep(0.001)


rw = RWLock()

# 1. Readers overlap: the barrier only passes if all three are inside read() at once.
inside_together = threading.Barrier(3, timeout=2)


def overlapping_reader() -> None:
    with rw.read():
        inside_together.wait()


readers = [threading.Thread(target=overlapping_reader) for _ in range(3)]
for r in readers:
    r.start()
for r in readers:
    r.join()

# 2. Writer preference: with a reader inside and a writer waiting, a new reader must wait for the writer.
order: list[str] = []
release_first_reader = threading.Event()


def first_reader() -> None:
    with rw.read():
        release_first_reader.wait(timeout=5)


def writer() -> None:
    with rw.write():
        order.append("writer")


def late_reader() -> None:
    with rw.read():
        order.append("late reader")


r1 = threading.Thread(target=first_reader)
r1.start()
poll_until(lambda: rw._readers == 1)
w = threading.Thread(target=writer)
w.start()
poll_until(lambda: rw._writers_waiting == 1)
r2 = threading.Thread(target=late_reader)
r2.start()
time.sleep(0.05)
assert order == []                                   # the late reader is blocked behind the waiting writer
release_first_reader.set()
for t in (r1, w, r2):
    t.join(timeout=5)
print("readers overlapped; order after the first reader left:", order)
```

Output:

```text
readers overlapped; order after the first reader left: ['writer', 'late reader']
```

| Choice | Benefit | Cost |
| --- | --- | --- |
| Reader preference | Maximum read throughput | Writers can starve under steady reads |
| Writer preference (above) | Writers get in promptly; readers see fresh data | Readers can starve under steady writes |
| Fair (FIFO ticket) | No starvation | More bookkeeping, less read overlap |
| Copy-on-write snapshot | Readers take no lock at all: `snapshot = self._data` | Each write copies; fine for rarely changing config |

**Follow-ups they ask:**

- "Can a reader upgrade to a writer?"
  Not with this design: two readers upgrading at once deadlock (each waits for the other to leave).
  Release and re-acquire as a writer, then re-validate.
- "Is it reentrant?"
  No; a thread that takes `read()` twice while a writer waits deadlocks itself, a classic trap.

---

## CT28. How do you do thread-safe lazy initialization or a singleton? Is double-checked locking needed in Python?

> "The simplest thread-safe singleton in Python is a module-level object: the import system runs a module once, under the import lock.
> If creation must be lazy because it is expensive or needs config, I use double-checked locking: check without the lock for the fast path, take the lock, check again, create.
> Without the second check two threads that both saw `None` both create an instance.
> `functools.cache` is not a substitute: it protects its own dict but does not stop two threads from running the function at the same time."

```python
import functools
import threading

# 1. Naive lazy init: the test hook widens the window between the check and the create.
naive_created = []


class NaiveClient:
    _instance = None

    @classmethod
    def get(cls, pause=None):
        if cls._instance is None:
            if pause:
                pause()
            cls._instance = cls()
        return cls._instance

    def __init__(self) -> None:
        naive_created.append(self)


# 2. Double-checked locking.
dcl_created = []


class Client:
    _instance = None
    _lock = threading.Lock()

    @classmethod
    def get(cls):
        inst = cls._instance
        if inst is None:                          # fast path: no lock once initialized
            with cls._lock:
                inst = cls._instance
                if inst is None:                  # second check: another thread may have won
                    inst = cls._instance = cls()
        return inst

    def __init__(self) -> None:
        dcl_created.append(self)


# 3. functools.cache runs the function in both threads if they arrive together.
both_inside = threading.Barrier(2, timeout=2)
cache_calls = []


@functools.cache
def load_settings() -> dict:
    cache_calls.append(1)
    both_inside.wait()                            # passes only if two calls are inside at the same time
    return {"env": "prod"}


def run_together(fn, n: int) -> None:
    start = threading.Barrier(n, timeout=2)

    def go() -> None:
        start.wait()
        fn()

    threads = [threading.Thread(target=go) for _ in range(n)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()


gap = threading.Barrier(2, timeout=2)
run_together(lambda: NaiveClient.get(pause=gap.wait), 2)
run_together(Client.get, 16)
run_together(load_settings, 2)
print(f"naive: {len(naive_created)} instances, double-checked: {len(dcl_created)}, "
      f"functools.cache calls: {len(cache_calls)}")
```

Output:

```text
naive: 2 instances, double-checked: 1, functools.cache calls: 2
```

**Follow-ups they ask:**

- "Is double-checked locking broken in Python the way it was in old Java?"
  The Java problem was a reference published before the constructor's writes were visible.
  In CPython the constructor call completes before the attribute store, and the lock release on the slow path publishes it; Python has no formal memory model document, so the lock-guarded second check is the part you rely on.
- "What if the initializer raises?"
  Nothing is stored, the lock is released by `with`, and the next caller retries; decide if that is what you want (retry storms) or cache the failure for a short time.

---

## CT29. How do you use `ThreadPoolExecutor`: `submit` vs `map`, `as_completed`, `wait`, exceptions, `max_workers`, and shutdown? (must know)

> "`submit` returns a `Future` per call; I combine it with `as_completed` to handle results as they finish, with per-item error handling.
> `map` returns results in input order, lazily, and re-raises the first exception when iteration reaches that item, which aborts the rest of the loop.
> `wait(return_when=FIRST_EXCEPTION)` lets me stop early on the first failure.
> The default `max_workers` is `min(32, os.process_cpu_count() + 4)` in 3.13+, tuned for I/O; I set it explicitly from the downstream limit, like the DB pool size.
> `shutdown(cancel_futures=True)` drops queued work, and in 3.14 `map(..., buffersize=n)` stops `map` from submitting the whole input up front."

```python
import time
from concurrent.futures import FIRST_EXCEPTION, ThreadPoolExecutor, as_completed, wait


def fetch(i: int) -> int:
    time.sleep(0.05 * (5 - i))                    # item 4 finishes first, item 0 last
    if i == 3:
        raise ConnectionError(f"item {i} failed")
    return i * 10


with ThreadPoolExecutor(max_workers=5, thread_name_prefix="fetch") as pool:
    futures = {pool.submit(fetch, i): i for i in range(5)}
    completion_order, failures = [], []
    for fut in as_completed(futures, timeout=5):
        try:
            completion_order.append(fut.result())
        except ConnectionError as exc:
            failures.append(str(exc))
    print("as_completed:", completion_order, failures)

    in_order: list[object] = []
    try:
        for value in pool.map(fetch, range(5), timeout=5):
            in_order.append(value)
    except ConnectionError as exc:
        in_order.append(f"raised: {exc}")
    print("map         :", in_order)

    done, not_done = wait([pool.submit(fetch, i) for i in range(5)], timeout=5, return_when=FIRST_EXCEPTION)
    print("wait        :", len(done), "done,", len(not_done), "still running at the first error")
```

Output:

```text
as_completed: [40, 20, 10, 0] ['item 3 failed']
map         : [0, 10, 20, 'raised: item 3 failed']
wait        : 2 done, 3 still running at the first error
```

```python
import os
import threading
from concurrent.futures import ThreadPoolExecutor

expected = min(32, (os.process_cpu_count() or 1) + 4)        # the formula in 3.13+
print("default max_workers matches the formula:", ThreadPoolExecutor()._max_workers == expected)

pool = ThreadPoolExecutor(max_workers=1)
started, release = threading.Event(), threading.Event()
running = pool.submit(lambda: (started.set(), release.wait(5)))
queued = [pool.submit(print, "never runs") for _ in range(5)]
started.wait(timeout=5)
pool.shutdown(wait=False, cancel_futures=True)                # cancel everything not yet started
release.set()
running.result(timeout=5)
print("cancelled:", sum(f.cancelled() for f in queued), "of", len(queued))

with ThreadPoolExecutor(max_workers=2) as pool:
    lazy = pool.map(str, range(10_000_000), buffersize=4)     # 3.14+: at most 4 submitted ahead
    print("first two:", next(lazy), next(lazy))
```

Output:

```text
default max_workers matches the formula: True
cancelled: 5 of 5
first two: 0 1
```

| Need | Use |
| --- | --- |
| Results as they finish, per-item errors | `submit` + `as_completed(timeout=...)` |
| Results in input order, fail fast | `map(timeout=...)` |
| Huge or infinite input | `map(..., buffersize=n)` (3.14+) or a bounded queue |
| Stop on first failure | `wait(return_when=FIRST_EXCEPTION)` then cancel the rest |
| A deadline for the whole batch | `wait(fs, timeout=...)`; running threads cannot be cancelled, only abandoned |

**Follow-ups they ask:**

- "Does `future.cancel()` stop a running task?"
  No: it only succeeds for futures that have not started; a running thread cannot be interrupted (CT15).
- "Why does `with ThreadPoolExecutor()` sometimes hang at exit?"
  `__exit__` calls `shutdown(wait=True)`, which waits for every running task; a task blocked without a timeout blocks the `with`.
- "Why `process_cpu_count` and not `cpu_count`?"
  3.13 switched the default from `os.cpu_count()` to `os.process_cpu_count()`, which respects CPU affinity (for example `taskset`); neither sees a container's CPU quota, so set `max_workers` explicitly in Kubernetes.
- "How many workers for I/O?"
  Enough to cover latency times throughput (Little's law: 200 requests per second at 50 ms each is 10 in flight), capped by what the downstream allows.

---

## CT30. Why must you not wait on the same pool from inside a task, and how do thread pools serve async code?

> "If a task submits a child task to its own pool and waits for it, and every worker is busy doing that, no worker is free to run the children: a deadlock with no locks in sight.
> The fix is a separate pool for the inner level, or restructuring so the parent returns and the caller combines results.
> In async code, a thread pool is the bridge for blocking calls: `asyncio.to_thread(fn)` runs a sync function in the loop's default executor and copies the current `contextvars` context; `loop.run_in_executor(pool, fn)` lets me choose the pool but does not copy the context."

```python
from concurrent.futures import ThreadPoolExecutor

pool = ThreadPoolExecutor(max_workers=1)


def child() -> str:
    return "child done"


def parent() -> str:
    inner = pool.submit(child)                    # queued behind parent, which holds the only worker
    try:
        return inner.result(timeout=0.5)
    except TimeoutError:                          # concurrent.futures.TimeoutError is TimeoutError since 3.11
        return "deadlock: parent waits for child, child waits for a free worker"


print(pool.submit(parent).result())
pool.shutdown()

inner_pool = ThreadPoolExecutor(max_workers=1)
outer_pool = ThreadPoolExecutor(max_workers=1)
print(outer_pool.submit(lambda: inner_pool.submit(child).result(timeout=5)).result())
outer_pool.shutdown()
inner_pool.shutdown()
```

Output:

```text
deadlock: parent waits for child, child waits for a free worker
child done
```

```python
import asyncio
import contextvars
import sys
import time

request_id = contextvars.ContextVar("request_id", default="-")


def blocking_call() -> str:
    time.sleep(0.1)                               # a sync SDK call: would freeze the loop if called directly
    return request_id.get()


async def main() -> None:
    request_id.set("req-7")
    start = time.perf_counter()
    ids = await asyncio.gather(*(asyncio.to_thread(blocking_call) for _ in range(5)))
    elapsed = time.perf_counter() - start
    print(f"5 blocking calls via to_thread overlapped: {elapsed < 0.3}; context copied: {set(ids)}")
    loop = asyncio.get_running_loop()
    via_executor = await loop.run_in_executor(None, blocking_call)
    if not sys.flags.thread_inherit_context:      # default build: executor threads start with an empty context
        print("run_in_executor saw:", via_executor)


asyncio.run(main())
```

Output (default build):

```text
5 blocking calls via to_thread overlapped: True; context copied: {'req-7'}
run_in_executor saw: -
```

**Follow-ups they ask:**

- "How big is the default executor?"
  The same `ThreadPoolExecutor` default, `min(32, cpu + 4)`, so a burst of `to_thread` calls queues behind 16 threads on a 12-core machine.
  Starlette and FastAPI run sync endpoints through AnyIO's thread limiter instead, 40 tokens by default in AnyIO 4.15; see [FastAPI](04-FastAPI.md) A2.
- "When is `to_thread` the wrong tool?"
  For CPU-bound work on the default build: the thread still needs the GIL and slows the event loop thread; use a `ProcessPoolExecutor` via `run_in_executor`.

---

## CT31. What is Python's memory model? Do you need to worry about visibility and reordering?

> "Python has no formal memory model document like Java's or C++'s.
> On the default build the GIL is a full barrier: whatever one thread wrote before releasing it is visible to the next thread that acquires it, so in practice bytecode-level operations appear sequentially consistent and you never see a torn object reference.
> The free-threaded build aims to keep the same Python-level behavior; the docs say built-in types use internal locks that 'behave similarly to the GIL' and document per-operation guarantees, while recommending explicit locks.
> What neither build gives you is atomicity of compound invariants, so the rule is the same as in any language: shared mutable state is accessed under a lock, or handed over through a queue, an event, or a future, all of which establish the ordering for you."

| Guarantee | Default build | Free-threaded build |
| --- | --- | --- |
| No torn reads of a reference or an int | Yes (GIL) | Yes (atomic loads and stores on object references) |
| Single built-in operations are indivisible | Effectively, by the GIL | Documented per type, via per-object locks |
| Writes visible after `Lock` release / `Event.set` / `Queue.put` | Yes | Yes |
| Compound invariants | No | No |
| Spinning on a plain flag eventually sees the write | In practice | In practice, but burns a core; use `Event` |

Safe publication: build the object fully, then hand it over through a primitive that orders the two threads.

```python
import threading
from types import MappingProxyType

settings = MappingProxyType({"pool": 5})         # readers only ever see a complete, read-only snapshot
ready = threading.Event()


def reload_settings() -> None:
    global settings
    fresh = {"pool": 10, "url": "db://primary"}  # build fully in a private object...
    settings = MappingProxyType(fresh)           # ...then publish with one reference assignment
    ready.set()                                  # set() happens-before wait() returning


seen: list[int] = []
reader = threading.Thread(target=lambda: (ready.wait(timeout=5), seen.append(settings["pool"])))
reader.start()
reload_settings()
reader.join()
print("reader saw pool =", seen[0])
```

Output:

```text
reader saw pool = 10
```

**Follow-ups they ask:**

- "Can the compiler or CPU reorder my Python statements?"
  Not observably between threads in CPython today: each bytecode is executed with the needed synchronization for object references.
  Do not reason at that level anyway; reason in terms of locks and hand-offs, which stay correct if implementations change.
- "Compare to C++."
  C++ gives you a formal model (`std::memory_order`, data races are undefined behavior); Python gives you memory safety and leaves logical races to you.

---

## CT32. How do you choose between threads, processes, and asyncio for a workload, with numbers? Explain Amdahl's law. (must know)

> "I measure the CPU share of one unit of work first.
> If it is mostly waiting, threads or asyncio overlap the waits and the ceiling is the downstream, not Python.
> If it is mostly Python CPU, threads on the default build give nothing, and processes give up to the core count minus pickling and startup.
> Amdahl's law bounds any of these: speedup is 1 over the serial fraction plus the parallel fraction divided by the worker count.
> With threads under the GIL, the serial fraction is the time spent holding the GIL, so a request that is 25% Python CPU and 75% I/O can never go faster than 4x with threads, no matter how many I add."

Amdahl's law: `speedup(n) = 1 / ((1 - p) + p / n)`, where `p` is the parallelizable fraction.
Worked example: a nightly risk batch spends 10% loading and aggregating (serial) and 90% pricing independent positions (parallel).

| Workers `n` | Speedup `1 / (0.1 + 0.9 / n)` | Efficiency (speedup / n) |
| --- | --- | --- |
| 1 | 1.00x | 100% |
| 4 | 3.08x | 77% |
| 12 | 5.71x | 48% |
| infinite | 10.00x | 0% |

The same bound, measured on threads under the GIL: each request burns about 12 ms of Python CPU and spends the rest of its 45-50 ms waiting, so `p` is about 0.75 and the ceiling is `1 / (1 - p)`, about 4x.

```python
import time
from concurrent.futures import ThreadPoolExecutor


def spin(n: int) -> int:
    total = 0
    for i in range(n):                           # fixed pure-Python work: holds the GIL
        total += i
    return total


def calibrate(target_seconds: float) -> int:
    n = 10_000
    while True:
        start = time.perf_counter()
        spin(n)
        if time.perf_counter() - start >= target_seconds:
            return n
        n *= 2


LOOPS = calibrate(0.01)


def request() -> None:
    spin(LOOPS)                                  # about 10 ms of CPU, serialized by the GIL
    time.sleep(0.03)                             # about 30 ms of I/O wait, GIL released


def run(workers: int, jobs: int) -> float:
    start = time.perf_counter()
    with ThreadPoolExecutor(workers) as pool:
        for fut in [pool.submit(request) for _ in range(jobs)]:
            fut.result()
    return time.perf_counter() - start


JOBS = 32
serial = run(1, JOBS)
cpu_start = time.process_time()
spin(LOOPS)
cpu = time.process_time() - cpu_start            # GIL-holding time per request
p = 1 - cpu / (serial / JOBS)                    # fraction of a request that can overlap
print(f"per request: {serial / JOBS * 1000:.1f} ms wall, {cpu * 1000:.1f} ms CPU, p = {p:.2f}")
for n in (2, 4, 8, 16):
    print(f"{n:2} threads: measured {serial / run(n, JOBS):.2f}x, ceiling min(n, 1/(1-p)) = {min(n, 1 / (1 - p)):.2f}x")
```

Single-machine numbers, Apple M3 Pro laptop (12 cores: 6 performance, 6 efficiency), CPython 3.14.7, machine busy; one of three runs:

```text
per request: 45.6 ms wall, 12.4 ms CPU, p = 0.73
 2 threads: measured 1.95x, ceiling min(n, 1/(1-p)) = 2.00x
 4 threads: measured 3.54x, ceiling min(n, 1/(1-p)) = 3.69x
 8 threads: measured 3.60x, ceiling min(n, 1/(1-p)) = 3.69x
16 threads: measured 3.62x, ceiling min(n, 1/(1-p)) = 3.69x
```

Across three runs today and two earlier ones, `p` was 0.73 to 0.75 and the plateau 3.5x to 4.0x.

The measured speedup rises with threads until it hits the GIL ceiling and then goes flat: adding threads beyond 8 buys nothing.

| Workload (measured on this machine, CT2, CT5, CT8, CT32) | Choice | Why |
| --- | --- | --- |
| 100 blocking HTTP calls of 200 ms, sync client | `ThreadPoolExecutor(20-50)` | Waits overlap; 30-60 us to start a thread is noise next to 200 ms |
| 10,000 concurrent websockets | asyncio | about 840 B per idle task vs about 35 KiB resident plus 16 MiB reserved per thread |
| Pure-Python CPU loop, 4 chunks | `ProcessPoolExecutor` (or 3.14t threads: 2.1-3.4x on 4 threads) | Default-build threads gave 0.8-1.1x |
| Hashing or compressing large buffers | Threads | hashlib released the GIL: 3.4-3.8x on 4 threads even with the GIL |
| Request = 25% Python CPU + 75% I/O | Threads up to about 4-8, then more processes | GIL ceiling about 4x per process (measured above) |
| Latency-sensitive I/O thread next to a CPU thread | Split into processes | A CPU hog added about 6.5 ms to a 1 ms sleep (CT5) |

**Follow-ups they ask:**

- "What is Gustafson's law?"
  The optimistic counterpart: with more workers you usually process a bigger problem in the same time, so the serial fraction shrinks relative to the work.
- "Why do measured speedups exceed Amdahl sometimes?"
  Cache effects (each worker's data fits in cache) or a wrong estimate of `p`; measure `p` rather than guessing it.

---

## CT33. Predict the output: what goes wrong with these threading snippets?

Each snippet was run and its output checked; say the answer first, then the reason.

**Snippet 1: the parentheses on `target`.**

```python
import threading


def work() -> None:
    print("working in", threading.current_thread().name)


t = threading.Thread(target=work(), name="worker")   # note the parentheses
t.start()
t.join()
print("done")
```

Output:

```text
working in MainThread
done
```

`work()` ran immediately in the main thread and passed `None` as the target, so the thread did nothing.

**Snippet 2: `run()` instead of `start()`.**

```python
import threading

show = lambda: print(threading.current_thread().name)
threading.Thread(target=show, name="worker-a").run()     # runs in the caller
b = threading.Thread(target=show, name="worker-b")
b.start()
b.join()
```

Output:

```text
MainThread
worker-b
```

**Snippet 3: starting a thread twice.**

```python
import threading

t = threading.Thread(target=lambda: None)
t.start()
t.join()
try:
    t.start()
except RuntimeError as exc:
    print(exc)
```

Output:

```text
threads can only be started once
```

**Snippet 4: an exception inside a thread.**

```python
import threading

threading.excepthook = lambda args: print("hook:", args.exc_type.__name__)


def boom() -> None:
    raise KeyError("missing")


t = threading.Thread(target=boom)
t.start()
t.join()
print("join returned normally, alive:", t.is_alive())
```

Output:

```text
hook: KeyError
join returned normally, alive: False
```

`join()` never re-raises; without a custom hook the traceback goes to stderr and the program carries on.

**Snippet 5: late binding in a comprehension of lambdas.**

```python
import threading

out: list[int] = []
threads = [threading.Thread(target=lambda: out.append(i)) for i in range(3)]
for t in threads:
    t.start()
for t in threads:
    t.join()
print(out)

out.clear()
threads = [threading.Thread(target=out.append, args=(i,)) for i in range(3)]
for t in threads:
    t.start()
for t in threads:
    t.join()
print(sorted(out))
```

Output:

```text
[2, 2, 2]
[0, 1, 2]
```

The lambdas close over the variable `i`, not its value, and run after the loop finished; pass values through `args`.

**Snippet 6: a failed future nobody looks at.**

```python
from concurrent.futures import ThreadPoolExecutor


def fail() -> None:
    raise RuntimeError("lost")


with ThreadPoolExecutor() as pool:
    pool.submit(fail)
print("no traceback anywhere")
```

Output:

```text
no traceback anywhere
```

The exception is stored in the future and discarded with it: a silent failure, and a classic production bug in fire-and-forget code.

**Snippet 7: where does `map` raise?**

```python
from concurrent.futures import ThreadPoolExecutor


def check(x: int) -> int:
    if x == 2:
        raise ValueError(x)
    return x


with ThreadPoolExecutor(max_workers=2) as pool:
    results = pool.map(check, range(4))
    print("map() returned without raising")
    try:
        for r in results:
            print("got", r)
    except ValueError as exc:
        print("raised while iterating, at item", exc)
```

Output:

```text
map() returned without raising
got 0
got 1
raised while iterating, at item 2
```

**Snippet 8: `Queue.join()` without `task_done()`.**

```python
import queue
import threading

q: queue.Queue[int] = queue.Queue()


def consumer() -> None:
    while True:
        q.get()                                   # forgot q.task_done()


threading.Thread(target=consumer, daemon=True).start()
q.put(1)
waiter = threading.Thread(target=q.join, daemon=True)
waiter.start()
waiter.join(timeout=0.2)
print("q.join() still blocked:", waiter.is_alive(), "| unfinished tasks:", q.unfinished_tasks)
```

Output:

```text
q.join() still blocked: True | unfinished tasks: 1
```

**Snippet 9: `notify()` without holding the lock.**

```python
import threading

cv = threading.Condition()
try:
    cv.notify()
except RuntimeError as exc:
    print(exc)
```

Output:

```text
cannot notify on un-acquired lock
```

**Snippet 10: daemon status is inherited.**

```python
import threading


def spawn_child() -> None:
    child = threading.Thread(target=lambda: None)
    print("child is daemon:", child.daemon)


parent = threading.Thread(target=spawn_child, daemon=True)
parent.start()
parent.join()
print("main-created thread is daemon:", threading.Thread(target=lambda: None).daemon)
```

Output:

```text
child is daemon: True
main-created thread is daemon: False
```

A thread's `daemon` flag defaults to its creator's, so helpers started from a daemon thread also die abruptly at exit.

---

## CT34. What concurrency pitfalls do you check for in code review?

- A shared counter, dict, or cache mutated with read-modify-write or check-then-act and no lock (CT6, CT21).
- "It works because of the GIL" reasoning; it breaks on 3.14t and when a key type has Python `__hash__`/`__eq__` (CT7, CT10).
- A lock held across I/O, a callback, logging to a slow handler, or a call into unknown code (CT23, CT24).
- Two locks acquired in different orders on different paths; no timeouts on locks that guard slow work (CT23).
- `Condition.wait()` under `if` instead of `while` or `wait_for` (CT17).
- `Queue.get()`, `join()`, `sock.recv()`, `future.result()` without timeouts in threads that must shut down (CT12, CT15).
- `task_done()` not in a `finally`, so one exception makes `Queue.join()` hang (CT20, CT33).
- Fire-and-forget `submit()` whose future is never checked (CT13, CT33).
- A task waiting on a future from the same bounded pool (CT30).
- `threading.local` state leaking between requests on pooled threads (CT14).
- `contextvars` assumed to flow into `run_in_executor` or new threads on the default build (CT14, CT30).
- Daemon threads doing writes that must complete (CT12).
- `fork()` (or the `fork` start method) after threads exist (CT25).
- CPU-bound Python in threads on the default build, or in `asyncio.to_thread` next to a latency-sensitive loop (CT8, CT30).
- Iterating a shared container while other threads write (CT22).
- Module-level mutable state in a multi-threaded web worker (see [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md)).

**Follow-ups they ask:**

- "What tooling helps?"
  `faulthandler` and `py-spy dump` for hangs, `sys.setswitchinterval` plus barriers for reproductions, the free-threaded build as a race amplifier, and ThreadSanitizer for C extensions.
  [fill in: a concurrency bug you found in review or production and how you proved the fix]

---

## Go deeper

- Pack: [Python core](02-Python-Core.md) Y9 and Y10, [Flask](03-Flask.md) F10 and F13, [FastAPI](04-FastAPI.md) A2, A18, A19, [Testing, debugging, production](10-Testing-Debugging-Production.md) T10, [Coding round](11-Coding-Round.md) K11 (thread-safe token bucket) and K12 (async fan-out), [Multiprocessing and parallelism](15-Multiprocessing-and-Parallelism.md), [Asyncio deep dive](16-Asyncio-Deep-Dive.md), [Concurrency in web services and coding](17-Concurrency-in-Web-Services-and-Coding.md).
- Vault, Python internals: [The GIL](../Python_Zero_to_Godhood/Chapter_10_CONCURRENCY_MECHANICS__THE_GLOBAL_INTERPRETER_LOCK.md), [Free-threaded Python internals](../Python_Zero_to_Godhood/Chapter_20_FREE-THREADED_PYTHON_GIL_REMOVAL_INTERNALS.md), [Subinterpreters](../Python_Zero_to_Godhood/Chapter_22_SUBINTERPRETERS__MULTI-CORE_PARALLELISM.md), [CPU- and I/O-bound concurrency (26)](../Python_Zero_to_Godhood/Chapter_26_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md), [CPU- and I/O-bound concurrency (27)](../Python_Zero_to_Godhood/Chapter_27_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md), [Shared memory and proxies](../Python_Zero_to_Godhood/Chapter_55_Advanced_Concurrency_Shared_Memory_and_Proxies.md), [Asyncio inception](../Python_Zero_to_Godhood/Volume_03_Generators_Iterators_and_Async_Inception/Chapter_10_Asyncio_Inception_Pathlib_and_Enum/Chapter_10_Asyncio_Inception_Pathlib_and_Enum.md).
- Vault, OS foundations: [Threads and concurrency](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L2-Threads-and-Concurrency.md), [PThreads case study](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L3-PThreads-Case-Study.md), [Thread performance](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L5-Thread-Performance-Considerations.md), [Synchronization constructs](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-3-Resource-Management/P3L4-Synchronization-Constructs.md), [Inter-process communication](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-3-Resource-Management/P3L3-Inter-Process-Communication.md), [The C++ memory model](../../C%2B%2B/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_76_The_CPP_Memory_Model/Chapter_76_The_CPP_Memory_Model.md) for contrast.
- Official: [threading](https://docs.python.org/3/library/threading.html), [queue](https://docs.python.org/3/library/queue.html), [concurrent.futures](https://docs.python.org/3/library/concurrent.futures.html), [Free-threading HOWTO](https://docs.python.org/3/howto/free-threading-python.html), [Thread safety guarantees](https://docs.python.org/3/library/threadsafety.html), [What's New in 3.14](https://docs.python.org/3/whatsnew/3.14.html), [PEP 703](https://peps.python.org/pep-0703/), [PEP 779](https://peps.python.org/pep-0779/), [faulthandler](https://docs.python.org/3/library/faulthandler.html), [contextvars](https://docs.python.org/3/library/contextvars.html), [py-spy](https://github.com/benfred/py-spy).
