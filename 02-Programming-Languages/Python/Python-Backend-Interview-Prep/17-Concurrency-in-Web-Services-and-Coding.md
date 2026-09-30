---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
sources: [https://docs.gunicorn.org/en/stable/design.html, https://docs.gunicorn.org/en/stable/settings.html, https://www.uvicorn.org/settings/, https://fastapi.tiangolo.com/async/, https://anyio.readthedocs.io/en/stable/threads.html, https://starlette.dev/threadpool/, https://flask.palletsprojects.com/en/stable/design/, https://flask.palletsprojects.com/en/stable/async-await/, https://docs.sqlalchemy.org/en/20/orm/contextual.html, https://docs.sqlalchemy.org/en/20/orm/extensions/asyncio.html, https://docs.sqlalchemy.org/en/20/core/pooling.html, https://www.postgresql.org/docs/current/explicit-locking.html, https://redis.io/docs/latest/develop/clients/patterns/distributed-locks/, https://redis.io/docs/latest/commands/set/, https://martin.kleppmann.com/2016/02/08/how-to-do-distributed-locking.html, https://docs.celeryq.dev/en/stable/userguide/workers.html, https://docs.python.org/3/library/threading.html, https://docs.python.org/3/library/asyncio-sync.html, https://docs.python.org/3/library/concurrent.futures.html, https://leetcode.com/problemset/concurrency/]
---

# Concurrency in web services and the concurrency coding round

Part 1 is how concurrency actually works inside a Flask or FastAPI service in production: processes, threads, the event loop, the database pool, and the races that cross process and machine boundaries.
Part 2 is the live concurrency coding round: the LeetCode concurrency set plus the problems backend interviews add (bounded queue, thread-safe LRU, parallel fetcher, async worker pool, periodic scheduler).
Near-certain: CW1 (the request's concurrency model and how many requests fit in flight), CW2 (FastAPI's threadpool), CW3 and CW4 (why in-memory state and read-modify-write break with N workers), and one of CW13, CW19, CW20, CW24, or CW26 as a live problem.
Primitives (locks, conditions, semaphores, the GIL, free-threading) are explained in [Concurrency fundamentals and threading](14-Concurrency-Fundamentals-and-Threading.md), processes in [Multiprocessing and parallelism](15-Multiprocessing-and-Parallelism.md), and the event loop in [Asyncio deep dive](16-Asyncio-Deep-Dive.md); this note applies them and does not re-explain them.

Every code sample here was run on 2026-09-30 under Python 3.14.7 with FastAPI 0.142.0, Starlette 1.7.0, anyio 4.15.1, uvicorn 0.54.0, gunicorn 26.2.0, Flask 3.1.3, SQLAlchemy 2.1.1, redis-py 8.1.0, and fakeredis 2.38.0 (gevent 26.9.0 in a separate venv): 111 pytest tests passed (74 behavior tests, several of them against real uvicorn and gunicorn servers on localhost ports, plus 37 checks that each module is byte-identical to its block here), and the 25 Part 2 tests also passed on the free-threaded 3.14.6 build with the GIL disabled.
Performance numbers are single-machine numbers from an Apple M3 Pro (12 cores) measured while other jobs kept the load average between roughly 40 and 70, so read them as ratios, not absolutes.

---

## Part 1: Concurrency in web services

CW1-CW11 are the questions, CW12 is predict-the-output, and a pitfalls list closes the part.

---

## CW1. Walk me through the concurrency model of a request end to end. How many requests can be in flight? (must know)

> "A request crosses four layers of concurrency, and the smallest one wins.
> The load balancer spreads connections over pods; in each pod a gunicorn or uvicorn master forks N worker processes that share the listening socket; inside each worker, requests run on threads (gunicorn `sync` or `gthread`), greenlets (`gevent`), or tasks on one event loop (uvicorn); and every request that touches the database needs a connection from that process's own pool.
> Capacity is the minimum of workers times per-worker concurrency and the database connections those workers can actually get, and throughput is that capacity divided by the time each request holds its slot (Little's law).
> Everything in process memory, including caches, counters, and locks, exists once per worker, so it is wrong the moment there are two workers."

```text
client -> load balancer (ALB / ingress)        spreads connections across pods (and keep-alive pins them)
            |
            v
   pod 1 .. pod R
     gunicorn or uvicorn master                  binds the socket, forks, restarts dead workers
       |-- worker process 1  (own memory, own GIL, own DB pool)
       |     sync:     1 request at a time
       |     gthread:  T OS threads, 1 request each
       |     gevent:   up to worker_connections greenlets, cooperative
       |     uvicorn:  1 event loop; async def = tasks, def = anyio threadpool (40 tokens)
       |        |
       |        v
       |     SQLAlchemy QueuePool: pool_size + max_overflow connections, waits pool_timeout
       |-- worker process 2 ... W
            |
            v
   PostgreSQL max_connections (or PgBouncer in front of it)
```

| Stack | Unit of concurrency | In flight per worker | A blocking call (DB, HTTP, sleep) | A CPU-bound call | Size the DB pool per worker to |
| --- | --- | --- | --- | --- | --- |
| Flask + gunicorn `sync` | process | 1 | blocks that worker only | blocks that worker only | 1-2 |
| Flask + gunicorn `gthread` | OS thread | `threads` (default 1) | blocks one thread | holds the GIL: slows every thread in the worker | `threads` |
| Flask + gunicorn `gevent` | greenlet | `worker_connections` (default 1000) | yields to other greenlets only if the library is monkey-patched or green; a C driver that is not blocks all of them | blocks every greenlet in the worker | peak concurrent DB work, not 1000 |
| FastAPI `async def` on uvicorn | task on one event loop | unbounded unless you bound it | blocks the loop: every request in the worker stalls | blocks the loop | the pool is your backpressure |
| FastAPI `def` on uvicorn | anyio worker thread | 40 (the default thread limiter) | blocks one thread and holds one token | GIL contention | at most the token count, or threads queue for connections |

**Measured** (Flask `/io` endpoint that does `time.sleep(0.2)` as a stand-in for a blocking DB call, 2 gunicorn workers, all requests fired at once over separate connections):

| Worker class | 8 concurrent requests | 40 concurrent requests | Ideal for 40 |
| --- | --- | --- | --- |
| `sync -w 2` | 0.81 s | 4.06 s | 40 x 0.2 / 2 = 4.0 s |
| `gthread -w 2 --threads 4` | 0.42 s | 1.24 s | 40 x 0.2 / 8 = 1.0 s |
| `gevent -w 2` | 0.21 s | 0.23 s | 0.2 s |

The `gthread` rows miss the ideal because a `gthread` worker accepts connections while `nr_conns < worker_connections` (1000 by default), not while it has a free thread: in four trials of 8 requests, one worker took 8/0, 7/1, 5/3, and 4/4 of them, so requests queue behind busy threads in one worker while the other idles.
That is visible in gunicorn's `workers/gthread.py`, and it is why per-worker thread counts do not simply add up.

**Worked example: how many requests can be in flight?**

Flask on gunicorn, 3 pods, 4 `gthread` workers per pod, 8 threads per worker:

```text
HTTP concurrency  = 3 pods x 4 workers x 8 threads                 = 96 requests executing
DB connections    = 3 x 4 x (pool_size 5 + max_overflow 10)         = 180 possible  > max_connections 100
fix               = pool_size=8, max_overflow=0 per worker          = 96 connections, one per thread
throughput bound  = 96 in flight / 0.050 s per request (Little's law) = 1,920 requests/s
```

Past 96, requests wait in the worker's accepted-connection queue and then the kernel backlog (gunicorn `backlog` defaults to 2048); latency grows before anything fails.

FastAPI on uvicorn, 3 pods, 2 workers per pod:

```text
async def + asyncpg, pool_size=10, max_overflow=0:
  HTTP in flight   = unbounded per worker (thousands of cheap tasks)
  DB in flight     = 3 x 2 x 10 = 60; request 61 awaits a connection for up to pool_timeout
def + sync driver, same pool:
  HTTP in flight   = 3 x 2 x 40 tokens = 240 threads
  DB in flight     = 60; the other 30 threads per worker block in pool checkout while holding tokens,
                     so every other def endpoint (and a def health check) queues behind them
```

**Follow-ups they ask:**

- "Why several processes instead of more threads?"
  The GIL on the default build lets one thread run Python bytecode at a time per process; processes give CPU parallelism and crash isolation, threads give cheap I/O concurrency.
  Details: [GIL and free-threading](14-Concurrency-Fundamentals-and-Threading.md) and [processes](15-Multiprocessing-and-Parallelism.md).
- "How many workers?"
  Gunicorn's `2 x cores + 1` is a starting point for sync workers; async workers need about one per core; containers should set it explicitly because `os.cpu_count()` sees host cores (see [Flask F10](03-Flask.md)).
- "Where does a request wait when everything is busy?"
  Kernel accept backlog, then the worker's internal queue, then the threadpool limiter or the DB pool; measure each (queue time is the first symptom of saturation, before errors).
- "What about the free-threaded build?"
  It lets `gthread` and FastAPI `def` threads run Python in parallel, which makes CPU-heavy views scale by threads, and also makes latent data races in shared module state far more likely (CW12, snippet 5).
- "Give me the numbers for a service you ran."
  **[fill in: workers per pod, threads or async, pool_size and max_overflow, replicas, the database's max_connections, and what saturated first]**

---

## CW2. In FastAPI, where do `def` endpoints run, what is the 40-thread limit, and what happens to request 41? (must know)

> "`async def` endpoints run as tasks on the worker's event loop.
> Plain `def` endpoints and plain `def` dependencies are sent to a worker thread with `anyio.to_thread.run_sync`, governed by anyio's default `CapacityLimiter`, which has 40 tokens per event loop, so 40 per uvicorn worker process.
> Request 41 is not rejected: its coroutine awaits a token and simply waits until a thread frees up, so latency doubles for the overflow while the loop keeps serving `async def` routes.
> The limit is a property you can raise in `lifespan`, but the database pool is usually the real ceiling, and threads waiting for pool connections hold tokens, which starves every other `def` endpoint."

Verified in the installed packages: `anyio/_backends/_asyncio.py` creates `CapacityLimiter(40)` as the default thread limiter (a `RunVar`, so one per event loop); `fastapi/routing.py` runs a non-coroutine endpoint through `run_in_threadpool`, and `starlette/concurrency.py` implements `run_in_threadpool` as `anyio.to_thread.run_sync`.
The same 40 tokens are shared by: `def` endpoints, `def` dependencies (a sync `yield` dependency's setup runs through `contextmanager_in_threadpool`; its teardown uses a fresh one-token limiter per call, in `fastapi/concurrency.py`, so cleanup cannot deadlock on a full pool), response-model validation for `def` endpoints, `UploadFile` reads and writes once the upload has spilled to disk, `StreamingResponse` over a sync iterator (`iterate_in_threadpool`), and every explicit `run_in_threadpool` or `anyio.to_thread.run_sync` call.
`asyncio.to_thread` is different: it uses the loop's default `ThreadPoolExecutor` (`min(32, os.process_cpu_count() + 4)` workers in 3.14), not anyio's limiter.

```python
# fastapi_concurrency.py
import asyncio
import os
import time
from collections.abc import AsyncIterator
from concurrent.futures import ProcessPoolExecutor
from contextlib import asynccontextmanager

import anyio.to_thread
from fastapi import FastAPI, Request
from fastapi.concurrency import run_in_threadpool


def count_primes(limit: int) -> int:
    """CPU-bound work: must be a module-level function so a worker process can import it."""
    sieve = bytearray([1]) * (limit + 1)
    sieve[0:2] = b"\x00\x00"
    for i in range(2, int(limit**0.5) + 1):
        if sieve[i]:
            sieve[i * i :: i] = bytes(len(range(i * i, limit + 1, i)))
    return sum(sieve)


@asynccontextmanager
async def lifespan(app: FastAPI) -> AsyncIterator[None]:
    # The limiter is per event loop, so set it inside the loop (lifespan), once per worker process.
    limiter = anyio.to_thread.current_default_thread_limiter()
    limiter.total_tokens = int(os.environ.get("THREADPOOL_TOKENS", "40"))
    with ProcessPoolExecutor(max_workers=2) as pool:  # created per worker, after gunicorn/uvicorn forks
        app.state.cpu_pool = pool
        yield  # on shutdown the with-block waits for running CPU jobs, then stops the processes


app = FastAPI(lifespan=lifespan)


@app.get("/healthz")
async def healthz() -> dict:
    return {"ok": True}


@app.get("/sync-sleep")
def sync_sleep() -> dict:
    time.sleep(0.2)  # a blocking driver call: fine in def, it runs on an anyio worker thread
    return {"ok": True}


@app.get("/async-blocking")
async def async_blocking() -> dict:
    time.sleep(0.1)  # BUG: blocks the event loop; every request in this process waits
    return {"ok": True}


@app.get("/async-nonblocking")
async def async_nonblocking() -> dict:
    await asyncio.sleep(0.1)
    return {"ok": True}


@app.get("/async-offloaded")
async def async_offloaded() -> dict:
    await run_in_threadpool(time.sleep, 0.1)  # same threadpool (and token budget) as def endpoints
    return {"ok": True}


@app.get("/primes/{limit}")
async def primes(limit: int, request: Request) -> dict:
    loop = asyncio.get_running_loop()
    count = await loop.run_in_executor(request.app.state.cpu_pool, count_primes, limit)
    return {"limit": limit, "primes": count}


@app.get("/threadpool")
async def threadpool() -> dict:
    limiter = anyio.to_thread.current_default_thread_limiter()
    return {"total": limiter.total_tokens, "borrowed": limiter.borrowed_tokens}
```

The load generator used for every measurement in this note (separate connections, no keep-alive, all requests launched at once):

```python
# load.py
import asyncio
import time

import httpx


async def fire(url: str, n: int, method: str = "GET") -> tuple[float, list[float], list[int]]:
    """Send n requests at once over n connections; return wall time, per-request latencies, statuses."""
    limits = httpx.Limits(max_connections=n, max_keepalive_connections=0)
    async with httpx.AsyncClient(limits=limits, timeout=30.0) as client:

        async def one() -> tuple[float, int]:
            start = time.perf_counter()
            response = await client.request(method, url)
            return time.perf_counter() - start, response.status_code

        start = time.perf_counter()
        results = await asyncio.gather(*(one() for _ in range(n)))
        wall = time.perf_counter() - start
    return wall, sorted(r[0] for r in results), [r[1] for r in results]
```

**Measured** on one uvicorn worker:

| Scenario | Requests | Wall time | Per-request latency |
| --- | --- | --- | --- |
| `def` endpoint, `time.sleep(0.2)`, 40 tokens | 50 | 0.44 s | 40 at about 0.24 s, the other 10 at about 0.43 s |
| same, `THREADPOOL_TOKENS=100` | 50 | 0.23 s | all about 0.22 s |
| `async def` with `time.sleep(0.1)` (blocking) | 10 | 1.05 s | 0.11 s to 1.05 s: served one at a time |
| `async def` with `await asyncio.sleep(0.1)` | 10 | 0.11 s | all 0.11 s |
| `async def` with `await run_in_threadpool(time.sleep, 0.1)` | 10 | 0.12 s | all 0.12 s |

The tests assert mechanism rather than timings, so they hold on a loaded machine: an `async def` route polls `limiter.borrowed_tokens` during the burst, and the peak is exactly 40 with the default limiter (and the burst needs a second 0.2 s round) and exactly 50 with 100 tokens.
Every request returns 200: request 41 waits for a token, it is not rejected.

**How to change the limit:** set `anyio.to_thread.current_default_thread_limiter().total_tokens` inside `lifespan` (it must run inside the event loop, once per worker process), as above, or pass your own `CapacityLimiter` to `anyio.to_thread.run_sync(fn, limiter=...)` to give one slow dependency its own budget (a bulkhead, CW9).
Raise it only together with the DB pool and after measuring; 200 threads each waiting on a 10-connection pool is worse than 40.

**`run_in_threadpool` vs `asyncio.to_thread`:** both move one blocking call off the loop; `run_in_threadpool` shares the 40-token budget with `def` endpoints, `asyncio.to_thread` uses the default executor, and both copy the current `contextvars` context into the thread (which has a consequence: CW12, snippet 1).

**CPU-bound work** is CW10.

**Follow-ups they ask:**

- "So should everything be `async def`?"
  Only if every call in it is non-blocking; one `requests.get` or sync ORM query in an `async def` stalls the whole worker (row 3 above).
  A `def` endpoint with a sync driver is a legitimate, simpler design; its concurrency is simply capped by tokens and pool size.
- "How do you see token starvation in production?"
  p99 latency of `def` routes rises while CPU is low and `async def` routes are fine; expose `limiter.borrowed_tokens` as a gauge (the `/threadpool` route above), and look for threads parked in `QueuePool` checkout in a `py-spy dump`.
- "What does uvicorn's `--limit-concurrency` do?"
  It is a connection-count breaker, not a queue: CW9.

---

## CW3. Why is an in-memory counter, cache, or rate limiter wrong in production, and what do you use instead? (must know)

> "Each gunicorn or uvicorn worker is a separate process with its own copy of every module global, and each pod is a separate machine, so an in-memory counter counts only the requests that one worker happened to receive.
> A `threading.Lock` fixes races between threads inside one worker and does nothing across workers or pods.
> Shared mutable state belongs in a shared store with atomic operations: the database (`UPDATE ... SET n = n + 1`, unique constraints) or Redis (`INCR`, `SET NX`, Lua scripts that run atomically).
> Sticky sessions only hide the problem: they break on scale-out, deploys, and failover, and they unbalance load."

```python
# counter_app.py
import os
import threading

import redis
from flask import Flask

_hits = 0  # BUG in production: one copy per worker process, and unguarded across threads
_hits_lock = threading.Lock()


def create_app() -> Flask:
    app = Flask(__name__)
    store = redis.Redis.from_url(os.environ.get("REDIS_URL", "redis://127.0.0.1:6379/0"))

    @app.get("/healthz")
    def healthz():
        return {"ok": True}

    @app.post("/hits/memory")
    def hit_memory():
        global _hits
        with _hits_lock:  # the lock fixes threads inside one process, not processes or pods
            _hits += 1
            return {"pid": os.getpid(), "count": _hits}

    @app.post("/hits/redis")
    def hit_redis():
        return {"pid": os.getpid(), "count": store.incr("hits")}  # atomic on the Redis server

    return app
```

**Measured** under `gunicorn 'counter_app:create_app()' -w 2 -k sync`, 40 concurrent POSTs to each route, Redis served by fakeredis's `TcpFakeServer` over real TCP:

| Route | Requests served per worker | Highest count any response reported |
| --- | --- | --- |
| `/hits/memory` | 20 and 20 | 20 (two separate counters, each "correct") |
| `/hits/redis` | 20 and 20 | 40, and the 40 responses carried exactly 1..40 |

The test asserts the invariant rather than the split: the per-worker maxima sum to 40, no single worker reports 40 when both served traffic, and Redis returns every value from 1 to 40 exactly once.

**What to use, by kind of state:**

| State | Wrong | Right |
| --- | --- | --- |
| Counter, quota | module global, `itertools.count()` | Redis `INCR` / `INCRBY` (atomic), or `UPDATE ... SET n = n + 1` |
| Rate limiter | per-process token bucket (limit silently becomes W x R times larger) | Redis script that does INCR and EXPIRE atomically (CW9), or the API gateway |
| Cache | per-process dict: fine for immutable reference data, wrong for anything invalidated | Redis with TTL; per-process `lru_cache` only for data that never changes during the process lifetime |
| Session / login state | server memory plus sticky sessions | signed cookie or JWT, or a server-side session in Redis |
| "Run once" job, leader | "the first worker that starts" | a lock or lease in Redis or PostgreSQL (CW7, CW8) |
| WebSocket fan-out | in-process list of connections | Redis pub/sub or a broker, each worker delivers to its own connections |

**Why Lua:** a Redis script runs atomically on the server, so "INCR, and if this is the first hit set an expiry" cannot be interleaved with another client, and a crash between the two commands cannot leave a counter without a TTL.
`MULTI`/`EXEC` also makes a group atomic, but cannot branch on a value read inside it.

**Follow-ups they ask:**

- "Why not sticky sessions?"
  They pin a user to a pod, so the state dies with the pod, scale-out moves users, a hot user overloads one pod, and the counter is still wrong across users.
- "Is `lru_cache` ever fine in a web worker?"
  Yes for pure functions of immutable inputs (parsed config, compiled regexes), with the caveat that concurrent first calls all compute (CW12, snippet 2).

---

## CW4. Two requests update the same row at once. What goes wrong, and how do you fix it? (must know)

> "The classic bug is read-modify-write in application code: both requests read balance 100, both compute 110, both write 110, and one deposit is lost; an ORM hides it because `account.balance += 10` looks atomic but is a SELECT now and an UPDATE later.
> The fixes, in the order I reach for them: make the database do the arithmetic (`UPDATE ... SET balance = balance + 10`), lock the row for the read-decide-write (`SELECT ... FOR UPDATE`), or detect the conflict with a version column and retry or return 409.
> The sibling bug is check-then-insert (double booking): both requests check that the seat is free, both insert; the fix is a unique constraint so the database decides the winner, and the loser gets an `IntegrityError` you map to 409.
> For retried POSTs, an idempotency key with a unique constraint makes the retry return the first result instead of charging twice."

The races are made deterministic in the tests: a `between` hook parks both requests on a `threading.Barrier(2)` after the read and before the write, so the bad interleaving happens every time instead of once a week.

```python
# data_races.py
from collections.abc import Callable

from sqlalchemy import ForeignKey, String, UniqueConstraint, select, update
from sqlalchemy.exc import IntegrityError
from sqlalchemy.orm import DeclarativeBase, Mapped, mapped_column, sessionmaker


class Base(DeclarativeBase):
    pass


class Account(Base):
    __tablename__ = "accounts"
    id: Mapped[int] = mapped_column(primary_key=True)
    balance: Mapped[int]
    version: Mapped[int] = mapped_column(default=1)


class BookingNoConstraint(Base):
    __tablename__ = "bookings_unsafe"
    id: Mapped[int] = mapped_column(primary_key=True)
    room: Mapped[int]
    night: Mapped[str] = mapped_column(String(10))
    guest: Mapped[str]


class Booking(Base):
    __tablename__ = "bookings"
    __table_args__ = (UniqueConstraint("room", "night"),)  # the database enforces "one guest per room-night"
    id: Mapped[int] = mapped_column(primary_key=True)
    room: Mapped[int]
    night: Mapped[str] = mapped_column(String(10))
    guest: Mapped[str]


class Payment(Base):
    __tablename__ = "payments"
    id: Mapped[int] = mapped_column(primary_key=True)
    amount: Mapped[int]


class IdempotencyKey(Base):
    __tablename__ = "idempotency_keys"
    key: Mapped[str] = mapped_column(primary_key=True)  # unique: the second insert fails
    payment_id: Mapped[int] = mapped_column(ForeignKey("payments.id"))


def noop() -> None:
    pass


def deposit_naive(Session: sessionmaker, account_id: int, amount: int,
                  between: Callable[[], None] = noop) -> None:
    with Session.begin() as session:
        account = session.get(Account, account_id)
        between()  # tests park both requests here, after the read and before the write
        account.balance += amount  # BUG: read-modify-write in Python; the last writer wins


def deposit_atomic(Session: sessionmaker, account_id: int, amount: int) -> None:
    with Session.begin() as session:
        session.execute(  # UPDATE accounts SET balance = balance + ? WHERE id = ?
            update(Account).where(Account.id == account_id).values(balance=Account.balance + amount)
        )


def deposit_optimistic(Session: sessionmaker, account_id: int, amount: int,
                       between: Callable[[], None] = noop, max_attempts: int = 5) -> int:
    """Compare-and-set on a version column; returns the number of attempts used."""
    for attempt in range(1, max_attempts + 1):
        with Session.begin() as session:
            balance, version = session.execute(
                select(Account.balance, Account.version).where(Account.id == account_id)
            ).one()
            if attempt == 1:
                between()
            result = session.execute(
                update(Account)
                .where(Account.id == account_id, Account.version == version)
                .values(balance=balance + amount, version=version + 1)
            )
            if result.rowcount == 1:
                return attempt
        # rowcount 0: someone else committed first; re-read and retry (or return 409 to the client)
    raise RuntimeError("too much contention")


def book_naive(Session: sessionmaker, room: int, night: str, guest: str,
               between: Callable[[], None] = noop) -> bool:
    with Session.begin() as session:
        taken = session.scalar(
            select(BookingNoConstraint).where(BookingNoConstraint.room == room,
                                              BookingNoConstraint.night == night)
        )
        between()
        if taken:  # BUG: check-then-insert; both requests pass the check
            return False
        session.add(BookingNoConstraint(room=room, night=night, guest=guest))
        return True


def book(Session: sessionmaker, room: int, night: str, guest: str) -> bool:
    """Just insert; the unique constraint decides the winner atomically. False maps to 409."""
    try:
        with Session.begin() as session:
            session.add(Booking(room=room, night=night, guest=guest))
        return True
    except IntegrityError:
        return False


def create_payment(Session: sessionmaker, idempotency_key: str, amount: int) -> int:
    """Retried POSTs with the same key create exactly one payment and all get its id."""
    try:
        with Session.begin() as session:
            payment = Payment(amount=amount)
            session.add(payment)
            session.flush()  # assigns payment.id
            session.add(IdempotencyKey(key=idempotency_key, payment_id=payment.id))
            return payment.id  # commit happens when the with-block exits
    except IntegrityError:  # another request with this key committed first; our payment rolled back
        with Session() as session:
            return session.get_one(IdempotencyKey, idempotency_key).payment_id
```

What the tests (SQLite file database, separate connections per thread) show:

| Function | Scenario | Result |
| --- | --- | --- |
| `deposit_naive` | two deposits of 10, both read before either writes | balance 110: one update lost |
| `deposit_atomic` | 8 threads, 200 deposits of 1 | 300 exactly |
| `deposit_optimistic` | same two-request interleaving | 120; attempts `[1, 2]`: the loser saw `rowcount == 0` and retried |
| `book_naive` | two bookings of room 101 for one night | both return True, 2 rows |
| `book` | 8 threads booking the same room-night, 20 rounds | exactly one True per round |
| `create_payment` | 6 concurrent retries with one idempotency key, 10 keys | 10 payments, every retry got the same id |

**PostgreSQL versions** (PostgreSQL-only syntax, not executed here):

```sql
-- 1. Atomic update: no read in the application at all.
UPDATE accounts SET balance = balance + 10 WHERE id = 1;

-- 2. Pessimistic: lock the row, decide in code, write, commit (keep the transaction short).
BEGIN;
SELECT balance FROM accounts WHERE id = 1 FOR UPDATE;   -- a second transaction blocks here
UPDATE accounts SET balance = 90 WHERE id = 1;
COMMIT;

-- 3. Optimistic: compare-and-set on a version column; 0 rows updated means someone else won.
UPDATE accounts SET balance = 110, version = version + 1 WHERE id = 1 AND version = 7;

-- 4. Double booking: the constraint is the lock.
ALTER TABLE bookings ADD CONSTRAINT one_guest_per_room_night UNIQUE (room, night);
INSERT INTO bookings (room, night, guest) VALUES (101, '2026-10-01', 'x')
ON CONFLICT (room, night) DO NOTHING RETURNING id;      -- no row back: already booked, return 409
```

In SQLAlchemy the pessimistic form is `select(Account).where(Account.id == 1).with_for_update()`; the test compiles it with the PostgreSQL dialect and checks it ends in `FOR UPDATE` (SQLite ignores it).

**Which fix when:**

| Situation | Fix |
| --- | --- |
| Arithmetic on a column (counters, balances, stock) | atomic `UPDATE ... SET x = x + :d`, plus a `CHECK (balance >= 0)` or a `WHERE balance >= :amount` guard |
| Decision needs the current value and is short | `SELECT ... FOR UPDATE` (`nowait=True` or `skip_locked=True` when waiting is wrong) |
| Read and write are separated by user think time or HTTP calls | version column (optimistic), expose it as an `ETag`, require `If-Match` |
| "At most one X per Y" | unique constraint (or an exclusion constraint for overlapping ranges in PostgreSQL) |
| Client retries a POST | idempotency key with a unique constraint, store the response |

**Isolation levels are not a substitute you get for free:** PostgreSQL's default READ COMMITTED allows the lost update above; REPEATABLE READ turns it into a serialization error (`40001`) on the second writer, and SERIALIZABLE also catches write skew, but both require a retry loop around the whole transaction.
Details and the anomaly table: [SQL and SQLAlchemy S9 and S10](06-SQL-and-SQLAlchemy.md); the in-process version of the same races is [CT21](14-Concurrency-Fundamentals-and-Threading.md).

**Follow-ups they ask:**

- "Will `SELECT ... FOR UPDATE` deadlock?"
  It can when two transactions lock the same rows in different orders; lock in a consistent order (sort ids), keep transactions short, and retry on `40P01` ([S11](06-SQL-and-SQLAlchemy.md)).
- "Have you hit one in production?"
  **[fill in: a lost update, double submit, or duplicate job you found, how you proved it (logs, a reproduction with a barrier), and which fix you shipped]**
- "Does SQLAlchemy protect me?"
  `version_id_col` gives optimistic locking for ORM updates; nothing protects a plain `obj.x += 1` (CW12, snippet 4).

---

## CW5. Is Flask thread-safe? What about `g`, `request`, module globals, the dev server, gevent, and async views?

> "Flask itself is: `request`, `session`, `g`, and `current_app` are proxies to context variables (`flask.globals` defines `_cv_app` and `_cv_request` as `ContextVar`s), so each thread or greenlet sees its own request and its own `g`.
> What is not thread-safe is anything I put at module level: a global 'current user', a dict used as a cache, a client object whose library is not thread-safe.
> The dev server has used threads by default since Flask 1.0 (`app.run` sets `threaded=True`), so those bugs show up even locally; gevent makes them show up at every I/O call, because greenlets switch there."

```python
# flask_state.py
import os
import threading
import time

from flask import Flask, g, request

_current_user = None  # BUG: module global shared by every thread (and greenlet) in the process


def create_app() -> Flask:
    app = Flask(__name__)

    @app.get("/healthz")
    def healthz():
        return {"ok": True}

    @app.get("/io")
    def io_bound():
        time.sleep(0.2)  # stands in for a blocking DB or HTTP call
        return {"pid": os.getpid(), "thread": threading.current_thread().name}

    @app.get("/user/global")
    def user_global():
        global _current_user
        _current_user = request.headers["X-User"]
        time.sleep(0.05)  # any I/O here lets another request overwrite the global
        return {"header": request.headers["X-User"], "seen": _current_user}

    @app.get("/user/g")
    def user_g():
        g.user = request.headers["X-User"]  # g lives in the app context: one per request
        time.sleep(0.05)
        return {"header": request.headers["X-User"], "seen": g.user}

    return app
```

Test: 8 threads send concurrent requests through the Flask test client, each with its own `X-User` header, released together by a barrier.
With `g`, every response saw its own user; with the module global, at least one response returned another request's user (in practice most of them do, because each request overwrites the global during the 50 ms sleep).

**Pitfalls:**

- **Module-level mutable state:** a dict cache without a lock can be read mid-update on the free-threaded build and can serve stale or cross-user data on any build; use `g` for per-request data and Redis or a locked structure for shared data.
- **Clients:** `requests.Session` is widely shared across threads but its thread-safety is not guaranteed by its docs; for any client, check the library's thread-safety notes before sharing one per worker; one SQLAlchemy `Engine` per process, one `Session` per request (CW6).
- **gevent monkey-patching:** patch before anything else imports `socket`, `ssl`, `threading`, or `time` (gunicorn's gevent worker calls `monkey.patch_all()` when the worker starts, which is too late for code that ran at import time in the master under `--preload`); C drivers like psycopg2 block the whole worker unless made green (for example with `psycogreen`); one CPU-heavy request stalls every greenlet in the worker; `threading.local` becomes greenlet-local after patching, which can surprise code that expected per-thread state.
- **Async views:** `async def` views work with `flask[async]`, but each request still occupies a WSGI worker thread while asgiref runs the coroutine on a separate event loop thread; details and a test are in [Flask F13](03-Flask.md).
- **Background threads started in a view** lose the request context when the view returns; pass plain data, not `request` or ORM objects, and prefer a task queue (CW8).

---

## CW6. How do SQLAlchemy sessions interact with threads, tasks, and workers?

> "An `Engine` and its pool are per process and thread-safe; a `Session` is not thread-safe and an `AsyncSession` is not safe to share between concurrent tasks, so it is one session per request, per task, or per unit of work, never a global.
> In Flask I use a session per request through a request-scoped dependency or `scoped_session`; in FastAPI a `yield` dependency that opens and closes one session per request.
> The pool sizes are per process, so the fleet total is replicas x workers x (pool_size + max_overflow), and it must fit under the database's `max_connections`.
> Under load, requests beyond the pool wait `pool_timeout` (30 s by default) and then fail with `QueuePool limit ... reached`, which is the signal to fix slow transactions or add backpressure, not to raise the pool size blindly."

```python
# sa_sessions.py
import asyncio

from sqlalchemy import create_engine, text
from sqlalchemy.ext.asyncio import AsyncEngine, async_sessionmaker
from sqlalchemy.orm import scoped_session, sessionmaker


def make_engine(url: str, pool_size: int, max_overflow: int, pool_timeout: float):
    # Engine and pool: one per process, created after fork, shared by all threads in the process.
    return create_engine(url, pool_size=pool_size, max_overflow=max_overflow,
                         pool_timeout=pool_timeout, pool_pre_ping=True)


def make_scoped(engine) -> scoped_session:
    # scoped_session: a registry that hands each thread its own Session (thread-local by default).
    return scoped_session(sessionmaker(engine, expire_on_commit=False))


async def one_session_per_task(engine: AsyncEngine, n: int) -> list[int]:
    """Concurrent tasks each open their own AsyncSession; never gather over one shared session."""
    SessionLocal = async_sessionmaker(engine, expire_on_commit=False)

    async def work(i: int) -> int:
        async with SessionLocal() as session:
            return (await session.execute(text("SELECT :i"), {"i": i})).scalar_one()

    async with asyncio.TaskGroup() as tg:
        tasks = [tg.create_task(work(i)) for i in range(n)]
    return [t.result() for t in tasks]
```

Verified by the tests:

- `scoped_session` returned four distinct sessions to four threads and the same session twice within a thread; `Session.remove()` at the end of the request closes it.
- With `pool_size=2, max_overflow=0, pool_timeout=0.3`, two threads holding connections made a third `engine.connect()` raise `sqlalchemy.exc.TimeoutError: QueuePool limit of size 2 overflow 0 reached, connection timed out, timeout 0.30` after about 0.3 s.
- 20 tasks each with their own `AsyncSession` ran concurrently under a `TaskGroup`; `asyncio.gather` over one shared `AsyncSession` failed with `InvalidRequestError: This session is provisioning a new connection; concurrent operations are not permitted`.

**Pool sizing across workers** (the full table is in [S18](06-SQL-and-SQLAlchemy.md)):

```text
fleet connections = max_replicas_during_deploy x workers_per_pod x (pool_size + max_overflow)
per-worker pool   = its real concurrency: 1-2 for sync, threads for gthread,
                    <= 40 for FastAPI def routes, "whatever backpressure you want" for async
```

**Follow-ups they ask:**

- "Why not one global session with a lock?"
  The session's identity map and transaction would be shared by unrelated requests: one request's rollback discards another's changes, and the lock serializes the worker.
- "What about `--preload` and fork?"
  An Engine created in the master before fork shares sockets with every worker; create it in the worker (app factory, lifespan) or call `engine.dispose(close=False)` in `post_fork`.

---

## CW7. How do you implement a distributed lock, and what can go wrong?

> "With Redis: `SET lock:name <random token> NX PX <lease>` acquires only if the key is absent and gives it an expiry in one atomic command, so a crashed holder cannot keep the lock forever.
> Release must check the token and delete in one step, which is a small Lua script; a plain `DEL` can delete a lock that expired and was taken by someone else.
> The lease is the weak point: a GC pause or a slow call longer than the lease lets two holders run at once, so the protected resource must reject stale holders with a fencing token, a number that only increases, which the storage checks.
> Redlock's multi-node scheme does not remove that problem because it still depends on timing assumptions, which is Martin Kleppmann's critique; for correctness I prefer the database: a PostgreSQL advisory lock or a row lock inside the transaction that does the work."

```python
# redis_lock.py
import secrets
import time
from collections.abc import Iterator
from contextlib import contextmanager

import redis

# Delete the key only if it still holds our token: GET and DEL must be one atomic step.
RELEASE = """
if redis.call("GET", KEYS[1]) == ARGV[1] then
    return redis.call("DEL", KEYS[1])
end
return 0
"""

# Extend the lease only if we still own it.
EXTEND = """
if redis.call("GET", KEYS[1]) == ARGV[1] then
    return redis.call("PEXPIRE", KEYS[1], ARGV[2])
end
return 0
"""


class LockLost(Exception):
    pass


class RedisLock:
    def __init__(self, client: redis.Redis, name: str, lease_ms: int = 10_000) -> None:
        self.client, self.key, self.lease_ms = client, f"lock:{name}", lease_ms
        self.token: str | None = None
        self.fence: int | None = None

    def acquire(self, wait_s: float = 0.0, retry_s: float = 0.01) -> bool:
        token = secrets.token_hex(16)
        deadline = time.monotonic() + wait_s
        while True:
            # SET key token NX PX lease: create only if absent, with an expiry, in one command.
            if self.client.set(self.key, token, nx=True, px=self.lease_ms):
                self.token = token
                self.fence = self.client.incr(f"{self.key}:fence")  # monotonically increasing
                return True
            if time.monotonic() >= deadline:
                return False
            time.sleep(retry_s)

    def extend(self) -> None:
        if not self.client.eval(EXTEND, 1, self.key, self.token, self.lease_ms):
            raise LockLost(self.key)

    def release(self) -> bool:
        released = bool(self.client.eval(RELEASE, 1, self.key, self.token))
        self.token = None
        return released  # False: the lease expired and someone else may hold the lock now


@contextmanager
def redis_lock(client: redis.Redis, name: str, lease_ms: int = 10_000,
               wait_s: float = 5.0) -> Iterator[RedisLock]:
    lock = RedisLock(client, name, lease_ms)
    if not lock.acquire(wait_s):
        raise TimeoutError(f"could not acquire {name}")
    try:
        yield lock
    finally:
        lock.release()


class FencedStore:
    """The protected resource checks the fencing token, so a stale holder cannot write."""

    def __init__(self) -> None:
        self.highest_fence = 0
        self.value: str | None = None

    def write(self, fence: int, value: str) -> None:
        if fence < self.highest_fence:
            raise PermissionError(f"stale fencing token {fence} < {self.highest_fence}")
        self.highest_fence, self.value = fence, value
```

Tested with fakeredis (which runs the Lua scripts through `lupa`):

- 8 threads doing 80 read-sleep-write increments under the lock: never more than one inside, final total exactly 80.
- A holder whose 50 ms lease expired: the next client acquires, the stale holder's `release()` returns False and leaves the new owner's key intact, and its `extend()` raises `LockLost`.
- The naive version (plain `DEL`): holder A's late release deletes B's lock and a third client gets in while B still thinks it holds it.
- Fencing: B's fence is larger than A's, so `FencedStore.write` rejects A's late write with `PermissionError`.

redis-py ships the same pattern as `client.lock(name, timeout=..., blocking_timeout=...)` (token plus Lua release in `redis/lock.py`); use it rather than your own in production, and add fencing yourself.

**PostgreSQL advisory locks** (PostgreSQL-only, not executed here):

```sql
-- Session-level: held until unlocked or the connection closes (breaks under PgBouncer transaction mode).
SELECT pg_try_advisory_lock(hashtext('nightly-report'));   -- true if we got it
SELECT pg_advisory_unlock(hashtext('nightly-report'));

-- Transaction-level: released automatically at COMMIT or ROLLBACK; safe with transaction pooling.
BEGIN;
SELECT pg_try_advisory_xact_lock(hashtext('nightly-report'));
-- do the work in this same transaction
COMMIT;
```

| Option | Good for | Watch out for |
| --- | --- | --- |
| Redis `SET NX PX` + Lua release | short leases, efficiency ("do not run this twice at once") | lease expiry, Redis failover losing the key, no fencing unless you add it |
| Redlock (N independent Redis nodes) | surviving one Redis node failure | clock and pause assumptions; debated for correctness |
| PostgreSQL advisory lock | coordinating work that writes to the same database | session locks and PgBouncer transaction mode; lock held while the connection lives |
| Row lock / unique constraint | correctness of the data itself | long transactions, deadlocks |
| ZooKeeper / etcd lease with revision | leader election, fencing via the revision number | another system to run |

---

## CW8. BackgroundTasks, threads, or Celery? How do you keep background work and cron jobs safe with many replicas?

> "FastAPI `BackgroundTasks` run in the same worker after the response is sent: fine for best-effort work like a log line or a cache warm, lost if the process restarts, and a `def` task still takes a threadpool token.
> A thread started from a request has the same durability problem plus lifecycle problems at shutdown.
> Anything that must happen goes through a durable queue: Celery, RQ, Dramatiq, or arq (asyncio), with late acknowledgement and idempotent tasks, because at-least-once delivery means a task can run twice.
> Scheduled jobs are the other trap: every replica runs its own scheduler, so a 'nightly' job runs R times unless one replica is elected or each run claims a lock for its time slot."

| Mechanism | Runs where | Survives restart | Retries | Use for |
| --- | --- | --- | --- | --- |
| `BackgroundTasks` | same worker, after the response | no | no | best-effort side effects |
| `threading.Thread` / executor | same worker | no | no | in-process batching with a clean shutdown (CW11) |
| Celery / RQ / Dramatiq / arq | separate worker fleet via a broker | yes (with acks late) | yes | emails, exports, payments follow-up |
| Kafka consumer | separate service | yes (committed offsets) | by design | event processing at volume |

**Celery concurrency pools** (the `--pool` option of `celery worker`; `--concurrency` defaults to the number of CPUs):

| Pool | Model | Use for | Caveat |
| --- | --- | --- | --- |
| `prefork` (default) | child processes | CPU-bound or mixed tasks | memory per child; `--max-tasks-per-child` to contain leaks |
| `threads` | thread pool | I/O-bound tasks with thread-safe clients | GIL for CPU work |
| `gevent` / `eventlet` | green threads | thousands of concurrent I/O waits | monkey-patching pitfalls (CW5) |
| `solo` | one task at a time in the main process | debugging, or one task per container | no concurrency at all |

```sh
celery -A app worker --pool=prefork --concurrency=4 --max-tasks-per-child=500 --prefetch-multiplier=1
celery -A app worker --pool=threads --concurrency=32 -Q io_bound
```

Settings that decide correctness (`task_acks_late`, `task_reject_on_worker_lost`, visibility timeout, time limits, enqueue after commit) are in [Microservices and Messaging M17](08-Microservices-and-Messaging.md); idempotent consumers are [M8](08-Microservices-and-Messaging.md).

**Cron across replicas:** claim the time slot atomically before running.

```python
# cron_once.py
import time
from collections.abc import Callable

import redis


def run_once_per_slot(client: redis.Redis, job: str, period_s: int, fn: Callable[[], None],
                      now: Callable[[], float] = time.time) -> bool:
    """Every replica's scheduler fires; only the first to claim this period's key runs the job."""
    slot = int(now()) // period_s
    claimed = client.set(f"cron:{job}:{slot}", "1", nx=True, ex=period_s * 2)
    if not claimed:
        return False  # another replica already ran (or is running) this period
    fn()  # make fn idempotent anyway: a crash after the claim means this period is skipped, not retried
    return True
```

Test: 6 simulated replicas fire at the same instant (released by a barrier) for 20 consecutive periods; exactly one runs each period and the job ran 20 times in total.

- This gives at most once per slot: if the winner crashes after claiming, that slot is skipped, so the job must be safe to run again next period (idempotent, "process everything not yet processed" rather than "process yesterday").
- Alternatives: a Kubernetes `CronJob` with `concurrencyPolicy: Forbid` (one pod, outside the web fleet), Celery beat as a single deployment, or `pg_try_advisory_xact_lock` in the job's own transaction.

---

## CW9. How do you add rate limiting and backpressure to a service?

> "Rate limiting protects me from clients: a shared counter per client key in Redis, 429 with `Retry-After` when exceeded.
> Backpressure protects my dependencies and me from overload: bound the concurrency of each downstream call with a semaphore (a bulkhead, so a slow pricing service cannot eat every worker), bound the queue in front of it, and shed excess load quickly with 503 and `Retry-After` instead of letting latency grow until everything times out.
> The limits must be set from measurement: concurrency = throughput x latency of the dependency, plus headroom."

```python
# backpressure.py
import asyncio
import time
from collections.abc import AsyncIterator
from contextlib import asynccontextmanager

import redis.asyncio as aioredis
from fastapi import Depends, FastAPI, HTTPException, Request


class Bulkhead:
    """At most `limit` concurrent calls to one dependency, at most `max_waiting` queued; reject the rest."""

    def __init__(self, limit: int, max_waiting: int) -> None:
        self._sem = asyncio.Semaphore(limit)
        self._max_waiting = max_waiting
        self._waiting = 0

    @asynccontextmanager
    async def slot(self) -> AsyncIterator[None]:
        if self._sem.locked() and self._waiting >= self._max_waiting:
            raise HTTPException(503, "downstream saturated", headers={"Retry-After": "1"})
        self._waiting += 1  # safe without a lock: no await between the check and the increment
        try:
            await self._sem.acquire()
        finally:
            self._waiting -= 1
        try:
            yield
        finally:
            self._sem.release()


# Fixed-window counter: INCR and EXPIRE in one atomic script, so a crash cannot leave a key without a TTL.
FIXED_WINDOW = """
local n = redis.call("INCR", KEYS[1])
if n == 1 then redis.call("PEXPIRE", KEYS[1], ARGV[1]) end
return {n, redis.call("PTTL", KEYS[1])}
"""


async def rate_limit(request: Request) -> None:
    store: aioredis.Redis = request.app.state.redis
    client = request.headers.get("X-API-Key", "anonymous")
    window_ms, limit = 1000, request.app.state.rate_limit
    key = f"rl:{client}:{int(time.time() * 1000) // window_ms}"
    count, ttl_ms = await store.eval(FIXED_WINDOW, 1, key, window_ms)
    if count > limit:
        retry_after = max(1, -(-ttl_ms // 1000))  # ceil to whole seconds
        raise HTTPException(429, "rate limit exceeded", headers={"Retry-After": str(retry_after)})


def create_app(store: aioredis.Redis, rate: int = 5, limit: int = 3, max_waiting: int = 2) -> FastAPI:
    app = FastAPI()
    app.state.redis, app.state.rate_limit = store, rate
    pricing_bulkhead = Bulkhead(limit=limit, max_waiting=max_waiting)

    @app.get("/quote", dependencies=[Depends(rate_limit)])
    async def quote() -> dict:
        async with pricing_bulkhead.slot():
            await asyncio.sleep(0.2)  # the slow downstream pricing service
        return {"price": 101.5}

    return app

```

Tests (in-process ASGI transport, fakeredis):

- Bulkhead with `limit=3, max_waiting=2`, 10 concurrent calls to a 0.2 s downstream: exactly 5 answered 200 (3 immediately, 2 after queueing), 5 got 503 with `Retry-After: 1`, and the whole burst took two rounds of 0.2 s; repeated 5 times.
- Rate limit of 5 per second per API key with the clock pinned to one window: 8 calls gave five 200s and three 429s with `Retry-After: 1`; another key was unaffected.
- The slot is released when the downstream raises.

**uvicorn `--limit-concurrency` is not a queue.**
Measured on one worker with `--limit-concurrency 10`: 9 concurrent requests all succeeded, but 10 concurrent requests all got 503 in about 20 ms, and 12 gave eleven 503s and one 200.
The check in `uvicorn/protocols/http/h11_impl.py` is `len(connections) >= limit or len(tasks) >= limit`, so it counts open connections, including idle keep-alive connections from a proxy, and effectively admits `limit - 1`.
Use it as a last-resort circuit breaker set well above normal concurrency, and do real backpressure in the app.

| Tool | Layer | Response |
| --- | --- | --- |
| API gateway / ingress rate limit | edge | 429 before the app sees it |
| Redis counter per key (fixed window above; sliding window or token bucket for smoother limits, see [System design X2](12-System-Design.md) and [K11](11-Coding-Round.md)) | app, shared | 429 + `Retry-After` |
| `asyncio.Semaphore` / anyio `CapacityLimiter` per dependency | app, per worker | queue a little, then 503 |
| DB pool size and `pool_timeout` | app, per worker | fail fast with a short timeout instead of 30 s |
| Circuit breaker ([M10](08-Microservices-and-Messaging.md)) | app, per dependency | stop calling a failing dependency |
| Queue length limit (broker max length, bounded `asyncio.Queue`) | async pipelines | reject or drop at enqueue |

---

## CW10. How do you handle CPU-bound work inside an endpoint?

> "Never on the event loop and not in the default threadpool: on the GIL build a CPU-bound thread still serializes with every other thread in the worker.
> For short CPU work (tens to hundreds of milliseconds) I create a `ProcessPoolExecutor` in `lifespan`, once per worker, and `await loop.run_in_executor(pool, fn, args)`; the function and arguments must be picklable and the pool is created after the server forks.
> For long or heavy work (seconds and up, reports, ML inference at volume) I enqueue a job and return 202 with a status URL, because a process pool still ties the HTTP request to the work and competes with the web workers for the same CPUs."

The `/primes/{limit}` route in CW2's `fastapi_concurrency.py` is the pattern; the test starts it under a real uvicorn and gets `{"limit": 100000, "primes": 9592}` back from a pool process.

- Size: web workers x pool processes must not exceed the cores the container actually has; two workers each with a 2-process pool is 4 CPU-heavy processes plus the event loops.
- On the free-threaded build, a thread (`asyncio.to_thread` or a dedicated `ThreadPoolExecutor`) runs Python in parallel and avoids pickling; the measured comparison is CW25.
- Pickling cost can exceed the work for large inputs: send ids or file paths, not DataFrames; shared memory is in [Multiprocessing and parallelism](15-Multiprocessing-and-Parallelism.md).
- Libraries that release the GIL (NumPy, hashing, compression, many C extensions) can use threads profitably even on the GIL build.

---

## CW11. How do you shut down gracefully with in-flight requests and background threads?

> "On SIGTERM the server must stop accepting new connections, let in-flight requests finish within a grace period, then run shutdown hooks that stop background threads and flush buffers, and exit before Kubernetes sends SIGKILL.
> Uvicorn does this on SIGTERM and then runs the `lifespan` shutdown; gunicorn's master forwards a graceful stop to its workers and waits `graceful_timeout` (30 s by default).
> Background threads need an explicit stop signal (a `threading.Event`) and a join; daemon threads are killed mid-write at interpreter exit, so I use them only for work that is safe to lose.
> In Kubernetes, readiness should fail first (or a short `preStop` sleep) so the load balancer stops sending new requests before the process stops accepting them."

```python
# graceful.py
import asyncio
import os
import queue
import threading
from collections.abc import AsyncIterator
from contextlib import asynccontextmanager

from fastapi import FastAPI, Request


class AuditFlusher:
    """Background thread that batches audit events; stop() drains what is left."""

    def __init__(self, sink_path: str, interval_s: float = 0.5) -> None:
        self._events: queue.Queue[str] = queue.Queue()
        self._stop = threading.Event()
        self._sink_path, self._interval_s = sink_path, interval_s
        self._thread = threading.Thread(target=self._run, name="audit-flusher")  # not daemon: we join it

    def start(self) -> None:
        self._thread.start()

    def record(self, event: str) -> None:
        self._events.put(event)

    def _flush(self) -> None:
        batch = []
        while True:
            try:
                batch.append(self._events.get_nowait())
            except queue.Empty:
                break
        if batch:
            with open(self._sink_path, "a") as f:
                f.writelines(line + "\n" for line in batch)

    def _run(self) -> None:
        while not self._stop.wait(self._interval_s):  # wakes early when stop() sets the event
            self._flush()
        self._flush()  # final drain after the stop signal

    def stop(self, timeout: float = 5.0) -> None:
        self._stop.set()
        self._thread.join(timeout)


@asynccontextmanager
async def lifespan(app: FastAPI) -> AsyncIterator[None]:
    flusher = AuditFlusher(os.environ["AUDIT_SINK"])
    flusher.start()
    app.state.audit = flusher
    yield
    # Runs after the server stopped accepting and in-flight requests finished (or the grace timeout hit).
    flusher.record("shutdown")
    flusher.stop()


app = FastAPI(lifespan=lifespan)


@app.get("/healthz")
async def healthz() -> dict:
    return {"ok": True}


@app.post("/orders")
async def create_order(request: Request) -> dict:
    await asyncio.sleep(1.0)  # a slow in-flight request when SIGTERM arrives
    request.app.state.audit.record("order-created")
    return {"status": "created"}
```

Tests against real processes:

- uvicorn: a POST that takes 1 s is in flight when SIGTERM arrives; 0.2 s later a new connection is refused (`httpx.ConnectError`), the in-flight request still returns 200, and the audit file contains `order-created` then `shutdown`, in that order, proving the lifespan shutdown ran after the request finished and the flusher drained its queue.
  uvicorn 0.54 re-raises the captured signal after a clean shutdown (`signal.raise_signal` in `uvicorn/server.py`), so the process exit status is `-SIGTERM`, not 0; do not treat that as a crash in scripts.
- gunicorn `sync` worker: a request in flight when the master gets SIGTERM completes with 200 and the master exits 0.

```text
t=0     SIGTERM to the pod; readiness probe starts failing (or preStop: sleep 5)
t=0..5  load balancer drains the endpoint; no new requests arrive
t=5     server stops accepting; in-flight requests finish (uvicorn --timeout-graceful-shutdown, gunicorn graceful_timeout)
t=...   lifespan shutdown: stop consumers, stop and join threads, flush, dispose the engine
t<30    process exits; terminationGracePeriodSeconds (30 s default) must exceed all of the above
```

**Pitfalls:** a grace timeout longer than `terminationGracePeriodSeconds` (SIGKILL wins); long-polling or streaming responses that never finish (give them a shutdown signal); `BackgroundTasks` still running when the grace period ends; Celery workers need `task_acks_late` so a killed task is redelivered.

---

## CW12. Predict the output: what goes wrong?

Each snippet was run; the stated output is what it printed.

**Snippet 1: A `ContextVar` set in a sync dependency.**

```python
# p1_contextvar_dependency.py
from contextvars import ContextVar
from typing import Annotated

from fastapi import Depends, FastAPI
from fastapi.testclient import TestClient

tenant: ContextVar[str] = ContextVar("tenant", default="unset")
app = FastAPI()


def set_tenant_sync() -> None:
    tenant.set("acme")  # runs in a worker thread, inside a copy of the request's context


async def set_tenant_async() -> None:
    tenant.set("acme")  # runs on the event loop, in the request task's own context


@app.get("/sync-dep")
async def sync_dep(_: Annotated[None, Depends(set_tenant_sync)]) -> dict:
    return {"tenant": tenant.get()}


@app.get("/async-dep")
async def async_dep(_: Annotated[None, Depends(set_tenant_async)]) -> dict:
    return {"tenant": tenant.get()}


client = TestClient(app)
print(client.get("/sync-dep").json(), client.get("/async-dep").json())
```

Output: `{'tenant': 'unset'} {'tenant': 'acme'}`.
The sync dependency ran in a worker thread inside a copy of the context (`anyio` copies it for `to_thread.run_sync`), so its `set()` never reaches the endpoint; set request-scoped context vars in `async def` dependencies or middleware, or return the value from the dependency instead.

**Snippet 2: `functools.lru_cache` under concurrent first calls.**

```python
# p2_lru_cache_threads.py
import threading
import time
from functools import lru_cache

calls = 0


@lru_cache(maxsize=128)
def load_config(env: str) -> dict:
    global calls
    calls += 1
    time.sleep(0.1)  # slow fetch from a config service
    return {"env": env}


barrier = threading.Barrier(8)


def worker() -> None:
    barrier.wait()
    load_config("prod")


threads = [threading.Thread(target=worker) for _ in range(8)]
for t in threads:
    t.start()
for t in threads:
    t.join()
print(calls, load_config.cache_info().hits, load_config.cache_info().misses)
```

Output: `8 0 8`: eight calls, zero hits, eight misses.
`lru_cache` keeps its own structure consistent across threads but does not hold a lock while the function runs, so simultaneous misses all compute (a cache stampede); CW21's `get_or_compute` is the single-flight fix.

**Snippet 3: `threading.local` in asyncio.**

```python
# p3_threading_local_asyncio.py
import asyncio
import threading
from contextvars import ContextVar

local = threading.local()
var: ContextVar[str] = ContextVar("user")


async def handle(user: str) -> tuple[str, str]:
    local.user = user
    var.set(user)
    await asyncio.sleep(0.01)  # another request runs here, on the same thread
    return local.user, var.get()


async def main() -> None:
    print(await asyncio.gather(handle("alice"), handle("bob")))


asyncio.run(main())
```

Output: `[('bob', 'alice'), ('bob', 'bob')]`.
Both tasks run on the same thread, so the thread-local holds whichever request wrote last; a `ContextVar` is per task (each task copies the context when it is created; more in [CT14](14-Concurrency-Fundamentals-and-Threading.md)).

**Snippet 4: The ORM lost update, no threads needed.**

```python
# p4_orm_lost_update.py
from sqlalchemy import create_engine
from sqlalchemy.orm import DeclarativeBase, Mapped, Session, mapped_column


class Base(DeclarativeBase):
    pass


class Account(Base):
    __tablename__ = "accounts"
    id: Mapped[int] = mapped_column(primary_key=True)
    balance: Mapped[int]


engine = create_engine("sqlite://")
Base.metadata.create_all(engine)
with Session(engine) as s:
    s.add(Account(id=1, balance=100))
    s.commit()

request_a, request_b = Session(engine), Session(engine)
a = request_a.get(Account, 1)  # both requests read balance = 100
b = request_b.get(Account, 1)
a.balance += 10
request_a.commit()  # UPDATE accounts SET balance=110
b.balance += 10
request_b.commit()  # UPDATE accounts SET balance=110 (not 120)
with Session(engine) as s:
    print(s.get(Account, 1).balance)
```

Output: `110`.
Each session read 100 and wrote its own computed value; the fix is CW4's atomic update or a version column.

**Snippet 5: `count += 1` from 8 threads, 200,000 times each.**

```python
# p5_unsafe_increment.py
import sys
import threading

count = 0


def work() -> None:
    global count
    for _ in range(200_000):
        count += 1  # LOAD_GLOBAL, BINARY_OP, STORE_GLOBAL: not atomic


threads = [threading.Thread(target=work) for _ in range(8)]
for t in threads:
    t.start()
for t in threads:
    t.join()
print(getattr(sys, "_is_gil_enabled", lambda: True)(), count)
```

Output: `True 1600000` on the standard 3.14.7 build in every run (also on 3.12 and 3.13, and even with `sys.setswitchinterval(1e-6)`), and `False` followed by a count between 221,043 and 239,150 in five runs on the free-threaded 3.14.6 build.
The GIL build switches threads only at specific points in the evaluation loop, which happen not to fall between this load and store, so the bug hides; that is not a language guarantee (the bytecode is in [CT6](14-Concurrency-Fundamentals-and-Threading.md)), and the free-threaded build loses about 85% of the increments.
Nondeterministic: the free-threaded count differs on every run.

**Snippet 6: `Condition.wait()` guarded by `if` instead of `while`.**

```python
# p6_condition_if.py
import threading
import time

items: list[int] = []
cond = threading.Condition()
errors: list[str] = []


def consumer() -> None:
    with cond:
        if not items:  # BUG: should be `while not items:` or cond.wait_for(lambda: items)
            cond.wait()
        try:
            items.pop()
        except IndexError:
            errors.append("popped from empty list")


threads = [threading.Thread(target=consumer) for _ in range(2)]
for t in threads:
    t.start()
time.sleep(0.2)  # both consumers are now waiting
with cond:
    items.append(1)
    cond.notify_all()  # wakes both, but there is only one item
for t in threads:
    t.join()
print(errors)
```

Output: `['popped from empty list']`.
`notify_all` woke both consumers, the first took the only item, and the second did not re-check the predicate; always `wait_for(predicate)` or loop ([CT17](14-Concurrency-Fundamentals-and-Threading.md)).

**Snippet 7: A module-level `asyncio.Semaphore` used from two event loops.**

```python
# p7_semaphore_two_loops.py
import asyncio

LIMIT = asyncio.Semaphore(1)  # module level: binds to the first loop that has to wait on it


async def call(i: int) -> int:
    async with LIMIT:
        await asyncio.sleep(0.01)
        return i


async def burst() -> list[int]:
    return await asyncio.gather(call(1), call(2))


print(asyncio.run(burst()))  # first loop: fine
try:
    print(asyncio.run(burst()))  # a second loop, as in a second test or a second asyncio.run
except RuntimeError as exc:
    print("RuntimeError:", exc)
```

Output: `[1, 2]` then `RuntimeError: <asyncio.locks.Semaphore object at 0x... [locked]> is bound to a different event loop`.
asyncio primitives bind to the first loop that has to wait on them; create them inside the running loop (in `lifespan`, or per test).

**Snippet 8: Exceptions in executor tasks nobody waits for.**

```python
# p8_executor_swallows.py
from concurrent.futures import ThreadPoolExecutor


def sync_inventory(sku: str) -> None:
    raise ConnectionError(f"inventory service down for {sku}")


with ThreadPoolExecutor(4) as pool:
    futures = [pool.submit(sync_inventory, s) for s in ("A", "B")]
print("done, no traceback printed")
print([type(f.exception()).__name__ for f in futures])
```

Output: `done, no traceback printed`, then `['ConnectionError', 'ConnectionError']`.
The exception is stored on the future and only surfaces through `result()` or `exception()`; call them (as CW20's crawler does), or add a done-callback that logs ([CT29](14-Concurrency-Fundamentals-and-Threading.md)).

---

## Pitfalls in concurrent services

- In-memory state (counters, caches, rate limits, "run once" flags) in a multi-worker, multi-replica service (CW3).
- `obj.x += 1` through the ORM, and check-then-insert without a unique constraint (CW4).
- A blocking call in `async def`: `requests`, `time.sleep`, a sync DB driver, `open().read()` on a slow volume, heavy JSON or pandas work (CW2).
- Raising the threadpool or DB pool without checking the other, or without checking `max_connections` across the fleet (CW1, CW6).
- A shared `Session` or `AsyncSession`, or `asyncio.gather` over one session (CW6).
- A lock with a lease shorter than the work, released with plain `DEL`, and no fencing (CW7).
- A scheduler in every replica (CW8), and `BackgroundTasks` for work that must happen.
- No backpressure: unbounded `gather` over user input, unbounded queues, 30-second pool timeouts in a request path (CW9).
- CPU-bound work on the event loop or in the default threadpool on the GIL build (CW10).
- Graceful-shutdown timeouts that exceed the orchestrator's kill timeout, and daemon threads that hold unflushed data (CW11).
- Engines, clients, or asyncio primitives created at import time and then inherited through fork or reused across event loops (CW6, CW12).

---

## Part 2: The concurrency coding round

CW13-CW27 are the problems; each has a solution, a correctness argument, and tests that run it many times.

---

## How to talk through a concurrency coding problem

Say these five things before and while you code; interviewers grade them more than the syntax.

1. **Shared state:** name exactly what is shared ("the turn", "the queue and its length", "the set of seen URLs") and who reads and writes it.
2. **Invariant:** one sentence that must hold at every moment ("the buffer never exceeds capacity", "foo and bar strictly alternate", "no fork is held by two philosophers").
3. **Primitive, and why:** Event for a one-shot "it happened", Semaphore for counted permits or handing a turn, Condition for "wait until this predicate over shared state is true", Barrier for "all N arrive", Lock for short critical sections, a queue when ownership can move instead of being shared.
   Prefer designs with no shared mutable state at all (one coordinator thread owns it, as in CW20).
4. **How it fails:** deadlock (circular wait, holding a lock while waiting for another), lost wakeup (`if` instead of `while`, notifying before the waiter waits without a predicate), starvation (a writer that never gets in), a race on check-then-act, and an exception that kills a thread while it holds a turn.
5. **How you would test it:** randomize start order, run hundreds of iterations, shrink the switch interval on the GIL build, run on the free-threaded build, force the bad interleaving with a barrier, and give every join a timeout so a deadlock fails the test instead of hanging CI.

The harness every Part 2 test uses:

```python
# test_harness_demo.py
import random
import sys
import threading

import pytest

from print_in_order import Foo


@pytest.fixture(autouse=True)
def fast_switching():
    old = sys.getswitchinterval()
    sys.setswitchinterval(1e-6)  # GIL build: switch threads far more often than every 5 ms
    yield
    sys.setswitchinterval(old)


def start_all(targets, join_timeout=10.0):
    threads = [threading.Thread(target=t) for t in targets]
    random.shuffle(threads)  # the start order must not matter
    for t in threads:
        t.start()
    for t in threads:
        t.join(join_timeout)
        assert not t.is_alive(), "deadlock or lost wakeup"  # a hang fails instead of freezing CI


def test_print_in_order_300_times():
    for _ in range(300):
        out, foo = [], Foo()
        start_all([lambda: foo.third(lambda: out.append("third")),
                   lambda: foo.second(lambda: out.append("second")),
                   lambda: foo.first(lambda: out.append("first"))])
        assert out == ["first", "second", "third"]
```

**Complexity talk:** for these problems "complexity" means the number of synchronization operations per item (usually O(1)), what blocks and for how long, and whether the solution needs busy waiting (never acceptable: `while not flag: pass` burns a core and, on the GIL build, starves the thread that would set the flag).

---

## CW13. Print in Order (LeetCode 1114). (must know)

Three threads call `first`, `second`, `third` in any order; the output must be first, second, third.
Shared state: two "done" facts.
Invariant: `second` runs only after `first` finished, `third` only after `second`.
Primitive: two `Event`s, because each fact becomes true once and stays true.

```python
# print_in_order.py
import threading
from collections.abc import Callable


class Foo:
    """LeetCode 1114: first() before second() before third(), whatever order the threads start."""

    def __init__(self) -> None:
        self._first_done = threading.Event()
        self._second_done = threading.Event()

    def first(self, printFirst: Callable[[], None]) -> None:
        printFirst()
        self._first_done.set()  # happens-before: everything above is visible to the waiter

    def second(self, printSecond: Callable[[], None]) -> None:
        self._first_done.wait()
        printSecond()
        self._second_done.set()

    def third(self, printThird: Callable[[], None]) -> None:
        self._second_done.wait()
        printThird()
```

- Correctness: `Event.set()` happens-before any `wait()` that returns because of it, so the print in `first` is visible to `second`; there is no shared counter to race on.
- Tested 300 times with shuffled start order and a 1 microsecond switch interval, on both builds.
- Alternatives: two `Semaphore(0)`s or a `Condition` with a turn counter; both work, Events read most clearly.
- Follow-up: "generalize to N stages" is a list of N-1 Events, or one Condition with `wait_for(lambda: turn == k)`.

---

## CW14. Print FooBar Alternately (LeetCode 1115).

Two threads, one prints "foo" n times and one "bar" n times; the output must be "foobar" repeated n times.
Shared state: whose turn it is.
Invariant: the counts differ by at most one and foo leads.
Primitive: two semaphores that pass one permit back and forth.

```python
# foobar.py
import threading
from collections.abc import Callable


class FooBar:
    """LeetCode 1115: two threads print "foobar" n times; a semaphore pair hands the turn back and forth."""

    def __init__(self, n: int) -> None:
        self.n = n
        self._foo_turn = threading.Semaphore(1)  # foo goes first
        self._bar_turn = threading.Semaphore(0)

    def foo(self, printFoo: Callable[[], None]) -> None:
        for _ in range(self.n):
            self._foo_turn.acquire()
            printFoo()
            self._bar_turn.release()

    def bar(self, printBar: Callable[[], None]) -> None:
        for _ in range(self.n):
            self._bar_turn.acquire()
            printBar()
            self._foo_turn.release()
```

- Exactly one permit exists across the two semaphores at any time, so exactly one thread can print; each thread gives the permit to the other after printing.
- A `Lock` cannot do this cleanly, because a lock must be released by the thread that acquired it; a semaphore has no owner.
- Tested for n in 1, 2, 50, 500 with shuffled starts.

---

## CW15. Print Zero Even Odd (LeetCode 1116).

Three threads print `0 1 0 2 0 3 ... 0 n`: `zero` prints the zeros, `odd` the odd numbers, `even` the even numbers.
The `zero` thread is the dispatcher: after each 0 it releases whichever semaphore owns the next number, and that thread hands the turn back.

```python
# zero_even_odd.py
import threading
from collections.abc import Callable


class ZeroEvenOdd:
    """LeetCode 1116: three threads print 0 1 0 2 0 3 ... 0 n."""

    def __init__(self, n: int) -> None:
        self.n = n
        self._zero = threading.Semaphore(1)
        self._odd = threading.Semaphore(0)
        self._even = threading.Semaphore(0)

    def zero(self, printNumber: Callable[[int], None]) -> None:
        for i in range(1, self.n + 1):
            self._zero.acquire()
            printNumber(0)
            (self._odd if i % 2 else self._even).release()  # zero decides whose turn is next

    def even(self, printNumber: Callable[[int], None]) -> None:
        for i in range(2, self.n + 1, 2):
            self._even.acquire()
            printNumber(i)
            self._zero.release()

    def odd(self, printNumber: Callable[[int], None]) -> None:
        for i in range(1, self.n + 1, 2):
            self._odd.acquire()
            printNumber(i)
            self._zero.release()
```

- Each thread knows its own sequence of numbers, so no shared counter is needed; the semaphores carry only the turn.
- Edge cases tested: n from 1 to 11 (n=1 means the `even` thread prints nothing and must still terminate) and n=101, 20 runs each.

---

## CW16. Building H2O (LeetCode 1117).

Hydrogen and oxygen threads arrive in any order; they must leave in groups of exactly two H and one O.
Invariant: at most two H and one O are "in the current molecule"; the molecule leaves only when all three are present.

```python
# h2o.py
import threading
from collections.abc import Callable


class H2O:
    """LeetCode 1117: every group of three released atoms is exactly two H and one O."""

    def __init__(self) -> None:
        self._h_slots = threading.Semaphore(2)  # at most two H in the current molecule
        self._o_slots = threading.Semaphore(1)  # at most one O
        self._bond = threading.Barrier(3)  # the molecule forms when all three have arrived

    def hydrogen(self, releaseHydrogen: Callable[[], None]) -> None:
        with self._h_slots:
            self._bond.wait()
            releaseHydrogen()

    def oxygen(self, releaseOxygen: Callable[[], None]) -> None:
        with self._o_slots:
            self._bond.wait()
            releaseOxygen()
```

- The semaphores cap membership; the `Barrier(3)` can only trip with two H and one O, because a third H cannot get a slot.
- A thread from the next molecule cannot pass its semaphore until a thread of the current molecule has printed and left the `with` block, so the output comes in clean triples.
- Tested with 60 H and 30 O threads started in random order, 20 runs, checking every consecutive triple is `H, H, O` in some order.
- Failure mode to mention: a `Barrier` without the semaphores can trip with three H.

---

## CW17. Fizz Buzz Multithreaded (LeetCode 1195).

Four threads (fizz, buzz, fizzbuzz, number) cooperate to print the sequence for 1..n.
Shared state: the current number.
Invariant: exactly one thread owns each number.
Primitive: one `Condition`, because each thread waits for a predicate over shared state.

```python
# fizzbuzz_mt.py
import threading
from collections.abc import Callable


class FizzBuzz:
    """LeetCode 1195: four threads; exactly one of them owns each number, a Condition passes the turn."""

    def __init__(self, n: int) -> None:
        self.n = n
        self._i = 1
        self._cv = threading.Condition()

    def _run(self, owns: Callable[[int], bool], emit: Callable[[int], None]) -> None:
        while True:
            with self._cv:
                self._cv.wait_for(lambda: self._i > self.n or owns(self._i))  # re-checks after every wakeup
                if self._i > self.n:
                    return
                emit(self._i)
                self._i += 1
                self._cv.notify_all()  # the next owner is one specific thread: wake all, it re-checks

    def fizz(self, printFizz: Callable[[], None]) -> None:
        self._run(lambda i: i % 3 == 0 and i % 5 != 0, lambda i: printFizz())

    def buzz(self, printBuzz: Callable[[], None]) -> None:
        self._run(lambda i: i % 5 == 0 and i % 3 != 0, lambda i: printBuzz())

    def fizzbuzz(self, printFizzBuzz: Callable[[], None]) -> None:
        self._run(lambda i: i % 15 == 0, lambda i: printFizzBuzz())

    def number(self, printNumber: Callable[[int], None]) -> None:
        self._run(lambda i: i % 3 != 0 and i % 5 != 0, printNumber)
```

- `wait_for` re-checks the predicate after every wakeup, so spurious or stolen wakeups are harmless.
- `notify_all` is required: the next owner is a specific thread, and `notify()` could wake the wrong one, which would go back to sleep and leave everyone waiting (a lost wakeup).
- Every thread's predicate includes `i > n`, so all four exit; forgetting that is the usual hang.
- Tested for n in 1, 3, 5, 15, 16, 100, 10 runs each.

---

## CW18. The Dining Philosophers (LeetCode 1226).

Five philosophers, five forks, each needs both neighbouring forks to eat; avoid deadlock.
Deadlock needs all four Coffman conditions; the standard fix breaks circular wait by imposing a global order on the forks.

```python
# dining.py
import threading
from collections.abc import Callable

Action = Callable[[], None]


class DiningPhilosophers:
    """LeetCode 1226: take the lower-numbered fork first, so a wait-for cycle cannot form."""

    def __init__(self) -> None:
        self._forks = [threading.Lock() for _ in range(5)]

    def wantsToEat(self, philosopher: int, pickLeftFork: Action, pickRightFork: Action,
                   eat: Action, putLeftFork: Action, putRightFork: Action) -> None:
        left, right = philosopher, (philosopher + 1) % 5
        first, second = sorted((left, right))  # global lock order breaks circular wait
        with self._forks[first], self._forks[second]:
            pickLeftFork()
            pickRightFork()
            eat()
            putLeftFork()
            putRightFork()
```

- Philosopher 4 needs forks 4 and 0 and takes 0 first, so the cycle "everyone holds the left fork and waits for the right" cannot form.
- Tests: 5 philosophers x 60 meals, 10 runs; a checker records which philosopher holds each fork and fails if a fork is ever held twice; every philosopher ate 60 times.
- The failure mode is tested too: a naive left-then-right version, with a barrier forcing everyone to hold the left fork first, has all five `acquire(timeout=0.3)` calls on the right fork fail.
- Other fixes: allow at most four philosophers to try at once (`Semaphore(4)`), or `acquire(timeout)` and back off (risks livelock without jitter); a single global lock works but serializes everyone.

---

## CW19. Design a bounded blocking queue (LeetCode 1188), then its asyncio twin. (must know)

`enqueue` blocks while full, `dequeue` blocks while empty, `size` returns the count.
Shared state: the deque.
Invariants: `0 <= len <= capacity`, FIFO.
Primitive: one lock with two conditions, so producers wait on "not full" and consumers on "not empty".

```python
# bounded_queue.py
import asyncio
import threading
from collections import deque
from typing import Generic, TypeVar

T = TypeVar("T")


class BoundedBlockingQueue(Generic[T]):
    """LeetCode 1188: enqueue blocks while full, dequeue blocks while empty."""

    def __init__(self, capacity: int) -> None:
        if capacity <= 0:
            raise ValueError("capacity must be positive")
        self._items: deque[T] = deque()
        self._capacity = capacity
        self._lock = threading.Lock()
        self._not_full = threading.Condition(self._lock)  # two conditions, one lock:
        self._not_empty = threading.Condition(self._lock)  # producers and consumers wait separately

    def enqueue(self, element: T, timeout: float | None = None) -> bool:
        with self._not_full:
            if not self._not_full.wait_for(lambda: len(self._items) < self._capacity, timeout):
                return False
            self._items.append(element)
            self._not_empty.notify()  # one item added: one consumer can proceed
            return True

    def dequeue(self, timeout: float | None = None) -> T:
        with self._not_empty:
            if not self._not_empty.wait_for(lambda: len(self._items) > 0, timeout):
                raise TimeoutError("queue empty")
            item = self._items.popleft()
            self._not_full.notify()  # one slot freed: one producer can proceed
            return item

    def size(self) -> int:
        with self._lock:
            return len(self._items)


class AsyncBoundedQueue(Generic[T]):
    """The asyncio twin: same shape, asyncio.Condition, and no thread safety (one event loop only)."""

    def __init__(self, capacity: int) -> None:
        self._items: deque[T] = deque()
        self._capacity = capacity
        lock = asyncio.Lock()
        self._not_full = asyncio.Condition(lock)
        self._not_empty = asyncio.Condition(lock)

    async def enqueue(self, element: T) -> None:
        async with self._not_full:
            await self._not_full.wait_for(lambda: len(self._items) < self._capacity)
            self._items.append(element)
            self._not_empty.notify()

    async def dequeue(self) -> T:
        async with self._not_empty:
            await self._not_empty.wait_for(lambda: len(self._items) > 0)
            item = self._items.popleft()
            self._not_full.notify()
            return item

    def size(self) -> int:
        return len(self._items)  # no lock needed: nothing else runs until we await
```

- `notify()` (not `notify_all`) is enough and cheaper: each enqueue creates exactly one item for one consumer, and each dequeue frees exactly one slot for one producer; `wait_for` handles any wakeup that finds the predicate false.
- Timeouts are the production addition: `enqueue(..., timeout)` returns False when the queue stays full, which is how you turn backpressure into a 503.
- Tests: capacities 1, 3, 16 with 4 producers x 500 items and 4 consumers, checking every item arrives exactly once and `size()` never exceeds capacity; a blocked `enqueue` on a full queue does not complete until a `dequeue`; `dequeue(timeout=0.05)` on an empty queue raises `TimeoutError`; the async twin with 3 producers and 3 consumers.
- In real code use `queue.Queue(maxsize)` or `asyncio.Queue(maxsize)`; since 3.13 both have `shutdown()` to wake blocked producers and consumers at the end (see [threading](14-Concurrency-Fundamentals-and-Threading.md) and [asyncio](16-Asyncio-Deep-Dive.md)).
- The async version needs no thread safety, only no `await` between check and act; it is not safe to call from other threads (use `loop.call_soon_threadsafe` or a `janus`-style bridge).

---

## CW20. Web Crawler Multithreaded (LeetCode 1242). (must know)

Crawl every URL reachable from `start_url` that has the same hostname, calling a blocking `get_urls(url)` in parallel.
Shared state: the set of seen URLs.
The design choice that removes the race: only the coordinator thread touches `seen`; workers only call `get_urls`.

```python
# web_crawler.py
from collections.abc import Callable
from concurrent.futures import FIRST_COMPLETED, ThreadPoolExecutor, wait
from urllib.parse import urlsplit


def crawl(start_url: str, get_urls: Callable[[str], list[str]], max_workers: int = 16) -> list[str]:
    """LeetCode 1242: every URL reachable from start_url on the same hostname, fetched in parallel.

    Only this (coordinator) thread touches `seen`, so it needs no lock; workers only run get_urls.
    """
    host = urlsplit(start_url).hostname
    seen = {start_url}
    with ThreadPoolExecutor(max_workers) as pool:
        pending = {pool.submit(get_urls, start_url)}
        while pending:
            done, pending = wait(pending, return_when=FIRST_COMPLETED)
            for future in done:
                for url in future.result():  # re-raises a worker's exception here, not silently
                    if url not in seen and urlsplit(url).hostname == host:
                        seen.add(url)
                        pending.add(pool.submit(get_urls, url))
    return sorted(seen)
```

- No lock is needed because `seen` has a single owner; the check-then-add race ("two workers both find /a unseen and both fetch it") cannot happen.
- `wait(..., FIRST_COMPLETED)` lets the coordinator schedule new work as soon as any fetch finishes, so the pool stays busy; `future.result()` re-raises worker exceptions instead of losing them (CW12, snippet 8).
- Tests: a 300-page synthetic site with links to another host and 2 ms of latency per page; with 1, 4, and 16 workers the result equals a sequential BFS, no URL is fetched twice, and other hosts are excluded; 16 workers finish in under a third of the 1-worker time; a failing `get_urls` raises in the caller.
- The alternative with a lock around `seen` plus worker threads that submit their own children is harder to terminate correctly (when is the crawl done?); the coordinator makes termination "no pending futures".

---

## CW21. Implement a thread-safe LRU cache, and stop concurrent misses from all computing. (must know)

Start from the `OrderedDict` LRU ([K1](11-Coding-Round.md)), put one lock around every operation, and keep slow work outside the lock.
The follow-up that separates seniors: when 16 threads miss the same key at once, compute it once (single flight) and let the others wait for that result.

```python
# ts_lru.py
import threading
from collections import OrderedDict
from collections.abc import Callable, Hashable
from concurrent.futures import Future
from typing import Any


class ThreadSafeLRU:
    """LRU cache safe for many threads, with single-flight loading: one compute per missing key."""

    def __init__(self, capacity: int) -> None:
        if capacity <= 0:
            raise ValueError("capacity must be positive")
        self._capacity = capacity
        self._data: OrderedDict[Hashable, Any] = OrderedDict()
        self._inflight: dict[Hashable, Future] = {}
        self._lock = threading.Lock()

    def get(self, key: Hashable, default: Any = None) -> Any:
        with self._lock:
            if key not in self._data:
                return default
            self._data.move_to_end(key)
            return self._data[key]

    def put(self, key: Hashable, value: Any) -> None:
        with self._lock:
            self._data[key] = value
            self._data.move_to_end(key)
            if len(self._data) > self._capacity:
                self._data.popitem(last=False)

    def __len__(self) -> int:
        with self._lock:
            return len(self._data)

    def get_or_compute(self, key: Hashable, compute: Callable[[Hashable], Any]) -> Any:
        with self._lock:
            if key in self._data:
                self._data.move_to_end(key)
                return self._data[key]
            future = self._inflight.get(key)
            owner = future is None
            if owner:
                future = self._inflight[key] = Future()
        if not owner:
            return future.result()  # wait for the owner's compute; re-raises its exception
        try:
            value = compute(key)  # slow work runs outside the lock
        except BaseException as exc:
            with self._lock:
                del self._inflight[key]  # the next caller retries
            future.set_exception(exc)
            raise
        self.put(key, value)
        with self._lock:
            del self._inflight[key]
        future.set_result(value)
        return value
```

- Every read also writes (`move_to_end`), so a read-write lock would not help; a single `Lock` held for O(1) work is the right primitive.
- Single flight: the first thread for a key registers a `Future` in `_inflight` and computes outside the lock; later threads wait on `future.result()`, which also re-raises the owner's exception.
  On failure the in-flight entry is removed so the next call retries instead of caching the error.
- Tests: 8 threads x 3,000 random get/put operations with the invariant `len <= capacity` and "value is consistent with its key"; 16 threads released together call `get_or_compute("k")` 20 times and the compute function runs once each time; four concurrent callers all see the backend error and the next call succeeds.
- Compare `functools.lru_cache`, which computes once per concurrent miss (CW12, snippet 2); in a web service the same idea at the Redis layer is stampede protection ([S24](06-SQL-and-SQLAlchemy.md)).

---

## CW22. Make a counter thread-safe, and benchmark a lock against per-thread counters.

```python
# counters.py
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor


class UnsafeCounter:
    def __init__(self) -> None:
        self.value = 0

    def increment(self) -> None:
        self.value += 1  # load, add, store: another thread can run between the load and the store


class LockedCounter:
    def __init__(self) -> None:
        self.value = 0
        self._lock = threading.Lock()

    def increment(self) -> None:
        with self._lock:
            self.value += 1


class ShardedCounter:
    """Each thread increments its own cell; reads sum the cells. No lock on the hot path."""

    def __init__(self) -> None:
        self._local = threading.local()
        self._cells: list[list[int]] = []
        self._lock = threading.Lock()  # guards the list of cells, taken once per thread

    def increment(self) -> None:
        cell = getattr(self._local, "cell", None)
        if cell is None:
            cell = self._local.cell = [0]
            with self._lock:
                self._cells.append(cell)
        cell[0] += 1  # only the owning thread writes this cell

    @property
    def value(self) -> int:
        with self._lock:
            return sum(cell[0] for cell in self._cells)  # exact once writers are done


def run(counter, threads: int, per_thread: int) -> float:
    def work() -> None:
        inc = counter.increment
        for _ in range(per_thread):
            inc()

    start = time.perf_counter()
    with ThreadPoolExecutor(threads) as pool:
        for f in [pool.submit(work) for _ in range(threads)]:
            f.result()
    return time.perf_counter() - start


def local_then_merge(threads: int, per_thread: int) -> tuple[int, float]:
    """No shared state at all: count in a local variable, merge the results at the end."""

    def work() -> int:
        n = 0
        for _ in range(per_thread):
            n += 1
        return n

    start = time.perf_counter()
    with ThreadPoolExecutor(threads) as pool:
        total = sum(pool.map(lambda _: work(), range(threads)))
    return total, time.perf_counter() - start


if __name__ == "__main__":
    threads, per_thread, repeats = 8, 200_000, 5
    gil = getattr(sys, "_is_gil_enabled", lambda: True)()
    print(f"{sys.version.split()[0]} gil={gil} threads={threads} increments={threads * per_thread}")
    for cls in (UnsafeCounter, LockedCounter, ShardedCounter):
        values, times = [], []
        for _ in range(repeats):
            c = cls()
            times.append(run(c, threads, per_thread))
            values.append(c.value)
        print(f"{cls.__name__:15} values={min(values)}..{max(values)} median={sorted(times)[repeats // 2]:.3f}s")
    times = sorted(local_then_merge(threads, per_thread)[1] for _ in range(repeats))
    print(f"{'LocalThenMerge':15} value={threads * per_thread} median={times[repeats // 2]:.3f}s")
```

Measured with 8 threads x 200,000 increments, median of 5 runs, `python counters.py` on each build:

| Counter | 3.14.7 (GIL) value | 3.14.7 time | 3.14.6 free-threaded value | 3.14.6 time |
| --- | --- | --- | --- | --- |
| `UnsafeCounter` | 1,600,000 in all 5 runs | 0.048 s | 427,253 to 472,478 | 0.049 s |
| `LockedCounter` | 1,600,000 | 0.139 s | 1,600,000 | 0.154 s |
| `ShardedCounter` (thread-local cells) | 1,600,000 | 0.114 s | 1,600,000 | 0.134 s |
| Local variable per thread, merged at the end | 1,600,000 | 0.022 s | 1,600,000 | 0.014 s |

- The lock costs about 3x over the unsafe version on either build, and it is correct; the unsafe version being "correct" on the GIL build is luck (CW12, snippet 5).
- The fastest correct design has no shared state: count locally and merge once, which is also what a `ProcessPoolExecutor` map-reduce does.
- `ShardedCounter` is the middle ground when callers cannot be restructured (a metrics library); its `threading.local` lookup costs most of what the lock saves here.
- In a service, the counter that matters is shared across processes, so it lives in Redis or the database (CW3); in CPython `itertools.count()` is sometimes used as an atomic counter on the GIL build, but it is not documented as thread-safe, so do not rely on it.

---

## CW23. Implement a read-write lock as context managers.

Many readers may hold it together; a writer needs it alone.
The plain `RWLock` and its trade-offs are [CT27](14-Concurrency-Fundamentals-and-Threading.md); here is the context-manager variant interviewers ask you to write, with writer preference so writers do not starve.

```python
# rwlock.py
import threading
from collections.abc import Iterator
from contextlib import contextmanager


class RWLock:
    """Many readers or one writer. Writer-preferring: once a writer waits, new readers queue behind it.

    Not reentrant, and a reader must not try to upgrade to a writer (it would wait for itself).
    """

    def __init__(self) -> None:
        self._cond = threading.Condition(threading.Lock())
        self._readers = 0
        self._writer = False
        self._writers_waiting = 0

    @contextmanager
    def read(self) -> Iterator[None]:
        with self._cond:
            self._cond.wait_for(lambda: not self._writer and self._writers_waiting == 0)
            self._readers += 1
        try:
            yield
        finally:
            with self._cond:
                self._readers -= 1
                if self._readers == 0:
                    self._cond.notify_all()  # a waiting writer may go now

    @contextmanager
    def write(self) -> Iterator[None]:
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
                self._cond.notify_all()  # wake readers and writers; they re-check their predicates
```

- State: `_readers` (count), `_writer` (flag), `_writers_waiting` (count); invariant: `_writer` implies `_readers == 0`, and at most one writer.
- Writer preference: readers also wait while `_writers_waiting > 0`, so a stream of overlapping readers cannot starve a writer; the trade-off is that readers can starve under a stream of writers.
- The `try/finally` around the writer's wait keeps `_writers_waiting` correct if the wait is interrupted.
- Tests: 6 readers x 200 and 3 writers x 100, checking no reader overlaps a writer and no two writers overlap, with the protected value exactly 300; four 0.1 s readers finish in under 0.3 s (they overlap); a writer gets in within 1 s while 4 readers loop continuously.
- Worth saying: with the GIL and short critical sections, a plain `Lock` is often faster than any RW lock; use one only when reads are long and frequent.

---

## CW24. Fetch many URLs in parallel three ways and compare. (must know)

A local test server that sleeps before answering stands in for a slow API, and records the peak number of concurrent requests it saw.

```python
# sleep_server.py
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlsplit


class SleepServer(ThreadingHTTPServer):
    request_queue_size = 128  # listen backlog; the socketserver default of 5 drops bursts of connects
    daemon_threads = True

    def __init__(self) -> None:
        super().__init__(("127.0.0.1", 0), SleepHandler)
        self.lock = threading.Lock()
        self.in_flight = 0
        self.max_in_flight = 0

    @property
    def base_url(self) -> str:
        return f"http://127.0.0.1:{self.server_address[1]}"


class SleepHandler(BaseHTTPRequestHandler):
    server: SleepServer

    def do_GET(self) -> None:
        delay = float(parse_qs(urlsplit(self.path).query).get("delay", ["0.1"])[0])
        with self.server.lock:
            self.server.in_flight += 1
            self.server.max_in_flight = max(self.server.max_in_flight, self.server.in_flight)
        time.sleep(delay)  # stands in for a slow upstream
        with self.server.lock:
            self.server.in_flight -= 1
        body = self.path.encode()
        self.send_response(200)
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, *args) -> None:
        pass
```

```python
# fetchers.py
import asyncio
import urllib.request
from concurrent.futures import ThreadPoolExecutor

import httpx


def fetch_one(url: str, timeout: float = 5.0) -> bytes:
    with urllib.request.urlopen(url, timeout=timeout) as response:
        return response.read()


def fetch_sequential(urls: list[str]) -> list[bytes]:
    return [fetch_one(u) for u in urls]


def fetch_threads(urls: list[str], max_workers: int = 16) -> list[bytes]:
    with ThreadPoolExecutor(max_workers) as pool:
        return list(pool.map(fetch_one, urls))  # map keeps input order and re-raises the first error


async def fetch_async(urls: list[str], limit: int = 16) -> list[bytes]:
    semaphore = asyncio.Semaphore(limit)  # bound in-flight requests, not just tasks created
    limits = httpx.Limits(max_connections=limit)
    async with httpx.AsyncClient(timeout=5.0, limits=limits) as client:

        async def one(url: str) -> bytes:
            async with semaphore:
                response = await client.get(url)
                response.raise_for_status()
                return response.content

        async with asyncio.TaskGroup() as tg:  # one failure cancels the rest (ExceptionGroup)
            tasks = [tg.create_task(one(u)) for u in urls]
        return [t.result() for t in tasks]  # input order
```

**Measured**, 40 URLs each answered after 0.1 s, median of 3 runs:

| Approach | Time | Why |
| --- | --- | --- |
| Sequential (`urllib`) | 4.34 s | 40 x (0.1 s + connection setup) |
| `ThreadPoolExecutor`, 16 threads | 0.33 s | 3 rounds of 0.1 s |
| asyncio + `Semaphore(16)` + `httpx.AsyncClient` | 0.38 s | 3 rounds of 0.1 s |
| asyncio, limit 40 | 0.15 s | 1 round |
| threads, 40 | 0.12 s | 1 round |

- For I/O-bound fan-out, threads and asyncio perform the same at this scale; asyncio wins when concurrency reaches thousands (a task is far cheaper than a thread), threads win when the client library is synchronous.
- The semaphore bounds requests in flight: the test asserts the server never saw more than 5 at once with `limit=5`; creating 10,000 tasks without it is a self-inflicted DDoS.
- Order is preserved in all three (`pool.map` and the task list); `TaskGroup` cancels the rest on the first failure and raises an `ExceptionGroup`; use `gather(..., return_exceptions=True)` when partial results are wanted.
- Test detail worth mentioning: `socketserver`'s default listen backlog is 5, which silently drops bursts of connections and adds retransmit delays that look like client slowness; the test server raises `request_queue_size` to 128.
- The per-call timeout, retries, and global deadline are CW26; a smaller version is [K12](11-Coding-Round.md).

---

## CW25. Count primes in parallel: processes vs threads, on the GIL and free-threaded builds.

```python
# cpu_parallel.py
import os
import sys
import time
from concurrent.futures import Executor, ProcessPoolExecutor, ThreadPoolExecutor


def is_prime(n: int) -> bool:
    if n < 2:
        return False
    if n % 2 == 0:
        return n == 2
    f = 3
    while f * f <= n:
        if n % f == 0:
            return False
        f += 2
    return True


def count_range(bounds: tuple[int, int]) -> int:
    lo, hi = bounds
    return sum(1 for n in range(lo, hi) if is_prime(n))  # pure Python: holds the GIL throughout


def chunks(limit: int, parts: int) -> list[tuple[int, int]]:
    step = -(-limit // parts)
    return [(lo, min(lo + step, limit)) for lo in range(0, limit, step)]


def count_primes(limit: int, executor: Executor | None = None, parts: int = 32) -> int:
    if executor is None:
        return count_range((0, limit))
    return sum(executor.map(count_range, chunks(limit, parts)))  # more parts than workers: balances load


if __name__ == "__main__":
    limit, workers = 2_000_000, 8
    gil = getattr(sys, "_is_gil_enabled", lambda: True)()
    print(f"{sys.version.split()[0]} gil={gil} cpus={os.cpu_count()} limit={limit} workers={workers}")
    start = time.perf_counter()
    expected = count_primes(limit)
    print(f"serial     {time.perf_counter() - start:6.2f}s primes={expected}")
    for name, make in (("threads", ThreadPoolExecutor), ("processes", ProcessPoolExecutor)):
        with make(workers) as ex:
            list(ex.map(time.sleep, [0.05] * workers))  # warm up: start every worker before timing
            start = time.perf_counter()
            assert count_primes(limit, ex) == expected
            print(f"{name:10} {time.perf_counter() - start:6.2f}s")
```

**Measured** with `python cpu_parallel.py` (primes below 2,000,000, 8 workers, 32 chunks, pools warmed up before timing), two runs each:

| | 3.14.7 (GIL) | 3.14.6 free-threaded |
| --- | --- | --- |
| Serial | 2.80 s, 3.09 s | 3.13 s, 2.97 s |
| 8 threads | 2.82 s, 2.93 s (no speedup) | 0.82 s, 0.79 s (about 3.8x) |
| 8 processes | 0.64 s, 0.69 s (about 4.4x) | 0.73 s, 0.80 s |

- On the GIL build, threads cannot run pure-Python bytecode in parallel, so CPU-bound work needs processes; on the free-threaded build, threads match processes without pickling or process startup.
- Speedup is below 8x because the M3 Pro mixes performance and efficiency cores and because the machine was under load; chunks of unequal cost (larger numbers take longer to test) are why there are 32 chunks for 8 workers.
- The warm-up matters: `ProcessPoolExecutor` starts workers on demand under the `spawn` and `forkserver` start methods, so an unwarmed first measurement includes process startup (details in [Multiprocessing and parallelism](15-Multiprocessing-and-Parallelism.md)).
- The test checks all three give 9,592 primes below 100,000.

---

## CW26. Write a rate-limited async worker pool with retries and a global timeout. (must know)

Requirements as usually stated: process a list of items with at most W concurrent calls, at most R calls per second overall, retry transient failures with backoff, and stop everything after T seconds, reporting what finished.

```python
# async_pool.py
import asyncio
import random
from collections.abc import Awaitable, Callable, Hashable, Sequence
from typing import Any


class TransientError(Exception):
    """Retryable failure (timeout, 429, 503); anything else is permanent."""


class RateLimiter:
    """Spaces call starts at least 1/rate apart across all workers."""

    def __init__(self, rate_per_s: float) -> None:
        self._interval = 1.0 / rate_per_s
        self._next = 0.0
        self._lock = asyncio.Lock()

    async def wait(self) -> None:
        async with self._lock:  # reserve a start time under the lock...
            now = asyncio.get_running_loop().time()
            start_at = max(now, self._next)
            self._next = start_at + self._interval
        await asyncio.sleep(start_at - now)  # ...and sleep outside it, so reservations stay cheap


async def run_pool(items: Sequence[Hashable], handler: Callable[[Any], Awaitable[Any]], *,
                   workers: int = 8, rate_per_s: float = 50.0, max_attempts: int = 3,
                   base_delay_s: float = 0.05, total_timeout_s: float = 10.0) -> dict[Hashable, Any]:
    """Process items with bounded concurrency, a global rate, retries, and one deadline for everything.

    Returns {item: result or exception}; items not finished by the deadline map to TimeoutError.
    """
    queue: asyncio.Queue = asyncio.Queue()
    for item in items:
        queue.put_nowait(item)
    limiter = RateLimiter(rate_per_s)
    results: dict[Hashable, Any] = {}

    async def attempt_with_retries(item: Hashable) -> Any:
        for attempt in range(1, max_attempts + 1):
            await limiter.wait()  # retries count against the rate too
            try:
                return await handler(item)
            except TransientError:
                if attempt == max_attempts:
                    raise
                backoff = base_delay_s * 2 ** (attempt - 1)
                await asyncio.sleep(backoff * random.uniform(0.5, 1.5))  # jitter spreads retries

    async def worker() -> None:
        while True:
            try:
                item = queue.get_nowait()  # all items are queued up front, so empty means done
            except asyncio.QueueEmpty:
                return
            try:
                results[item] = await attempt_with_retries(item)
            except Exception as exc:  # record and move on; CancelledError is not an Exception
                results[item] = exc

    try:
        async with asyncio.timeout(total_timeout_s):
            async with asyncio.TaskGroup() as tg:
                for _ in range(workers):
                    tg.create_task(worker())
    except TimeoutError:
        pass  # the TaskGroup already cancelled and awaited every worker
    for item in items:
        results.setdefault(item, TimeoutError(f"not finished within {total_timeout_s}s"))
    return results
```

- Shared state: the queue (one consumer per `get_nowait`, so no item is processed twice), the limiter's next start time (guarded by an `asyncio.Lock`), and `results` (safe without a lock: one event loop, no `await` between check and write).
- The limiter reserves a start slot under the lock and sleeps outside it, so waiting callers do not serialize on the lock.
- `asyncio.timeout` around the `TaskGroup` cancels all workers at the deadline; `CancelledError` is a `BaseException`, so the worker's `except Exception` does not swallow it, and the unfinished items are reported as `TimeoutError`.
- Tests: 40 items, 4 workers, 200 per second: peak concurrency 4 and call starts at least 5 ms apart; a flaky item succeeds on attempt 3, a permanently 503 item fails after 3 attempts, a `ValueError` item is not retried; with a 0.3 s deadline, the hanging item is cancelled (its `CancelledError` handler ran) and reported as `TimeoutError` while the other three completed, all in under 1 s.
- Production extensions to name: per-call timeouts (`asyncio.timeout` around `handler`), honouring `Retry-After`, a retry budget so retries cannot multiply load during an outage, and a circuit breaker ([M10](08-Microservices-and-Messaging.md)).

---

## CW27. Build a scheduler that runs a callback every N ms without drift, and stops cleanly.

```python
# periodic.py
import logging
import threading
import time
from collections.abc import Callable

log = logging.getLogger(__name__)


class Periodic:
    """Run callback every interval_s on a background thread, on a fixed grid (no drift), with a clean stop.

    Deadlines are start + k * interval, not "sleep(interval) after each run", so callback time and
    wake-up latency do not accumulate. If a run overruns, the missed ticks are skipped, not burst.
    """

    def __init__(self, interval_s: float, callback: Callable[[], None],
                 clock: Callable[[], float] = time.monotonic) -> None:
        self._interval = interval_s
        self._callback = callback
        self._clock = clock
        self._stop = threading.Event()
        self._thread = threading.Thread(target=self._run, name="periodic", daemon=True)
        self.skipped = 0

    def start(self) -> "Periodic":
        self._thread.start()
        return self

    def _run(self) -> None:
        start = self._clock()
        tick = 1
        while True:
            delay = start + tick * self._interval - self._clock()
            if delay < 0:  # overran one or more ticks: jump to the next future tick on the grid
                missed = int(-delay // self._interval) + 1
                self.skipped += missed
                tick += missed
                delay += missed * self._interval
            if self._stop.wait(delay):  # returns True as soon as stop() is called
                return
            try:
                self._callback()
            except Exception:
                log.exception("periodic callback failed")  # keep the schedule alive
            tick += 1

    def stop(self, timeout: float | None = 5.0) -> None:
        self._stop.set()
        if threading.current_thread() is not self._thread:  # stop() may be called from the callback
            self._thread.join(timeout)
```

- Drift correction: deadlines are `start + k x interval`, so callback time and late wake-ups do not accumulate; `time.monotonic` is immune to wall-clock changes.
- Overruns: when a callback takes longer than an interval, the missed ticks are skipped (counted in `skipped`) rather than run back to back; say which policy you chose and why (for a heartbeat, skipping is right; for "process each minute's bucket", catch up instead).
- Clean stop: `Event.wait(delay)` returns immediately when `stop()` sets the event, so stopping a 10-second timer takes milliseconds; `stop()` from inside the callback does not try to join its own thread.
- An exception in the callback is logged and the schedule continues; a thread that dies silently is the classic production failure.
- **Measured**, 50 ticks at 20 ms with 5 ms of work per tick: a naive `sleep(interval)` loop finished 449 to 575 ms late; `Periodic` finished 1 to 3 ms late, or one interval late in a run where it skipped a tick because the machine was overloaded.
- Tests (both builds): 12 ticks at 40 ms with 10 ms of work land on the grid (median lateness past a grid point under 8 ms, where the naive loop spreads across the whole interval); a 55 ms overrun skips at least two ticks with no catch-up burst and later ticks stay on the grid; `stop()` on a 10 s timer returns in under 0.1 s.
- The asyncio version is the same loop with `loop.time()` and `asyncio.sleep` (or `asyncio.timeout` for the stop), run as a task started in `lifespan` and cancelled at shutdown.

---

## Go deeper

- Sibling notes: [Concurrency fundamentals and threading](14-Concurrency-Fundamentals-and-Threading.md), [Multiprocessing and parallelism](15-Multiprocessing-and-Parallelism.md), [Asyncio deep dive](16-Asyncio-Deep-Dive.md), [Python core Y9 and Y10](02-Python-Core.md), [FastAPI A2, A18, A19](04-FastAPI.md), [Flask F9, F10, F13](03-Flask.md), [SQL and SQLAlchemy S9-S11, S18, S20](06-SQL-and-SQLAlchemy.md), [Microservices and Messaging M8, M10, M17](08-Microservices-and-Messaging.md), [Testing, debugging, production T9, T10](10-Testing-Debugging-Production.md), [Coding round K1, K11, K12](11-Coding-Round.md), [System design X1, X2](12-System-Design.md).
- Vault chapters: [The GIL](../Python_Zero_to_Godhood/Chapter_10_CONCURRENCY_MECHANICS__THE_GLOBAL_INTERPRETER_LOCK.md), [CPU and I/O bound concurrency (26)](../Python_Zero_to_Godhood/Chapter_26_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md), [CPU and I/O bound concurrency (27)](../Python_Zero_to_Godhood/Chapter_27_CPU__IO_BOUND_SYSTEM_CONCURRENCY.md), [Shared memory and proxies](../Python_Zero_to_Godhood/Chapter_55_Advanced_Concurrency_Shared_Memory_and_Proxies.md), [Asyncio inception](../Python_Zero_to_Godhood/Volume_03_Generators_Iterators_and_Async_Inception/Chapter_10_Asyncio_Inception_Pathlib_and_Enum/Chapter_10_Asyncio_Inception_Pathlib_and_Enum.md).
- Operating systems: [Threads and concurrency](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L2-Threads-and-Concurrency.md), [Thread design considerations](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L4-Thread-Design-Considerations.md), [Synchronization constructs](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-3-Resource-Management/P3L4-Synchronization-Constructs.md), [Inter-process communication](../../../01-CS-Foundations/Operating-Systems/GIOS/Part-3-Resource-Management/P3L3-Inter-Process-Communication.md).
- Official docs: [gunicorn design](https://docs.gunicorn.org/en/stable/design.html), [gunicorn settings](https://docs.gunicorn.org/en/stable/settings.html), [uvicorn settings](https://www.uvicorn.org/settings/), [FastAPI concurrency and async](https://fastapi.tiangolo.com/async/), [AnyIO worker threads](https://anyio.readthedocs.io/en/stable/threads.html), [Flask async](https://flask.palletsprojects.com/en/stable/async-await/), [SQLAlchemy contextual sessions](https://docs.sqlalchemy.org/en/20/orm/contextual.html), [SQLAlchemy pooling](https://docs.sqlalchemy.org/en/20/core/pooling.html), [PostgreSQL explicit locking](https://www.postgresql.org/docs/current/explicit-locking.html), [Redis distributed locks](https://redis.io/docs/latest/develop/clients/patterns/distributed-locks/), [How to do distributed locking (Kleppmann)](https://martin.kleppmann.com/2016/02/08/how-to-do-distributed-locking.html), [Celery workers](https://docs.celeryq.dev/en/stable/userguide/workers.html), [threading](https://docs.python.org/3/library/threading.html), [asyncio synchronization primitives](https://docs.python.org/3/library/asyncio-sync.html), [concurrent.futures](https://docs.python.org/3/library/concurrent.futures.html), [LeetCode concurrency problems](https://leetcode.com/problemset/concurrency/).
